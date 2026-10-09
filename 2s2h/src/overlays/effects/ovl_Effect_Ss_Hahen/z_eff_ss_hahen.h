#ifndef _Z64_EFFECT_SS_HAHEN_H_
#define _Z64_EFFECT_SS_HAHEN_H_

#include "global.h"

#define HAHEN_OBJECT_DEFAULT -1

#define HAHEN_SMALL (1 << 0)
#define HAHEN_XLU (1 << 1)

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ Gfx* dList;
    /* 0x28 */ s16 flags;
    /* 0x2A */ s16 scale;
    /* 0x2C */ s16 objectId;
    /* 0x2C */ s16 life;
} EffectSsHahenInitParams; // size = 0x30

#endif
