#ifndef Z_BG_JYA_IRONOBJ_H
#define Z_BG_JYA_IRONOBJ_H

#include "ultra64.h"
#include "global.h"

struct BgJyaIronobj;

typedef void (*BgJyaIronobjActionFunc)(struct BgJyaIronobj*, PlayState*);

typedef struct BgJyaIronobj {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgJyaIronobjActionFunc actionFunc;
    /* 0x0168 */ ColliderCylinder colCylinder;
} BgJyaIronobj; // size = 0x01B4

#endif
