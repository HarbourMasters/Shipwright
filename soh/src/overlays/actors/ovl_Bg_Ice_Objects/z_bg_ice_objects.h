#ifndef Z_BG_ICE_OBJECTS_H
#define Z_BG_ICE_OBJECTS_H

#include "ultra64.h"
#include "global.h"

struct BgIceObjects;

typedef void (*BgIceObjectsActionFunc) (struct BgIceObjects*, PlayState*);

typedef struct BgIceObjects {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgIceObjectsActionFunc actionFunc;
    /* 0x0168 */ Vec3f targetPos;
} BgIceObjects; // size = 0x0174

#endif
