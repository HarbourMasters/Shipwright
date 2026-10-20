#include "StreamedMusic.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>

#include <libultraship/libultra.h>
#include <ship/Context.h>
#include <ship/resource/ResourceManager.h>
#include "soh/resource/type/AudioSequence.h"
#include "AudioEditor.h"
#include "functions.h"
#include "sequence.h"
#include "variables.h"

// mixer.h has no extern "C" guard, so the piece of its API used here is declared instead.
extern "C" bool SOH_OpusStream_ArmContinue(const void* source);
extern "C" void SOH_OpusStream_Update();

namespace {

constexpr size_t kNumSeqPlayers = 4;

// OTRAudio_Thread renders 60 audio frames per second of output, whatever the game's frame rate.
constexpr float kAudioFramesPerSecond = 60.0f;
constexpr float kFadeTimerUnitsPerAudioFrame = 4.0f;
constexpr float kMaxFadeSeconds = 60.0f;
using Clock = std::chrono::steady_clock;
constexpr auto kFadeOutWaitSlack = std::chrono::milliseconds(500);
// Long enough for a fade-less start issued right after a stop to turn the pair into a crossfade.
constexpr uint8_t kStopGraceFrames = 3;
// Audio_PlaySceneSequence's resume fade, as the seq command scales it.
constexpr uint16_t kResumeFadeTimer = 0x1E << 3;

struct Fades {
    float inSeconds;
    float outSeconds;
};

struct PendingStart {
    uint8_t seqId;
    uint8_t seqArgs;
    bool valid;
};

std::unordered_map<uint16_t, Fades> sStreamedSequences;
std::array<std::optional<uint16_t>, kNumSeqPlayers> sPlayerStreamedSeq{};
std::array<std::optional<Clock::time_point>, kNumSeqPlayers> sFadeOutDeadline{};
std::array<PendingStart, kNumSeqPlayers> sPendingStart{};
std::array<uint8_t, kNumSeqPlayers> sStopGrace{};
std::array<bool, kNumSeqPlayers> sStopSaved{};
std::array<uint8_t, kNumSeqPlayers> sStopSeqId{};
std::array<uint8_t, kNumSeqPlayers> sStopSeqArgs{};
bool sStartingShadow = false;
std::array<bool, kNumSeqPlayers> sResumeRequested{};
std::unordered_map<uint16_t, const void*> sStreamedSource;

uint16_t SecondsToFadeTimer(float seconds) {
    return static_cast<uint16_t>((seconds * kAudioFramesPerSecond * kFadeTimerUnitsPerAudioFrame) + 0.5f);
}

const Fades* FindFades(uint16_t seqId) {
    const auto it = sStreamedSequences.find(seqId);
    return it == sStreamedSequences.end() ? nullptr : &it->second;
}

float FadeOutSecondsOn(uint8_t playerIdx) {
    const std::optional<uint16_t> seqId = sPlayerStreamedSeq.at(playerIdx);
    const Fades* fades = seqId.has_value() ? FindFades(seqId.value()) : nullptr;
    return fades == nullptr ? 0.0f : fades->outSeconds;
}

uint16_t FadeOutTimerOn(uint8_t playerIdx) {
    return SecondsToFadeTimer(FadeOutSecondsOn(playerIdx));
}

// Must not consume `seqReplaced`: the start itself still reads it.
uint16_t ResolveSeqId(uint8_t playerIdx, uint8_t seqId) {
    return gAudioContext.seqReplaced[playerIdx] ? gAudioContext.seqToPlay[playerIdx]
                                                : AudioEditor_GetReplacementSeq(seqId);
}

void QueueFadeOut(uint8_t playerIdx, uint16_t fadeOutTimer) {
    Audio_QueueCmdS32(0x83000000 | (static_cast<uint32_t>(playerIdx) << 16),
                      (fadeOutTimer * gAudioContext.audioBufferParameters.updatesPerFrame) / 4);
}

bool IsFadingOut(uint8_t playerIdx) {
    return sFadeOutDeadline.at(playerIdx).has_value();
}

void HoldForFadeOut(uint8_t playerIdx) {
    const std::chrono::duration<float> fade(FadeOutSecondsOn(playerIdx));
    sFadeOutDeadline.at(playerIdx) =
        Clock::now() + std::chrono::duration_cast<Clock::duration>(fade) + kFadeOutWaitSlack;
    sPendingStart.at(playerIdx).valid = false;
}

void ClearStopRecord(uint8_t playerIdx) {
    sStopGrace.at(playerIdx) = 0;
    sStopSaved.at(playerIdx) = false;
}

int8_t FindShadowPlayer(uint8_t playerIdx) {
    static constexpr uint8_t kOrder[] = { SEQ_PLAYER_BGM_SUB, SEQ_PLAYER_FANFARE };

    for (const uint8_t candidate : kOrder) {
        if (candidate != playerIdx && !gAudioContext.seqPlayers[candidate].enabled && !IsFadingOut(candidate) &&
            !sPendingStart.at(candidate).valid && sStopGrace.at(candidate) == 0) {
            return static_cast<int8_t>(candidate);
        }
    }
    return -1;
}

const void* FindStreamedNoteSource(uint8_t playerIdx) {
    const SequencePlayer* seqPlayer = &gAudioContext.seqPlayers[playerIdx];

    for (int32_t i = 0; i < gAudioContext.numNotes; i++) {
        const Note* note = &gAudioContext.notes[i];
        const SequenceLayer* layer = note->playbackState.parentLayer;
        if (layer != NO_LAYER && layer->channel->seqPlayer == seqPlayer && note->synthesisState.opusFile != nullptr &&
            layer->sound != nullptr && layer->sound->sample != nullptr) {
            return layer->sound->sample->sampleAddr;
        }
    }
    return nullptr;
}

void PrepareStartingTrack(uint8_t playerIdx, uint8_t seqId, uint16_t* fadeTimer) {
    const uint16_t resolvedSeqId = ResolveSeqId(playerIdx, seqId);
    const bool resume = std::exchange(sResumeRequested.at(playerIdx), false);
    const auto source = sStreamedSource.find(resolvedSeqId);
    if (resume && source != sStreamedSource.end() && SOH_OpusStream_ArmContinue(source->second)) {
        if (*fadeTimer == 0) {
            *fadeTimer = kResumeFadeTimer;
        }
        return;
    }

    const Fades* fades = FindFades(resolvedSeqId);
    if (*fadeTimer == 0 && fades != nullptr) {
        *fadeTimer = SecondsToFadeTimer(fades->inSeconds);
    }
}

// False when the caller has to fall back to fading out before starting.
bool TryCrossfade(uint8_t playerIdx, uint8_t seqId) {
    if (!sPlayerStreamedSeq.at(playerIdx).has_value() || IsFadingOut(playerIdx)) {
        return false;
    }

    uint8_t oldSeqId;
    uint8_t oldSeqArgs;
    if (sStopSaved.at(playerIdx)) {
        if (sStopSeqId.at(playerIdx) == seqId) {
            ClearStopRecord(playerIdx);
            return false;
        }
        oldSeqId = sStopSeqId.at(playerIdx);
        oldSeqArgs = sStopSeqArgs.at(playerIdx);
    } else {
        oldSeqId = gActiveSeqs[playerIdx].seqId & 0xFF;
        oldSeqArgs = (gActiveSeqs[playerIdx].seqId >> 8) & 0x7F;
        if (oldSeqId == static_cast<uint8_t>(NA_BGM_DISABLED) || oldSeqId == seqId) {
            return false;
        }
    }

    const uint16_t fadeOutTimer = FadeOutTimerOn(playerIdx);
    if (fadeOutTimer == 0) {
        ClearStopRecord(playerIdx);
        return true;
    }

    const int8_t shadow = FindShadowPlayer(playerIdx);
    if (shadow < 0) {
        return false;
    }

    if (!SOH_OpusStream_ArmContinue(FindStreamedNoteSource(playerIdx))) {
        return false;
    }

    sFadeOutDeadline.at(shadow).reset();
    sPendingStart.at(shadow).valid = false;
    ClearStopRecord(shadow);
    sPlayerStreamedSeq.at(shadow).reset();
    sStartingShadow = true;
    Audio_StartSequence(static_cast<uint8_t>(shadow), oldSeqId, oldSeqArgs, 0);
    sStartingShadow = false;
    QueueFadeOut(static_cast<uint8_t>(shadow), fadeOutTimer);

    ClearStopRecord(playerIdx);
    return true;
}

bool CanStartNow(uint8_t playerIdx, uint8_t seqId) {
    if (!sPlayerStreamedSeq.at(playerIdx).has_value() || !gAudioContext.seqPlayers[playerIdx].enabled ||
        (gActiveSeqs[playerIdx].seqId & 0xFF) == seqId ||
        (sStopSaved.at(playerIdx) && sStopSeqId.at(playerIdx) == seqId)) {
        return true;
    }
    return FadeOutTimerOn(playerIdx) == 0;
}

} // namespace

