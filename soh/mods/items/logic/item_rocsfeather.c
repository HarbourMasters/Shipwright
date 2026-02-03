/**
 * item_rocsfeather.c - Roc's Feather from Oracle games
 *
 * Controls:
 *   C Button: High jump (works on ground and in water)
 *
 * Features:
 *   - Single high jump with sparkle effects
 *   - Reduced jump velocity in water
 */

#include "z64.h"
#include "item_rocsfeather.h"
#include "../custom_items.h"
#include "../helpers/equip_helper.h"
#include "../helpers/fx_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"

void Handle_RocsFeather(Player* p, PlayState* play) {
    ItemInputState in;
    ItemInput_Update(&in, ITEM_ROCS_FEATHER, p, play);

    if (!in.wasEquipped || !in.isPressed) return;

    s32 isOnGround = (p->actor.bgCheckFlags & BGCHECKFLAG_GROUND);
    s32 inWater = (p->stateFlags1 & PLAYER_STATE1_IN_WATER);

    // Can jump on ground OR in water (with reduced force)
    if (isOnGround || inWater) {
        f32 jumpVel = inWater ? ROCSFEATHER_WATER_JUMP_VELOCITY : ROCSFEATHER_JUMP_VELOCITY;
        p->actor.velocity.y = jumpVel;
        Player_PlaySfx(p, ROCSFEATHER_SOUND_JUMP);
        FX_SpawnSparkles(p, play);
    }
}
