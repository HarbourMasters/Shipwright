#ifndef _Z64_EFFECT_SS_DT_BUBBLE_H_
#define _Z64_EFFECT_SS_DT_BUBBLE_H_

#include "global.h"

typedef enum {
    /* 0 */ DTBUBBLE_COLOR_PROFILE_RED,
    /* 1 */ DTBUBBLE_COLOR_PROFILE_GREEN,
    /* 2 */ DTBUBBLE_COLOR_PROFILE_BLUE,
    /* 3 */ DTBUBBLE_COLOR_PROFILE_CLEAR
} DtBubbleColorProfile;

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ Color_RGBA8 primColor;
    /* 0x28 */ Color_RGBA8 envColor;
    /* 0x2C */ s16 scale;
    /* 0x2E */ s16 life;
    /* 0x30 */ s16 colorProfile;
    /* 0x32 */ s16 randXZ; // randomly moves in the xz plane if true
    /* 0x34 */ u8 customColor;
} EffectSsDtBubbleInitParams; // size = 0x3C

#endif
