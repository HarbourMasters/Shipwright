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
static constexpr uint8_t INVALID_INSTRUMENT = 0xFF;
static constexpr uint16_t NUM_FONTS = 0x100;

// Per-sequence remap table. Populated lazily. INVALID_INSTRUMENT means not yet assigned
static uint8_t sInstrumentRemap[NUM_FONTS][NUM_NORMAL_INSTRUMENTS];

// Prevents the same replacement from being assigned twice within a sequence (shuffle)
static bool sReplacementUsed[NUM_FONTS][NUM_NORMAL_INSTRUMENTS];

// When instrument A is swapped for B, the pitch range boundaries used to select between
// B's low/normal/high samples should still come from A, otherwise B's native split points
// may misroute notes into the wrong sample bucket.
struct RangeOverride {
    Instrument* replacement = nullptr;
    Instrument* original = nullptr;
};

static constexpr size_t MAX_RANGE_OVERRIDES = 256;
static RangeOverride sRangeOverrides[MAX_RANGE_OVERRIDES];
static size_t sRangeOverrideCount = 0;

// Whether a channel belongs to the active main BGM sequence player as opposed to fanfare, etc
static bool IsMainBgmChannel(SequenceChannel* channel) {
    if (channel == nullptr || channel->seqPlayer == nullptr) {
        return false;
    }

    return channel->seqPlayer->playerIdx == SEQ_PLAYER_BGM_MAIN && channel->seqPlayer->enabled;
}

// Is this instrument ID is a normal melodic instrument
static bool IsNormalInstrument(uint8_t instId) {
    return instId >= MIN_NORMAL_INSTRUMENT && instId <= MAX_NORMAL_INSTRUMENT;
}


// Does this instrument exists in the given sound font
static bool IsValidInstrument(uint8_t fontId, uint8_t instId) {
    if (!IsNormalInstrument(instId)) {
        return false;
    }

    return Audio_GetInstrumentInner(fontId, instId) != nullptr;
}

// Random instrument from Audio from 0..count
static uint32_t RandomIndex(uint32_t count) {
    if (count <= 1) {
        return 0;
    }

    return Audio_NextRandom() % count;
}

// Clears all per-sequence remap and range override state
static void ResetState() {
    memset(sInstrumentRemap, INVALID_INSTRUMENT, sizeof(sInstrumentRemap));
    memset(sReplacementUsed, 0, sizeof(sReplacementUsed));

    memset(sRangeOverrides, 0, sizeof(sRangeOverrides));
    sRangeOverrideCount = 0;
}

// Records that a replacement instrument should use another instrument's range boundaries
static void AddRangeOverride(Instrument* original, Instrument* replacement) {
    if (original == nullptr || replacement == nullptr || original == replacement) {
        return;
    }

    for (size_t i = 0; i < sRangeOverrideCount; i++) {
        if (sRangeOverrides[i].replacement == replacement) {
            sRangeOverrides[i].original = original;
            return;
        }
    }

    if (sRangeOverrideCount >= MAX_RANGE_OVERRIDES) {
        return;
    }

    const RangeOverride override = { .replacement = replacement, .original = original };
    sRangeOverrides[sRangeOverrideCount++] = override;
}

// Finds the original instrument whose range boundaries should be used for a replacement
static Instrument* FindOriginalRangeInstrument(Instrument* replacement) {
    for (size_t i = 0; i < sRangeOverrideCount; i++) {
        if (sRangeOverrides[i].replacement == replacement) {
            return sRangeOverrides[i].original;
        }
    }

    return nullptr;
}

// Stores a newly chosen remap and records its range override
static uint8_t CommitRemap(uint8_t fontId, uint8_t originalInstId, uint8_t replacementInstId) {
    sInstrumentRemap[fontId][originalInstId] = replacementInstId;
    sReplacementUsed[fontId][replacementInstId] = true;

    Instrument* original = Audio_GetInstrumentInner(fontId, originalInstId);
    Instrument* replacement = Audio_GetInstrumentInner(fontId, replacementInstId);

    AddRangeOverride(original, replacement);

    return replacementInstId;
}

