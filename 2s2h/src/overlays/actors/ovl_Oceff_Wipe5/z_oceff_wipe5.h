#ifndef Z_OCEFF_WIPE5_H
#define Z_OCEFF_WIPE5_H

#include "global.h"

#define OCEFF_WIPE5_GET_SONG_TYPE(thisx) ((thisx)->params & 0xF)

struct OceffWipe5;

typedef struct OceffWipe5 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s16 counter;
} OceffWipe5; // size = 0x148

#endif // Z_OCEFF_WIPE5_H
