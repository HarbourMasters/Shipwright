#ifndef Z_EN_IN_H
#define Z_EN_IN_H

#include "global.h"
#include "assets/objects/object_in/object_in.h"

struct EnIn;

typedef void (*EnInActionFunc)(struct EnIn*, PlayState*);

typedef enum {
    /* 0 */ ENIN_UNK_TYPE,
    /* 1 */ ENIN_HORSE_RIDER_YELLOW_SHIRT,
    /* 2 */ ENIN_HORSE_RIDER_BLUE_SHIRT,
    /* 3 */ ENIN_YELLOW_SHIRT,
    /* 4 */ ENIN_BLUE_SHIRT
} EnInType;

#define ENIN_GET_TYPE(thisx) ((thisx)->params & 0x1FF)
#define ENIN_GET_PATH_INDEX(thisx) (((thisx)->params & 0x7E00) >> 9) // Only used with ENIN_UNK_TYPE

#define ENIN_PATH_INDEX_NONE 0x3F

typedef enum EnInAnimation {
    /* -1 */ ENIN_ANIM_NONE = -1,
    /*  0 */ ENIN_ANIM_0,
    /*  1 */ ENIN_ANIM_1,
    /*  2 */ ENIN_ANIM_2,
    /*  3 */ ENIN_ANIM_3,
    /*  4 */ ENIN_ANIM_4,
    /*  5 */ ENIN_ANIM_5,
    /*  6 */ ENIN_ANIM_6,
    /*  7 */ ENIN_ANIM_7,
    /*  8 */ ENIN_ANIM_8,
    /*  9 */ ENIN_ANIM_9,
    /* 10 */ ENIN_ANIM_10,
    /* 11 */ ENIN_ANIM_11,
    /* 12 */ ENIN_ANIM_12,
    /* 13 */ ENIN_ANIM_13,
    /* 14 */ ENIN_ANIM_14,
    /* 15 */ ENIN_ANIM_15,
    /* 16 */ ENIN_ANIM_16,
    /* 17 */ ENIN_ANIM_17,
    /* 18 */ ENIN_ANIM_MAX
} EnInAnimation;

typedef enum EnInAnimation2 {
    /* -1 */ ENIN_ANIM2_NONE = -1,
    /*  0 */ ENIN_ANIM2_0,
    /*  1 */ ENIN_ANIM2_1,
    /*  2 */ ENIN_ANIM2_2,
    /*  3 */ ENIN_ANIM2_3,
    /*  4 */ ENIN_ANIM2_4,
    /*  5 */ ENIN_ANIM2_5,
    /*  6 */ ENIN_ANIM2_6,
    /*  7 */ ENIN_ANIM2_7,
    /*  8 */ ENIN_ANIM2_MAX
} EnInAnimation2;

typedef struct EnIn {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnInActionFunc actionFunc;
    /* 0x148 */ SkelAnime skelAnime;
    /* 0x18C */ ColliderJntSph colliderJntSph;
    /* 0x1AC */ ColliderJntSphElement colliderJntSphElement;
    /* 0x1EC */ ColliderCylinder colliderCylinder;
    /* 0x238 */ UNK_TYPE1 unk238[0x4];
    /* 0x23C */ u8 unk23C;
    /* 0x23D */ u8 unk23D;
    /* 0x240 */ Path* path;
    /* 0x244 */ s16 unk244;
    /* 0x248 */ Vec3f unk248;
    /* 0x254 */ Vec3f unk254;
    /* 0x260 */ u8 unk260;
    /* 0x261 */ u8 unk261;
    /* 0x262 */ Vec3s jointTable[OBJECT_IN_LIMB_MAX];
    /* 0x2DA */ Vec3s morphTable[OBJECT_IN_LIMB_MAX];
    /* 0x352 */ Vec3s trackTarget;
    /* 0x358 */ Vec3s headRot;
    /* 0x35E */ Vec3s torsoRot;
    /* 0x364 */ UNK_TYPE1 unk364[0x12];
    /* 0x376 */ s16 fidgetTableY[OBJECT_IN_LIMB_MAX];
    /* 0x39E */ s16 fidgetTableZ[OBJECT_IN_LIMB_MAX];
    /* 0x3C6 */ UNK_TYPE1 unk3C6[0xBC];
    /* 0x482 */ s16 unk482;
    /* 0x484 */ s16 unk484;
    /* 0x486 */ s16 unk486;
    /* 0x488 */ s16 animIndex2;
    /* 0x48A */ u16 unk48A;
    /* 0x48C */ s32 unk48C;
    /* 0x490 */ UNK_TYPE1 unk490[0x4];
    /* 0x494 */ s32 unk494;
    /* 0x498 */ UNK_TYPE1 unk498[0xC];
    /* 0x4A4 */ struct EnIn* unk4A4;
    /* 0x4A8 */ s32 unk4A8;
    /* 0x4AC */ s32 unk4AC;
    /* 0x4B0 */ s32 unk4B0;
    /* 0x4B4 */ Vec3f unk4B4;
    /* 0x4C0 */ f32 unk4C0;
    /* 0x4C0 */ f32 unk4C4;
    /* 0x4C0 */ s32 prevTalkState;
} EnIn; // size = 0x4CC

#endif // Z_EN_IN_H
