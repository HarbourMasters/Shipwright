#ifndef Z_OCEFF_WIPE_H
#define Z_OCEFF_WIPE_H

#include "ultra64.h"
#include "global.h"

typedef enum {
    /* 0x00 */ OCEFF_WIPE_ZL,
    /* 0x01 */ OCEFF_WIPE_SOT
} OceffWipeType;

struct OceffWipe;

typedef struct OceffWipe {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s16 timer;
} OceffWipe; // size = 0x0150

#endif
