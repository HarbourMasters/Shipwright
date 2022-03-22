#ifndef Z_EFF_SS_DEAD_DB_H
#define Z_EFF_SS_DEAD_DB_H

#include "ultra64.h"
#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ s16 scale;
    /* 0x26 */ s16 scaleStep;
    /* 0x28 */ Color_RGBA8 primColor;
    /* 0x2C */ Color_RGBA8 envColor;
    /* 0x30 */ s16 unused;
    /* 0x34 */ s32 unk_34;
    /* 0x38 */ s16 playSound;
} EffectSsDeadDbInitParams; // size = 0x3C

#endif
