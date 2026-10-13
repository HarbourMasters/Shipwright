#ifndef Z_OBJ_SWPRIZE_H
#define Z_OBJ_SWPRIZE_H

#include "global.h"

#define OBJ_SWPRIZE_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define OBJ_SWPRIZE_GET_TYPE(thisx) (((thisx)->params >> 8) & 3)

struct ObjSwprize;

typedef void (*ObjSwprizeActionFunc)(struct ObjSwprize*, PlayState*);

typedef struct ObjSwprize {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ObjSwprizeActionFunc actionFunc;
    /* 0x148 */ s16 timer;
} ObjSwprize; // size = 0x14C

#endif // Z_OBJ_SWPRIZE_H
