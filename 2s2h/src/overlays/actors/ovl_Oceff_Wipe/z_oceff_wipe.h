#ifndef Z_OCEFF_WIPE_H
#define Z_OCEFF_WIPE_H

#include "global.h"

typedef enum {
    /* 0x0 */ OCEFF_WIPE_ZL,
    /* 0x1 */ OCEFF_WIPE_SOT
} OceffWipeType;

struct OceffWipe;

typedef struct OceffWipe {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s16 counter;
} OceffWipe; // size = 0x148

#endif // Z_OCEFF_WIPE_H
