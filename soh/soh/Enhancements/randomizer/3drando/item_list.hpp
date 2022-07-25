#pragma once

#include "item.hpp"
#include "keys.hpp"

void ItemTable_Init();
Item& ItemTable(uint32_t itemKey);
void NewItem(uint32_t itemKey, Item item);
