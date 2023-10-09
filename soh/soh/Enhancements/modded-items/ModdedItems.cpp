#include "ModdedItems.h"
#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
#include "textures/parameter_static/parameter_static.h"
#include "textures/item_name_static/item_name_static.h"
#include "functions.h"
#include "z64save.h"
#include "variables.h"

struct ModdedItemData {
    ModdedItemActionFunc itemAction;
    std::string itemIcon;
    std::string itemNameTexture;
    std::string itemName;
    std::function<s32(void)> currentAmmoGetter;
    std::function<s32(void)> maxAmmoGetter;
};

static std::map<ModdedItem, ModdedItemData> moddedItems;

bool ModdedItems_RegisterModdedItem(s32 modId, s32 itemId, ModdedItemActionFunc itemAction, std::string itemIcon, std::string itemNameTexture, std::string itemName, std::function<s32(void)> currentAmmoGetter, std::function<s32(void)> maxAmmoGetter) {
    if (modId == 0) {
        //can't register a new vanilla item
        return false;
    }

    ModdedItem moddedItem = { modId, itemId };

    if (moddedItems.contains(moddedItem))  {
        //can't overwrite an existing item
        return false;
    }

    ModdedItemData moddedItemData = { itemAction, itemIcon, itemNameTexture, itemName, currentAmmoGetter, maxAmmoGetter };

    moddedItems.insert({ moddedItem, moddedItemData });

    return true;
}

std::vector<ModdedItem> ModdedItems_GetRegisteredModdedItems() {
    std::vector<ModdedItem> items;
    for (std::map<ModdedItem, ModdedItemData>::iterator itr = moddedItems.begin(); itr != moddedItems.end(); ++itr) {
        items.push_back(itr->first);
    }
    
    return items;
}

void ModdedItems_ExecuteModdedItemAction(PlayState* play, Player* player, s32 modId, s32 itemId) {
    ModdedItem moddedItem = { modId, itemId };

    if (moddedItems.contains(moddedItem)) {
        moddedItems[moddedItem].itemAction(play, player, moddedItem);
    }
}

void* ModdedItems_GetModdedItemIcon(s32 modId, s32 itemId) {
    ModdedItem moddedItem = { modId, itemId };

    if (moddedItems.contains(moddedItem)) {
        return (void*)moddedItems[moddedItem].itemIcon.c_str();
    }

	//in case the item is not found
	return (void*)gItemIconSoldOutTex;
}

const char* ModdedItems_GetModdedItemName(s32 modId, s32 itemId) {
    ModdedItem moddedItem = { modId, itemId };

    if (moddedItems.contains(moddedItem)) {
        return moddedItems[moddedItem].itemName.c_str();
    }

	return "[Name not found]";
}

const char* ModdedItems_GetModdedItemNameTexture(s32 modId, s32 itemId, s32 language) {
    ModdedItem moddedItem = { modId, itemId };

    if (moddedItems.contains(moddedItem)) {
        return moddedItems[moddedItem].itemNameTexture.c_str();
    }

	//in case the item is not found
	return gDekuStickItemNameENGTex;
}

s32 ModdedItems_GetCurrentAmmo(s32 modId, s32 itemId) {
    ModdedItem moddedItem = { modId, itemId };

    if (moddedItems.contains(moddedItem)) {
        return moddedItems[moddedItem].currentAmmoGetter();
    }

	//in case the item is not found
    return 0;
}

s32 ModdedItems_GetMaxAmmo(s32 modId, s32 itemId) {
    ModdedItem moddedItem = { modId, itemId };

    if (moddedItems.contains(moddedItem)) {
        return moddedItems[moddedItem].maxAmmoGetter();
    }

	//in case the item is not found
    return -1;
}