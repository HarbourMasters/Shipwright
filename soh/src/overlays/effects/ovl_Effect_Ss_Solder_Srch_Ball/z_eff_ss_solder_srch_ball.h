#ifndef Z_EFF_SS_SOLDERSRCHBALL_H
#define Z_EFF_SS_SOLDERSRCHBALL_H

#include "ultra64.h"
#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ s16 unused;
    /* 0x28 */ s16* linkDetected;
} EffectSsSolderSrchBallInitParams;

#endif
