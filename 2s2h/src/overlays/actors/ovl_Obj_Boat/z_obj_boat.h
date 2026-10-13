#ifndef Z_OBJ_BOAT_H
#define Z_OBJ_BOAT_H

#include "global.h"

#define OBJBOAT_GET_PATH_INDEX(thisx) (((thisx)->params >> 7) & 0x1F)
#define OBJBOAT_GET_4000(thisx) ((thisx)->params & 0x4000)

struct ObjBoat;

typedef struct ObjBoat {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ u8 curPointIndex;
    /* 0x15D */ s8 direction; // To follow the path
    /* 0x15E */ u8 lastPointIndex; // max point if direction is negative, first point if forwards
    /* 0x15F */ union {
        u8 timer;
        u8 cueId;
    };
    /* 0x160 */ s16 angle; // Angle used to set rotations
    /* 0x162 */ UNK_TYPE1 pad_162;
    /* 0x163 */ u8 maxPointIndex; // point at the other end from 0
    /* 0x164 */ Vec3s* pathPoints;
} ObjBoat; // size = 0x168

#endif // Z_OBJ_BOAT_H
