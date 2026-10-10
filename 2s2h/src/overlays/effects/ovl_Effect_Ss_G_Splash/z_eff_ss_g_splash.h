#ifndef _Z64_EFFECT_SS_G_SPLASH_H_
#define _Z64_EFFECT_SS_G_SPLASH_H_

#include "global.h"

typedef enum EffSsGSplashType {
    /* 0 */ EFFSSGSPLASH_TYPE_0,
    /* 1 */ EFFSSGSPLASH_TYPE_1,
    /* 2 */ EFFSSGSPLASH_TYPE_2
} EffSsGSplashType;

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ u8 type;
    /* 0x0D */ u8 customColor;
    /* 0x0E */ s16 scale;
    /* 0x10 */ Color_RGBA8 primColor;
    /* 0x14 */ Color_RGBA8 envColor;
} EffectSsGSplashInitParams; // size = 0x18

#endif
