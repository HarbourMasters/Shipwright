#ifndef Z_EN_M_FIRE1_H
#define Z_EN_M_FIRE1_H

#include "global.h"

struct EnMFire1;

typedef struct EnMFire1 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ f32 timer;
} EnMFire1; // size = 0x194

#endif // Z_EN_M_FIRE1_H
