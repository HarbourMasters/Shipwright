#include "item_pool.hpp"
#include "location_access.hpp"
#include "random.hpp"
#include "shops.hpp"
#include "../location.h"

#include <array>
#include <math.h>
#include <map>
#include <spdlog/spdlog.h>
#include "z64item.h"

PriceSettingsStruct::PriceSettingsStruct(RandomizerSettingKey _main,
                        RandomizerSettingKey _fixedPrice,
                        RandomizerSettingKey _range1,
                        RandomizerSettingKey _range2,
                        RandomizerSettingKey _noWallet,
                        RandomizerSettingKey _childWallet,
                        RandomizerSettingKey _adultWallet,
                        RandomizerSettingKey _giantWallet,
                        RandomizerSettingKey _tycoonWallet,
                        RandomizerSettingKey _affordable){
   main = _main;
   fixedPrice = _fixedPrice;
   range1 = _range1;
   range2 = _range2;
   noWallet = _noWallet;
   childWallet = _childWallet;
   adultWallet = _adultWallet;
   giantWallet= _giantWallet;
   tycoonWallet= _tycoonWallet;
   affordable= _affordable;
}


static std::array<std::vector<Text>, 0xF1> trickNameTable; // Table of trick names for ice traps
bool initTrickNames = false; //Indicates if trick ice trap names have been initialized yet

//Set vanilla shop item locations before potentially shuffling
void PlaceVanillaShopItems() {
    auto ctx = Rando::Context::GetInstance();
    //Loop to place vanilla items in each location
    for (RandomizerCheck& randomizerCheck : Rando::StaticData::GetShopLocations()) {
        ctx->GetItemLocation(randomizerCheck)->PlaceVanillaItem();
    }
}

//These are the vanilla shop items, but in a priority order of importance
//However many shop item slots were cleared, this will return 64 minus that number of vanilla shop items to be placed with assumed fill
//The first 32 items here will always be present in shops
//Shopsanity 4 will only have the first 32, shopsanity 1 will have the first 56, etc.
//Shopsanity random will have anywhere from the first 32 to the first 56, so the order of items after 32 is relevant
std::vector<RandomizerGet> GetMinVanillaShopItems(int total_replaced) {
    std::vector<RandomizerGet> minShopItems = {
        RG_BUY_DEKU_SHIELD,
        RG_BUY_HYLIAN_SHIELD,
        RG_BUY_GORON_TUNIC,
        RG_BUY_ZORA_TUNIC,
        RG_BUY_DEKU_NUTS_5,
        RG_BUY_BOMBS_20,
        RG_BUY_BOMBCHUS_10,
        RG_BUY_DEKU_STICK_1,
        //^First 8 items: Exist on shopsanity 7 or less
        RG_BUY_FAIRYS_SPIRIT,
        RG_BUY_DEKU_SEEDS_30,
        RG_BUY_ARROWS_10,
        RG_BUY_BLUE_FIRE,
        RG_BUY_RED_POTION_30,
        RG_BUY_GREEN_POTION,
        RG_BUY_DEKU_NUTS_10,
        RG_BUY_BOMBCHUS_10,
        //^First 16 items: Exist on shopsanity 6 or less
        RG_BUY_BOMBCHUS_10,
        RG_BUY_BOMBCHUS_20,
        RG_BUY_BOMBS_525,
        RG_BUY_BOMBS_535,
        RG_BUY_BOMBS_10,
        RG_BUY_DEKU_NUTS_5,
        RG_BUY_ARROWS_30,
        RG_BUY_ARROWS_50,
        //^First 24 items: Exist on shopsanity 5 or less
        RG_BUY_ARROWS_10,
        RG_BUY_FAIRYS_SPIRIT,
        RG_BUY_BOTTLE_BUG,
        RG_BUY_FISH,
        //^First 28 items from OoTR
        RG_BUY_HYLIAN_SHIELD,
        RG_BUY_BOTTLE_BUG,
        RG_BUY_DEKU_STICK_1,
        RG_BUY_DEKU_STICK_1,
        //^First 32 items: Exist on shopsanity 4 or less
        RG_BUY_BLUE_FIRE,
        RG_BUY_FISH,
        RG_BUY_BOMBCHUS_10,
        RG_BUY_DEKU_NUTS_5,
        RG_BUY_ARROWS_10,
        RG_BUY_BOMBCHUS_20,
        RG_BUY_BOMBS_535,
        RG_BUY_RED_POTION_30,
        //^First 40 items: Exist on shopsanity 3 or less
        RG_BUY_BOMBS_30,
        RG_BUY_BOMBCHUS_20,
        RG_BUY_DEKU_NUTS_5,
        RG_BUY_ARROWS_10,
        RG_BUY_DEKU_NUTS_5,
        RG_BUY_ARROWS_30,
        RG_BUY_RED_POTION_40,
        RG_BUY_FISH,
        //^First 48 items: Exist on shopsanity 2 or less
        RG_BUY_BOMBCHUS_20,
        RG_BUY_ARROWS_30,
        RG_BUY_RED_POTION_50,
        RG_BUY_ARROWS_30,
        RG_BUY_DEKU_NUTS_5,
        RG_BUY_ARROWS_50,
        RG_BUY_ARROWS_50,
        RG_BUY_GREEN_POTION,
        //^First 56 items: Exist on shopsanity 1 or less
        RG_BUY_POE,
        RG_BUY_POE,
        RG_BUY_HEART,
        RG_BUY_HEART,
        RG_BUY_HEART,
        RG_BUY_HEART,
        RG_BUY_HEART,
        RG_BUY_HEART,
        //^All 64 items: Only exist with shopsanity 0
    };
    //Now delete however many items there are to replace
    for (int i = 0; i < total_replaced; i++) {
        minShopItems.pop_back();
    }
    return minShopItems;
}

//This table contains a cumulative probability for each possible shop price based on
// a beta distribution with alpha = 1.5, beta = 2, and the result of the distribution, a float in [0.0, 1.0),
// being mutliplied by 60, casted to an integer, then multiplied by 5 to give a value in range [0, 295] in increments of 5.
// Meaning the first value is the probability of 0, the next value is the probability of 0 plus the probability of 5, etc.
//Probabilities generated using a python script with 1 billion trials, so should hopefully be pretty good
//Average price ~126
//~38% chance of needing no wallet, ~45% chance of needing 1, ~17% chance of needing 2
static constexpr std::array<double, 60> ShopPriceProbability= {
    0.005326994, 0.014908518, 0.027114719, 0.041315285, 0.057136304, 0.074325887, 0.092667151, 0.112002061, 0.132198214, 0.153125390,
    0.174696150, 0.196810540, 0.219388148, 0.242361379, 0.265657012, 0.289205134, 0.312970402, 0.336877590, 0.360881110, 0.384932772,
    0.408976198, 0.432982176, 0.456902494, 0.480686053, 0.504313389, 0.527746488, 0.550938554, 0.573856910, 0.596465330, 0.618736235,
    0.640646600, 0.662162782, 0.683240432, 0.703859801, 0.724001242, 0.743631336, 0.762722631, 0.781259986, 0.799198449, 0.816521905,
    0.833208595, 0.849243398, 0.864579161, 0.879211177, 0.893112051, 0.906263928, 0.918639420, 0.930222611, 0.940985829, 0.950914731,
    0.959992180, 0.968187000, 0.975495390, 0.981884488, 0.987344345, 0.991851853, 0.995389113, 0.997937921, 0.999481947, 1.000000000,
};

// Generate random number from 5 to wallet max
int GetPriceFromMax(int max) {
    return Random(1, max) * 5; // random range of 1 - wallet max / 5, where wallet max is the highest it goes as a multiple of 5
}

