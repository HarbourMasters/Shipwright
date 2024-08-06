#pragma once
#include "../context.h"

#include <vector>

struct ItemAndPrice {
    Text Name;
    int Price;
    bool Repurchaseable;
};

extern void PlaceVanillaShopItems();
extern std::vector<RandomizerGet> GetMinVanillaShopItems(int total_replaced);
extern int GetRandomShopPrice();
extern int16_t GetRandomScrubPrice();
extern int GetShopsanityReplaceAmount();
extern Text GetIceTrapName(uint8_t id);
extern int GetShopIndex(RandomizerCheck loc);

extern std::vector<ItemAndPrice> NonShopItems;
