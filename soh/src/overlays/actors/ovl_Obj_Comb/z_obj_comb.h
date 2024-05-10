#ifndef Z_OBJ_COMB_H
#define Z_OBJ_COMB_H

#include <libultraship/libultra.h>
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
    /*        */ BeehiveIdentity beehiveIdentity;
} ObjComb; // size = 0x01B4

void ObjComb_Break(ObjComb* objComb, PlayState* play);

#endif
