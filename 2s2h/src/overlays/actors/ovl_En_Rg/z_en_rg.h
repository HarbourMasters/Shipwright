#ifndef Z_EN_RG_H
#define Z_EN_RG_H

#include "global.h"
#include "objects/object_oF1d_map/object_oF1d_map.h"

struct EnRg;

typedef void (*EnRgActionFunc)(struct EnRg*, PlayState*);

#define ENRG_GET_PATH_INDEX(thisx) ((((thisx)->params & 0x7F80) >> 7) & 0xFF)

#define ENRG_PATH_INDEX_NONE 0xFF

#define ENRG_FIDGET_TABLE_LEN 3

typedef struct {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ UNK_TYPE1 unk_03[0xD];
    /* 0x10 */ Vec3f unk_10;
    /* 0x1C */ Vec3f unk_1C;
    /* 0x28 */ Vec3f unk_28;
    /* 0x34 */ f32 unk_34;
    /* 0x38 */ f32 unk_38;
} EnRgStruct; // size = 0x3C

typedef struct EnRg {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnRgActionFunc actionFunc;
    /* 0x18C */ Actor* unk_18C;
    /* 0x190 */ ColliderCylinder collider1;
    /* 0x1DC */ ColliderSphere collider2;
    /* 0x234 */ Path* path;
    /* 0x238 */ Vec3s jointTable[GORON_LIMB_MAX];
    /* 0x2A4 */ Vec3s morphTable[GORON_LIMB_MAX];
    /* 0x310 */ u16 unk_310;
    /* 0x314 */ f32 animPlaySpeed;
    /* 0x318 */ s16 unk_318;
    /* 0x31A */ UNK_TYPE1 unk31A[2];
    /* 0x31C */ s16 unk_31C;
    /* 0x31E */ s16 unk_31E;
    /* 0x320 */ s16 unk_320;
    /* 0x322 */ s16 unk_322;
    /* 0x324 */ s16 unk_324;
    /* 0x326 */ s16 unk_326;
    /* 0x328 */ s16 fidgetTableZ[ENRG_FIDGET_TABLE_LEN];
    /* 0x32E */ s16 fidgetTableY[ENRG_FIDGET_TABLE_LEN];
    /* 0x334 */ s32 animIndex;
    /* 0x338 */ UNK_TYPE1 unk338[4];
    /* 0x33C */ s32 unk_33C;
    /* 0x340 */ s32 unk_340;
    /* 0x344 */ s32 unk_344;
    /* 0x348 */ s32 numCheckpointsAheadOfPlayer;
    /* 0x34C */ EnRgStruct unk_34C[32];
} EnRg; // size = 0xACC

#endif // Z_EN_RG_H
