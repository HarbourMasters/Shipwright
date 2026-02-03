/**
 * item_switchhook.c - Switch Hook from Oracle of Ages
 *
 * TODO: Not yet implemented
 *
 * Planned Features:
 *   - Fire hook at objects/enemies
 *   - Swap positions with target on hit
 *   - Can swap with pots, blocks, and certain enemies
 */

#include "z64.h"
#include "../custom_items.h"
#include "../helpers/equip_helper.h"
#include "../helpers/movement_helper.h"
#include "macros.h"
#include "functions.h"

void Player_InitSwitchHookIA(PlayState* play, Player* this) {}

void Handle_SwitchHook(Player* this, PlayState* play) {
    ItemInputState input;
    ItemInput_Update(&input, ITEM_SWITCH_HOOK, this, play);
    if (!input.wasEquipped) return;

    // TODO: Implement switch hook mechanics
}

s32 Player_UpperAction_SwitchHook(Player* this, PlayState* play) { return 0; }
