#ifndef Z_OBJ_DINNER_H
#define Z_OBJ_DINNER_H

#include "global.h"

struct ObjDinner;

typedef struct ObjDinner {
    /* 0x000 */ Actor actor;
    /* 0x144 */ char unk_144[0x4];
} ObjDinner; // size = 0x148

#endif // Z_OBJ_DINNER_H
