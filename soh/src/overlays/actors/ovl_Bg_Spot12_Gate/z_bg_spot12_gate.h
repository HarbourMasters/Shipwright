#ifndef Z_BG_SPOT12_GATE_H
#define Z_BG_SPOT12_GATE_H

#include "ultra64.h"
#include "global.h"

struct BgSpot12Gate;

typedef void (*BgSpot12GateActionFunc)(struct BgSpot12Gate*, PlayState*);

typedef struct BgSpot12Gate {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgSpot12GateActionFunc actionFunc;
    /* 0x0168 */ s16 unk_168;
} BgSpot12Gate; // size = 0x016C

#endif
