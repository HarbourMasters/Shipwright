/**
 * Shovel Item Header
 * Digging item with all restrictions (no water, no attacking, no shielding)
 */

#ifndef ITEM_SHOVEL_H
#define ITEM_SHOVEL_H

#include "z64.h"
#include "../custom_items.h"

// Radius
#define SHOVEL_USE_RADIUS       80.0f
#define SHOVEL_BEAN_RADIUS      100.0f
#define SHOVEL_DOOR_ANA_RADIUS  120.0f
#define SHOVEL_HOLE_RADIUS      40.0f

// Drops
#define SHOVEL_ITEM_DROP_CHANCE 15
#define SHOVEL_FAIRY_CHANCE     1

// Animation
#define SHOVEL_ANIM_DURATION    30
#define SHOVEL_HOLE_LIFETIME    300

// Scene
#ifndef SCENE_GRAVEYARD
#define SCENE_GRAVEYARD 0x53
#endif

// Actor IDs
#ifndef ACTOR_EN_REEBA
#define ACTOR_EN_REEBA 0x001C
#endif
#ifndef ACTOR_EN_DEKUNUTS
#define ACTOR_EN_DEKUNUTS 0x0060
#endif
#ifndef ACTOR_EN_DNS
#define ACTOR_EN_DNS 0x011A
#endif
#ifndef ACTOR_EN_HINTNUTS
#define ACTOR_EN_HINTNUTS 0x0192
#endif

// Item drop types
#ifndef ITEM00_RUPEE_GREEN
#define ITEM00_RUPEE_GREEN 0x00
#endif
#ifndef ITEM00_RUPEE_BLUE
#define ITEM00_RUPEE_BLUE 0x01
#endif
#ifndef ITEM00_RUPEE_RED
#define ITEM00_RUPEE_RED 0x02
#endif
#ifndef ITEM00_HEART
#define ITEM00_HEART 0x03
#endif
#ifndef ITEM00_BOMBS_A
#define ITEM00_BOMBS_A 0x04
#endif
#ifndef ITEM00_MAGIC_LARGE
#define ITEM00_MAGIC_LARGE 0x08
#endif
#ifndef ITEM00_MAGIC_SMALL
#define ITEM00_MAGIC_SMALL 0x09
#endif
#ifndef ITEM00_FAIRY
#define ITEM00_FAIRY 0x0A
#endif
#ifndef ITEM00_NUTS
#define ITEM00_NUTS 0x0C
#endif
#ifndef ITEM00_HEART_PIECE
#define ITEM00_HEART_PIECE 0x06
#endif

// State aliases
#define shActive     gCustomItemState.shovelActive
#define shAnimating  gCustomItemState.shovelAnimating
#define shAnimTimer  gCustomItemState.shovelAnimTimer
#define shHoleActor  gCustomItemState.shovelHoleActor

#endif // ITEM_SHOVEL_H
