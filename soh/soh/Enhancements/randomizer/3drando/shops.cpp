#include "item_pool.hpp"
#include "../location_access.h"
#include "random.hpp"
#include "shops.hpp"
#include "../location.h"

#include <array>
#include <math.h>
#include <map>
#include <spdlog/spdlog.h>
#include "z64item.h"

PriceSettingsStruct::PriceSettingsStruct(RandomizerSettingKey _main, RandomizerSettingKey _fixedPrice,
                                         RandomizerSettingKey _range1, RandomizerSettingKey _range2,
                                         RandomizerSettingKey _noWallet, RandomizerSettingKey _childWallet,
                                         RandomizerSettingKey _adultWallet, RandomizerSettingKey _giantWallet,
                                         RandomizerSettingKey _tycoonWallet, RandomizerSettingKey _affordable) {
    main = _main;
    fixedPrice = _fixedPrice;
    range1 = _range1;
    range2 = _range2;
    noWallet = _noWallet;
    childWallet = _childWallet;
    adultWallet = _adultWallet;
    giantWallet = _giantWallet;
    tycoonWallet = _tycoonWallet;
    affordable = _affordable;
}

// Set vanilla shop item locations before potentially shuffling
void PlaceVanillaShopItems() {
    auto ctx = Rando::Context::GetInstance();
    // Loop to place vanilla items in each location
    for (RandomizerCheck& randomizerCheck : Rando::StaticData::GetShopLocations()) {
        ctx->GetItemLocation(randomizerCheck)->PlaceVanillaItem();
    }
}

// These are the vanilla shop items, but in a priority order of importance
// However many shop item slots were cleared, this will return 64 minus that number of vanilla shop items to be placed
// with assumed fill The first 32 items here will always be present in shops Shopsanity 4 will only have the first 32,
// shopsanity 1 will have the first 56, etc. Shopsanity random will have anywhere from the first 32 to the first 56, so
// the order of items after 32 is relevant
std::vector<RandomizerGet> GetMinVanillaShopItems(int total_replaced) {
    std::vector<RandomizerGet> minShopItems = {
        RG_BUY_DEKU_SHIELD, RG_BUY_HYLIAN_SHIELD, RG_BUY_GORON_TUNIC, RG_BUY_ZORA_TUNIC, RG_BUY_DEKU_NUTS_5,
        RG_BUY_BOMBS_20, RG_BUY_BOMBCHUS_10, RG_BUY_DEKU_STICK_1,
        //^First 8 items: Exist on shopsanity 7 or less
        RG_BUY_FAIRYS_SPIRIT, RG_BUY_DEKU_SEEDS_30, RG_BUY_ARROWS_10, RG_BUY_BLUE_FIRE, RG_BUY_RED_POTION_30,
        RG_BUY_GREEN_POTION, RG_BUY_DEKU_NUTS_10, RG_BUY_BOMBCHUS_10,
        //^First 16 items: Exist on shopsanity 6 or less
        RG_BUY_BOMBCHUS_10, RG_BUY_BOMBCHUS_20, RG_BUY_BOMBS_525, RG_BUY_BOMBS_535, RG_BUY_BOMBS_10, RG_BUY_DEKU_NUTS_5,
        RG_BUY_ARROWS_30, RG_BUY_ARROWS_50,
        //^First 24 items: Exist on shopsanity 5 or less
        RG_BUY_ARROWS_10, RG_BUY_FAIRYS_SPIRIT, RG_BUY_BOTTLE_BUG, RG_BUY_FISH,
        //^First 28 items from OoTR
        RG_BUY_HYLIAN_SHIELD, RG_BUY_BOTTLE_BUG, RG_BUY_DEKU_STICK_1, RG_BUY_DEKU_STICK_1,
        //^First 32 items: Exist on shopsanity 4 or less
        RG_BUY_BLUE_FIRE, RG_BUY_FISH, RG_BUY_BOMBCHUS_10, RG_BUY_DEKU_NUTS_5, RG_BUY_ARROWS_10, RG_BUY_BOMBCHUS_20,
        RG_BUY_BOMBS_535, RG_BUY_RED_POTION_30,
        //^First 40 items: Exist on shopsanity 3 or less
        RG_BUY_BOMBS_30, RG_BUY_BOMBCHUS_20, RG_BUY_DEKU_NUTS_5, RG_BUY_ARROWS_10, RG_BUY_DEKU_NUTS_5, RG_BUY_ARROWS_30,
        RG_BUY_RED_POTION_40, RG_BUY_FISH,
        //^First 48 items: Exist on shopsanity 2 or less
        RG_BUY_BOMBCHUS_20, RG_BUY_ARROWS_30, RG_BUY_RED_POTION_50, RG_BUY_ARROWS_30, RG_BUY_DEKU_NUTS_5,
        RG_BUY_ARROWS_50, RG_BUY_ARROWS_50, RG_BUY_GREEN_POTION,
        //^First 56 items: Exist on shopsanity 1 or less
        RG_BUY_POE, RG_BUY_POE, RG_BUY_HEART, RG_BUY_HEART, RG_BUY_HEART, RG_BUY_HEART, RG_BUY_HEART, RG_BUY_HEART,
        //^All 64 items: Only exist with shopsanity 0
    };
    // Now delete however many items there are to replace
    for (int i = 0; i < total_replaced; i++) {
        minShopItems.pop_back();
    }
    return minShopItems;
}

