#pragma once
#include "z64.h"

#ifdef __cplusplus

#include <string>
#include <map>
#include <functional>

struct ModdedItem {
    s32 modId;
    s32 itemId;

    bool operator<(const ModdedItem &rhs) const {
        
        if (modId == rhs.modId) {
            return itemId < rhs.itemId;
        }
        return modId < rhs.modId;
    }

    bool operator==(const ModdedItem &rhs) const {
        return modId == rhs.modId && itemId == rhs.itemId;
    }
};

using ModdedItemActionFunc = std::function<void(PlayState*, Player*, ModdedItem)>;

bool ModdedItems_RegisterModdedItem(s32 modId, s32 itemId, ModdedItemActionFunc itemAction, std::string itemIcon, std::string itemNameTexture, std::string itemName);

extern "C" {
#endif

void ModdedItems_ExecuteModdedItemAction(PlayState* play, Player* player, s32 modId, s32 itemId);
void* ModdedItems_GetModdedItemIcon(s32 modId, s32 itemId);
const char* ModdedItems_GetModdedItemNameTexture(s32 modId, s32 itemId, s32 language);
const char* ModdedItems_GetModdedItemName(s32 modId, s32 itemId);

#ifdef __cplusplus
}
#endif