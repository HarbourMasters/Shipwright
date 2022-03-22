#ifndef Z_EFF_SS_BUBBLE_H
#define Z_EFF_SS_BUBBLE_H

#include "ultra64.h"
#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ f32 yPosOffset;
    /* 0x10 */ f32 yPosRandScale;
    /* 0x14 */ f32 xzPosRandScale;
    /* 0x18 */ f32 scale;
} EffectSsBubbleInitParams; // size = 0x1C

#endif
