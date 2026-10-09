#ifndef Z_OBJ_MILK_BIN_H
#define Z_OBJ_MILK_BIN_H

#include "global.h"

struct ObjMilkBin;

typedef struct ObjMilkBin {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 unk_144[0x4];
    /* 0x148 */ ColliderCylinder collider;
    /* 0x194 */ s32 disableDraw;
    /* 0x198 */ s32 type;
} ObjMilkBin; // size = 0x19C

typedef enum {
    /* 0 */ OBJ_MILK_BIN_TYPE_DEFAULT,
    /* 1 */ OBJ_MILK_BIN_TYPE_1, // This actor will kill itself if the player has obtained the milk bottle and the current time is the night of the second day onwards.
    /* 2 */ OBJ_MILK_BIN_TYPE_2 // Disables drawing and collision
} ObjMilkBinType;

#endif // Z_OBJ_MILK_BIN_H
