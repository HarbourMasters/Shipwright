#ifndef Z_EN_GE3_H
#define Z_EN_GE3_H

#include "ultra64.h"
#include "global.h"
#include "overlays/actors/ovl_En_GeldB/z_en_geldb.h"

struct EnGe3;

typedef void (*EnGe3ActionFunc)(struct EnGe3*, PlayState*);

typedef struct EnGe3 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderCylinder collider;
    /* 0x0198 */ SkelAnime skelAnime;
    /* 0x01DC */ Vec3s jointTable[GELDB_LIMB_MAX];
    /* 0x026C */ Vec3s morphTable[GELDB_LIMB_MAX];
    /* 0x02FC */ s16 eyeIndex;
    /* 0x02FE */ s16 blinkTimer;
    /* 0x0300 */ Vec3s headRot;
    /* 0x0306 */ Vec3s unk_306;
    /* 0x030C */ u16 unk_30C;
    /* 0x0310 */ EnGe3ActionFunc actionFunc;
} EnGe3; // size = 0x0314

#endif
