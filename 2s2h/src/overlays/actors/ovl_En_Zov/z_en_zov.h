#ifndef Z_EN_ZOV_H
#define Z_EN_ZOV_H

#include "global.h"
#include "z64snap.h"
#include "objects/object_zov/object_zov.h"

struct EnZov;

typedef void (*EnZovActionFunc)(struct EnZov*, PlayState*);

#define ENZOV_GET_F(thisx) ((thisx)->params & 0xF)
#define ENZOV_GET_FE00(thisx) (((thisx)->params & 0xFE00) >> 9)

#define ENZOV_F_1 1
#define ENZOV_F_2 2

typedef struct EnZov {
    /* 0x000 */ PictoActor picto;
    /* 0x148 */ Vec3s jointTable[LULU_LIMB_MAX];
    /* 0x1D2 */ Vec3s morphTable[LULU_LIMB_MAX];
    /* 0x25C */ SkelAnime skelAnime;
    /* 0x2A0 */ ColliderCylinder collider;
    /* 0x2EC */ s16 unk_2EC;
    /* 0x2EE */ s16 unk_2EE;
    /* 0x2F0 */ Vec3s headRot;
    /* 0x2F6 */ Vec3s torsoRot;
    /* 0x2FC */ Vec3f unk_2FC;
    /* 0x308 */ Vec3f unk_308;
    /* 0x314 */ Vec3f unk_314;
    /* 0x320 */ u16 unk_320;
    /* 0x322 */ s16 animIndex;
    /* 0x324 */ s16 unk_324;
    /* 0x326 */ s16 cueId;
    /* 0x328 */ s16 csIdList[2];
    /* 0x32C */ s16 csIdIndex;
    /* 0x330 */ EnZovActionFunc actionFunc;
} EnZov; // size = 0x334

#endif // Z_EN_ZOV_H
