#ifndef Z_EN_TG_H
#define Z_EN_TG_H

#include "global.h"
#include "objects/object_mu/object_mu.h"

struct EnTg;

typedef void (*EnTgActionFunc)(struct EnTg*, PlayState*);

typedef struct EnTgHeartEffect {
    /* 0x00 */ u8 isEnabled;
    /* 0x01 */ u8 timer;
    /* 0x04 */ f32 scale;
    /* 0x08 */ UNK_TYPE1 unk8[0xC];
    /* 0x14 */ Vec3f pos;
    /* 0x20 */ Vec3f unusedZeroVec;
    /* 0x2C */ Vec3f velocity;
    /* 0x38 */ s16 angle;
} EnTgHeartEffect; // size = 0x3C

typedef struct EnTg {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnTgActionFunc actionFunc;
    /* 0x148 */ SkelAnime skelAnime;
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ UNK_TYPE1 unk1D8[0x18];
    /* 0x1F0 */ Vec3s jointTable[HONEY_AND_DARLING_LIMB_MAX];
    /* 0x26E */ Vec3s morphTable[HONEY_AND_DARLING_LIMB_MAX];
    /* 0x2EC */ s16 spawnHeartTimer;
    /* 0x2F0 */ EnTgHeartEffect effects[10];
} EnTg; // size = 0x548

#endif // Z_EN_TG_H
