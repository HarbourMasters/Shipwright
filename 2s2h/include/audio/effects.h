#ifndef AUDIO_EFFECTS_H
#define AUDIO_EFFECTS_H

#include "PR/ultratypes.h"
#include "unk.h"

struct Note;
struct SequencePlayer;

/* Multi-Point ADSR Envelope (Attack, Decay, Sustain, Release) */

typedef enum AdsrStatus {
    /* 0 */ ADSR_STATUS_DISABLED,
    /* 1 */ ADSR_STATUS_INITIAL,
    /* 2 */ ADSR_STATUS_START_LOOP,
    /* 3 */ ADSR_STATUS_LOOP,
    /* 4 */ ADSR_STATUS_FADE,
    /* 5 */ ADSR_STATUS_HANG,
    /* 6 */ ADSR_STATUS_DECAY,
    /* 7 */ ADSR_STATUS_RELEASE,
    /* 8 */ ADSR_STATUS_SUSTAIN
} AdsrStatus;

// Special commands for `delay` in `EnvelopePoint`
// Any value above 0 is treated as a delay
#define ADSR_DISABLE 0
#define ADSR_HANG -1
#define ADSR_GOTO -2
#define ADSR_RESTART -3

typedef struct EnvelopePoint {
    /* 0x0 */ s16 delay;
    /* 0x2 */ s16 arg;
} EnvelopePoint; // size = 0x4

typedef struct AdsrSettings {
    /* 0x0 */ u8 decayIndex; // index used to obtain adsr decay rate from adsrDecayTable
    /* 0x1 */ u8 sustain;
    /* 0x4 */ EnvelopePoint* envelope;
} AdsrSettings; // size = 0x8

typedef struct AdsrState {
    union {
        struct {
            /* 0x00 */ u8 unused : 1;
            /* 0x00 */ u8 hang : 1;
            /* 0x00 */ u8 decay : 1;
            /* 0x00 */ u8 release : 1;
            /* 0x00 */ u8 status : 4;
        } s;
        /* 0x00 */ u8 asByte;
    } action;
    /* 0x01 */ u8 envelopeIndex;
    /* 0x02 */ s16 delay;
    /* 0x04 */ f32 sustain;
    /* 0x08 */ f32 velocity;
    /* 0x0C */ f32 fadeOutVel;
    /* 0x10 */ f32 current;
    /* 0x14 */ f32 target;
    /* 0x18 */ UNK_TYPE1 pad18[4];
    /* 0x1C */ EnvelopePoint* envelope;
} AdsrState; // size = 0x20


/* Vibrato */

typedef struct VibratoSubStruct {
    /* 0x0 */ u16 vibratoRateStart;
    /* 0x2 */ u16 vibratoDepthStart;
    /* 0x4 */ u16 vibratoRateTarget;
    /* 0x6 */ u16 vibratoDepthTarget;
    /* 0x8 */ u16 vibratoRateChangeDelay;
    /* 0xA */ u16 vibratoDepthChangeDelay;
    /* 0xC */ u16 vibratoDelay;
} VibratoSubStruct; // size = 0xE

typedef struct VibratoState {
    /* 0x00 */ VibratoSubStruct* vibSubStruct; // Something else?
    /* 0x04 */ u32 time; // 0x400 is 1 unit of time, 0x10000 is 1 period
    /* 0x08 */ s16* curve;
    /* 0x0C */ f32 depth;
    /* 0x10 */ f32 rate;
    /* 0x14 */ u8 active;
    /* 0x16 */ u16 rateChangeTimer;
    /* 0x18 */ u16 depthChangeTimer;
    /* 0x1A */ u16 delay;
} VibratoState; // size = 0x1C


/* Portamento */

typedef enum PortamentoMode {
    /* 0 */ PORTAMENTO_MODE_OFF,
    /* 1 */ PORTAMENTO_MODE_1,
    /* 2 */ PORTAMENTO_MODE_2,
    /* 3 */ PORTAMENTO_MODE_3,
    /* 4 */ PORTAMENTO_MODE_4,
    /* 5 */ PORTAMENTO_MODE_5
} PortamentoMode;

#define PORTAMENTO_IS_SPECIAL(x) ((x).mode & 0x80)
#define PORTAMENTO_MODE(x) ((x).mode & ~0x80)

// Pitch sliding by up to one octave in the positive direction. Negative
// direction is "supported" by setting extent to be negative. The code
// extrapolates exponentially in the wrong direction in that case, but that
// doesn't prevent seqplayer from doing it, AFAICT.
typedef struct Portamento {
    /* 0x0 */ u8 mode; // bit 0x80 denotes something; the rest are an index 0-5
    /* 0x2 */ u16 cur;
    /* 0x4 */ u16 speed;
    /* 0x8 */ f32 extent;
} Portamento; // size = 0xC

void AudioScript_SequencePlayerProcessSound(struct SequencePlayer* seqPlayer);

void AudioEffects_InitAdsr(AdsrState* adsr, EnvelopePoint* envelope, s16* volOut);
void AudioEffects_InitVibrato(struct Note* note);
void AudioEffects_InitPortamento(struct Note* note);

f32 AudioEffects_UpdateAdsr(AdsrState* adsr);
void AudioEffects_UpdatePortamentoAndVibrato(struct Note* note);

#endif
