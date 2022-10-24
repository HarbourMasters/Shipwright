#pragma once

#include "z64item.h"

void ItemUseFromInventory_SetItemAndSlot(ItemID item, InventorySlot slot);
void ItemUseFromInventory_UpdateBottleSlot(ItemID item);

bool ItemUseFromInventory_IsValidItemForUse(GlobalContext* globalCtx);
bool ItemUseFromInventory_BottleWasUsed();
