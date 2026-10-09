#ifndef Z_OBJ_DORA_H
#define Z_OBJ_DORA_H

#include "global.h"

struct ObjDora;

typedef void (*ObjDoraActionFunc)(struct ObjDora*, PlayState*);

typedef struct ObjDora {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderTris colliderTris;
    /* 0x164 */ ColliderTrisElement colliderTrisElements[6];
    /* 0x38C */ ObjDoraActionFunc actionFunc;
    /* 0x390 */ Vec3f gongForce;
    /* 0x39C */ Vec2s gongAngle;
    /* 0x3A0 */ s16 lastGongHitType;
    /* 0x3A2 */ Vec2s gongRotation; // gongAngle * gongForce. Used in ObjDora_Draw.
    /* 0x3A6 */ s16 rupeeDropTimer;
    /* 0x3A8 */ s16 collisionCooldownTimer;
    /* 0x3AA */ s16 unk3AA; // Set but not used
} ObjDora; // size = 0x3AC

#endif // Z_OBJ_DORA_H
