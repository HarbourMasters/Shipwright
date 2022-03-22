#ifndef Z_EFF_SS_EN_ICE_H
#define Z_EFF_SS_EN_ICE_H

#include "ultra64.h"
#include "global.h"

typedef struct {
    /* 0x00 */ Actor* actor;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ f32 scale;
    /* 0x14 */ Vec3f velocity;
    /* 0x20 */ Vec3f accel;
    /* 0x2C */ Color_RGBA8 primColor;
    /* 0x30 */ Color_RGBA8 envColor;
    /* 0x34 */ s32 life;
    /* 0x38 */ s16 type;
} EffectSsEnIceInitParams; // size = 0x3C

#endif
