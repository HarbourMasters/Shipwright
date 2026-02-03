/**
 * item_pending_2.c - Placeholder item 2 (slot 0xB3)
 *
 * TODO: Not yet implemented
 *
 * This slot is reserved for a future custom item.
 */

#include "z64.h"
#include "../custom_items.h"
#include "../helpers/equip_helper.h"
#include "../helpers/fx_helper.h"
#include "macros.h"
#include "functions.h"

void Player_InitPending2IA(PlayState* play, Player* this) {}

void Handle_Pending2(Player* this, PlayState* play) {
    ItemInputState input;
    ItemInput_Update(&input, ITEM_PENDING_2, this, play);
    if (!input.wasEquipped) return;

    // TODO: Implement item logic
}

s32 Player_UpperAction_Pending2(Player* this, PlayState* play) { return 0; }
