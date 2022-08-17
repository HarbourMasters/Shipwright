#pragma once
#ifdef __cplusplus
#include <stdint.h>
#endif

#define CHEST_ANIM_SHORT 0
#define CHEST_ANIM_LONG 1

#define GET_ITEM(itemId, objectId, drawId, textId, field, chestAnim, modIndex, getItemId) \
    { itemId, field, (chestAnim != CHEST_ANIM_SHORT ? 1 : -1) * (drawId + 1), textId, objectId, modIndex, getItemId, drawId, true }

#define GET_ITEM_NONE \
    { ITEM_NONE, 0, 0, 0, 0, 0, 0, 0, false }

typedef struct {
    /* 0x00 */ uint16_t itemId;
    /* 0x01 */ uint16_t field; // various bit-packed data
    /* 0x02 */ int16_t gi;     // defines the draw id and chest opening animation
    /* 0x03 */ uint16_t textId;
    /* 0x04 */ uint16_t objectId;
    /* 0x06 */ uint16_t modIndex; // 0 = Vanilla, 1 = Randomizer, future mods will increment up?
    /* 0x08 */ int16_t getItemId;
    /* 0x0A */ uint16_t gid; // Stores the GID value unmodified for future reference.
    /* 0x0C */ uint16_t collectable; // determines whether the item can be collected on the overworld. Will be true in most cases.
} GetItemEntry;                   // size = 0x0F
