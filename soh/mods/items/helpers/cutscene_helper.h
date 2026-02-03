/**
 * cutscene_helper.h - Mini-cutscene system for item activations
 */

#ifndef CUTSCENE_HELPER_H
#define CUTSCENE_HELPER_H

#include "z64.h"

/**
 * State for item-triggered cutscenes.
 */
typedef struct {
    u8 active;              // 1 if cutscene running
    u8 phase;               // Current phase index
    s16 timer;              // Frames since start
    s16 originalCameraMode; // Restored on end
    Vec3f cameraOffset;     // Relative to player
    f32 cameraOrbitAngle;   // Radians
    f32 cameraOrbitSpeed;   // Radians/frame
    f32 cameraOrbitRadius;  // Distance from player
    f32 cameraHeight;       // Height above player
} CutsceneItemState;

/**
 * Start a mini-cutscene. Freezes player input and nearby enemies.
 * @param player Player instance
 * @param play PlayState instance
 * @param state Cutscene state to initialize
 */
void CutsceneHelper_Start(Player* player, PlayState* play, CutsceneItemState* state);

/**
 * Update cutscene each frame. Call until returns 1.
 * @param player Player instance
 * @param play PlayState instance
 * @param state Cutscene state
 * @return 1 if finished, 0 if still running
 */
u8 CutsceneHelper_Update(Player* player, PlayState* play, CutsceneItemState* state);

/**
 * End cutscene and restore player control.
 * @param player Player instance
 * @param play PlayState instance
 * @param state Cutscene state
 */
void CutsceneHelper_End(Player* player, PlayState* play, CutsceneItemState* state);

/**
 * Configure camera orbit parameters.
 * @param state Cutscene state
 * @param radius Distance from player
 * @param height Height above player
 * @param speed Rotation speed (radians/frame)
 */
void CutsceneHelper_SetCameraOrbit(CutsceneItemState* state, f32 radius, f32 height, f32 speed);

/**
 * Freeze all enemies within radius.
 * @param play PlayState instance
 * @param center Center point
 * @param radius Effect radius
 */
void CutsceneHelper_FreezeEnemies(PlayState* play, Vec3f* center, f32 radius);

/**
 * Unfreeze all enemies.
 * @param play PlayState instance
 */
void CutsceneHelper_UnfreezeEnemies(PlayState* play);

#endif // CUTSCENE_HELPER_H