// Returns the existing remap for an instrument or assigns a new shuffled replacement
static uint8_t GetOrCreateRemappedInstrument(uint8_t fontId, uint8_t originalInstId) {
    if (!IsNormalInstrument(originalInstId)) {
        return originalInstId;
    }

    uint8_t& remapped = sInstrumentRemap[fontId][originalInstId];

    if (remapped != INVALID_INSTRUMENT) {
        return remapped;
    }

    uint8_t candidates[NUM_NORMAL_INSTRUMENTS];
    uint8_t candidateCount = 0;

    for (uint8_t instId = MIN_NORMAL_INSTRUMENT; instId <= MAX_NORMAL_INSTRUMENT; instId++) {
        if (!IsValidInstrument(fontId, instId) || sReplacementUsed[fontId][instId]) {
            continue;
        }

        candidates[candidateCount++] = instId;
    }

    // If every valid replacement is already used allow repeats as a fallback
    if (candidateCount == 0) {
        for (uint8_t instId = MIN_NORMAL_INSTRUMENT; instId <= MAX_NORMAL_INSTRUMENT; instId++) {
            if (!IsValidInstrument(fontId, instId)) {
                continue;
            }

            candidates[candidateCount++] = instId;
        }
    }

    if (candidateCount == 0) {
        remapped = originalInstId;
        return remapped;
    }

    // Prefer not mapping an instrument to itself
    for (int32_t attempt = 0; attempt < 8; attempt++) {
        const uint8_t candidate = candidates[RandomIndex(candidateCount)];

        if (candidate != originalInstId || candidateCount == 1) {
            return CommitRemap(fontId, originalInstId, candidate);
        }
    }

    // Guaranteed non-self fallback if possible
    for (uint8_t i = 0; i < candidateCount; i++) {
        if (candidates[i] != originalInstId) {
            return CommitRemap(fontId, originalInstId, candidates[i]);
        }
    }

    return CommitRemap(fontId, originalInstId, candidates[0]);
}

// Uses the original instrument's pitch boundaries to decide which region of the replacement to play
static SoundFontSound* GetReplacementSoundUsingOriginalRange(Instrument* original, Instrument* replacement, int32_t semitone) {
    if (original == nullptr || replacement == nullptr) {
        return nullptr;
    }

    if (semitone < original->normalRangeLo) {
        // Only use the replacement's low sample if it actually has a low range.
        if (replacement->normalRangeLo != 0 && replacement->lowNotesSound.sample != nullptr) {
            return &replacement->lowNotesSound;
        }

        return &replacement->normalNotesSound;
    }

    if (semitone > original->normalRangeHi) {
        // Only use the replacement's high sample if it actually has a high range.
        if (replacement->normalRangeHi != 0x7F && replacement->highNotesSound.sample != nullptr) {
            return &replacement->highNotesSound;
        }

        return &replacement->normalNotesSound;
    }

    return &replacement->normalNotesSound;
}

// Resets remap state whenever the main BGM sequence player starts a new sequence
static void OnSeqPlayerInit(int32_t playerIdx, int32_t seqId) {
    if (playerIdx != SEQ_PLAYER_BGM_MAIN) {
        return;
    }

    ResetState();
}

// Replaces normal music instrument IDs with their shuffled per-sequence remap
static void OnSeqInstrumentSet(void* channelPtr, uint8_t* instId) {
    if (!CVAR_RANDOM_MUSIC_INSTRUMENTS_VALUE || instId == nullptr || !IsNormalInstrument(*instId)) {
        return;
    }

    SequenceChannel* channel = static_cast<SequenceChannel*>(channelPtr);
    if (!IsMainBgmChannel(channel)) {
        return;
    }

    const uint8_t fontId = channel->fontId;
    const uint8_t originalInstId = *instId;

    *instId = GetOrCreateRemappedInstrument(fontId, originalInstId);
}

// Overrides replacement instrument sample selection to preserve the original instrument's range behavior
static void OnSeqInstrumentGetSound(void* instrumentPtr, int32_t semitone, void** soundPtr) {
    if (!CVAR_RANDOM_MUSIC_INSTRUMENTS_VALUE || instrumentPtr == nullptr || soundPtr == nullptr) {
        return;
    }

    Instrument* replacement = static_cast<Instrument*>(instrumentPtr);
    Instrument* original = FindOriginalRangeInstrument(replacement);

    if (original == nullptr) {
        return;
    }

    SoundFontSound* sound = GetReplacementSoundUsingOriginalRange(original, replacement, semitone);

    if (sound != nullptr) {
        *soundPtr = sound;
    }
}

static void RegisterHooks() {
    COND_HOOK(OnSeqPlayerInit, CVAR_RANDOM_MUSIC_INSTRUMENTS_VALUE, OnSeqPlayerInit);
    COND_HOOK(OnSeqInstrumentSet, CVAR_RANDOM_MUSIC_INSTRUMENTS_VALUE, OnSeqInstrumentSet);
    COND_HOOK(OnSeqInstrumentGetSound, CVAR_RANDOM_MUSIC_INSTRUMENTS_VALUE, OnSeqInstrumentGetSound);
}

} // namespace RandomMusicInstruments

static RegisterShipInitFunc initFunc(RandomMusicInstruments::RegisterHooks, { CVAR_RANDOM_MUSIC_INSTRUMENTS_NAME });
