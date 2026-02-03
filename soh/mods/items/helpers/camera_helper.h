/**
 * camera_helper.h - First-person aiming and camera utilities
 */
#ifndef CAMERA_HELPER_H
#define CAMERA_HELPER_H
#include "z64.h"
#include "z64player.h"
#include "macros.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CAMERA_MODE_FIRST_PERSON = 0,
    CAMERA_MODE_Z_TARGET = 1,
    CAMERA_MODE_FREE = 2
} ItemCameraMode;

typedef struct {
    ItemCameraMode mode;
    u8 firstPersonActive;
} ItemCameraState;

// === First-Person Mode ===

/**
 * Enter first-person weapon aiming mode.
 * @param player Player instance
 * @param play PlayState instance
 */
void FirstPerson_Init(Player* player, PlayState* play);

/**
 * Update first-person mode each frame. Call while aiming.
 * @param player Player instance
 * @param play PlayState instance
 */
void FirstPerson_Update(Player* player, PlayState* play);

/**
 * Exit first-person mode and restore normal camera.
 * @param player Player instance
 * @param play PlayState instance
 */
void FirstPerson_Exit(Player* player, PlayState* play);

/**
 * @param player Player instance
 * @return Aim yaw in binary angle (0x0000-0xFFFF)
 */
s16 FirstPerson_GetAimYaw(Player* player);

/**
 * @param player Player instance
 * @return Aim pitch in binary angle (0x0000-0xFFFF)
 */
s16 FirstPerson_GetAimPitch(Player* player);

/**
 * Draw aiming reticle at raycast hit point.
 * @param player Player instance
 * @param play PlayState instance
 * @param range Max raycast distance (0 = default 10000)
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 */
void FirstPerson_DrawReticle(Player* player, PlayState* play, f32 range, u8 r, u8 g, u8 b);

// === Item Camera State Machine ===

/**
 * Initialize camera state. Auto-detects Z-targeting vs first-person.
 * @param state Camera state struct to initialize
 * @param player Player instance
 * @param play PlayState instance
 */
void ItemCamera_Init(ItemCameraState* state, Player* player, PlayState* play);

/**
 * Update camera state. Handles Z-target transitions.
 * @param state Camera state struct
 * @param player Player instance
 * @param play PlayState instance
 */
void ItemCamera_Update(ItemCameraState* state, Player* player, PlayState* play);

/**
 * Clean up camera state and exit any active modes.
 * @param state Camera state struct
 * @param player Player instance
 * @param play PlayState instance
 */
void ItemCamera_Exit(ItemCameraState* state, Player* player, PlayState* play);

/**
 * Toggle first-person mode on/off.
 * @param state Camera state struct
 * @param player Player instance
 * @param play PlayState instance
 */
void ItemCamera_ToggleFirstPerson(ItemCameraState* state, Player* player, PlayState* play);

/**
 * Get aim yaw based on current camera mode.
 * @param state Camera state struct
 * @param player Player instance
 * @param play PlayState instance
 * @return Yaw toward target (Z) or aim direction (first-person)
 */
s16 ItemCamera_GetAimYaw(ItemCameraState* state, Player* player, PlayState* play);

/**
 * Get aim pitch based on current camera mode.
 * @param state Camera state struct
 * @param player Player instance
 * @return Pitch (only in first-person, 0 otherwise)
 */
s16 ItemCamera_GetAimPitch(ItemCameraState* state, Player* player);

/**
 * Switch to free movement mode (no camera lock).
 * @param state Camera state struct
 * @param player Player instance
 * @param play PlayState instance
 */
void ItemCamera_SetFreeMode(ItemCameraState* state, Player* player, PlayState* play);

// === Camera Direction Utilities ===

/**
 * @param play PlayState instance
 * @return Camera facing yaw in binary angle
 */
s16 Camera_GetDirectionYaw(PlayState* play);

/**
 * @param play PlayState instance
 * @return Camera facing pitch in binary angle
 */
s16 Camera_GetDirectionPitch(PlayState* play);

/**
 * Smoothly interpolate yaw/pitch toward camera direction.
 * @param currentYaw Pointer to current yaw (modified in place)
 * @param currentPitch Pointer to current pitch (modified in place)
 * @param play PlayState instance
 * @param yawSpeed Interpolation speed for yaw
 * @param pitchSpeed Interpolation speed for pitch
 */
void Camera_InterpolateToDirection(s16* currentYaw, s16* currentPitch, PlayState* play, s16 yawSpeed, s16 pitchSpeed);

// === Stick Input for Projectile Control ===

/**
 * Convert analog stick to yaw/pitch deltas.
 * @param play PlayState instance
 * @param outYawDelta Output yaw delta
 * @param outPitchDelta Output pitch delta
 * @param sensitivity Multiplier for stick values
 */
void Input_GetStickDirection(PlayState* play, s16* outYawDelta, s16* outPitchDelta, s16 sensitivity);

/**
 * Update projectile direction from stick input (linear).
 * @param yaw Pointer to yaw (modified in place)
 * @param pitch Pointer to pitch (modified in place)
 * @param play PlayState instance
 * @param turnSpeed Turn rate per frame
 * @param pitchMax Max pitch angle limit
 */
void Projectile_UpdateDirectionFromStick(s16* yaw, s16* pitch, PlayState* play, s16 turnSpeed, s16 pitchMax);

/**
 * Update projectile rotation from stick input (radial).
 * @param yaw Pointer to yaw (modified in place)
 * @param pitch Pointer to pitch (modified in place)
 * @param play PlayState instance
 * @param turnSpeed Turn rate scaling
 * @param pitchMax Max pitch angle limit
 */
void Projectile_UpdateRotationFromStick(s16* yaw, s16* pitch, PlayState* play, s16 turnSpeed, s16 pitchMax);

#ifdef __cplusplus
}
#endif
#endif
