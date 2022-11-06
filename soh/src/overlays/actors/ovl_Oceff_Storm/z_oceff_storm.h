#ifndef Z_OCEFF_STORM_H
#define Z_OCEFF_STORM_H

#include "ultra64.h"
#include "global.h"

struct OceffStorm;

typedef void (*OceffStormActionFunc)(struct OceffStorm*, PlayState*);

typedef struct OceffStorm {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ u16 counter;
    /* 0x014E */ u8 primColorAlpha;
    /* 0x014F */ u8 vtxAlpha;
    /* 0x0150 */ s16 posYOff;
    /* 0x0152 */ s16 posYOffAdd;
    /* 0x0154 */ OceffStormActionFunc actionFunc;
} OceffStorm; // size = 0x0158

#endif
