#ifndef Z_EFF_SS_BOMB2_H
#define Z_EFF_SS_BOMB2_H

#include "ultra64.h"
#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ s16 scale;
    /* 0x26 */ s16 scaleStep;
    /* 0x28 */ u8 drawMode;
} EffectSsBomb2InitParams; // size = 0x30

#endif
