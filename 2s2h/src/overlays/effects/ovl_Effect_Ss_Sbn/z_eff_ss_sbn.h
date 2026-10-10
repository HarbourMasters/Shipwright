#ifndef _Z64_EFFECT_SS_SBN_H_
#define _Z64_EFFECT_SS_SBN_H_

#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ CollisionPoly* colPoly;
    /* 0x10 */ f32 scale;
} EffectSsSbnInitParams; // size = 0x14

#endif
