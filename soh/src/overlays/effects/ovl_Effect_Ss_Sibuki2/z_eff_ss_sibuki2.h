#ifndef Z_EFF_SS_SIBUKI2_H
#define Z_EFF_SS_SIBUKI2_H

#include <libultraship/libultra.h>
#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ s16 scale;
} EffectSsSibuki2InitParams; // size = 0x28

#endif
