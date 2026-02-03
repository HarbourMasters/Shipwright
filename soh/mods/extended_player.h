/**
 * extended_player.h - Extended player item action system
 *
 * Maps custom item IDs (ITEM_xxx) to player actions (PLAYER_IA_xxx).
 * Provides lookup functions for item behavior, model groups, and initialization.
 *
 * Used by: z_player.c, kaleido_scope, item logic files
 */
#ifndef EXTENDED_PLAYER_H
#define EXTENDED_PLAYER_H
#include <stdint.h>
#include <stdbool.h>
#include "z64player.h"
#include "z64item.h"
#ifdef __cplusplus
extern "C" {
#endif

// Vanilla array sizes (these are the original array sizes before custom items)
#define VANILLA_SITEMACTIONS_SIZE 56      // Original sItemActions size (up to ITEM_CLAIM_CHECK)
#define VANILLA_PLAYER_IA_COUNT 67        // PLAYER_IA 0x00-0x42 (67 actions)

// Custom item range in ITEM_xxx enum
#define CUSTOM_ITEM_START 0x9C            // ITEM_ROCS_FEATHER
#define CUSTOM_ITEM_END 0xB5              // ITEM_PENDING_3

// Custom PLAYER_IA range
#define CUSTOM_PLAYER_IA_START 0x43       // PLAYER_IA_ROCS_FEATHER
#define CUSTOM_PLAYER_IA_END 0x5C         // PLAYER_IA_PENDING_3

// ============================================================================
// FUNCTION POINTER TYPES
// ============================================================================
struct Player;
struct PlayState;
typedef int32_t (*ItemActionUpdateFunc)(struct Player* player, struct PlayState* play);
typedef void (*ItemActionInitFunc)(struct PlayState* play, struct Player* player);

// ============================================================================
// HELPER FUNCTIONS - Use these instead of directly accessing arrays
// ============================================================================

/**
 * Get the PLAYER_IA_xxx value for a given ITEM_xxx value.
 * Handles both vanilla and custom items using switch for custom items.
 */
int8_t ExtPlayer_GetItemAction(int32_t item);

/**
 * Get the model group for a given PLAYER_IA_xxx value.
 * Handles both vanilla and custom item actions.
 */
uint8_t ExtPlayer_GetActionModelGroup(int32_t itemAction);

/**
 * Get the update function for a given PLAYER_IA_xxx value.
 * Handles both vanilla and custom item actions.
 */
ItemActionUpdateFunc ExtPlayer_GetItemActionUpdateFunc(int32_t itemAction);

/**
 * Get the init function for a given PLAYER_IA_xxx value.
 * Handles both vanilla and custom item actions.
 */
ItemActionInitFunc ExtPlayer_GetItemActionInitFunc(int32_t itemAction);

/**
 * Check if an item ID is a custom item.
 */
static inline bool ExtPlayer_IsCustomItem(int32_t item) {
    return (item >= CUSTOM_ITEM_START && item <= CUSTOM_ITEM_END);
}

/**
 * Check if a PLAYER_IA value is a custom action.
 */
static inline bool ExtPlayer_IsCustomItemAction(int32_t itemAction) {
    return (itemAction >= CUSTOM_PLAYER_IA_START && itemAction <= CUSTOM_PLAYER_IA_END);
}

#ifdef __cplusplus
}
#endif
#endif // EXTENDED_PLAYER_H
