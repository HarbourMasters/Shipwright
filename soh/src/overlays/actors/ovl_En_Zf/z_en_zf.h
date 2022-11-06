#ifndef Z_EN_ZF_H
#define Z_EN_ZF_H

#include "ultra64.h"
#include "global.h"

struct EnZf;

typedef void (*EnZfActionFunc)(struct EnZf*, PlayState*);

typedef enum {
    /* -2 */ ENZF_TYPE_DINOLFOS = -2,
    /* -1 */ ENZF_TYPE_LIZALFOS_LONE, // Not a miniboss, e.g. Spirit Temple
    /*  0 */ ENZF_TYPE_LIZALFOS_MINIBOSS_A, // Pair with B
    /*  1 */ ENZF_TYPE_LIZALFOS_MINIBOSS_B // Pair with A
} EnZfType;

typedef enum {
    /*  0 */ ENZF_ACTION_DROP_IN,
    /*  3 */ ENZF_ACTION_3 = 3, // stop and choose action?
    /*  5 */ ENZF_ACTION_APPROACH_PLAYER = 5,
    /*  6 */ ENZF_ACTION_6,
    /*  7 */ ENZF_ACTION_7, // more sidestepping?
    /*  8 */ ENZF_ACTION_RECOIL_FROM_BLOCKED_SLASH,
    /*  9 */ ENZF_ACTION_SLASH,
    /* 11 */ ENZF_ACTION_JUMP_BACK = 11,
    /* 12 */ ENZF_ACTION_CIRCLE_AROUND_PLAYER,
    /* 13 */ ENZF_ACTION_JUMP_FORWARD,
    /* 14 */ ENZF_ACTION_STUNNED,
    /* 15 */ ENZF_ACTION_DIE,
    /* 16 */ ENZF_ACTION_DAMAGED,
    /* 18 */ ENZF_ACTION_SHEATHE_SWORD = 18,
    /* 19 */ ENZF_ACTION_HOP_AWAY,
    /* 20 */ ENZF_ACTION_HOP_AND_TAUNT,
    /* 21 */ ENZF_ACTION_DRAW_SWORD,
    /* 22 */ ENZF_ACTION_JUMP_UP
} EnZfAction;

