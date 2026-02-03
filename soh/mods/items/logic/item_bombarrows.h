/**
 * Bomb Arrows Item Header
 * Arrows that explode on impact, consuming both arrows and bombs
 * Uses real bomb actors for authentic vanilla explosion behavior
 */

#ifndef ITEM_BOMBARROWS_H
#define ITEM_BOMBARROWS_H

#include "z64.h"
#include "../custom_items.h"

// States
#define BOMBARROW_STATE_IDLE       0
#define BOMBARROW_STATE_CHARGING   1
#define BOMBARROW_STATE_FLYING     2

// State aliases - maps to gCustomItemState fields
#define baActive       gCustomItemState.bombArrowActive
#define baState        gCustomItemState.bombArrowState
#define baBombActor    gCustomItemState.bombArrowBombActor
#define baArrowActor   gCustomItemState.bombArrowArrowActor
#define baFirstPerson  gCustomItemState.bombArrowFirstPersonActive
#define baButtonMask   gCustomItemState.bombArrowButtonMask

#endif // ITEM_BOMBARROWS_H
