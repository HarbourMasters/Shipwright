#ifndef Z64AUDIO_H
#define Z64AUDIO_H

#include <ship/utils/binarytools/endianness.h>

#include "PR/ultratypes.h"
#include "PR/os_voice.h"
#include "audiothread_cmd.h"
#include "libc/stddef.h"
#include "unk.h"
#include "audiothread_cmd.h"

#include "audio/effects.h"
#include "audio/heap.h"
#include "audio/load.h"
#include "audio/reverb.h"
#include "audio/soundfont.h"
#include "sequence.h"

#define NO_LAYER ((SequenceLayer*)(-1))

#define TATUMS_PER_BEAT 48

#define IS_SEQUENCE_CHANNEL_VALID(ptr) ((uintptr_t)(ptr) != (uintptr_t)&gAudioCtx.sequenceChannelNone)
#define SEQ_NUM_CHANNELS 16
#define SEQ_IO_VAL_NONE -1

typedef enum {
    /* 0 */ AUDIO_HEAP_RESET_STATE_NONE,
    /* 1 */ AUDIO_HEAP_RESET_STATE_RESETTING,
    /* 2 */ AUDIO_HEAP_RESET_STATE_RESETTING_ALT // Never set to
} AudioHeapResetState;

typedef enum {
    /* 0x00 */ AUDIO_CUSTOM_FUNCTION_SEQ_0,
    /* 0x01 */ AUDIO_CUSTOM_FUNCTION_SEQ_1,
    /* 0x02 */ AUDIO_CUSTOM_FUNCTION_SEQ_2,
    /* 0x03 */ AUDIO_CUSTOM_FUNCTION_SEQ_3,
    /* 0xFE */ AUDIO_CUSTOM_FUNCTION_SYNTH = 0xFE,
    /* 0xFF */ AUDIO_CUSTOM_FUNCTION_REVERB
} AudioCustomFunctions;

typedef enum {
    /* 0 */ SEQPLAYER_STATE_0,
    /* 1 */ SEQPLAYER_STATE_FADE_IN,
    /* 2 */ SEQPLAYER_STATE_FADE_OUT
} SeqPlayerState;

#define MAX_CHANNELS_PER_BANK 3

#define MUTE_FLAGS_STOP_SAMPLES (1 << 3)           // prevent further noteSubEus from playing
#define MUTE_FLAGS_STOP_LAYER (1 << 4)           // stop something in seqLayer scripts
#define MUTE_FLAGS_SOFTEN (1 << 5)      // lower volume, by default to half
#define MUTE_FLAGS_STOP_NOTES (1 << 6)  // prevent further notes from playing
#define MUTE_FLAGS_STOP_SCRIPT (1 << 7) // stop processing sequence/channel scripts

#define AUDIO_LERPIMP(v0, v1, t) (v0 + ((v1 - v0) * t))

// size of a single sample point
#define SAMPLE_SIZE sizeof(s16)

// Samples are processed in groups of 16 called a "frame"
#define SAMPLES_PER_FRAME ADPCMFSIZE

// The length of one left/right channel is 13 frames
#define DMEM_1CH_SIZE (13 * SAMPLES_PER_FRAME * SAMPLE_SIZE)
// Both left and right channels
#define DMEM_2CH_SIZE (2 * DMEM_1CH_SIZE)

#define AIBUF_LEN (88 * SAMPLES_PER_FRAME) // number of samples
#define AIBUF_SIZE (AIBUF_LEN * SAMPLE_SIZE) // number of bytes

// Filter sizes
#define FILTER_SIZE (8 * SAMPLE_SIZE)
#define FILTER_BUF_PART1 (8 * SAMPLE_SIZE)
#define FILTER_BUF_PART2 (8 * SAMPLE_SIZE)

// Must be the same amount of samples as copied by aDuplicate() (audio microcode)
#define WAVE_SAMPLE_COUNT 64

#define AUDIO_RELOCATED_ADDRESS_START K0BASE

// To be used with AudioThread_CountAndReleaseNotes()
#define AUDIO_NOTE_RELEASE (1 << 0)
#define AUDIO_NOTE_SAMPLE_NOTES (1 << 1)

typedef enum {
    /*  0x1 */ AUDIO_ERROR_NO_INST = 1,
    /*  0x3 */ AUDIO_ERROR_INVALID_INST_ID = 3,
    /*  0x4 */ AUDIO_ERROR_INVALID_DRUM_SFX_ID,
    /*  0x5 */ AUDIO_ERROR_NO_DRUM_SFX,
    /* 0x10 */ AUDIO_ERROR_FONT_NOT_LOADED = 0x10
} AudioError;

#define AUDIO_ERROR(fontId, id, err) (((fontId << 8) + id) + (err << 24))

typedef enum {
    /* 0 */ SOUNDMODE_STEREO,
    /* 1 */ SOUNDMODE_HEADSET,
    /* 2 */ SOUNDMODE_SURROUND_EXTERNAL,
    /* 3 */ SOUNDMODE_MONO,
    /* 4 */ SOUNDMODE_SURROUND
} SoundMode;

