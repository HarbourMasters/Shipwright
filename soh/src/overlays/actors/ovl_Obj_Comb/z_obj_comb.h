#ifndef Z_OBJ_COMB_H
#define Z_OBJ_COMB_H

#include "ultra64.h"
#include "global.h"

struct ObjComb;

typedef void (*ObjCombActionFunc)(struct ObjComb*, PlayState*);

typedef struct ObjComb {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ObjCombActionFunc actionFunc;
    /* 0x0150 */ ColliderJntSph collider;
    /* 0x0170 */ ColliderJntSphElement colliderItems[1];
    /* 0x01B0 */ s16 unk_1B0;
    /* 0x01B2 */ s16 unk_1B2;
} ObjComb; // size = 0x01B4

#endif
