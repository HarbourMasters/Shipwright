#ifndef Z_BG_INGATE_H
#define Z_BG_INGATE_H

#include "ultra64.h"
#include "global.h"

struct BgInGate;

typedef void (*BgInGateActionFunc)(struct BgInGate*, GlobalContext*);

typedef struct BgInGate {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgInGateActionFunc actionFunc;
} BgInGate; // size = 0x0168

#endif