struct Note;
struct NotePool;
struct SequenceChannel;
struct SequenceLayer;

// A node in a circularly linked list. Each node is either a head or an item:
// - Items can be either detached (prev = NULL), or attached to a list.
//   'value' points to something of interest.
// - List heads are always attached; if a list is empty, its head points
//   to itself. 'count' contains the size of the list.
// If the list holds notes, 'pool' points back to the pool where it lives.
// Otherwise, that member is NULL.
typedef struct AudioListItem {
    /* 0x00 */ struct AudioListItem* prev;
    /* 0x04 */ struct AudioListItem* next;
    union {
        /* 0x08 */ void* value; // either Note* or SequenceLayer*
        /* 0x08 */ s32 count;
               } u;
    /* 0x0C */ struct NotePool* pool;
} AudioListItem; // size = 0x10

typedef struct NotePool {
    /* 0x00 */ AudioListItem disabled;
    /* 0x10 */ AudioListItem decaying;
    /* 0x20 */ AudioListItem releasing;
    /* 0x30 */ AudioListItem active;
} NotePool; // size = 0x40

typedef struct {
    /* 0x00 */ u8* pc; // program counter
    /* 0x04 */ u8* stack[4];
    /* 0x14 */ u8 remLoopIters[4]; // remaining loop iterations
    /* 0x18 */ u8 depth;
    /* 0x19 */ s8 value;
} SeqScriptState; // size = 0x1C

// Also known as a Group, according to debug strings.
typedef struct SequencePlayer {
    /* 0x000 */ u8 enabled : 1;
    /* 0x000 */ u8 finished : 1;
    /* 0x000 */ u8 muted : 1;
    /* 0x000 */ u8 seqDmaInProgress : 1;
    /* 0x000 */ u8 fontDmaInProgress : 1;
    /* 0x000 */ u8 recalculateVolume : 1;
    /* 0x000 */ u8 stopScript : 1;
    /* 0x000 */ u8 applyBend : 1;
    /* 0x001 */ u8 state;
    /* 0x002 */ u8 noteAllocPolicy;
    /* 0x003 */ u8 muteFlags;
    // 2S2H [Custom Audio]. Was originally u8 seqId. Made 16 bit to allow for more than 255 sequences.
    /* 0x004 */ u16 seqId;
    // 2S2H [Custom Audio]. Was originally u8 defaultFont. Made 16 bit to allow for more than 255 sound fonts.
    /* 0x005 */ u16 defaultFont;
    /* 0x006 */ u8 unk_06[1];
    /* 0x007 */ s8 playerIndex;
    /* 0x008 */ u16 tempo; // tatums per minute
    /* 0x00A */ u16 tempoAcc;
    /* 0x00C */ s16 tempoChange;
    /* 0x00E */ s16 transposition;
    /* 0x010 */ u16 delay;
    /* 0x012 */ u16 fadeTimer;
    /* 0x014 */ u16 storedFadeTimer;
    /* 0x016 */ u16 unk_16;
    /* 0x018 */ u8* seqData;
    /* 0x01C */ f32 fadeVolume;
    /* 0x020 */ f32 fadeVelocity;
    /* 0x024 */ f32 volume;
    /* 0x028 */ f32 muteVolumeScale;
    /* 0x02C */ f32 fadeVolumeScale;
    /* 0x030 */ f32 appliedFadeVolume;
    /* 0x034 */ f32 bend;
    /* 0x038 */ struct SequenceChannel* channels[16];
    /* 0x078 */ SeqScriptState scriptState;
    /* 0x094 */ u8* shortNoteVelocityTable;
    /* 0x098 */ u8* shortNoteGateTimeTable;
    /* 0x09C */ NotePool notePool;
    /* 0x0DC */ s32 skipTicks;
    /* 0x0E0 */ u32 scriptCounter;
    /* 0x0E4 */ UNK_TYPE1 unk_E4[0x74]; // unused struct members for sequence/sound font dma management, according to sm64 decomp
    /* 0x158 */ s8 seqScriptIO[8];
    // #region 2S2H [Port][Audio]
    /*       */ f32 portVolumeScale; // volume from the sliders in the menubar
} SequencePlayer; // size = 0x160

typedef union {
    struct {
        /* 0x0 */ u8 unused : 2;
        /* 0x0 */ u8 type : 2;
        /* 0x0 */ u8 strongRight : 1;
        /* 0x0 */ u8 strongLeft : 1;
        /* 0x0 */ u8 strongReverbRight : 1;
        /* 0x0 */ u8 strongReverbLeft : 1;
    };
    /* 0x0 */ u8 asByte;
} StereoData; // size = 0x1

