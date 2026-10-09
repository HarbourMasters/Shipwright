#ifndef Z_BG_SPOUT_FIRE_H
#define Z_BG_SPOUT_FIRE_H

#include "global.h"

struct BgSpoutFire;

typedef void (*BgSpoutFireActionFunc)(struct BgSpoutFire*, PlayState*);

typedef struct BgSpoutFire {
    /* 0x000 */ Actor actor;
    /* 0x144 */ BgSpoutFireActionFunc actionFunc;
    /* 0x148 */ s16 timer;
    /* 0x14A */ s16 flameTexIndex;
    /* 0x14C */ ColliderCylinder collider;
} BgSpoutFire; // size = 0x198

#endif // Z_BG_SPOUT_FIRE_H