// This table contains a cumulative probability for each possible shop price based on
//  a beta distribution with alpha = 1.5, beta = 2, and the result of the distribution, a float in [0.0, 1.0),
//  being mutliplied by 60, casted to an integer, then multiplied by 5 to give a value in range [0, 295] in increments
//  of 5. Meaning the first value is the probability of 0, the next value is the probability of 0 plus the probability
//  of 5, etc.
// Probabilities generated using a python script with 1 billion trials, so should hopefully be pretty good
// Average price ~126
//~38% chance of needing no wallet, ~45% chance of needing 1, ~17% chance of needing 2
static constexpr std::array<double, 60> ShopPriceProbability = {
    0.005326994, 0.014908518, 0.027114719, 0.041315285, 0.057136304, 0.074325887, 0.092667151, 0.112002061, 0.132198214,
    0.153125390, 0.174696150, 0.196810540, 0.219388148, 0.242361379, 0.265657012, 0.289205134, 0.312970402, 0.336877590,
    0.360881110, 0.384932772, 0.408976198, 0.432982176, 0.456902494, 0.480686053, 0.504313389, 0.527746488, 0.550938554,
    0.573856910, 0.596465330, 0.618736235, 0.640646600, 0.662162782, 0.683240432, 0.703859801, 0.724001242, 0.743631336,
    0.762722631, 0.781259986, 0.799198449, 0.816521905, 0.833208595, 0.849243398, 0.864579161, 0.879211177, 0.893112051,
    0.906263928, 0.918639420, 0.930222611, 0.940985829, 0.950914731, 0.959992180, 0.968187000, 0.975495390, 0.981884488,
    0.987344345, 0.991851853, 0.995389113, 0.997937921, 0.999481947, 1.000000000,
};

// Generate random number from 5 to wallet max
int GetPriceFromMax(int max) {
    // random range of 1 - wallet max / 5, where wallet max is the highest it goes as a multiple of 5
    return Random(1, max) * 5;
}

uint16_t GetPriceFromSettings(Rando::Location* loc, PriceSettingsStruct priceSettings) {
    auto ctx = Rando::Context::GetInstance();
    switch (ctx->GetOption(priceSettings.main).Get()) {
        case RO_PRICE_VANILLA:
            return loc->GetVanillaPrice();
        case RO_PRICE_CHEAP_BALANCED:
            return GetCheapBalancedPrice();
        case RO_PRICE_BALANCED: {
            double random = RandomDouble(); // Randomly generated probability value
            for (size_t i = 0; i < ShopPriceProbability.size(); i++) {
                if (random < ShopPriceProbability[i]) {
                    // The randomly generated value has surpassed the total probability up to this point, so this is the
                    // generated price i in range [0, 59], output in range [0, 295] in increments of 5
                    return static_cast<uint16_t>(i) * 5;
                }
            }
            return 150;
        }
        case RO_PRICE_FIXED:
            return (uint16_t)ctx->GetOption(priceSettings.fixedPrice).Get() * 5;
        case RO_PRICE_RANGE: {
            uint16_t range1 = (uint16_t)ctx->GetOption(priceSettings.range1).Get() * 5;
            uint16_t range2 = (uint16_t)ctx->GetOption(priceSettings.range2).Get() * 5;
            return range1 < range2 ? Random(range1, range2 + 1) : Random(range2, range1 + 1);
        }
        case RO_PRICE_SET_BY_WALLET: {
            bool isTycoon = ctx->GetOption(RSK_INCLUDE_TYCOON_WALLET).Get();
            uint16_t noWeight = ctx->GetOption(priceSettings.noWallet).Get();
            uint16_t childWeight = ctx->GetOption(priceSettings.childWallet).Get();
            uint16_t adultWeight = ctx->GetOption(priceSettings.adultWallet).Get();
            uint16_t giantWeight = ctx->GetOption(priceSettings.giantWallet).Get();
            uint16_t tycoonWeight = isTycoon ? ctx->GetOption(priceSettings.tycoonWallet).Get() : 0;
            uint16_t totalWeight = noWeight + childWeight + adultWeight + giantWeight + tycoonWeight;
            if (totalWeight == 0) { // if no weight, return from sane range
                return Random(0, 501);
            }
            int16_t selected = Random(1, totalWeight + 1);
            selected = selected - noWeight;
            if (selected <= 0) {
                return 0;
            }
            selected = selected - childWeight;
            if (selected <= 0) {
                return Random(1, 100);
            }
            selected = selected - adultWeight;
            if (selected <= 0) {
                return Random(100, 201);
            }
            selected = selected - giantWeight;
            if (selected <= 0) {
                return Random(201, 501);
            }
            return Random(501, 999);
        }
    }
    SPDLOG_ERROR("GetPriceFromSettings has failed to return a price for location {}, assigning a default value.",
                 loc->GetName());
    assert(false);
    return 69; // this should never happen, if it does, EASTER EGG that tells us something is wrong
}

