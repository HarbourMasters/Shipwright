#ifndef Z_EN_FR_H
#define Z_EN_FR_H

#include "global.h"

struct EnFr;

#define ENFR_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define ENFR_GET_ATTENTION_RANGE_TYPE(thisx) (((thisx)->params >> 7) & 0xF)

typedef struct EnFr {
    /* 0x000 */ Actor actor;
} EnFr; // size = 0x144

#endif // Z_EN_FR_H
