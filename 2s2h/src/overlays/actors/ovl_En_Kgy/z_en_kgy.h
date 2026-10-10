#ifndef Z_EN_KGY_H
#define Z_EN_KGY_H

#include "global.h"
#include "objects/object_kgy/object_kgy.h"
#include "overlays/actors/ovl_En_Kbt/z_en_kbt.h"
#include "overlays/actors/ovl_Obj_Ice_Poly/z_obj_ice_poly.h"

struct EnKgy;

typedef void (*EnKgyActionFunc)(struct EnKgy*, PlayState*);

#define ENKGY_GET_1F(thisx) ((thisx)->params & 0x1F)
#define ENKGY_GET_SWITCH_FLAG(thisx) (((thisx)->params & 0xFE00) >> 9)

typedef struct EnKgy {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_KGY_LIMB_MAX];
    /* 0x212 */ Vec3s morphTable[OBJECT_KGY_LIMB_MAX];
    /* 0x29C */ u16 unk_29C;
    /* 0x2A0 */ EnKbt* zubora;
    /* 0x2A4 */ ObjIcePoly* iceBlock;
    /* 0x2A8 */ Vec3f unk_2A8;
    /* 0x2B4 */ Vec3f unk_2B4;
    /* 0x2C0 */ Vec3f unk_2C0;
    /* 0x2CC */ Vec3s headRot;
    /* 0x2D2 */ s16 animIndex;
    /* 0x2D4 */ s16 csIdList[6];
    /* 0x2E0 */ s16 csIdIndex;
    /* 0x2E2 */ s16 animIndex2;
    /* 0x2E4 */ s16 unk_2E4;
    /* 0x2E6 */ s16 unk_2E6;
    /* 0x2E8 */ s16 unk_2E8;
    /* 0x2EA */ s16 getItemId;
    /* 0x2EC */ LightInfo lightInfo;
    /* 0x2FC */ LightNode* lightNode;
    /* 0x300 */ s16 unk_300;
    /* 0x304 */ EnKgyActionFunc actionFunc;
} EnKgy; // size = 0x308

#endif // Z_EN_KGY_H
