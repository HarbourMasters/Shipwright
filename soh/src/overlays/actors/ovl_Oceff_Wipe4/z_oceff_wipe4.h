#ifndef Z_OCEFF_WIPE4_H
#define Z_OCEFF_WIPE4_H

#include "ultra64.h"
#include "global.h"

typedef enum {
    /* 0x00 */ OCEFF_WIPE4_SCARECROWS,
    /* 0x01 */ OCEFF_WIPE4_UNUSED
} OceffWipe4Type;

struct OceffWipe4;

typedef struct OceffWipe4 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s16 timer;
} OceffWipe4; // size = 0x0150

#endif
