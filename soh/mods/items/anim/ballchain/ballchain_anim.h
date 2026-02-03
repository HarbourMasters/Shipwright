/**
 * Ball and Chain Animation/Pose Header
 * Pose data and functions for ball chain item
 */

#ifndef BALLCHAIN_ANIM_H
#define BALLCHAIN_ANIM_H

#include "z64.h"
#include "z64player.h"

#ifdef __cplusplus
extern "C" {
#endif

// Pose functions
void BallChain_ResetPose(Player* p);
void BallChain_SetEquipPose(Player* p);
void BallChain_SetSpinPose(Player* p, f32 stickX, f32 stickY);

// Lean multiplier constant (exported for external use)
#define BALLCHAIN_POSE_LEAN_MULT  3500.0f

#ifdef __cplusplus
}
#endif

#endif // BALLCHAIN_ANIM_H
