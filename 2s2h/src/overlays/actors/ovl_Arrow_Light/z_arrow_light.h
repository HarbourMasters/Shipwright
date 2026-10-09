#ifndef Z_ARROW_LIGHT_H
#define Z_ARROW_LIGHT_H

#include "global.h"

struct ArrowLight;

typedef void (*ArrowLightActionFunc)(struct ArrowLight*, PlayState*);

typedef struct ArrowLight {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s16 radius;
    /* 0x146 */ u16 timer;
    /* 0x148 */ u8 alpha;
    /* 0x14C */ Vec3f firedPos;
    /* 0x158 */ f32 height;
    /* 0x15C */ f32 screenFillIntensity;
    /* 0x160 */ ArrowLightActionFunc actionFunc;
} ArrowLight; // size = 0x164


#endif // Z_ARROW_LIGHT_H
