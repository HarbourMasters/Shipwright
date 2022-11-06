#ifndef Z_OCEFF_SPOT_H
#define Z_OCEFF_SPOT_H

#include "ultra64.h"
#include "global.h"

struct OceffSpot;

typedef void (*OceffSpotActionFunc)(struct OceffSpot*, PlayState*);

typedef struct OceffSpot {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ LightNode* lightNode1;
    /* 0x0150 */ LightInfo lightInfo1;
    /* 0x0160 */ LightNode* lightNode2;
    /* 0x0164 */ LightInfo lightInfo2;
    /* 0x0174 */ f32 unk_174;
    /* 0x0178 */ u16 timer;
    /* 0x017C */ OceffSpotActionFunc actionFunc;
} OceffSpot; // size = 0x0180

#endif