typedef struct {
    /* 0x00 */ u8 targetReverbVol;
    /* 0x01 */ u8 gain; // Increases volume by a multiplicative scaling factor. Represented as a UQ4.4 number
    /* 0x02 */ u8 pan;
    /* 0x03 */ u8 surroundEffectIndex;
    /* 0x04 */ StereoData stereoData;
    /* 0x05 */ u8 combFilterSize;
    /* 0x06 */ u16 combFilterGain;
    /* 0x08 */ f32 freqScale;
    /* 0x0C */ f32 velocity;
    /* 0x10 */ s16* filter;
    /* 0x14 */ s16* filterBuf;
} NoteAttributes; // size = 0x18

// Also known as a SubTrack, according to sm64 debug strings.
typedef struct SequenceChannel {
    /* 0x00 */ u8 enabled : 1;
    /* 0x00 */ u8 finished : 1;
    /* 0x00 */ u8 stopScript : 1;
    /* 0x00 */ u8 muted : 1; // sets SequenceLayer.muted
    /* 0x00 */ u8 hasInstrument : 1;
    /* 0x00 */ u8 stereoHeadsetEffects : 1;
    /* 0x00 */ u8 largeNotes : 1; // notes specify duration and velocity
    /* 0x00 */ u8 unused : 1;
    union {
        struct {
            /* 0x01 */ u8 freqScale : 1;
            /* 0x01 */ u8 volume : 1;
            /* 0x01 */ u8 pan : 1;
        } s;
        /* 0x01 */ u8 asByte;
    } changes;
    /* 0x02 */ u8 noteAllocPolicy;
    /* 0x03 */ u8 muteFlags;
    /* 0x04 */ u8 targetReverbVol; // or dry/wet mix
    /* 0x05 */ u8 notePriority; // 0-3
    /* 0x06 */ u8 someOtherPriority;
    // 2S2H [Custom Audio]. Was originally u8 fontId. Made 16 bit to allow for more than 255 sound fonts.
    /* 0x07 */ u16 fontId;
    /* 0x08 */ u8 reverbIndex;
    /* 0x09 */ u8 bookOffset;
    /* 0x0A */ u8 newPan;
    /* 0x0B */ u8 panChannelWeight;  // proportion of pan that comes from the channel (0..128)
    /* 0x0C */ u8 gain; // Increases volume by a multiplicative scaling factor. Represented as a UQ4.4 number
    /* 0x0D */ u8 velocityRandomVariance;
    /* 0x0E */ u8 gateTimeRandomVariance;
    /* 0x0F */ u8 combFilterSize;
    /* 0x10 */ u8 surroundEffectIndex;
    /* 0x11 */ u8 channelIndex;
    /* 0x12 */ VibratoSubStruct vibrato;
    /* 0x20 */ u16 delay;
    /* 0x22 */ u16 combFilterGain;
    /* 0x24 */ u16 unk_22; // Used for indexing data
    /* 0x26 */ s16 instOrWave; // either 0 (none), instrument index + 1, or
                             // 0x80..0x83 for sawtooth/triangle/sine/square waves.
    /* 0x28 */ s16 transposition;
    /* 0x2C */ f32 volumeScale;
    /* 0x30 */ f32 volume;
    /* 0x34 */ s32 pan;
    /* 0x38 */ f32 appliedVolume;
    /* 0x3C */ f32 freqScale;
    /* 0x40 */ u8 (*dynTable)[][2];
    /* 0x44 */ struct Note* noteUnused;
    /* 0x48 */ struct SequenceLayer* layerUnused;
    /* 0x4C */ Instrument* instrument;
    /* 0x50 */ SequencePlayer* seqPlayer;
    /* 0x54 */ struct SequenceLayer* layers[4];
    /* 0x64 */ SeqScriptState scriptState;
    /* 0x80 */ AdsrSettings adsr;
    /* 0x88 */ NotePool notePool;
    /* 0xC8 */ s8 seqScriptIO[8]; // bridge between sound script and audio lib, "io ports"
    /* 0xD0 */ u8* sfxState; // SfxChannelState
    /* 0xD4 */ s16* filter;
    /* 0xD8 */ StereoData stereoData;
    /* 0xDC */ s32 startSamplePos;
    /* 0xE0 */ s32 unk_E0;
} SequenceChannel; // size = 0xE4

