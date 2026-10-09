#ifndef Z_OBJ_SHUTTER_H
#define Z_OBJ_SHUTTER_H

#include "global.h"

struct ObjShutter;

typedef struct ObjShutter {
    /* 0x000 */ Actor actor;
    /* 0x144 */ u8 scheduleResult;
    /* 0x148 */ f32 verticalOffset;
} ObjShutter; // size = 0x14C

#endif // Z_OBJ_SHUTTER_H
