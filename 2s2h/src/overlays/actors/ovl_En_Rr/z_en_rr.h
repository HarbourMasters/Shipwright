#ifndef Z_EN_RR_H
#define Z_EN_RR_H

#include "global.h"

struct EnRr;

typedef void (*EnRrActionFunc)(struct EnRr*, PlayState*);

typedef enum {
    /* 0 */ LIKE_LIKE_PARAM_0,
    /* 1 */ LIKE_LIKE_PARAM_1,
    /* 2 */ LIKE_LIKE_PARAM_2,
    /* 3 */ LIKE_LIKE_PARAM_3
} LikeLikeParam;

typedef enum LikeLikeBodyPart {
    /*  0 */ LIKE_LIKE_BODYPART_0,
    /*  1 */ LIKE_LIKE_BODYPART_1,
    /*  2 */ LIKE_LIKE_BODYPART_2,
    /*  3 */ LIKE_LIKE_BODYPART_3,
    /*  4 */ LIKE_LIKE_BODYPART_4,
    /*  5 */ LIKE_LIKE_BODYPART_5,
    /*  6 */ LIKE_LIKE_BODYPART_6,
    /*  7 */ LIKE_LIKE_BODYPART_7,
    /*  8 */ LIKE_LIKE_BODYPART_8,
    /*  9 */ LIKE_LIKE_BODYPART_9,
    /* 10 */ LIKE_LIKE_BODYPART_10,
    /* 11 */ LIKE_LIKE_BODYPART_11,
    /* 12 */ LIKE_LIKE_BODYPART_12,
    /* 13 */ LIKE_LIKE_BODYPART_13,
    /* 14 */ LIKE_LIKE_BODYPART_14,
    /* 15 */ LIKE_LIKE_BODYPART_15,
    /* 16 */ LIKE_LIKE_BODYPART_16,
    /* 17 */ LIKE_LIKE_BODYPART_17,
    /* 18 */ LIKE_LIKE_BODYPART_18,
    /* 19 */ LIKE_LIKE_BODYPART_19,
    /* 20 */ LIKE_LIKE_BODYPART_MAX
} LikeLikeBodyPart;

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ s16 unk_14;
    /* 0x16 */ s16 unk_16;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ Vec3s unk_1A;
} EnRrStruct; // size = 0x20

typedef struct EnRr {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnRrActionFunc actionFunc;
    /* 0x148 */ ColliderCylinder collider1;
    /* 0x194 */ ColliderCylinder collider2;
    /* 0x1E0 */ u8 drawDmgEffType;
    /* 0x1E1 */ u8 unk_1E1;
    /* 0x1E2 */ u8 unk_1E2;
    /* 0x1E4 */ s16 unk_1E4;
    /* 0x1E6 */ s16 unk_1E6;
    /* 0x1E8 */ s16 unk_1E8;
    /* 0x1EA */ s16 unk_1EA;
    /* 0x1EC */ s16 unk_1EC;
    /* 0x1EE */ s16 unk_1EE;
    /* 0x1F0 */ s16 unk_1F0;
    /* 0x1F2 */ s16 unk_1F2;
    /* 0x1F4 */ s16 unk_1F4;
    /* 0x1F6 */ s16 unk_1F6;
    /* 0x1F8 */ s16 unk_1F8;
    /* 0x1FA */ s16 unk_1FA;
    /* 0x1FC */ s16 unk_1FC;
    /* 0x200 */ f32 unk_200;
    /* 0x204 */ f32 unk_204;
    /* 0x208 */ f32 unk_208;
    /* 0x20C */ f32 unk_20C;
    /* 0x210 */ f32 unk_210;
    /* 0x214 */ f32 unk_214;
    /* 0x218 */ f32 unk_218;
    /* 0x21C */ f32 drawDmgEffAlpha;
    /* 0x220 */ f32 drawDmgEffScale;
    /* 0x224 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x228 */ Vec3f unk_228;
    /* 0x234 */ Vec3f bodyPartsPos[LIKE_LIKE_BODYPART_MAX];
    /* 0x324 */ EnRrStruct unk_324[5];
} EnRr; // size = 0x3C4

#endif // Z_EN_RR_H
