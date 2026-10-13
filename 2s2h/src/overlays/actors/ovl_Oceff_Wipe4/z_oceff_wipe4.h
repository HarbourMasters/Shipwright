#ifndef Z_OCEFF_WIPE4_H
#define Z_OCEFF_WIPE4_H

#include "global.h"

typedef enum {
    /* 0 */ OCEFF_WIPE4_SCARECROWS,
    /* 1 */ OCEFF_WIPE4_UNUSED
} OceffWipe4Type;

struct OceffWipe4;

typedef struct OceffWipe4 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s16 counter;     
} OceffWipe4; // size = 0x148

#endif // Z_OCEFF_WIPE4_H
