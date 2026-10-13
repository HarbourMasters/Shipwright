#ifndef Z_OCEFF_SPOT_H
#define Z_OCEFF_SPOT_H

#include "global.h"

struct OceffSpot;

typedef void (*OceffSpotActionFunc)(struct OceffSpot*, PlayState*);

typedef struct OceffSpot {
    /* 0x000 */ Actor actor;
    /* 0x144 */ LightNode* lightNode1;
    /* 0x148 */ LightInfo lightInfo1;
    /* 0x158 */ LightNode* lightNode2;
    /* 0x15C */ LightInfo lightInfo2;
    /* 0x16C */ f32 unk16C;
    /* 0x170 */ u16 timer;
    /* 0x174 */ OceffSpotActionFunc actionFunc;
} OceffSpot; // size = 0x178

#endif // Z_OCEFF_SPOT_H
