#ifndef Z_EN_VALI_H
#define Z_EN_VALI_H

#include "ultra64.h"
#include "global.h"

struct EnVali;

typedef void (*EnValiActionFunc)(struct EnVali*, PlayState*);

typedef enum {
    /* 0x00 */ EN_VALI_LIMB_NONE,
    /* 0x01 */ EN_VALI_LIMB_NUCLEUS_BASE,
    /* 0x02 */ EN_VALI_LIMB_NUCLEUS,
    /* 0x03 */ EN_VALI_LIMB_RIGHT_MANDIBLE_BASE_BASE,
    /* 0x04 */ EN_VALI_LIMB_RIGHT_MANDIBLE_BASE,
    /* 0x05 */ EN_VALI_LIMB_RIGHT_MANDIBLE,
    /* 0x06 */ EN_VALI_LIMB_LEFT_MANDIBLE_BASE_BASE,
    /* 0x07 */ EN_VALI_LIMB_LEFT_MANDIBLE_BASE,
    /* 0x08 */ EN_VALI_LIMB_LEFT_MANDIBLE,
    /* 0x09 */ EN_VALI_LIMB_LEFT_ARM_BASE,
    /* 0x0A */ EN_VALI_LIMB_LEFT_UPPER_ARM_BASE,
    /* 0x0B */ EN_VALI_LIMB_LEFT_FOREARM_BASE,
    /* 0x0C */ EN_VALI_LIMB_LEFT_CLAW_BODY_BASE,
    /* 0x0D */ EN_VALI_LIMB_LEFT_CLAW_TIP_BASE,
    /* 0x0E */ EN_VALI_LIMB_LEFT_CLAW_TIP,
    /* 0x0F */ EN_VALI_LIMB_LEFT_CLAW_BODY,
    /* 0x10 */ EN_VALI_LIMB_LEFT_FOREARM,
    /* 0x11 */ EN_VALI_LIMB_LEFT_UPPER_ARM,
    /* 0x12 */ EN_VALI_LIMB_RIGHT_ARM_BASE,
    /* 0x13 */ EN_VALI_LIMB_RIGHT_UPPER_ARM_BASE,
    /* 0x14 */ EN_VALI_LIMB_RIGHT_FOREARM_BASE,
    /* 0x15 */ EN_VALI_LIMB_RIGHT_CLAW_BODY_BASE,
    /* 0x16 */ EN_VALI_LIMB_RIGHT_CLAW_TIP_BASE,
    /* 0x17 */ EN_VALI_LIMB_RIGHT_CLAW_TIP,
    /* 0x18 */ EN_VALI_LIMB_RIGHT_CLAW_BODY,
    /* 0x19 */ EN_VALI_LIMB_RIGHT_FOREARM,
    /* 0x1A */ EN_VALI_LIMB_RIGHT_UPPER_ARM,
    /* 0x1B */ EN_VALI_LIMB_INNER_HOOD,
    /* 0x1C */ EN_VALI_LIMB_OUTER_HOOD,
    /* 0x1D */ EN_VALI_LIMB_MAX
} EnValiLimb;

typedef struct EnVali {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnValiActionFunc actionFunc;
    /* 0x0194 */ u8 lightningTimer;
    /* 0x0195 */ u8 slingshotReactionTimer;
    /* 0x0196 */ s16 timer;
    /* 0x0198 */ Vec3s jointTable[EN_VALI_LIMB_MAX];
    /* 0x0246 */ Vec3s morphTable[EN_VALI_LIMB_MAX];
    /* 0x02F4 */ f32 armScale;
    /* 0x02F8 */ f32 floatHomeHeight; // Used as a centre for floating when visible (home is used for the lurk height)
    /* 0x02FC */ ColliderQuad leftArmCollider;
    /* 0x037C */ ColliderQuad rightArmCollider;
    /* 0x03FC */ ColliderCylinder bodyCollider;
} EnVali; // size = 0x0448

typedef enum {
    /* 0 */ BARI_TYPE_NORMAL,
    /* 1 */ BARI_TYPE_SWORD_DAMAGE
} EnValiType;

#endif
