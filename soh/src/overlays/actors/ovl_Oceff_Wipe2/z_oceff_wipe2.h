#ifndef Z_OCEFF_WIPE2_H
#define Z_OCEFF_WIPE2_H

#include "ultra64.h"
#include "global.h"

struct OceffWipe2;

typedef struct OceffWipe2 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s16 timer;
} OceffWipe2; // size = 0x0150

#endif
