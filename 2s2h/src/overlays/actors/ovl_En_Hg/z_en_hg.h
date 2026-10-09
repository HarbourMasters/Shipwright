#ifndef Z_EN_HG_H
#define Z_EN_HG_H

#include "global.h"
#include "objects/object_harfgibud/object_harfgibud.h"


struct EnHg;

typedef void (*EnHgActionFunc)(struct EnHg*, PlayState*);

typedef struct EnHg {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ SkelAnime skelAnime;
    /* 0x1D4 */ EnHgActionFunc actionFunc;
    /* 0x1D8 */ MtxF mf;
    /* 0x218 */ s32 csIdIndex;
    /* 0x21C */ s32 animIndex;
    /* 0x220 */ Vec3s jointTable[PAMELAS_FATHER_GIBDO_LIMB_MAX];
    /* 0x292 */ Vec3s morphTable[PAMELAS_FATHER_GIBDO_LIMB_MAX];
    /* 0x304 */ UNK_TYPE1 pad304[0xC];
    /* 0x310 */ u16 csIdList[4];
} EnHg; // size = 0x318

#endif // Z_EN_HG_H
