/**
 * Spinner Item Header
 * Definitions, constants, collider data, and breakable rock list
 */

#ifndef ITEM_SPINNER_H
#define ITEM_SPINNER_H

#include "z64.h"
#include "../custom_items.h"

// States
#define SPINNER_STATE_IDLE           0
#define SPINNER_STATE_CHARGING       1
#define SPINNER_STATE_RIDING         2
#define SPINNER_STATE_ATTACKING      3
#define SPINNER_STATE_HOMING_WINDUP  4  // Wind back
#define SPINNER_STATE_HOMING_AIM     5  // Aim at target
#define SPINNER_STATE_HOMING_LAUNCH  6  // Arc attack
#define SPINNER_STATE_RECOIL         7

// Homing timings
#define HOMING_WINDUP_DURATION  10
#define HOMING_AIM_DURATION     5

// Physics
#define SPINNER_CHARGE_MAX       60
#define SPINNER_SPEED_MIN        12.0f
#define SPINNER_SPEED_MAX        25.0f
#define SPINNER_SPEED_HOMING     30.0f
#define SPINNER_HOVER_HEIGHT     12.0f
#define SPINNER_Y_OFFSET         5.0f
#define SPINNER_STEER_RATE       0x400
#define SPINNER_RIDE_DURATION    120
#define SPINNER_ATTACK_DURATION  20
#define SPINNER_RECOIL_DURATION  15
#define SPINNER_HOMING_ARC       120.0f

// Collider radii
#define SPINNER_COL_RADIUS       24
#define SPINNER_COL_RADIUS_ATK   30
#define SPINNER_COL_RADIUS_HOME  30
#define SPINNER_COL_HEIGHT       16

// Damage values (Master Sword regular slash = 2)
#define SPINNER_DMG_RIDE         2
#define SPINNER_DMG_ATTACK       2
#define SPINNER_DMG_HOMING       4

// State aliases - maps to gCustomItemState fields
#define sActive      gCustomItemState.spinnerActive
#define sState       gCustomItemState.timer2
#define sCharge      gCustomItemState.timer1
#define sSpeed       gCustomItemState.sharedProjectilePos.x
#define sAngle       gCustomItemState.spinnerWallBumpTimer
#define sTimer       gCustomItemState.globalCooldownTimer
#define sAtkTimer    gCustomItemState.spinnerSpinAttackTimer
#define sTarget      gCustomItemState.gustJarTarget
#define sHomingTime  gCustomItemState.sharedProjectilePos.y

// Breakable rock actor IDs - add/remove as needed
static const s16 sBreakableRockIds[] = {
    0x014E,  // Silver boulder
    0x01D2,  // Bronze boulder
    0x0127,  // Brown boulder
    0x0130,  // Rolling boulders
};
#define BREAKABLE_ROCK_COUNT (sizeof(sBreakableRockIds) / sizeof(sBreakableRockIds[0]))

// Cucco actor IDs
#define ACTOR_EN_NIW         0x0019
#define ACTOR_EN_ATTACK_NIW  0x0144

// Collider init
static ColliderCylinderInit sSpinnerColInit = {
    { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER, AC_NONE, OC1_ON | OC1_TYPE_ALL, OC2_TYPE_PLAYER, COLSHAPE_CYLINDER },
    { ELEMTYPE_UNK2, { DMG_SLASH_MASTER, 0x00, SPINNER_DMG_RIDE }, { 0, 0, 0 }, TOUCH_ON | TOUCH_SFX_NORMAL, BUMP_NONE, OCELEM_ON },
    { SPINNER_COL_RADIUS, SPINNER_COL_HEIGHT, 0, { 0, 0, 0 } },
};

#endif // ITEM_SPINNER_H