extern "C" void SOH_StreamedMusic_RegisterSequence(uint16_t seqId, uint8_t isStreamed, const char* resourcePath) {
    if (isStreamed == 0) {
        sStreamedSequences.erase(seqId);
        return;
    }

    const auto sequence = std::static_pointer_cast<SOH::AudioSequence>(
        Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(resourcePath));
    Fades fades{ 0.0f, 0.0f };
    if (sequence != nullptr) {
        fades.inSeconds = std::clamp(sequence->fadeInSeconds, 0.0f, kMaxFadeSeconds);
        fades.outSeconds = std::clamp(sequence->fadeOutSeconds, 0.0f, kMaxFadeSeconds);
    }
    sStreamedSequences.insert_or_assign(seqId, fades);
}

extern "C" void SOH_StreamedMusic_SequenceStarted(uint8_t playerIdx, uint16_t seqId) {
    if (sStreamedSequences.contains(seqId)) {
        sPlayerStreamedSeq.at(playerIdx) = seqId;
    } else {
        sPlayerStreamedSeq.at(playerIdx).reset();
    }
}

extern "C" uint8_t SOH_StreamedMusic_SequenceStopped(uint8_t playerIdx, uint16_t fadeTimer) {
    if (fadeTimer != 0 || FadeOutTimerOn(playerIdx) == 0) {
        return 0;
    }

    if (gActiveSeqs[playerIdx].seqId != NA_BGM_DISABLED) {
        sStopSeqId.at(playerIdx) = gActiveSeqs[playerIdx].seqId & 0xFF;
        sStopSeqArgs.at(playerIdx) = (gActiveSeqs[playerIdx].seqId >> 8) & 0x7F;
        sStopSaved.at(playerIdx) = true;
    }
    sStopGrace.at(playerIdx) = kStopGraceFrames;
    return 1;
}

