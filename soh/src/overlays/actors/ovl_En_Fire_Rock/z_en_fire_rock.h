#ifndef Z_EN_FIRE_ROCK_H
#define Z_EN_FIRE_ROCK_H

#include "ultra64.h"
#include "global.h"

#include "overlays/actors/ovl_En_Encount2/z_en_encount2.h"

typedef enum {
    /* 0x00 */ FIRE_ROCK_SPAWNED_FALLING1,
    /* 0x01 */ FIRE_ROCK_BROKEN_PIECE1,
    /* 0x02 */ FIRE_ROCK_BROKEN_PIECE2,
    /* 0x03 */ FIRE_ROCK_SPAWNED_FALLING2,
    /* 0x05 */ FIRE_ROCK_CEILING_SPOT_SPAWNER = 5,
    /* 0x06 */ FIRE_ROCK_ON_FLOOR
} EnFireRockType;

struct EnFireRock;

typedef void (*EnFireRockActionFunc)(struct EnFireRock*, PlayState*);

typedef struct EnFireRock {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ Vec3f angularVelocity;
    /* 0x0158 */ Vec3f rockRotation;
    /* 0x0164 */ char unk164[0x4];
    /* 0x0168 */ EnFireRockActionFunc actionFunc;
    /* 0x016C */ f32 scale;
    /* 0x0170 */ Vec3f relativePos;
    /* 0x017C */ Vec3f unk_17C; // set but unused?
    /* 0x0188 */ s16 timer;
    /* 0x018A */ s16 timer2;
    /* 0x018C */ s16 type;
    /* 0x018E */ u8 playerNearby;
    /* 0x0190 */ EnEncount2* spawner;
    /* 0x0194 */ ColliderCylinder collider;
} EnFireRock; // size = 0x01E0

#endif
