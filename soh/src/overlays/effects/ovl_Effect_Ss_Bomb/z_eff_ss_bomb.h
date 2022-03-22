#ifndef Z_EFF_SS_BOMB_H
#define Z_EFF_SS_BOMB_H

#include "ultra64.h"
#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
} EffectSsBombInitParams; // size = 0x24

#endif
