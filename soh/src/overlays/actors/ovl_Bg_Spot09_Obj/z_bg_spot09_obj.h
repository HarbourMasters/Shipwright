#ifndef Z_BG_SPOT09_OBJ_H
#define Z_BG_SPOT09_OBJ_H

#include "ultra64.h"
#include "global.h"

struct BgSpot09Obj;

typedef struct BgSpot09Obj {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ char unk_164[0x04];
} BgSpot09Obj; // size = 0x0168

#endif
