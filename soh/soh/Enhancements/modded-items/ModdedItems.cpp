#include "ModdedItems.h"
#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
#include "textures/parameter_static/parameter_static.h"
#include "textures/item_name_static/item_name_static.h"
#include "functions.h"
#include "z64save.h"
#include "variables.h"

struct ModdedItemFuncs {
    ModdedItemActionFunc itemAction;
    ModdedItemIconGetterFunc iconGetter;
    ModdedItemItemNameGetterFunc itemNameGetter;
};

static std::map<ModdedItem, ModdedItemFuncs> moddedItems;

bool ModdedItems_RegisterModdedItem(s32 modId, s32 itemId, ModdedItemActionFunc itemAction, ModdedItemIconGetterFunc iconGetter, ModdedItemItemNameGetterFunc itemNameGetter) {
    if (modId == 0) {
        //can't register a new vanilla item
        return false;
    }

    ModdedItem moddedItem = { modId, itemId };

    if (moddedItems.contains(moddedItem))  {
        //can't overwrite an existing item
        return false;
    }

    ModdedItemFuncs moddedItemFuncs = { itemAction, iconGetter, itemNameGetter };

    moddedItems.insert({ moddedItem, moddedItemFuncs });

    return true;
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
        return (void*)moddedItems[moddedItem].iconGetter(moddedItem);
    }

	//in case the item is not found
	return (void*)gItemIconDekuStickTex;
}

const char* ModdedItems_GetModdedItemNameTexture(s32 modId, s32 itemId, s32 language) {
    ModdedItem moddedItem = { modId, itemId };

    if (moddedItems.contains(moddedItem)) {
        return moddedItems[moddedItem].itemNameGetter(moddedItem);
    }

	//in case the item is not found
	return gDekuStickItemNameENGTex;
}