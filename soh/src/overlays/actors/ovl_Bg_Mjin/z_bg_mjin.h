#ifndef Z_BG_MJIN_H
#define Z_BG_MJIN_H

#include "ultra64.h"
#include "global.h"

struct BgMjin;

typedef void (*BgMjinActionFunc)(struct BgMjin*, PlayState*);

typedef struct BgMjin {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ s8 objBankIndex;
    /* 0x0168 */ BgMjinActionFunc actionFunc;
} BgMjin; // size = 0x016C

#endif
