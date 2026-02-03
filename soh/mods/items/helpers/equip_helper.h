/**
 * equip_helper.h - Item input and equip state management
 */

#ifndef EQUIP_HELPER_H
#define EQUIP_HELPER_H

#include "z64.h"
#include "z64player.h"

#ifdef __cplusplus
extern "C" {
#endif

static const u16 sButtonMasks[8] = {
    BTN_B, BTN_CLEFT, BTN_CDOWN, BTN_CRIGHT,
    BTN_DUP, BTN_DDOWN, BTN_DLEFT, BTN_DRIGHT
};

#define ITEM_BLOCK_STATE1 ( \
    PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_LOADING | \
    PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_TALKING | PLAYER_STATE1_GETTING_ITEM | \
    PLAYER_STATE1_DAMAGED | PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE | \
    PLAYER_STATE1_CLIMBING_LADDER | PLAYER_STATE1_ON_HORSE | PLAYER_STATE1_HOOKSHOT_FALLING | \
    PLAYER_STATE1_CHARGING_SPIN_ATTACK \
)

/**
 * Input state for custom item polling.
 */
typedef struct {
    u16 equippedButton;     // Button mask (BTN_CLEFT, etc.)
    u8 isPressed;           // Pressed this frame
    u8 isHeld;              // Currently held
    u8 isReleased;          // Released this frame
    u8 wasEquipped;         // Item is on a C-button
    u8 otherButtonPressed;  // Another action button pressed
    u8 damageTaken;         // Player took damage
} ItemInputState;

/**
 * Equip state with callbacks.
 */
typedef struct {
    u8 isEquipped;          // Item currently active
    u8 shouldUnequip;       // Pending unequip
    s8 prevInvincibility;   // For damage detection
} ItemEquipState;

typedef void (*EquipCallback)(PlayState* play, Player* player);
typedef void (*UnequipCallback)(PlayState* play, Player* player);

/**
 * Get button mask for equipped item.
 * @param itemId ITEM_xxx constant
 * @param play PlayState instance
 * @return Button mask or 0 if not equipped
 */
u16 ItemInput_GetEquippedButton(u8 itemId, PlayState* play);

/**
 * Update input state for a custom item.
 * @param out Output state struct
 * @param itemId ITEM_xxx constant
 * @param player Player instance
 * @param play PlayState instance
 */
void ItemInput_Update(ItemInputState* out, u8 itemId, Player* player, PlayState* play);

/**
 * Check if player took damage since last frame.
 * @param player Player instance
 * @param prevInvincibility Previous invincibility value (updated)
 * @return 1 if damage taken, 0 otherwise
 */
u8 ItemInput_CheckDamage(Player* player, s8* prevInvincibility);

/**
 * Check if another action button was pressed.
 * @param equippedButton Button to exclude
 * @param input Input struct
 * @return 1 if other button pressed
 */
u8 ItemInput_CheckOtherButtons(u16 equippedButton, Input* input);

/**
 * Check if item activation is blocked.
 * @param player Player instance
 * @param play PlayState instance
 * @return 1 if blocked
 */
u8 ItemInput_IsBlocked(Player* player, PlayState* play);

/**
 * Check if item activation is blocked (extended).
 * @param player Player instance
 * @param play PlayState instance
 * @param skipOptionalBlockers Skip water/shield/attack checks
 * @return 1 if blocked
 */
u8 ItemInput_IsBlockedEx(Player* player, PlayState* play, u8 skipOptionalBlockers);

/**
 * Trigger held item put-away animation.
 * @param player Player instance
 * @param play PlayState instance
 */
void ItemInput_RequestItemChange(Player* player, PlayState* play);

/**
 * Check if player can be interrupted by custom item.
 * @param player Player instance
 * @return 1 if can interrupt
 */
u8 ItemInput_CanInterrupt(Player* player);

/**
 * Update equip state and call callbacks.
 * @param state Equip state
 * @param input Input state
 * @param onEquip Called when equipped
 * @param onUnequip Called when unequipped
 * @param player Player instance
 * @param play PlayState instance
 * @return 1 if equipped
 */
u8 ItemEquip_Update(ItemEquipState* state, ItemInputState* input, EquipCallback onEquip,
                    UnequipCallback onUnequip, Player* player, PlayState* play);

/**
 * Play equip sound effect.
 */
void ItemEquip_PlayEquipSFX(PlayState* play, Player* player);

/**
 * Play unequip sound effect.
 */
void ItemEquip_PlayUnequipSFX(PlayState* play, Player* player);

/**
 * Consume magic.
 * @param play PlayState instance
 * @param amount Magic to consume
 */
void ItemMagic_Consume(PlayState* play, s16 amount);

/**
 * Check if player has enough magic.
 * @param play PlayState instance
 * @param amount Required magic
 * @return 1 if enough
 */
s32 ItemMagic_HasEnough(PlayState* play, s16 amount);

/**
 * Check if player owns any sword.
 * @return 1 if owns sword
 */
u8 ItemSword_HasAnySword(void);

/**
 * Get current sword on A button.
 * @return ITEM_xxx or ITEM_NONE
 */
u8 ItemSword_GetCurrentASword(void);

/**
 * Force equip Kokiri Sword to A button.
 */
void ItemSword_EquipKokiriToA(void);

/**
 * Restore previous A button item.
 * @param prevItem Item to restore
 */
void ItemSword_RestoreA(u8 prevItem);

/**
 * Check if custom item is held via vanilla system.
 * @param player Player instance
 * @param itemAction PLAYER_IA_xxx constant
 * @return 1 if active
 */
u8 ItemHeld_IsActive(Player* player, s32 itemAction);

/**
 * Get button for equipped item (alias for ItemInput_GetEquippedButton).
 * @param itemId ITEM_xxx constant
 * @param play PlayState instance
 * @return Button mask or 0
 */
u16 ItemHeld_GetEquippedButton(u8 itemId, PlayState* play);

/**
 * Check if item's button is held.
 * @param itemId ITEM_xxx constant
 * @param player Player instance
 * @param play PlayState instance
 * @return 1 if held
 */
u8 ItemHeld_IsButtonHeld(u8 itemId, Player* player, PlayState* play);

/**
 * Check if item's button was pressed this frame.
 * @param itemId ITEM_xxx constant
 * @param player Player instance
 * @param play PlayState instance
 * @return 1 if pressed
 */
u8 ItemHeld_IsButtonPressed(u8 itemId, Player* player, PlayState* play);

#ifdef __cplusplus
}
#endif

#endif // EQUIP_HELPER_H
