#ifndef Z_EFF_SS_KIRAKIRA_H
#define Z_EFF_SS_KIRAKIRA_H

#include "ultra64.h"
#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ Color_RGBA8 primColor;
    /* 0x28 */ Color_RGBA8 envColor;
    /* 0x2C */ s16 alphaStep;
    /* 0x2E */ s16 scale;
    /* 0x30 */ s32 life;
    /* 0x34 */ s16 rotSpeed;
    /* 0x36 */ s16 yaw;
    /* 0x38 */ u8 updateMode;
} EffectSsKiraKiraInitParams; // size = 0x3C

#endif
