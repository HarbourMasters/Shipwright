#ifndef Z_ARROW_ICE_H
#define Z_ARROW_ICE_H

#include "global.h"

struct ArrowIce;

typedef void (*ArrowIceActionFunc)(struct ArrowIce*, PlayState*);

typedef struct ArrowIce {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s16 radius;
    /* 0x146 */ u16 timer;
    /* 0x148 */ u8 alpha;
    /* 0x14C */ Vec3f firedPos;
    /* 0x158 */ f32 height;
    /* 0x15C */ f32 blueingEffectMagnitude;
    /* 0x160 */ ArrowIceActionFunc actionFunc;
} ArrowIce; // size = 0x164

#endif // Z_ARROW_ICE_H
