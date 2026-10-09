#ifndef Z_OBJ_ROOMTIMER_H
#define Z_OBJ_ROOMTIMER_H

#include "global.h"

#define ROOMTIMER_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 9) & 0x7F)

struct ObjRoomtimer;

typedef void (*ObjRoomtimerActionFunc)(struct ObjRoomtimer*, PlayState*);

typedef struct ObjRoomtimer {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ObjRoomtimerActionFunc actionFunc;
    /* 0x148 */ s32 switchFlag;
} ObjRoomtimer; // size = 0x14C

#endif // Z_OBJ_ROOMTIMER_H
