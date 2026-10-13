#ifndef AUDIO_REVERB_H
#define AUDIO_REVERB_H

#include "PR/ultratypes.h"
#include "audio/soundfont.h"

#define REVERB_INDEX_NONE -1

typedef enum ReverbDataType {
    /* 0 */ REVERB_DATA_TYPE_SETTINGS, // Reverb Settings (Init)
    /* 1 */ REVERB_DATA_TYPE_DELAY, // Reverb Delay (numSamples)
    /* 2 */ REVERB_DATA_TYPE_DECAY, // Reverb Decay Ratio
    /* 3 */ REVERB_DATA_TYPE_SUB_VOLUME, // Reverb Sub-Volume
    /* 4 */ REVERB_DATA_TYPE_VOLUME, // Reverb Volume
    /* 5 */ REVERB_DATA_TYPE_LEAK_RIGHT, // Reverb Leak Right Channel
    /* 6 */ REVERB_DATA_TYPE_LEAK_LEFT, // Reverb Leak Left Channel
    /* 7 */ REVERB_DATA_TYPE_FILTER_LEFT, // Reverb Left Filter
    /* 8 */ REVERB_DATA_TYPE_FILTER_RIGHT, // Reverb Right Filter
    /* 9 */ REVERB_DATA_TYPE_9 // Reverb Unk
} ReverbDataType;

typedef struct {
    /* 0x00 */ u8 downsampleRate;
    /* 0x02 */ u16 delayNumSamples;
    /* 0x04 */ u16 decayRatio; // determines how fast reverb dissipates
    /* 0x06 */ u16 subDelay;
    /* 0x08 */ u16 subVolume;
    /* 0x0A */ u16 volume;
    /* 0x0C */ u16 leakRtl;
    /* 0x0E */ u16 leakLtr;
    /* 0x10 */ s8 mixReverbIndex;
    /* 0x12 */ u16 mixReverbStrength;
    /* 0x14 */ s16 lowPassFilterCutoffLeft;
    /* 0x16 */ s16 lowPassFilterCutoffRight;
} ReverbSettings; // size = 0x18

/**
 * Stores an entry of decompressed samples in a reverb ring buffer.
 * By storing the sample in a ring buffer, the time it takes to loop
 * around back to the same sample acts as a delay, leading to an echo effect.
 */
typedef struct {
    /* 0x00 */ s16 numSamplesAfterDownsampling; // never read
    /* 0x02 */ s16 numSamples; // never read
    /* 0x04 */ s16* toDownsampleLeft;
    /* 0x08 */ s16* toDownsampleRight; // data pointed to by left and right are adjacent in memory
    /* 0x0C */ s32 startPos; // start pos in ring buffer
    /* 0x10 */ s16 size; // first length in ring buffer (from startPos, at most until end)
    /* 0x12 */ s16 wrappedSize; // second length in ring buffer (from pos 0)
    /* 0x14 */ u16 loadResamplePitch;
    /* 0x16 */ u16 saveResamplePitch;
    /* 0x18 */ u16 saveResampleNumSamples;
} ReverbBufferEntry; // size = 0x1C

typedef struct {
    /* 0x000 */ u8 resampleFlags;
    /* 0x001 */ u8 useReverb;
    /* 0x002 */ u8 framesToIgnore;
    /* 0x003 */ u8 curFrame;
    /* 0x004 */ u8 downsampleRate;
    /* 0x005 */ s8 mixReverbIndex; // mix in reverb from this index. set to 0xFF to not mix any
    /* 0x006 */ u16 delayNumSamples; // number of samples between echos
    /* 0x008 */ s16 mixReverbStrength; // the gain/amount to mix in reverb from mixReverbIndex
    /* 0x00A */ s16 volume;
    /* 0x00C */ u16 decayRatio; // determines how fast reverb dissipate
    /* 0x00E */ u16 downsamplePitch;
    /* 0x010 */ s16 leakRtl;
    /* 0x012 */ s16 leakLtr;
    /* 0x014 */ u16 subDelay; // number of samples between sub echos
    /* 0x016 */ s16 subVolume; // strength of the sub echos
    /* 0x018 */ u8 resampleEffectOn;
    /* 0x019 */ s8 resampleEffectExtraSamples;
    /* 0x01A */ u16 resampleEffectLoadUnk;
    /* 0x01C */ u16 resampleEffectSaveUnk;
    /* 0x01E */ u8 delayNumSamplesAfterDownsampling;
    /* 0x020 */ s32 nextReverbBufPos;
    /* 0x024 */ s32 delayNumSamplesUnk; // May be bufSizePerChan
    /* 0x028 */ s16* leftReverbBuf;
    /* 0x02C */ s16* rightReverbBuf;
    /* 0x030 */ s16* leftLoadResampleBuf;
    /* 0x034 */ s16* rightLoadResampleBuf;
    /* 0x038 */ s16* leftSaveResampleBuf;
    /* 0x03C */ s16* rightSaveResampleBuf;
    /* 0x040 */ ReverbBufferEntry bufEntry[2][5];
    /* 0x158 */ ReverbBufferEntry subBufEntry[2][5];
    /* 0x270 */ s16* filterLeft;
    /* 0x274 */ s16* filterRight;
    /* 0x278 */ s16* filterLeftInit;
    /* 0x27C */ s16* filterRightInit;
    /* 0x280 */ s16* filterLeftState;
    /* 0x284 */ s16* filterRightState;
    /* 0x288 */ TunedSample tunedSample;
    /* 0x290 */ Sample sample;
    /* 0x2A0 */ AdpcmLoop loop;
} SynthesisReverb; // size = 0x2D0

extern ReverbSettings* gReverbSettingsTable[10];

#endif
