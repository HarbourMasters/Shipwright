#ifndef Z_EFF_SS_G_RIPPLE_H
#define Z_EFF_SS_G_RIPPLE_H

#include "ultra64.h"
#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16 radius;
    /* 0x0E */ s16 radiusMax;
    /* 0x10 */ s16 life;
} EffectSsGRippleInitParams; // size = 0x14

#endif
