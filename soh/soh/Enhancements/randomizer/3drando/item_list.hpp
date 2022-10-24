#pragma once

#include "item.hpp"
#include "keys.hpp"

void ItemTable_Init();
Item& ItemTable(uint32_t itemKey);
Item& ItemFromGIID(int giid);
void NewItem(uint32_t itemKey, Item item);
std::array<Item, KEY_ENUM_MAX>* GetFullItemTable_();
