#ifndef Z_BG_GATE_SHUTTER_H
#define Z_BG_GATE_SHUTTER_H

#include "ultra64.h"
#include "global.h"

struct BgGateShutter;

typedef void (*BgGateShutterActionFunc)(struct BgGateShutter*, PlayState*);

typedef struct BgGateShutter {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgGateShutterActionFunc actionFunc;
    /* 0x0168 */ s16 openingState; // 1 if gate is opening
    /* 0x016C */ Vec3f somePos;
    /* 0x0178 */ s16 unk_178;
} BgGateShutter; // size = 0x017C

#endif
