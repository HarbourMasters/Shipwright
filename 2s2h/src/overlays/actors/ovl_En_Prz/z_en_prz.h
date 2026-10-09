#ifndef Z_EN_PRZ_H
#define Z_EN_PRZ_H

#include "global.h"
#include "objects/object_pr/object_pr.h"

struct EnPrz;

typedef void (*EnPrzActionFunc)(struct EnPrz*, PlayState*);

#define ENPRZ_GET(thisx) ((thisx)->params & 0xFF)

typedef struct EnPrz {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_PR_2_LIMB_MAX];
    /* 0x1A6 */ Vec3s morphTable[OBJECT_PR_2_LIMB_MAX];
    /* 0x1C4 */ EnPrzActionFunc actionFunc;
    /* 0x1C8 */ u8 unk_1C8;
    /* 0x1CC */ Vec3f unk_1CC;
    /* 0x1D8 */ Vec3f unk_1D8;
    /* 0x1E4 */ s16 unk_1E4;
    /* 0x1E6 */ s16 unk_1E6;
    /* 0x1E8 */ s16 unk_1E8;
    /* 0x1EA */ s16 unk_1EA;
    /* 0x1EC */ s16 unk_1EC;
    /* 0x1EE */ s16 unk_1EE;
    /* 0x1F0 */ UNK_TYPE1 unk1F0[2];
    /* 0x1F2 */ s16 unk_1F2;
    /* 0x1F4 */ UNK_TYPE1 unk1F4[8];
    /* 0x1FC */ s16 unk_1FC;
    /* 0x1FE */ s16 unk_1FE;
    /* 0x200 */ UNK_TYPE1 unk200[2];
    /* 0x202 */ s16 unk_202;
    /* 0x204 */ s32 animIndex;
    /* 0x208 */ f32 unk_208;
    /* 0x20C */ f32 unk_20C;
    /* 0x210 */ f32 unk_210;
    /* 0x214 */ f32 unk_214;
    /* 0x218 */ f32 unk_218;
    /* 0x21C */ UNK_TYPE1 unk21C[4];
    /* 0x220 */ Actor* unk_220;
    /* 0x224 */ ColliderCylinder collider;
} EnPrz; // size = 0x270

#endif // Z_EN_PRZ_H
