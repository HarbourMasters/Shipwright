#ifndef ITEM_DESIRE_SENSOR_H
#define ITEM_DESIRE_SENSOR_H

#include "../custom_items.h"

// States
#define DSENSOR_STATE_IDLE      0
#define DSENSOR_STATE_SENSING   1   // Mini-cutscene: player frozen, mystical sparkles
#define DSENSOR_STATE_RESULT    2   // Result revealed: golden burst or Ganondorf laugh

// Timings (frames at 20fps)
#define DSENSOR_SENSING_DURATION    50  // ~2.5 seconds sensing phase
#define DSENSOR_RESULT_DURATION     40  // ~2 seconds result hold

// Health cost: 3 hearts = 0x30 health units (each heart = 0x10)
#define DSENSOR_HEALTH_COST         0x30

// Sound effects
#define DSENSOR_SE_FOUND    NA_SE_SY_CORRECT_CHIME  // 0x4802 - Major item found
#define DSENSOR_SE_NOTHING  NA_SE_EN_GANON_LAUGH     // 0x39C7 - No major item
#define DSENSOR_SE_ERROR    NA_SE_SY_ERROR            // Not enough health / not rando

// State aliases (map to CustomItemState fields)
#define dsActive    gCustomItemState.desireSensorActive
#define dsState     gCustomItemState.desireSensorState
#define dsTimer     gCustomItemState.desireSensorTimer
#define dsResult    gCustomItemState.desireSensorResult

#endif // ITEM_DESIRE_SENSOR_H
