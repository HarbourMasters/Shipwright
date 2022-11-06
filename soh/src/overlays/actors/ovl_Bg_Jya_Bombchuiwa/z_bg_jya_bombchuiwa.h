#ifndef Z_BG_JYA_BOMBCHUIWA_H
#define Z_BG_JYA_BOMBCHUIWA_H

#include "ultra64.h"
#include "global.h"

struct BgJyaBombchuiwa;

typedef void (*BgJyaBombchuiwaActionFunc)(struct BgJyaBombchuiwa*, PlayState*);

typedef struct BgJyaBombchuiwa {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ BgJyaBombchuiwaActionFunc actionFunc;
    /* 0x0150 */ ColliderJntSph collider;
    /* 0x0170 */ ColliderJntSphElement colliderItems;
    /* 0x01B0 */ f32 lightRayIntensity;
    /* 0x01B4 */ s16 timer;
    /* 0x01B6 */ u8 drawFlags; // Used to determine how the actor is drawn.
} BgJyaBombchuiwa; // size = 0x01B8

#endif
