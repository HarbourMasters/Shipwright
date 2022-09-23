#pragma once
#include "item.hpp"
#include "item_location.hpp"

#include <vector>
#include <array>

struct ItemAndPrice {
    Text Name;
    int Price;
    bool Repurchaseable;
};

extern void PlaceVanillaShopItems();
extern std::vector<uint32_t> GetMinVanillaShopItems(int total_replaced);
extern int GetRandomShopPrice();
extern int16_t GetRandomScrubPrice();
extern int GetShopsanityReplaceAmount();
extern Text GetIceTrapName(uint8_t id);
extern int GetShopIndex(uint32_t loc);
extern int TransformShopIndex(int index);

extern std::vector<ItemAndPrice> NonShopItems;
