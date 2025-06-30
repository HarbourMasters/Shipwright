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

static std::array<std::vector<Text>, 0xF1> trickNameTable; // Table of trick names for ice traps
bool initTrickNames = false; // Indicates if trick ice trap names have been initialized yet

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

/* Initialize the table of trick names with an easy, medium, and hard name for each language.
   english, french, german // spanish */
void InitTrickNames() {
    trickNameTable[RG_KOKIRI_SWORD] = {
        Text{ "Korok Sword", "Épée Korok", "Korok-Schwert" },     // "Espada Korok"
        Text{ "Hero's Sword", "Épée du Héros", "Heroenschwert" }, // "Espada del héroe"
        Text{ "Razor Sword", "Lame Rasoir", "Elfenschwert" },     // "Espada de esmeril"
    };
    trickNameTable[RG_MASTER_SWORD] = {
        Text{ "Goddess Sword", "Épée de la déesse", "Schwert der Göttin" }, // "Espada Divina"
        Text{ "Gilded Sword", "Excalibur", "Schmirgelklinge" },             // "Espada de los Sabios"
        Text{ "Magical Sword", "Lame dorée", "Magisches Schwert" },         // "Fay"
    };
    trickNameTable[RG_GIANTS_KNIFE] = {
        Text{ "Medigoron's Sword", "l'Épée de Medigoron", "Medigoron-Schwert" }, // "La espada de Medigoron"
        Text{ "Razor Sword", "Lame Rasoir", "Elfenschwert" },                    // "Espada de esmeril"
        Text{ "Royal Claymore", "Claymore Royale", "Königs-Zweihänder" },        // "Royal Claymore"
    };
    trickNameTable[RG_BIGGORON_SWORD] = {
        Text{ "Power Sword", "Épée de Puissance", "Schwert der Macht" }, // "Espada de poder",
        Text{ "Fierce Deity Sword", "Épée du dieu démon",
              "Schwert der grimmischen Gottheit" },                              // "Espada de la Fiera Deidad"
        Text{ "Tempered Sword", "Épée de Légende Nv.2", "Master-Schwert Lv.2" }, // "Espada Maestra mejorada"
        Text{ "Biggoron's Knife", "Lame de Grogoron", "Biggoron-Messer" },       // "Daga de Biggoron"
    };
    trickNameTable[RG_DEKU_SHIELD] = {
        Text{ "Boko Shield", "Bouclier Boko", "Bokschild" },        // "Escudo Boko"
        Text{ "Ordon Shield", "Bouclier de Toal", "Ordon-Schild" }, // "Escudo de Ordon"
        Text{ "Wooden Shield", "Bouclier de Bois", "Holzschild" },  // "Escudo de madera"
    };
    trickNameTable[RG_HYLIAN_SHIELD] = {
        Text{ "Hyrule Shield", "Bouclier d'Hyrule", "Hyrule-Schild" },   // "Escudo Hylian"
        Text{ "Goddess Shield", "Bouclier Sacré", "Göttlicher Schild" }, // "Escudo Divino"
        Text{ "Hero's Shield", "Bouclier du Héros", "Heroenschild" },    // "Escudo del héroe"
    };
    trickNameTable[RG_MIRROR_SHIELD] = {
        Text{ "Magic Mirror", "Miroir Magique", "Magiespiegel" },             // "Escudo mágico"
        Text{ "Magical Shield", "Bouclier Magique", "Magischer Schild" },     // "Escudo arcano"
        Text{ "Mirror of Twilight", "Miroir des Ombres", "Schattenspiegel" }, // "Espejo del Crepúsculo"
    };
    trickNameTable[RG_GORON_TUNIC] = {
        Text{ "Gerudo Top", "Tunique Gerudo", "Damengewand" },                  // "Pechera gerudo"
        Text{ "Flamebreaker Armor", "Armure de Pierre", "Anti-Feuer-Rüstung" }, // "Armadura ignífuga"
        Text{ "Red Mail", "Habits Rouges", "Rotes Gewand" },                    // "Ropas rojas"
    };
    trickNameTable[RG_ZORA_TUNIC] = {
        Text{ "Rito Tunic", "Tunique Rito", "Orni-Gewand" },       // "Sayo rito"
        Text{ "Mermaid Suit", "Costume de sirène", "Nixenanzug" }, // "Costume de sirène"
        Text{ "Zora Armor", "Armure Zora", "Zora-Panzer" },        // "Túnica Zora"
        Text{ "Blue Mail", "Habits Bleus", "Blaues Gewand" },      // "Ropas azules"
    };
    trickNameTable[RG_IRON_BOOTS] = {
        Text{ "Iron Hoofs", "Patins de Plomb", "Eisenhufe€" },             // "Botas férreas"
        Text{ "Snow Boots", "Bottes de Neige", "Schneestiefel€" },         // "Botas de nieve"
        Text{ "Red Boots", "Bottes rouges", "Rote Stiefel€" },             // "Botas rojas"
        Text{ "Zora Greaves", "Bottes Zora", "Zora-Beinschutz" },          // "Zora Greaves"
        Text{ "Boots of Power", "Bottes de Puissance", "Stärkestiefel€" }, // "Botas de plomo"
    };
    trickNameTable[RG_HOVER_BOOTS] = {
        Text{ "Hover Hoofs", "Patins des airs", "Gleithufe€" },         // "Botas flotadoras"
        Text{ "Golden Boots", "Bottes dorées", "Goldstiefel€" },        // "Botas de Oro"
        Text{ "Pegasus Boots", "Bottes pégase", "Pegasus-Stiefel€" },   // "Botas de Pegaso"
        Text{ "Boots of Speed", "Bottes de vitesse", "Tempostiefel€" }, // "Botas del desierto"
    };
    trickNameTable[RG_WEIRD_EGG] = {
        Text{ "Poached Egg", "Oeuf à la coque", "Spiegelei" }, // "Huevo pasado"
        Text{ "Lon Lon Egg", "Oeuf Lon Lon", "Lon Lon-Ei" },   // "Huevo Lon Lon"
        Text{ "Zora Egg", "Oeuf Zora", "Zora-Ei" },            // "Huevo Zora"
    };
    trickNameTable[RG_ZELDAS_LETTER] = {
        Text{ "Ruto's Letter", "Lettre de Ruto", "Rutos Brief" },                            // "Carta de Ruto"
        Text{ "Royal Letter", "Lettre Eoyale", "Royaler Brief" },                            // "Carta para Kafei"
        Text{ "Zelda's Business Card", "Carte d'affaires de Zelda", "Zeldas Visitenkarte" }, // "Carta"
        Text{ "Letter to Kafei", "Lettre pour Kafei", "Brief an Kafei" },                    // "Carta para Kafei"
        Text{ "Goat's Letter", "Lettre de la Chèvre", "Brief der Ziege" },                   // "Carta de la Cabra"
        Text{ "Maggie's Letter", "Lettre de Maggy", "Dolores' Brief" },                      // "Carta de Dolores"
    };
    trickNameTable[RG_BOOMERANG] = {
        Text{ "Banana", "Banane", "Banane" },                                 // "Plátano"
        Text{ "Prank Fetch Toy", "Inséparable Bâtonnet", "Boomerang" },       // "Bumerang"
        Text{ "Gale Boomerang", "Boomerang Tornade", "Sturmbumerang" },       // "Bumerán tornado"
        Text{ "Magic Boomerang", "Boomerang Magique", "Magischer Bumerang" }, // "Bumerán mágico"
    };
    trickNameTable[RG_LENS_OF_TRUTH] = {
        Text{ "Sheikah-leidoscope", "Sheikah-léidoscope", "Shiekah-leidoskop" }, // "Monóculo de la Verdad"
        Text{ "Sheikah Sensor", "Sonar Sheikah", "Shiekah-Sensor" },             // "Sensor Sheikah"
        Text{ "Crystal of Vision", "Cristal de Vision", "Kristall der Sicht" },  // "Cristal de Visión"
        Text{ "Magnifying Lens", "Loupe", "Magische Lupe" },                     // "Lente Aumentadora"
    };
    trickNameTable[RG_MEGATON_HAMMER] = {
        Text{ "Goron Gavel", "Masse Perforatrice", "Goronen-Hammer" }, // "Mazo Goron"
        Text{ "Magic Hammer", "Marteau Magique", "Magischer Hammer" }, // "Martillo mágico"
        Text{ "Skull Hammer", "Maillet Ressort", "Schädel-Hammer" },   // "Martillo de hierro"
    };
    trickNameTable[RG_STONE_OF_AGONY] = {
        Text{ "Cave Charm", "Charme de grotte", "Höhlenamulett" },                 // "Amuleto de la cueva"
        Text{ "Stone of Agahnim", "Fragment d'Agahnim", "Stein des Agahnim" },     // "Piedra de Agahnim"
        Text{ "Shard of Agony", "Fragment de Souffrance", "Scherbe des Wissens" }, // "Piedra de la Agonía"
        Text{ "Pirate's Charm", "Pierre de Pirate", "Links Talisman" },            // "Amuleto Pirata"
    };
    trickNameTable[RG_DINS_FIRE] = {
        Text{ "Eldin's Fire", "Feu d'Eldin", "Eldins Feuerinferno" },           // "Fuego de Eldin"
        Text{ "Din's Blaze", "Flamme de Din", "Dins Flammen" },                 // "Poder de Din"
        Text{ "Magic Lantern", "Lanterne Magique", "Magische Laterne" },        // "Linterna mágica"
        Text{ "Ether Medallion", "Médaillon d'Éther", "Luft-Medaillon" },       // "Medallón de Tesoro"
        Text{ "Bombos Medallion", "Médaillon des Flammes", "Feuer-Medaillon" }, // "Medallón del Temblor"
    };
    trickNameTable[RG_FARORES_WIND] = {
        Text{ "Faron's Wind", "Vent de Firone", "Phirones Donnersturm" },              // "Viento de Farone"
        Text{ "Farore's Windfall", "Zéphyr de Farore", "Farores Wind" },               // "Valor de Farore"
        Text{ "Tingle Air", "Tingle de aire", "Tingle Luft" },                         // "Tingle Air"
        Text{ "Travel Medallion", "Amulette de téléportation", "Teleport-Medaillon" }, // "Medallón Maligno"
        Text{ "Irene's Taxi", "Le taxi d'Aëline", "Irenes Taxi" },                     // "El taxi de Airín"
    };
    trickNameTable[RG_NAYRUS_LOVE] = {
        Text{ "Lanayru's Love", "Amour de Lanelle", "Ranelles Umarmung" }, // "Amor de Lanayru"
        Text{ "Nayru's Passion", "Passion de Nayru", "Nayrus Hingabe" },   // "Sabiduría de Nayru"
        Text{ "Tingle Shield", "Bouclier Tingle", "Tingleschild" },        // "Escudo de hormigueo"
        Text{ "Shield Spell", "Bouclier Magique", "Schildzauber" },        // "Hechizo de Protección"
        Text{ "Magic Armor", "Armure Magique", "Magie-Rüstung" },          // "Armadura mágica"
    };
    trickNameTable[RG_FIRE_ARROWS] = {
        Text{ "Fire Rod", "Baguette de feu", "Feuerstab" },     // "Cetro de fuego"
        Text{ "Bomb Arrow", "Flèche-Bombe", "Bomben-Pfeile€" }, // "Flecha bomba"
        Text{ "Red Candle", "Bougie Rouge", "Rote Kerze" },     // "Vela roja"
    };
    trickNameTable[RG_ICE_ARROWS] = {
        Text{ "Ice Rod", "Baguette des Glaces", "Eisstab" },                       // "Cetro de Hielo"
        Text{ "Ancient Arrow", "Flèche Archéonique", "Antike Pfeile€" },           // "Flecha ancestral"
        Text{ "Ice Trap Arrow", "Flèche de Piège de Glace", "Eisfallen-Pfeile€" }, // "Cetro de hielo"
    };
    trickNameTable[RG_LIGHT_ARROWS] = {
        Text{ "Wind Arrow", "Flèche de Vent", "Wind-Pfeile€" },                     // "Flecha del Viento"
        Text{ "Wand of Gamelon", "Baguette de Gamelon", "Zauberstab von Gamelon" }, // "Varita de Gamelón"
        Text{ "Shock Arrow", "Flèches Électriques", "Elektro-Pfeile€" },            // "Flecha eléctrica"
        Text{ "Silver Arrow", "Flèches d'Argent", "Silber-Pfeile€" },               // "Flecha de plata"
    };
    trickNameTable[RG_GERUDO_MEMBERSHIP_CARD] = {
        Text{ "Desert Title Deed", "Abonnement Gerudo", "Wüsten-Urkunde" }, // "Escritura del desierto"
        Text{ "Sickle Moon Flag", "Drapeau du croissant de lune",
              "Mondsichel-Fahne" },                                           // "Bandera de la Luna Creciente"
        Text{ "Complimentary ID", "Bon de félicitation", "Laudatio-Schein" }, // "Cupón especial"
        Text{ "Gerudo's Card", "Carte Goron", "Gerudo-Symbol" },              // "Tóken Gerudo"
        Text{ "Gerudo's Membership Card", "Autographe de Nabooru", "Gerudo-Mitgliedskarte" }, // "Tarjeta Gerudo"
    };

    trickNameTable[RG_MAGIC_BEAN_PACK] = {
        Text{ "Funky Bean Pack", "Paquet de Fèves Magiques", "Wunderbohnen-Packung" },    // "Lote de frijoles mágicos"
        Text{ "Grapple Berries", "Baies de grappin", "Grapple-Beeren€" },                 // "Bayas de garfio"
        Text{ "Crenel Bean Pack", "Paquet de Haricots Gonggle", "Gongolerbsen-Packung" }, // "Lote de alubias mágicas"
        Text{ "Mystical Seed Pack", "Pack de graines mystiques", "Saatbeutel" }, // "Paquete de semillas místicas"
    };
    trickNameTable[RG_DOUBLE_DEFENSE] = {
        Text{ "Diamond Hearts", "Coeurs de Diamant", "Diamantherzen€" },            // "Contenedor de diamante"
        Text{ "Double Damage", "Double Souffrance", "Doppelte Angriffskraft" },     // "Doble daño receptivo"
        Text{ "Quadruple Defence", "Quadruple Défence", "Vierfache Verteidigung" }, // "Defensa cuádruple"
    };

    trickNameTable[RG_POCKET_EGG] = {
        Text{ "Arpagos Egg", "Oeuf d'Arpagos", "Arpagos-Ei" }, // "Huevo de Arpagos"
        Text{ "Lon Lon Egg", "oeuf Lon Lon", "Lon Lon-Ei" },   // "Huevo Lon Lon"
        Text{ "Zora Egg", "oeuf Zora", "Zora-Ei" },            // "Huevo del Pez Viento"
    };
    trickNameTable[RG_POCKET_EGG] = {
        Text{ "D.I.Y. Alarm Clock", "Réveille-matin improvisé",
              "Improvisierter Wecker" },                                 // "Alarma emplumada portátil"
        Text{ "Kakariko Cucco", "Cocotte Cocorico", "Kakariko-Huhn" },   // "Cuco de Kakariko"
        Text{ "Hatched Cucco", "Cocotte éclose", "Geschlüpftes Küken" }, // "Pollo de bolsillo"
    };
    trickNameTable[RG_COJIRO] = {
        Text{ "Blucco", "Chair-Qui-Poule", "Blenni" },           // "Cucazul"
        Text{ "Piyoko", "Piyoko", "Piyoko" },                    // "Piyoko"
        Text{ "Dark Cucco", "Cocotte Sombre", "Dunkles Huhn" },  // "Cucco oscuro"
        Text{ "Grog's Cucco", "Cocotte de Grog", "Grogs Huhn" }, // "Cuco de Grog"
    };
    trickNameTable[RG_ODD_MUSHROOM] = {
        Text{ "Magic Mushroom", "Champignon magique", "Magischer Pilz" },  // "Champiñón mágico"
        Text{ "Endura Shroom", "Champi Vigueur", "Fittling" },             // "Champiñón del bosque"
        Text{ "Sleepy Toadstool", "Crapaud Fatigué", "Schlummermorchel" }, // "Seta durmiente"
        Text{ "Mushroom", "Champignon", "Pilz" },                          // "Seta"
    };
    trickNameTable[RG_ODD_POTION] = {
        Text{ "Odd Medicine", "Élixir suspect", "Moderpaket" },               // "Poción rara"
        Text{ "Granny's Poultice", "Mixture de Granny", "Omas Mixtur" },      // "Medicina de la abuela"
        Text{ "Mushroom Poultice", "Mixture de champignon", "Pilzumschlag" }, // "Medicina de champiñones"
        Text{ "Secret Medicine", "Médicament", "Geheime Medizin" },           // "Pócima secreta"
        Text{ "Mushroom Spores", "Spores de Champignons", "Pilzsporen" },     // "Esporas de hongos"
        Text{ "Hanyu Spore", "Hanyu Spore", "Hanyu-Sporen" },                 // "Espora Hanyu"
    };
    trickNameTable[RG_POACHERS_SAW] = {
        Text{ "Carpenter's Saw", "Scie du charpentier", "Zimmermannssäge" },      // "Sierra del carpintero"
        Text{ "Poacher's Sword", "Hache du chasseur", "Wildererschwert" },        // "Espada del capataz"
        Text{ "Ancient Bladesaw", "Longue Épée Archéonique", "Antike Großsäge" }, // "Mandoble ancestral"
        Text{ "Woodcutter's Axe", "Hache du Bûcheron", "Holzfälleraxt" },         // "Hacha de leñador"
        Text{ "Grog's Saw", "Scie de Grog", "Grogs Säge" },                       // "Sierra del Cazador Furtivo"
    };
    trickNameTable[RG_BROKEN_SWORD] = {
        Text{ "Broken Biggoron's Sword", "Épée brisée de Grogoron",
              "zerbr. Biggoron-Schwert" },                                                // "Espada de Biggoron rota"
        Text{ "Broken Giant's Knife", "Lame des Géants brisée", "zerbr. Langschwert" },   // "Daga gigante rota"
        Text{ "Broken Noble Sword", "Épée noble brisée", "zerbr. Edelschwert" },          // "Espada noble rota"
        Text{ "Broken Picori Blade", "Épée Minish brisée", "zerbr. Schwert der Minish" }, // "Espada minish rota"
        Text{ "Decayed Master Sword", "Épée de légende pourrie",
              "zerfr. Master-Schwert" }, // "Espada decadente de leyenda"
    };
    trickNameTable[RG_PRESCRIPTION] = {
        Text{ "Biggoron's Prescription", "Ordonnance de Grogoron", "Biggorons Rezept" }, // "Receta de Biggoron"
        Text{ "Eyedrop Prescription", "Ordonnance de gouttes", "Augentropfen-Rezept" },  // "Receta ocular"
        Text{ "Urgent Prescription", "Ordonnance urgente", "Dringendes Rezept" },        // "Prescripción"
        Text{ "Swordsman's Scroll", "Précis d'escrime", "Schwert-Rolle" },               // "Esgrimidorium"
        Text{ "Portrait of Oren", "Portrait d'Orlène", "Portrait von Oreen" },           // "Retrato de Oren"
        Text{ "Letter to King Zora", "Lettre au roi Zora", "Brief an König Zora" },      // "Carta al Rey Zora"
    };
    trickNameTable[RG_EYEBALL_FROG] = {
        Text{ "Don Gero", "Don Gero", "Don Gero" },                             // "Don Gero"
        Text{ "Hot-Footed Frog", "Grenouille à pieds chauds", "Spurtkröte" },   // "Rana de patas calientes"
        Text{ "Lost Swordsmith", "Forgeron perdu", "Vermisster Schmied" },      // "Espadachín perdido"
        Text{ "Eyedrop Frog", "Grenouille-qui-louche", "Augentropfen-Frosch" }, // "Globo Ocular de Rana"
    };
    trickNameTable[RG_EYEDROPS] = {
        Text{ "Biggoron's Eyedrops", "Gouttes de Grogoron", "Biggorons Augentropfen" },       // "Gotas de Biggoron"
        Text{ "Hyrule's Finest Eyedrops", "Eau du Lac Hylia", "Hyrules beste Augentropfen" }, // "Gotas oculares"
        Text{ "Moon's Tear", "Larme de Lune", "Mondträne" },                                  // "Lágrima de Luna"
        Text{ "Engine Grease", "Graisse moteur", "Schmierfett" },                             // "Grasa del motor"
        Text{ "Zora Perfume", "Parfum Zora", "Zora-Parfüm" },                                 // "Perfume Zora"
    };
    trickNameTable[RG_CLAIM_CHECK] = {
        Text{ "Clay Check", "Certificat Grogoron", "Berg-Urkunde" },      // "Comprobant e de Reclamación"
        Text{ "Ancient Tablet", "Stèle ancienne", "Alte Steintafel" },    // "Litografía arcana"
        Text{ "Sheikah Slate", "Tablette Sheikah", "Shiekah-Stein" },     // "Piedra Sheikah"
        Text{ "Cyclone Slate", "Ardoise des tornades", "Froschkompass" }, // "Pizarra de los Torbellinos"
    };

    trickNameTable[RG_GOLD_SKULLTULA_TOKEN] = {
        Text{ "Skulltula Token", "Bon de Skulltula dorée", "Skulltula-Beweis" },          // "Símbolo de Skulltula"
        Text{ "Golden Skulltula Spirit", "Pièce de Skulltula dorée", "Skulltula-Seele" }, // "Tóken de Skulltula Dorada"
        Text{ "Gold Walltula Token", "Jeton de Walltula dorée", "Walltula-Symbol" },      // "Skulltula dorada"
        Text{ "Maiamai", "Ti'gorneau", "Maimai" },                                        // "Maimai"
        Text{ "Gratitude Crystal", "Cristal de gratitude", "Juwel der Güte" },            // "Gema de gratitud"
        Text{ "Korok Seed", "Noix korogu", "Krog-Samen" },                                // "Semilla de kolog"
    };

    trickNameTable[RG_PROGRESSIVE_HOOKSHOT] = {
        Text{ "Progressive Grappling Hook", "Lance-chaîne (prog.)", "Enterhaken (prog.)" }, // "Garra progresiva"
        Text{ "Progressive Clawshot", "Grappin-griffe (prog.)", "Greifhaken (prog.)" },     // "Zarpa progresiva"
        Text{ "Progressive Gripshot", "Grappince (prog.)", "Tauschhaken (prog.)" },         // "Enganchador progresivo"
        Text{ "Progressive Rope", "Corde (prog.)", "Giftwurm (prog.)" },                    // "Cuerda progresivo"
    };
    trickNameTable[RG_PROGRESSIVE_STRENGTH] = {
        Text{ "Power Glove", "Gant de Puissance (prog.)", "Krafthandschuh (prog.)" },  // "Guanteletes progresivos"
        Text{ "Power Bracelet", "Bracelet de Force (prog.)", "Kraftarmband (prog.)" }, // "Brasaletes progresivos"
        Text{ "Magic Bracelet", "Bracelet Magique (prog.)", "Magiearmband (prog.)" },  // "Manoplas progresivas"
    };
    trickNameTable[RG_PROGRESSIVE_BOMB_BAG] = {
        Text{ "Progressive Bomb Capacity", "Capacité de bombes (prog.)",
              "Bomben-Kapazität (prog.)" }, // "Mayor capacidad de bombas"
        Text{ "Progressive Bomb Pack", "Paquet de bombes (prog.)",
              "Bombenrucksack (prog.)" }, // "Zurrón de bombas progresivo"
        Text{ "Progressive Bomb Box", "Boîte à bombes (prog.)",
              "Bombenschachtel (prog.)" }, // "Bolsa de bombas progresiva"
        Text{ "Progressive Blast Mask", "Masque d'Explosion (prog.)",
              "Ka-Bumm-Maske (prog.)" }, // "Máscara explosiva progresiva"
        Text{ "Progressive Powder Kegs", "Baril de Poudre (prog.)",
              "Pulverfass (prog.)" }, // "Barril de polvo progresivo"
        Text{ "Progressive Remote Bombs", "Bombes à distance (prog.)",
              "Fernzünderbomben (prog.)€" }, // "Bombas remotas progresivas"
    };
    trickNameTable[RG_PROGRESSIVE_BOW] = {
        Text{ "Progressive Arrow Capacity", "Capacité de flèches (prog.)",
              "Pfeil-Kapazität (prog.)" },                                               // "Mayor capacidad de flechas"
        Text{ "Progressive Hero's Bow", "Arc du héros (prog.)", "Heroenbogen (prog.)" }, // "Arco del héroe progresivo"
        Text{ "Progressive Arrow Holder", "Arbalète (prog.)", "Köcher (prog.)" },        // "Ballesta progresiva"
        Text{ "Progressive Crossbow", "Arbalète (prog.)", "Armbrust (prog.)" },          // "Ballesta progresiva"
        Text{ "Progressive Sacred Bow", "Arc sacré (prog)", "Heiliger Bogen (prog.)" },  // "Arco Sagrado Progresivo"
        Text{ "Progressive Lynel Bow", "Arc de Lynel (prog.)",
              "Leunenbogen (prog.)" }, // "Arco de centaleón Progresivo"
    };
    trickNameTable[RG_PROGRESSIVE_SLINGSHOT] = {
        Text{ "Progressive Seed Capacity", "Capacité de graines (prog.)",
              "Kern-Kapazität (prog.)" },                                        // "Mayor capacidad de semillas"
        Text{ "Progressive Catapult", "Catapulte (prog.)", "Katapult (prog.)" }, // "Catapulta progresiva"
        Text{ "Progressive Scattershot", "Lance-Pierre rafale (prog.)",
              "Streuschleuder (prog.)" }, // "Resortera múltiple progresiva"
        Text{ "Progressive Seed Launcher", "Lanceur de semences (prog.)",
              "Blasrohr (prog.)" }, // "Lanzador de semillas progresivo"
        Text{ "Progressive Seed Satchel", "Sac de graines (prog.)",
              "Saatbeutel (prog.)" }, // "Bolsa de semillas progresiva"
    };
    trickNameTable[RG_PROGRESSIVE_WALLET] = {
        Text{ "Progressive Rupee Capacity", "Capacité de rubis (prog.)",
              "Rubin-Kapazität (prog.)" },                                             // "Mayor capacidad de rupias"
        Text{ "Progressive Purse", "Sacoche (prog.)", "Portemonnaie (prog.)" },        // "Cartera de rupias progresiva"
        Text{ "Progressive Rupee Bag", "Sac à rubis (prog.)", "Rubintasche (prog.)" }, // "Zurrón de rupias progresivo"
        Text{ "Progressive Rupoor Capacity", "Capacité de Roupir (prog.)",
              "Rubinfalle (prog.)" }, // "Capacidad progresiva Rupobre"
        Text{ "Progressive Spoils Bag", "Sac à Butin (prog.)", "Beutetasche (prog.)" }, // "Bolsa de trofeos progresiva"
        Text{ "Progressive Ruby Bag", "Capacité du sac Ruby (prog.)", "Rubytasche (prog.)" }, // "Bolso Ruby progresivo"
    };
    trickNameTable[RG_PROGRESSIVE_SCALE] = {
        Text{ "Progressive Flippers", "Palmes de Zora (prog.)",
              "Schwimmflossen (prog.)€" }, // "Aletas de zora progresiva"
        Text{ "Progressive Dragon's Scale", "Écaille du dragon d'eau (prog.)",
              "Drachen-Schuppe (prog.)" }, // "Escama dragón acuático progresiva"
        Text{ "Progressive Diving Ability", "Plongée (prog.)", "Tauchfähigkeit (prog.)" }, // "Buceo progresivo"
        Text{ "Progressive Pearl", "Perle (prog.)", "Perle (prog.)" },                     // "Perla progresiva"
        Text{ "Progressive Scute", "Bulle (prog.)", "Schuppe (prog.)" }, // "Fragmento Zora progresivo"
    };
    trickNameTable[RG_PROGRESSIVE_NUT_UPGRADE] = {
        Text{ "Progressive Nut Pack", "Paquet de noix (prog.)",
              "Nußrucksack (prog.)" },                                                  // "Mayor capacidad de semillas"
        Text{ "Progressive Bait Bag", "Sac à Appâts (prog.)", "Futtertasche (prog.)" }, // "Bolsa de cebo progresiva"
        Text{ "Progressive Pear Capacity", "Capacité de poire (prog.)",
              "Putput-Kapazität (prog.)" },                                        // "Capacidad progresiva de pera"
        Text{ "Progressive Nut Bag", "Sac de noix (prog.)", "Nußbeutel (prog.)" }, // "Bolsa de nueces progresiva"
        Text{ "Progressive Husk Capacity", "Capacité de noisettes (prog.)",
              "Schalen-Kapazität (prog.)€" }, // "Mayor capacidad de castañas"
    };
    trickNameTable[RG_PROGRESSIVE_STICK_UPGRADE] = {
        Text{ "Progressive Stick Bag", "Sac de bâtons (prog.)",
              "Stabbeutel (prog.)" }, // "Mayor capacidad de ramas deku"
        Text{ "Progressive Stick Pack", "Paquet de bâtons Mojo (prog.)",
              "Stabrucksack (prog.)" }, // "Mayor capacidad de bastones"
        Text{ "Progressive Branch Capacity", "Capacité de la succursale (prog.)",
              "Ast-Kapazität (prog.)" }, // "Capacidad progresiva de la sucursal"
        Text{ "Progressive Rod Capacity", "Capacité de tiges (prog.)",
              "Stock-Kapazität (prog.)" }, // "Mayor capacidad de cetros deku"
    };
    trickNameTable[RG_PROGRESSIVE_BOMBCHUS] = {
        Text{ "Progressive Bomblings", "Bombinsectes (prog.)", "Bombenmäuse (prog.)€" }, // "Bombinsectos progresivos"
        Text{ "Progressive Sentrobe Bombs", "Bombe de Sphérodrone (prog.)",
              "Rokopterbomben (prog.)€" },                                          // "Bomba de helicobot progresivo"
        Text{ "Progressive Bomb-ombs", "Bombe Soldat (prog.)", "Bob-omb (prog.)" }, // "Soldado bomba progresivo"
        Text{ "Progressive Missiles", "Missiles (prog.)", "Missiles (prog.)€" },    // "Misiles progresivos"
        Text{ "Progressive Bombchu Bag", "Sac à Bombchu (prog.)",
              "Krabbelminenbeutel (prog.)" }, // "Bombachus progresivos"
    };
    trickNameTable[RG_PROGRESSIVE_MAGIC_METER] = {
        Text{ "Progressive Stamina Meter", "Jauge d'endurance (prog.)",
              "Ausdauer (prog.)" }, // "Medidor de vigor progresivo"
        Text{ "Progressive Energy Gauge", "Jauge d'énergie (prog.)",
              "Energie (prog.)" }, // "Medidor de energía progresivo"
        Text{ "Progressive Magic Powder", "Poudre magique (prog.)",
              "Zauberpulver (prog.)" }, // "Medidor de carga progresivo"
    };
    trickNameTable[RG_PROGRESSIVE_OCARINA] = {
        Text{ "Progressive Memento", "Souvenir (prog.)", "Souvenir (prog.)" },   // "Silbato progresivo"
        Text{ "Progressive Whistle", "Siffler (prog.)", "Pfeife (prog.)" },      // "Silbido progresivo"
        Text{ "Progressive Flute", "Flûte (prog.)", "Flöte (prog.)" },           // "Flauta progresiva"
        Text{ "Progressive Recorder", "Harmonica (prog.)", "Rekorder (prog.)" }, // "Armónica progresiva"
    };
    trickNameTable[RG_PROGRESSIVE_GORONSWORD] = {
        Text{ "Progressive Titan Blade", "Lame des Titans (prog.)",
              "Titanenklinge (prog.)" }, // "Hoja del Titán progresiva"
        Text{ "Progressive Goron Knife", "Lame Goron (prog.)", "Goronenmesser (prog.)" }, // "Daga Goron progresiva"
        Text{ "Progressive Giant Sword", "Épée géante (prog.)",
              "Riesenschwert (prog.)" }, // "Espada gigante progresiva"
        Text{ "Progressive Darknut Sword", "Épée de Darknut (prog.)",
              "Gardeschwert (prog.)" }, // "Espada Darknut progresiva"
        Text{ "Progressive Power Sword", "Épée de Puissance (prog.)",
              "Schwert der Macht (prog.)" }, // "Espada de poder progresiva"
        Text{ "Progressive Big Stabby", "Gros coup de poignard (prog.)",
              "Großschwert (prog.)" }, // "Gran puñalada progresiva"
    };

    trickNameTable[RG_EMPTY_BOTTLE] = {
        Text{ "Empty Canteen", "Cantine vide", "Leere Feldflasche" },    // "cantimplora vacía"
        Text{ "Vial of Winds", "Fiole de vents", "Phiole der Winde" },   // "Vial de Vientos"
        Text{ "Tingle Bottle", "Flacon de Tingle", "Tingle-Flasche" },   // "Botella de Tingle"
        Text{ "Magic Bottle", "Flacon magique", "Magische Flasche" },    // "Frasco feérico"
        Text{ "Glass Bottle", "Flacon de verre", "Glasflasche" },        // "Botella de cristal"
        Text{ "Bottle with Water", "Flacon d'eau", "Flasche (Wasser)" }, // "Botella Tingle"
    };
    trickNameTable[RG_BOTTLE_WITH_MILK] = {
        Text{ "Bottle with Chateau Romani", "Flacon de cuvée Romani",
              "Flasche (Chateau Romani)" }, // "Botella de Reserva Romani"
        Text{ "Bottle with Premium Milk", "Flacon avec lait de qualité supérieure",
              "Flasche (Vollmilch)" }, // "Biberón con leche Premium"
        Text{ "Bottle with Mystery Milk", "Flacon de lait grand cru",
              "Flasche (Verdächtige Milch)" }, // "Botella de leche extra"
        Text{ "Bottle with Fresh Milk", "Flacon de lait frais",
              "Flasche (Frische Milch)" }, // "Botella de leche fresca"
    };
    trickNameTable[RG_BOTTLE_WITH_RED_POTION] = {
        Text{ "Bottle with Red Chu Jelly", "Flacon de gelée Chuchu rouge",
              "Flasche (Rotes Schleim-Gelee)" }, // "Jugo de Chuchu Rojo"
        Text{ "Bottle with Hibiscus Potion", "Flacon de potion de Hibiscus",
              "Flasche (Hibiskus-Trank)" }, // "Botella de poción de Hibisco"
        Text{ "Bottle with Medicine of Life", "Flacon d'élixir rouge",
              "Flasche (Roter Trank)" }, // "Botella de medicina de la vida"
        Text{ "Bottle with Heart Potion", "Flacon de potion de soin",
              "Flasche (Herztrank)" }, // "Botella de poción de salud"
    };
    trickNameTable[RG_BOTTLE_WITH_GREEN_POTION] = {
        Text{ "Bottle with Green Chu Jelly", "Flacon de gelée Chuchu verte",
              "Flasche (Grünes Schleim-Gelee)" }, // "Jugo de Chuchu Verde"
        Text{ "Bottle with Lamp Oil", "Flacon de Huile à lanterne",
              "Flasche (Lampenöl)" }, // "Botella de Aceite de candil"
        Text{ "Bottle with Medicine of Magic", "Flacon d'élixir vert",
              "Flasche (Grüner Trank)" }, // "Botella de medicina mágica"
        Text{ "Bottle with Stamina Potion", "Flacon d'Endurol",
              "Flasche (Ausdauertrank)" }, // "Botella de elixir vigorizante"
    };
    trickNameTable[RG_BOTTLE_WITH_BLUE_POTION] = {
        Text{ "Bottle with Blue Chu Jelly", "Flacon de gelée Chuchu bleue",
              "Flasche (Blaues Schleim-Gelee)" }, // "Jugo de Chuchu Azul"
        Text{ "Bottle with Water of Life", "Flacon d'élixir bleu",
              "Flasche (Blauer Trank)" }, // "Botella de agua de la vida"
        Text{ "Bottle with Air Potion", "Flacon de potion d'oxygène", "Flasche (Lufttrank)" }, // "Botella de oxígeno"
    };
    trickNameTable[RG_BOTTLE_WITH_FAIRY] = {
        Text{ "Bottle with Forest Firefly", "Flacon avec une luciole",
              "Flasche (Glühwürmchen)" }, // "Luciérnaga del bosque"
        Text{ "Bottle with Deku Princess", "Flacon avec Deku Princess",
              "Flasche (Deku-Prinzessin)" }, // "Botella con Deku Princess"
        Text{ "Bottle with Stray Fairy", "Flacon avec une fée perdue",
              "Flasche (Verirrte Fee)" }, // "Hada perdida en una botella"
    };
    trickNameTable[RG_BOTTLE_WITH_FISH] = {
        Text{ "Bottle with Small Jabu-Jabu", "Flacon avec mini Jabu-Jabu",
              "Flasche (Baby Jabu)" }, // "Lord Chapu-Chapu embotellado"
        Text{ "Bottle with Reekfish", "Flacon avec Reekfish", "Flasche (Dunstforelle)" }, // "Reekfish embotellada"
        Text{ "Bottle with Hyrule Bass", "Flacon avec perche d'Hyrule",
              "Flasche (Hyrulebarsch)" }, // "Locha de Hyrule embotellada"
        Text{ "Bottle with Hyrule Loach", "Flacon avec loche d'Hyrule",
              "Flasche (Hylianische Forelle)" }, // "Perca de Términa embotellada"
    };
    trickNameTable[RG_BOTTLE_WITH_BLUE_FIRE] = {
        Text{ "Bottle with Will-O-Wisp", "Flacon avec feu follet", "Flasche (Rotes Feuer)" }, // "Botella de llama azul"
        Text{ "Bottle with Ancient Flame", "Flacon de flamme ancienne",
              "Flasche (Antike Flamme)" }, // "Botella de fuego ancestral"
        Text{ "Bottle with a Blue Candle", "Flacon avec une bougie bleue",
              "Flasche (Blaue Kerze)" },                                               // "Botella con una vela azul"
        Text{ "Bottle with Red Ice", "Flacon de Glace Rouge", "Flasche (Rotes Eis)" }, // "Botella de Hielo rojo"
        Text{ "Bottle with Nayru's Flame", "Flacon de flamme de Nayru",
              "Flasche (Nayrus Flamme)" }, // "Botella de llamas de Nayru"
    };
    trickNameTable[RG_BOTTLE_WITH_BUGS] = {
        Text{ "Bottle with Baby Tektites", "Flacon de bébé Araknon",
              "Flasche (Baby Arachno)" },                                             // "Tektites en una botella"
        Text{ "Bottle with A Beetle", "Flacon avec un scarabée", "Flasche (Wanze)" }, // "Botella con un escarabajo"
        Text{ "Bottle with Lanayru Ants", "Flacon de fourmis de Lanelle",
              "Flasche (Ranelle-Ameise)" },                                      // "Celestarabajo embotellado"
        Text{ "Bottle with Insects", "Flacon de bibittes", "Flasche (Insekt)" }, // "Saltabosques embotellados"
        Text{ "Bottle with a Golden Bee", "Flacon avec une abeille dorée",
              "Flasche (Goldene Biene)" }, // "Botella con una abeja dorada"
    };
    trickNameTable[RG_BOTTLE_WITH_POE] = {
        Text{ "Bottle with Ghini", "Flacon avec Ghini", "Flasche (Ghini)" },                // "Ghini en una botella"
        Text{ "Bottle with Reapling", "Flacon avec Âme Damnée", "Flasche (Zombie)" },       // "Reapling en una botella"
        Text{ "Bottle with Imp Poe", "Flacon avec Spectre", "Flasche (Geist)" },            // "Espectro en una botella"
        Text{ "Bottle with Anti-Fairy", "Flacon avec Tetdoss", "Flasche (Knochenfratze)" }, // "Whisp en una botella"
    };

    trickNameTable[RG_RUTOS_LETTER] = {
        Text{ "Bottle with Maggie's Letter", "Flacon avec lettre de Maggy",
              "Flasche (Dolores Brief)" }, // "Carta de Dolores"
        Text{ "Bottle with Letter to Kafei", "Flacon avec lettre pour Kafei",
              "Flasche (Brief an Kafei)" }, // "Carta para Kafei"
        Text{ "Bottle with Zelda's Letter", "Flacon avec Lettre de Zelda",
              "Flasche (Zeldas Brief)" }, // "Carta náutica"
    };
    trickNameTable[RG_BOTTLE_WITH_BIG_POE] = {
        Text{ "Bottle with Composer Brother", "Flacon avec un compositeur",
              "Flasche (Ludwig Brahmstein)" },                                        // "Hermana Poe embotellada"
        Text{ "Bottle with Jalhalla", "Flacon avec Jalhalla", "Flasche (Jalhalla)" }, // "Yaihalla embotellado"
        Text{ "Bottle with Grim Repoe", "Flacon avec le Faucheur",
              "Flasche (Tagschwärmer)" }, // "Bubble en una botella"
    };

    trickNameTable[RG_ZELDAS_LULLABY] = {
        Text{ "Ballad of the Goddess", "Chant de la déesse", "Hymne der Göttin" }, // "Cántico de la Diosa"
        Text{ "Song of Healing", "Chant de l'apaisement", "Lied der Befreiung" },  // "Canción de curación"
        Text{ "Song of the Hero", "Chant du héros", "Heldenlied" },                // "Canción del héroe"
    };
    trickNameTable[RG_EPONAS_SONG] = {
        Text{ "Song of Birds", "Chant des oiseaux", "Vogelruf" },            // "Cantar del ave"
        Text{ "Song of Soaring", "Chant de l'envol", "Lied der Schwingen" }, // "Canción del viento"
        Text{ "Song of Horse", "Chant du cheval", "Lied des Pferdes" },      // "Chant du cheval"
    };
    trickNameTable[RG_SARIAS_SONG] = {
        Text{ "Mido's Song", "La chanson de Mido", "Midos Lied" },         // "La canción de Mido"
        Text{ "Kass' Theme", "Le thème de Kass", "Kashiwa Thema" },        // "El tema de Kass"
        Text{ "Tune of Echoes", "Chant des Échos ", "Melodie des Echos" }, // "Melodía del Eco"
    };
    trickNameTable[RG_SUNS_SONG] = {
        Text{ "Song of Passing", "Mambo de Manbo", "Kleine Tag- und Nachtmusik" },      // "Melodía del transcurrir"
        Text{ "Command Melody", "Air du marionnettiste", "Sonate des Puppenspielers" }, // "Cara al Sol"
        Text{ "Moon's Song", "La chanson de Moon", "Lied des Mondes" },                 // "La canción de la luna"
    };
    trickNameTable[RG_SONG_OF_TIME] = {
        Text{ "Song of Double Time", "Chant accéléré", "Thema der Zeit im Wind" }, // "Canción del doble tiempo"
        Text{ "Inverted Song of Time", "Chant du temps inversé",
              "Ballade des Kronos" },                                 // "Canción del tiempo invertida"
        Text{ "Tune of Ages", "Chant du Temps", "Melodie der Zeit" }, // "Melodía del Tiempo"
    };
    trickNameTable[RG_SONG_OF_STORMS] = {
        Text{ "Ballad of Gales", "Requiem de la tornade", "Kanon des Sturmes" }, // "Melodía del Tornado"
        Text{ "Frog's Song of Soul", "Rap des grenouilles", "Krötenrap" },       // "Canción del alma de la rana"
        Text{ "Wind's Requiem", "Mélodie du vent", "Lied des Windes" },          // "Melodía del Viento"
    };
    trickNameTable[RG_MINUET_OF_FOREST] = {
        Text{ "Saria's Karaoke", "Karaoké de Saria", "Salias Karaoke" },            // "Dueto del bosque"
        Text{ "Sonata of Awakening", "Sonate de l'éveil", "Sonate des Erwachens" }, // "Sonata del despertar"
        Text{ "Wind God's Aria", "Hymne du dieu du vent",
              "Hymne des Zephirgottes" }, // "Melodía del Espíritu del Viento"
    };
    trickNameTable[RG_BOLERO_OF_FIRE] = {
        Text{ "Darunia's Tango", "Tango de Darunia", "Darunias Tango" },             // "Coro del fuego"
        Text{ "Tune of Currents", "Chants des Flux", "Lied des Zeitstroms" },        // "Melodía de las Corrientes"
        Text{ "Goron Lullaby", "Berceuse des Gorons", "Goronisches Schlummerlied" }, // "Nana goron"
    };
    trickNameTable[RG_SERENADE_OF_WATER] = {
        Text{ "Ruto's Blues", "Blues de Ruto", "Rutos Blues" },                           // "Sonata del agua"
        Text{ "New Wave Bossa Nova", "Bossa-nova des flots", "Bossa Nova der Kaskaden" }, // "Bossanova de las olas"
        Text{ "Manbo's Mambo", "Mambo de Manbo", "Manbos Mambo" },                        // "Mambo de Manbo"
    };
    trickNameTable[RG_REQUIEM_OF_SPIRIT] = {
        Text{ "Nabooru's Reggae", "Reggae de Nabooru", "Naborus Reggae" },          // "Reggae del espíritu"
        Text{ "Elegy of Emptiness", "Hymne du vide", "Elegie des leeren Herzens" }, // "Elegía al vacío"
        Text{ "Earth God's Lyric", "Hymne du dieu de la terre",
              "Hymne des Terragottes" }, // "Melodía del Espíritu de la Tierra"
    };
    trickNameTable[RG_NOCTURNE_OF_SHADOW] = {
        Text{ "Impa's Death Metal", "Death métal d'Impa", "Impas Death Metal" }, // "Diurno de la sombra"
        Text{ "Oath to Order", "Ode de l'appel", "Gesang des Himmels" },         // "Oda al orden"
        Text{ "Song of Discovery", "Chant des secrets", "Schatzsucherlied" },    // "Canto revelador"
    };
    trickNameTable[RG_PRELUDE_OF_LIGHT] = {
        Text{ "Rauru's Sing-Along", "Chansonnette de Rauru", "Raurus Singalong" }, // "Predulio de luz"
        Text{ "Ballad of the Wind Fish", "Ballade sur Poisson-Rêve",
              "Ballade vom Windfisch" },                                   // "Balada del Piez Viento"
        Text{ "Song of Light", "Chant de la lumière", "Lied des Lichts" }, // "Sonidos de la luz"
    };

    trickNameTable[RG_KOKIRI_EMERALD] = {
        Text{ "Pendant of Courage", "Pendentif du courage", "Amulett des Mutes" }, // "Colgante del valor"
        Text{ "Farore's Pearl", "Perle de Farore", "Farores Deamont" },            // "Orbe de Farore"
        Text{ "Aquanine", "Smaragdine", "Smaragdstahl" },                          // "Yerbánida"
        Text{ "Farore's Emerald", "Émeraude de Farore", "Farore-Smaragd" },        // "Esmeralda de Farore"
        Text{ "Kokiri's Peridot", "Péridot Kokiri", "Kokiri-Peridot" },            // "Ágata de los Kokiri"
    };
    trickNameTable[RG_GORON_RUBY] = {
        Text{ "Pendant of Power", "Pendentif de la force", "Amulett der Stärke" }, // "Colgante del poder"
        Text{ "Din's Pearl", "Perle de Din", "Dins Deamont" },                     // "Orbe de Din"
        Text{ "Crimsonine", "Alzanine", "Scharlachstahl" },                        // "Bermellina"
        Text{ "Din's Ruby", "Rubis de Din", "Din-Rubin" },                         // "Rubí de Din"
        Text{ "Goron's Garnet", "Grenat Goron", "Goronen-Granat" },                // "Topacio de los Goron"
    };
    trickNameTable[RG_ZORA_SAPPHIRE] = {
        Text{ "Pendant of Wisdom", "Pendentif de la sagesse", "Amulett der Weisheit" }, // "Colgante de la sabiduría"
        Text{ "Nayru's Pearl", "Perle de Nayru", "Nayrus Deamont" },                    // "Orbe de Nayru"
        Text{ "Azurine", "Aquanine", "Azurstahl" },                                     // "Azurina"
        Text{ "Nayru's Sapphire", "Saphir de Nayru", "Nayru-Saphir" },                  // "Zafiro de Nayru"
        Text{ "Zora's Aquamarine", "Aquamarine Zora", "Zora-Aquamarin" },               // "Lapislázuli de los Zora"
    };
    trickNameTable[RG_FOREST_MEDALLION] = {
        Text{ "Wind Medallion", "Médaillon du vent", "Amulett des Windes" }, // "Medallón del Viento"
        Text{ "Wind Element", "Elément Vent", "Wind-Element" },              // "Elemento de aire"
        Text{ "Saria's Medallion", "Médaillon de Saria", "Salias Amulett" }, // "Medallón de Saria"
        Text{ "Sign of Air", "Glyphe de l'air", "Zeichen der Luft" },        // "Glifo de aire"
        Text{ "Medallion of Forest", "Médaillon du Temple de la Forêt",
              "Medaillon des Waldes" }, // "Medalla del Bosque"
    };
    trickNameTable[RG_FIRE_MEDALLION] = {
        Text{ "Fire Element", "Elément Feu", "Flammen-Element" },                          // "Elemento de fuego"
        Text{ "Darunia's Medallion", "Médaillon de Darunia", "Darunias Amulett" },         // "Medallón de Darunia"
        Text{ "Sign of Fire", "Glyphe de feu", "Zeichen des Feuers" },                     // "Glifo de fuego"
        Text{ "Medallion of Fire", "Médaillon du Temple du Feu", "Medaillon des Feuers" }, // "Medalla del Fuego"
    };
    trickNameTable[RG_WATER_MEDALLION] = {
        Text{ "Water Element", "Elément Eau", "Tropfen-Element" },                             // "Elemento de agua"
        Text{ "Ice Medallion", "Médaillon de glace", "Amulett des Eises" },                    // "Medallón Helado"
        Text{ "Ruto's Medallion", "Médaillon de Ruto", "Rutos Amulett" },                      // "Medallón de Ruto"
        Text{ "Sign of Water", "Glyphe de l'eau", "Zeichen des Wassers" },                     // "Glifo de agua"
        Text{ "Medallion of Water", "Médaillon du Temple de l'Eau", "Medaillon des Wassers" }, // "Medalla del Agua"
    };
    trickNameTable[RG_SPIRIT_MEDALLION] = {
        Text{ "Earth Element", "Elément Terre", "Erd-Element" },                  // "Elemento de tierra"
        Text{ "Nabooru's Medallion", "Médaillon de Nabooru", "Naborus Amulett" }, // "Medallón de Nabooru"
        Text{ "Sign of Earth", "Glyphe de la Terre", "Zeichen der Erde" },        // "Glifo de la tierra"
        Text{ "Medallion of Spirit", "Médaillon du Temple de l'Esprit",
              "Medaillon der Geister" }, // "Medalla del Espíritu"
    };
    trickNameTable[RG_SHADOW_MEDALLION] = {
        Text{ "Fused Shadow", "Cristal d'ombre", "Schattenkristall" },                // "Sombra Fundida"
        Text{ "Impa's Medallion", "Médaillon d'Impa", "Impas Amulett" },              // "Medallón de Impa"
        Text{ "Sign of Illusion", "Glyphe de l'illusion", "Zeichen der Illusionen" }, // "Glifo de ilusión"
        Text{ "Medallion of Shadow", "Médaillon du Temple de l'Ombre",
              "Medaillon des Schattens" }, // "Medalla de la Sombra"
    };
    trickNameTable[RG_LIGHT_MEDALLION] = {
        Text{ "Compass of Light", "Boussole de lumière", "Licht-Kompaß" },                      // "Brújula de Luz"
        Text{ "Rauru's Medallion", "Médaillon de Rauru", "Raurus Amulett" },                    // "Medallón de Rauru"
        Text{ "Sign of Destiny", "Glyphe du destin", "Zeichen des Schicksals" },                // "Glifo del destino"
        Text{ "Medallion of Light", "Médaillon du temple de lumière", "Medaillon des Lichts" }, // "Medalla de la Luz"
    };

    trickNameTable[RG_RECOVERY_HEART] = {
        Text{ "Love", "Bisou", "Liebe" }, // "Te amo"
        Text{ "Life", "Vie", "Leben" },   // "vida"
        Text{ "HP", "PV", "KP" },         // "VP"
    };
    trickNameTable[RG_GREEN_RUPEE] = {
        Text{ "Rupee (1)", "Rubis (1)", "Rubin (1)" },        // "Peso hyliano"
        Text{ "One Ruby", "Un rubis", "ein Ruby" },           // "Un rubí"
        Text{ "One Rupee", "Un rubis", "ein Rubin" },         // "Guaraní hyliano"
        Text{ "False Greg", "Faux Greg", "falscher Greg" },   // "Falso Greg"
        Text{ "Rupoor (1)", "Roupir (1)", "Rubinfalle (1)" }, // "Rupobre (1)"
    };
    trickNameTable[RG_BLUE_RUPEE] = {
        Text{ "Blupee", "Bleubi", "Fünfer" },                 // "Azupia"
        Text{ "Five Rubies", "Cinq Rubys", "fünf Rubies€" },  // "Cinco rubíes"
        Text{ "Five Rupees", "Cinq rubis", "fünf Rubine€" },  // "Bolívar hyliano"
        Text{ "Rupee (5)", "Rubis (5)", "Rubin (5)" },        // "Peso hyliano"
        Text{ "Rupoor (5)", "Roupir (5)", "Rubinfalle (5)" }, // "Rupobre (5)"
    };
    trickNameTable[RG_RED_RUPEE] = {
        Text{ "Big 20", "Grand 20", "Zwanni" },                    // "Los 20 grandes"
        Text{ "Twenty Rubies", "vingt rubis", "zwanzig Rubies€" }, // "Veinte rubíes"
        Text{ "Twenty Rupees", "Vingt rubis", "zwanzig Rubine€" }, // "Colon hyliano"
        Text{ "Rupee (20)", "Rubis (20)", "Rubin (20)" },          // "Peso hyliano"
        Text{ "Rupoor (20)", "Roupir (20)", "Rubinfalle (20)" },   // "Rupobre (20)"
    };
    trickNameTable[RG_PURPLE_RUPEE] = {
        Text{ "Purpee", "Pourbi", "Fuffi" },                          // "morupiua"
        Text{ "Fifty Rubies", "Cinquante rubis", "fünfzig Rubies€" }, // "Cincuenta rubíes"
        Text{ "Fifty Rupees", "Cinquante rubis", "fünfzig Rubine€" }, // "Balboa hyliano"
        Text{ "Rupee (50)", "Rubis (50)", "Rubin (50)" },             // "Peso hyliano"
        Text{ "Rupoor (50)", "Roupir (50)", "Rubinfalle (50)" },      // "Rupobre (50)"
    };
    trickNameTable[RG_HUGE_RUPEE] = {
        Text{ "Hugo", "Or Rubi", "zwei Hunnis€" },                               // "Oro Rubi"
        Text{ "Two Hundred Rubies", "Deux cents rubis", "zweihundert Rubies€" }, // "Doscientos rubíes"
        Text{ "Diamond", "Diamant", "Diamant" },                                 // "Diamante"
        Text{ "Huge Ruby", "Énorme rubis", "großer Ruby" },                      // "Rubi gigante"
        Text{ "Two Hundred Rupees", "Deux cent rubis", "zweihundert Rubine€" },  // "Euro hyliano"
        Text{ "Rupee (200)", "Rubis (200)", "Rubin (200)" },                     // "Dólar hyliano"
    };
    trickNameTable[RG_PIECE_OF_HEART] = {
        Text{ "Pizza Heart", "Fromage de cœur", "Harzteil" },                             // "Pieza de Chorizo"
        Text{ "Little Bit Of Love", "Un peu d'amour", "etwas Liebe" },                    // "Un poco de amor"
        Text{ "Rare Peach Stone", "Pierre de pêche rare", "ein seltener Pfirsichstein" }, // "Pierre de pêche rare"
    };
    trickNameTable[RG_HEART_CONTAINER] = {
        Text{ "Crystal Heart", "Cœur de cristal", "Kristallherz" }, // "Corazón de cristal"
        Text{ "Life Heart", "Cœur de vie", "Lebensherz" },          // "Vida Corazón"
        Text{ "Lots of Love", "Beaucoup d'amour", "viel Liebe" },   // "Mucho amor"
    };
    trickNameTable[RG_TRIFORCE_PIECE] = {
        Text{ "Piece of Cheese", "Morceau de Fromage", "Käseteil" },        // "Piece of Cheese"
        Text{ "Triforce Shard", "Éclat de Triforce", "Triforce-Fragment" }, // "Triforce Shard"
        Text{ "Shiny Rock", "Caillou Brillant", "glänzender Stein" },       // "Shiny Rock"
    };
    trickNameTable[RG_GOHMA_SOUL] = {
        Text{ "Spider Sense", "Sens de l'Araignée", "Spinnensinn" },
        Text{ "Deku Spirit", "Parasite Mojo", "Deku Geist" },
        Text{ "Ghost of Ghoma", "Fantôme de Gohma", "Gohmas Geist" },
    };
    trickNameTable[RG_KING_DODONGO_SOUL] = {
        Text{ "Lizard Soul", "Âme d'un Lézard", "Reptilienseele" },
        Text{ "Regal Remains", "Restes Délicieux", "royale Überreste€" },
        Text{ "Dodongo's Core", "Coeur de Dodongo", "Dodongos Kern" },
    };
    trickNameTable[RG_BARINADE_SOUL] = {
        Text{ "Parasitic Poltergeist", "Poltergeist Parasite", "infektiöser Poltergeist" },
        Text{ "Jabu Insides", "Entrailles de Jabu-Jabu", "Jabus Innereien" },
        Text{ "Barinade Bacteria", "Bactérie de Barinade", "Barinades Bakterien" },
    };
    trickNameTable[RG_PHANTOM_GANON_SOUL] = {
        Text{ "Bigger Poe", "Âme Gigantesque", "ein großer Nachtschwärmer" },
        Text{ "Sacred Forest Pine Tree", "Grande Perche du Bosquet Sacré", "Kiefer der Heiligen Lichtung" },
        Text{ "Ganon's Phantom", "Fantôme de Ganon", "Ganons Phantom" },
    };
    trickNameTable[RG_VOLVAGIA_SOUL] = {
        Text{ "Dragon Roast", "Friture du Dragon", "Drachenbraten" },
        Text{ "Hot n' Ready", "Sauce Barbecue", "Barbecue Sauce" },
        Text{ "Volvagia's Vitality", "Vitalité de Volcania", "Volvagias Vitalität" },
    };
    trickNameTable[RG_MORPHA_SOUL] = {
        Text{ "Dihydrogen Monoxide", "Monoxyde de Dihydrogène", "Dihydrogenmonoxid" },
        Text{ "Morpha Molecules", "Molécule de Morpha", "Morphas Molekyle" },
        Text{ "Wet Stuff", "Truc Mouillé", "nasses Zeug" },
    };
    trickNameTable[RG_BONGO_BONGO_SOUL] = {
        Text{ "Shadow Soul", "Âme de l'Ombre", "Schattenseele" },
        Text{ "Dark Essence", "Essence Sombre", "Dunkle Essenz" },
        Text{ "Bongo Bongo's Bongo", "Bongo de Bongo Bongo", "Bongo Bongos Bongo" },
    };
    trickNameTable[RG_TWINROVA_SOUL] = {
        Text{ "Sandy Ashes", "Cendres des Vieilles", "Sandige Asche" },
        Text{ "Spiritual Spirit", "Esprit Spirituel", "Geisterhafter Geist" },
        Text{ "Twin Rovers", "Duo Angélique", "Twinrovas Seile" },
    };
    trickNameTable[RG_GANON_SOUL] = {
        Text{ "Pure Evil", "Mal Incarné", "Das pure Böse" },
        Text{ "Ganon's Ghost", "Le Malin", "Ganons Geist" },
        Text{ "Pork", "Porc", "Schweinefleisch" },
    };
    trickNameTable[RG_FISHING_POLE] = {
        Text{ "Fish Tickler", "Fish Tickler", "Fischkitzler" },
        Text{ "Floating Lure", "Floating Lure", "Schwimmer" },
        Text{ "Fishing Reel", "Fishing Reel", "Angelschnur" },
    };
    trickNameTable[RG_OCARINA_A_BUTTON] = {
        Text{ "Ocarina J Button", "Touche Ha de l'Ocarina", "J-Taste der Okarina" },
        Text{ "Ocarina Ayy Button", "Touche Ah de l'Ocarina", "A-Taste der Flöte" },
        Text{ "Ocarina A Trigger", "Bumper A de l'Ocarina", "A-Trigger der Okarina" },
    };
    trickNameTable[RG_OCARINA_C_UP_BUTTON] = {
        Text{ "Ocarina C North Button", "Touche C Nord de l'Ocarina", "C-Nord-Taste der Okarina" },
        Text{ "Ocarina C App Button", "Touche C'est Haut de l'Ocarina", "C-Oben-Taste des Rekorders" },
        Text{ "Ocarina Sup Button", "Touche O de l'Ocarina", "C-Oben-Trigger der Okarina" },
    };
    trickNameTable[RG_OCARINA_C_DOWN_BUTTON] = {
        Text{ "Ocarina C South Button", "Touche C Sud de l'Ocarina", "C-Süd-Taste der Okarina" },
        Text{ "Ocarina Z Down Button", "Touche Z Bas de l'Ocarina", "Z-Unten-Taste der Okarina" },
        Text{ "Ocarina See Down Button", "Touche C'est Bas de l'Ocarina", "C-Unten-Taste der Harfe" },
        Text{ "Ocarina C Dawn Button", "Touche Séba de l'Ocarina", "C-Unten-Trigger der Okarina" },
    };
    trickNameTable[RG_OCARINA_C_LEFT_BUTTON] = {
        Text{ "Ocarina C West Button", "Touche C Ouest de l'Ocarina", "C-West-Taste der Okarina" },
        Text{ "Ocarina Sea Left Button", "Touche Cégoche de L'Ocarina", "C-Links-Taste der Nixenglocke" },
        Text{ "Ocarina C Lift Button", "Touche C'est Gauche de l'Ocarina", "C-Zelda-Taste der Okarina" },
        Text{ "Ocarina Rewind Button", "Touche Rembobiner de l'Ocarina", "C-Links-Trigger der Okarina" },
    };
    trickNameTable[RG_OCARINA_C_RIGHT_BUTTON] = {
        Text{ "Ocarina C East Button", "Touche C Est de l'Ocarina", "C-Ost-Taste der Okarina" },
        Text{ "Ocarina C Wright Button", "Touche C'est Droite de l'Ocarina", "C-Rechts-Trigger der Okarina" },
        Text{ "Overworld C Right Button", "Trou Droit de l'Ocarina", "C-Rechts-Taste der E-Gitarre" },
    };

    /*
    //Names for individual upgrades, in case progressive names are replaced
    trickNameTable[GI_HOOKSHOT] = {
        Text{"Grappling Hook", "Grappin-griffe", "Enterhaken" }, // "Gancho lanzable"
        Text{"Clawshot", "Lance-chaîne", "Greifhaken" }, // "Zarpa"
        Text{"Gripshot", "Grappince", "Tauschhaken" }, // "Enganchador"
    };
    trickNameTable[GI_LONGSHOT] = {
        Text{"Longshot, no strings attached", "Grappin sans attrape", "Enterhaken, Umtausch ausgeschloßen"}, // "Gancho
    lanzable más largo" Text{"Double Clawshot", "Double-grappin", "Doppelhaken" }, // "Superzarpa" Text{"Switch Hook",
    "Great grappin", "Tauschhaken" }, // "Gancho chulo"
    };
    trickNameTable[GI_BOMB_BAG_1] = {
        Text{"Bomb Capacity (20)", "Capacité de bombes (20)", "Bomben-Kapazität (20)" }, // "Bolsa de bombas (contiene
    20)" Text{"Bronze Bomb Bag", "Sac de Bombes de bronze", "Bronzene Bombentasche" }, // "Saco de bronce de bombas"
        Text{"Small Bomb Bag", "Petit Sac de Bombes", "Kleine Bombentasche" }, // "Zurrón de bombas pequeño"
    };
    trickNameTable[GI_BOMB_BAG_2] = {
        Text{"Bomb Capacity (30)", "Capacité de bombes (30)", "Bomben-Kapazität (30)" }, // "Bolsa de bombas (contiene
    30)" Text{"Silver Bomb Bag", "Sac de Bombes d'argent", "Silberne Bombentasche" }, // "Saco plateado de bombas"
        Text{"Medium Bomb Bag", "Sac de Bombes moyen", "Mittlere Bombentasche" }, // "Zurrón de bombas mediano"
    };
    trickNameTable[GI_BOMB_BAG_3] = {
        Text{"Bomb Capacity (40)", "Capacité de bombes (40)", "Bomben-Kapazität (40)" }, // "Bolsa de bombas (contiene
    40)" Text{"Golden Bomb Bag", "Sac de Bombes d'or", "Goldene Bombentasche" }, // "Saco dorado de bombas" Text{"Large
    Bomb Bag", "Gros Sac de Bombes", "Gigantische Bombentasche" }, // "Zurrón de bombas grande"
    };
    trickNameTable[GI_BOW_1] = {
        Text{"Bow", "Arc", "Bogen" }, // "Arco del Hada"
        Text{"Hero's Bow", "Arc du héros", "Heroenbogen" }, // "Arco del héroe"
        Text{"Small Quiver", "Petit carquois", "Kleiner Köcher" }, // "Saco de flechas pequeño"
    };
    trickNameTable[GI_BOW_2] = {
        Text{"Arrow Capacity (40)", "Capacité de flèches (40)", "Pfeil-Kapazität (40)" }, // "Capacidad de flechas (40)"
        Text{"Silver Quiver", "Carquois d'argent", "Silberner Köcher" }, // "Carcaj plateado"
        Text{"Medium Quiver", "Carquois moyen", "Mittlerer Köcher" }, // "Saco de flechas mediano"
    };
    trickNameTable[GI_BOW_3] = {
        Text{"Arrow Capacity (50)", "Capacité de flèches (50)", "Pfeil-Kapazität (50)" }, // "Capacidad de flechas (50)"
        Text{"Golden Quiver", "Carquois d'or", "Goldener Köcher" }, // "Carcaj dorado"
        Text{"Large Quiver", "Gros carquois", "Gigantischer Köcher" }, // "Saco de flechas grande"
    };
    trickNameTable[GI_SLINGSHOT_1] = {
        Text{"Slingshot", "Lance-Pierre", "Schleuder" }, // "Tirachinas del Hada"
        Text{"Scattershot", "Lance-Pierre rafale", "Streuschleuder" }, // "Tirachinas múltiple"
        Text{"Small Seed Satchel", "Petit sac de graines", "Kleine Munitionstasche" }, // "Bolsa de semillas pequeña"
    };
    trickNameTable[GI_SLINGSHOT_2] = {
        Text{"Deku Seed Capacity (40)", "Capacité de graines (40)", "Deku-Kern-Kapazität (40)" }, // "Capacidad de
    semillas (40)" Text{"Silver Deku Seed Bullet Bag", "Sac de graines d'argent", "Silberne Munitionstasche" }, //
    "Bolsa de balas (contiene 40)" Text{"Medium Seed Satchel", "Sac de graines moyen", "Mittlere Munitionstasche" }, //
    "Bolsa de semillas mediana"
    };
    trickNameTable[GI_SLINGSHOT_3] = {
        Text{"Deku Seed Capacity (50)", "Capacité de graines (50)", "Deku-Kern-Kapazität (50)" }, // "Capacidad de
    semillas (50)" Text{"Golden Deku Seed Bullet Bag", "Sac de graines d'or", "Goldene Munitionstasche" }, // "Bolsa de
    balas (contiene 50)" Text{"Large Seed Satchel", "Gros sac de graines", "Gigantische Munitionstasche" }, // "Bolsa de
    semillas grande"
    };
    trickNameTable[GI_STRENGTH_1] = {
        Text{"Goron's Gauntlet", "Gantelet Goron", "Goronen-Handschuhe€" }, // "Brazalete amarillo"
        Text{"Power Bracelet", "Bracelet de force", "Kraftarmband" }, // "Brazalete de fuerza"
        Text{"Magic Bracelet", "Bracelet de Lavio", "Magiearmband" }, // "Brazalete de Ravio"
    };
    trickNameTable[GI_STRENGTH_2] = {
        Text{"Silver Bracelets", "Bracelets d'argent", "Silberarmband" }, // "Guantes Moguma"
        Text{"Power Gloves", "Gant de puissance", "Silberhandschuhe€" }, // "Guante del Poder"
        Text{"Magic Gauntlets", "Gantelet magique", "Magiehandschuhe€" }, // "Guante mágico"
    };
    trickNameTable[GI_STRENGTH_3] = {
        Text{"Golden Bracelets", "Bracelets d'or", "Goldarmband" }, // "Guantelete de Thanos"
        Text{"Titan's Mitts", "Moufle de titan", "Goldhandschuhe€" }, // "Guantes de Titán"
        Text{"Magnetic Gloves", "Magnéto-gants", "Magnethandschuhe€" }, // "Guantes de fuego"
    };
    trickNameTable[GI_SCALE_1] = {
        Text{"Silver Pearl", "Perle d'argent", "Silberne Perle" }, // "Perla de Plata progresiva"
        Text{"Adult Scale", "Écaille d'adulte", "Große Schuppe" }, // "Bola de bolos zora"
        Text{"Zora Scale", "Écaille Zora", "Zora-Schuppe" }, // "Escama de Zora"
    };
    trickNameTable[GI_SCALE_2] = {
        Text{"Golden Pearl", "Perle d'or", "Goldene Perle" }, // "Perla de Oro progresiva"
        Text{"Giant Scale", "Écaille de géant", "Riesenschuppe" }, // "Escama de Faren"
        Text{"Water Dragon Scale", "Écaille du dragon de l'eau", "Wasserdrachenschuppe" }, // "Escama de dragón
    acuático"
    };
    trickNameTable[GI_WALLET_1] = {
        Text{"Rupee Capacity (200)", "Capacité de rubis (200)", "Rubinkapazität (200)" }, // "Capacidad de rupias (200)"
        Text{"Silver Wallet", "Bourse d'argent", "Silberne Geldbörse" }, // "Cartera de rupias de adulto"
        Text{"Medium Wallet", "Bourse moyenne", "Mittlere Geldbörse" }, // "Zurrón de rupias mediano"
    };
    trickNameTable[GI_WALLET_2] = {
        Text{"Rupee Capacity (500)", "Capacité de rubis (500)", "Rubin-Kapazität (500)" }, // "Capacidad de rupias
    (500)" Text{"Golden Wallet", "Bourse d'or", "Riesenportemonnaie" }, // "Cartera de rupias gigante" Text{"Large
    Wallet", "Grosse Bourse", "Gigantische Geldbörse" }, // "Zurrón de rupias grande"
    };
    trickNameTable[GI_WALLET_3] = {
        Text{"Rupee Capacity (999)", "Capacité de rubis (999)", "Rubin-Kapazität (999) }, // "Capacidad de rupias (999)"
        Text{"Golden Wallet", "Bourse d'or", "Goldenes Portemonnaie" }, // "Cartera de ricachón"
        Text{"Large Wallet", "Grosse Bourse", "Gigantische Geldbörse" }, // "Zurrón de rupias gigante"
    };
    trickNameTable[GI_DEKU_NUT_UPGRADE_1] = {
        Text{"Deku Bomb Capacity (30)", "Capacité de bombes Mojo (30)", "Deku-Bomben-Kapazität (30)" }, // "Capacidad de
    semillas deku (40)" Text{"Baba Nut Capacity (30)", "Capacité de noix Baba (30)", "Ranha-Nuß-Kapazität (30)" }, //
    "Capacidad de nueces baba (40)" Text{"Deku Nut Pack (30)", "Paquet de noix Mojo (30)", "Deku-Nußrucksack (30)" }, //
    "Capacidad de nueces mojo (40)"
    };
    trickNameTable[GI_DEKU_NUT_UPGRADE_2] = {
        Text{"Deku Bomb Capacity (40)", "Capacité de bombes Mojo (40)", "Deku-Bomben-Kapazität (40)" }, // "Capacidad de
    semillas deku (50)" Text{"Baba Nut Capacity (40)", "Capacité de noix Baba (40)", "Ranha-Nuß-Kapazität (40)" }, //
    "Capacidad de nueces baba (50)" Text{"Deku Nut Pack (40)", "Paquet de noix Mojo (40)", "Deku-Nußrucksack (40)" }, //
    "Capacidad de nueces mojo (50)"
    };
    trickNameTable[GI_DEKU_STICK_UPGRADE_1] = {
        Text{"Deku Rod Capacity (20)", "Capacité de tiges Mojo (20)", "Deku-Stock-Kapazität (20)" }, // "Capacidad de
    palos mojo (20)" Text{"Boko Stick Capacity (20)", "Capacité de Bâtons Boko (20)", "Bokstock-Kapazität (20)" }, //
    "Capacidad de palos boko (20)" Text{"Deku Stick Pack (20)", "Paquet de bâtons Mojo (20)", "Deku-Stabrucksack (20)"
    }, // "Capacidad de bastones deku (20)"
    };
    trickNameTable[GI_DEKU_STICK_UPGRADE_2] = {
        Text{"Deku Rod Capacity (30)", "Capacité de tiges Mojo (30)", "Deku-Stock-Kapazität (30)" }, // "Capacidad de
    palos mojo (30)" Text{"Boko Stick Capacity (30)", "Capacité de Bâtons Boko (30)", "Bokstock-Kapazität (30)" }, //
    "Capacidad de palos boko (30)" Text{"Deku Stick Pack (30)", "Paquet de bâtons Mojo (30)", "Deku-Stabrucksack (30)"
    }, // "Capacidad de bastones deku (30)"
    };
    trickNameTable[GI_MAGIC_1] = {
        Text{"Stamina Meter", "Jauge d'endurance", "Ausdauerleiste" }, // "Medidor de vigor"
        Text{"Energy Meter", "Jauge d'énergie", "Energieleiste" }, // "Medidor de energía"
        Text{"Magic Powder", "Poudre magique", "Zauberpulver" }, // "Medidor de carga"
    };
    trickNameTable[GI_MAGIC_2] = {
        Text{"Enhanced Stamina Meter", "Jauge d'endurance améliorée", "Verb. Ausdauerleiste" }, // "Medidor de vigor
    mejorado" Text{"Enhanced Energy Meter", "Jauge d'énergie améliorée", "Verb. Energieleiste" }, // "Medidor de energía
    mejorado" Text{"Enhanced Magic Powder", "Poudre magique améliorée", "Verb. Zauberpulver" }, // "Medidor de carga
    mejorado"
    };
    trickNameTable[GI_OCARINA_1] = {
        Text{"Ocarina", "Ocarina", "Okarina" }, // "Ocarina"
        Text{"Saria's Ocarina", "Ocarina de Saria", "Salias Okarina" }, // "Ocarina de Saria"
        Text{"Wood Ocarina", "Ocarina de bois", "Holzokarina" }, // "Ocarina del Hada"
    };
    trickNameTable[GI_OCARINA_2] = {
        Text{"Flute", "Flûte", "Flöte" }, // "Flauta"
        Text{"Zelda's Ocarina", "Ocarina de Zelda", "Zeldas Okarina" }, // "Ocarina de Zelda"
        Text{"Ocarina of Winds", "Ocarina des vents", "Okarina des Windes" }, // "Ocarina del Viento"
    };
    trickNameTable[GI_CUCCO] = {
        Text{"D.I.Y. Alarm Clock", "Réveille-matin improvisé", "Improvisierter Wecker"}, // "Alarma emplumada"
        Text{"Kakariko Cucco", "Cocotte Cocorico", "Kakariko Huhn" }, // "Cuco de Kakariko"
        Text{"Hatched Cucco", "Cocotte éclose", "Geschlüpftes Küken" }, // "Pollo"
    };
    trickNameTable[GI_MASK_KEATON] = {
        Text{"Kee... Something Mask", "Masque de Quiche", "Keaton-Maske" }, // "Máscara Kealgo"
        Text{"Kitsune Mask", "Masque de Kitsune", "Kitsune-Maske" }, // "Máscara Kitsune"
        Text{"Kafei's Mask", "Masque de Kafei", "Kafeis Maske" }, // "Máscara de Kafei"
    };
    trickNameTable[GI_MASK_SKULL] = {
        Text{"Skull Kid's Mask", "Masque de Skull Kid", "Horror-Kids-Maske" }, // "Máscara de Skull Kid"
        Text{"Stalfos Mask", "Masque de squelette", "Stalfos-Maske" }, // "Máscara de Stalfos"
        Text{"Captain's Hat", "Heaume du capitaine", "Helm des Hauptmanns" }, // "Casco del capitán"
    };
    trickNameTable[GI_MASK_SPOOKY] = {
        Text{"Skrik Mask", "Masque Skrik", "Grusel-Maske" }, // "Máscara Escalofriante"
        Text{"ReDead Mask", "Masque de Remort", "Remort-Maske" }, // "Máscara de ReDead"
        Text{"Gibdo Mask", "Masque de Gibdo", "Gibdo-Maske" }, // "Careta de Gibdo"
    };
    trickNameTable[GI_MASK_BUNNY] = {
        Text{"Peppy Mask", "Masque de Peppy", "Peppy-Maske" }, // "Capucha de Pascua"
        Text{"Bunny Ears", "Oreilles de lapin", "Ohrlöffel" }, // "Orejas de conejo"
        Text{"Postman's Hat", "Casquette du facteur", "Mütze des Postboten" }, // "Gorra de cartero"
    };
    trickNameTable[GI_MASK_GORON] = {
        Text{"Goro Mask", "Masque Goro", "Garos Maske" }, // "Máscara Goro"
        Text{"Mask of Goron", "Masque des Gorons", "Goronen-Haut" }, // "Máscara de los Goron"
        Text{"Darunia Mask", "Masque de Darunia", }, "Darunias Maske" // "Máscara de Darmani"
    };
    trickNameTable[GI_MASK_ZORA] = {
        Text{"Zola Mask", "Masque Zola", "Zola-Maske" }, // "Máscara Zola"
        Text{"Mask of Zora", "Masque des Zoras", "Zora-Schuppen€" }, // "Máscara de los Zora"
        Text{"Ruto Mask", "Masque de Ruto", "Rutos Maske" }, // "Máscara de Mikau"
    };
    trickNameTable[GI_MASK_GERUDO] = {
        Text{"Ganguro Mask", "Masque de Ganguro", "Ganguro-Maske" }, // "Máscara Canguro"
        Text{"Mask of Gerudo", "Masque des Gerudos", "Gerudo-Perücke" }, // "Máscara de las Gerudo"
        Text{"Nabooru Mask", "Masque de Nabooru", "Naborus Maske" }, // "Máscara de Nabooru"
    };
    trickNameTable[GI_MASK_TRUTH] = {
        Text{"Sheikah Mask", "Masque Sheikah", "Shiekah-Maske" }, // "Máscara Sheikah"
        Text{"Mask of Gossip", "Masque de potins", "Maske der Mythen" }, // "Máscara chismosa"
        Text{"Eye of Truth", "oeil de vérité", "Auge der Wahrheit" }, // "Ojo de la Verdad"
    };
    */
}

// Generate a fake name for the ice trap based on the item it's displayed as
Text GetIceTrapName(uint8_t id) {
    // If the trick names table has not been initialized, do so
    if (!initTrickNames) {
        InitTrickNames();
        initTrickNames = true;
    }
    // Randomly get the easy, medium, or hard name for the given item id
    return RandomElement(trickNameTable[id]);
}