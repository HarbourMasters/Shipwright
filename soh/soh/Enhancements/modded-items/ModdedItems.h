#pragma once
#include "z64.h"

#ifdef __cplusplus

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
using ModdedItemIconGetterFunc = std::function<const char*(ModdedItem)>;
using ModdedItemItemNameGetterFunc = std::function<const char*(ModdedItem)>;

bool ModdedItems_RegisterModdedItem(s32 modId, s32 itemId, ModdedItemActionFunc itemAction, ModdedItemIconGetterFunc iconGetter);

extern "C" {
#endif

void ModdedItems_ExecuteModdedItemAction(PlayState* play, Player* player, s32 modId, s32 itemId);
void* ModdedItems_GetModdedItemIcon(s32 modId, s32 itemId);
const char* ModdedItems_GetModdedItemNameTexture(s32 modId, s32 itemId, s32 language);

#ifdef __cplusplus
}
#endif