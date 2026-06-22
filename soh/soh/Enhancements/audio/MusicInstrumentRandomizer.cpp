#include <stdint.h>
#include <string.h>

#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "global.h"
#include "functions.h"
#include "variables.h"
u32 Audio_NextRandom(void);
}

static constexpr int32_t CVAR_RANDOM_MUSIC_INSTRUMENTS_DEFAULT = 0;
#define CVAR_RANDOM_MUSIC_INSTRUMENTS_NAME CVAR_ENHANCEMENT("RandomMusicInstruments")
#define CVAR_RANDOM_MUSIC_INSTRUMENTS_VALUE \
    CVarGetInteger(CVAR_RANDOM_MUSIC_INSTRUMENTS_NAME, CVAR_RANDOM_MUSIC_INSTRUMENTS_DEFAULT)

namespace RandomMusicInstruments {

static constexpr uint8_t MIN_NORMAL_INSTRUMENT = 0x00;
static constexpr uint8_t MAX_NORMAL_INSTRUMENT = 0x7D;
static constexpr uint8_t NUM_NORMAL_INSTRUMENTS = 0x7E;

static int32_t sCurrentMainSeqId = NA_BGM_DISABLED;
static uint32_t sMainSeqPlayCount = 0;

static bool IsMainBgmChannel(SequenceChannel* channel) {
    if (channel == nullptr) {
        return false;
    }

    SequencePlayer* seqPlayer = channel->seqPlayer;
    if (seqPlayer == nullptr) {
        return false;
    }

    return seqPlayer->playerIdx == SEQ_PLAYER_BGM_MAIN && seqPlayer->enabled;
}

static bool IsNormalInstrument(uint8_t instId) {
    return instId <= MAX_NORMAL_INSTRUMENT;
}

static bool IsValidInstrument(uint8_t fontId, uint8_t instId) {
    if (!IsNormalInstrument(instId)) {
        return false;
    }

    return Audio_GetInstrumentInner(fontId, instId) != nullptr;
}

static uint32_t RandomIndex(uint32_t count) {
    if (count <= 1) {
        return 0;
    }

    return static_cast<uint32_t>(Audio_NextRandom()) % count;
}

static uint8_t GetRandomInstrumentFromSameFont(uint8_t fontId, uint8_t originalInstId) {
    uint8_t candidates[NUM_NORMAL_INSTRUMENTS];
    uint8_t candidateCount = 0;

    for (uint8_t instId = MIN_NORMAL_INSTRUMENT; instId <= MAX_NORMAL_INSTRUMENT; instId++) {
        if (!IsValidInstrument(fontId, instId)) {
            continue;
        }

        candidates[candidateCount++] = instId;
    }

    if (candidateCount == 0) {
        return originalInstId;
    }

    if (candidateCount == 1) {
        return candidates[0];
    }

    // Try a few times to avoid returning the original instrument.
    // If the font is weird and we keep hitting the same one, whatever.
    for (int32_t attempt = 0; attempt < 8; attempt++) {
        const uint8_t candidate = candidates[RandomIndex(candidateCount)];

        if (candidate != originalInstId) {
            return candidate;
        }
    }

    // Guaranteed fallback to something different if possible.
    for (uint8_t i = 0; i < candidateCount; i++) {
        if (candidates[i] != originalInstId) {
            return candidates[i];
        }
    }

    return originalInstId;
}

static void OnSeqPlayerInit(int32_t playerIdx, int32_t seqId) {
    if (playerIdx != SEQ_PLAYER_BGM_MAIN) {
        return;
    }

    sCurrentMainSeqId = seqId;
    sMainSeqPlayCount++;
}

static void OnSeqInstrumentSet(void* channelPtr, uint8_t* instId) {
    if (instId == nullptr) {
        return;
    }

    if (!CVAR_RANDOM_MUSIC_INSTRUMENTS_VALUE) {
        return;
    }

    // Do not touch drums, sfx mode, synthetic/reverb specials, etc.
    if (!IsNormalInstrument(*instId)) {
        return;
    }

    SequenceChannel* channel = static_cast<SequenceChannel*>(channelPtr);
    if (!IsMainBgmChannel(channel)) {
        return;
    }

    SequencePlayer* seqPlayer = channel->seqPlayer;
    if (seqPlayer == nullptr || seqPlayer->seqId == NA_BGM_DISABLED) {
        return;
    }

    // This is mostly documentation/state right now, but it keeps this tied to
    // a real BGM sequence play instead of acting globally.
    if (sCurrentMainSeqId == NA_BGM_DISABLED || sMainSeqPlayCount == 0) {
        return;
    }

    const uint8_t fontId = channel->fontId;
    const uint8_t originalInstId = *instId;

    *instId = GetRandomInstrumentFromSameFont(fontId, originalInstId);
}

static void RegisterHooks() {
    COND_HOOK(OnSeqPlayerInit, CVAR_RANDOM_MUSIC_INSTRUMENTS_VALUE, OnSeqPlayerInit);
    COND_HOOK(OnSeqInstrumentSet, CVAR_RANDOM_MUSIC_INSTRUMENTS_VALUE, OnSeqInstrumentSet);
}

} // namespace RandomMusicInstruments

static RegisterShipInitFunc initFunc(RandomMusicInstruments::RegisterHooks, { CVAR_RANDOM_MUSIC_INSTRUMENTS_NAME });
