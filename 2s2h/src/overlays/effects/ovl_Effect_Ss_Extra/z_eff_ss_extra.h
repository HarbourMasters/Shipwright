#ifndef _Z64_EFFECT_SS_EXTRA_H_
#define _Z64_EFFECT_SS_EXTRA_H_

#include "global.h"

#define EXTRA_SCORE_30 30
#define EXTRA_SCORE_60 60
#define EXTRA_SCORE_100 100

typedef enum {
    /* 0 */ EXTRA_SCORE_INDEX_30,
    /* 1 */ EXTRA_SCORE_INDEX_60,
    /* 2 */ EXTRA_SCORE_INDEX_100
} ExtraScoreIndex;

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ s16 scale;
    /* 0x26 */ s16 scoreIndex;
} EffectSsExtraInitParams; // size = 0x28

#endif
