#ifndef Z_OBJ_WIND_H
#define Z_OBJ_WIND_H

#include "global.h"

#define OBJ_WIND_GET_TYPE(thisx) (((thisx)->params >> 7) & 0x1F)
#define OBJ_WIND_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)

struct ObjWind;

typedef struct ObjWind {
    /* 0x000 */ Actor actor;
    /* 0x144 */ u8 isUnderWater;
} ObjWind; // size = 0x148

#endif // Z_OBJ_WIND_H
