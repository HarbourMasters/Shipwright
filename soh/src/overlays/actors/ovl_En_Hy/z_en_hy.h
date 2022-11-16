#ifndef Z_EN_HY_H
#define Z_EN_HY_H

#include "ultra64.h"
#include "global.h"

typedef enum {
    /*  0 */ ENHY_TYPE_AOB,
    /*  1 */ ENHY_TYPE_COB,
    /*  2 */ ENHY_TYPE_AHG_2,
    /*  3 */ ENHY_TYPE_BOJ_3,
    /*  4 */ ENHY_TYPE_AHG_4,
    /*  5 */ ENHY_TYPE_BOJ_5,
    /*  6 */ ENHY_TYPE_BBA,
    /*  7 */ ENHY_TYPE_BJI_7,
    /*  8 */ ENHY_TYPE_CNE_8,
    /*  9 */ ENHY_TYPE_BOJ_9,
    /* 10 */ ENHY_TYPE_BOJ_10,
    /* 11 */ ENHY_TYPE_CNE_11,
    /* 12 */ ENHY_TYPE_BOJ_12,
    /* 13 */ ENHY_TYPE_AHG_13,
    /* 14 */ ENHY_TYPE_BOJ_14,
    /* 15 */ ENHY_TYPE_BJI_15,
    /* 16 */ ENHY_TYPE_BOJ_16,
    /* 17 */ ENHY_TYPE_AHG_17,
    /* 18 */ ENHY_TYPE_BOB_18,
    /* 19 */ ENHY_TYPE_BJI_19,
    /* 20 */ ENHY_TYPE_AHG_20,
    /* 21 */ ENHY_TYPE_MAX
} EnHyType;

struct EnHy;

typedef void (*EnHyActionFunc)(struct EnHy*, PlayState*);

typedef struct EnHy {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnHyActionFunc actionFunc;
    /* 0x0194 */ char unk_194; // unused
    /* 0x0195 */ u8 pathReverse;
    /* 0x0196 */ s8 objBankIndexHead;
    /* 0x0197 */ s8 objBankIndexSkel2; // 7 < limb < 15 (upper part?) (always same as objBankIndexSkel1)
    /* 0x0198 */ s8 objBankIndexSkel1; // sets the object used when drawing the skeleton for limb <= 7 (lower part?)
    /* 0x0199 */ s8 objBankIndexOsAnime;
    /* 0x019C */ ColliderCylinder collider;
    /* 0x01E8 */ struct_80034A14_arg1 unk_1E8;
    /* 0x0210 */ Path* path;
    /* 0x0214 */ s8 waypoint;
    /* 0x0215 */ s8 unk_215;
    /* 0x0216 */ char unk_216[2]; // unused
    /* 0x0218 */ s16 curEyeIndex;
    /* 0x021A */ s16 nextEyeIndexTimer;
    /* 0x021C */ s16 unk_21C[16]; // bodyWiggleY ?
    /* 0x023C */ s16 unk_23C[16]; // bodyWiggleZ ?
    /* 0x025C */ f32 unkRange;
    /* 0x0260 */ s32 unkGetItemId;
    /* 0x0264 */ Vec3f modelOffset;
    /* 0x0270 */ Vec3s jointTable[16];
    /* 0x02D0 */ Vec3s morphTable[16];
    /* 0x0330 */ u16 unk_330;
    /*        */ GetItemEntry getItemEntry;
} EnHy; // size = 0x0334

#endif