// Might also be known as a Track, according to sm64 debug strings (?).
typedef struct SequenceLayer {
    /* 0x00 */ u8 enabled : 1;
    /* 0x00 */ u8 finished : 1;
    /* 0x00 */ u8 muted : 1;
    /* 0x00 */ u8 continuousNotes : 1; // keep the same note for consecutive notes with the same sound
    /* 0x00 */ u8 bit3 : 1; // "loaded"?
    /* 0x00 */ u8 ignoreDrumPan : 1;
    /* 0x00 */ u8 bit1 : 1; // "has initialized continuous notes"?
    /* 0x00 */ u8 notePropertiesNeedInit : 1;
    /* 0x01 */ StereoData stereoData;
    /* 0x02 */ u8 instOrWave;
    /* 0x03 */ u8 gateTime;
    /* 0x04 */ u8 semitone;
    /* 0x05 */ u8 portamentoTargetNote;
    /* 0x06 */ u8 pan; // 0..128
    /* 0x07 */ u8 notePan;
    /* 0x08 */ u8 surroundEffectIndex;
    /* 0x09 */ u8 targetReverbVol;
    union {
        struct {
            /* 0x0A */ u16 bit_0 : 1;
            /* 0x0A */ u16 bit_1 : 1;
            /* 0x0A */ u16 bit_2 : 1;
            /* 0x0A */ u16 useVibrato : 1;
            /* 0x0A */ u16 bit_4 : 1;
            /* 0x0A */ u16 bit_5 : 1;
            /* 0x0A */ u16 bit_6 : 1;
            /* 0x0A */ u16 bit_7 : 1;
            /* 0x0A */ u16 bit_8 : 1;
            /* 0x0A */ u16 bit_9 : 1;
            /* 0x0A */ u16 bit_A : 1;
            /* 0x0A */ u16 bit_B : 1;
            /* 0x0A */ u16 bit_C : 1;
            /* 0x0A */ u16 bit_D : 1;
            /* 0x0A */ u16 bit_E : 1;
            /* 0x0A */ u16 bit_F : 1;
        } s;
        /* 0x0A */ u16 asByte;
    } unk_0A;
    /* 0x0C */ VibratoSubStruct vibrato;
    /* 0x1A */ s16 delay;
    /* 0x1C */ s16 gateDelay;
    /* 0x1E */ s16 delay2;
    /* 0x20 */ u16 portamentoTime;
    /* 0x22 */ s16 transposition; // #semitones added to play commands
                                  // (seq instruction encoding only allows referring to the limited range
                                  // 0..0x3F; this makes 0x40..0x7F accessible as well)
    /* 0x24 */ s16 shortNoteDefaultDelay;
    /* 0x26 */ s16 lastDelay;
    /* 0x28 */ AdsrSettings adsr;
    /* 0x30 */ Portamento portamento;
    /* 0x3C */ struct Note* note;
    /* 0x40 */ f32 freqScale;
    /* 0x44 */ f32 bend;
    /* 0x48 */ f32 velocitySquare2;
    /* 0x4C */ f32 velocitySquare; // not sure which one of those corresponds to the sm64 original
    /* 0x50 */ f32 noteVelocity;
    /* 0x54 */ f32 noteFreqScale;
    /* 0x58 */ Instrument* instrument;
    /* 0x5C */ TunedSample* tunedSample;
    /* 0x60 */ SequenceChannel* channel; // Not SequenceChannel?
    /* 0x64 */ SeqScriptState scriptState;
    /* 0x80 */ AudioListItem listItem;
} SequenceLayer; // size = 0x90

typedef struct {
    /* 0x000 */ s16 adpcmState[16];
    /* 0x020 */ s16 finalResampleState[16];
    /* 0x040 */ s16 filterState[32];
    /* 0x080 */ s16 unusedState[16];
    /* 0x0A0 */ s16 haasEffectDelayState[32];
    /* 0x0E0 */ s16 combFilterState[128];
    /* 0x1E0 */ s16 surroundEffectState[128];
} NoteSynthesisBuffers; // size = 0x2E0

struct OggOpusFile;

typedef struct {
    /* 0x00 */ u8 atLoopPoint : 1;
    /* 0x00 */ u8 stopLoop : 1;
    /* 0x01 */ u8 sampleDmaIndex;
    /* 0x02 */ u8 prevHaasEffectLeftDelaySize;
    /* 0x03 */ u8 prevHaasEffectRightDelaySize;
    /* 0x04 */ u8 curReverbVol;
    /* 0x05 */ u8 numParts;
    /* 0x06 */ u16 samplePosFrac; // Fractional part of the sample position
    /* 0x08 */ u16 surroundEffectGain;
    /* 0x0C */ s32 samplePosInt; // Integer part of the sample position
    /* 0x10 */ NoteSynthesisBuffers* synthesisBuffers;
    /* 0x14 */ s16 curVolLeft;
    /* 0x16 */ s16 curVolRight;
    /* 0x18 */ UNK_TYPE1 unk_14[0x6];
    /* 0x1E */ u8 combFilterNeedsInit;
    /* 0x1F */ u8 unk_1F;
    struct OggOpusFile* opusFile; // Only for streamed opus audio
    ///* 0x20 */ UNK_TYPE1 unk_20[0x4];
} NoteSynthesisState; // size = 0x24

typedef enum {
    /* 0 */ PLAYBACK_STATUS_0,
    /* 1 */ PLAYBACK_STATUS_1,
    /* 2 */ PLAYBACK_STATUS_2
} NotePlaybackStatus;

