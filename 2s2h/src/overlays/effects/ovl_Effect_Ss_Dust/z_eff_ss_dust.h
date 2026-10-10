#ifndef _Z64_EFFECT_SS_DUST_H_
#define _Z64_EFFECT_SS_DUST_H_

#include "global.h"

#define DUST_DRAWFLAG0 (0)
#define DUST_DRAWFLAG1 (1 << 0)
#define DUST_DRAWFLAG2 (1 << 1)
#define DUST_DRAWFLAG_RAND_COLOR_OFFSET (1 << 2)

typedef enum {
    /* 0 */ DUST_UPDATE_NORMAL,
    /* 1 */ DUST_UPDATE_FIRE
} DustUpdateMode;

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ Color_RGBA8 primColor;
    /* 0x28 */ Color_RGBA8 envColor;
    /* 0x2C */ s16 scale;
    /* 0x2E */ s16 scaleStep;
    /* 0x30 */ s16 life;
    /* 0x32 */ u16 drawFlags;
    /* 0x34 */ u8 updateMode;
} EffectSsDustInitParams; // size = 0x38

#endif
