/**
 * extended_inventory.h - Extended inventory system for custom items
 *
 * Manages a 2-page inventory system (48 total slots).
 * Page 1: Vanilla OOT items (slots 0-23)
 * Page 2: Custom items (slots 24-47)
 *
 * Page switching: Press L button in pause menu to toggle pages.
 */
#ifndef EXTENDED_INVENTORY_H
#define EXTENDED_INVENTORY_H
#include <stdint.h>
#include <stdbool.h>
#include "z64item.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int currentPage;        // 0 = vanilla, 1 = custom items
    int16_t pageSwitchTimer; // Cooldown to prevent rapid switching
} ExtendedInventoryState;

/**
 * @return Pointer to the global extended inventory state
 */
ExtendedInventoryState* ExtInv_GetState(void);

/**
 * Reset inventory state to defaults (page 0, no cooldown)
 */
void ExtInv_Reset(void);

/**
 * Update inventory state each frame (handles page switch cooldown)
 */
void ExtInv_Update(void);

/**
 * @return true if page switch cooldown has elapsed
 */
bool ExtInv_CanSwitchPage(void);

/**
 * Toggle between page 0 and page 1
 */
void ExtInv_SwitchPage(void);

/**
 * @return Current inventory page (0 or 1)
 */
int ExtInv_GetCurrentPage(void);

/**
 * Convert visual slot (0-23) to actual inventory slot based on current page
 * @param visualSlot - The slot position shown on screen (0-23)
 * @return Actual inventory slot index (0-47)
 */
int ExtInv_GetInventorySlot(int visualSlot);

/**
 * @param slot - Inventory slot to check
 * @return true if slot belongs to current page
 */
bool ExtInv_IsSlotOnCurrentPage(uint8_t slot);

/**
 * @param slot - Inventory slot
 * @return Page number (0 or 1) where this slot belongs
 */
int ExtInv_GetPageForSlot(uint8_t slot);

/**
 * @param itemId - Item ID (ITEM_xxx constant)
 * @return Age requirement (AGE_REQ_ADULT, AGE_REQ_CHILD, or AGE_REQ_NONE)
 */
uint8_t ExtInv_GetItemAgeReq(uint16_t itemId);

/**
 * @param slot - Inventory slot
 * @return Age requirement for items in this slot
 */
uint8_t ExtInv_GetSlotAgeReq(uint8_t slot);

/**
 * @param itemId - Custom item ID
 * @param language - Language index for localization
 * @return Pointer to item name texture, or NULL if not found
 */
void* ExtInv_GetCustomItemNameTex(uint16_t itemId, uint8_t language);

/**
 * @param itemId - Item ID
 * @return Pointer to item icon texture
 */
void* ExtInv_GetItemIcon(uint16_t itemId);

/**
 * @param itemId - Item ID
 * @return Inventory slot for this item, or 0xFF if not found
 */
uint8_t ExtInv_GetItemSlot(uint16_t itemId);
extern const uint8_t gPage2Items[24];
#define AGE_REQ_ADULT LINK_AGE_ADULT
#define AGE_REQ_CHILD LINK_AGE_CHILD
#define AGE_REQ_NONE 9
extern const uint8_t gPage2ItemAgeReqs[24];
// Roc's Feather and Roc's Cape share slot 24 (progressive upgrade system)
#define SLOT_ROCS 24              // Shared slot for Roc's Feather/Cape progressive
#define SLOT_ROCS_FEATHER 24      // Alias for compatibility
#define SLOT_ROCS_CAPE 24         // Now same slot as Feather (upgrade replaces it)
#define SLOT_WHIP 25
#define SLOT_SPINNER 26
#define SLOT_BOMB_ARROWS 27
#define SLOT_FIRE_ROD 28
#define SLOT_DEMISE_DESTRUCTION 29
#define SLOT_DEKU_LEAF 30
#define SLOT_TIME_GATE 31
#define SLOT_BEETLE 32
#define SLOT_SWITCH_HOOK 33
#define SLOT_ICE_ROD 34
#define SLOT_ZONAI_PERMAFROST 35
#define SLOT_MOGMA_MITTS 36
#define SLOT_GUST_JAR 37
#define SLOT_BALL_AND_CHAIN 38
#define SLOT_DESIRE_SENSOR 39
#define SLOT_LIGHT_ROD 40
#define SLOT_HYLIAS_GRACE 41
#define SLOT_PENDING_2 42
#define SLOT_PENDING_1 43
#define SLOT_PENDING_3 44
#define SLOT_CANE_OF_SOMARIA 45
#define SLOT_SHOVEL 46
#define SLOT_DOMINION_ROD 47
static inline uint8_t ExtInv_GetPage2AgeReq(uint8_t slot) {
    if (slot >= 24 && slot < 48) {
        return gPage2ItemAgeReqs[slot - 24];
    }
    return 9;
}
static inline bool ExtInv_CheckAgeReqForSlot(uint8_t slot, bool isAdult) {
    uint8_t req = ExtInv_GetSlotAgeReq(slot);
    return (req == 9) || (req == 0 && isAdult) || (req == 1 && !isAdult);
}
static inline bool ExtInv_ShouldRenderGrayscale(uint8_t slot, bool isAdult) {
    return !ExtInv_CheckAgeReqForSlot(slot, isAdult);
}
static inline void ExtInv_InitializePage2Items(void) {
    extern SaveContext gSaveContext;
    for (int i = 0; i < 24; i++) {
        if (gSaveContext.inventory.items[24 + i] == ITEM_NONE) {
            gSaveContext.inventory.items[24 + i] = gPage2Items[i];
        }
    }
}
static inline void ExtInv_ClearPage2Items(void) {
    extern SaveContext gSaveContext;
    for (int i = 24; i < 48; i++) {
        gSaveContext.inventory.items[i] = ITEM_NONE;
    }
}
static inline void ExtInv_GiveItem(uint8_t slot, uint8_t itemId) {
    extern SaveContext gSaveContext;
    if (slot >= 24 && slot < 48) {
        gSaveContext.inventory.items[slot] = itemId;
    }
}
static inline void ExtInv_SetItemById(uint8_t itemId) {
    extern SaveContext gSaveContext;
    uint8_t slot = ExtInv_GetItemSlot(itemId);
    if (slot != 0xFF) {
        gSaveContext.inventory.items[slot] = itemId;
    }
}
static inline void ExtInv_VerifyConsistency(void) {
    const int expectedSize = 24;
    const int actualSize = sizeof(gPage2Items) / sizeof(gPage2Items[0]);
    if (actualSize != expectedSize) {}
    const int ageReqSize = sizeof(gPage2ItemAgeReqs) / sizeof(gPage2ItemAgeReqs[0]);
    if (ageReqSize != expectedSize) {}
}
#ifdef __cplusplus
}
#endif
#endif
