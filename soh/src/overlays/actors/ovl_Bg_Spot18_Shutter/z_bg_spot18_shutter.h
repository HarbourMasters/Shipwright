#ifndef Z_BG_SPOT18_SHUTTER_H
#define Z_BG_SPOT18_SHUTTER_H

#include "ultra64.h"
#include "global.h"

struct BgSpot18Shutter;

typedef void (*BgSpot18ShutterActionFunc)(struct BgSpot18Shutter*, PlayState*);

typedef struct BgSpot18Shutter {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgSpot18ShutterActionFunc actionFunc;
} BgSpot18Shutter; // size = 0x0168

#endif
