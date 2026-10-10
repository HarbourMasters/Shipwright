#ifndef Z_EN_JGAME_TSN_H
#define Z_EN_JGAME_TSN_H

#include "global.h"
#include "objects/object_tsn/object_tsn.h"

struct EnJgameTsn;

typedef void (*EnJgameTsnActionFunc)(struct EnJgameTsn*, PlayState*);

#define ENJGAMETSN_GET_PATH_INDEX(thisx) ((thisx)->params & 0xFF)

typedef struct {
    /* 0x00 */ Vec3s* points;
    /* 0x04 */ s32 count;
} EnJgameTsnStruct; // size = 0x8

typedef struct EnJgameTsn {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ SkelAnime skelAnime;
    /* 0x1D4 */ EnJgameTsnActionFunc actionFunc;
    /* 0x1D8 */ EnJgameTsnStruct unk_1D8[4];
    /* 0x1F8 */ EnJgameTsnStruct unk_1F8;
    /* 0x200 */ EnJgameTsnStruct unk_200;
    /* 0x208 */ u8* unk_208[4];
    /* 0x218 */ s32 unk_218;
    /* 0x21C */ s32 unk_21C;
    /* 0x220 */ Vec3s jointTable[OBJECT_TSN_LIMB_MAX];
    /* 0x286 */ Vec3s morphTable[OBJECT_TSN_LIMB_MAX];
    /* 0x2EC */ Vec3s headRot;
    /* 0x2F2 */ Vec3s torsoRot;
    /* 0x2F8 */ s16 unk_2F8;
    /* 0x2FA */ s16 unk_2FA;
    /* 0x2FC */ s16 unk_2FC;
    /* 0x2FE */ s16 unk_2FE;
    /* 0x300 */ u16 unk_300;
} EnJgameTsn; // size = 0x304

#endif // Z_EN_JGAME_TSN_H
