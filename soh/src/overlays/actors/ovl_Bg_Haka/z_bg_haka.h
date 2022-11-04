#ifndef Z_BG_HAKA_H
#define Z_BG_HAKA_H

#include "ultra64.h"
#include "global.h"

struct BgHaka;

typedef void (*BgHakaActionFunc)(struct BgHaka*, GlobalContext*);

typedef struct BgHaka {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgHakaActionFunc actionFunc;
    /*        */ u8 state;
} BgHaka; // size = 0x0168

#endif
