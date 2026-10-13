#ifndef Z_OCEFF_STORM_H
#define Z_OCEFF_STORM_H

#include "global.h"

struct OceffStorm;

typedef void (*OceffStormActionFunc)(struct OceffStorm*, PlayState*);

typedef struct OceffStorm {
    /* 0x000 */ Actor actor;
    /* 0x144 */ u16 counter;
    /* 0x146 */ u8 primColorAlpha;
    /* 0x147 */ u8 vtxAlpha;
    /* 0x148 */ s16 posYOff;
    /* 0x14A */ s16 posYOffAdd;
    /* 0x14C */ OceffStormActionFunc actionFunc;
} OceffStorm; // size = 0x150

typedef enum {
    /* 1 */ OCEFF_STORM_ACTION_PARAM_1 = 1
} OceffStormActionParam;

#endif // Z_OCEFF_STORM_H
