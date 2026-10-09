#ifndef Z_BG_OPEN_SPOT_H
#define Z_BG_OPEN_SPOT_H

#include "global.h"

struct BgOpenSpot;

typedef struct BgOpenSpot {
    /* 0x000 */ Actor actor;
    /* 0x144 */ AnimatedMaterial* texScrolls;
} BgOpenSpot; // size = 0x148

#endif // Z_BG_OPEN_SPOT_H
