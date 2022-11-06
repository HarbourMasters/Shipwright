#ifndef Z_BG_GND_ICEBLOCK_H
#define Z_BG_GND_ICEBLOCK_H

#include "ultra64.h"
#include "global.h"

struct BgGndIceblock;

typedef void (*BgGndIceblockActionFunc)(struct BgGndIceblock*, PlayState*);

typedef struct BgGndIceblock {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ BgGndIceblockActionFunc actionFunc;
    /* 0x0168 */ Vec3f targetPos;
} BgGndIceblock; // size = 0x0174

#endif
