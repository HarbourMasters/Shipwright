#ifndef Z_BG_KIN2_BOMBWALL_H
#define Z_BG_KIN2_BOMBWALL_H

#define BG_KIN2_BOMBWALL_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)

#include "global.h"

struct BgKin2Bombwall;

typedef void (*BgKin2BombwallActionFunc)(struct BgKin2Bombwall*, PlayState*);

typedef struct BgKin2Bombwall {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ColliderCylinder collider;
    /* 0x1A8 */ BgKin2BombwallActionFunc actionFunc;
    /* 0x1AC */ s8 timer;
} BgKin2Bombwall; // size = 0x1B0

#endif // Z_BG_KIN2_BOMBWALL_H
