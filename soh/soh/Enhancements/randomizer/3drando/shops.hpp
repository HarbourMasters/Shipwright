#pragma once
#include "../context.h"

#include <vector>
#include <array>

struct PriceSettingsStruct {
    RandomizerSettingKey main;
    RandomizerSettingKey fixedPrice;
    RandomizerSettingKey range1;
    RandomizerSettingKey range2;
    RandomizerSettingKey noWallet;
    RandomizerSettingKey childWallet;
    RandomizerSettingKey adultWallet;
    RandomizerSettingKey giantWallet;
    RandomizerSettingKey tycoonWallet;
    RandomizerSettingKey affordable;

    PriceSettingsStruct(RandomizerSettingKey _main,
                        RandomizerSettingKey _fixedPrice,
                        RandomizerSettingKey _range1,
                        RandomizerSettingKey _range2,
                        RandomizerSettingKey _noWallet,
                        RandomizerSettingKey _childWallet,
                        RandomizerSettingKey _adultWallet,
                        RandomizerSettingKey _giantWallet,
                        RandomizerSettingKey _tycoonWallet,
                        RandomizerSettingKey _affordable);
};

extern void PlaceVanillaShopItems();
extern std::vector<RandomizerGet> GetMinVanillaShopItems(int total_replaced);
extern uint16_t GetRandomPrice(Rando::Location* loc, PriceSettingsStruct priceSettings);
extern uint16_t GetCheapBalancedPrice();
extern int GetShopsanityReplaceAmount();
extern Text GetIceTrapName(uint8_t id);


