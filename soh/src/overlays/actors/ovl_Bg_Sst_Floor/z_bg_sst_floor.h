#ifndef Z_BG_SST_FLOOR_H
#define Z_BG_SST_FLOOR_H

#include "ultra64.h"
#include "global.h"

struct BgSstFloor;

typedef struct BgSstFloor {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x014C */ s16 drumPhase;
    /* 0x0166 */ s16 drumAmp;
    /* 0x0168 */ s16 drumHeight;
} BgSstFloor; // size = 0x016C

typedef enum {
    /* 0 */ BONGOFLOOR_REST,
    /* 1 */ BONGOFLOOR_HIT
} BgSstFloorParams;

#endif
