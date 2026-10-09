#ifndef Z_EN_WARP_UZU_H
#define Z_EN_WARP_UZU_H

#include "global.h"

struct EnWarpUzu;

typedef void (*EnWarpUzuActionFunc)(struct EnWarpUzu*, PlayState*);

typedef struct EnWarpUzu {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnWarpUzuActionFunc actionFunc;
    /* 0x148 */ ColliderCylinder collider;
} EnWarpUzu; // size = 0x194

#endif // Z_EN_WARP_UZU_H
