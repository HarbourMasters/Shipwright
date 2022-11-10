#ifndef Z_BG_SPOT05_SOKO_H
#define Z_BG_SPOT05_SOKO_H

#include "ultra64.h"
#include "global.h"

struct BgSpot05Soko;

typedef void (*BgSpot05SokoActionFunc)(struct BgSpot05Soko*, PlayState*);

typedef struct BgSpot05Soko {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgSpot05SokoActionFunc actionFunc;
    /* 0x0168 */ s32 switchFlag;
} BgSpot05Soko; // size = 0x016C

#endif
