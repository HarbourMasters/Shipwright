#ifndef _Z64_EFFECT_SS_EN_FIRE_H_
#define _Z64_EFFECT_SS_EN_FIRE_H_

#include "global.h"

#define ENFIRE_FLAGS_BODYPART_POS_VEC3S (1 << 15)
#define ENFIRE_PARAM_USE_SCALE (1 << 15)

typedef struct {
    /* 0x00 */ Actor* actor;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ s16 scale;
    /* 0x12 */ s16 params;
    /* 0x14 */ s16 flags;
    /* 0x16 */ s16 bodyPart;
} EffectSsEnFireInitParams; // size = 0x18

#endif
