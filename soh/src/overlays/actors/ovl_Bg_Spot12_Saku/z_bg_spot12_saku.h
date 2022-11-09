#ifndef Z_BG_SPOT12_SAKU_H
#define Z_BG_SPOT12_SAKU_H

#include "ultra64.h"
#include "global.h"

struct BgSpot12Saku;

typedef void (*BgSpot12SakuActionFunc)(struct BgSpot12Saku*, PlayState*);

typedef struct BgSpot12Saku {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgSpot12SakuActionFunc actionFunc;
    /* 0x0168 */ s16 timer;
} BgSpot12Saku; // size = 0x016C

#endif
