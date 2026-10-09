#ifndef Z_OBJ_GRASS_UNIT_H
#define Z_OBJ_GRASS_UNIT_H

#include "global.h"

#define OBJGRASSUNIT_GET_DROPTABLE(thisx) (((thisx)->params >> 8) & 0x1F)
#define OBJGRASSUNIT_GET_PATTERN(thisx) ((thisx)->params & 1)

struct ObjGrassUnit;

typedef struct ObjGrassUnit {
    /* 0x000 */ Actor actor;
} ObjGrassUnit; // size = 0x144

#endif // Z_OBJ_GRASS_UNIT_H