uint16_t GetPriceFromSettings(Rando::Location *loc, PriceSettingsStruct priceSettings) {
   auto ctx = Rando::Context::GetInstance();
   switch (ctx->GetOption(priceSettings.main).Value<uint8_t>()){
      case RO_PRICE_VANILLA: 
         return loc->GetVanillaPrice();
      case RO_PRICE_CHEAP_BALANCED:
         return GetCheapBalancedPrice();
      case RO_PRICE_BALANCED:{
         double random = RandomDouble(); //Randomly generated probability value
         for (size_t i = 0; i < ShopPriceProbability.size(); i++) {
            if (random < ShopPriceProbability[i]) {
            //The randomly generated value has surpassed the total probability up to this point, so this is the generated price
                  return  i * 5; //i in range [0, 59], output in range [0, 295] in increments of 5
            }
         }
         return 150;
      }
      case RO_PRICE_FIXED:
         return (uint16_t)ctx->GetOption(priceSettings.fixedPrice).Value<uint8_t>() * 5;
      case RO_PRICE_RANGE:{
         uint16_t range1 = (uint16_t)ctx->GetOption(priceSettings.range1).Value<uint8_t>() * 5;
         uint16_t range2 = (uint16_t)ctx->GetOption(priceSettings.range2).Value<uint8_t>() * 5;
         return range1 < range2 ? Random(range1, range2+1) : Random(range2, range1+1);
      }
      case RO_PRICE_SET_BY_WALLET:{
         bool isTycoon = ctx->GetOption(RSK_INCLUDE_TYCOON_WALLET).Value<bool>();
         uint16_t noWeight = ctx->GetOption(priceSettings.noWallet).Value<uint8_t>();
         uint16_t childWeight = ctx->GetOption(priceSettings.childWallet).Value<uint8_t>();
         uint16_t adultWeight = ctx->GetOption(priceSettings.adultWallet).Value<uint8_t>();
         uint16_t giantWeight = ctx->GetOption(priceSettings.giantWallet).Value<uint8_t>();
         uint16_t tycoonWeight = isTycoon ? ctx->GetOption(priceSettings.tycoonWallet).Value<uint8_t>() : 0;
         uint16_t totalWeight = noWeight + childWeight + adultWeight + giantWeight + tycoonWeight;
         if (totalWeight == 0){ //if no weight, return from sane range
            return Random(0, 501);
         }
         int16_t selected = Random(1, totalWeight + 1);
         selected = selected - noWeight;
         if (selected <= 0){
            return 0;
         }
         selected = selected - childWeight;
         if (selected <= 0){
            return Random(1, 100);
         }
         selected = selected - adultWeight;
         if (selected <= 0){
            return Random(100, 201);
         }
         selected = selected - giantWeight;
         if (selected <= 0){
            return Random(201, 501);
         }
         return Random(501, 999);
      }
   }
   SPDLOG_ERROR("GetPriceFromSettings has failed to return a price for location {}, assigning a default value.", loc->GetName());
   assert(false);
   return 69; //this should never happen, if it does, EASTER EGG that tells us something is wrong
}

uint16_t GetRandomPrice(Rando::Location *loc, PriceSettingsStruct priceSettings) {
   uint16_t initialPrice = GetPriceFromSettings(loc, priceSettings);
   auto ctx = Rando::Context::GetInstance();
   if (ctx->GetOption(priceSettings.affordable) && !ctx->GetOption(priceSettings.main).Is(RO_PRICE_FIXED)){
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

//Similar to above, beta distribution with alpha = 1, beta = 2,
// multiplied by 20 instead of 60 to give values in rage [0, 95] in increments of 5
//Average price ~31
static constexpr std::array<double, 20> CheapPriceProbability = {
    0.097500187, 0.190002748, 0.277509301, 0.360018376, 0.437522571, 0.510021715, 0.577520272, 0.640029304, 0.697527584, 0.750024535,
    0.797518749, 0.840011707, 0.877508776, 0.910010904, 0.937504342, 0.960004661, 0.977502132, 0.989998967, 0.997500116, 1.000000000,
};

uint16_t GetCheapBalancedPrice() {
    double random = RandomDouble();
    for (size_t i = 0; i < CheapPriceProbability.size(); i++) {
        if (random < CheapPriceProbability[i]) {
            return i * 5; // i in range [0, 19], output in range [0, 95] in increments of 5
        }
    }
    return -1;
}

//Get 0 to 7, or a random number from 1-7 depending on shopsanity setting
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
            return 8; //temporarily unreachable due to logic limitations
        }
    } else { //Random, get number in [1, 7]
        return Random(1, 8);
    }
}

