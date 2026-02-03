/**
 * movement_helper.c - Player movement utilities for custom items
 *
 * bgCheckFlags reference (z64actor.h):
 * - 0x001 BGCHECKFLAG_GROUND   = On ground
 * - 0x002                      = Just touched ground (1 frame)
 * - 0x004                      = Just left ground (1 frame)
 * - 0x008 BGCHECKFLAG_WALL     = Touching wall
 * - 0x010 BGCHECKFLAG_CEILING  = Touching ceiling
 * - 0x020 BGCHECKFLAG_WATER    = On/below water surface
 * - 0x040                      = Just touched water
 * - 0x100                      = Crushed between floor/ceiling
 */

#include "movement_helper.h"
#include "functions.h"
#include "variables.h"

#ifndef BGCHECKFLAG_GROUND
#define BGCHECKFLAG_GROUND   0x0001
#define BGCHECKFLAG_WALL     0x0008
#define BGCHECKFLAG_CEILING  0x0010
#define BGCHECKFLAG_WATER    0x0020
#define BGCHECKFLAG_CRUSHED  0x0100
#endif

void Movement_SpawnJumpSparkles(Player* player, PlayState* play, int count) {
    static Color_RGBA8 sPrimColor = { 200, 255, 255, 255 };
    static Color_RGBA8 sEnvColor = { 0, 150, 255, 255 };

    Vec3f pos;
    Vec3f vel = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.1f, 0.0f };

    for (int i = 0; i < count; i++) {
        pos.x = player->actor.world.pos.x + Rand_CenteredFloat(20.0f);
        pos.z = player->actor.world.pos.z + Rand_CenteredFloat(20.0f);
        pos.y = player->actor.world.pos.y + Rand_ZeroOne() * 10.0f;

        EffectSsKiraKira_SpawnSmall(play, &pos, &vel, &accel, &sPrimColor, &sEnvColor);
    }
}

s32 Movement_IsOnGround(Player* player) {
    return (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND);
}

s32 Movement_IsTouchingWall(Player* player) {
    return (player->actor.bgCheckFlags & BGCHECKFLAG_WALL);
}
