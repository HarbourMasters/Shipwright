/**
 * Zonai Permafrost Item Header
 * Time-freeze spell - freezes all target actors and day/night cycle for 10 seconds
 *
 * Sequence: Din's Fire cast (honoo1 -> honoo2 -> honoo3) -> time freeze -> free movement
 * No cooldown. 12 MP cost.
 */

#ifndef ITEM_ZONAI_PERMAFROST_H
#define ITEM_ZONAI_PERMAFROST_H

#include "z64.h"
#include "../custom_items.h"

// States
#define ZPERM_STATE_IDLE       0
#define ZPERM_STATE_CASTING    1   // Din's Fire animation (honoo1 -> honoo2 -> honoo3)
#define ZPERM_STATE_ACTIVE     2   // Freeze active, Link free to move
#define ZPERM_STATE_ENDING     3   // Cleanup frame

// Casting sub-phases (Din's Fire 3-part animation)
#define ZPERM_CAST_HONOO1   0
#define ZPERM_CAST_HONOO2   1
#define ZPERM_CAST_HONOO3   2

// Magic
#define ZPERM_MAGIC_COST        12

// Freeze duration
#define ZPERM_FREEZE_DURATION   600    // 10 seconds at 20fps

// Freeze timer refresh value (set on actors each frame)
// Must be > 1 because DECR decrements each frame before the check
#define ZPERM_FREEZE_REFRESH    3

// Visual flicker: last 2 seconds (40 frames) the green particles flicker
#define ZPERM_FLICKER_START     40

// Din's Fire animation speed (half of vanilla 0.83 because we double-update)
#define ZPERM_ANIM_SPEED        0.415f

// State aliases
#define zpActive    gCustomItemState.zonaiPermafrostActive
#define zpState     gCustomItemState.zonaiPermafrostState
#define zpSubPhase  gCustomItemState.zonaiPermafrostSubPhase
#define zpTimer     gCustomItemState.zonaiPermafrostTimer
#define zpSavedTime gCustomItemState.zonaiPermafrostSavedTimeIncr

#endif // ITEM_ZONAI_PERMAFROST_H