extern "C" uint8_t SOH_StreamedMusic_PrepareStart(uint8_t playerIdx, uint8_t seqId, uint8_t seqArgs,
                                                  uint16_t* fadeTimer) {
    if (sStartingShadow) {
        return 0;
    }
    if (*fadeTimer != 0) {
        ClearStopRecord(playerIdx);
        PrepareStartingTrack(playerIdx, seqId, fadeTimer);
        return 0;
    }

    if (TryCrossfade(playerIdx, seqId)) {
        PrepareStartingTrack(playerIdx, seqId, fadeTimer);
        return 0;
    }

    if (!IsFadingOut(playerIdx)) {
        if (CanStartNow(playerIdx, seqId)) {
            ClearStopRecord(playerIdx);
            PrepareStartingTrack(playerIdx, seqId, fadeTimer);
            return 0;
        }

        QueueFadeOut(playerIdx, FadeOutTimerOn(playerIdx));
        HoldForFadeOut(playerIdx);
        ClearStopRecord(playerIdx);
    }

    sPendingStart.at(playerIdx) = { seqId, seqArgs, true };
    // Callers polling func_800FA0B4() would otherwise re-request the start every frame.
    gActiveSeqs[playerIdx].seqId = seqId | (seqArgs << 8);
    return 1;
}

extern "C" void SOH_StreamedMusic_ResumeNextStart(uint8_t playerIdx) {
    sResumeRequested.at(playerIdx) = true;
}

extern "C" void SOH_StreamedMusic_Update() {
    SOH_OpusStream_Update();
    for (uint8_t playerIdx = 0; playerIdx < kNumSeqPlayers; playerIdx++) {
        const std::optional<uint16_t> streamedSeq = sPlayerStreamedSeq.at(playerIdx);
        if (streamedSeq.has_value()) {
            const void* source = FindStreamedNoteSource(playerIdx);
            if (source != nullptr) {
                sStreamedSource.insert_or_assign(streamedSeq.value(), source);
            }
        }

        uint8_t& grace = sStopGrace.at(playerIdx);
        if (grace != 0 && --grace == 0) {
            QueueFadeOut(playerIdx, FadeOutTimerOn(playerIdx));
        }

        if (sStopSaved.at(playerIdx) && !gAudioContext.seqPlayers[playerIdx].enabled) {
            sStopSaved.at(playerIdx) = false;
        }

        std::optional<Clock::time_point>& deadline = sFadeOutDeadline.at(playerIdx);
        if (!deadline.has_value()) {
            continue;
        }
        if (gAudioContext.seqPlayers[playerIdx].enabled && (Clock::now() < deadline.value())) {
            continue;
        }
        deadline.reset();

        PendingStart& pending = sPendingStart.at(playerIdx);
        if (!pending.valid) {
            continue;
        }

        pending.valid = false;
        ClearStopRecord(playerIdx);
        sPlayerStreamedSeq.at(playerIdx).reset();
        Audio_StartSequence(playerIdx, pending.seqId, pending.seqArgs, 0);
    }
}

extern "C" void SOH_StreamedMusic_Reset() {
    sFadeOutDeadline.fill(std::nullopt);
    sPendingStart.fill(PendingStart{});
    sResumeRequested.fill(false);
    for (uint8_t playerIdx = 0; playerIdx < kNumSeqPlayers; playerIdx++) {
        ClearStopRecord(playerIdx);
    }
}

extern "C" uint8_t SOH_StreamedMusic_ShouldSplitChannels(uint8_t playerIdx) {
    // An empty player is about to receive the enemy sequence, queued a step earlier.
    const bool shouldSplit =
        gAudioContext.seqPlayers[playerIdx].enabled && !sPlayerStreamedSeq.at(playerIdx).has_value();
    return static_cast<uint8_t>(shouldSplit);
}
