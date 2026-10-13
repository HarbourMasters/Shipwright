#ifndef Z_EN_MUTO_H
#define Z_EN_MUTO_H

#include "global.h"
#include "objects/object_toryo/object_toryo.h"

struct EnMuto;

typedef void (*EnMutoActionFunc)(struct EnMuto*, PlayState*);

typedef struct EnMuto {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_TORYO_LIMB_MAX];
    /* 0x1EE */ Vec3s morphTable[OBJECT_TORYO_LIMB_MAX];
    /* 0x254 */ EnMutoActionFunc actionFunc;
    /* 0x258 */ Vec3s headRot;
    /* 0x25E */ Vec3s headRotTarget;
    /* 0x262 */ Vec3s waistRot;
    /* 0x26A */ Vec3s waistRotTarget;
    /* 0x270 */ s16 unusedCounter;
    /* 0x274 */ f32 animEndFrame;
    /* 0x278 */ s16 isInDialogue;
    /* 0x27A */ s16 textIdIndex;
    /* 0x27C */ s16 yawTowardsTarget;
    /* 0x280 */ s32 isInMayorsRoom;
    /* 0x284 */ s32 shouldSetHeadRotation;
    /* 0x288 */ Actor* targetActor;
    /* 0x28C */ s32 cutsceneState;
    /* 0x290 */ s32 animIndex;
    /* 0x294 */ ColliderCylinder collider;
} EnMuto; // size = 0x2E0

#endif // Z_EN_MUTO_H
