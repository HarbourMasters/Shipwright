#ifndef Z_EN_WF_H
#define Z_EN_WF_H

#include "ultra64.h"
#include "global.h"

struct EnWf;

typedef void (*EnWfActionFunc)(struct EnWf*, PlayState*);

typedef enum {
    /*  0 */ WOLFOS_LIMB_NONE,
    /*  1 */ WOLFOS_LIMB_ROOT,
    /*  2 */ WOLFOS_LIMB_BACK_LEFT_THIGH,
    /*  3 */ WOLFOS_LIMB_BACK_LEFT_SHIN,
    /*  4 */ WOLFOS_LIMB_BACK_LEFT_PASTERN,
    /*  5 */ WOLFOS_LIMB_BACK_LEFT_PAW,
    /*  6 */ WOLFOS_LIMB_TAIL,
    /*  7 */ WOLFOS_LIMB_ABDOMEN,
    /*  8 */ WOLFOS_LIMB_BACK_RIGHT_THIGH,
    /*  9 */ WOLFOS_LIMB_BACK_RIGHT_SHIN,
    /* 10 */ WOLFOS_LIMB_BACK_RIGHT_PASTERN,
    /* 11 */ WOLFOS_LIMB_BACK_RIGHT_PAW,
    /* 12 */ WOLFOS_LIMB_THORAX,
    /* 13 */ WOLFOS_LIMB_FRONT_RIGHT_UPPER_LEG,
    /* 14 */ WOLFOS_LIMB_FRONT_RIGHT_LOWER_LEG,
    /* 15 */ WOLFOS_LIMB_FRONT_RIGHT_CLAW,
    /* 16 */ WOLFOS_LIMB_HEAD_ROOT,
    /* 17 */ WOLFOS_LIMB_HEAD,
    /* 18 */ WOLFOS_LIMB_EYES,
    /* 19 */ WOLFOS_LIMB_FRONT_LEFT_UPPER_LEG,
    /* 20 */ WOLFOS_LIMB_FRONT_LEFT_LOWER_LEG,
    /* 21 */ WOLFOS_LIMB_FRONT_LEFT_CLAW,
    /* 22 */ WOLFOS_LIMB_MAX
} EnWfLimb;

typedef enum {
    /*  0 */ WOLFOS_ACTION_WAIT_TO_APPEAR,
    /*  2 */ WOLFOS_ACTION_DIE = 2,
    /*  3 */ WOLFOS_ACTION_DAMAGED,
    /*  4 */ WOLFOS_ACTION_TURN_TOWARDS_PLAYER,
    /*  5 */ WOLFOS_ACTION_BACKFLIP_AWAY,
    /*  6 */ WOLFOS_ACTION_WAIT,
    /*  7 */ WOLFOS_ACTION_BLOCKING,
    /*  8 */ WOLFOS_ACTION_SLASH,
    /*  9 */ WOLFOS_ACTION_RUN_AT_PLAYER,
    /* 10 */ WOLFOS_ACTION_SEARCH_FOR_PLAYER,
    /* 11 */ WOLFOS_ACTION_RUN_AROUND_PLAYER,
    /* 12 */ WOLFOS_ACTION_RECOIL_FROM_BLOCKED_SLASH,
    /* 14 */ WOLFOS_ACTION_SIDESTEP = 14,
    /* 15 */ WOLFOS_ACTION_STUNNED
} EnWfAction;

typedef struct EnWf {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ Vec3s bodyPartsPos[10];
    /* 0x0188 */ SkelAnime skelAnime;
    /* 0x01CC */ Vec3s jointTable[WOLFOS_LIMB_MAX];
    /* 0x0250 */ Vec3s morphTable[WOLFOS_LIMB_MAX];
    /* 0x02D4 */ s32 action; // Used instead of checking the actionFunc directly (but also in range comparisons)
    /* 0x02D8 */ char unk_2D8[4]; // Unused
    /* 0x02DC */ EnWfActionFunc actionFunc;
    /* 0x02E0 */ s16 unk_2E0; // Used, but has no effect
    /* 0x02E2 */ s16 unk_2E2;
    /* 0x02E4 */ s16 fireTimer;
    /* 0x02E6 */ u8 damageEffect;
    /* 0x02E8 */ s32 actionTimer; // Used to make an action last for a certain amount of time
    /* 0x02EC */ f32 runSpeed;
    /* 0x02F0 */ char unk_2F0[4];
    /* 0x02F4 */ f32 unk_2F4; // Set and not used
    /* 0x02F8 */ s16 slashStatus; // Whether to slash again or not, and whether to cry
    /* 0x02FA */ s16 unk_2FA; // Set and not used
    /* 0x02FC */ s16 switchFlag;
    /* 0x02FE */ s16 runAngle;
    /* 0x0300 */ s16 unk_300; // Set, but ineffectual (see comment in Draw)
    /* 0x0302 */ u8 eyeIndex;
    /* 0x0304 */ ColliderJntSph colliderSpheres;
    /* 0x0324 */ ColliderJntSphElement colliderSpheresElements[4];
    /* 0x0424 */ ColliderCylinder colliderCylinderBody;
    /* 0x0470 */ ColliderCylinder colliderCylinderTail;
    /* 0x04BC */ Vec3f unk_4BC;
    /* 0x04C8 */ Vec3f unk_4C8;
    /* 0x04D4 */ Vec3s unk_4D4;
} EnWf; // size = 0x04DC

typedef enum {
    /* 0 */ WOLFOS_NORMAL,
    /* 1 */ WOLFOS_WHITE
} EnWfType;

#endif
