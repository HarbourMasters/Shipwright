/**
 * Beetle Item Header
 * Controllable flying beetle that can grab items and damage enemies
 */

#ifndef ITEM_BEETLE_H
#define ITEM_BEETLE_H

#include "z64.h"
#include "../custom_items.h"

#ifdef __cplusplus
extern "C" {
#endif

// States
#define BEETLE_STATE_IDLE       0
#define BEETLE_STATE_AIMING     1
#define BEETLE_STATE_FLYING     2
#define BEETLE_STATE_RETURNING  3

// Movement
#define BEETLE_SPEED            12.0f
#define BEETLE_RETURN_SPEED     18.0f
#define BEETLE_MAX_DISTANCE     800.0f
#define BEETLE_CATCH_DISTANCE   50.0f
#define BEETLE_LAUNCH_OFFSET_Y  40.0f
#define BEETLE_LAUNCH_OFFSET_XZ 30.0f

// Collision
#define BEETLE_DAMAGE_RADIUS    15.0f
#define BEETLE_DAMAGE_HEIGHT    18.0f
#define BEETLE_GRAB_RADIUS      20.0f
#define BEETLE_DMG_FLAGS        0x00000010

// Timing
#define BEETLE_MAX_TIME         600

// Control
#define BEETLE_TURN_SPEED       0x360
#define BEETLE_PITCH_MAX        0x3000
#define BEETLE_CAM_DISTANCE     120.0f
#define BEETLE_CAM_HEIGHT       30.0f

// Model
#define BEETLE_MODEL_SCALE      0.02f
#define BEETLE_ANGLE_TO_RAD     (M_PI / 0x8000)

// Wing animation
#define BEETLE_WING_ANIM_SPEED  0.7f
#define BEETLE_WING_SCALE_MIN   0.3f
#define BEETLE_WING_SCALE_MAX   1.0f

// Sounds
#define BEETLE_SFX_LAUNCH       NA_SE_IT_SWORD_SWING
#define BEETLE_SFX_FLY          NA_SE_EN_BIRI_FLY
#define BEETLE_SFX_HIT          NA_SE_IT_SHIELD_BOUND
#define BEETLE_SFX_CATCH        NA_SE_PL_CATCH_BOOMERANG
#define BEETLE_SFX_RETURN       NA_SE_PL_CHANGE_ARMS
#define BEETLE_SFX_EQUIP        NA_SE_PL_CHANGE_ARMS

// State aliases
#define beetleActive            gCustomItemState.beetleActive
#define beetleState             gCustomItemState.beetleState
#define beetlePos               gCustomItemState.beetlePos
#define beetleRot               gCustomItemState.beetleRot
#define beetleGrabbed           gCustomItemState.beetleGrabbed
#define beetleWingScale         gCustomItemState.beetleWingScale
#define beetleWingDir           gCustomItemState.beetleWingDir
#define beetleTimer             gCustomItemState.beetleTimer
#define beetleFirstPerson       gCustomItemState.beetleFirstPersonActive
#define beetleCollider          gCustomItemState.beetleCollider
#define beetleStartPos          gCustomItemState.beetleStartPos
#define beetleSubCamId          gCustomItemState.beetleSubCamId

// Object functions
void Beetle_UpdateWingAnimation(f32* scale, s8* direction);

// State check
u8 Beetle_IsFlying(void);

#ifdef __cplusplus
}
#endif

#endif
