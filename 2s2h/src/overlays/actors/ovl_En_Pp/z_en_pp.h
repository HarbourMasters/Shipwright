#ifndef Z_EN_PP_H
#define Z_EN_PP_H

#include "global.h"
#include "objects/object_pp/object_pp.h"

struct EnPp;

typedef void (*EnPpActionFunc)(struct EnPp*, PlayState*);

#define EN_PP_GET_TYPE(thisx) ((thisx)->params)
#define EN_PP_DOES_CHARGE_IN_STRAIGHT_LINES(thisx) (((thisx)->params >> 8) & 1)
#define EN_PP_GET_ATTACK_RANGE(thisx) ((thisx)->world.rot.z)

typedef enum {
    /*  0 */ EN_PP_TYPE_MASKED,
    /*  1 */ EN_PP_TYPE_NO_MASK,
    /*  2 */ EN_PP_TYPE_UNMASKED,
    /*  7 */ EN_PP_TYPE_FRAGMENT_BASE = 7,
    /*  8 */ EN_PP_TYPE_FRAGMENT_BODY = HIPLOOP_LIMB_BODY + EN_PP_TYPE_FRAGMENT_BASE,
    /* 10 */ EN_PP_TYPE_FRAGMENT_FRONT_LEFT_LOWER_LEG = HIPLOOP_LIMB_FRONT_LEFT_LOWER_LEG + EN_PP_TYPE_FRAGMENT_BASE,
    /* 13 */ EN_PP_TYPE_FRAGMENT_FRONT_RIGHT_LOWER_LEG = HIPLOOP_LIMB_FRONT_RIGHT_LOWER_LEG + EN_PP_TYPE_FRAGMENT_BASE,
    /* 17 */ EN_PP_TYPE_FRAGMENT_LEFT_WING_MIDDLE = HIPLOOP_LIMB_LEFT_WING_MIDDLE + EN_PP_TYPE_FRAGMENT_BASE,
    /* 23 */ EN_PP_TYPE_FRAGMENT_RIGHT_WING_MIDDLE = HIPLOOP_LIMB_RIGHT_WING_MIDDLE + EN_PP_TYPE_FRAGMENT_BASE,
    /* 26 */ EN_PP_TYPE_FRAGMENT_CENTER_WING_MIDDLE = HIPLOOP_LIMB_CENTER_WING_MIDDLE + EN_PP_TYPE_FRAGMENT_BASE
} EnPpType;

typedef enum EnPpBodyPart {
    /*  0 */ EN_PP_BODYPART_BODY,
    /*  1 */ EN_PP_BODYPART_FRONT_LEFT_LOWER_LEG,
    /*  2 */ EN_PP_BODYPART_FRONT_LEFT_UPPER_LEG,
    /*  3 */ EN_PP_BODYPART_LEFT_WING_MIDDLE,
    /*  4 */ EN_PP_BODYPART_BACK_RIGHT_LOWER_LEG,
    /*  5 */ EN_PP_BODYPART_RIGHT_WING_MIDDLE,
    /*  6 */ EN_PP_BODYPART_CENTER_WING_BASE,
    /*  7 */ EN_PP_BODYPART_CENTER_WING_MIDDLE,
    /*  8 */ EN_PP_BODYPART_BACK_LEFT_LOWER_LEG,
    /*  9 */ EN_PP_BODYPART_RIGHT_EYE,
    /* 10 */ EN_PP_BODYPART_LEFT_EYE,
    /* 11 */ EN_PP_BODYPART_MAX
} EnPpBodyPart;

typedef struct EnPp {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[HIPLOOP_LIMB_MAX];
    /* 0x224 */ Vec3s morphTable[HIPLOOP_LIMB_MAX];
    /* 0x2C0 */ EnPpActionFunc actionFunc;
    /* 0x2C4 */ s16 chargesInStraightLines; // If false, the Hiploop will instead charge directly at the player.
    /* 0x2C6 */ s16 fragmentIndex;
    /* 0x2C8 */ s16 action;
    /* 0x2CA */ s16 timer;
    /* 0x2CC */ s16 secondaryTimer;
    /* 0x2C8 */ s16 hornColliderOn;
    /* 0x2D0 */ s16 targetRotY;
    /* 0x2D2 */ union {
                    s16 isCharging;
                    s16 hasDoneFirstRoar;
                    s16 hasLandedFromJump;
                    s16 maskDetachState;
                } actionVar;
    /* 0x2D4 */ s16 maskBounceAngularVelocity; // Controls the speed that the Hiploop rotates to face the player when an attack bounces off the mask.
    /* 0x2D6 */ s16 deadBodyPartsSpawnedCount;
    /* 0x2D8 */ f32 animEndFrame;
    /* 0x2DC */ f32 chargeAndBounceSpeed;
    /* 0x2DC */ Vec3f ledgeCheckPos[3];
    /* 0x304 */ Vec3f wallCheckPos;
    /* 0x310 */ Vec3f backFootPos[2];
    /* 0x328 */ Vec3f maskFlamesBasePos;
    /* 0x334 */ Vec3f maskPos;
    /* 0x340 */ Vec3f maskRot;
    /* 0x34C */ Vec3f maskVelocity;
    /* 0x358 */ Vec3f targetPos;
    /* 0x364 */ Vec3f fragmentPos;
    /* 0x370 */ s32 fragmentFlameCount;
    /* 0x374 */ Vec3f fragmentFlamesPos[10];
    /* 0x3EC */ Vec3s fragmentAngularVelocity;
    /* 0x3F2 */ s16 drawDmgEffTimer;
    /* 0x3F4 */ s16 drawDmgEffType;
    /* 0x3F8 */ f32 drawDmgEffScale;
    /* 0x3FC */ f32 drawDmgEffFrozenSteamScale;
    /* 0x400 */ f32 attackRange;
    /* 0x404 */ s32 hasBeenDamaged;
    /* 0x408 */ Vec3f bodyPartsPos[EN_PP_BODYPART_MAX];
    /* 0x48C */ s16 bodyPartIndex;
    /* 0x490 */ f32 maskAccelY;
    /* 0x494 */ ColliderJntSph maskCollider;
    /* 0x4B4 */ ColliderJntSphElement maskColliderElements[1];
    /* 0x4F4 */ ColliderJntSph bodyCollider;
    /* 0x514 */ ColliderJntSphElement bodyColliderElements[1];
    /* 0x554 */ ColliderQuad hornCollider;
    /* 0x5D4 */ CollisionPoly* floorPolyForCircleShadow;
    /* 0x5D8 */ s32 blureIndex;
    /* 0x5DC */ s32 animIndex;
    /* 0x5DC */ Vec3f damagedVelocity;
} EnPp; // size = 0x5EC

#endif // Z_EN_PP_H
