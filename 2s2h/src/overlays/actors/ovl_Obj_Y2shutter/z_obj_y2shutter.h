#ifndef Z_OBJ_Y2SHUTTER_H
#define Z_OBJ_Y2SHUTTER_H

#include "global.h"

struct ObjY2shutter;

typedef struct ObjY2shutter {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ s16 openTimer;
    /* 0x15E */ u8 isStationary; // true if the door is neither opening nor closing
    /* 0x15F */ u8 settleTimer;
} ObjY2shutter; // size = 0x160

typedef enum {
    /* 0 */ SHUTTER_BARRED,
    /* 1 */ SHUTTER_GRATED
} ShutterType;

#define OBJY2SHUTTER_GET_TYPE(thisx) (((thisx)->params >> 7) & 1)
#define OBJY2SHUTTER_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)

#endif // Z_OBJ_Y2SHUTTER_H
