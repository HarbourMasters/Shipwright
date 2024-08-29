#pragma once
#include "../context.h"

#include <vector>
#include <array>

extern void PlaceVanillaShopItems();
extern std::vector<RandomizerGet> GetMinVanillaShopItems(int total_replaced);
extern int GetRandomShopPrice();
extern int16_t GetCheapBalancedPrice();
extern int GetShopsanityReplaceAmount();
extern Text GetIceTrapName(uint8_t id);
extern int GetShopIndex(RandomizerCheck loc);
extern int TransformShopIndex(int index);