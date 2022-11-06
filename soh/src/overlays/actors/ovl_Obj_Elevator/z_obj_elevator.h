#ifndef Z_OBJ_ELEVATOR_H
#define Z_OBJ_ELEVATOR_H

#include "ultra64.h"
#include "global.h"

struct ObjElevator;

typedef void (*ObjElevatorActionFunc)(struct ObjElevator*, PlayState*);

typedef struct ObjElevator {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ ObjElevatorActionFunc actionFunc;
    /* 0x0168 */ f32 unk_168;
    /* 0x016C */ f32 unk_16C;
    /* 0x0170 */ u8 unk_170;
} ObjElevator; // size = 0x0174

#endif
