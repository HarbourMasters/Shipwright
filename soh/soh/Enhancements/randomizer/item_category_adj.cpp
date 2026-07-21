#include "item_category_adj.h"
#include "z64item.h"
#include "variables.h"
#include "macros.h"
#include "functions.h"

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

    // Downgrade keys to junk if the player already has skeleton key
    if (category == ITEM_CATEGORY_SMALL_KEY && Flags_GetRandomizerInf(RAND_INF_HAS_SKELETON_KEY)) {
        category = ITEM_CATEGORY_JUNK;
    }

    return category;
}

// Singular, since this names a single check's contents (e.g. what a chest's
// size and texture convey), not a group of them.
const CustomMessage& ItemCategoryName(GetItemCategory category) {
    static const CustomMessage major = CustomMessage("Major Item", "Wichtiger Gegenstand", "Objet majeur");
    static const CustomMessage bossKeys = CustomMessage("Boss Key", "Master-Schlüssel", "Clé d'Or");
    static const CustomMessage smallKeys = CustomMessage("Small Key", "Kleiner Schlüssel", "Petite Clé");
    static const CustomMessage tokens = CustomMessage("Skulltula Token", "Skulltula-Symbol", "Symbole de Skulltula");
    static const CustomMessage hearts = CustomMessage("Heart", "Herz", "Cœur");
    static const CustomMessage lesser = CustomMessage("Lesser Item", "Kleinerer Gegenstand", "Objet mineur");
    static const CustomMessage junk = CustomMessage("Junk Item", "Nutzloser Gegenstand", "Objet inutile");
    switch (category) {
        case ITEM_CATEGORY_MAJOR:
            return major;
        case ITEM_CATEGORY_BOSS_KEY:
            return bossKeys;
        case ITEM_CATEGORY_SMALL_KEY:
            return smallKeys;
        case ITEM_CATEGORY_SKULLTULA_TOKEN:
            return tokens;
        case ITEM_CATEGORY_HEALTH:
            return hearts;
        case ITEM_CATEGORY_LESSER:
            return lesser;
        case ITEM_CATEGORY_JUNK:
        default:
            return junk;
    }
}
