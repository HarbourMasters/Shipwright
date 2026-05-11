#include <stdint.h>
#include "item_category_adj.h"
#include "z64item.h"
#include "variables.h"
#include "macros.h"

GetItemCategory Randomizer_AdjustItemCategory(GetItemEntry item) {
    GetItemCategory category = item.getItemCategory;

    // Downgrade bombchus to lesser if the player already has bombchus
    if (INV_CONTENT(ITEM_BOMBCHU) == ITEM_BOMBCHU &&
        ((item.modIndex == MOD_RANDOMIZER && item.getItemId == RG_PROGRESSIVE_BOMBCHU_BAG) ||
         (item.modIndex == MOD_NONE &&
          (item.getItemId == GI_BOMBCHUS_5 || item.getItemId == GI_BOMBCHUS_10 || item.getItemId == GI_BOMBCHUS_20)))) {
        category = ITEM_CATEGORY_LESSER;
    }

    // Downgrade bottles to lesser if the player already has a bottle
    if ((item.modIndex == MOD_RANDOMIZER && item.getItemId >= RG_BOTTLE_WITH_RED_POTION &&
         item.getItemId <= RG_BOTTLE_WITH_POE) ||
        (item.modIndex == MOD_NONE && (item.getItemId == GI_BOTTLE || item.getItemId == GI_MILK_BOTTLE))) {
        if (gSaveContext.inventory.items[SLOT_BOTTLE_1] != ITEM_NONE) {
            category = ITEM_CATEGORY_LESSER;
        }
    }

    return category;
}
