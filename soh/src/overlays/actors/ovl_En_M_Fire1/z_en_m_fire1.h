#ifndef Z_EN_M_FIRE1_H
#define Z_EN_M_FIRE1_H

#include "ultra64.h"
#include "global.h"

struct EnMFire1;

typedef struct EnMFire1 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderCylinder collider;
    /* 0x0198 */ f32 timer;
} EnMFire1; // size = 0x019C

#endif
