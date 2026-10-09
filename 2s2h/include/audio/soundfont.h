#ifndef AUDIO_SOUNDFONT_H
#define AUDIO_SOUNDFONT_H

#include "PR/ultratypes.h"

struct EnvelopePoint;

typedef struct AdpcmLoop {
    /* 0x00 */ u32 start;
    /* 0x04 */ u32 loopEnd;   // numSamples position into the sample where the loop ends
    /* 0x08 */ u32 count; // The number of times the loop is played before the sound completes. Setting count to -1 indicates that the loop should play indefinitely.
    /* 0x0C */ u32 sampleEnd; // total number of s16-samples in the sample audio clip
    /* 0x10 */ s16 predictorState[16]; // only exists if count != 0. 8-byte aligned
} AdpcmLoop; // size = 0x30 (or 0x10)

/**
 * The procedure used to design the codeBook is based on an adaptive clustering algorithm.
 * The size of the codeBook is (8 * order * numPredictors) and is 8-byte aligned
 */
typedef struct AdpcmBook {
    /* 0x0 */ s32 order;
    /* 0x4 */ s32 numPredictors;
    /* 0x8 */ s16* codeBook; // a table of prediction coefficients that the coder selects from to optimize sound quality.
} AdpcmBook; // size >= 0x8

typedef enum SampleCodec {
    /* 0 */ CODEC_ADPCM, // 16 2-byte samples (32 bytes) compressed into 4-bit samples (8 bytes) + 1 header byte
    /* 1 */ CODEC_S8, // 16 2-byte samples (32 bytes) compressed into 8-bit samples (16 bytes)
    /* 2 */ CODEC_S16_INMEMORY,
    /* 3 */ CODEC_SMALL_ADPCM, // 16 2-byte samples (32 bytes) compressed into 2-bit samples (4 bytes) + 1 header byte
    /* 4 */ CODEC_REVERB,
    /* 5 */ CODEC_S16,
    /* 6 */ CODEC_UNK6,
    /* 7 */ CODEC_UNK7, // processed as uncompressed samples
    // 2S2H [Custom Audio] allow decoding other audio formats
    /* 8 */ CODEC_OPUS,
} SampleCodec;

typedef enum SampleMedium {
    /* 0 */ MEDIUM_RAM,
    /* 1 */ MEDIUM_UNK,
    /* 2 */ MEDIUM_CART,
    /* 3 */ MEDIUM_DISK_DRIVE,
    /* 5 */ MEDIUM_RAM_UNLOADED = 5
} SampleMedium;

typedef struct Sample {
    union {
        struct {
        ///* 0x0 */ u32 unk_0 : 1;
        /* 0x0 */ u32 codec : 4; // The state of compression or decompression, See `SampleCodec`
        /* 0x0 */ u32 medium : 2; // Medium where sample is currently stored. See `SampleMedium`
        /* 0x0 */ u32 unk_bit26 : 1;
        /* 0x0 */ u32 isRelocated : 1; // Has the sample header been relocated (offsets to pointers)

        };
        u32 asU32;
    };
    /* 0x1 */ u32 size;  // Size of the sample
    u32 fileSize;
    /* 0x4 */ u8* sampleAddr; // Raw sample data. Offset from the start of the sample bank or absolute address to either rom or ram
    /* 0x8 */ AdpcmLoop* loop; // Adpcm loop parameters used by the sample. Offset from the start of the sound font / pointer to ram
    /* 0xC */ AdpcmBook* book; // Adpcm book parameters used by the sample. Offset from the start of the sound font / pointer to ram
} Sample; // size = 0x10

typedef struct TunedSample {
    /* 0x0 */ Sample* sample;
    /* 0x4 */ f32 tuning; // frequency scale factor
} TunedSample; // size = 0x8

/**
 * Stores an entry of decompressed samples in a reverb ring buffer.
 * By storing the sample in a ring buffer, the time it takes to loop
 * around back to the same sample acts as a delay, leading to an echo effect.
 */
typedef struct Instrument {
    /* 0x00 */ u8 isRelocated; // have the envelope and all samples been relocated (offsets to pointers)
    /* 0x01 */ u8 normalRangeLo;
    /* 0x02 */ u8 normalRangeHi;
    /* 0x03 */ u8 adsrDecayIndex; // index used to obtain adsr decay rate from adsrDecayTable
    /* 0x04 */ struct EnvelopePoint* envelope;
    /* 0x08 */ TunedSample lowPitchTunedSample;
    /* 0x10 */ TunedSample normalPitchTunedSample;
    /* 0x18 */ TunedSample highPitchTunedSample;
} Instrument; // size = 0x20

typedef struct Drum {
    /* 0x0 */ u8 adsrDecayIndex; // index used to obtain adsr decay rate from adsrDecayTable
    /* 0x1 */ u8 pan;
    /* 0x2 */ u8 isRelocated; // have tunedSample.sample and envelope been relocated (offsets to pointers)
    /* 0x4 */ TunedSample tunedSample;
    /* 0xC */ struct EnvelopePoint* envelope;
} Drum; // size = 0x10

typedef struct SoundEffect {
    /* 0x0 */ TunedSample tunedSample;
} SoundEffect; // size = 0x8

typedef struct SoundFont {
    /* 0x00 */ u8 numInstruments;
    /* 0x01 */ u8 numDrums;
    /* 0x02 */ u8 sampleBankId1;
    /* 0x03 */ u8 sampleBankId2;
    /* 0x04 */ u16 numSfx;
    /* 0x08 */ Instrument** instruments;
    /* 0x0C */ Drum** drums;
    /* 0x10 */ SoundEffect* soundEffects;
    s32 fntIndex;
} SoundFont; // size = 0x14

#endif
