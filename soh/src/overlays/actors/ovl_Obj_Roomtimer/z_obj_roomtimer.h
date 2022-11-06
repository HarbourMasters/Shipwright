#ifndef Z_OBJ_ROOMTIMER_H
#define Z_OBJ_ROOMTIMER_H

#include "ultra64.h"
#include "global.h"
#include "z64.h"

struct ObjRoomtimer;

typedef void (*ObjRoomtimerActionFunc)(struct ObjRoomtimer*, PlayState*);

typedef struct ObjRoomtimer {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ObjRoomtimerActionFunc actionFunc;
    /* 0x0150 */ u32 switchFlag;
} ObjRoomtimer; // size = 0x0154

#endif
