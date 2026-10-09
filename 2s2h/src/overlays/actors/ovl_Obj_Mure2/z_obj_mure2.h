#ifndef Z_OBJ_MURE2_H
#define Z_OBJ_MURE2_H

#include "global.h"

struct ObjMure2;

typedef void (*ObjMure2ActionFunc)(struct ObjMure2*, PlayState*);

#define OBJ_MURE2_CHILD_MAX 12
#define OBJ_MURE2_GET_CHILD_TYPE(thisx) ((thisx)->params & 3)
#define OBJ_MURE2_GET_CHILD_UPPER_PARAMS(thisx) ((((thisx)->params) >> 8) & 0x1F)

typedef struct ObjMure2 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ObjMure2ActionFunc actionFunc;
    /* 0x148 */ Actor* actors[OBJ_MURE2_CHILD_MAX];
    /* 0x178 */ u16 spawnFlags;
    /* 0x17C */ f32 rangeMultiplier;
} ObjMure2; // size = 0x180

#endif // Z_OBJ_MURE2_H