typedef struct {
    /* 0x00 */ u8 priority;
    /* 0x01 */ u8 waveId;
    /* 0x02 */ u8 harmonicIndex; // the harmonic index for the synthetic wave contained in gWaveSamples (also matches the base 2 logarithm of the harmonic order)
    // 2S2H [Custom Audio]. Was originally u8 fontId. Made 16 bit to allow for more than 255 sound fonts.
    /* 0x03 */ u16 fontId;
    /* 0x04 */ u8 status;
    /* 0x05 */ u8 stereoHeadsetEffects;
    /* 0x06 */ s16 adsrVolScaleUnused;
    /* 0x08 */ f32 portamentoFreqScale;
    /* 0x0C */ f32 vibratoFreqScale;
    /* 0x18 */ SequenceLayer* wantedParentLayer;
    /* 0x14 */ SequenceLayer* parentLayer;
    /* 0x10 */ SequenceLayer* prevParentLayer;
    /* 0x1C */ NoteAttributes attributes;
    /* 0x34 */ AdsrState adsr;
    /* 0x54 */ Portamento portamento;
    /* 0x60 */ VibratoState vibratoState;
    /* 0x7C */ UNK_TYPE1 pad7C[0x4];
    /* 0x80 */ u8 unk_80;
    /* 0x84 */ u32 startSamplePos;
    /* 0x88 */ UNK_TYPE1 unk_BC[0x1C]; 
} NotePlaybackState; // size = 0xA4

typedef struct {
    struct {
        /* 0x00 */ volatile u8 enabled : 1;
        /* 0x00 */ u8 needsInit : 1;
        /* 0x00 */ u8 finished : 1;
        /* 0x00 */ u8 unused : 1;
        /* 0x00 */ u8 strongRight : 1;
        /* 0x00 */ u8 strongLeft : 1;
        /* 0x00 */ u8 strongReverbRight : 1;
        /* 0x00 */ u8 strongReverbLeft : 1;
    } bitField0;
    struct {
        /* 0x01 */ u8 reverbIndex : 3;
        /* 0x01 */ u8 bookOffset : 2;
        /* 0x01 */ u8 isSyntheticWave : 1;
        /* 0x01 */ u8 hasTwoParts : 1;
        /* 0x01 */ u8 useHaasEffect : 1;
    } bitField1;
    /* 0x02 */ u8 gain; // Increases volume by a multiplicative scaling factor. Represented as a UQ4.4 number
    /* 0x03 */ u8 haasEffectLeftDelaySize;
    /* 0x04 */ u8 haasEffectRightDelaySize;
    /* 0x05 */ u8 targetReverbVol;
    /* 0x06 */ u8 harmonicIndexCurAndPrev; // bits 3..2 store curHarmonicIndex, bits 1..0 store prevHarmonicIndex
    /* 0x07 */ u8 combFilterSize;
    /* 0x08 */ u16 targetVolLeft;
    /* 0x0A */ u16 targetVolRight;
    /* 0x0C */ u16 frequencyFixedPoint;
    /* 0x0E */ u16 combFilterGain;
        union {
    /* 0x10 */ TunedSample* tunedSample;
    /* 0x10 */ s16* waveSampleAddr; // used for synthetic waves
        };
    /* 0x14 */ s16* filter;
    /* 0x18 */ UNK_TYPE1 unk_18;
    /* 0x19 */ u8 surroundEffectIndex;
    /* 0x1A */ UNK_TYPE1 unk_1A[0x6];
} NoteSampleState; // size = 0x20

typedef struct Note {
    /* 0x00 */ AudioListItem listItem;
    /* 0x10 */ NoteSynthesisState synthesisState;
    /* 0x34 */ NotePlaybackState playbackState;
    /* 0xD8 */ NoteSampleState sampleState;
} Note; // size = 0xF8

/**
 * The high-level audio specifications requested when initializing or resetting the audio pool.
 * Most often resets during scene transitions, but will highly depend on game play.
 */ 
typedef struct {
    /* 0x00 */ u32 samplingFreq; // Target sampling rate in Hz
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 numNotes;
    /* 0x06 */ u8 numSequencePlayers;
    /* 0x07 */ u8 unk_07; // unused, set to zero
    /* 0x08 */ u8 unk_08; // unused, set to zero
    /* 0x09 */ u8 numReverbs;
    /* 0x0C */ ReverbSettings* reverbSettings;
    /* 0x10 */ u16 sampleDmaBufSize1;
    /* 0x12 */ u16 sampleDmaBufSize2;
    /* 0x14 */ u16 unk_14;
    /* 0x18 */ size_t persistentSeqCacheSize;        // size of cache on audio pool to store sequences persistently
    /* 0x1C */ size_t persistentFontCacheSize;       // size of cache on audio pool to store soundFonts persistently
    /* 0x20 */ size_t persistentSampleBankCacheSize; // size of cache on audio pool to store entire sample banks persistently
    /* 0x24 */ size_t temporarySeqCacheSize;         // size of cache on audio pool to store sequences temporarily
    /* 0x28 */ size_t temporaryFontCacheSize;        // size of cache on audio pool to store soundFonts temporarily
    /* 0x2C */ size_t temporarySampleBankCacheSize;  // size of cache on audio pool to store entire sample banks temporarily
    /* 0x30 */ size_t persistentSampleCacheSize;     // size of cache on audio pool to store individual samples persistently
    /* 0x34 */ size_t temporarySampleCacheSize;      // size of cache on audio pool to store individual samples temporarily
} AudioSpec; // size = 0x38

