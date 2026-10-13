#ifndef Z_OBJ_TOKEI_STEP_H
#define Z_OBJ_TOKEI_STEP_H

#include "global.h"

struct ObjTokeiStep;

typedef void (*ObjTokeiStepFunc)(struct ObjTokeiStep*, struct PlayState*);

typedef struct ObjTokeiStepPanel {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ f32 posChangeY;
    /* 0x10 */ s16 startFallingTimer; // Timer for when a step starts falling after the previous step has started
                                      // bouncing during opening cutscene
    /* 0x12 */ s8 numBounces; // Counter for times the step falls below its final Y pos, and thus will bounce back up
                              // during opening cutscene
    /* 0x13 */ s8 hasSoundPlayed;
} ObjTokeiStepPanel; // size = 0x14

typedef struct ObjTokeiStep {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjTokeiStepFunc actionFunc;
    /* 0x160 */ ObjTokeiStepPanel panels[7];
} ObjTokeiStep; // size = 0x1EC

#endif // Z_OBJ_TOKEI_STEP_H
