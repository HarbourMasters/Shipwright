#ifndef Z_EN_FD_FIRE_H
#define Z_EN_FD_FIRE_H

#include "ultra64.h"
#include "global.h"

struct EnFdFire;

typedef void (*EnFdFireActionFunc)(struct EnFdFire* this, PlayState* play);

typedef struct EnFdFire {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnFdFireActionFunc actionFunc;
    /* 0x0150 */ ColliderCylinder collider;
    /* 0x019C */ f32 spawnRadius;
    /* 0x01A0 */ f32 scale;
    /* 0x01A4 */ char unk_1A4[0x2];
    /* 0x01A6 */ s16 deathTimer;
    /* 0x01A8 */ s16 tile2Y;
} EnFdFire; // size = 0x01AC

#endif
