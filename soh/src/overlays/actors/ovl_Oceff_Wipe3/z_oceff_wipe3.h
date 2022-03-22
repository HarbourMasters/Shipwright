#ifndef Z_OCEFF_WIPE3_H
#define Z_OCEFF_WIPE3_H

#include "ultra64.h"
#include "global.h"

struct OceffWipe3;

typedef struct OceffWipe3 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s16 counter;
} OceffWipe3; // size = 0x0150

#endif