/**
 * The audio buffer stores the fully processed digital audio before it is sent to the audio interface (AI), then to the
 * digital-analog converter (DAC), then to play on the speakers. The audio buffer is written to by the rsp after
 * processing audio commands, and the audio buffer is read by AI which sends the data to the DAC.
 * This struct parameterizes that buffer.
 */
typedef struct {
    /* 0x00 */ s16 specUnk4;
    /* 0x02 */ u16 samplingFreq; // Target sampling rate in Hz
    /* 0x04 */ u16 aiSamplingFreq; // True sampling rate set to the audio interface (AI) for the audio digital-analog converter (DAC)
    /* 0x06 */ s16 numSamplesPerFrameTarget;
    /* 0x08 */ s16 numSamplesPerFrameMax;
    /* 0x0A */ s16 numSamplesPerFrameMin;
    /* 0x0C */ s16 updatesPerFrame; // for each frame of the audio thread (default 60 fps), number of updates to process audio
    /* 0x0E */ s16 numSamplesPerUpdate;
    /* 0x10 */ s16 numSamplesPerUpdateMax;
    /* 0x12 */ s16 numSamplesPerUpdateMin;
    /* 0x14 */ s16 numSequencePlayers;
    /* 0x18 */ f32 resampleRate;
    /* 0x1C */ f32 updatesPerFrameInv; // inverse (reciprocal) of updatesPerFrame
    /* 0x20 */ f32 updatesPerFrameInvScaled; // updatesPerFrameInv scaled down by a factor of 256
    /* 0x24 */ f32 updatesPerFrameScaled; // updatesPerFrame scaled down by a factor of 4
} AudioBufferParameters; // size = 0x28

typedef struct {
    union {
        u32 opArgs;
        struct {
#ifdef IS_BIGENDIAN
            u8 op;
            u8 arg0;
            u8 arg1;
            u8 arg2;
#else
            u8 arg2;
            u8 arg1;
            u8 arg0;
            u8 op;
#endif
        };
    };
    union {
        /* 0x4 */ void* data;
        /* 0x4 */ f32 asFloat;
        /* 0x4 */ s32 asInt;
        /* 0x4 */ u16 asUShort;
        /* 0x4 */ s8 asSbyte;
        /* 0x4 */ u8 asUbyte;
        /* 0x4 */ u32 asUInt;
        /* 0x4 */ uintptr_t asPtr;
    };
} AudioCmd;

typedef struct {
    /* 0x00 */ OSTask task;
    /* 0x40 */ OSMesgQueue* taskQueue;
    /* 0x44 */ void* unk_44; // probably a message that gets unused.
    /* 0x48 */ char unk_48[0x8];
} AudioTask; // size = 0x50