//Initialize the table of trick names with an easy, medium, and hard name for each language
void InitTrickNames() {
    trickNameTable[RG_KOKIRI_SWORD] = {
        Text{"Korok Sword", "Épée Korok", "Espada Korok"},
        Text{"Hero's Sword", "Épée du Héros", "Espada del héroe"},
        Text{"Razor Sword", "Lame Rasoir", "Espada de esmeril"}
    };
    trickNameTable[RG_MASTER_SWORD] = {
        Text{"Goddess Sword", "Épée de la déesse", "Espada Divina"},
        Text{"Gilded Sword", "Excalibur", "Espada de los Sabios"},
        Text{"Magical Sword", "Lame dorée", "Fay"}
    };
    trickNameTable[RG_GIANTS_KNIFE] = {
        Text{"Medigoron's Sword", "l'Épée de Medigoron", "La espada de Medigoron"},
        Text{"Razor Sword", "Lame Rasoir", "Espada de esmeril"},
        Text{"Royal Claymore", "Claymore Royale", "Royal Claymore"}
    };
    trickNameTable[RG_BIGGORON_SWORD] = {
        Text{"Power Sword", "Épée de Puissance", "Espada de poder"},
        Text{"Fierce Deity Sword", "Épée du dieu démon", "Espada de la Fiera Deidad"},
        Text{"Tempered Sword", "Épée de Légende Nv.2", "Espada Maestra mejorada"},
        Text{"Biggoron's Knife", "Lame de Grogoron", "Daga de Biggoron"}
    };
    trickNameTable[RG_DEKU_SHIELD] = {
        Text{"Boko Shield", "Bouclier Boko", "Escudo Boko"},
        Text{"Ordon Shield", "Bouclier de Toal", "Escudo de Ordon"},
        Text{"Wooden Shield", "Bouclier de Bois", "Escudo de madera"}
    };
    trickNameTable[RG_HYLIAN_SHIELD] = {
        Text{"Hyrule Shield", "Bouclier d'Hyrule", "Escudo Hylian"},
        Text{"Goddess Shield", "Bouclier Sacré", "Escudo Divino"},
        Text{"Hero's Shield", "Bouclier du Héros", "Escudo del héroe"}
    };
    trickNameTable[RG_MIRROR_SHIELD] = {
        Text{"Magic Mirror", "Miroir Magique", "Escudo mágico"},
        Text{"Magical Shield", "Bouclier Magique", "Escudo arcano"},
        Text{"Mirror of Twilight", "Miroir des Ombres", "Espejo del Crepúsculo"}
    };
    trickNameTable[RG_GORON_TUNIC] = {
        Text{"Gerudo Top", "Tunique Gerudo", "Pechera gerudo"},
        Text{"Flamebreaker Armor", "Armure de Pierre", "	Armadura ignífuga"},
        Text{"Red Mail", "Habits Rouges", "Ropas rojas"}
    };
    trickNameTable[RG_ZORA_TUNIC] = {
        Text{"Rito Tunic", "Tunique Rito", "Sayo rito"},
        Text{"Mermaid Suit", "Costume de sirène", "Costume de sirène"},
        Text{"Zora Armor", "Armure Zora", "Túnica Zora"},
        Text{"Blue Mail", "Habits Bleus", "Ropas azules"}
    };
    trickNameTable[RG_IRON_BOOTS] = {
        Text{"Iron Hoofs", "Patins de Plomb", "Botas férreas"},
        Text{"Snow Boots", "Bottes de Neige", "Botas de nieve"},
        Text{"Red Boots", "Bottes rouges", "Botas rojas"},
        Text{"Zora Greaves", "Bottes Zora", "Zora Greaves"},
        Text{"Boots of Power", "Bottes de Puissance", "Botas de plomo"}
    };
    trickNameTable[RG_HOVER_BOOTS] = {
        Text{"Hover Hoofs", "Patins des airs", "Botas flotadoras"},
        Text{"Golden Boots", "Bottes dorées", "Botas de Oro"},
        Text{"Pegasus Boots", "Bottes pégase", "Botas de Pegaso"},
        Text{"Boots of Speed", "Bottes de vitesse", "Botas del desierto"}
    };
    trickNameTable[RG_WEIRD_EGG] = {
        Text{"Poached Egg", "Oeuf à la coque", "Huevo pasado"},
        Text{"Lon Lon Egg", "Oeuf Lon Lon", "Huevo Lon Lon"},
        Text{"Zora Egg", "Oeuf Zora", "Huevo Zora"}
    };
    trickNameTable[RG_ZELDAS_LETTER] = {
      Text{"Ruto's Letter", "Lettre de Ruto", "Carta de Ruto"},
        Text{"Royal Letter", "Lettre Eoyale", "Carta para Kafei"},
        Text{"Zelda's Business Card", "Carte d'affaires de Zelda", "Carta"},
        Text{"Letter to Kafei", "Lettre pour Kafei", "Carta para Kafei "},
        Text{"Goat's Letter", "Lettre de la Chèvre", "Carta de la Cabra"},
        Text{"Maggie's Letter", "Lettre de Maggy", "Carta de Dolores"}
    };
    trickNameTable[RG_BOOMERANG] = {
        Text{"Banana", "Banane", "Plátano"},
        Text{"Prank Fetch Toy", "Inséparable Bâtonnet", "Bumerang"},
        Text{"Gale Boomerang", "Boomerang Tornade", "Bumerán tornado"},
        Text{"Magic Boomerang", "Boomerang Magique", "Bumerán mágico"}
    };
    trickNameTable[RG_LENS_OF_TRUTH] = {
        Text{"Sheikah-leidoscope", "Sheikah-léidoscope", "Monóculo de la Verdad"},
        Text{"Sheikah Sensor", "Sonar Sheikah", "Sensor Sheikah"},
        Text{"Crystal of Vision", "Cristal de Vision", "Cristal de Visión"},
        Text{"Magnifying Lens", "Loupe", "Lente Aumentadora"}
    };
    trickNameTable[RG_MEGATON_HAMMER] = {
        Text{"Goron Gavel", "Masse Perforatrice", "Mazo Goron"},
        Text{"Magic Hammer", "Marteau Magique", "Martillo mágico"},
        Text{"Skull Hammer", "Maillet Ressort", "Martillo de hierro"}
    };
    trickNameTable[RG_STONE_OF_AGONY] = {
        Text{"Cave Charm", "Charme de grotte", "Amuleto de la cueva"},
        Text{"Stone of Agahnim", "Fragment d'Agahnim", "Piedra de Agahnim"},
        Text{"Shard of Agony", "Fragment de Souffrance", "Piedra de la Agonía"},
        Text{"Pirate's Charm", "Pierre de Pirate", "Amuleto Pirata"}
    };
    trickNameTable[RG_DINS_FIRE] = {
        Text{"Eldin's Fire", "Feu d'Eldin", "Fuego de Eldin"},
        Text{"Din's Blaze", "Flamme de Din", "Poder de Din"},
        Text{"Magic Lantern", "Lanterne Magique", "Linterna mágica"},
        Text{"Ether Medallion", "Médaillon d'Éther", "Medallón de Tesoro"},
        Text{"Bombos Medallion", "Médaillon des Flammes", "Medallón del Temblor"}
    };
    trickNameTable[RG_FARORES_WIND] = {
        Text{"Faron's Wind", "Vent de Firone", "Viento de Farone"},
        Text{"Farore's Windfall", "Zéphyr de Farore", "Valor de Farore"},
        Text{"Tingle Air", "Tingle Air", "Tingle de aire"},
        Text{"Travel Medallion", "Amulette de téléportation", "Medallón Maligno"},
        Text{"Irene's Taxi", "Le taxi d'Aëline", "El taxi de Airín"}
    };
    trickNameTable[RG_NAYRUS_LOVE] = {
        Text{"Lanayru's Love", "Amour de Lanelle", "Amor de Lanayru"},
        Text{"Nayru's Passion", "Passion de Nayru", "Sabiduría de Nayru"},
        Text{"Tingle Shield", "Bouclier Tingle", "Escudo de hormigueo"},
        Text{"Shield Spell", "Bouclier Magique", "Hechizo de Protección"},
        Text{"Magic Armor", "Armure Magique", "Armadura mágica"}
    };
    trickNameTable[RG_FIRE_ARROWS] = {
        Text{"Fire Rod", "Baguette de feu", "Cetro de fuego"},
        Text{"Bomb Arrow", "Flèche-Bombe", "Flecha bomba"},
        Text{"Red Candle", "Bougie Rouge", "Vela roja"}
    };
    trickNameTable[RG_ICE_ARROWS] = {
        Text{"Ice Rod", "Baguette des Glaces", "Cetro de Hielo"},
        Text{"Ancient Arrow", "Flèche Archéonique", "Flecha ancestral"},
        Text{"Ice Trap Arrow", "Flèche de Piège de Glace", "Cetro de hielo"}
    };
    trickNameTable[RG_LIGHT_ARROWS] = {
        Text{"Wind Arrow", "Flèche de Vent", "Flecha del Viento"},
        Text{"Wand of Gamelon", "Baguette de Gamelon", "Varita de Gamelón"},
        Text{"Shock Arrow", "Flèches Électriques", "Flecha eléctrica"},
        Text{"Silver Arrow", "Flèches d'Argent", "Flecha de plata"}
    };
    trickNameTable[RG_GERUDO_MEMBERSHIP_CARD] = {
        Text{"Desert Title Deed", "Abonnement Gerudo", "Escritura del desierto"},
        Text{"Sickle Moon Flag", "Drapeau du croissant de lune", "Bandera de la Luna Creciente"},
        Text{"Complimentary ID", "Bon de félicitation", "Cupón especial"},
        Text{"Gerudo's Card", "Carte Goron", "Tóken Gerudo"},
        Text{"Gerudo's Membership Card", "Autographe de Nabooru", "Tarjeta Gerudo"}
    };

    trickNameTable[RG_MAGIC_BEAN_PACK] = {
        Text{"Funky Bean Pack", "Paquet de Fèves Magiques", "Lote de frijoles mágicos"},
        Text{"Grapple Berries", "Baies de grappin", "Bayas de garfio"},
        Text{"Crenel Bean Pack", "Paquet de Haricots Gonggle", "Lote de alubias mágicas"},
        Text{"Mystical Seed Pack", "Pack de graines mystiques", "Paquete de semillas místicas"}
    };
    trickNameTable[RG_DOUBLE_DEFENSE] = {
        Text{"Diamond Hearts", "Coeurs de Diamant", "Contenedor de diamante"},
        Text{"Double Damage", "Double Souffrance", "Doble daño receptivo"},
        Text{"Quadruple Defence", "Quadruple Défence", "Defensa cuádruple"}
    };

    trickNameTable[RG_POCKET_EGG] = {
        Text{"Arpagos Egg", "Oeuf d'Arpagos", "Huevo de Arpagos"},
        Text{"Lon Lon Egg", "oeuf Lon Lon", "Huevo Lon Lon"},
        Text{"Zora Egg", "oeuf Zora", "Huevo del Pez Viento"}
    };
    trickNameTable[RG_POCKET_EGG] = {
        Text{"D.I.Y. Alarm Clock", "Réveille-matin improvisé", "Alarma emplumada portátil"},
        Text{"Kakariko Cucco", "Cocotte Cocorico", "Cuco de Kakariko"},
        Text{"Hatched Cucco", "Cocotte éclose", "Pollo de bolsillo"}
    };
    trickNameTable[RG_COJIRO] = {
        Text{"Blucco", "Chair-Qui-Poule", "Cucazul"},
        Text{"Piyoko", "Piyoko", "Piyoko"},
        Text{"Dark Cucco", "Cocotte Sombre", "Cucco oscuro"},
        Text{"Grog's Cucco", "Cocotte de Grog", "Cuco de Grog"}
    };
    trickNameTable[RG_ODD_MUSHROOM] = {
        Text{"Magic Mushroom", "Champignon magique", "Champiñón mágico"},
        Text{"Endura Shroom", "Champi Vigueur", "Champiñón del bosque"},
        Text{"Sleepy Toadstool", "Crapaud Fatigué", "Seta durmiente"},
        Text{"Mushroom", "Champignon", "Seta"}
    };
    trickNameTable[RG_ODD_POTION] = {
        Text{"Odd Medicine", "Élixir suspect", "Poción rara"},
        Text{"Granny's Poultice", "Mixture de Granny", "Medicina de la abuela"},
        Text{"Mushroom Poultice", "Mixture de champignon", "Medicina de champiñones"},
        Text{"Secret Medicine", "Médicament", "Pócima secreta"},
        Text{"Mushroom Spores", "Spores de Champignons", "Esporas de hongos"},
        Text{"Hanyu Spore", "Hanyu Spore", "Espora Hanyu"}
    };
    trickNameTable[RG_POACHERS_SAW] = {
        Text{"Carpenter's Saw", "Scie du charpentier", "Sierra del carpintero"},
        Text{"Poacher's Sword", "Hache du chasseur", "Espada del capataz"},
        Text{"Ancient Bladesaw", "Longue Épée Archéonique", "Mandoble ancestral"},
        Text{"Woodcutter's Axe", "Hache du Bûcheron", "Hacha de leñador"},
        Text{"Grog's Saw", "Scie de Grog", "Sierra del Cazador Furtivo"}
    };
    trickNameTable[RG_BROKEN_SWORD] = {
        Text{"Broken Biggoron's Sword", "Épée brisée de Grogoron", "Espada de Biggoron rota"},
        Text{"Broken Giant's Knife", "Lame des Géants brisée", "Daga gigante rota"},
        Text{"Broken Noble Sword", "Épée noble brisée", "Espada noble rota"},
        Text{"Broken Picori Blade", "Épée Minish brisée", "Espada minish rota"},
        Text{"Decayed Master Sword", "Épée de légende pourrie", "Espada decadente de leyenda"}
    };
    trickNameTable[RG_PRESCRIPTION] = {
        Text{"Biggoron's Prescription", "Ordonnance de Grogoron", "Receta de Biggoron"},
        Text{"Eyedrop Prescription", "Ordonnance de gouttes", "Receta ocular"},
        Text{"Urgent Prescription", "Ordonnance urgente", "Prescripción"},
        Text{"Swordsman's Scroll", "Précis d'escrime", "Esgrimidorium"},
        Text{"Portrait of Oren", "Portrait d'Orlène", "Retrato de Oren"},
        Text{"Letter to King Zora", "Lettre au roi Zora", "Carta al Rey Zora"}
    };
    trickNameTable[RG_EYEBALL_FROG] = {
        Text{"Don Gero", "Don Gero", "Don Gero"},
        Text{"Hot-Footed Frog", "Grenouille à pieds chauds", "Rana de patas calientes"},
        Text{"Lost Swordsmith", "Forgeron perdu", "Espadachín perdido"},
        Text{"Eyedrop Frog", "Grenouille-qui-louche", "Globo Ocular de Rana"}
    };
    trickNameTable[RG_EYEDROPS] = {
        Text{"Biggoron's Eyedrops", "Gouttes de Grogoron", "Gotas de Biggoron"},
        Text{"Hyrule's Finest Eyedrops", "Eau du Lac Hylia", "Gotas oculares"},
        Text{"Moon's Tear", "Larme de Lune", "Lágrima de Luna"},
        Text{"Engine Grease", "Graisse moteur", "Grasa del motor"},
        Text{"Zora Perfume", "Parfum Zora", "Perfume Zora"}
    };
    trickNameTable[RG_CLAIM_CHECK] = {
        Text{"Clay Check", "Certificat Grogoron", "Comprobante de Reclamación"},
        Text{"Ancient Tablet", "Stèle ancienne", "Litografía arcana"},
        Text{"Sheikah Slate", "Tablette Sheikah", "Piedra Sheikah"},
        Text{"Cyclone Slate", "Ardoise des tornades", "Pizarra de los Torbellinos"}
    };

    trickNameTable[RG_GOLD_SKULLTULA_TOKEN] = {
        Text{"Skulltula Token", "Bon de Skulltula dorée", "Símbolo de Skulltula"},
        Text{"Golden Skulltula Spirit", "Pièce de Skulltula dorée", "Tóken de Skulltula Dorada"},
        Text{"Gold Walltula Token", "Jeton de Walltula dorée", "Skulltula dorada"},
        Text{"Maiamai", "Ti'gorneau", "Maimai"},
        Text{"Gratitude Crystal", "Cristal de gratitude", "Gema de gratitud"},
        Text{"Korok Seed", "Noix korogu", "Semilla de kolog"}
    };

    trickNameTable[RG_PROGRESSIVE_HOOKSHOT] = {
        Text{"Progressive Grappling Hook", "Lance-chaîne (prog.)", "Garra progresiva"},
        Text{"Progressive Clawshot", "Grappin-griffe (prog.)", "Zarpa progresiva"},
        Text{"Progressive Gripshot", "Grappince (prog.)", "Enganchador progresivo"},
        Text{"Progressive Rope", "Corde (prog.)", "Cuerda progresivo"}
    };
    trickNameTable[RG_PROGRESSIVE_STRENGTH] = {
        Text{"Power Glove", "Gant de Puissance (prog.)", "Guanteletes progresivos"},
        Text{"Power Bracelet", "Bracelet de Force (prog.)", "Brasaletes progresivos"},
        Text{"Magic Bracelet", "Bracelet Magique (prog.)", "Manoplas progresivas"}
    };
    trickNameTable[RG_PROGRESSIVE_BOMB_BAG] = {
        Text{"Progressive Bomb Capacity", "Capacité de bombes (prog.)", "Mayor capacidad de bombas"},
        Text{"Progressive Bomb Pack", "Paquet de bombes (prog.)", "Zurrón de bombas progresivo"},
        Text{"Progressive Bomb Box", "Boîte à bombes (prog.)", "Bolsa de bombas progresiva"},
        Text{"Progressive Blast Mask", "Masque d'Explosion (prog.)", "Máscara explosiva progresiva"},
        Text{"Progressive Powder Kegs", "Baril de Poudre (prog.)", "Barril de polvo progresivo"},
        Text{"Progressive Remote Bombs", "Bombes à distance (prog.)", "Bombas remotas progresivas"}
    };
    trickNameTable[RG_PROGRESSIVE_BOW] = {
        Text{"Progressive Arrow Capacity", "Capacité de flèches (prog.)", "Mayor capacidad de flechas"},
        Text{"Progressive Hero's Bow", "Arc du héros (prog.)", "Arco del héroe progresivo"},
        Text{"Progressive Arrow Holder", "Arbalète (prog.)", "Ballesta progresiva"},
        Text{"Progressive Crossbow", "Arbalète (prog.)", "Ballesta progresiva"},
        Text{"Progressive Sacred Bow", "Arc sacré (prog)", "Arco Sagrado Progresivo"},
        Text{"Progressive Lynel Bow", "Arc de Lynel (prog.)", "Arco de centaleón Progresivo"}
    };
    trickNameTable[RG_PROGRESSIVE_SLINGSHOT] = {
        Text{"Progressive Seed Capacity", "Capacité de graines (prog.)", "Mayor capacidad de semillas"},
        Text{"Progressive Catapult", "Catapulte (prog.)", "Catapulta progresiva"},
        Text{"Progressive Scattershot", "Lance-Pierre rafale (prog.)", "Resortera múltiple progresiva"},
        Text{"Progressive Seed Launcher", "Lanceur de semences (prog.)", "Lanzador de semillas progresivo"},
        Text{"Progressive Seed Satchel", "Sac de graines (prog.)", "Bolsa de semillas progresiva"}
    };
    trickNameTable[RG_PROGRESSIVE_WALLET] = {
        Text{"Progressive Rupee Capacity", "Capacité de rubis (prog.)", "Mayor capacidad de rupias"},
        Text{"Progressive Purse", "Sacoche (prog.)", "Cartera de rupias progresiva"},
        Text{"Progressive Rupee Bag", "Sac à rubis (prog.)", "Zurrón de rupias progresivo"},
        Text{"Progressive Rupoor Capacity", "Capacité de Roupir (prog.)", "Capacidad progresiva Rupobre"},
        Text{"Progressive Spoils Bag", "Sac à Butin (prog.)", "Bolsa de trofeos progresiva"},
        Text{"Progressive Ruby Bag", "Capacité du sac Ruby (prog.)", "Bolso Ruby progresivo"}
    };
    trickNameTable[RG_PROGRESSIVE_SCALE] = {
        Text{"Progressive Flippers", "Palmes de Zora (prog.)", "Aletas de zora progresiva"},
        Text{"Progressive Dragon's Scale", "Écaille du dragon d'eau (prog.)", "Escama dragón acuático progresiva"},
        Text{"Progressive Diving Ability", "Plongée (prog.)", "Buceo progresivo"},
        Text{"Progressive Pearl", "Perle (prog.)", "Perla progresiva"},
        Text{"Progressive Scute", "Bulle (prog.)", "Fragmento Zora progresivo"}
    };
    trickNameTable[RG_PROGRESSIVE_NUT_UPGRADE] = {
        Text{"Progressive Nut Pack", "Paquet de noix (prog.)", "Mayor capacidad de semillas"},
        Text{"Progressive Bait Bag", "Sac à Appâts (prog.)", "Bolsa de cebo progresiva"},
        Text{"Progressive Pear Capacity", "Capacité de poire (prog.)", "Capacidad progresiva de pera"},
        Text{"Progressive Nut Bag", "Sac de noix (prog.)", "Bolsa de nueces progresiva"},
        Text{"Progressive Husk Capacity", "Capacité de noisettes (prog.)", "Mayor capacidad de castañas"}
    };
    trickNameTable[RG_PROGRESSIVE_STICK_UPGRADE] = {
        Text{"Progressive Stick Bag", "Sac de bâtons (prog.)", "Mayor capacidad de ramas deku"},
        Text{"Progressive Stick Pack", "Paquet de bâtons Mojo (prog.)", "Mayor capacidad de bastones"},
        Text{"Progressive Branch Capacity", "Capacité de la succursale (prog.)", "Capacidad progresiva de la sucursal"},
        Text{"Progressive Rod Capacity", "Capacité de tiges (prog.)", "Mayor capacidad de cetros deku"}
    };
    trickNameTable[RG_PROGRESSIVE_BOMBCHUS] = {
        Text{"Progressive Bomblings", "Bombinsectes (prog.)", "Bombinsectos progresivos"},
        Text{"Progressive Sentrobe Bombs", "Bombe de Sphérodrone (prog.)", "Bomba de helicobot progresivo"},
        Text{"Progressive Bomb-ombs", "Bombe Soldat (prog.)", "Soldado bomba progresivo"},
        Text{"Progressive Missiles", "Missiles (prog.)", "Misiles progresivos"},
        Text{"Progressive Bombchu Bag", "Sac à Bombchu (prog.)", "Bombachus progresivos"}
    };
    trickNameTable[RG_PROGRESSIVE_MAGIC_METER] = {
        Text{"Progressive Stamina Meter", "Jauge d'endurance (prog.)", "Medidor de vigor progresivo"},
        Text{"Progressive Energy Gauge", "Jauge d'énergie (prog.)", "Medidor de energía progresivo"},
        Text{"Progressive Magic Powder", "Poudre magique (prog.)", "Medidor de carga progresivo"}
    };
    trickNameTable[RG_PROGRESSIVE_OCARINA] = {
        Text{"Progressive Memento", "Souvenir (prog.)", "Silbato progresivo"},
        Text{"Progressive Whistle", "Siffler (prog.)", "Silbido progresivo"},
        Text{"Progressive Flute", "Flûte (prog.)", "Flauta progresiva"},
        Text{"Progressive Recorder", "Harmonica (prog.)", "Armónica progresiva"}
    };
    trickNameTable[RG_PROGRESSIVE_GORONSWORD] = {
        Text{"Progressive Titan Blade", "Lame des Titans (prog.)", "Hoja del Titán progresiva"},
        Text{"Progressive Goron Knife", "Lame Goron (prog.)", "Daga Goron progresiva"},
        Text{"Progressive Giant Sword", "Épée géante (prog.)", "Espada gigante progresiva"},
        Text{"Progressive Darknut Sword", "Épée de Darknut (prog.)", "Espada Darknut progresiva"},
        Text{"Progressive Power Sword", "Épée de Puissance (prog.)", "Espada de poder progresiva"},
        Text{"Progressive Big Stabby", "Gros coup de poignard (prog.)", "Gran puñalada progresiva"}
    };

    trickNameTable[RG_EMPTY_BOTTLE] = {
        Text{"Empty Canteen", "Cantine vide", "cantimplora vacía"},
        Text{"Vial of Winds", "Fiole de vents", "Vial de Vientos"},
        Text{"Tingle Bottle", "Flacon de Tingle", "Botella de Tingle"},
        Text{"Magic Bottle", "Flacon magique", "Frasco feérico"},
        Text{"Glass Bottle", "Flacon de verre", "Botella de cristal"},
        Text{"Bottle with Water", "Flacon d'eau", "Botella Tingle"}
    };
    trickNameTable[RG_BOTTLE_WITH_MILK] = {
        Text{"Bottle with Chateau Romani", "Flacon de cuvée Romani", "Botella de Reserva Romani"},
        Text{"Bottle with Premium Milk", "Flacon avec lait de qualité supérieure", "Biberón con leche Premium"},
        Text{"Bottle with Mystery Milk", "Flacon de lait grand cru", "Botella de leche extra"},
        Text{"Bottle with Fresh Milk", "Flacon de lait frais", "Botella de leche fresca"},};
    trickNameTable[RG_BOTTLE_WITH_RED_POTION] = {
        Text{"Bottle with Red Chu Jelly", "Flacon de gelée Chuchu rouge", "Jugo de Chuchu Rojo"},
        Text{"Bottle with Hibiscus Potion", "Flacon de potion de Hibiscus", "Botella de poción de Hibisco"},
        Text{"Bottle with Medicine of Life", "Flacon d'élixir rouge", "Botella de medicina de la vida"},
        Text{"Bottle with Heart Potion", "Flacon de potion de soin", "Botella de poción de salud"}
    };
    trickNameTable[RG_BOTTLE_WITH_GREEN_POTION] = {
        Text{"Bottle with Green Chu Jelly", "Flacon de gelée Chuchu verte", "Jugo de Chuchu Verde"},
        Text{"Bottle with Lamp Oil", "Flacon de Huile à lanterne", "Botella de Aceite de candil "},
        Text{"Bottle with Medicine of Magic", "Flacon d'élixir vert", "Botella de medicina mágica"},
        Text{"Bottle with Stamina Potion", "Flacon d'Endurol", "Botella de elixir vigorizante"}
    };
    trickNameTable[RG_BOTTLE_WITH_BLUE_POTION] = {
        Text{"Bottle with Blue Chu Jelly", "Flacon de gelée Chuchu bleue", "Jugo de Chuchu Azul"},
        Text{"Bottle with Water of Life", "Flacon d'élixir bleu", "Botella de agua de la vida"},
        Text{"Bottle with Air Potion", "Flacon de potion d'oxygène", "Botella de oxígeno"}
    };
    trickNameTable[RG_BOTTLE_WITH_FAIRY] = {
        Text{"Bottle with Forest Firefly", "Flacon avec une luciole", "Luciérnaga del bosque"},
        Text{"Bottle with Deku Princess", "Flacon avec Deku Princess", "Botella con Deku Princess"},
        Text{"Bottle with Stray Fairy", "Flacon avec une fée perdue", "Hada perdida en una botella"}
    };
    trickNameTable[RG_BOTTLE_WITH_FISH] = {
        Text{"Bottle with Small Jabu-Jabu", "Flacon avec mini Jabu-Jabu", "Lord Chapu-Chapu embotellado"},
        Text{"Bottle with Reekfish", "Flacon avec Reekfish", "Reekfish embotellada"},
        Text{"Bottle with Hyrule Bass", "Flacon avec perche d'Hyrule", "Locha de Hyrule embotellada"},
        Text{"Bottle with Hyrule Loach", "Flacon avec loche d'Hyrule", "Perca de Términa embotellada"}
    };
    trickNameTable[RG_BOTTLE_WITH_BLUE_FIRE] = {
        Text{"Bottle with Will-O-Wisp", "Flacon avec feu follet", "Botella de llama azul"},
        Text{"Bottle with Ancient Flame", "Flacon de flamme ancienne", "Botella de fuego ancestral"},
        Text{"Bottle with a Blue Candle", "Flacon avec une bougie bleue", "Botella con una vela azul"},
        Text{"Bottle with Red Ice", "Flacon de Glace Rouge", "Botella de Hielo rojo"},
        Text{"Bottle with Nayru's Flame", "Flacon de flamme de Nayru", "Botella de llamas de Nayru"}
    };
    trickNameTable[RG_BOTTLE_WITH_BUGS] = {
        Text{"Bottle with Baby Tektites", "Flacon de bébé Araknon", "Tektites en una botella"},
        Text{"Bottle with A Beetle", "Flacon avec un scarabée", "Botella con un escarabajo"},
        Text{"Bottle with Lanayru Ants", "Flacon de fourmis de Lanelle", "Celestarabajo embotellado"},
        Text{"Bottle with Insects", "Flacon de bibittes", "Saltabosques embotellados"},
        Text{"Bottle with a Golden Bee", "Flacon avec une abeille dorée", "Botella con una abeja dorada"}
    };
    trickNameTable[RG_BOTTLE_WITH_POE] = {
        Text{"Bottle with Ghini", "Flacon avec Ghini", "Ghini en una botella"},
        Text{"Bottle with Reapling", "Flacon avec Âme Damnée", "Reapling en una botella"},
        Text{"Bottle with Imp Poe", "Flacon avec Spectre", "Espectro en una botella"},
        Text{"Bottle with Anti-Fairy", "Flacon avec Tetdoss", "Whisp en una botella"}
    };

    trickNameTable[RG_RUTOS_LETTER] = {
        Text{"Bottle with Maggie's Letter", "Flacon avec lettre de Maggy", "Carta de Dolores"},
        Text{"Bottle with Letter to Kafei", "Flacon avec lettre pour Kafei", "Carta para Kafei"},
        Text{"Bottle with Zelda's Letter", "Flacon avec Lettre de Zelda", "Carta náutica"}
    };
    trickNameTable[RG_BOTTLE_WITH_BIG_POE] = {
        Text{"Bottle with Composer Brother", "Flacon avec un compositeur", "Hermana Poe embotellada"},
        Text{"Bottle with Jalhalla", "Flacon avec Jalhalla", "Yaihalla embotellado"},
        Text{"Bottle with Grim Repoe", "Flacon avec le Faucheur", "Bubble en una botella"}
    };

    trickNameTable[RG_ZELDAS_LULLABY] = {
        Text{"Ballad of the Goddess", "Chant de la déesse", "Cántico de la Diosa"},
        Text{"Song of Healing", "Chant de l'apaisement", "Canción de curación"},
        Text{"Song of the Hero", "Chant du héros", "Canción del héroe"}
    };
    trickNameTable[RG_EPONAS_SONG] = {
        Text{"Song of Birds","Chant des oiseaux","Cantar del ave"},
        Text{"Song of Soaring", "Chant de l'envol", "Canción del viento"},
        Text{"Song of Horse", "Chant du cheval", "Chant du cheval"}
    };
    trickNameTable[RG_SARIAS_SONG] = {
        Text{"Mido's Song", "La chanson de Mido", "La canción de Mido"},
        Text{"Kass' Theme", "Le thème de Kass", "El tema de Kass"},
        Text{"Tune of Echoes", "Chant des Échos ", "Melodía del Eco "}
    };
    trickNameTable[RG_SUNS_SONG] = {
        Text{"Song of Passing", "Mambo de Manbo", "Melodía del transcurrir"},
        Text{"Command Melody", "Air du marionnettiste", "Cara al Sol"},
        Text{"Moon's Song", "La chanson de Moon", "La canción de la luna"}
    };
    trickNameTable[RG_SONG_OF_TIME] = {
        Text{"Song of Double Time", "Chant accéléré", "Canción del doble tiempo"},
        Text{"Inverted Song of Time", "Chant du temps inversé", "Canción del tiempo invertida"},
        Text{"Tune of Ages", "Chant du Temps", "Melodía del Tiempo"}
    };
    trickNameTable[RG_SONG_OF_STORMS] = {
        Text{"Ballad of Gales", "Requiem de la tornade", "Melodía del Tornado"},
        Text{"Frog's Song of Soul", "Rap des grenouilles", "Canción del alma de la rana"},
        Text{"Wind's Requiem", "Mélodie du vent", "Melodía del Viento"}
    };
    trickNameTable[RG_MINUET_OF_FOREST] = {
        Text{"Saria's Karaoke", "Karaoké de Saria", "Dueto del bosque"},
        Text{"Sonata of Awakening", "Sonate de l'éveil", "Sonata del despertar"},
        Text{"Wind God's Aria", "Hymne du dieu du vent", "Melodía del Espíritu del Viento"}
    };
    trickNameTable[RG_BOLERO_OF_FIRE] = {
        Text{"Darunia's Tango", "Tango de Darunia", "Coro del fuego"},
        Text{"Tune of Currents", "Chants des Flux", "Melodía de las Corrientes"},
        Text{"Goron Lullaby", "Berceuse des Gorons", "Nana goron"}
    };
    trickNameTable[RG_SERENADE_OF_WATER] = {
        Text{"Ruto's Blues", "Blues de Ruto", "Sonata del agua"},
        Text{"New Wave Bossa Nova", "Bossa-nova des flots", "Bossanova de las olas"},
        Text{"Manbo's Mambo", "Mambo de Manbo", "Mambo de Manbo"}
    };
    trickNameTable[RG_REQUIEM_OF_SPIRIT] = {
        Text{"Nabooru's Reggae", "Reggae de Nabooru", "Reggae del espíritu"},
        Text{"Elegy of Emptiness", "Hymne du vide", "Elegía al vacío"},
        Text{"Earth God's Lyric", "Hymne du dieu de la terre", "Melodía del Espíritu de la Tierra"}
    };
    trickNameTable[RG_NOCTURNE_OF_SHADOW] = {
        Text{"Impa's Death Metal", "Death métal d'Impa", "Diurno de la sombra"},
        Text{"Oath to Order", "Ode de l'appel", "Oda al orden"},
        Text{"Song of Discovery", "Chant des secrets", "Canto revelador"}
    };
    trickNameTable[RG_PRELUDE_OF_LIGHT] = {
        Text{"Rauru's Sing-Along", "Chansonnette de Rauru", "Predulio de luz"},
        Text{"Ballad of the Wind Fish", "Ballade sur Poisson-Rêve", "Balada del Piez Viento"},
        Text{"Song of Light", "Chant de la lumière", "Sonidos de la luz"}
    };

    trickNameTable[RG_KOKIRI_EMERALD] = {
        Text{"Pendant of Courage", "Pendentif du courage", "Colgante del valor"},
        Text{"Farore's Pearl", "Perle de Farore", "Orbe de Farore"},
        Text{"Aquanine", "Smaragdine", "Yerbánida"},
        Text{"Farore's Emerald", "Émeraude de Farore", "Esmeralda de Farore"},
        Text{"Kokiri's Peridot", "Péridot Kokiri", "Ágata de los Kokiri"}
    };
    trickNameTable[RG_GORON_RUBY] = {
        Text{"Pendant of Power", "Pendentif de la force", "Colgante del poder"},
        Text{"Din's Pearl", "Perle de Din", "Orbe de Din"},
        Text{"Crimsonine", "Alzanine", "Bermellina"},
        Text{"Din's Ruby", "Rubis de Din", "Rubí de Din"},
        Text{"Goron's Garnet", "Grenat Goron", "Topacio de los Goron"}
    };
    trickNameTable[RG_ZORA_SAPPHIRE] = {
        Text{"Pendant of Wisdom", "Pendentif de la sagesse", "Colgante de la sabiduría"},
        Text{"Nayru's Pearl", "Perle de Nayru", "Orbe de Nayru"},
        Text{"Azurine", "Aquanine", "Azurina"},
        Text{"Nayru's Sapphire", "Saphir de Nayru", "Zafiro de Nayru"},
        Text{"Zora's Aquamarine", "Aquamarine Zora", "Lapislázuli de los Zora"}
    };
    trickNameTable[RG_FOREST_MEDALLION] = {
        Text{"Wind Medallion", "Médaillon du vent", "Medallón del Viento"},
        Text{"Wind Element", "Elément Vent", "Elemento de aire"},
        Text{"Saria's Medallion", "Médaillon de Saria", "Medallón de Saria"},
        Text{"Sign of Air", "Glyphe de l'air", "Glifo de aire"},
        Text{"Medallion of Forest", "Médaillon du Temple de la Forêt", "Medalla del Bosque"}
    };
    trickNameTable[RG_FIRE_MEDALLION] = {
        Text{"Fire Element", "Elément Feu", "Elemento de fuego"},
        Text{"Darunia's Medallion", "Médaillon de Darunia", "Medallón de Darunia"},
        Text{"Sign of Fire", "Glyphe de feu", "Glifo de fuego"},
        Text{"Medallion of Fire", "Médaillon du Temple du Feu", "Medalla del Fuego"}
    };
    trickNameTable[RG_WATER_MEDALLION] = {
        Text{"Water Element", "Elément Eau", "Elemento de agua"},
        Text{"Ice Medallion", "Médaillon de glace", "Medallón Helado"},
        Text{"Ruto's Medallion", "Médaillon de Ruto", "Medallón de Ruto"},
        Text{"Sign of Water", "Glyphe de l'eau", "Glifo de agua"},
        Text{"Medallion of Water", "Médaillon du Temple de l'Eau", "Medalla del Agua"}
    };
    trickNameTable[RG_SPIRIT_MEDALLION] = {
        Text{"Earth Element", "Elément Terre", "Elemento de tierra"},
        Text{"Nabooru's Medallion", "Médaillon de Nabooru", "Medallón de Nabooru"},
        Text{"Sign of Earth", "Glyphe de la Terre", "Glifo de la tierra"},
        Text{"Medallion of Spirit", "Médaillon du Temple de l'Esprit", "Medalla del Espíritu"}
    };
    trickNameTable[RG_SHADOW_MEDALLION] = {
        Text{"Fused Shadow", "Cristal d'ombre", "Sombra Fundida"},
        Text{"Impa's Medallion", "Médaillon d'Impa", "Medallón de Impa"},
        Text{"Sign of Illusion", "Glyphe de l'illusion", "Glifo de ilusión"},
        Text{"Medallion of Shadow", "Médaillon du Temple de l'Ombre", "Medalla de la Sombra"}
    };
    trickNameTable[RG_LIGHT_MEDALLION] = {
        Text{"Compass of Light", "Boussole de lumière", "Brújula de Luz"},
        Text{"Rauru's Medallion", "Médaillon de Rauru", "Medallón de Rauru"},
        Text{"Sign of Destiny", "Glyphe du destin", "Glifo del destino"},
        Text{"Medallion of Light", "Médaillon du temple de lumière", "Medalla de la Luz"}
    };

    trickNameTable[RG_RECOVERY_HEART] = {
        Text{"Love", "Bisou", "Te amo"},
        Text{"Life", "Vie", "vida"},
        Text{"HP", "VP", "VP"}
    };
    trickNameTable[RG_GREEN_RUPEE] = {
        Text{"False Greg", "Faux Greg", "Falso Greg"},
        Text{"One Ruby", "Un rubis", "Un rubí"},
        Text{"Rupoor (1)", "Roupir (1)", "Rupobre (1)"},
        Text{"One Rupee", "Un rubis", "Guaraní hyliano"},
        Text{"Rupee (1)", "Rubis (1)", "Peso hyliano"}
    };
    trickNameTable[RG_BLUE_RUPEE] = {
        Text{"Blupee", "Bleubi", "Azupia"},
        Text{"Five Rubies", "Cinq Rubys", "Cinco rubíes"},
        Text{"Five Rupees", "Cinq rubis", "Bolívar hyliano"},
        Text{"Rupee (5)", "Rubis (5)", "Peso hyliano"},
        Text{"Rupoor (5)", "Roupir (5)", "Rupobre (5)"}
    };
    trickNameTable[RG_RED_RUPEE] = {
        Text{"Big 20", "Grand 20", "Los 20 grandes"},
        Text{"Twenty Rubies", "vingt rubis", "Veinte rubíes"},
        Text{"Rupoor (20)", "Roupir (20)", "Rupobre (20)"},
        Text{"Twenty Rupees", "Vingt rubis", "Colon hyliano"},
        Text{"Rupee (20)", "Rubis (20)", "Peso hyliano"}
    };
    trickNameTable[RG_PURPLE_RUPEE] = {
        Text{"Purpee", "pourbi", "morupiua"},
        Text{"Fifty Rubies", "cinquante rubis", "Cincuenta rubíes"},
        Text{"Rupoor (50)", "Roupir (50)", "Rupobre (50)"},
        Text{"Fifty Rupees", "Cinquante rubis", "Balboa hyliano"},
        Text{"Rupee (50)", "Rubis (50)", "Peso hyliano"}
    };
    trickNameTable[RG_HUGE_RUPEE] = {
        Text{"Hugo", "Or Rubi", "Oro Rubi"},
        Text{"Two Hundred Rubies", "deux cents rubis", "Doscientos rubíes"},
        Text{"Diamond", "Diamant", "Diamante"},
        Text{"Huge Ruby", "Énorme rubis", "Rubi gigante"},
        Text{"Two Hundred Rupees", "Deux cent rubis", "Euro hyliano"},
        Text{"Rupee (200)", "Rubis (200)", "Dólar hyliano"}
    };
    trickNameTable[RG_PIECE_OF_HEART] = {
        Text{"Pizza Heart", "Fromage de cœur", "Pieza de Chorizo"},
        Text{"Little Bit Of Love", "Un peu d'amour", "Un poco de amor"},
        Text{"Rare Peach Stone", "Pierre de pêche rare", "Pierre de pêche rare"}
    };
    trickNameTable[RG_HEART_CONTAINER] = {
        Text{"Crystal Heart", "Cœur de cristal", "Corazón de cristal"},
        Text{"Life Heart", "Cœur de vie", "Vida Corazón"},
        Text{"Lots of Love", "Beaucoup d'amour", "Mucho amor"}
    };
    trickNameTable[RG_TRIFORCE_PIECE] = {
        Text{"Piece of Cheese", "Morceau de Fromage", "Piece of Cheese"},
        Text{"Triforce Shard", "Éclat de Triforce", "Triforce Shard"},
        Text{"Shiny Rock", "Caiiloux Brillant", "Shiny Rock"}
    };

    trickNameTable[RG_GOHMA_SOUL] = {
        Text{"Spider Sense", "", ""},
        Text{"Deku Spirit", "", ""},
        Text{"Ghost of Ghoma", "", ""}
    };
    trickNameTable[RG_KING_DODONGO_SOUL] = {
        Text{"Lizard Soul", "", ""},
        Text{"Regal Remains", "", ""},
        Text{"Dodongo's Core", "", ""}
    };
    trickNameTable[RG_BARINADE_SOUL] = {
        Text{"Parasitic Poltergeist", "", ""},
        Text{"Jabu Insides", "", ""},
        Text{"Barinade Bacteria", "", ""}
    };
    trickNameTable[RG_PHANTOM_GANON_SOUL] = {
        Text{"Bigger Poe", "", ""},
        Text{"Sacred Forest Pine Tree", "", ""},
        Text{"Ganon's Phantom", "", ""}
    };
    trickNameTable[RG_VOLVAGIA_SOUL] = {
        Text{"Dragon Roast", "", ""},
        Text{"Hot n' Ready", "", ""},
        Text{"Volvagia's Vitality", "", ""}
    };
    trickNameTable[RG_MORPHA_SOUL] = {
        Text{"Dihydrogen Monoxide", "", ""},
        Text{"Morpha Molecules", "", ""},
        Text{"Wet Stuff", "", ""}
    };
    trickNameTable[RG_BONGO_BONGO_SOUL] = {
        Text{"Shadow Soul", "", ""},
        Text{"Dark Essence", "", ""},
        Text{"Bongo Bongo's Bongo", "", ""}
    };
    trickNameTable[RG_TWINROVA_SOUL] = {
        Text{"Sandy Ashes", "", ""},
        Text{"Spiritual Spirit", "", ""},
        Text{"Twin Rovers", "", ""}
    };
    trickNameTable[RG_GANON_SOUL] = {
        Text{"Pure Evil", "", ""},
        Text{"Ganon's Ghost", "", ""},
        Text{"Pork", "", ""}
    };

    trickNameTable[RG_FISHING_POLE] = {
        Text{"Fish Tickler", "Fish Tickler", "Fish Tickler"},
        Text{"Floating Lure", "Floating Lure", "Floating Lure"},
        Text{"Fishing Reel", "Fishing Reel", "Fishing Reel"}
    };

    trickNameTable[RG_OCARINA_A_BUTTON] = {
        Text{"Ocarina J Button", "", ""},
        Text{"Ocarina Ayy Button", "", ""},
        Text{"Ocarina A Trigger", "", ""}
    };
    trickNameTable[RG_OCARINA_C_UP_BUTTON] = {
        Text{"Ocarina C North Button", "", ""},
        Text{"Ocarina C App Button", "", ""},
        Text{"Ocarina Sup Button", "", ""}
    };
    trickNameTable[RG_OCARINA_C_DOWN_BUTTON] = {
        Text{"Ocarina C South Button", "", ""},
        Text{"Ocarina Z Down Button", "", ""},
        Text{"Ocarina See Down Button", "", ""},
        Text{"Ocarina C Dawn Button", "", ""}
    };
    trickNameTable[RG_OCARINA_C_LEFT_BUTTON] = {
        Text{"Ocarina C West Button", "", ""},
        Text{"Ocarina Sea Left Button", "", ""},
        Text{"Ocarina C Lift Button", "", ""},
        Text{"Ocarina Rewind Button", "", ""}
    };
    trickNameTable[RG_OCARINA_C_RIGHT_BUTTON] = {
        Text{"Ocarina C East Button", "", ""},
        Text{"Ocarina C Wright Button", "", ""},
        Text{"Overworld C Right Button", "", ""}
    };

    /*
    //Names for individual upgrades, in case progressive names are replaced
    trickNameTable[GI_HOOKSHOT] = {
        Text{"Grappling Hook", "Grappin-griffe", "Gancho lanzable"},
        Text{"Clawshot", "Lance-chaîne", "Zarpa"},
        Text{"Gripshot", "Grappince", "Enganchador"}
    };
    trickNameTable[GI_LONGSHOT] = {
        Text{"Longshot, no strings attached", "Grappin sans attrape", "Gancho lanzable más largo"},
        Text{"Double Clawshot", "Double-grappin", "Superzarpa"},
        Text{"Switch Hook", "Great grappin", "Gancho chulo"}
    };
    trickNameTable[GI_BOMB_BAG_1] = {
        Text{"Bomb Capacity (20)", "Capacité de bombes (20)", "Bolsa de bombas (contiene 20)"},
        Text{"Bronze Bomb Bag", "Sac de Bombes de bronze", "Saco de bronce de bombas"},
        Text{"Small Bomb Bag", "Petit Sac de Bombes", "Zurrón de bombas pequeño"}
    };
    trickNameTable[GI_BOMB_BAG_2] = {
        Text{"Bomb Capacity (30)", "Capacité de bombes (30)", "Bolsa de bombas (contiene 30)"},
        Text{"Silver Bomb Bag", "Sac de Bombes d'argent", "Saco plateado de bombas"},
        Text{"Medium Bomb Bag", "Sac de Bombes moyen", "Zurrón de bombas mediano"}
    };
    trickNameTable[GI_BOMB_BAG_3] = {
        Text{"Bomb Capacity (40)", "Capacité de bombes (40)", "Bolsa de bombas (contiene 40)"},
        Text{"Golden Bomb Bag", "Sac de Bombes d'or", "Saco dorado de bombas"},
        Text{"Large Bomb Bag", "Gros Sac de Bombes", "Zurrón de bombas grande"}
    };
    trickNameTable[GI_BOW_1] = {
        Text{"Bow", "Arc", "Arco del Hada"},
        Text{"Hero's Bow", "Arc du héros", "Arco del héroe"},
        Text{"Small Quiver", "Petit carquois", "Saco de flechas pequeño"}
    };
    trickNameTable[GI_BOW_2] = {
        Text{"Arrow Capacity (40)", "Capacité de flèches (40)", "Capacidad de flechas (40)"},
        Text{"Silver Quiver", "Carquois d'argent", "Carcaj plateado"},
        Text{"Medium Quiver", "Carquois moyen", "Saco de flechas mediano"}
    };
    trickNameTable[GI_BOW_3] = {
        Text{"Arrow Capacity (50)", "Capacité de flèches (50)", "Capacidad de flechas (50)"},
        Text{"Golden Quiver", "Carquois d'or", "Carcaj dorado"},
        Text{"Large Quiver", "Gros carquois", "Saco de flechas grande"}
    };
    trickNameTable[GI_SLINGSHOT_1] = {
        Text{"Slingshot", "Lance-Pierre", "Tirachinas del Hada"},
        Text{"Scattershot", "Lance-Pierre rafale", "Tirachinas múltiple"},
        Text{"Small Seed Satchel", "Petit sac de graines", "Bolsa de semillas pequeña"}
    };
    trickNameTable[GI_SLINGSHOT_2] = {
        Text{"Deku Seed Capacity (40)", "Capacité de graines (40)", "Capacidad de semillas (40)"},
        Text{"Silver Deku Seed Bullet Bag", "Sac de graines d'argent", "Bolsa de balas (contiene 40)"},
        Text{"Medium Seed Satchel", "Sac de graines moyen", "Bolsa de semillas mediana"}
    };
    trickNameTable[GI_SLINGSHOT_3] = {
        Text{"Deku Seed Capacity (50)", "Capacité de graines (50)", "Capacidad de semillas (50)"},
        Text{"Golden Deku Seed Bullet Bag", "Sac de graines d'or", "Bolsa de balas (contiene 50)"},
        Text{"Large Seed Satchel", "Gros sac de graines", "Bolsa de semillas grande"}
    };
    trickNameTable[GI_STRENGTH_1] = {
        Text{"Goron's Gauntlet", "Gantelet Goron", "Brazalete amarillo"},
        Text{"Power Bracelet", "Bracelet de force", "Brazalete de fuerza"},
        Text{"Magic Bracelet", "Bracelet de Lavio", "Brazalete de Ravio"}
    };
    trickNameTable[GI_STRENGTH_2] = {
        Text{"Silver Bracelets", "Bracelets d'argent", "Guantes Moguma"},
        Text{"Power Gloves", "Gant de puissance", "Guante del Poder"},
        Text{"Magic Gauntlets", "Gantelet magique", "Guante mágico"}
    };
    trickNameTable[GI_STRENGTH_3] = {
        Text{"Golden Bracelets", "Bracelets d'or", "Guantelete de Thanos"},
        Text{"Titan's Mitts", "Moufle de titan", "Guantes de Titán"},
        Text{"Magnetic Gloves", "Magnéto-gants", "Guantes de fuego"}
    };
    trickNameTable[GI_SCALE_1] = {
        Text{"Silver Pearl", "Perle d'argent", "Perla de Plata progresiva"},
        Text{"Adult Scale", "Écaille d'adulte", "Bola de bolos zora"},
        Text{"Zora Scale", "Écaille Zora", "Escama de Zora"}
    };
    trickNameTable[GI_SCALE_2] = {
        Text{"Golden Pearl", "Perle d'or", "Perla de Oro progresiva"},
        Text{"Giant Scale", "Écaille de géant", "Escama de Faren"},
        Text{"Water Dragon Scale", "Écaille du dragon de l'eau", "Escama de dragón acuático"}
    };
    trickNameTable[GI_WALLET_1] = {
        Text{"Rupee Capacity (200)", "Capacité de rubis (200)", "Capacidad de rupias (200)"},
        Text{"Silver Wallet", "Bourse d'argent", "Cartera de rupias de adulto"},
        Text{"Medium Wallet", "Bourse moyenne", "Zurrón de rupias mediano"}
    };
    trickNameTable[GI_WALLET_2] = {
        Text{"Rupee Capacity (500)", "Capacité de rubis (500)", "Capacidad de rupias (500)"},
        Text{"Golden Wallet", "Bourse d'or", "Cartera de rupias gigante"},
        Text{"Large Wallet", "Grosse Bourse", "Zurrón de rupias grande"}
    };
    trickNameTable[GI_WALLET_3] = {
        Text{"Rupee Capacity (999)", "Capacité de rubis (999)", "Capacidad de rupias (999)"},
        Text{"Golden Wallet", "Bourse d'or", "Cartera de ricachón"},
        Text{"Large Wallet", "Grosse Bourse", "Zurrón de rupias gigante"}
    };
    trickNameTable[GI_DEKU_NUT_UPGRADE_1] = {
        Text{"Deku Bomb Capacity (30)", "Capacité de bombes Mojo (30)", "Capacidad de semillas deku (40)"},
        Text{"Baba Nut Capacity (30)", "Capacité de noix Baba (30)", "Capacidad de nueces baba (40)"},
        Text{"Deku Nut Pack (30)", "Paquet de noix Mojo (30)", "Capacidad de nueces mojo (40)"}
    };
    trickNameTable[GI_DEKU_NUT_UPGRADE_2] = {
        Text{"Deku Bomb Capacity (40)", "Capacité de bombes Mojo (40)", "Capacidad de semillas deku (50)"},
        Text{"Baba Nut Capacity (40)", "Capacité de noix Baba (40)", "Capacidad de nueces baba (50)"},
        Text{"Deku Nut Pack (40)", "Paquet de noix Mojo (40)", "Capacidad de nueces mojo (50)"}
    };
    trickNameTable[GI_DEKU_STICK_UPGRADE_1] = {
        Text{"Deku Rod Capacity (20)", "Capacité de tiges Mojo (20)", "Capacidad de palos mojo (20)"},
        Text{"Boko Stick Capacity (20)", "Capacité de Bâtons Boko (20)", "Capacidad de palos boko (20)"},
        Text{"Deku Stick Pack (20)", "Paquet de bâtons Mojo (20)", "Capacidad de bastones deku (20)"}
    };
    trickNameTable[GI_DEKU_STICK_UPGRADE_2] = {
        Text{"Deku Rod Capacity (30)", "Capacité de tiges Mojo (30)", "Capacidad de palos mojo (30)"},
        Text{"Boko Stick Capacity (30)", "Capacité de Bâtons Boko (30)", "Capacidad de palos boko (30)"},
        Text{"Deku Stick Pack (30)", "Paquet de bâtons Mojo (30)", "Capacidad de bastones deku (30)"}
    };
    trickNameTable[GI_MAGIC_1] = {
        Text{"Stamina Meter", "Jauge d'endurance", "Medidor de vigor"},
        Text{"Energy Meter", "Jauge d'énergie", "Medidor de energía"},
        Text{"Magic Powder", "Poudre magique", "Medidor de carga"}
    };
    trickNameTable[GI_MAGIC_2] = {
        Text{"Enhanced Stamina Meter", "Jauge d'endurance améliorée", "Medidor de vigor mejorado"},
        Text{"Enhanced Energy Meter", "Jauge d'énergie améliorée", "Medidor de energía mejorado"},
        Text{"Enhanced Magic Powder", "Poudre magique améliorée", "Medidor de carga mejorado"}
    };
    trickNameTable[GI_OCARINA_1] = {
        Text{"Ocarina", "Ocarina", "Ocarina"},
        Text{"Saria's Ocarina", "Ocarina de Saria", "Ocarina de Saria"},
        Text{"Wood Ocarina", "Ocarina de bois", "Ocarina del Hada"}
    };
    trickNameTable[GI_OCARINA_2] = {
        Text{"Flute", "Flûte", "Flauta"},
        Text{"Zelda's Ocarina", "Ocarina de Zelda", "Ocarina de Zelda"},
        Text{"Ocarina of Winds", "Ocarina des vents", "Ocarina del Viento"}
    };
    trickNameTable[GI_CUCCO] = {
        Text{"D.I.Y. Alarm Clock", "Réveille-matin improvisé", "Alarma emplumada"},
        Text{"Kakariko Cucco", "Cocotte Cocorico", "Cuco de Kakariko"},
        Text{"Hatched Cucco", "Cocotte éclose", "Pollo"}
    };
    trickNameTable[GI_MASK_KEATON] = {
        Text{"Kee... Something Mask", "Masque de Quiche", "Máscara Kealgo"},
        Text{"Kitsune Mask", "Masque de Kitsune", "Máscara Kitsune"},
        Text{"Kafei's Mask", "Masque de Kafei", "Máscara de Kafei"}
    };
    trickNameTable[GI_MASK_SKULL] = {
        Text{"Skull Kid's Mask", "Masque de Skull Kid", "Máscara de Skull Kid"},
        Text{"Stalfos Mask", "Masque de squelette", "Máscara de Stalfos"},
        Text{"Captain's Hat", "Heaume du capitaine", "Casco del capitán"}
    };
    trickNameTable[GI_MASK_SPOOKY] = {
        Text{"Skrik Mask", "Masque Skrik", "Máscara Escalofriante"},
        Text{"ReDead Mask", "Masque de Remort", "Máscara de ReDead"},
        Text{"Gibdo Mask", "Masque de Gibdo", "Careta de Gibdo"}
    };
    trickNameTable[GI_MASK_BUNNY] = {
        Text{"Peppy Mask", "Masque de Peppy", "Capucha de Pascua"},
        Text{"Bunny Ears", "Oreilles de lapin", "Orejas de conejo"},
        Text{"Postman's Hat", "Casquette du facteur", "Gorra de cartero"}
    };
    trickNameTable[GI_MASK_GORON] = {
        Text{"Goro Mask", "Masque Goro", "Máscara Goro"},
        Text{"Mask of Goron", "Masque des Gorons", "Máscara de los Goron"},
        Text{"Darunia Mask", "Masque de Darunia", "Máscara de Darmani"}
    };
    trickNameTable[GI_MASK_ZORA] = {
        Text{"Zola Mask", "Masque Zola", "Máscara Zola"},
        Text{"Mask of Zora", "Masque des Zoras", "Máscara de los Zora"},
        Text{"Ruto Mask", "Masque de Ruto", "Máscara de Mikau"}
    };
    trickNameTable[GI_MASK_GERUDO] = {
        Text{"Ganguro Mask", "Masque de Ganguro", "Máscara Canguro"},
        Text{"Mask of Gerudo", "Masque des Gerudos", "Máscara de las Gerudo"},
        Text{"Nabooru Mask", "Masque de Nabooru", "Máscara de Nabooru"}
    };
    trickNameTable[GI_MASK_TRUTH] = {
        Text{"Sheikah Mask", "Masque Sheikah", "Máscara Sheikah"},
        Text{"Mask of Gossip", "Masque de potins", "Máscara chismosa"},
        Text{"Eye of Truth", "oeil de vérité", "Ojo de la Verdad"}
    };
    */
}

//Generate a fake name for the ice trap based on the item it's displayed as
Text GetIceTrapName(uint8_t id) {
    //If the trick names table has not been initialized, do so
    if (!initTrickNames) {
        InitTrickNames();
        initTrickNames = true;
    }
    //Randomly get the easy, medium, or hard name for the given item id
    return RandomElement(trickNameTable[id]);
}