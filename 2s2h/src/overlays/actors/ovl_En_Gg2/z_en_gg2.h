#ifndef Z_EN_GG2_H
#define Z_EN_GG2_H

#include "global.h"
#include "objects/object_gg/object_gg.h"

struct EnGg2;

typedef void (*EnGg2ActionFunc)(struct EnGg2*, PlayState*);

#define ENGG2_GET_PATH_INDEX(thisx) (((thisx)->params & 0xFC00) >> 0xA)

#define ENGG2_PATH_INDEX_NONE 0x3F

typedef struct EnGg2 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 unk144[0x4C];
    /* 0x190 */ SkelAnime skelAnime;
    /* 0x1D4 */ EnGg2ActionFunc actionFunc;
    /* 0x1D8 */ Path* path;
    /* 0x1DC */ s32 unk_1DC;
    /* 0x1E0 */ Vec3s headRot;
    /* 0x1E6 */ Vec3s torsoRot;
    /* 0x1EC */ Vec3s jointTable[OBJECT_GG_LIMB_MAX];
    /* 0x264 */ Vec3s morphTable[OBJECT_GG_LIMB_MAX];
    /* 0x2DC */ UNK_TYPE1 unk2DC[0x8];
    /* 0x2E4 */ s16 csId;
    /* 0x2E6 */ UNK_TYPE1 unk2E6[4];
    /* 0x2EA */ s16 unk_2EA;
    /* 0x2EC */ s16 unk_2EC;
    /* 0x2EE */ s16 animIndex;
    /* 0x2F0 */ u8 unk_2F0;
    /* 0x2F1 */ u8 unk_2F1;
    /* 0x2F2 */ s16 unk_2F2;
    /* 0x2F4 */ s16 unk_2F4;
    /* 0x2F6 */ s16 unk_2F6;
    /* 0x2F8 */ UNK_TYPE1 unk2F8[0xC];
    /* 0x304 */ Vec3f unk_304;
} EnGg2; // size = 0x310

#endif // Z_EN_GG2_H
