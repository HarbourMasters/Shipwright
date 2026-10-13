#ifndef Z_BG_DBLUE_ELEVATOR_H
#define Z_BG_DBLUE_ELEVATOR_H

#include "global.h"

#define BG_DBLUE_ELEVATOR_GET_INDEX(thisx) (((thisx)->params >> 8) & 0x3)
#define BG_DBLUE_ELEVATOR_GET_SWITCH_FLAG(thisx, x) (((thisx)->params + (x)) & 0x7F)

typedef enum {
    /* 0 */ BG_DBLUE_ELEVATOR_WATER_FLOW_STOPPED,
    /* 1 */ BG_DBLUE_ELEVATOR_WATER_FLOW_FORWARD,
    /* 2 */ BG_DBLUE_ELEVATOR_WATER_FLOW_REVERSED,
    /* 3 */ BG_DBLUE_ELEVATOR_WATER_FLOW_BOTH_DIRECTIONS
} BgDblueElevatorWaterFlow;

struct BgDblueElevator;

typedef void (*BgDblueElevatorActionFunc)(struct BgDblueElevator*, PlayState*);

typedef struct BgDblueElevator {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgDblueElevatorActionFunc actionFunc;
    /* 0x160 */ f32 posStep;
    /* 0x164 */ f32 posOffset;
    /* 0x168 */ s8 direction;
    /* 0x169 */ s8 activationTimer;
    /* 0x16A */ s8 pauseTimer;
    /* 0x16B */ s8 isWithinWaterBoxXZ;
    /* 0x16C */ f32 waterSurfacePosY;
} BgDblueElevator; // size = 0x170

struct BgDblueElevatorStruct1;

typedef s32 (*BgDblueElevatorStruct1Func)(struct BgDblueElevator*, PlayState*);

typedef struct BgDblueElevatorStruct1 {
    /* 0x00 */ s32 isHorizontal;
    /* 0x04 */ BgDblueElevatorStruct1Func getWaterFlow;
    /* 0x08 */ f32 targetPosOffset;
    /* 0x0C */ s8 pauseDuration;
    /* 0x0D */ s8 initialDirection;
    /* 0x10 */ f32 accelerationStep;
    /* 0x14 */ f32 decelerationStep;
    /* 0x18 */ f32 targetPosStep;
} BgDblueElevatorStruct1; // size = 0x1C

#endif // Z_BG_DBLUE_ELEVATOR_H