typedef enum {
    /* 0x00 */ ENZF_LIMB_NONE,
    /* 0x01 */ ENZF_LIMB_ROOT,
    /* 0x02 */ ENZF_LIMB_BODY_ROOT,
    /* 0x03 */ ENZF_LIMB_UPPER_BODY_ROOT,
    /* 0x04 */ ENZF_LIMB_NECK_ROOT,
    /* 0x05 */ ENZF_LIMB_HEAD_ROOT,
    /* 0x06 */ ENZF_LIMB_JAW_ROOT_ROOT,
    /* 0x07 */ ENZF_LIMB_JAW_ROOT,
    /* 0x08 */ ENZF_LIMB_JAW,
    /* 0x09 */ ENZF_LIMB_HEAD,
    /* 0x0A */ ENZF_LIMB_NECK,
    /* 0x0B */ ENZF_LIMB_RIGHT_ARM_ROOT,
    /* 0x0C */ ENZF_LIMB_RIGHT_UPPER_ARM_ROOT,
    /* 0x0D */ ENZF_LIMB_RIGHT_FOREARM_ROOT,
    /* 0x0E */ ENZF_LIMB_SWORD_ROOT,
    /* 0x0F */ ENZF_LIMB_SWORD,
    /* 0x10 */ ENZF_LIMB_RIGHT_FOREARM,
    /* 0x11 */ ENZF_LIMB_RIGHT_UPPER_ARM,
    /* 0x12 */ ENZF_LIMB_LEFT_ARM_ROOT,
    /* 0x13 */ ENZF_LIMB_LEFT_UPPER_ARM_ROOT,
    /* 0x14 */ ENZF_LIMB_LEFT_FOREARM_ROOT,
    /* 0x15 */ ENZF_LIMB_LEFT_HAND_ROOT,
    /* 0x16 */ ENZF_LIMB_LEFT_HAND,
    /* 0x17 */ ENZF_LIMB_LEFT_FOREARM,
    /* 0x18 */ ENZF_LIMB_LEFT_UPPER_ARM,
    /* 0x19 */ ENZF_LIMB_CHEST_ARMOR,
    /* 0x1A */ ENZF_LIMB_TAIL_ROOT,
    /* 0x1B */ ENZF_LIMB_TAIL_BASE_ROOT,
    /* 0x1C */ ENZF_LIMB_TAIL_TIP_ROOT,
    /* 0x1D */ ENZF_LIMB_TAIL_TIP,
    /* 0x1E */ ENZF_LIMB_TAIL_BASE,
    /* 0x1F */ ENZF_LIMB_SCABBARD_ROOT_ROOT,
    /* 0x20 */ ENZF_LIMB_SCABBARD_ROOT,
    /* 0x21 */ ENZF_LIMB_SCABBARD,
    /* 0x22 */ ENZF_LIMB_RIGHT_LEG_ROOT,
    /* 0x23 */ ENZF_LIMB_RIGHT_THIGH_ROOT,
    /* 0x24 */ ENZF_LIMB_RIGHT_SHIN_ROOT,
    /* 0x25 */ ENZF_LIMB_RIGHT_FOOT_ROOT,
    /* 0x26 */ ENZF_LIMB_RIGHT_FOOT,
    /* 0x27 */ ENZF_LIMB_RIGHT_SHIN,
    /* 0x28 */ ENZF_LIMB_RIGHT_THIGH,
    /* 0x29 */ ENZF_LIMB_LEFT_LEG_ROOT,
    /* 0x2A */ ENZF_LIMB_LEFT_THIGH_ROOT,
    /* 0x2B */ ENZF_LIMB_LEFT_SHIN_ROOT,
    /* 0x2C */ ENZF_LIMB_LEFT_FOOT_ROOT,
    /* 0x2D */ ENZF_LIMB_LEFT_FOOT,
    /* 0x2E */ ENZF_LIMB_LEFT_SHIN,
    /* 0x2F */ ENZF_LIMB_LEFT_THIGH,
    /* 0x30 */ ENZF_LIMB_TRUNK,
    /* 0x31 */ ENZF_LIMB_MAX
} EnZfLimb; // used for both Lizalfos and Dinalfos

typedef struct EnZf {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[ENZF_LIMB_MAX];
    /* 0x02B6 */ Vec3s morphTable[ENZF_LIMB_MAX];
    /* 0x03DC */ s32 action;
    /* 0x03E0 */ s32 unk_3E0; // Set but unused
    /* 0x03E4 */ s32 hopAnimIndex;
    /* 0x03E8 */ EnZfActionFunc actionFunc;
    /* 0x03EC */ s16 headRot;
    /* 0x03EE */ s16 headRotTemp;
    /* 0x03F0 */ s32 unk_3F0; // attack timer?
    /* 0x03F4 */ s16 unk_3F4; // tag timer?
    /* 0x03F6 */ s16 iceTimer;
    /* 0x03F8 */ s16 unk_3F8; // boolean, to do with movement
    /* 0x03FA */ s16 swordSheathed; // boolean
    /* 0x03FC */ s16 clearFlag;
    /* 0x03FE */ s16 curPlatform;
    /* 0x0400 */ s16 homePlatform; // Platform to return to, changed by some functions?
    /* 0x0402 */ s16 nextPlatform;
    /* 0x0404 */ u8 alpha;
    /* 0x0408 */ f32 unk_408; // related to XZ speeds
    /* 0x040C */ f32 unk_40C; // related to y velocity
    /* 0x0410 */ u8 damageEffect;
    /* 0x0414 */ s32 blureIndex;
    /* 0x0418 */ ColliderCylinder bodyCollider;
    /* 0x0464 */ ColliderQuad swordCollider;
    /* 0x04E4 */ Vec3f rightFootPos;
    /* 0x04F0 */ Vec3f leftFootPos;
    /* 0x04FC */ Vec3f bodyPartsPos[9];
} EnZf; // size = 0x0568

#endif
