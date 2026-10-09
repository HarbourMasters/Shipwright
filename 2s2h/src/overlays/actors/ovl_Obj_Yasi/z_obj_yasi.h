#ifndef Z_OBJ_YASI_H
#define Z_OBJ_YASI_H

#include "global.h"

struct ObjYasi;

typedef struct ObjYasi {
    /* 0x000 */ DynaPolyActor dyna;
} ObjYasi; // size = 0x15C

#define PALM_TREE_IS_WIDE(thisx) ((thisx)->params & 1)

#endif // Z_OBJ_YASI_H
