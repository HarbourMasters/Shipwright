#ifndef Z_BG_ICE_SHUTTER_H
#define Z_BG_ICE_SHUTTER_H

#include "ultra64.h"
#include "global.h"

struct BgIceShutter;

typedef void (*BgIceShutterActionFunc)(struct BgIceShutter*, PlayState*);

typedef struct BgIceShutter {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgIceShutterActionFunc actionFunc;
} BgIceShutter; // size = 0x0168

#endif
