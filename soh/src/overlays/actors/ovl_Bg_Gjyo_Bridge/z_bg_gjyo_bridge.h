#ifndef Z_BG_GJYO_BRIDGE_H
#define Z_BG_GJYO_BRIDGE_H

#include "ultra64.h"
#include "global.h"

struct BgGjyoBridge;

typedef void (*BgGjyoBridgeActionFunc)(struct BgGjyoBridge*, PlayState*);

typedef struct BgGjyoBridge {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgGjyoBridgeActionFunc actionFunc;
} BgGjyoBridge; // size = 0x0168

#endif
