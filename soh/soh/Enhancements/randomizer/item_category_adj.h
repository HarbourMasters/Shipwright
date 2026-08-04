#pragma once

#ifndef ITEM_CATEGORY_ADJ_H
#define ITEM_CATEGORY_ADJ_H

#include "../item-tables/ItemTableTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

GetItemCategory Randomizer_AdjustItemCategory(GetItemEntry item);

#ifdef __cplusplus
}

#include "soh/Enhancements/custom-message/CustomMessageManager.h"

// Localized name for an item category, e.g. the label a chest's size and
// texture convey. Shared by the hint and check trackers.
const CustomMessage& ItemCategoryName(GetItemCategory category);
#endif

#endif
