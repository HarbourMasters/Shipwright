#ifndef Z_EN_BEE_H
#define Z_EN_BEE_H

#include "global.h"
#include "assets/objects/object_bee/object_bee.h"

struct EnBee;

typedef void (*EnBeeActionFunc)(struct EnBee*, PlayState*);

typedef enum {
    /* 0 */ BEE_BEHAVIOR_IDLE,
    /* 1 */ BEE_BEHAVIOR_ATTACK
} BeeBehaviorType;

typedef struct EnBee {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_BEE_LIMB_MAX];
    /* 0x1C4 */ Vec3s morphTable[OBJECT_BEE_LIMB_MAX];
    /* 0x200 */ EnBeeActionFunc actionFunc;
    /* 0x204 */ s16 attackDelayTimer;
    /* 0x206 */ s16 sfxHitTimer;
    /* 0x208 */ s16 isHostile;  // set and not used
    /* 0x20C */ s32 targetYaw;
    /* 0x210 */ s32 flightHoverOffset;
    /* 0x214 */ s32 posIndex;
    /* 0x218 */ s32 instanceId;
    /* 0x21C */ Vec3f targetPos[2];
    /* 0x234 */ UNK_TYPE1 pad234[8];
    /* 0x23C */ ColliderCylinder collider;
} EnBee; // size = 0x288

#endif // Z_EN_BEE_H