typedef struct {
    /* 0x0000 */ char unk_0000;
    /* 0x0001 */ s8 numSynthesisReverbs;
    /* 0x0002 */ u16 unk_2; // reads from audio spec unk_14, never used, always set to 0x7FFF
    /* 0x0004 */ u16 unk_4;
    /* 0x0006 */ char unk_0006[0xA];
    /* 0x0010 */ s16* adpcmCodeBook;
    /* 0x0014 */ NoteSampleState* sampleStateList;
    /* 0x0018 */ SynthesisReverb synthesisReverbs[4];
    /* 0x0B58 */ char unk_0B58[0x30];
    /* 0x0B88 */ Sample* usedSamples[128];
    /* 0x0D88 */ AudioPreloadReq preloadSampleStack[128];
    /* 0x1788 */ s32 numUsedSamples;
    /* 0x178C */ s32 preloadSampleStackTop;
    /* 0x1790 */ AudioAsyncLoad asyncLoads[0x10];
    /* 0x1D10 */ OSMesgQueue asyncLoadUnkMediumQueue;
    /* 0x1D28 */ char unk_1D08[0x40];
    /* 0x1D68 */ AudioAsyncLoad* curUnkMediumLoad;
    /* 0x1D6C */ u32 slowLoadPos;
    /* 0x1D70 */ AudioSlowLoad slowLoads[2];
    /* 0x1E38 */ OSPiHandle* cartHandle;
    /* 0x1E2C */ OSPiHandle* driveHandle;
    /* 0x1E40 */ OSMesgQueue externalLoadQueue;
    /* 0x1E58 */ OSMesg externalLoadMesgBuf[0x10];
    /* 0x1E98 */ OSMesgQueue preloadSampleQueue;
    /* 0x1EB0 */ OSMesg preloadSampleMesgBuf[0x10];
    /* 0x1EF0 */ OSMesgQueue curAudioFrameDmaQueue;
    /* 0x1F08 */ OSMesg currAudioFrameDmaMesgBuf[0x40];
    /* 0x2008 */ OSIoMesg currAudioFrameDmaIoMesgBuf[0x40];
    /* 0x2608 */ OSMesgQueue syncDmaQueue;
    /* 0x2620 */ OSMesg syncDmaMesg;
    /* 0x2624 */ OSIoMesg syncDmaIoMesg;
    /* 0x263C */ SampleDma* sampleDmas;
    /* 0x2640 */ u32 sampleDmaCount;
    /* 0x2644 */ u32 sampleDmaListSize1;
    /* 0x2648 */ s32 unused2648;
    /* 0x264C */ u8 sampleDmaReuseQueue1[0x100]; // read pos <= write pos, wrapping mod 256
    /* 0x274C */ u8 sampleDmaReuseQueue2[0x100];
    /* 0x284C */ u8 sampleDmaReuseQueue1RdPos; // Read position for dma 1
    /* 0x284D */ u8 sampleDmaReuseQueue2RdPos; // Read position for dma 2
    /* 0x284E */ u8 sampleDmaReuseQueue1WrPos; // Write position for dma 1
    /* 0x284F */ u8 sampleDmaReuseQueue2WrPos; // Write position for dma 2
    /* 0x2850 */ AudioTable* sequenceTable;
    /* 0x2854 */ AudioTable* soundFontTable;
    /* 0x2858 */ AudioTable* sampleBankTable;
    /* 0x285C */ char unk_285C[0x4];
    /* 0x2860 */ u8* sequenceFontTable;
    /* 0x2864 */ u16 numSequences;
    /* 0x2868 */ SoundFont* soundFontList;
    /* 0x286C */ AudioBufferParameters audioBufferParameters;
    /* 0x2994 */ f32 unk_2870;
    /* 0x2898 */ s32 sampleDmaBufSize1;
    /* 0x289C */ s32 sampleDmaBufSize2;
    /* 0x28A0 */ char unk_287C[0x10];
    /* 0x28B0 */ s32 sampleDmaBufSize;
    /* 0x28B4 */ s32 maxAudioCmds;
    /* 0x28B8 */ s32 numNotes;
    /* 0x2898 */ s16 maxTempo;
    /* 0x28BE */ s8 soundMode;
    /* 0x28C0 */ s32 totalTaskCount; // The total number of times the top-level function on the audio thread is run since the last audio reset
    /* 0x28C4 */ s32 curAudioFrameDmaCount;
    /* 0x28C8 */ s32 rspTaskIndex;
    /* 0x28CC */ s32 curAiBufferIndex;
    /* 0x28AC */ Acmd* abiCmdBufs[2]; // Pointer to audio heap where the audio binary interface command lists are stored. Two lists that alternative every frame
    /* 0x28B4 */ Acmd* curAbiCmdBuf;
    /* 0x28DC */ AudioTask* curTask;
    /* 0x28C0 */ AudioTask rspTask[2];
    /* 0x2980 */ f32 unk_2960;
    /* 0x2984*/ s32 refreshRate;
    /* 0x2988 */ s16* aiBuffers[3]; // Pointers to the audio buffer allocated on the initPool contained in the audio heap. Stores fully processed digital audio before transferring to the audio interface (AI)
    /* 0x2994 */ s16 numSamplesPerFrame[3]; // Number of samples to transfer to the audio interface buffer
    /* 0x299C */ u32 audioRandom;
    /* 0x29A0 */ s32 audioErrorFlags;
    /* 0x29A4 */ volatile u32 resetTimer;
    /* 0x29A8 */ u32 (*customSeqFunctions[4])(s8 value, SequenceChannel* channel);
    /* 0x29B8 */ s8 unk_29B8;
    /* 0x29BC */ s32 numAbiCmdsMax; // sMaxAbiCmdCnt
    /* 0x29C0 */ AudioAllocPool sessionPool; // A sub-pool to main pool, contains all sub-pools and data that changes every audio reset
    /* 0x29D0 */ AudioAllocPool externalPool; // pool allocated on an external device. Never used in game
    /* 0x29E0 */ AudioAllocPool initPool; // A sub-pool to the main pool, contains all sub-pools and data that persists every audio reset
    /* 0x29F0 */ AudioAllocPool miscPool; // A sub-pool to the session pool, 
    /* 0x2A00 */ char unk_29D0[0x20]; // probably two unused pools
    /* 0x2A20 */ AudioAllocPool cachePool; // The common pool for all cache entries
    /* 0x2A30 */ AudioAllocPool persistentCommonPool; // A sub-pool to the cache pool, contains all caches for data stored persistently
    /* 0x2A40 */ AudioAllocPool temporaryCommonPool; // A sub-pool to the cache pool, contains all caches for data stored temporarily
    /* 0x2A50 */ AudioCache seqCache; // Cache to store sequences
    /* 0x2B60 */ AudioCache fontCache; // Cache to store soundFonts
    /* 0x2C70 */ AudioCache sampleBankCache; // Cache for loading entire sample banks
    /* 0x2D80 */ AudioAllocPool permanentPool; // Pool to stores audio data that is always loaded in. Primarily used for sfxs
    // 2S2H [Custom Audio] Allow for new soundfonts. Was originally permanentEntries[32], sized in AudioLoad_Init
    /* 0x2D90 */ AudioCacheEntry* permanentEntries; // indificual entries to the permanent pool
    /*        */ s32 permanentEntriesCapacity;
    /* 0x3690 */ AudioSampleCache persistentSampleCache; // Stores individual samples persistently
    /* 0x40A4 */ AudioSampleCache temporarySampleCache; // Stores individual samples temporarily
    /* 0x4338 */ AudioSessionPoolSplit sessionPoolSplit; // splits session pool into the cache pool and misc pool
    /* 0x4348 */ AudioCachePoolSplit cachePoolSplit; // splits cache pool into the persistent & temporary common pools
    /* 0x4350 */ AudioCommonPoolSplit persistentCommonPoolSplit; // splits persistent common pool into caches for sequences, soundFonts, sample banks
    /* 0x435C */ AudioCommonPoolSplit temporaryCommonPoolSplit; // splits temporary common pool into caches for sequences, soundFonts, sample banks
    /* 0x4368 */ u8 sampleFontLoadStatus[0x30];
    /* 0x4398 */ u8* fontLoadStatus; // 2S2H [Port] [Custom Audio] Allow for new soundfonts. Was originally fontLoadStatus[0x30]
    /* 0x43C8 */ u8* seqLoadStatus;
    /* 0x4448 */ volatile u8 resetStatus;
    /* 0x4449 */ u8 specId;
    /* 0x444C */ s32 audioResetFadeOutFramesLeft;
    /* 0x4450 */ f32* adsrDecayTable; // A table on the audio heap that stores decay rates used for ADSR
    /* 0x4454 */ u8* audioHeap;
    /* 0x4458 */ size_t audioHeapSize;
    /* 0x445C */ Note* notes;
    /* 0x4460 */ SequencePlayer seqPlayers[5];
    /* 0x4B40 */ SequenceLayer sequenceLayers[80];
    /* 0x7840 */ SequenceChannel sequenceChannelNone;
    /* 0x7924 */ s32 sampleStateOffset; // Start of the list of sample states for this update. Resets after each audio frame.
    /* 0x7928 */ AudioListItem layerFreeList;
    /* 0x7938 */ NotePool noteFreeLists;
    /* 0x7978 */ u8 threadCmdWritePos;
    /* 0x7979 */ u8 threadCmdReadPos;
    /* 0x797A */ u8 threadCmdQueueFinished;
    /* 0x797C */ u16 threadCmdChannelMask[5]; // bit-packed for 16 channels. When processing an audio thread channel command on all channels, only process channels with their bit set.
    /* 0x7988 */ OSMesgQueue* audioResetQueueP;
    /* 0x798C */ OSMesgQueue* taskStartQueueP;
    /* 0x7990 */ OSMesgQueue* threadCmdProcQueueP;
    /* 0x7994 */ OSMesgQueue taskStartQueue;
    /* 0x79AC */ OSMesgQueue threadCmdProcQueue;
    /* 0x79C4 */ OSMesgQueue audioResetQueue;
    /* 0x79DC */ OSMesg taskStartMsgs[1];
    /* 0x79E0 */ OSMesg audioResetMesgs[1];
    /* 0x79E4 */ OSMesg threadCmdProcMsgBuf[4];
    /* 0x79F4 */ AudioCmd threadCmdBuf[0x100]; // Audio commands used to transfer audio requests from the graph thread to the audio thread
    /* 0x81F4 */ UNK_TYPE1 unk_81F4[4];
    u16 seqToPlay[5];
    u8 seqReplaced[5];
} AudioContext; // size = 0x81F8

