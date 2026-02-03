/**
 * movement_helper.h - Player movement utilities for custom items
 */

#ifndef MOVEMENT_HELPER_H
#define MOVEMENT_HELPER_H

#include "z64.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Check if player is on ground.
 * DEPRECATED: Use (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) directly.
 * @param player Player instance
 * @return Non-zero if on ground
 */
s32 Movement_IsOnGround(Player* player);

/**
 * Check if player is touching a wall.
 * DEPRECATED: Use (player->actor.bgCheckFlags & BGCHECKFLAG_WALL) directly.
 * @param player Player instance
 * @return Non-zero if touching wall
 */
s32 Movement_IsTouchingWall(Player* player);

/**
 * Spawn sparkle particles around player.
 * @param player Player instance
 * @param play PlayState instance
 * @param count Number of sparkles
 */
void Movement_SpawnJumpSparkles(Player* player, PlayState* play, int count);

#ifdef __cplusplus
}
#endif

#endif // MOVEMENT_HELPER_H
