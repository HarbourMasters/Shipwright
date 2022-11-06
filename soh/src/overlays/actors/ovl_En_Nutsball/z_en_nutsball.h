#ifndef Z_EN_NUTSBALL_H
#define Z_EN_NUTSBALL_H

#include "ultra64.h"
#include "global.h"

struct EnNutsball;

typedef void (*EnNutsballActionFunc)(struct EnNutsball*, PlayState*);

typedef struct EnNutsball {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnNutsballActionFunc actionFunc;
    /* 0x0150 */ s8 objBankIndex;
    /* 0x0152 */ s16 timer;
    /* 0x0154 */ ColliderCylinder collider;
} EnNutsball; // size = 0x01A0

#endif
