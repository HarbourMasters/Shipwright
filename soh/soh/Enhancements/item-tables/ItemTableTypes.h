#pragma once
#ifdef __cplusplus
#include <stdint.h>
#endif

#define CHEST_ANIM_SHORT 0
#define CHEST_ANIM_LONG 1

/**
 * Flag to indicate which type of Actor has given the player an item. ITEM_FROM_NPC by default,
 * should be set by the actor giving the item before they send the GetItemEntry signal to the player.
 * ITEM_FROM_NPC is the default for the GET_ITEM macro so that the random NPC actors giving items
 * don't need to manually update the GetItemEntry before sending it to the player. Instead just the
 * specific actors (item00, en_box, en_si, etc.) can set their corresponding ITEM_FROM value.
 */
typedef enum GetItemFrom {
    ITEM_FROM_NPC,
    ITEM_FROM_SKULLTULA,
    ITEM_FROM_FREESTANDING,
    ITEM_FROM_CHEST,
} GetItemFrom;

typedef enum GetItemCategory {
    /* 0x00 */ ITEM_CATEGORY_JUNK,
    /* 0x01 */ ITEM_CATEGORY_LESSER,
    /* 0x02 */ ITEM_CATEGORY_BOSS_KEY,
    /* 0x03 */ ITEM_CATEGORY_SMALL_KEY,
    /* 0x04 */ ITEM_CATEGORY_SKULLTULA_TOKEN,
    /* 0x05 */ ITEM_CATEGORY_MAJOR,
} GetItemCategory;

#define GET_ITEM(itemId, objectId, drawId, textId, field, chestAnim, itemCategory, modIndex, getItemId) \
    { itemId, field, (chestAnim != CHEST_ANIM_SHORT ? 1 : -1) * (drawId + 1), textId, objectId, modIndex, getItemId, drawId, true, ITEM_FROM_NPC, itemCategory, NULL }

#define GET_ITEM_NONE \
    { ITEM_NONE, 0, 0, 0, 0, 0, 0, 0, false, ITEM_FROM_NPC, ITEM_CATEGORY_JUNK, NULL }

typedef struct PlayState PlayState;
typedef struct GetItemEntry GetItemEntry;

typedef void (*CustomDrawFunc)(PlayState* play, GetItemEntry* getItemEntry);

typedef struct GetItemEntry {
    /* 0x00 */ uint16_t itemId;
    /* 0x01 */ uint16_t field; // various bit-packed data
    /* 0x02 */ int16_t gi;     // defines the draw id and chest opening animation
    /* 0x03 */ uint16_t textId;
    /* 0x04 */ uint16_t objectId;
    /* 0x06 */ uint16_t modIndex; // 0 = Vanilla, 1 = Randomizer, future mods will increment up?
    /* 0x08 */ int16_t getItemId;
    /* 0x0A */ uint16_t gid; // Stores the GID value unmodified for future reference.
    /* 0x0C */ uint16_t collectable; // determines whether the item can be collected on the overworld. Will be true in most cases.
    /* 0x0E */ GetItemFrom getItemFrom;
    /* 0x0F */ GetItemCategory getItemCategory; // Primarily made and used for chest size/texture matches contents
    CustomDrawFunc drawFunc;
};                   // size = 0x0F
