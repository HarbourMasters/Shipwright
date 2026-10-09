#ifndef Z_EN_BOM_BOWL_MAN_H
#define Z_EN_BOM_BOWL_MAN_H

#include "global.h"
#include "objects/object_cs/object_cs.h"

struct EnBomBowlMan;

typedef void (*EnBomBowlManActionFunc)(struct EnBomBowlMan*, PlayState*);

#define ENBOMBOWLMAN_GET_F(thisx) ((thisx)->params & 0xF)
#define ENBOMBOWLMAN_GET_F0(thisx) (((thisx)->params >> 4) & 0xF)
#define ENBOMBOWLMAN_GET_PATH_INDEX(thisx) (((thisx)->params >> 8) & 0xFF)

#define ENBOMBOWLMAN_PATH_INDEX_NONE 0x3F

#define ENBOMBOWLMAN_F0_0 0
#define ENBOMBOWLMAN_F0_2 2

typedef struct EnBomBowlMan {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_CS_LIMB_MAX];
    /* 0x206 */ Vec3s morphTable[OBJECT_CS_LIMB_MAX];
    /* 0x284 */ EnBomBowlManActionFunc actionFunc;
    /* 0x288 */ s16 unk_288;
    /* 0x28A */ s16 unk_28A;
    /* 0x28C */ UNK_TYPE1 unk28C[2];
    /* 0x28E */ s16 unk_28E;
    /* 0x290 */ s16 unk_290;
    /* 0x292 */ UNK_TYPE1 unk292[2];
    /* 0x294 */ Path* path;
    /* 0x298 */ s16 unk_298;
    /* 0x29A */ s16 pathIndex;
    /* 0x29C */ s16 unk_29C;
    /* 0x29E */ s16 unk_29E;
    /* 0x2A0 */ Vec3f unk_2A0;
    /* 0x2AC */ UNK_TYPE1 unk2AC[0xC];
    /* 0x2B8 */ s16 unk_2B8;
    /* 0x2BA */ s16 unk_2BA;
    /* 0x2BC */ s16 unk_2BC;
    /* 0x2BE */ UNK_TYPE1 unk2BE[2];
    /* 0x2C0 */ s16 unk_2C0;
    /* 0x2C2 */ s16 unk_2C2;
    /* 0x2C4 */ f32 animEndFrame;
    /* 0x2C8 */ f32 unk_2C8;
    /* 0x2CC */ s16 csIdList[4];
    /* 0x2D4 */ s16 unk_2D4;
    /* 0x2D6 */ s16 csId;
    /* 0x2D8 */ struct EnBomBowlMan* unk_2D8;
    /* 0x2DC */ struct EnBomBowlMan* unk_2DC[5];
    /* 0x2F0 */ s16 unk_2F0;
    /* 0x2F2 */ s16 unk_2F2;
    /* 0x2F4 */ s16 unk_2F4;
    /* 0x2F6 */ s16 unk_2F6;
    /* 0x2F8 */ s32 animIndex;
} EnBomBowlMan; // size = 0x2FC

#endif // Z_EN_BOM_BOWL_MAN_H
