#ifndef Z_BG_GND_FIREMEIRO_H
#define Z_BG_GND_FIREMEIRO_H

#include "ultra64.h"
#include "global.h"

struct BgGndFiremeiro;

typedef void (*BgGndFiremeiroActionFunc)(struct BgGndFiremeiro*, PlayState*);

typedef struct BgGndFiremeiro {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ Vec3f initPos;
    /* 0x0170 */ u16 timer;
    /* 0x0174 */ BgGndFiremeiroActionFunc actionFunc;
} BgGndFiremeiro; // size = 0x0178

#endif
