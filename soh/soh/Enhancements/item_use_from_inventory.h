#pragma once

#include "z64item.h"

void ItemUseFromInventory_SetItemAndSlot(ItemID item, InventorySlot slot);
void ItemUseFromInventory_UpdateBottleSlot(ItemID item);

bool ItemUseFromInventory_IsValidItemForUse(GlobalContext* globalCtx);
bool ItemUseFromInventory_BottleWasUsed();

ItemID        inventoryUsedItem = ITEM_NONE, inventoryPrevCLeftItem = ITEM_NONE;
InventorySlot inventoryUsedSlot = SLOT_NONE, inventoryPrevCLeftSlot = SLOT_NONE;

bool itemWasUsedFromInventory    = false;
bool usingItemFromInventory      = false;
bool bottleWasUsedFromInventory  = false;
bool swingingBottleFromInventory = false;
