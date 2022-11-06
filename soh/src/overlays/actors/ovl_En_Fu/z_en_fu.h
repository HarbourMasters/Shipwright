#ifndef Z_EN_FU_H
#define Z_EN_FU_H

#include "ultra64.h"
#include "global.h"

struct EnFu;

typedef void (*EnFuActionFunc)(struct EnFu*, PlayState*);

typedef enum {
    /* 0x00 */ FU_LIMB_ROOT,
    /* 0x01 */ FU_LIMB_TORSO,
    /* 0x02 */ FU_LIMB_LEFT_THIGH,
    /* 0x03 */ FU_LIMB_LEFT_LEG,
    /* 0x04 */ FU_LIMB_LEFT_FOOT,
    /* 0x05 */ FU_LIMB_RIGHT_THIGH,
    /* 0x06 */ FU_LIMB_RIGHT_LEG,
    /* 0x07 */ FU_LIMB_RIGHT_FOOT,
    /* 0x08 */ FU_LIMB_CHEST_MUSIC_BOX,
    /* 0x09 */ FU_LIMB_LEFT_ARM_MUSIC_BOX,
    /* 0x0A */ FU_LIMB_UNK,
    /* 0x0B */ FU_LIMB_RIGHT_SHOULDER,
    /* 0x0C */ FU_LIMB_RIGHT_ARM,
    /* 0x0D */ FU_LIMB_RIGHT_HAND_AND_CRANK,
    /* 0x0E */ FU_LIMB_HEAD,
    /* 0x0F */ FU_LIMB_HORN,
    /* 0x10 */ FU_LIMB_MAX
} EnFuLimb;

typedef struct EnFu {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderCylinder collider;
    /* 0x0198 */ SkelAnime skelanime;
    /* 0x01DC */ Vec3s jointTable[FU_LIMB_MAX];
    /* 0x023C */ Vec3s morphTable[FU_LIMB_MAX];
    /* 0x029C */ Vec3s lookAngleOffset; // offset applied to neck rotation to look at link when he is close
    /* 0x02A2 */ Vec3s unk_2A2; 
    /* 0x02A8 */ u16 behaviorFlags;
    /* 0x02AA */ u16 facialExpression;
    /* 0x02AC */ EnFuActionFunc actionFunc;
} EnFu; // size = 0x02B0

#endif
