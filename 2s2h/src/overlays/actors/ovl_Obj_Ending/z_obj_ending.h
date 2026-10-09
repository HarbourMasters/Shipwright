#ifndef Z_OBJ_ENDING_H
#define Z_OBJ_ENDING_H

#include "global.h"

struct ObjEnding;

typedef struct {
    /* 0x0 */ Gfx* dLists[2];
    /* 0x8 */ AnimatedMaterial *animMat;
} ObjEndingModelInfo; // size = 0xC

typedef struct ObjEnding {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ObjEndingModelInfo* modelInfo;
    /* 0x148 */ AnimatedMaterial* animMat;
} ObjEnding; // size = 0x14C

#endif // Z_OBJ_ENDING_H
