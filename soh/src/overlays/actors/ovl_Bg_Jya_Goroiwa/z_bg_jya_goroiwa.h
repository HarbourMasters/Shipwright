#ifndef Z_BG_JYA_GOROIWA_H
#define Z_BG_JYA_GOROIWA_H

#include "ultra64.h"
#include "global.h"

struct BgJyaGoroiwa;

typedef void (*BgJyaGoroiwaFunc)(struct BgJyaGoroiwa*, PlayState*);

typedef struct BgJyaGoroiwa {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ BgJyaGoroiwaFunc actionFunc;
    /* 0x0150 */ ColliderJntSph collider;
    /* 0x0170 */ ColliderJntSphElement colliderItem;
    /* 0x01B0 */ f32 speedFactor;
    /* 0x01B4 */ s16 hasHit;
    /* 0x01B6 */ s16 waitTimer;
    /* 0x01B8 */ f32 yOffsetSpeed;
} BgJyaGoroiwa; // size = 0x01BC

#endif
