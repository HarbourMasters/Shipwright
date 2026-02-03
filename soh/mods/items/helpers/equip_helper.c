/**
 * equip_helper.c - Item input and equip state management
 */

#include "equip_helper.h"
#include "../custom_items.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "libultraship/bridge.h"

typedef struct {
    u32 frameCount;
    u8 cachedItems[8];
    u16 cachedButtons[256];
} EquipCache;

static EquipCache sEquipCache = { 0 };

static void EquipCache_Update(PlayState* play) {
    if (sEquipCache.frameCount == play->gameplayFrames) return;
    sEquipCache.frameCount = play->gameplayFrames;

    for (int i = 0; i < 256; i++) sEquipCache.cachedButtons[i] = 0;

    u8 dpadEnabled = CVarGetInteger("gEnhancements.DpadEquips", 0);
    u8 maxSlot = dpadEnabled ? 8 : 4;

    for (u8 slot = 1; slot < maxSlot; slot++) {
        u8 itemId = gSaveContext.equips.buttonItems[slot];
        sEquipCache.cachedItems[slot] = itemId;
        if (itemId != ITEM_NONE && itemId < 256) {
            sEquipCache.cachedButtons[itemId] = sButtonMasks[slot];
        }
    }
}

u16 ItemInput_GetEquippedButton(u8 itemId, PlayState* play) {
    EquipCache_Update(play);
    return sEquipCache.cachedButtons[itemId];
}

void ItemInput_Update(ItemInputState* out, u8 itemId, Player* player, PlayState* play) {
    out->equippedButton = ItemInput_GetEquippedButton(itemId, play);
    out->wasEquipped = (out->equippedButton != 0);

    if (!out->wasEquipped) {
        out->isPressed = out->isHeld = out->isReleased = out->otherButtonPressed = out->damageTaken = 0;
        return;
    }

    u16 press = play->state.input[0].press.button;
    u16 held = play->state.input[0].cur.button;

    out->isPressed = (press & out->equippedButton) != 0;
    out->isHeld = (held & out->equippedButton) != 0;
    out->isReleased = !out->isHeld && !out->isPressed;
    out->otherButtonPressed = ItemInput_CheckOtherButtons(out->equippedButton, &play->state.input[0]);
    out->damageTaken = 0;
}

u8 ItemInput_CheckDamage(Player* player, s8* prevInvincibility) {
    u8 damage = (player->invincibilityTimer > 0 && *prevInvincibility == 0);
    *prevInvincibility = player->invincibilityTimer;
    return damage;
}

u8 ItemInput_CheckOtherButtons(u16 equippedButton, Input* input) {
    static const u16 sActionButtons = BTN_A | BTN_B | BTN_R | BTN_START |
        BTN_CLEFT | BTN_CDOWN | BTN_CRIGHT | BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT;
    return (input->press.button & (sActionButtons & ~equippedButton)) != 0;
}

u8 ItemInput_IsBlockedEx(Player* player, PlayState* play, u8 skipOptionalBlockers) {
    if (player->stateFlags1 & ITEM_BLOCK_STATE1) return 1;
    if (player->stateFlags1 & PLAYER_STATE1_START_CHANGING_HELD_ITEM) return 1;
    if (play->shootingGalleryStatus != 0) return 1;

    if (!skipOptionalBlockers) {
        if (player->meleeWeaponState != 0) return 1;
        if (player->stateFlags1 & PLAYER_STATE1_SHIELDING) return 1;
        if ((player->stateFlags1 & PLAYER_STATE1_IN_WATER) && !(player->actor.bgCheckFlags & 0x0001)) return 1;
    }

    return 0;
}

u8 ItemInput_IsBlocked(Player* player, PlayState* play) {
    return ItemInput_IsBlockedEx(player, play, 0);
}