typedef struct {
    /* 0x00 */ u8 targetReverbVol;
    /* 0x01 */ u8 gain; // Increases volume by a multiplicative scaling factor. Represented as a UQ4.4 number
    /* 0x02 */ u8 pan;
    /* 0x03 */ u8 surroundEffectIndex;
    /* 0x04 */ StereoData stereoData;
    /* 0x08 */ f32 frequency;
    /* 0x0C */ f32 velocity;
    /* 0x10 */ char unk_0C[0x4];
    /* 0x14 */ s16* filter;
    /* 0x18 */ u8 combFilterSize;
    /* 0x1A */ u16 combFilterGain;
} NoteSubAttributes; // size = 0x1A

#define MAX_AUTHENTIC_SEQID 128

typedef void (*AudioCustomUpdateFunction)(void);
typedef u32 (*AudioCustomSeqFunction)(s8 value, SequenceChannel* channel);
typedef void* (*AudioCustomReverbFunction)(Sample*, s32, s8, s32);
typedef Acmd* (*AudioCustomSynthFunction)(Acmd*, s32, s32);

typedef struct {
    char* seqData;
    int32_t seqDataSize;
    uint16_t seqNumber;
    uint8_t medium;
    uint8_t cachePolicy;
    int32_t numFonts;
    uint16_t fonts[16];
} SequenceData;

#endif
