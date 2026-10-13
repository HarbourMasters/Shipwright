#ifndef Z_EN_BOMBERS_H
#define Z_EN_BOMBERS_H

#include "global.h"
#include "objects/object_cs/object_cs.h"

struct EnBombers;

typedef void (*EnBombersActionFunc)(struct EnBombers*, PlayState*);

#define ENBOMBERS_GET_F(thisx) ((thisx)->params & 0xF)
#define ENBOMBERS_GET_F0(thisx) (((thisx)->params >> 4) & 0xF)

typedef enum {
    /* 0 */ ENBOMBERS_F_0,
    /* 1 */ ENBOMBERS_F_1,
    /* 2 */ ENBOMBERS_F_2,
    /* 3 */ ENBOMBERS_F_3,
    /* 4 */ ENBOMBERS_F_4
} EnBombersParam;

#define ENBOMBERS_F0_0 0

typedef struct EnBombers {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_CS_LIMB_MAX];
    /* 0x206 */ Vec3s morphTable[OBJECT_CS_LIMB_MAX];
    /* 0x284 */ EnBombersActionFunc actionFunc;
    /* 0x288 */ s16 unk_288;
    /* 0x28A */ s16 unk_28A;
    /* 0x28C */ UNK_TYPE1 unk28C[2];
    /* 0x28E */ s16 unk_28E;
    /* 0x290 */ s16 unk_290;
    /* 0x294 */ Vec3f unk_294;
    /* 0x2A0 */ s16 unk_2A0;
    /* 0x2A2 */ UNK_TYPE1 unk2A2[2];
    /* 0x2A4 */ s16 unk_2A4;
    /* 0x2A6 */ s16 unk_2A6;
    /* 0x2A8 */ s16 unk_2A8;
    /* 0x2AA */ s16 unk_2AA;
    /* 0x2AC */ s16 unk_2AC;
    /* 0x2B0 */ f32 animEndFrame;
    /* 0x2B4 */ f32 unk_2B4;
    /* 0x2B8 */ s16 unk_2B8;
    /* 0x2BA */ s16 unk_2BA;
    /* 0x2BC */ s16 unk_2BC;
    /* 0x2BE */ s16 unk_2BE;
    /* 0x2C0 */ s16 unk_2C0;
    /* 0x2C4 */ s32 animIndex;
    /* 0x2C8 */ ColliderCylinder collider;
} EnBombers; // size = 0x314

#endif // Z_EN_BOMBERS_H
