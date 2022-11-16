#ifndef Z_BG_TOKI_SWD_H
#define Z_BG_TOKI_SWD_H

#include "ultra64.h"
#include "global.h"

struct BgTokiSwd;

typedef void (*BgTokiSwdActionFunc)(struct BgTokiSwd*, PlayState*);

typedef struct BgTokiSwd {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ BgTokiSwdActionFunc actionFunc;
    /* 0x0150 */ ColliderCylinder collider;
} BgTokiSwd; // size = 0x019C

#endif
