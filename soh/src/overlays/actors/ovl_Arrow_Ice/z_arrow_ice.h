#ifndef Z_ARROW_ICE_H
#define Z_ARROW_ICE_H

#include "ultra64.h"
#include "global.h"

struct ArrowIce;

typedef void (*ArrowIceActionFunc)(struct ArrowIce*, PlayState*);

typedef struct ArrowIce {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s16 radius;
    /* 0x014E */ u16 timer;
    /* 0x0150 */ u8 alpha;
    /* 0x0154 */ Vec3f unkPos;
    /* 0x0160 */ f32 unk_160;
    /* 0x0164 */ f32 unk_164;
    /* 0x0168 */ ArrowIceActionFunc actionFunc;
} ArrowIce; // size = 0x016C

#endif
