#ifndef Z_BG_SPOT11_BAKUDANKABE_H
#define Z_BG_SPOT11_BAKUDANKABE_H

#include "ultra64.h"
#include "global.h"

struct BgSpot11Bakudankabe;

typedef struct BgSpot11Bakudankabe {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ ColliderCylinder collider;
} BgSpot11Bakudankabe; // size = 0x01B0

#endif
