#ifndef Z_EFF_SS_G_MAGMA2_H
#define Z_EFF_SS_G_MAGMA2_H

#include "ultra64.h"
#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Color_RGBA8 primColor;
    /* 0x10 */ Color_RGBA8 envColor;
    /* 0x14 */ s16 updateRate;
    /* 0x16 */ s16 drawMode;
    /* 0x18 */ s16 scale;
} EffectSsGMagma2InitParams; // size = 0x1C

#endif
