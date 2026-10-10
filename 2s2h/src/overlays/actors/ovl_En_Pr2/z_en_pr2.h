#ifndef Z_EN_PR2_H
#define Z_EN_PR2_H

#include "global.h"
#include "objects/object_pr/object_pr.h"

struct EnPr2;

typedef void (*EnPr2ActionFunc)(struct EnPr2*, PlayState*);

#define ENPR2_GET_F(thisx) ((thisx)->params & 0xF)
#define ENPR2_GET_FF0(thisx) (((thisx)->params >> 4) & 0xFF)

#define ENPR2_PATH_INDEX_NONE 0x3F
#define ENPR2_PARAMS(paramF, paramFF0) (((paramF) & 0xF) | (((paramFF0) << 4) & 0xFF0))

typedef struct EnPr2 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_PR_2_LIMB_MAX];
    /* 0x1A6 */ Vec3s morphTable[OBJECT_PR_2_LIMB_MAX];
    /* 0x1C4 */ EnPr2ActionFunc actionFunc;
    /* 0x1C8 */ s16 pathIndex;
    /* 0x1CC */ Path* path;
    /* 0x1D0 */ s32 unk_1D0;
    /* 0x1D4 */ s16 unk_1D4;
    /* 0x1D6 */ s16 unk_1D6;
    /* 0x1D8 */ s16 unk_1D8;
    /* 0x1DA */ s16 unk_1DA;
    /* 0x1DC */ s16 unk_1DC;
    /* 0x1DE */ s16 unk_1DE;
    /* 0x1E0 */ s16 unk_1E0;
    /* 0x1E2 */ UNK_TYPE1 unk1E2[2];
    /* 0x1E4 */ s16 unk_1E4;
    /* 0x1E6 */ s16 unk_1E6;
    /* 0x1E8 */ UNK_TYPE1 unk1E8[4];
    /* 0x1EC */ s16 unk_1EC;
    /* 0x1EE */ s16 unk_1EE;
    /* 0x1F0 */ s16 unk_1F0;
    /* 0x1F2 */ s16 unk_1F2;
    /* 0x1F4 */ s16 unk_1F4;
    /* 0x1F8 */ f32 animEndFrame;
    /* 0x1FC */ f32 unk_1FC;
    /* 0x200 */ f32 unk_200;
    /* 0x204 */ f32 unk_204;
    /* 0x208 */ f32 unk_208;
    /* 0x20C */ f32 unk_20C;
    /* 0x210 */ s32 animIndex;
    /* 0x214 */ UNK_TYPE1 unk214[4];
    /* 0x218 */ s32 unk_218;
    /* 0x21C */ Vec3f unk_21C;
    /* 0x228 */ Vec3f unk_228;
    /* 0x234 */ Vec3f unk_234[5];
    /* 0x270 */ Vec3f unk_270;
    /* 0x27C */ ColliderCylinder collider;
} EnPr2; // size = 0x2C8

#endif // Z_EN_PR2_H
