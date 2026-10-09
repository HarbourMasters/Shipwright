#ifndef Z_OBJ_DOWSING_H
#define Z_OBJ_DOWSING_H

#include "global.h"

#define DOWSING_GET_TYPE(thisx) ((thisx)->params >> 7)
#define DOWSING_GET_FLAG(thisx) ((thisx)->params & 0x7F)

struct ObjDowsing;

typedef enum {
    /* 1 */ DOWSING_COLLECTIBLE = 1,
    /* 2 */ DOWSING_CHEST,
    /* 3 */ DOWSING_SWITCH
} DowsingType;

typedef struct ObjDowsing {
    /* 0x000 */ Actor actor;
} ObjDowsing; // size = 0x144

#endif // Z_OBJ_DOWSING_H
