#ifndef _Z64_EFFECT_SS_DEAD_DD_H_
#define _Z64_EFFECT_SS_DEAD_DD_H_

#include "global.h"

typedef enum {
    /* 0 */ DEADDD_TYPE_0,
    /* 1 */ DEADDD_TYPE_1 // Unused
} DeadDDType;

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ s16 scale;
    /* 0x26 */ s16 scaleStep;
    /* 0x28 */ Color_RGBA8 primColor;
    /* 0x2C */ Color_RGBA8 envColor;
    /* 0x30 */ s16 alphaStep;
    /* 0x34 */ s32 life;
    /* 0x38 */ f32 randPosScale;
    /* 0x3C */ s32 randIter;
    /* 0x40 */ u8 type;
} EffectSsDeadDdInitParams; // size = 0x44

#endif
