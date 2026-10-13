#ifndef Z_EN_PR_H
#define Z_EN_PR_H

#include "global.h"
#include "objects/object_pr/object_pr.h"

struct EnPr;

typedef void (*EnPrActionFunc)(struct EnPr*, PlayState*);

#define ENPR_GET_FF00(thisx) (((thisx)->params >> 8) & 0xFF)

#define ENPR_FF00_MAX 20
#define ENPR_FF00_MIN 0

typedef enum EnPrBodyPart {
    /*  0 */ ENPR_BODYPART_0,
    /*  1 */ ENPR_BODYPART_1,
    /*  2 */ ENPR_BODYPART_2,
    /*  3 */ ENPR_BODYPART_3,
    /*  4 */ ENPR_BODYPART_4,
    /*  5 */ ENPR_BODYPART_5,
    /*  6 */ ENPR_BODYPART_6,
    /*  7 */ ENPR_BODYPART_7,
    /*  8 */ ENPR_BODYPART_8,
    /*  9 */ ENPR_BODYPART_9,
    /* 10 */ ENPR_BODYPART_MAX
} EnPrBodyPart;

typedef struct EnPr {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_PR_1_LIMB_MAX];
    /* 0x1C4 */ Vec3s morphTable[OBJECT_PR_1_LIMB_MAX];
    /* 0x200 */ EnPrActionFunc actionFunc;
    /* 0x204 */ u8 unk_204;
    /* 0x206 */ s16 unk_206;
    /* 0x208 */ s16 unk_208;
    /* 0x20A */ s16 unk_20A;
    /* 0x20C */ s16 unk_20C;
    /* 0x20E */ s16 unk_20E;
    /* 0x210 */ s16 unk_210;
    /* 0x212 */ s16 unk_212;
    /* 0x214 */ s16 unk_214;
    /* 0x216 */ s16 unk_216;
    /* 0x218 */ UNK_TYPE1 unk218[4];
    /* 0x21C */ s32 animIndex;
    /* 0x220 */ UNK_TYPE1 unk220[0x8];
    /* 0x228 */ s32 bodyPartsCount;
    /* 0x22C */ s16 unk_22C;
    /* 0x22E */ s16 drawDmgEffAlpha;
    /* 0x230 */ s16 drawDmgEffType;
    /* 0x234 */ f32 unk_234;
    /* 0x238 */ f32 unk_238;
    /* 0x23C */ Vec3f bodyPartsPos[ENPR_BODYPART_MAX];
    /* 0x2B4 */ f32 unk_2B4;
    /* 0x2B8 */ f32 unk_2B8;
    /* 0x2BC */ f32 animEndFrame;
    /* 0x2C0 */ f32 unk_2C0;
    /* 0x2C4 */ f32 unk_2C4;
    /* 0x2C8 */ f32 unk_2C8;
    /* 0x2CC */ f32 unk_2CC;
    /* 0x2D0 */ s16 unk_2D0;
    /* 0x2D2 */ s16 unk_2D2;
    /* 0x2D4 */ Vec3f unk_2D4;
    /* 0x2E0 */ Vec3f unk_2E0;
    /* 0x2EC */ UNK_TYPE1 unk2EC[0xC];
    /* 0x2F8 */ Actor* unk_2F8;
    /* 0x2FC */ ColliderCylinder collider;
} EnPr; // size = 0x348

#endif // Z_EN_PR_H
