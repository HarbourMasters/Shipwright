#ifndef Z_EFF_SS_G_SPLASH_H
#define Z_EFF_SS_G_SPLASH_H

#include "ultra64.h"
#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ u8 type;
    /* 0x0D */ u8 customColor;
    /* 0x0E */ s16 scale;
    /* 0x10 */ Color_RGBA8 primColor;
    /* 0x14 */ Color_RGBA8 envColor;
} EffectSsGSplashInitParams; // size = 0x18

#endif
