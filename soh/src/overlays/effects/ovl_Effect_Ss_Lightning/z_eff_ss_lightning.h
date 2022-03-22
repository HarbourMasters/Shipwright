#ifndef Z_EFF_SS_LIGHTNING_H
#define Z_EFF_SS_LIGHTNING_H

#include "ultra64.h"
#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Color_RGBA8 primColor;
    /* 0x10 */ Color_RGBA8 envColor;
    /* 0x14 */ s16 scale;
    /* 0x16 */ s16 yaw;
    /* 0x18 */ s16 life;
    /* 0x1A */ s16 numBolts;
} EffectSsLightningInitParams; // size = 0x1C

#endif
