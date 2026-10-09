#ifndef _Z64_EFFECT_SS_D_FIRE_H_
#define _Z64_EFFECT_SS_D_FIRE_H_

#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ s16 scale;
    /* 0x26 */ s16 scaleStep;
    /* 0x28 */ s16 alpha;
    /* 0x2A */ s16 alphaStep;
    /* 0x2C */ s16 fadeDelay;
    /* 0x30 */ s32 life;
} EffectSsDFireInitParams; // size = 0x34

#endif
