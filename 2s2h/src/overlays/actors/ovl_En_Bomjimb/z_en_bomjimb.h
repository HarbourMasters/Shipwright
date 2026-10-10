#ifndef Z_EN_BOMJIMB_H
#define Z_EN_BOMJIMB_H

#include "global.h"
#include "objects/object_cs/object_cs.h"

struct EnBomjimb;

typedef void (*EnBomjimbActionFunc)(struct EnBomjimb*, PlayState*);

#define ENBOMJIMB_GET_F(thisx) ((thisx)->params & 0xF)
#define ENBOMJIMB_GET_F0(thisx) (((thisx)->params >> 4) & 0xF)
#define ENBOMJIMB_GET_FF00(thisx) (((thisx)->params >> 8) & 0xFF)

typedef enum {
    /* 0 */ ENBOMJIMB_F_0,
    /* 1 */ ENBOMJIMB_F_1,
    /* 2 */ ENBOMJIMB_F_2,
    /* 3 */ ENBOMJIMB_F_3,
    /* 4 */ ENBOMJIMB_F_4
} EnBomjimbParamF;

typedef enum {
    /* 0 */ ENBOMJIMB_F0_0,
    /* 1 */ ENBOMJIMB_F0_1,
    /* 2 */ ENBOMJIMB_F0_2
} EnBomjimbParamF0;

typedef struct EnBomjimb {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_CS_LIMB_MAX];
    /* 0x206 */ Vec3s morphTable[OBJECT_CS_LIMB_MAX];
    /* 0x284 */ EnBomjimbActionFunc actionFunc;
    /* 0x288 */ s16 unk_288;
    /* 0x28A */ s16 unk_28A;
    /* 0x28C */ UNK_TYPE1 unk28C[4];
    /* 0x290 */ s16 unk_290;
    /* 0x294 */ Vec3f unk_294;
    /* 0x2A0 */ Vec3f unk_2A0;
    /* 0x2AC */ s16 unk_2AC;
    /* 0x2AE */ s16 unk_2AE;
    /* 0x2B0 */ s16 unk_2B0;
    /* 0x2B2 */ s16 unk_2B2;
    /* 0x2B4 */ f32 unk_2B4;
    /* 0x2B8 */ f32 animEndFrame;
    /* 0x2BC */ UNK_TYPE1 unk2BC[4];
    /* 0x2C0 */ s16 unk_2C0;
    /* 0x2C2 */ s16 unk_2C2;
    /* 0x2C4 */ s16 unk_2C4;
    /* 0x2C6 */ s16 unk_2C6;
    /* 0x2C8 */ s16 unk_2C8;
    /* 0x2CA */ s16 unk_2CA;
    /* 0x2CC */ s16 unk_2CC;
    /* 0x2D0 */ s32 unk_2D0;
    /* 0x2D4 */ s16 unk_2D4;
    /* 0x2D6 */ s16 unk_2D6;
    /* 0x2D8 */ UNK_TYPE1 unk2D8[4];
    /* 0x2DC */ s32 animIndex;
    /* 0x2E0 */ u8 unk_2E0;
    /* 0x2E4 */ Actor* unk_2E4;
    /* 0x2E8 */ ColliderCylinder collider;
} EnBomjimb; // size = 0x334

#endif // Z_EN_BOMJIMB_H
