#ifndef Z_EN_NUTSBALL_H
#define Z_EN_NUTSBALL_H

#include "global.h"

struct EnNutsball;

typedef struct EnNutsball {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s16 timer;
    /* 0x146 */ s16 timerThreshold;
    /* 0x148 */ ColliderCylinder collider;
} EnNutsball; // size = 0x194

#endif // Z_EN_NUTSBALL_H