uint16_t GetRandomPrice(Rando::Location* loc, PriceSettingsStruct priceSettings) {
    uint16_t initialPrice = GetPriceFromSettings(loc, priceSettings);
    auto ctx = Rando::Context::GetInstance();
    if (ctx->GetOption(priceSettings.affordable) && !ctx->GetOption(priceSettings.main).Is(RO_PRICE_FIXED)) {
        if (initialPrice > 500) {
            return 505;
        } else if (initialPrice > 200) {
            return 205;
        } else if (initialPrice > 99) {
            return 100;
        } else if (initialPrice > 0) {
            return 5;
        }
        return 0;
    } else {
        return initialPrice;
    }
}

// Similar to above, beta distribution with alpha = 1, beta = 2,
//  multiplied by 20 instead of 60 to give values in rage [0, 95] in increments of 5
// Average price ~31
static constexpr std::array<double, 20> CheapPriceProbability = {
    0.097500187, 0.190002748, 0.277509301, 0.360018376, 0.437522571, 0.510021715, 0.577520272,
    0.640029304, 0.697527584, 0.750024535, 0.797518749, 0.840011707, 0.877508776, 0.910010904,
    0.937504342, 0.960004661, 0.977502132, 0.989998967, 0.997500116, 1.000000000,
};

uint16_t GetCheapBalancedPrice() {
    double random = RandomDouble();
    for (size_t i = 0; i < CheapPriceProbability.size(); i++) {
        if (random < CheapPriceProbability[i]) {
            // i in range [0, 19], output in range [0, 95] in increments of 5
            return static_cast<uint16_t>(i) * 5;
        }
    }
    return -1;
}

// Get 0 to 7, or a random number from 1-7 depending on shopsanity setting
int GetShopsanityReplaceAmount() {
    auto ctx = Rando::Context::GetInstance();
    if (ctx->GetOption(RSK_SHOPSANITY).Is(RO_SHOPSANITY_OFF)) {
        return 0;
    } else if (ctx->GetOption(RSK_SHOPSANITY).Is(RO_SHOPSANITY_SPECIFIC_COUNT)) {
        if (ctx->GetOption(RSK_SHOPSANITY_COUNT).Is(RO_SHOPSANITY_COUNT_ZERO_ITEMS)) {
            return 0;
        } else if (ctx->GetOption(RSK_SHOPSANITY_COUNT).Is(RO_SHOPSANITY_COUNT_ONE_ITEM)) {
            return 1;
        } else if (ctx->GetOption(RSK_SHOPSANITY_COUNT).Is(RO_SHOPSANITY_COUNT_TWO_ITEMS)) {
            return 2;
        } else if (ctx->GetOption(RSK_SHOPSANITY_COUNT).Is(RO_SHOPSANITY_COUNT_THREE_ITEMS)) {
            return 3;
        } else if (ctx->GetOption(RSK_SHOPSANITY_COUNT).Is(RO_SHOPSANITY_COUNT_FOUR_ITEMS)) {
            return 4;
        } else if (ctx->GetOption(RSK_SHOPSANITY_COUNT).Is(RO_SHOPSANITY_COUNT_FIVE_ITEMS)) {
            return 5;
        } else if (ctx->GetOption(RSK_SHOPSANITY_COUNT).Is(RO_SHOPSANITY_COUNT_SIX_ITEMS)) {
            return 6;
        } else if (ctx->GetOption(RSK_SHOPSANITY_COUNT).Is(RO_SHOPSANITY_COUNT_SEVEN_ITEMS)) {
            return 7;
        } else if (ctx->GetOption(RSK_SHOPSANITY_COUNT).Is(RO_SHOPSANITY_COUNT_EIGHT_ITEMS)) {
            return 8; // temporarily unreachable due to logic limitations
        } else {
            assert(false);
            return 0;
        }
    } else { // Random, get number in [1, 7]
        return Random(1, 8);
    }
}
