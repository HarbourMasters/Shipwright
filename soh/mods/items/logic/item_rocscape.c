/**
 * item_rocscape.c - Roc's Cape from Four Swords Adventures
 *
 * Controls:
 *   C Button (ground): High jump with sparkle effects
 *   C Button (air):    Double jump (once per airtime)
 *
 * Features:
 *   - Ground jump with sparkle effects
 *   - Double jump resets on landing
 *   - Reduced jump velocity in water
 *   - Shockwave effect on double jump
 */

#include "z64.h"
#include "item_rocscape.h"
#include "../custom_items.h"
#include "../helpers/equip_helper.h"
#include "../helpers/fx_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"

void Handle_RocsCape(Player* p, PlayState* play) {
    ItemInputState in;
    ItemInput_Update(&in, ITEM_ROCS_CAPE, p, play);

    if (!in.wasEquipped) return;

    s32 isOnGround = (p->actor.bgCheckFlags & BGCHECKFLAG_GROUND);
    s32 inWater = (p->stateFlags1 & PLAYER_STATE1_IN_WATER);

    // Reset double jump when on ground
    if (isOnGround) rcJumpCount = 0;

    if (!in.isPressed) return;

    if (isOnGround || inWater) {
        // Ground/water jump
        f32 jumpVel = inWater ? ROCSCAPE_WATER_JUMP_VELOCITY : ROCSCAPE_JUMP_VELOCITY;
        p->actor.velocity.y = jumpVel;
        Player_PlaySfx(p, ROCSCAPE_SOUND_JUMP);
        FX_SpawnSparkles(p, play);
    } else if (rcJumpCount == 0) {
        // Double jump (once while airborne)
        rcJumpCount = 1;
        p->actor.velocity.y = ROCSCAPE_DOUBLE_JUMP_VELOCITY;
        Player_PlaySfx(p, ROCSCAPE_SOUND_DOUBLE);
        FX_SpawnSparkles(p, play);

        // Spawn shockwave at Link's feet (floorHeight), smaller scale
        Vec3f shockwavePos;
        shockwavePos.x = p->actor.world.pos.x;
        shockwavePos.y = p->actor.floorHeight + 2.0f;
        shockwavePos.z = p->actor.world.pos.z;
        FX_SpawnShockwaveSmall(play, &shockwavePos, 60, 150);
    }
}
