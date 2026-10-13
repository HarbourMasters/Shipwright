#ifndef Z_EN_MK_H
#define Z_EN_MK_H

#include "global.h"
#include "objects/object_mk/object_mk.h"

struct EnMk;

typedef void (*EnMkActionFunc)(struct EnMk*, PlayState*);

typedef struct EnMk {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ SkelAnime skelAnime;
    /* 0x1D4 */ Vec3s jointTable[MARINE_RESEARCHER_LIMB_MAX];
    /* 0x222 */ Vec3s morphTable[MARINE_RESEARCHER_LIMB_MAX];
    /* 0x270 */ Vec3s headRot;
    /* 0x276 */ s16 csIdList[2];
    /* 0x27A */ u16 unk_27A;
    /* 0x27C */ s16 animIndex;
    /* 0x280 */ EnMkActionFunc actionFunc;
} EnMk; // size = 0x284

#endif // Z_EN_MK_H