void ItemInput_RequestItemChange(Player* player, PlayState* play) {
    if (player->heldItemAction >= 0 && player->heldItemAction != PLAYER_IA_NONE) {
        player->heldItemId = ITEM_NONE;
        player->stateFlags1 |= PLAYER_STATE1_START_CHANGING_HELD_ITEM;
    }
}

u8 ItemInput_CanInterrupt(Player* player) {
    if (player->meleeWeaponState != 0) return 0;
    if (player->stateFlags1 & (PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_CARRYING_ACTOR |
        PLAYER_STATE1_READY_TO_FIRE | PLAYER_STATE1_BOOMERANG_THROWN)) return 0;
    return 1;
}

void ItemEquip_PlayEquipSFX(PlayState* play, Player* player) {
    Audio_PlaySoundGeneral(NA_SE_PL_CHANGE_ARMS, &player->actor.world.pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

void ItemEquip_PlayUnequipSFX(PlayState* play, Player* player) {
    Audio_PlaySoundGeneral(NA_SE_PL_CHANGE_ARMS, &player->actor.world.pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

u8 ItemEquip_Update(ItemEquipState* state, ItemInputState* input, EquipCallback onEquip,
                    UnequipCallback onUnequip, Player* player, PlayState* play) {
    if (!input->wasEquipped) {
        if (state->isEquipped && onUnequip) onUnequip(play, player);
        state->isEquipped = 0;
        return 0;
    }

    if (ItemInput_CheckDamage(player, &state->prevInvincibility)) {
        if (state->isEquipped && onUnequip) onUnequip(play, player);
        state->isEquipped = 0;
        return 0;
    }

    if (input->otherButtonPressed) {
        if (state->isEquipped && onUnequip) onUnequip(play, player);
        state->isEquipped = 0;
        return 0;
    }

    if (!state->isEquipped && input->isPressed) {
        if (onEquip) onEquip(play, player);
        state->isEquipped = 1;
    }

    return state->isEquipped;
}

void ItemMagic_Consume(PlayState* play, s16 amount) {
    if (gSaveContext.magic >= amount) gSaveContext.magic -= amount;
}

s32 ItemMagic_HasEnough(PlayState* play, s16 amount) {
    return (gSaveContext.magicCapacity > 0 && gSaveContext.magic >= amount);
}

u8 ItemSword_HasAnySword(void) {
    if (CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_KOKIRI)) return 1;
    if (CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER)) return 1;
    if (CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BIGGORON)) return 1;
    return 0;
}

u8 ItemSword_GetCurrentASword(void) {
    u8 aButton = gSaveContext.equips.buttonItems[0];
    if (aButton == ITEM_SWORD_KOKIRI || aButton == ITEM_SWORD_MASTER ||
        aButton == ITEM_SWORD_BGS || aButton == ITEM_SWORD_KNIFE) {
        return aButton;
    }
    return ITEM_NONE;
}

void ItemSword_EquipKokiriToA(void) {
    gSaveContext.equips.buttonItems[0] = ITEM_SWORD_KOKIRI;
}

void ItemSword_RestoreA(u8 prevItem) {
    if (prevItem != ITEM_NONE) {
        gSaveContext.equips.buttonItems[0] = prevItem;
    }
}

u8 ItemHeld_IsActive(Player* player, s32 itemAction) {
    return (player->heldItemAction == itemAction);
}

u16 ItemHeld_GetEquippedButton(u8 itemId, PlayState* play) {
    return ItemInput_GetEquippedButton(itemId, play);
}

u8 ItemHeld_IsButtonHeld(u8 itemId, Player* player, PlayState* play) {
    u16 button = ItemInput_GetEquippedButton(itemId, play);
    if (button == 0) return 0;
    return (play->state.input[0].cur.button & button) != 0;
}

u8 ItemHeld_IsButtonPressed(u8 itemId, Player* player, PlayState* play) {
    u16 button = ItemInput_GetEquippedButton(itemId, play);
    if (button == 0) return 0;
    return (play->state.input[0].press.button & button) != 0;
}
