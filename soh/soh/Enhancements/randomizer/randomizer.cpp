#include "randomizer.h"
#include <nlohmann/json.hpp>
#include "3drando/settings.hpp"
#include <fstream>
#include <variables.h>
#include <macros.h>
#include <objects/gameplay_keep/gameplay_keep.h>
#include <functions.h>
#include <libultraship/Cvar.h>
#include <textures/icon_item_static/icon_item_static.h>
#include <textures/icon_item_24_static/icon_item_24_static.h>
#include <libultraship/ImGuiImpl.h>
#include <thread>
#include "3drando/rando_main.hpp"
#include "../../UIWidgets.hpp"
#include <ImGui/imgui_internal.h>
#include "../custom-message/CustomMessageTypes.h"
#include "../item-tables/ItemTableManager.h"
#include "../../../src/overlays/actors/ovl_En_GirlA/z_en_girla.h"
#include <stdexcept>
#include "randomizer_check_objects.h"
#include <sstream>
#include "draw.h"
#include "rando_hash.h"

using json = nlohmann::json;
using namespace std::literals::string_literals;

std::unordered_map<std::string, RandomizerCheck> SpoilerfileCheckNameToEnum;
std::unordered_map<std::string, RandomizerGet> SpoilerfileGetNameToEnum;
std::unordered_map<RandomizerGet, std::vector<std::string>> EnumToSpoilerfileGetName;
std::set<RandomizerCheck> excludedLocations;

u8 generated;

const std::string Randomizer::getItemMessageTableID = "Randomizer";
const std::string Randomizer::hintMessageTableID = "RandomizerHints";
const std::string Randomizer::merchantMessageTableID = "RandomizerMerchants";
const std::string Randomizer::rupeeMessageTableID = "RandomizerRupees";
const std::string Randomizer::NaviRandoMessageTableID = "RandomizerNavi";

static const char* englishRupeeNames[80] = {
    "Rupees",       "Bitcoin",       "Bananas",      "Cornflakes", "Gummybears",   "Floopies",    "Dollars",
    "Lemmings",     "Emeralds",      "Bucks",        "Rubles",     "Diamonds",     "Moons",       "Stars",
    "Mana",         "Doll Hairs",    "Dogecoin",     "Mushrooms",  "Experience",   "Friends",     "Coins",
    "Rings",        "Gil",           "Pokédollars",  "Bells",      "Orbs",         "Bottle Caps", "Simoleons",
    "Pokémon",      "Toys",          "Smackaroos",   "Zorkmids",   "Zenny",        "Bones",       "Souls",
    "Studs",        "Munny",         "Rubies",       "Gald",       "Gold",         "Shillings",   "Pounds",
    "Glimmer",      "Potch",         "Robux",        "V-Bucks",    "Bratwürste",   "Mesetas",     "Coal",
    "Euro",         "Spoons",        "Cucumbers",    "FPS",        "Shekels",      "Yen",         "Canadian Dollars",
    "Dollarydoos",  "Copper",        "Silver",       "Platinum",   "Gems",         "Minerals",    "Vespene Gas",
    "Lumber",       "Jiggies",       "Mumbo Tokens", "KF7 Ammo",   "Remote Mines", "Credits",     "Doubloons",
    "Ether",        "Doge",          "Cards",        "Talent",     "Poko",         "Lira",        "Kroner",
    "Store Credit", "Social Credit", "Cocoa Beans"
};

static const char* germanRupeeNames[41] = {
    "Rubine",     "Mäuse",       "Kröten",        "Münzen",     "Euro",       "Mark",     "Bananen",
    "Gummibären", "Bonbons",     "Diamanten",     "Bratwürste", "Bitcoin",    "Dogecoin", "Monde",
    "Sterne",     "Brause UFOs", "Taler",         "Sternis",    "Schillinge", "Freunde",  "Seelen",
    "Gil",        "Zenny",       "Pfandflaschen", "Knochen",    "Pilze",      "Smaragde", "Kronkorken",
    "Pokédollar", "Brötchen",    "EXP",           "Wagenchips", "Moos",       "Knete",    "Kohle",
    "Kies",       "Radieschen",  "Diridari",      "Steine",     "Kartoffeln", "Penunze"
};

static const char* frenchRupeeNames[36] = {
    "Rubis",       "Bitcoin", "Bananes",   "Euros",     "Dollars", "Émeraudes",  "Joyaux",   "Diamants",
    "Balles",      "Pokémon", "Pièces",    "Lunes",     "Étoiles", "Dogecoin",   "Anneaux",  "Radis",
    "Pokédollars", "Zennies", "Pépètes",   "Mailles",   "Éthers",  "Clochettes", "Capsules", "Gils",
    "Champignons", "Blés",    "Halos",     "Munnies",   "Orens",   "Florens",    "Crédits",  "Galds",
    "Bling",       "Orbes",   "Baguettes", "Croissants"
};

Randomizer::Randomizer() {
    for (auto areaIt : RandomizerCheckObjects::GetAllRCObjects()) {
        for (auto locationIt : areaIt.second) {
            SpoilerfileCheckNameToEnum[locationIt.rcSpoilerName] = locationIt.rc;
        }
        SpoilerfileCheckNameToEnum["Invalid Location"] = RC_UNKNOWN_CHECK;
        SpoilerfileCheckNameToEnum["Link's Pocket"] = RC_LINKS_POCKET;
    }

    for (auto item: *RandoMain::GetFullItemTable()) {
        // Easiest way to filter out all the empty values from the array, since we still technically want the 0/RG_NONE entry
        if (item.GetName().english.empty()) continue;
        SpoilerfileGetNameToEnum[item.GetName().english] = item.GetRandomizerGet();
        SpoilerfileGetNameToEnum[item.GetName().french] = item.GetRandomizerGet();
        EnumToSpoilerfileGetName[item.GetRandomizerGet()] = {
            item.GetName().english,
            item.GetName().english,
            item.GetName().french,
        };
    }
}

Sprite* Randomizer::GetSeedTexture(uint8_t index) {
    return &gSeedTextures[index];
}

Randomizer::~Randomizer() { 
    this->randoSettings.clear();
    this->itemLocations.clear();
    this->merchantPrices.clear();
}

std::unordered_map<std::string, RandomizerInf> spoilerFileTrialToEnum = {
    { "the Forest Trial", RAND_INF_TRIALS_DONE_FOREST_TRIAL },
    { "l'épreuve de la Forêt", RAND_INF_TRIALS_DONE_FOREST_TRIAL },
    { "the Fire Trial", RAND_INF_TRIALS_DONE_FIRE_TRIAL },
    { "l'épreuve du Feu", RAND_INF_TRIALS_DONE_FIRE_TRIAL },
    { "the Water Trial", RAND_INF_TRIALS_DONE_WATER_TRIAL },
    { "l'épreuve de l'Eau", RAND_INF_TRIALS_DONE_WATER_TRIAL },
    { "the Spirit Trial", RAND_INF_TRIALS_DONE_SPIRIT_TRIAL },
    { "l'épreuve de l'Esprit", RAND_INF_TRIALS_DONE_SPIRIT_TRIAL },
    { "the Shadow Trial", RAND_INF_TRIALS_DONE_SHADOW_TRIAL },
    { "l'épreuve de l'Ombre", RAND_INF_TRIALS_DONE_SHADOW_TRIAL },
    { "the Light Trial", RAND_INF_TRIALS_DONE_LIGHT_TRIAL },
    { "l'épreuve de la Lumière", RAND_INF_TRIALS_DONE_LIGHT_TRIAL }
};

std::unordered_map<s16, s16> getItemIdToItemId = {
    { GI_BOW, ITEM_BOW },
    { GI_ARROW_FIRE, ITEM_ARROW_FIRE },
    { GI_DINS_FIRE, ITEM_DINS_FIRE },
    { GI_SLINGSHOT, ITEM_SLINGSHOT },
    { GI_OCARINA_FAIRY, ITEM_OCARINA_FAIRY },
    { GI_OCARINA_OOT, ITEM_OCARINA_TIME },
    { GI_HOOKSHOT, ITEM_HOOKSHOT },
    { GI_LONGSHOT, ITEM_LONGSHOT },
    { GI_ARROW_ICE, ITEM_ARROW_ICE },
    { GI_FARORES_WIND, ITEM_FARORES_WIND },
    { GI_BOOMERANG, ITEM_BOOMERANG },
    { GI_LENS, ITEM_LENS },
    { GI_HAMMER, ITEM_HAMMER },
    { GI_ARROW_LIGHT, ITEM_ARROW_LIGHT },
    { GI_NAYRUS_LOVE, ITEM_NAYRUS_LOVE },
    { GI_BOTTLE, ITEM_BOTTLE },
    { GI_POTION_RED, ITEM_POTION_RED },
    { GI_POTION_GREEN, ITEM_POTION_GREEN },
    { GI_POTION_BLUE, ITEM_POTION_BLUE },
    { GI_FAIRY, ITEM_FAIRY },
    { GI_FISH, ITEM_FISH },
    { GI_MILK_BOTTLE, ITEM_MILK_BOTTLE },
    { GI_LETTER_RUTO, ITEM_LETTER_RUTO },
    { GI_BLUE_FIRE, ITEM_BLUE_FIRE },
    { GI_BUGS, ITEM_BUG },
    { GI_BIG_POE, ITEM_BIG_POE },
    { GI_POE, ITEM_POE },
    { GI_WEIRD_EGG, ITEM_WEIRD_EGG },
    { GI_LETTER_ZELDA, ITEM_LETTER_ZELDA },
    { GI_POCKET_EGG, ITEM_POCKET_EGG },
    { GI_COJIRO, ITEM_COJIRO },
    { GI_ODD_MUSHROOM, ITEM_ODD_MUSHROOM },
    { GI_ODD_POTION, ITEM_ODD_POTION },
    { GI_SAW, ITEM_SAW },
    { GI_SWORD_BROKEN, ITEM_SWORD_BROKEN },
    { GI_PRESCRIPTION, ITEM_PRESCRIPTION },
    { GI_FROG, ITEM_FROG },
    { GI_EYEDROPS, ITEM_EYEDROPS },
    { GI_CLAIM_CHECK, ITEM_CLAIM_CHECK }
};

std::unordered_map<std::string, RandomizerSettingKey> SpoilerfileSettingNameToEnum = {
    { "Open Settings:Forest", RSK_FOREST },
    { "Open Settings:Kakariko Gate", RSK_KAK_GATE },
    { "Open Settings:Door of Time", RSK_DOOR_OF_TIME },
    { "Open Settings:Zora's Fountain", RSK_ZORAS_FOUNTAIN },
    { "Open Settings:Gerudo Fortress", RSK_GERUDO_FORTRESS },
    { "Open Settings:Rainbow Bridge", RSK_RAINBOW_BRIDGE },
    { "Open Settings:Stone Count", RSK_RAINBOW_BRIDGE_STONE_COUNT },
    { "Open Settings:Medallion Count", RSK_RAINBOW_BRIDGE_MEDALLION_COUNT },
    { "Open Settings:Reward Count", RSK_RAINBOW_BRIDGE_REWARD_COUNT },
    { "Open Settings:Dungeon Count", RSK_RAINBOW_BRIDGE_DUNGEON_COUNT },
    { "Open Settings:Token Count", RSK_RAINBOW_BRIDGE_TOKEN_COUNT },
    { "Open Settings:Random Ganon's Trials", RSK_RANDOM_TRIALS },
    { "Open Settings:Trial Count", RSK_TRIAL_COUNT },
    { "Shuffle Settings:Link's Pocket", RSK_LINKS_POCKET},
    { "Shuffle Settings:Shuffle Gerudo Card", RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD },
    { "Shuffle Settings:Shopsanity", RSK_SHOPSANITY },
    { "Shuffle Settings:Scrub Shuffle", RSK_SHUFFLE_SCRUBS },
    { "Shuffle Settings:Shuffle Cows", RSK_SHUFFLE_COWS },
    { "Shuffle Settings:Tokensanity", RSK_SHUFFLE_TOKENS },
    { "Shuffle Settings:Shuffle Adult Trade", RSK_SHUFFLE_ADULT_TRADE },
    { "Shuffle Settings:Shuffle Magic Beans", RSK_SHUFFLE_MAGIC_BEANS },
    { "Start with Deku Shield", RSK_STARTING_DEKU_SHIELD },
    { "Start with Kokiri Sword", RSK_STARTING_KOKIRI_SWORD },
    { "Start with Fairy Ocarina", RSK_STARTING_OCARINA },
    { "Shuffle Dungeon Items:Maps/Compasses", RSK_STARTING_MAPS_COMPASSES },
    { "Shuffle Dungeon Items:Small Keys", RSK_KEYSANITY },
    { "Shuffle Dungeon Items:Gerudo Fortress Keys", RSK_GERUDO_KEYS },
    { "Shuffle Dungeon Items:Boss Keys", RSK_BOSS_KEYSANITY },
    { "Shuffle Dungeon Items:Ganon's Boss Key", RSK_GANONS_BOSS_KEY },
    { "World Settings:Ammo Drops", RSK_ENABLE_BOMBCHU_DROPS },
    { "World Settings:Bombchus in Logic", RSK_BOMBCHUS_IN_LOGIC },
    { "Misc Settings:Gossip Stone Hints", RSK_GOSSIP_STONE_HINTS },
    { "Misc Settings:Hint Clarity", RSK_HINT_CLARITY },
    { "Misc Settings:Hint Distribution", RSK_HINT_DISTRIBUTION },
    { "Misc Settings:Blue Fire Arrows", RSK_BLUE_FIRE_ARROWS },
    { "Misc Settings:Sunlight Arrows", RSK_SUNLIGHT_ARROWS },
    { "Skip Child Zelda", RSK_SKIP_CHILD_ZELDA },
    { "Start with Consumables", RSK_STARTING_CONSUMABLES },
    { "Start with Max Rupees", RSK_FULL_WALLETS },
    { "Timesaver Settings:Cuccos to return", RSK_CUCCO_COUNT },
    { "Timesaver Settings:Big Poe Target Count", RSK_BIG_POE_COUNT },
    { "Timesaver Settings:Skip Child Stealth", RSK_SKIP_CHILD_STEALTH },
    { "Timesaver Settings:Skip Epona Race", RSK_SKIP_EPONA_RACE },
    { "Timesaver Settings:Skip Tower Escape", RSK_SKIP_TOWER_ESCAPE },
    { "Timesaver Settings:Complete Mask Quest", RSK_COMPLETE_MASK_QUEST },
    { "Timesaver Settings:Skip Scarecrow's Song", RSK_SKIP_SCARECROWS_SONG },
    { "Timesaver Settings:Enable Glitch-Useful Cutscenes", RSK_ENABLE_GLITCH_CUTSCENES },
};

std::string sanitize(std::string stringValue) {
    // Add backslashes.
    for (auto i = stringValue.begin();;) {
        auto const pos =
            std::find_if(i, stringValue.end(), [](char const c) { return '\\' == c || '\'' == c || '"' == c; });
        if (pos == stringValue.end()) {
            break;
        }
        i = std::next(stringValue.insert(pos, '\\'), 2);
    }

    // Removes others.
    stringValue.erase(std::remove_if(stringValue.begin(), stringValue.end(),
                                     [](char const c) { return '\n' == c || '\r' == c || '\0' == c || '\x1A' == c; }),
                      stringValue.end());

    return stringValue;
}

#pragma optimize("", off)
#pragma GCC push_options
#pragma GCC optimize ("O0")
bool Randomizer::SpoilerFileExists(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        std::ifstream spoilerFileStream(sanitize(spoilerFileName));
        if (!spoilerFileStream) {
            return false;
        } else {
            return true;
        }
    }

    return false;
}
#pragma GCC pop_options
#pragma optimize("", on)

void Randomizer::LoadRandomizerSettings(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        ParseRandomizerSettingsFile(spoilerFileName);
    }

    for(auto randoSetting : gSaveContext.randoSettings) {
        if(randoSetting.key == RSK_NONE) break;
        this->randoSettings[randoSetting.key] = randoSetting.value;
    }
}

void Randomizer::LoadHintLocations(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        ParseHintLocationsFile(spoilerFileName);
    }

    CustomMessageManager::Instance->ClearMessageTable(Randomizer::hintMessageTableID);
    CustomMessageManager::Instance->AddCustomMessageTable(Randomizer::hintMessageTableID);

    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_ALTAR_CHILD,
                                           { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, gSaveContext.childAltarText,
                                             gSaveContext.childAltarText, gSaveContext.childAltarText });
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_ALTAR_ADULT,
                                           { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, gSaveContext.adultAltarText,
                                             gSaveContext.adultAltarText, gSaveContext.adultAltarText });
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_GANONDORF,
                                           { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM, gSaveContext.ganonHintText,
                                             gSaveContext.ganonHintText, gSaveContext.ganonHintText });
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_GANONDORF_NOHINT,
                                           { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM, gSaveContext.ganonText,
                                             gSaveContext.ganonText, gSaveContext.ganonText });

    this->childAltarText = gSaveContext.childAltarText;
    this->adultAltarText = gSaveContext.adultAltarText;
    this->ganonHintText = gSaveContext.ganonHintText;
    this->ganonText = gSaveContext.ganonText;

    for (auto hintLocation : gSaveContext.hintLocations) {
        if(hintLocation.check == RC_LINKS_POCKET) break;
        this->hintLocations[hintLocation.check] = hintLocation.hintText;
        CustomMessageManager::Instance->CreateMessage(
            Randomizer::hintMessageTableID, hintLocation.check, { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, hintLocation.hintText, hintLocation.hintText, hintLocation.hintText });
    }
}

std::vector<RandomizerCheck> shopItemRandomizerChecks = {
    RC_KF_SHOP_ITEM_1,
    RC_KF_SHOP_ITEM_2,
    RC_KF_SHOP_ITEM_3,
    RC_KF_SHOP_ITEM_4,
    RC_KF_SHOP_ITEM_5,
    RC_KF_SHOP_ITEM_6,
    RC_KF_SHOP_ITEM_7,
    RC_KF_SHOP_ITEM_8,
    RC_GC_SHOP_ITEM_1,
    RC_GC_SHOP_ITEM_2,
    RC_GC_SHOP_ITEM_3,
    RC_GC_SHOP_ITEM_4,
    RC_GC_SHOP_ITEM_5,
    RC_GC_SHOP_ITEM_6,
    RC_GC_SHOP_ITEM_7,
    RC_GC_SHOP_ITEM_8,
    RC_ZD_SHOP_ITEM_1,
    RC_ZD_SHOP_ITEM_2,
    RC_ZD_SHOP_ITEM_3,
    RC_ZD_SHOP_ITEM_4,
    RC_ZD_SHOP_ITEM_5,
    RC_ZD_SHOP_ITEM_6,
    RC_ZD_SHOP_ITEM_7,
    RC_ZD_SHOP_ITEM_8,
    RC_KAK_BAZAAR_ITEM_1,
    RC_KAK_BAZAAR_ITEM_2,
    RC_KAK_BAZAAR_ITEM_3,
    RC_KAK_BAZAAR_ITEM_4,
    RC_KAK_BAZAAR_ITEM_5,
    RC_KAK_BAZAAR_ITEM_6,
    RC_KAK_BAZAAR_ITEM_7,
    RC_KAK_BAZAAR_ITEM_8,
    RC_KAK_POTION_SHOP_ITEM_1,
    RC_KAK_POTION_SHOP_ITEM_2,
    RC_KAK_POTION_SHOP_ITEM_3,
    RC_KAK_POTION_SHOP_ITEM_4,
    RC_KAK_POTION_SHOP_ITEM_5,
    RC_KAK_POTION_SHOP_ITEM_6,
    RC_KAK_POTION_SHOP_ITEM_7,
    RC_KAK_POTION_SHOP_ITEM_8,
    RC_MARKET_BAZAAR_ITEM_1,
    RC_MARKET_BAZAAR_ITEM_2,
    RC_MARKET_BAZAAR_ITEM_3,
    RC_MARKET_BAZAAR_ITEM_4,
    RC_MARKET_BAZAAR_ITEM_5,
    RC_MARKET_BAZAAR_ITEM_6,
    RC_MARKET_BAZAAR_ITEM_7,
    RC_MARKET_BAZAAR_ITEM_8,
    RC_MARKET_POTION_SHOP_ITEM_1,
    RC_MARKET_POTION_SHOP_ITEM_2,
    RC_MARKET_POTION_SHOP_ITEM_3,
    RC_MARKET_POTION_SHOP_ITEM_4,
    RC_MARKET_POTION_SHOP_ITEM_5,
    RC_MARKET_POTION_SHOP_ITEM_6,
    RC_MARKET_POTION_SHOP_ITEM_7,
    RC_MARKET_POTION_SHOP_ITEM_8,
    RC_MARKET_BOMBCHU_SHOP_ITEM_1,
    RC_MARKET_BOMBCHU_SHOP_ITEM_2,
    RC_MARKET_BOMBCHU_SHOP_ITEM_3,
    RC_MARKET_BOMBCHU_SHOP_ITEM_4,
    RC_MARKET_BOMBCHU_SHOP_ITEM_5,
    RC_MARKET_BOMBCHU_SHOP_ITEM_6,
    RC_MARKET_BOMBCHU_SHOP_ITEM_7,
    RC_MARKET_BOMBCHU_SHOP_ITEM_8,
};

// Reference soh/src/overlays/actors/ovl_En_GirlA/z_en_girla.h
std::unordered_map<RandomizerGet, EnGirlAShopItem> randomizerGetToEnGirlShopItem = {
    { RG_BUY_DEKU_NUT_5,    SI_DEKU_NUTS_5 },
    { RG_BUY_ARROWS_30,     SI_ARROWS_30 },
    { RG_BUY_ARROWS_50,     SI_ARROWS_50 },
    { RG_BUY_BOMBS_525,     SI_BOMBS_5_R25 },
    { RG_BUY_DEKU_NUT_10,   SI_DEKU_NUTS_10 },
    { RG_BUY_DEKU_STICK_1,  SI_DEKU_STICK },
    { RG_BUY_BOMBS_10,      SI_BOMBS_10 },
    { RG_BUY_FISH,          SI_FISH },
    { RG_BUY_RED_POTION_30, SI_RED_POTION_R30 },
    { RG_BUY_GREEN_POTION,  SI_GREEN_POTION },
    { RG_BUY_BLUE_POTION,   SI_BLUE_POTION },
    { RG_BUY_HYLIAN_SHIELD, SI_HYLIAN_SHIELD },
    { RG_BUY_DEKU_SHIELD,   SI_DEKU_SHIELD },
    { RG_BUY_GORON_TUNIC,   SI_GORON_TUNIC },
    { RG_BUY_ZORA_TUNIC,    SI_ZORA_TUNIC },
    { RG_BUY_HEART,         SI_HEART },
    { RG_BUY_BOMBCHU_10,    SI_BOMBCHU_10_1 },
    { RG_BUY_BOMBCHU_20,    SI_BOMBCHU_20_1 },
    { RG_BUY_DEKU_SEEDS_30, SI_DEKU_SEEDS_30 },
    { RG_BUY_BLUE_FIRE,     SI_BLUE_FIRE },
    { RG_BUY_BOTTLE_BUG,    SI_BUGS },
    { RG_BUY_POE,           SI_POE },
    { RG_BUY_FAIRYS_SPIRIT, SI_FAIRY },
    { RG_BUY_ARROWS_10,     SI_ARROWS_10 },
    { RG_BUY_BOMBS_20,      SI_BOMBS_20 },
    { RG_BUY_BOMBS_30,      SI_BOMBS_30 },
    { RG_BUY_BOMBS_535,     SI_BOMBS_5_R35 },
    { RG_BUY_RED_POTION_40, SI_RED_POTION_R40 },
    { RG_BUY_RED_POTION_50, SI_RED_POTION_R50 },
};

void Randomizer::LoadMerchantMessages(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        ParseHintLocationsFile(spoilerFileName);
    }

    CustomMessageManager::Instance->ClearMessageTable(Randomizer::merchantMessageTableID);
    CustomMessageManager::Instance->AddCustomMessageTable(Randomizer::merchantMessageTableID);

    // Prices have a chance of being 0, and the "sell" message below doesn't really make sense for a free item, so manually adding a "free" variation here
    CustomMessageManager::Instance->CreateMessage(Randomizer::merchantMessageTableID, TEXT_SCRUB_RANDOM,
        { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM,
            "\x12\x38\x82\All right! You win! In return for&sparing me, I will give you a&%gmysterious item%w!&Please, take it!\x07\x10\xA3",
            "\x12\x38\x82\In Ordnung! Du gewinnst! Im Austausch&dafür, dass du mich verschont hast,&werde ich dir einen %gmysteriösen&Gegenstand%w geben! Bitte nimm ihn!\x07\x10\xA3",
            "\x12\x38\x82\J'me rends! Laisse-moi partir et en&échange, je te donne un %gobjet &mystérieux%w! Vas-y prends le!\x07\x10\xA3",
        });
    // Currently a scrub message is created for each price between 5-95, identified by the price itself. Soon we'll migrate this
    // to be more consistent with shop items, where they are identified by randomizer_inf and only generated where necessary
    for (u32 price = 5; price <= 95; price += 5) {
        CustomMessageManager::Instance->CreateMessage(Randomizer::merchantMessageTableID, TEXT_SCRUB_RANDOM + price,
            { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM,
              "\x12\x38\x82\All right! You win! In return for&sparing me, I will sell you a&%gmysterious item%w!&%r" +
                  std::to_string(price) + " Rupees%w it is!\x07\x10\xA3",
              "\x12\x38\x82\Aufgeben! Ich verkaufe dir einen&%ggeheimnisvollen Gegenstand%w&für %r" +
                  std::to_string(price) + " Rubine%w!\x07\x10\xA3",
              "\x12\x38\x82J'abandonne! Tu veux bien m'acheter&un %gobjet mystérieux%w?&Ça fera %r" +
                  std::to_string(price) + " Rubis%w!\x07\x10\xA3"
            });
    }
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::merchantMessageTableID, TEXT_BEAN_SALESMAN,
        {
            TEXTBOX_TYPE_BLACK,
            TEXTBOX_POS_BOTTOM,
            "I tried to be a %rmagic bean%w salesman,&but it turns out my marketing skills&weren't worth "
            "beans!^Anyway, want to buy my&%gmysterious item%w for 60 Rupees?\x1B&%gYes&No%w",
            "Möchten Sie einen geheimnisvollen&Gegenstand für 60 Rubine?\x1B&%gJa&Nein%w",
            "J'ai essayé d'être un vendeur de&%rharicots magiques%w, mais j'étais&mauvais au niveau du marketing et&ça "
            "me courait sur le haricot...^Enfin bref, ça te dirait de m'acheter un&"
            "%gobjet mystérieux%w pour 60 Rubis?\x1B&%gOui&Non%w",
        });

    for (int index = 0; index < NUM_SHOP_ITEMS; index++) {
        RandomizerCheck shopItemCheck = shopItemRandomizerChecks[index];
        RandomizerGet shopItemGet = this->itemLocations[shopItemCheck];
        // TODO: This should eventually be replaced with a full fledged trick model & trick name system
        if (shopItemGet == RG_ICE_TRAP) {
            shopItemGet = RG_HUGE_RUPEE;
        }
        std::vector<std::string> shopItemName = EnumToSpoilerfileGetName[shopItemGet];
        u16 shopItemPrice = merchantPrices[shopItemCheck];
        // Each shop item has two messages, one for when the cursor is over it, and one for when you select it and are
        // prompted buy/don't buy, so we're adding the first at {index}, and the second at {index + NUM_SHOP_ITEMS}
        CustomMessageManager::Instance->CreateMessage(
            Randomizer::merchantMessageTableID, TEXT_SHOP_ITEM_RANDOM + index, { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_VARIABLE,
                "\x08%r" + shopItemName[0] + ": " + std::to_string(shopItemPrice) + " Rupees&%wSpecial deal! ONE LEFT!&Get it while it lasts!\x0A\x02",
                "\x08%r" + shopItemName[1] + ": " + std::to_string(shopItemPrice) + " Rubine&%wSonderangebot! NUR NOCH EINES VERFÜGBAR!&Beeilen Sie sich!\x0A\x02",
                "\x08%r" + shopItemName[2] + ": " + std::to_string(shopItemPrice) + " Rubis&%wOffre spéciale! DERNIER EN STOCK!&Faites vite!\x0A\x02",
        });
        CustomMessageManager::Instance->CreateMessage(
            Randomizer::merchantMessageTableID, TEXT_SHOP_ITEM_RANDOM + index + NUM_SHOP_ITEMS, { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_VARIABLE,
                "\x08" + shopItemName[0] + ": " + std::to_string(shopItemPrice) + " Rupees\x09&&\x1B%gBuy&Don't buy%w\x09\x02",
                "\x08" + shopItemName[1] + ": " + std::to_string(shopItemPrice) + " Rubine\x09&&\x1B%gKaufen&Nicht kaufen%w\x09\x02",
                "\x08" + shopItemName[2] + ": " + std::to_string(shopItemPrice) + " Rubis\x09&&\x1B%gAcheter&Ne pas acheter%w\x09\x02",
        });
    }
}

void Randomizer::LoadItemLocations(const char* spoilerFileName, bool silent) {
    if (strcmp(spoilerFileName, "") != 0) {
        ParseItemLocationsFile(spoilerFileName, silent);
    }

    for (auto itemLocation : gSaveContext.itemLocations) {
        this->itemLocations[itemLocation.check] = itemLocation.get;
    }

    itemLocations[RC_UNKNOWN_CHECK] = RG_NONE;
}

void Randomizer::LoadRequiredTrials(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        ParseRequiredTrialsFile(spoilerFileName);
    }
}

void Randomizer::ParseRandomizerSettingsFile(const char* spoilerFileName) {
    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream)
        return;

    bool success = false;

    try {
        // clear out existing settings
        // RANDOTODO don't use magic number for settings array size
        for(size_t i = 0; i < 300; i++) {
            gSaveContext.randoSettings[i].key = RSK_NONE;
            gSaveContext.randoSettings[i].value = 0;
        }

        json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;
        json settingsJson = spoilerFileJson["settings"];

        int index = 0;

        for (auto it = settingsJson.begin(); it != settingsJson.end(); ++it) {
            // todo load into cvars for UI
            
            std::string numericValueString;
            if(SpoilerfileSettingNameToEnum.count(it.key())) {
                gSaveContext.randoSettings[index].key = SpoilerfileSettingNameToEnum[it.key()];
                // this is annoying but the same strings are used in different orders
                // and i don't want the spoilerfile to just have numbers instead of
                // human readable settings values so it'll have to do for now
                switch(gSaveContext.randoSettings[index].key) {
                    case RSK_FOREST:
                        if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Closed Deku") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_KAK_GATE:
                        if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = 1;
                        }
                        break;
                    case RSK_DOOR_OF_TIME:
                        if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Song only") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_ZORAS_FOUNTAIN:
                        if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Closed as child") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_GERUDO_FORTRESS:
                        if(it.value() == "Normal") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Fast") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_RAINBOW_BRIDGE:
                        if(it.value() == "Always open") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Stones") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Medallions") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "Dungeon rewards") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "Dungeons") {
                            gSaveContext.randoSettings[index].value = 5;
                        } else if(it.value() == "Tokens") {
                            gSaveContext.randoSettings[index].value = 6;
                        }
                        break;
                    case RSK_RAINBOW_BRIDGE_STONE_COUNT:
                    case RSK_RAINBOW_BRIDGE_MEDALLION_COUNT:
                    case RSK_RAINBOW_BRIDGE_REWARD_COUNT:
                    case RSK_RAINBOW_BRIDGE_DUNGEON_COUNT:
                    case RSK_RAINBOW_BRIDGE_TOKEN_COUNT:
                    case RSK_TRIAL_COUNT:
                    case RSK_BIG_POE_COUNT:
                    case RSK_CUCCO_COUNT:
                        numericValueString = it.value();
                        gSaveContext.randoSettings[index].value = std::stoi(numericValueString);
                        break;
                    case RSK_SHOPSANITY:
                        if(it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "0 Items") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "1 Item") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "2 Items") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "3 Items") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "4 Items") {
                            gSaveContext.randoSettings[index].value = 5;
                        } else if(it.value() == "Random") {
                            gSaveContext.randoSettings[index].value = 6;
                        }
                        break;
                    case RSK_SHUFFLE_SCRUBS:
                        if(it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Affordable") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Expensive") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Random Prices") {
                            gSaveContext.randoSettings[index].value = 3;
                        }
                        break;
                    case RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD:
                    case RSK_SHUFFLE_COWS:
                    case RSK_SHUFFLE_ADULT_TRADE:
                    case RSK_SHUFFLE_MAGIC_BEANS:
                    case RSK_RANDOM_TRIALS:
                    case RSK_STARTING_DEKU_SHIELD:
                    case RSK_STARTING_KOKIRI_SWORD:
                    case RSK_COMPLETE_MASK_QUEST:
                    case RSK_SKIP_SCARECROWS_SONG:
                    case RSK_ENABLE_GLITCH_CUTSCENES:
                    case RSK_BLUE_FIRE_ARROWS:
                    case RSK_SUNLIGHT_ARROWS:
                    case RSK_BOMBCHUS_IN_LOGIC:
                        if(it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "On") {
                            gSaveContext.randoSettings[index].value = 1;
                        }
                        break;
                    // Uses Ammo Drops option for now. "Off" not yet implemented
                    case RSK_ENABLE_BOMBCHU_DROPS:
                        if (it.value() == "On") {
                            gSaveContext.randoSettings[index].value = 0;
                        } else if (it.value() == "On + Bombchu") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if (it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_STARTING_MAPS_COMPASSES:
                        if(it.value() == "Start With") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Own Dungeon") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = 5;
                        }
                        break;
                    case RSK_STARTING_OCARINA:
                        if(it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Fairy Ocarina") {
                            gSaveContext.randoSettings[index].value = 1;
                        }
                        break;
                    case RSK_GOSSIP_STONE_HINTS:
                        if(it.value() == "No Hints") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Need Nothing") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Mask of Truth") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Stone of Agony") {
                            gSaveContext.randoSettings[index].value = 3;
                        }
                        break;
                    case RSK_HINT_CLARITY:
                        if(it.value() == "Obscure") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Ambiguous") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Clear") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_HINT_DISTRIBUTION:
                        if(it.value() == "Useless") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Balanced") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Strong") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Very Strong") {
                            gSaveContext.randoSettings[index].value = 3;
                        }
                        break;
                    case RSK_GERUDO_KEYS:
                        if (it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 0;
                        } else if (it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if (it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if (it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = 3;
                        }
                        break;
                    case RSK_KEYSANITY:
                        if(it.value() == "Start With") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Own Dungeon") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = 5;
                        }
                        break;
                    case RSK_BOSS_KEYSANITY:
                        if(it.value() == "Start With") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Own Dungeon") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = 5;
                        }
                        break;
                    case RSK_GANONS_BOSS_KEY:
                        if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 0;
                        } else if(it.value() == "Own dungeon") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Start with") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = 5;
                        }
                        break;
                    case RSK_SKIP_CHILD_ZELDA:
                        gSaveContext.randoSettings[index].value = it.value();
                        break;
                    case RSK_STARTING_CONSUMABLES:
                    case RSK_FULL_WALLETS:
                        if(it.value() == "No") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Yes") {
                            gSaveContext.randoSettings[index].value = 1;
                        }
                        break;
                    case RSK_SKIP_CHILD_STEALTH:
                    case RSK_SKIP_EPONA_RACE:
                    case RSK_SKIP_TOWER_ESCAPE:
                        if(it.value() == "Don't Skip") {
                            gSaveContext.randoSettings[index].value = 0;
                        } else if (it.value() == "Skip") {
                            gSaveContext.randoSettings[index].value = 1;
                        }
                        break;
                    case RSK_SHUFFLE_TOKENS:
                        if (it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = 0;
                        } else if (it.value() == "Dungeons") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if (it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if (it.value() == "All Tokens") {
                            gSaveContext.randoSettings[index].value = 3;
                        }
                        break;
                    case RSK_LINKS_POCKET:
                        if (it.value() == "Dungeon Reward") {
                            gSaveContext.randoSettings[index].value = 0;
                        } else if (it.value() == "Advancement") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if (it.value() == "Anything") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if (it.value() == "Nothing") {
                            gSaveContext.randoSettings[index].value = 3;
                        }
                        break;
                }
                index++;
            }
        }

        success = true;
    } catch (const std::exception& e) {
        return;
    }
}

std::string AltarIconString(char iconChar) {
    std::string iconString = "";
    switch (iconChar) {
        case '0':
            // Kokiri Emerald
            iconString += 0x13;
            iconString += 0x6C;
            break;
        case '1':
            // Goron Ruby
            iconString += 0x13;
            iconString += 0x6D;
            break;
        case '2':
            // Zora Sapphire
            iconString += 0x13;
            iconString += 0x6E;
            break;
        case '3':
            // Forest Medallion
            iconString += 0x13;
            iconString += 0x66;
            break;
        case '4':
            // Fire Medallion
            iconString += 0x13;
            iconString += 0x67;
            break;
        case '5':
            // Water Medallion
            iconString += 0x13;
            iconString += 0x68;
            break;
        case '6':
            // Spirit Medallion
            iconString += 0x13;
            iconString += 0x69;
            break;
        case '7':
            // Shadow Medallion
            iconString += 0x13;
            iconString += 0x6A;
            break;
        case '8':
            // Light Medallion
            iconString += 0x13;
            iconString += 0x6B;
            break;
        case 'o':
            // Open DOT (master sword)
            iconString += 0x13;
            iconString += 0x3C;
            break;
        case 'c':
            // Closed DOT (fairy ocarina)
            iconString += 0x13;
            iconString += 0x07;
            break;
        case 'i':
            // Intended DOT (oot)
            iconString += 0x13;
            iconString += 0x08;
            break;
        case 'l':
            // Light Arrow (for bridge reqs)
            iconString += 0x13;
            iconString += 0x12;
            break;
        case 'b':
            // Boss Key (ganon boss key location)
            iconString += 0x13;
            iconString += 0x74;
            break;
        case 'L':
            // Bow with Light Arrow
            iconString += 0x13;
            iconString += 0x3A;
            break;
        case 'k':
            // Kokiri Tunic
            iconString += 0x13;
            iconString += 0x41;
            break;
    }
    return iconString;
}

std::string FormatJsonHintText(std::string jsonHint) {
    std::string formattedHintMessage = jsonHint;

    // add icons to altar text
    for (char iconChar : {'0', '1', '2', '3', '4', '5', '6', '7', '8', 'o', 'c', 'i', 'l', 'b', 'L', 'k'}) {
        std::string textToReplace = "$";
        textToReplace += iconChar;
        size_t start_pos = formattedHintMessage.find(textToReplace);
        if(!(start_pos == std::string::npos)) {
            std::string iconString = AltarIconString(iconChar);
            formattedHintMessage.replace(start_pos, textToReplace.length(), iconString);
        }
    }

    formattedHintMessage += 0x02;

    return formattedHintMessage;
}

void Randomizer::ParseHintLocationsFile(const char* spoilerFileName) {
    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream)
        return;

    bool success = false;

    try {
        json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;

        std::string childAltarJsonText = spoilerFileJson["childAltarText"].get<std::string>();
        std::string formattedChildAltarText = FormatJsonHintText(childAltarJsonText);
        memcpy(gSaveContext.childAltarText, formattedChildAltarText.c_str(), formattedChildAltarText.length());

        std::string adultAltarJsonText = spoilerFileJson["adultAltarText"].get<std::string>();
        std::string formattedAdultAltarText = FormatJsonHintText(adultAltarJsonText);
        memcpy(gSaveContext.adultAltarText, formattedAdultAltarText.c_str(), formattedAdultAltarText.length());

        std::string ganonHintJsonText = spoilerFileJson["ganonHintText"].get<std::string>();
        std::string formattedGanonHintJsonText = FormatJsonHintText(ganonHintJsonText);
        memcpy(gSaveContext.ganonHintText, formattedGanonHintJsonText.c_str(), formattedGanonHintJsonText.length());

        std::string ganonJsonText = spoilerFileJson["ganonText"].get<std::string>();
        std::string formattedGanonJsonText = FormatJsonHintText(ganonJsonText);
        memcpy(gSaveContext.ganonText, formattedGanonJsonText.c_str(), formattedGanonJsonText.length());

        json hintsJson = spoilerFileJson["hints"];
        int index = 0;
        for (auto it = hintsJson.begin(); it != hintsJson.end(); ++it) {
            gSaveContext.hintLocations[index].check = SpoilerfileCheckNameToEnum[it.key()];

            std::string hintMessage = FormatJsonHintText(it.value());
            memcpy(gSaveContext.hintLocations[index].hintText, hintMessage.c_str(), hintMessage.length());

            index++;
        }

        success = true;
    } catch (const std::exception& e) {
        return;
    }
}

void Randomizer::ParseRequiredTrialsFile(const char* spoilerFileName) {
    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream) {
        return;
    }

    this->trialsRequired.clear();

    try {
        json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;
        json trialsJson = spoilerFileJson["requiredTrials"];

        for (auto it = trialsJson.begin(); it != trialsJson.end(); it++) {
            this->trialsRequired[spoilerFileTrialToEnum[it.value()]] = true;
        }
    } catch (const std::exception& e) {
        return;
    }
}

void Randomizer::ParseItemLocationsFile(const char* spoilerFileName, bool silent) {
    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream)
        return;

    bool success = false;

    try {
        json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;
        json locationsJson = spoilerFileJson["locations"];
        json hashJson = spoilerFileJson["file_hash"];

        int index = 0;
        for (auto it = hashJson.begin(); it != hashJson.end(); ++it) {
            gSaveContext.seedIcons[index] = gSeedTextures[it.value()].id;
            index++;
        }

        index = 0;
        for (auto it = locationsJson.begin(); it != locationsJson.end(); ++it) {
            if (it->is_structured()) {
                json itemJson = *it;
                for (auto itemit = itemJson.begin(); itemit != itemJson.end(); ++itemit) {
                    // todo handle prices
                    if (itemit.key() == "item") {
                        gSaveContext.itemLocations[index].check = SpoilerfileCheckNameToEnum[it.key()];
                        gSaveContext.itemLocations[index].get = SpoilerfileGetNameToEnum[itemit.value()];
                    } else if (itemit.key() == "price") {
                        merchantPrices[gSaveContext.itemLocations[index].check] = itemit.value();
                    }
                }
            } else {
                gSaveContext.itemLocations[index].check = SpoilerfileCheckNameToEnum[it.key()];
                gSaveContext.itemLocations[index].get = SpoilerfileGetNameToEnum[it.value()];
            }

            index++;
        }

        if(!silent) {
            Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
        success = true;
    } catch (const std::exception& e) {
        Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        return;
    }
}

bool Randomizer::IsTrialRequired(RandomizerInf trial) {
    return this->trialsRequired.contains(trial);
}

RandomizerGet Randomizer::GetRandomizerGetFromActor(s16 actorId, s16 sceneNum, s16 actorParams) {
    return this->itemLocations[GetCheckFromActor(actorId, sceneNum, actorParams)];
}

RandomizerGet Randomizer::GetRandomizerGetFromKnownCheck(RandomizerCheck randomizerCheck) {
    return this->itemLocations[randomizerCheck];
}

GetItemID Randomizer::GetItemIdFromActor(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogItemId) {
    return GetItemIdFromRandomizerGet(GetRandomizerGetFromActor(actorId, sceneNum, actorParams), ogItemId);
}

ItemObtainability Randomizer::GetItemObtainabilityFromRandomizerCheck(RandomizerCheck randomizerCheck) {
    return GetItemObtainabilityFromRandomizerGet(GetRandomizerGetFromKnownCheck(randomizerCheck));
}

ItemObtainability Randomizer::GetItemObtainabilityFromRandomizerGet(RandomizerGet randoGet) {

    // Shopsanity with at least one item shuffled allows for a third wallet upgrade.
    // This is needed since Plentiful item pool also adds a third progressive wallet
    // but we should *not* get Tycoon's Wallet in that mode.
    u8 numWallets = GetRandoSettingValue(RSK_SHOPSANITY) > 1 ? 3 : 2;
    switch (randoGet) {
        case RG_NONE:
        case RG_TRIFORCE:
        case RG_HINT:
        case RG_MAX:
        case RG_SOLD_OUT:
        // TODO: Implement key rings
        case RG_FOREST_TEMPLE_KEY_RING:
        case RG_FIRE_TEMPLE_KEY_RING:
        case RG_WATER_TEMPLE_KEY_RING:
        case RG_SPIRIT_TEMPLE_KEY_RING:
        case RG_SHADOW_TEMPLE_KEY_RING:
        case RG_BOTTOM_OF_THE_WELL_KEY_RING:
        case RG_GERUDO_TRAINING_GROUNDS_KEY_RING:
        case RG_GERUDO_FORTRESS_KEY_RING:
        case RG_GANONS_CASTLE_KEY_RING:
            return CANT_OBTAIN_MISC;

        // Equipment
        case RG_KOKIRI_SWORD:
            return !CHECK_OWNED_EQUIP(EQUIP_SWORD, 0) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BIGGORON_SWORD:
            return !gSaveContext.bgsFlag ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DEKU_SHIELD:
        case RG_BUY_DEKU_SHIELD:
            return !CHECK_OWNED_EQUIP(EQUIP_SHIELD, 0) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_HYLIAN_SHIELD:
        case RG_BUY_HYLIAN_SHIELD:
            return !CHECK_OWNED_EQUIP(EQUIP_SHIELD, 1) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_MIRROR_SHIELD:
            return !CHECK_OWNED_EQUIP(EQUIP_SHIELD, 2) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GORON_TUNIC:
        case RG_BUY_GORON_TUNIC:
            return !CHECK_OWNED_EQUIP(EQUIP_TUNIC, 1) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_ZORA_TUNIC:
        case RG_BUY_ZORA_TUNIC:
            return !CHECK_OWNED_EQUIP(EQUIP_TUNIC, 2) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_IRON_BOOTS:
            return !CHECK_OWNED_EQUIP(EQUIP_BOOTS, 1) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_HOVER_BOOTS:
            return !CHECK_OWNED_EQUIP(EQUIP_BOOTS, 2) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

        // Inventory Items
        case RG_PROGRESSIVE_STICK_UPGRADE:
            return CUR_UPG_VALUE(UPG_STICKS) < 3 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_PROGRESSIVE_NUT_UPGRADE:
            return CUR_UPG_VALUE(UPG_NUTS) < 3 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_PROGRESSIVE_BOMB_BAG:
            return CUR_UPG_VALUE(UPG_BOMB_BAG) < 3 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BOMBS_5:
        case RG_BOMBS_10:
        case RG_BOMBS_20:
        case RG_BUY_BOMBS_525:
        case RG_BUY_BOMBS_535:
        case RG_BUY_BOMBS_10:
        case RG_BUY_BOMBS_20:
        case RG_BUY_BOMBS_30:
            return CUR_UPG_VALUE(UPG_BOMB_BAG) ? CAN_OBTAIN : CANT_OBTAIN_NEED_UPGRADE;
        case RG_PROGRESSIVE_BOW:
            return CUR_UPG_VALUE(UPG_QUIVER) < 3 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_ARROWS_5:
        case RG_ARROWS_10:
        case RG_ARROWS_30:
        case RG_BUY_ARROWS_10:
        case RG_BUY_ARROWS_30:
        case RG_BUY_ARROWS_50:
            return CUR_UPG_VALUE(UPG_QUIVER) ? CAN_OBTAIN : CANT_OBTAIN_NEED_UPGRADE;
        case RG_PROGRESSIVE_SLINGSHOT:
            return CUR_UPG_VALUE(UPG_BULLET_BAG) < 3 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DEKU_SEEDS_30:
        case RG_BUY_DEKU_SEEDS_30:
            return CUR_UPG_VALUE(UPG_BULLET_BAG) ? CAN_OBTAIN : CANT_OBTAIN_NEED_UPGRADE;
        case RG_PROGRESSIVE_OCARINA:
            switch (INV_CONTENT(ITEM_OCARINA_FAIRY)) {
                case ITEM_NONE:
                case ITEM_OCARINA_FAIRY:
                    return CAN_OBTAIN;
                case ITEM_OCARINA_TIME:
                default:
                    return CANT_OBTAIN_ALREADY_HAVE;
            }
        case RG_BOMBCHU_5:
        case RG_BOMBCHU_10:
        case RG_BOMBCHU_20:
        case RG_PROGRESSIVE_BOMBCHUS:
            return CAN_OBTAIN;
        case RG_BUY_BOMBCHU_10:
        case RG_BUY_BOMBCHU_20:
        case RG_BOMBCHU_DROP:
            // If Bombchus aren't in logic, you need a bomb bag to purchase them
            // If they are in logic, you need to have already obtained them somewhere else
            // Bombchu Drop is only used as a bowling reward, so it needs the same logic
            if (GetRandoSettingValue(RSK_BOMBCHUS_IN_LOGIC)) {
                return INV_CONTENT(ITEM_BOMBCHU) == ITEM_BOMBCHU ? CAN_OBTAIN : CANT_OBTAIN_NEED_UPGRADE;
            } else {
                return CUR_UPG_VALUE(UPG_BOMB_BAG) ? CAN_OBTAIN : CANT_OBTAIN_NEED_UPGRADE;
            }
        case RG_PROGRESSIVE_HOOKSHOT:
            switch (INV_CONTENT(ITEM_HOOKSHOT)) {
                case ITEM_NONE:
                case ITEM_HOOKSHOT:
                    return CAN_OBTAIN;
                case ITEM_LONGSHOT:
                default:
                    return CANT_OBTAIN_ALREADY_HAVE;
            }
        case RG_BOOMERANG:
            return INV_CONTENT(ITEM_BOOMERANG) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_LENS_OF_TRUTH:
            return INV_CONTENT(ITEM_LENS) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_MAGIC_BEAN:
        case RG_MAGIC_BEAN_PACK:
            return AMMO(ITEM_BEAN) < 10 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_MEGATON_HAMMER:
            return INV_CONTENT(ITEM_HAMMER) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_ARROWS:
            return INV_CONTENT(ITEM_ARROW_FIRE) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_ICE_ARROWS:
            return INV_CONTENT(ITEM_ARROW_ICE) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_LIGHT_ARROWS:
            return INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DINS_FIRE:
            return INV_CONTENT(ITEM_DINS_FIRE) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FARORES_WIND:
            return INV_CONTENT(ITEM_FARORES_WIND) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_NAYRUS_LOVE:
            return INV_CONTENT(ITEM_NAYRUS_LOVE) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

        // Bottles
        case RG_EMPTY_BOTTLE:
        case RG_BOTTLE_WITH_MILK:
        case RG_BOTTLE_WITH_RED_POTION:
        case RG_BOTTLE_WITH_GREEN_POTION:
        case RG_BOTTLE_WITH_BLUE_POTION:
        case RG_BOTTLE_WITH_FAIRY:
        case RG_BOTTLE_WITH_FISH:
        case RG_BOTTLE_WITH_BLUE_FIRE:
        case RG_BOTTLE_WITH_BUGS:
        case RG_BOTTLE_WITH_POE:
        case RG_RUTOS_LETTER:
        case RG_BOTTLE_WITH_BIG_POE:
            return Inventory_HasEmptyBottleSlot() ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

        // Bottle Refills
        case RG_MILK:
        case RG_RED_POTION_REFILL:
        case RG_GREEN_POTION_REFILL:
        case RG_BLUE_POTION_REFILL:
        case RG_BUY_FISH:
        case RG_BUY_RED_POTION_30:
        case RG_BUY_GREEN_POTION:
        case RG_BUY_BLUE_POTION:
        case RG_BUY_BLUE_FIRE:
        case RG_BUY_BOTTLE_BUG:
        case RG_BUY_POE:
        case RG_BUY_FAIRYS_SPIRIT:
        case RG_BUY_RED_POTION_40:
        case RG_BUY_RED_POTION_50:
            return Inventory_HasEmptyBottle() ? CAN_OBTAIN : CANT_OBTAIN_NEED_EMPTY_BOTTLE;

        // Trade Items
        // TODO: Do we want to be strict about any of this?
        // case RG_WEIRD_EGG:
        // case RG_ZELDAS_LETTER:
        // case RG_POCKET_EGG:
        // case RG_COJIRO:
        // case RG_ODD_MUSHROOM:
        // case RG_ODD_POTION:
        // case RG_POACHERS_SAW:
        // case RG_BROKEN_SWORD:
        // case RG_PRESCRIPTION:
        // case RG_EYEBALL_FROG:
        // case RG_EYEDROPS:
        // case RG_CLAIM_CHECK:
        // case RG_PROGRESSIVE_GORONSWORD:
        // case RG_GIANTS_KNIFE:

        // Misc Items
        case RG_STONE_OF_AGONY:
            return !CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GERUDO_MEMBERSHIP_CARD:
            return !CHECK_QUEST_ITEM(QUEST_GERUDO_CARD) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DOUBLE_DEFENSE:
            return !gSaveContext.doubleDefense ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GOLD_SKULLTULA_TOKEN:
            return gSaveContext.inventory.gsTokens < 100 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_PROGRESSIVE_STRENGTH:
            return CUR_UPG_VALUE(UPG_STRENGTH) < 3 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_PROGRESSIVE_WALLET:
            return CUR_UPG_VALUE(UPG_WALLET) < numWallets ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_PROGRESSIVE_SCALE:
            return CUR_UPG_VALUE(UPG_SCALE) < 2 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_PROGRESSIVE_MAGIC_METER:
        case RG_MAGIC_SINGLE:
        case RG_MAGIC_DOUBLE:
            return gSaveContext.magicLevel < 2 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

        // Songs
        case RG_ZELDAS_LULLABY:
            return !CHECK_QUEST_ITEM(QUEST_SONG_LULLABY) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_EPONAS_SONG:
            return !CHECK_QUEST_ITEM(QUEST_SONG_EPONA) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SARIAS_SONG:
            return !CHECK_QUEST_ITEM(QUEST_SONG_SARIA) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SUNS_SONG:
            return !CHECK_QUEST_ITEM(QUEST_SONG_SUN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SONG_OF_TIME:
            return !CHECK_QUEST_ITEM(QUEST_SONG_TIME) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SONG_OF_STORMS:
            return !CHECK_QUEST_ITEM(QUEST_SONG_STORMS) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_MINUET_OF_FOREST:
            return !CHECK_QUEST_ITEM(QUEST_SONG_MINUET) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BOLERO_OF_FIRE:
            return !CHECK_QUEST_ITEM(QUEST_SONG_BOLERO) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SERENADE_OF_WATER:
            return !CHECK_QUEST_ITEM(QUEST_SONG_SERENADE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_REQUIEM_OF_SPIRIT:
            return !CHECK_QUEST_ITEM(QUEST_SONG_REQUIEM) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_NOCTURNE_OF_SHADOW:
            return !CHECK_QUEST_ITEM(QUEST_SONG_NOCTURNE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_PRELUDE_OF_LIGHT:
            return !CHECK_QUEST_ITEM(QUEST_SONG_PRELUDE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

        // Dungeon Items
        case RG_DEKU_TREE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_YDAN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DODONGOS_CAVERN_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_DDAN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_JABU_JABUS_BELLY_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_BDAN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FOREST_TEMPLE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_BMORI1) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_TEMPLE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_HIDAN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_WATER_TEMPLE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_MIZUSIN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SPIRIT_TEMPLE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_JYASINZOU) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SHADOW_TEMPLE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_HAKADAN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BOTTOM_OF_THE_WELL_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_HAKADANCH) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_ICE_CAVERN_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_ICE_DOUKUTO) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DEKU_TREE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_YDAN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DODONGOS_CAVERN_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_DDAN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_JABU_JABUS_BELLY_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_BDAN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FOREST_TEMPLE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_BMORI1) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_TEMPLE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_HIDAN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_WATER_TEMPLE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_MIZUSIN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SPIRIT_TEMPLE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_JYASINZOU) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SHADOW_TEMPLE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_HAKADAN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BOTTOM_OF_THE_WELL_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_HAKADANCH) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_ICE_CAVERN_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_ICE_DOUKUTO) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FOREST_TEMPLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_BMORI1) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_TEMPLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_HIDAN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_WATER_TEMPLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_MIZUSIN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SPIRIT_TEMPLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_JYASINZOU) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SHADOW_TEMPLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_HAKADAN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GANONS_CASTLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_GANON) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        // TODO: Handle MQ key counts
        case RG_FOREST_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_BMORI1] < 5 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_HIDAN] < 8 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_WATER_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_MIZUSIN] < 6 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SPIRIT_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_JYASINZOU] < 5 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SHADOW_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_HAKADAN] < 5 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BOTTOM_OF_THE_WELL_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_HAKADANCH] < 3 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_MEN] < 9 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GERUDO_FORTRESS_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_GERUDOWAY] < 4 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GANONS_CASTLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_GANONTIKA] < 2 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_TREASURE_GAME_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_TAKARAYA] < 6 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

        // Dungeon Rewards
        case RG_KOKIRI_EMERALD:
            return !CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GORON_RUBY:
            return !CHECK_QUEST_ITEM(QUEST_GORON_RUBY) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_ZORA_SAPPHIRE:
            return !CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FOREST_MEDALLION:
            return !CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_MEDALLION:
            return !CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_WATER_MEDALLION:
            return !CHECK_QUEST_ITEM(QUEST_MEDALLION_WATER) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SPIRIT_MEDALLION:
            return !CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SHADOW_MEDALLION:
            return !CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_LIGHT_MEDALLION:
            return !CHECK_QUEST_ITEM(QUEST_MEDALLION_LIGHT) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

        case RG_RECOVERY_HEART:
        case RG_GREEN_RUPEE:
        case RG_BLUE_RUPEE:
        case RG_RED_RUPEE:
        case RG_PURPLE_RUPEE:
        case RG_HUGE_RUPEE:
        case RG_PIECE_OF_HEART:
        case RG_HEART_CONTAINER:
        case RG_ICE_TRAP:
        case RG_DEKU_NUTS_5:
        case RG_DEKU_NUTS_10:
        case RG_DEKU_STICK_1:
        case RG_TREASURE_GAME_HEART:
        case RG_TREASURE_GAME_GREEN_RUPEE:
        case RG_BUY_DEKU_NUT_5:
        case RG_BUY_DEKU_NUT_10:
        case RG_BUY_DEKU_STICK_1:
        case RG_BUY_HEART:
        default:
            return CAN_OBTAIN;
    }
}

GetItemID Randomizer::GetItemIdFromRandomizerGet(RandomizerGet randoGet, GetItemID ogItemId) {
    switch (randoGet) {
        case RG_NONE:
            return ogItemId;
        case RG_TRIFORCE:
        case RG_HINT:
        case RG_MAX:
        case RG_SOLD_OUT:
            return GI_NONE;

        // Equipment
        case RG_KOKIRI_SWORD:
            return GI_SWORD_KOKIRI;
        case RG_PROGRESSIVE_GORONSWORD: //todo progressive?
            return GI_SWORD_BGS;
        case RG_GIANTS_KNIFE:
            return GI_SWORD_KNIFE;
        case RG_BIGGORON_SWORD:
            return GI_SWORD_BGS;
        case RG_DEKU_SHIELD:
        case RG_BUY_DEKU_SHIELD:
            return GI_SHIELD_DEKU;
        case RG_HYLIAN_SHIELD:
        case RG_BUY_HYLIAN_SHIELD:
            return GI_SHIELD_HYLIAN;
        case RG_MIRROR_SHIELD:
            return GI_SHIELD_MIRROR;
        case RG_GORON_TUNIC:
        case RG_BUY_GORON_TUNIC:
            return GI_TUNIC_GORON;
        case RG_ZORA_TUNIC:
        case RG_BUY_ZORA_TUNIC:
            return GI_TUNIC_ZORA;
        case RG_IRON_BOOTS:
            return GI_BOOTS_IRON;
        case RG_HOVER_BOOTS:
            return GI_BOOTS_HOVER;

        // Inventory Items
        case RG_PROGRESSIVE_STICK_UPGRADE:
            switch (CUR_UPG_VALUE(UPG_STICKS)) {
                case 0:
                case 1:
                    return GI_STICK_UPGRADE_20;
                case 2:
                    return GI_STICK_UPGRADE_30;
            }
        case RG_PROGRESSIVE_NUT_UPGRADE:
            switch (CUR_UPG_VALUE(UPG_NUTS)) {
                case 0:
                case 1:
                    return GI_NUT_UPGRADE_30;
                case 2:
                    return GI_NUT_UPGRADE_40;
            }
        case RG_PROGRESSIVE_BOMB_BAG:
            switch (CUR_UPG_VALUE(UPG_BOMB_BAG)) {
                case 0:
                    return GI_BOMB_BAG_20;
                case 1:
                    return GI_BOMB_BAG_30;
                case 2:
                    return GI_BOMB_BAG_40;
            }
        case RG_BOMBS_5:
        case RG_BUY_BOMBS_525:
        case RG_BUY_BOMBS_535:
            return GI_BOMBS_5;
        case RG_BOMBS_10:
        case RG_BUY_BOMBS_10:
            return GI_BOMBS_10;
        case RG_BOMBS_20:
        case RG_BUY_BOMBS_20:
            return GI_BOMBS_20;
        case RG_BUY_BOMBS_30:
            return GI_BOMBS_30;
        case RG_PROGRESSIVE_BOW:
            switch (CUR_UPG_VALUE(UPG_QUIVER)) {
                case 0:
                    return GI_BOW;
                case 1:
                    return GI_QUIVER_40;
                case 2:
                    return GI_QUIVER_50;
            }
        case RG_ARROWS_5:
        case RG_BUY_ARROWS_10:
            return GI_ARROWS_SMALL;
        case RG_ARROWS_10:
        case RG_BUY_ARROWS_30:
            return GI_ARROWS_MEDIUM;
        case RG_ARROWS_30:
        case RG_BUY_ARROWS_50:
            return GI_ARROWS_LARGE;
        case RG_PROGRESSIVE_SLINGSHOT:
            switch (CUR_UPG_VALUE(UPG_BULLET_BAG)) {
                case 0:
                    return GI_SLINGSHOT;
                case 1:
                    return GI_BULLET_BAG_40;
                case 2:
                    return GI_BULLET_BAG_50;
            }
        case RG_DEKU_SEEDS_30:
        case RG_BUY_DEKU_SEEDS_30:
            return GI_SEEDS_30;
        case RG_PROGRESSIVE_OCARINA:
            switch (INV_CONTENT(ITEM_OCARINA_FAIRY)) {
                case ITEM_NONE:
                    return GI_OCARINA_FAIRY;
                case ITEM_OCARINA_FAIRY:
                    return GI_OCARINA_OOT;
            }
        case RG_PROGRESSIVE_BOMBCHUS:
            if (INV_CONTENT(ITEM_BOMBCHU) == ITEM_NONE) {
                return GI_BOMBCHUS_20;
            }
            if (AMMO(ITEM_BOMBCHU) < 5) {
                return GI_BOMBCHUS_10;
            }
            return GI_BOMBCHUS_5;
        case RG_BOMBCHU_5:
        case RG_BOMBCHU_DROP:
            return GI_BOMBCHUS_5;
        case RG_BOMBCHU_10:
        case RG_BUY_BOMBCHU_10:
            return GI_BOMBCHUS_10;
        case RG_BOMBCHU_20:
        case RG_BUY_BOMBCHU_20:
            return GI_BOMBCHUS_20;
        case RG_PROGRESSIVE_HOOKSHOT:
            switch (INV_CONTENT(ITEM_HOOKSHOT)) {
                case ITEM_NONE:
                    return GI_HOOKSHOT;
                case ITEM_HOOKSHOT:
                    return GI_LONGSHOT;
            }
        case RG_BOOMERANG:
            return GI_BOOMERANG;
        case RG_LENS_OF_TRUTH:
            return GI_LENS;
        case RG_MAGIC_BEAN:
            return GI_BEAN;
        case RG_MEGATON_HAMMER:
            return GI_HAMMER;
        case RG_FIRE_ARROWS:
            return GI_ARROW_FIRE;
        case RG_ICE_ARROWS:
            return GI_ARROW_ICE;
        case RG_LIGHT_ARROWS:
            return GI_ARROW_LIGHT;
        case RG_DINS_FIRE:
            return GI_DINS_FIRE;
        case RG_FARORES_WIND:
            return GI_FARORES_WIND;
        case RG_NAYRUS_LOVE:
            return GI_NAYRUS_LOVE;

        // Bottles
        case RG_EMPTY_BOTTLE:
            return GI_BOTTLE;
        case RG_BOTTLE_WITH_MILK:
            return GI_MILK_BOTTLE;
        case RG_RUTOS_LETTER:
            return GI_LETTER_RUTO;

        // Bottle Refills
        case RG_MILK:
            return GI_MILK;
        case RG_RED_POTION_REFILL:
        case RG_BUY_RED_POTION_30:
        case RG_BUY_RED_POTION_40:
        case RG_BUY_RED_POTION_50:
            return GI_POTION_RED;
        case RG_GREEN_POTION_REFILL:
        case RG_BUY_GREEN_POTION:
            return GI_POTION_GREEN;
        case RG_BLUE_POTION_REFILL:
        case RG_BUY_BLUE_POTION:
            return GI_POTION_BLUE;
        case RG_BUY_FISH:
            return GI_FISH;
        case RG_BUY_BLUE_FIRE:
            return GI_BLUE_FIRE;
        case RG_BUY_BOTTLE_BUG:
            return GI_BUGS;
        case RG_BUY_POE:
            return GI_POE;
        case RG_BUY_FAIRYS_SPIRIT:
            return GI_FAIRY;

        // Trade Items
        case RG_WEIRD_EGG:
            return GI_WEIRD_EGG;
        case RG_ZELDAS_LETTER:
            return GI_LETTER_ZELDA;
        case RG_POCKET_EGG:
            return GI_POCKET_EGG;
        case RG_COJIRO:
            return GI_COJIRO;
        case RG_ODD_MUSHROOM:
            return GI_ODD_MUSHROOM;
        case RG_ODD_POTION:
            return GI_ODD_POTION;
        case RG_POACHERS_SAW:
            return GI_SAW;
        case RG_BROKEN_SWORD:
            return GI_SWORD_BROKEN;
        case RG_PRESCRIPTION:
            return GI_PRESCRIPTION;
        case RG_EYEBALL_FROG:
            return GI_FROG;
        case RG_EYEDROPS:
            return GI_EYEDROPS;
        case RG_CLAIM_CHECK:
            return GI_CLAIM_CHECK;

        // Misc Items
        case RG_STONE_OF_AGONY:
            return GI_STONE_OF_AGONY;
        case RG_GERUDO_MEMBERSHIP_CARD:
            return GI_GERUDO_CARD;
        case RG_GOLD_SKULLTULA_TOKEN:
            return GI_SKULL_TOKEN;
        case RG_PROGRESSIVE_STRENGTH:
            switch (CUR_UPG_VALUE(UPG_STRENGTH)) {
                case 0:
                    return GI_BRACELET;
                case 1:
                    return GI_GAUNTLETS_SILVER;
                case 2:
                    return GI_GAUNTLETS_GOLD;
            }
        case RG_PROGRESSIVE_WALLET:
            switch (CUR_UPG_VALUE(UPG_WALLET)) {
                case 0:
                    return GI_WALLET_ADULT;
                case 1:
                    return GI_WALLET_GIANT;
                case 2:
                    return (GetItemID)RG_TYCOON_WALLET;
            }
        case RG_PROGRESSIVE_SCALE:
            switch (CUR_UPG_VALUE(UPG_SCALE)) {
                case 0:
                    return GI_SCALE_SILVER;
                case 1:
                    return GI_SCALE_GOLD;
            }
        case RG_PROGRESSIVE_MAGIC_METER:
            switch (gSaveContext.magicLevel) {
                case 0:
                    return (GetItemID)RG_MAGIC_SINGLE;
                case 1:
                    return (GetItemID)RG_MAGIC_DOUBLE;
            }

        case RG_DEKU_TREE_MAP:
        case RG_DODONGOS_CAVERN_MAP:
        case RG_JABU_JABUS_BELLY_MAP:
        case RG_FOREST_TEMPLE_MAP:
        case RG_FIRE_TEMPLE_MAP:
        case RG_WATER_TEMPLE_MAP:
        case RG_SPIRIT_TEMPLE_MAP:
        case RG_SHADOW_TEMPLE_MAP:
        case RG_BOTTOM_OF_THE_WELL_MAP:
        case RG_ICE_CAVERN_MAP:
            if (GetRandoSettingValue(RSK_STARTING_MAPS_COMPASSES) < 3) {
                return GI_MAP;
            } else {
                return (GetItemID)randoGet;
            }

        case RG_DEKU_TREE_COMPASS:
        case RG_DODONGOS_CAVERN_COMPASS:
        case RG_JABU_JABUS_BELLY_COMPASS:
        case RG_FOREST_TEMPLE_COMPASS:
        case RG_FIRE_TEMPLE_COMPASS:
        case RG_WATER_TEMPLE_COMPASS:
        case RG_SPIRIT_TEMPLE_COMPASS:
        case RG_SHADOW_TEMPLE_COMPASS:
        case RG_BOTTOM_OF_THE_WELL_COMPASS:
        case RG_ICE_CAVERN_COMPASS:
            if (GetRandoSettingValue(RSK_STARTING_MAPS_COMPASSES) < 3) {
                return GI_COMPASS;
            } else {
                return (GetItemID)randoGet;
            }

        case RG_FOREST_TEMPLE_BOSS_KEY:
        case RG_FIRE_TEMPLE_BOSS_KEY:
        case RG_WATER_TEMPLE_BOSS_KEY:
        case RG_SPIRIT_TEMPLE_BOSS_KEY:
        case RG_SHADOW_TEMPLE_BOSS_KEY:
            if (GetRandoSettingValue(RSK_BOSS_KEYSANITY) < 3) {
                return GI_KEY_BOSS;
            } else {
                return (GetItemID)randoGet;
            }
        case RG_GANONS_CASTLE_BOSS_KEY:
            if (GetRandoSettingValue(RSK_GANONS_BOSS_KEY) < 3) {
                return GI_KEY_BOSS;
            } else {
                return (GetItemID)randoGet;
            }

        case RG_FOREST_TEMPLE_SMALL_KEY:
        case RG_FIRE_TEMPLE_SMALL_KEY:
        case RG_WATER_TEMPLE_SMALL_KEY:
        case RG_SPIRIT_TEMPLE_SMALL_KEY:
        case RG_SHADOW_TEMPLE_SMALL_KEY:
        case RG_BOTTOM_OF_THE_WELL_SMALL_KEY:
        case RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY:
        case RG_GANONS_CASTLE_SMALL_KEY:
            if (GetRandoSettingValue(RSK_KEYSANITY) < 3) {
                return GI_KEY_SMALL;
            } else {
                return (GetItemID)randoGet;
            }
        case RG_GERUDO_FORTRESS_SMALL_KEY:
            if (GetRandoSettingValue(RSK_GERUDO_KEYS) == 0) {
                return GI_KEY_SMALL;
            } else {
                return (GetItemID)randoGet;
            }

        case RG_RECOVERY_HEART:
        case RG_BUY_HEART:
            return GI_HEART;
        case RG_GREEN_RUPEE:
            return GI_RUPEE_GREEN;
        case RG_BLUE_RUPEE:
            return GI_RUPEE_BLUE;
        case RG_RED_RUPEE:
            return GI_RUPEE_RED;
        case RG_PURPLE_RUPEE:
            return GI_RUPEE_PURPLE;
        case RG_HUGE_RUPEE:
            return GI_RUPEE_GOLD;
        case RG_PIECE_OF_HEART:
            return GI_HEART_PIECE;
        case RG_HEART_CONTAINER:
            return GI_HEART_CONTAINER;
        
        case RG_DEKU_NUTS_5:
        case RG_BUY_DEKU_NUT_5:
            return GI_NUTS_5;
        case RG_DEKU_NUTS_10:
        case RG_BUY_DEKU_NUT_10:
            return GI_NUTS_10;
        case RG_DEKU_STICK_1:
        case RG_BUY_DEKU_STICK_1:
            return GI_STICKS_1;
        case RG_TREASURE_GAME_SMALL_KEY:
            return GI_DOOR_KEY;
        case RG_TREASURE_GAME_HEART:
            return GI_HEART_PIECE_WIN;
        case RG_TREASURE_GAME_GREEN_RUPEE:
            return GI_RUPEE_GREEN_LOSE;
        default:
            if (!IsItemVanilla(randoGet)) {
                return (GetItemID)randoGet;
            }
            return ogItemId;
    }
}

bool Randomizer::IsItemVanilla(RandomizerGet randoGet) {
    switch (randoGet) {
        case RG_NONE:
        case RG_KOKIRI_SWORD: 
        case RG_GIANTS_KNIFE: 
        case RG_BIGGORON_SWORD:
        case RG_DEKU_SHIELD:
        case RG_HYLIAN_SHIELD:
        case RG_MIRROR_SHIELD:
        case RG_GORON_TUNIC:
        case RG_ZORA_TUNIC:
        case RG_IRON_BOOTS:
        case RG_HOVER_BOOTS:
        case RG_BOOMERANG:
        case RG_LENS_OF_TRUTH:
        case RG_MEGATON_HAMMER:
        case RG_STONE_OF_AGONY:
        case RG_DINS_FIRE:
        case RG_FARORES_WIND:
        case RG_NAYRUS_LOVE:
        case RG_FIRE_ARROWS:
        case RG_ICE_ARROWS:
        case RG_LIGHT_ARROWS:
        case RG_GERUDO_MEMBERSHIP_CARD:
        case RG_MAGIC_BEAN:
        case RG_WEIRD_EGG: 
        case RG_ZELDAS_LETTER:
        case RG_RUTOS_LETTER:
        case RG_POCKET_EGG:
        case RG_COJIRO:
        case RG_ODD_MUSHROOM:
        case RG_ODD_POTION:
        case RG_POACHERS_SAW:
        case RG_BROKEN_SWORD:
        case RG_PRESCRIPTION:
        case RG_EYEBALL_FROG:
        case RG_EYEDROPS:
        case RG_CLAIM_CHECK:
        case RG_GOLD_SKULLTULA_TOKEN:
        case RG_PROGRESSIVE_HOOKSHOT:
        case RG_PROGRESSIVE_STRENGTH:
        case RG_PROGRESSIVE_BOMB_BAG:
        case RG_PROGRESSIVE_BOW:
        case RG_PROGRESSIVE_SLINGSHOT:
        case RG_PROGRESSIVE_SCALE:
        case RG_PROGRESSIVE_NUT_UPGRADE:
        case RG_PROGRESSIVE_STICK_UPGRADE:
        case RG_PROGRESSIVE_BOMBCHUS:
        case RG_PROGRESSIVE_OCARINA:
        case RG_PROGRESSIVE_GORONSWORD:
        case RG_EMPTY_BOTTLE:
        case RG_BOTTLE_WITH_MILK:
        case RG_RECOVERY_HEART:
        case RG_GREEN_RUPEE:
        case RG_BLUE_RUPEE:
        case RG_RED_RUPEE:
        case RG_PURPLE_RUPEE:
        case RG_HUGE_RUPEE:
        case RG_PIECE_OF_HEART:
        case RG_HEART_CONTAINER:
        case RG_MILK:
        case RG_BOMBS_5:
        case RG_BOMBS_10:
        case RG_BOMBS_20:
        case RG_BOMBCHU_5:
        case RG_BOMBCHU_10:
        case RG_BOMBCHU_20:
        case RG_BOMBCHU_DROP:
        case RG_ARROWS_5:
        case RG_ARROWS_10:
        case RG_ARROWS_30:
        case RG_DEKU_NUTS_5:
        case RG_DEKU_NUTS_10:
        case RG_DEKU_SEEDS_30:
        case RG_DEKU_STICK_1:
        case RG_RED_POTION_REFILL:
        case RG_GREEN_POTION_REFILL:
        case RG_BLUE_POTION_REFILL:
        case RG_TREASURE_GAME_HEART:
        case RG_TREASURE_GAME_GREEN_RUPEE:
        case RG_BUY_DEKU_NUT_5:
        case RG_BUY_ARROWS_30:
        case RG_BUY_ARROWS_50:
        case RG_BUY_BOMBS_525:
        case RG_BUY_DEKU_NUT_10:
        case RG_BUY_DEKU_STICK_1:
        case RG_BUY_BOMBS_10:
        case RG_BUY_FISH:
        case RG_BUY_RED_POTION_30:
        case RG_BUY_GREEN_POTION:
        case RG_BUY_BLUE_POTION:
        case RG_BUY_HYLIAN_SHIELD:
        case RG_BUY_DEKU_SHIELD:
        case RG_BUY_GORON_TUNIC:
        case RG_BUY_ZORA_TUNIC:
        case RG_BUY_HEART:
        case RG_BUY_BOMBCHU_10:
        case RG_BUY_BOMBCHU_20:
        case RG_BUY_DEKU_SEEDS_30:
        case RG_SOLD_OUT:
        case RG_BUY_BLUE_FIRE:
        case RG_BUY_BOTTLE_BUG:
        case RG_BUY_POE:
        case RG_BUY_FAIRYS_SPIRIT:
        case RG_BUY_ARROWS_10:
        case RG_BUY_BOMBS_20:
        case RG_BUY_BOMBS_30:
        case RG_BUY_BOMBS_535:
        case RG_BUY_RED_POTION_40:
        case RG_BUY_RED_POTION_50:
            return true;
        case RG_PROGRESSIVE_WALLET:
            if (CUR_UPG_VALUE(UPG_WALLET) < 2) {
                return true;
            } else {
                return false;
            }
        case RG_FOREST_TEMPLE_SMALL_KEY:
        case RG_FIRE_TEMPLE_SMALL_KEY:
        case RG_WATER_TEMPLE_SMALL_KEY:
        case RG_SPIRIT_TEMPLE_SMALL_KEY:
        case RG_SHADOW_TEMPLE_SMALL_KEY:
        case RG_BOTTOM_OF_THE_WELL_SMALL_KEY:
        case RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY:
        case RG_GANONS_CASTLE_SMALL_KEY:
            if (GetRandoSettingValue(RSK_KEYSANITY) > 2) {
                return false;
            }
            return true;
        case RG_GERUDO_FORTRESS_SMALL_KEY:
            if (GetRandoSettingValue(RSK_GERUDO_KEYS) != 0) {
                return false;
            }
            return true;
        case RG_FOREST_TEMPLE_BOSS_KEY:
        case RG_FIRE_TEMPLE_BOSS_KEY:
        case RG_WATER_TEMPLE_BOSS_KEY:
        case RG_SPIRIT_TEMPLE_BOSS_KEY:
        case RG_SHADOW_TEMPLE_BOSS_KEY:
            if (GetRandoSettingValue(RSK_BOSS_KEYSANITY) > 2) {
                return false;
            }
            return true;
        case RG_GANONS_CASTLE_BOSS_KEY:
            if (GetRandoSettingValue(RSK_GANONS_BOSS_KEY) > 2) {
                return false;
            }
            return true;
        case RG_DEKU_TREE_COMPASS:
        case RG_DODONGOS_CAVERN_COMPASS:
        case RG_JABU_JABUS_BELLY_COMPASS:
        case RG_FOREST_TEMPLE_COMPASS:
        case RG_FIRE_TEMPLE_COMPASS:
        case RG_WATER_TEMPLE_COMPASS:
        case RG_SPIRIT_TEMPLE_COMPASS:
        case RG_SHADOW_TEMPLE_COMPASS:
        case RG_BOTTOM_OF_THE_WELL_COMPASS:
        case RG_ICE_CAVERN_COMPASS:
        case RG_DEKU_TREE_MAP:
        case RG_DODONGOS_CAVERN_MAP:
        case RG_JABU_JABUS_BELLY_MAP:
        case RG_FOREST_TEMPLE_MAP:
        case RG_FIRE_TEMPLE_MAP:
        case RG_WATER_TEMPLE_MAP:
        case RG_SPIRIT_TEMPLE_MAP:
        case RG_SHADOW_TEMPLE_MAP:
        case RG_BOTTOM_OF_THE_WELL_MAP:
        case RG_ICE_CAVERN_MAP:
            if (GetRandoSettingValue(RSK_STARTING_MAPS_COMPASSES) > 2) {
                return false;
            }
            return true;
        default:
            return false;
    }
}

bool Randomizer::CheckContainsVanillaItem(RandomizerCheck randoCheck) {
    RandomizerGet randoGet = this->itemLocations[randoCheck];
    return IsItemVanilla(randoGet);
}

std::string Randomizer::GetAdultAltarText() const {
    return this->adultAltarText;
}

std::string Randomizer::GetChildAltarText() const {
    return this->childAltarText;
}

std::string Randomizer::GetGanonText() const {
    return ganonText;
}

std::string Randomizer::GetGanonHintText() const {
    return ganonHintText;
}

ScrubIdentity Randomizer::IdentifyScrub(s32 sceneNum, s32 actorParams, s32 respawnData) {
    struct ScrubIdentity scrubIdentity;

    scrubIdentity.randomizerCheck = RC_UNKNOWN_CHECK;
    scrubIdentity.getItemId = GI_NONE;
    scrubIdentity.itemPrice = -1;
    scrubIdentity.isShuffled = GetRandoSettingValue(RSK_SHUFFLE_SCRUBS) > 0;

    // Based on z_en_dns.c 93-113
    switch (actorParams) {
        case 0x00:
            scrubIdentity.getItemId = GI_NUTS_5_2;
            break;
        case 0x01:
            scrubIdentity.getItemId = GI_STICKS_1;
            break;
        case 0x02:
            scrubIdentity.getItemId = GI_HEART_PIECE;
            break;
        case 0x03:
            scrubIdentity.getItemId = GI_SEEDS_30;
            break;
        case 0x04:
            scrubIdentity.getItemId = GI_SHIELD_DEKU;
            break;
        case 0x05:
            scrubIdentity.getItemId = GI_BOMBS_5;
            break;
        case 0x06:
            scrubIdentity.getItemId = GI_ARROWS_LARGE;
            break;
        case 0x07:
            scrubIdentity.getItemId = GI_POTION_RED;
            break;
        case 0x08:
            scrubIdentity.getItemId = GI_POTION_GREEN;
            break;
        case 0x09:
            scrubIdentity.getItemId = GI_STICK_UPGRADE_20;
            break;
        case 0x0A:
            scrubIdentity.getItemId = GI_NUT_UPGRADE_30;
            break;
    }

    // TODO: Handle MQ scrubs
    switch (sceneNum) {
        case SCENE_DDAN: // Dodongo's Cavern
            switch (actorParams) {
                case 0x00:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT;
                    scrubIdentity.randomizerCheck = RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT;
                    break;
                case 0x01:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS;
                    scrubIdentity.randomizerCheck = RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS;
                    break;
                case 0x03:
                case 0x06:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT;
                    scrubIdentity.randomizerCheck = RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT;
                    break;
                case 0x04:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY;
                    scrubIdentity.randomizerCheck = RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY;
                    break;
            }
            break;
        case SCENE_BDAN: // Jabu Jabu's Belly
            switch (actorParams) {
                case 0x00:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_JABU_JABUS_BELLY_DEKU_SCRUB;
                    scrubIdentity.randomizerCheck = RC_JABU_JABUS_BELLY_DEKU_SCRUB;
                    break;
            }
            break;
        case SCENE_GANONTIKA: // Ganon's Castle
            switch (actorParams) {
                case 0x05:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT;
                    scrubIdentity.randomizerCheck = RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT;
                    break;
                case 0x03:
                case 0x06:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT;
                    scrubIdentity.randomizerCheck = RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT;
                    break;
                case 0x07:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_RIGHT;
                    scrubIdentity.randomizerCheck = RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT;
                    break;
                case 0x08:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_LEFT;
                    scrubIdentity.randomizerCheck = RC_GANONS_CASTLE_DEKU_SCRUB_LEFT;
                    break;
            }
            break;
        case SCENE_KAKUSIANA: // Grotto
            switch (respawnData) { 
                case 0xE6: // Hyrule Field Scrub Grotto
                    switch (actorParams) {
                        case 0x02:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_HF_DEKU_SCRUB_GROTTO;
                            scrubIdentity.randomizerCheck = RC_HF_DEKU_SCRUB_GROTTO;
                            scrubIdentity.isShuffled = true;
                            break;
                    }
                    break;
                case 0xEB: // ZR Scrub Grotto
                    switch (actorParams) {
                        case 0x07:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_ZR_DEKU_SCRUB_GROTTO_REAR;
                            scrubIdentity.randomizerCheck = RC_ZR_DEKU_SCRUB_GROTTO_REAR;
                            break;
                        case 0x08:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_ZR_DEKU_SCRUB_GROTTO_FRONT;
                            scrubIdentity.randomizerCheck = RC_ZR_DEKU_SCRUB_GROTTO_FRONT;
                            break;
                    }
                    break;
                case 0xEE: // Sacred Forest Meadow Scrub Grotto
                    switch (actorParams) {
                        case 0x07:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_SFM_DEKU_SCRUB_GROTTO_REAR;
                            scrubIdentity.randomizerCheck = RC_SFM_DEKU_SCRUB_GROTTO_REAR;
                            break;
                        case 0x08:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_SFM_DEKU_SCRUB_GROTTO_FRONT;
                            scrubIdentity.randomizerCheck = RC_SFM_DEKU_SCRUB_GROTTO_FRONT;
                            break;
                    }
                    break;
                case 0xEF: // Lake Hylia Scrub Grotto
                    switch (actorParams) {
                        case 0x00:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_LEFT;
                            scrubIdentity.randomizerCheck = RC_LH_DEKU_SCRUB_GROTTO_LEFT;
                            break;
                        case 0x05:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_RIGHT;
                            scrubIdentity.randomizerCheck = RC_LH_DEKU_SCRUB_GROTTO_RIGHT;
                            break;
                        case 0x03:
                        case 0x06:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_CENTER;
                            scrubIdentity.randomizerCheck = RC_LH_DEKU_SCRUB_GROTTO_CENTER;
                            break;
                    }
                    break;
                case 0xF0: // Gerudo Valley Scrub Grotto
                    switch (actorParams) {
                        case 0x07:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GV_DEKU_SCRUB_GROTTO_REAR;
                            scrubIdentity.randomizerCheck = RC_GV_DEKU_SCRUB_GROTTO_REAR;
                            break;
                        case 0x08:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GV_DEKU_SCRUB_GROTTO_FRONT;
                            scrubIdentity.randomizerCheck = RC_GV_DEKU_SCRUB_GROTTO_FRONT;
                            break;
                    }
                    break;
                case 0xF5: // Lost Woods Scrub Grotto
                    switch (actorParams) {
                        case 0x03:
                        case 0x06:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_GROTTO_REAR;
                            scrubIdentity.randomizerCheck = RC_LW_DEKU_SCRUB_GROTTO_REAR;
                            break;
                        case 0x0A:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_GROTTO_FRONT;
                            scrubIdentity.randomizerCheck = RC_LW_DEKU_SCRUB_GROTTO_FRONT;
                            scrubIdentity.isShuffled = true;
                            break;
                    }
                    break;
                case 0xF9: // Death Mountain Crater Scrub Grotto
                    switch (actorParams) {
                        case 0x00:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_LEFT;
                            scrubIdentity.randomizerCheck = RC_DMC_DEKU_SCRUB_GROTTO_LEFT;
                            break;
                        case 0x05:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_RIGHT;
                            scrubIdentity.randomizerCheck = RC_DMC_DEKU_SCRUB_GROTTO_RIGHT;
                            break;
                        case 0x03:
                        case 0x06:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_CENTER;
                            scrubIdentity.randomizerCheck = RC_DMC_DEKU_SCRUB_GROTTO_CENTER;
                            break;
                    }
                    break;
                case 0xFB: // Gerudo City Scrub Grotto
                    switch (actorParams) {
                        case 0x00:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_LEFT;
                            scrubIdentity.randomizerCheck = RC_GC_DEKU_SCRUB_GROTTO_LEFT;
                            break;
                        case 0x05:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_RIGHT;
                            scrubIdentity.randomizerCheck = RC_GC_DEKU_SCRUB_GROTTO_RIGHT;
                            break;
                        case 0x03:
                        case 0x06:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_CENTER;
                            scrubIdentity.randomizerCheck = RC_GC_DEKU_SCRUB_GROTTO_CENTER;
                            break;
                    }
                    break;
                case 0xFC: // Lon Lon Ranch Scrub Grotto
                    switch (actorParams) {
                        case 0x00:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_LEFT;
                            scrubIdentity.randomizerCheck = RC_LLR_DEKU_SCRUB_GROTTO_LEFT;
                            break;
                        case 0x05:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_RIGHT;
                            scrubIdentity.randomizerCheck = RC_LLR_DEKU_SCRUB_GROTTO_RIGHT;
                            break;
                        case 0x03:
                        case 0x06:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_CENTER;
                            scrubIdentity.randomizerCheck = RC_LLR_DEKU_SCRUB_GROTTO_CENTER;
                            break;
                    }
                    break;
                case 0xFD: // Desert Colossus Scrub Grotto
                    switch (actorParams) {
                        case 0x07:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_COLOSSUS_DEKU_SCRUB_GROTTO_REAR;
                            scrubIdentity.randomizerCheck = RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR;
                            break;
                        case 0x08:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT;
                            scrubIdentity.randomizerCheck = RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT;
                            break;
                    }
                    break;
            }
            break;
        case SCENE_SPOT10: // Lost woods
            switch (actorParams) {
                case 0x00:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT;
                    scrubIdentity.randomizerCheck = RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT;
                    break;
                case 0x01:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT;
                    scrubIdentity.randomizerCheck = RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT;
                    break;
                case 0x09:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_BRIDGE;
                    scrubIdentity.randomizerCheck = RC_LW_DEKU_SCRUB_NEAR_BRIDGE;
                    scrubIdentity.isShuffled = true;
                    break;
            }
            break;
        case SCENE_SPOT17: // Death Mountain Crater
            switch (actorParams) {
                case 0x05:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB;
                    scrubIdentity.randomizerCheck = RC_DMC_DEKU_SCRUB;
                    break;
            }
            break;
    }
    
    if (merchantPrices.find(scrubIdentity.randomizerCheck) != merchantPrices.end()) {
        scrubIdentity.itemPrice = merchantPrices[scrubIdentity.randomizerCheck];
    }

    return scrubIdentity;
}

ShopItemIdentity Randomizer::IdentifyShopItem(s32 sceneNum, u8 slotIndex) {
    ShopItemIdentity shopItemIdentity;

    shopItemIdentity.randomizerCheck = RC_UNKNOWN_CHECK;
    shopItemIdentity.ogItemId = GI_NONE;
    shopItemIdentity.itemPrice = -1;
    shopItemIdentity.enGirlAShopItem = 0x32;

    switch (sceneNum) {
        case SCENE_SHOP1:
            // Bazaar (SHOP1) scene is reused, so use entranceIndex to determine if we're entering from KaK or market
            if (gSaveContext.entranceIndex == 0xB7) {
                switch (slotIndex) {
                    case 0:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_1;
                        shopItemIdentity.randomizerCheck = RC_KAK_BAZAAR_ITEM_1;
                        shopItemIdentity.ogItemId = GI_SHIELD_HYLIAN;
                        break;
                    case 1:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_2;
                        shopItemIdentity.randomizerCheck = RC_KAK_BAZAAR_ITEM_2;
                        shopItemIdentity.ogItemId = GI_BOMBS_5;
                        break;
                    case 2:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_3;
                        shopItemIdentity.randomizerCheck = RC_KAK_BAZAAR_ITEM_3;
                        shopItemIdentity.ogItemId = GI_NUTS_5_2;
                        break;
                    case 3:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_4;
                        shopItemIdentity.randomizerCheck = RC_KAK_BAZAAR_ITEM_4;
                        shopItemIdentity.ogItemId = GI_HEART;
                        break;
                    case 4:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_5;
                        shopItemIdentity.randomizerCheck = RC_KAK_BAZAAR_ITEM_5;
                        shopItemIdentity.ogItemId = GI_ARROWS_SMALL;
                        break;
                    case 5:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_6;
                        shopItemIdentity.randomizerCheck = RC_KAK_BAZAAR_ITEM_6;
                        shopItemIdentity.ogItemId = GI_ARROWS_LARGE;
                        break;
                    case 6:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_7;
                        shopItemIdentity.randomizerCheck = RC_KAK_BAZAAR_ITEM_7;
                        shopItemIdentity.ogItemId = GI_STICKS_1;
                        break;
                    case 7:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_8;
                        shopItemIdentity.randomizerCheck = RC_KAK_BAZAAR_ITEM_8;
                        shopItemIdentity.ogItemId = GI_ARROWS_MEDIUM;
                        break;
                }
            } else {
                switch (slotIndex) {
                    case 0:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_1;
                        shopItemIdentity.randomizerCheck = RC_MARKET_BAZAAR_ITEM_1;
                        shopItemIdentity.ogItemId = GI_SHIELD_HYLIAN;
                        break;
                    case 1:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_2;
                        shopItemIdentity.randomizerCheck = RC_MARKET_BAZAAR_ITEM_2;
                        shopItemIdentity.ogItemId = GI_BOMBS_5;
                        break;
                    case 2:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_3;
                        shopItemIdentity.randomizerCheck = RC_MARKET_BAZAAR_ITEM_3;
                        shopItemIdentity.ogItemId = GI_NUTS_5_2;
                        break;
                    case 3:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_4;
                        shopItemIdentity.randomizerCheck = RC_MARKET_BAZAAR_ITEM_4;
                        shopItemIdentity.ogItemId = GI_HEART;
                        break;
                    case 4:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_5;
                        shopItemIdentity.randomizerCheck = RC_MARKET_BAZAAR_ITEM_5;
                        shopItemIdentity.ogItemId = GI_ARROWS_SMALL;
                        break;
                    case 5:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_6;
                        shopItemIdentity.randomizerCheck = RC_MARKET_BAZAAR_ITEM_6;
                        shopItemIdentity.ogItemId = GI_ARROWS_LARGE;
                        break;
                    case 6:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_7;
                        shopItemIdentity.randomizerCheck = RC_MARKET_BAZAAR_ITEM_7;
                        shopItemIdentity.ogItemId = GI_STICKS_1;
                        break;
                    case 7:
                        shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_8;
                        shopItemIdentity.randomizerCheck = RC_MARKET_BAZAAR_ITEM_8;
                        shopItemIdentity.ogItemId = GI_ARROWS_MEDIUM;
                        break;
                }
            }
            break;
        case SCENE_KOKIRI_SHOP:
            switch (slotIndex) {
                case 0:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1;
                    shopItemIdentity.randomizerCheck = RC_KF_SHOP_ITEM_1;
                    shopItemIdentity.ogItemId = GI_SHIELD_DEKU;
                    break;
                case 1:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_2;
                    shopItemIdentity.randomizerCheck = RC_KF_SHOP_ITEM_2;
                    shopItemIdentity.ogItemId = GI_NUTS_5_2;
                    break;
                case 2:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_3;
                    shopItemIdentity.randomizerCheck = RC_KF_SHOP_ITEM_3;
                    shopItemIdentity.ogItemId = GI_NUTS_10;
                    break;
                case 3:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_4;
                    shopItemIdentity.randomizerCheck = RC_KF_SHOP_ITEM_4;
                    shopItemIdentity.ogItemId = GI_STICKS_1;
                    break;
                case 4:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_5;
                    shopItemIdentity.randomizerCheck = RC_KF_SHOP_ITEM_5;
                    shopItemIdentity.ogItemId = GI_SEEDS_30;
                    break;
                case 5:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_6;
                    shopItemIdentity.randomizerCheck = RC_KF_SHOP_ITEM_6;
                    shopItemIdentity.ogItemId = GI_ARROWS_SMALL;
                    break;
                case 6:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_7;
                    shopItemIdentity.randomizerCheck = RC_KF_SHOP_ITEM_7;
                    shopItemIdentity.ogItemId = GI_ARROWS_MEDIUM;
                    break;
                case 7:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_8;
                    shopItemIdentity.randomizerCheck = RC_KF_SHOP_ITEM_8;
                    shopItemIdentity.ogItemId = GI_HEART;
                    break;
            }
            break;
        case SCENE_GOLON:
            switch (slotIndex) {
                case 0:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_1;
                    shopItemIdentity.randomizerCheck = RC_GC_SHOP_ITEM_1;
                    shopItemIdentity.ogItemId = GI_BOMBS_5;
                    break;
                case 1:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_2;
                    shopItemIdentity.randomizerCheck = RC_GC_SHOP_ITEM_2;
                    shopItemIdentity.ogItemId = GI_BOMBS_10;
                    break;
                case 2:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_3;
                    shopItemIdentity.randomizerCheck = RC_GC_SHOP_ITEM_3;
                    shopItemIdentity.ogItemId = GI_BOMBS_20;
                    break;
                case 3:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_4;
                    shopItemIdentity.randomizerCheck = RC_GC_SHOP_ITEM_4;
                    shopItemIdentity.ogItemId = GI_BOMBS_30;
                    break;
                case 4:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_5;
                    shopItemIdentity.randomizerCheck = RC_GC_SHOP_ITEM_5;
                    shopItemIdentity.ogItemId = GI_TUNIC_GORON;
                    break;
                case 5:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_6;
                    shopItemIdentity.randomizerCheck = RC_GC_SHOP_ITEM_6;
                    shopItemIdentity.ogItemId = GI_HEART;
                    break;
                case 6:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_7;
                    shopItemIdentity.randomizerCheck = RC_GC_SHOP_ITEM_7;
                    shopItemIdentity.ogItemId = GI_POTION_RED;
                    break;
                case 7:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_8;
                    shopItemIdentity.randomizerCheck = RC_GC_SHOP_ITEM_8;
                    shopItemIdentity.ogItemId = GI_HEART;
                    break;
            }
            break;
        case SCENE_ZOORA:
            switch (slotIndex) {
                case 0:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_1;
                    shopItemIdentity.randomizerCheck = RC_ZD_SHOP_ITEM_1;
                    shopItemIdentity.ogItemId = GI_TUNIC_ZORA;
                    break;
                case 1:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_2;
                    shopItemIdentity.randomizerCheck = RC_ZD_SHOP_ITEM_2;
                    shopItemIdentity.ogItemId = GI_ARROWS_SMALL;
                    break;
                case 2:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_3;
                    shopItemIdentity.randomizerCheck = RC_ZD_SHOP_ITEM_3;
                    shopItemIdentity.ogItemId = GI_HEART;
                    break;
                case 3:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_4;
                    shopItemIdentity.randomizerCheck = RC_ZD_SHOP_ITEM_4;
                    shopItemIdentity.ogItemId = GI_ARROWS_MEDIUM;
                    break;
                case 4:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_5;
                    shopItemIdentity.randomizerCheck = RC_ZD_SHOP_ITEM_5;
                    shopItemIdentity.ogItemId = GI_NUTS_5_2;
                    break;
                case 5:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_6;
                    shopItemIdentity.randomizerCheck = RC_ZD_SHOP_ITEM_6;
                    shopItemIdentity.ogItemId = GI_ARROWS_LARGE;
                    break;
                case 6:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_7;
                    shopItemIdentity.randomizerCheck = RC_ZD_SHOP_ITEM_7;
                    shopItemIdentity.ogItemId = GI_FISH;
                    break;
                case 7:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_8;
                    shopItemIdentity.randomizerCheck = RC_ZD_SHOP_ITEM_8;
                    shopItemIdentity.ogItemId = GI_POTION_RED;
                    break;
            }
            break;
        case SCENE_DRAG:
            switch (slotIndex) {
                case 0:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_1;
                    shopItemIdentity.randomizerCheck = RC_KAK_POTION_SHOP_ITEM_1;
                    shopItemIdentity.ogItemId = GI_POTION_GREEN;
                    break;
                case 1:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_2;
                    shopItemIdentity.randomizerCheck = RC_KAK_POTION_SHOP_ITEM_2;
                    shopItemIdentity.ogItemId = GI_BLUE_FIRE;
                    break;
                case 2:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_3;
                    shopItemIdentity.randomizerCheck = RC_KAK_POTION_SHOP_ITEM_3;
                    shopItemIdentity.ogItemId = GI_POTION_RED;
                    break;
                case 3:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_4;
                    shopItemIdentity.randomizerCheck = RC_KAK_POTION_SHOP_ITEM_4;
                    shopItemIdentity.ogItemId = GI_FAIRY;
                    break;
                case 4:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_5;
                    shopItemIdentity.randomizerCheck = RC_KAK_POTION_SHOP_ITEM_5;
                    shopItemIdentity.ogItemId = GI_NUTS_5_2;
                    break;
                case 5:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_6;
                    shopItemIdentity.randomizerCheck = RC_KAK_POTION_SHOP_ITEM_6;
                    shopItemIdentity.ogItemId = GI_BUGS;
                    break;
                case 6:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_7;
                    shopItemIdentity.randomizerCheck = RC_KAK_POTION_SHOP_ITEM_7;
                    shopItemIdentity.ogItemId = GI_POE;
                    break;
                case 7:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_8;
                    shopItemIdentity.randomizerCheck = RC_KAK_POTION_SHOP_ITEM_8;
                    shopItemIdentity.ogItemId = GI_FISH;
                    break;
            }
            break;
        case SCENE_ALLEY_SHOP:
            switch (slotIndex) {
                case 0:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_1;
                    shopItemIdentity.randomizerCheck = RC_MARKET_POTION_SHOP_ITEM_1;
                    shopItemIdentity.ogItemId = GI_POTION_GREEN;
                    break;
                case 1:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_2;
                    shopItemIdentity.randomizerCheck = RC_MARKET_POTION_SHOP_ITEM_2;
                    shopItemIdentity.ogItemId = GI_BLUE_FIRE;
                    break;
                case 2:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_3;
                    shopItemIdentity.randomizerCheck = RC_MARKET_POTION_SHOP_ITEM_3;
                    shopItemIdentity.ogItemId = GI_POTION_RED;
                    break;
                case 3:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_4;
                    shopItemIdentity.randomizerCheck = RC_MARKET_POTION_SHOP_ITEM_4;
                    shopItemIdentity.ogItemId = GI_FAIRY;
                    break;
                case 4:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_5;
                    shopItemIdentity.randomizerCheck = RC_MARKET_POTION_SHOP_ITEM_5;
                    shopItemIdentity.ogItemId = GI_NUTS_5_2;
                    break;
                case 5:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_6;
                    shopItemIdentity.randomizerCheck = RC_MARKET_POTION_SHOP_ITEM_6;
                    shopItemIdentity.ogItemId = GI_BUGS;
                    break;
                case 6:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_7;
                    shopItemIdentity.randomizerCheck = RC_MARKET_POTION_SHOP_ITEM_7;
                    shopItemIdentity.ogItemId = GI_POE;
                    break;
                case 7:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_8;
                    shopItemIdentity.randomizerCheck = RC_MARKET_POTION_SHOP_ITEM_8;
                    shopItemIdentity.ogItemId = GI_FISH;
                    break;
            }
            break;
        case SCENE_NIGHT_SHOP:
            switch (slotIndex) {
                case 0:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_1;
                    shopItemIdentity.randomizerCheck = RC_MARKET_BOMBCHU_SHOP_ITEM_1;
                    shopItemIdentity.ogItemId = GI_BOMBCHUS_10;
                    break;
                case 1:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_2;
                    shopItemIdentity.randomizerCheck = RC_MARKET_BOMBCHU_SHOP_ITEM_2;
                    shopItemIdentity.ogItemId = GI_BOMBCHUS_10;
                    break;
                case 2:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_3;
                    shopItemIdentity.randomizerCheck = RC_MARKET_BOMBCHU_SHOP_ITEM_3;
                    shopItemIdentity.ogItemId = GI_BOMBCHUS_10;
                    break;
                case 3:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_4;
                    shopItemIdentity.randomizerCheck = RC_MARKET_BOMBCHU_SHOP_ITEM_4;
                    shopItemIdentity.ogItemId = GI_BOMBCHUS_10;
                    break;
                case 4:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_5;
                    shopItemIdentity.randomizerCheck = RC_MARKET_BOMBCHU_SHOP_ITEM_5;
                    shopItemIdentity.ogItemId = GI_BOMBCHUS_20;
                    break;
                case 5:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_6;
                    shopItemIdentity.randomizerCheck = RC_MARKET_BOMBCHU_SHOP_ITEM_6;
                    shopItemIdentity.ogItemId = GI_BOMBCHUS_20;
                    break;
                case 6:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_7;
                    shopItemIdentity.randomizerCheck = RC_MARKET_BOMBCHU_SHOP_ITEM_7;
                    shopItemIdentity.ogItemId = GI_BOMBCHUS_20;
                    break;
                case 7:
                    shopItemIdentity.randomizerInf = RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_8;
                    shopItemIdentity.randomizerCheck = RC_MARKET_BOMBCHU_SHOP_ITEM_8;
                    shopItemIdentity.ogItemId = GI_BOMBCHUS_20;
                    break;
            }
            break;
    }

    RandomizerGet randoGet = GetRandomizerGetFromKnownCheck(shopItemIdentity.randomizerCheck);
    if (randomizerGetToEnGirlShopItem.find(randoGet) != randomizerGetToEnGirlShopItem.end()) {
        shopItemIdentity.enGirlAShopItem = randomizerGetToEnGirlShopItem[randoGet];
    }

    if (merchantPrices.find(shopItemIdentity.randomizerCheck) != merchantPrices.end()) {
        shopItemIdentity.itemPrice = merchantPrices[shopItemIdentity.randomizerCheck];
    }

    return shopItemIdentity;
}

u8 Randomizer::GetRandoSettingValue(RandomizerSettingKey randoSettingKey) {
    return this->randoSettings[randoSettingKey];
}

GetItemID Randomizer::GetItemIdFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogItemId) {
    return GetItemIdFromRandomizerGet(this->itemLocations[randomizerCheck], ogItemId);
}

RandomizerCheck Randomizer::GetCheckFromActor(s16 actorId, s16 sceneNum, s16 actorParams) {
    if (!gSaveContext.n64ddFlag) {
        return RC_UNKNOWN_CHECK;
    }

    switch(sceneNum) {
        case 0:
            switch (actorParams) {
                case 2083:
                    return RC_DEKU_TREE_MAP_CHEST;
                case 22789:
                    return RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST;
                case 161:
                    return RC_DEKU_TREE_SLINGSHOT_CHEST;
                case 2050:
                    return RC_DEKU_TREE_COMPASS_CHEST;
                case 22790:
                    return RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST;
                case 22788:
                    return RC_DEKU_TREE_BASEMENT_CHEST;
                case 8200:
                    return RC_DEKU_TREE_GS_COMPASS_ROOM;
                case 8196:
                    return RC_DEKU_TREE_GS_BASEMENT_VINES;
                case 8194:
                    return RC_DEKU_TREE_GS_BASEMENT_GATE;
                case 8193:
                    return RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM;
            }
            break;
        case 1:
            switch (actorParams) {
                case 2088:
                    return RC_DODONGOS_CAVERN_MAP_CHEST;
                case 2053:
                    return RC_DODONGOS_CAVERN_COMPASS_CHEST;
                case 22982:
                    return RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST;
                case 1604:
                    return RC_DODONGOS_CAVERN_BOMB_BAG_CHEST;
                case 21802:
                    return RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST;
                case 4372:
                    return RC_DODONGOS_CAVERN_GOSSIP_STONE;
                case 8464:
                    return RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS;
                case 8450:
                    return RC_DODONGOS_CAVERN_GS_SCARECROW;
                case 8452:
                    return RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS;
                case 8449:
                    return RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS;
                case 8456:
                    return RC_DODONGOS_CAVERN_GS_BACK_ROOM;
            }
            break;
        case 2:
            switch (actorParams) {
                case 4289:
                    return RC_JABU_JABUS_BELLY_BOOMERANG_CHEST;
                case 6178:
                    return RC_JABU_JABUS_BELLY_MAP_CHEST;
                case -18428:
                    return RC_JABU_JABUS_BELLY_COMPASS_CHEST;
                case 8712:
                    return RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM;
                case 8705:
                    return RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER;
                case 8706:
                    return RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER;
                case 8708:
                    return RC_JABU_JABUS_BELLY_GS_NEAR_BOSS;
            }
            break;
        case 3:
            switch (actorParams) {
                case 22595:
                    return RC_FOREST_TEMPLE_FIRST_ROOM_CHEST;
                case 30784:
                    return RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST;
                case 22789:
                    return RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST;
                case 6177:
                    return RC_FOREST_TEMPLE_MAP_CHEST;
                case 22601:
                    return RC_FOREST_TEMPLE_WELL_CHEST;
                case -30364:
                    return RC_FOREST_TEMPLE_EYE_SWITCH_CHEST;
                case 10222:
                    return RC_FOREST_TEMPLE_BOSS_KEY_CHEST;
                case 30786:
                    return RC_FOREST_TEMPLE_FLOORMASTER_CHEST;
                case 30797:
                    return RC_FOREST_TEMPLE_RED_POE_CHEST;
                case -20340:
                    return RC_FOREST_TEMPLE_BOW_CHEST;
                case 6159:
                    return RC_FOREST_TEMPLE_BLUE_POE_CHEST;
                case 22855:
                    return RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST;
                case 22827:
                    return RC_FOREST_TEMPLE_BASEMENT_CHEST;
                case 8962:
                    return RC_FOREST_TEMPLE_GS_FIRST_ROOM;
                case 8968:
                    return RC_FOREST_TEMPLE_GS_LOBBY;
                case 8961:
                    return RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD;
                case 8964:
                    return RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD;
                case 8976:
                    return RC_FOREST_TEMPLE_GS_BASEMENT;
            }
            break;
        case 4:
            switch (actorParams) {
                case 22593:
                    return RC_FIRE_TEMPLE_NEAR_BOSS_CHEST;
                case 31936:
                    return RC_FIRE_TEMPLE_FLARE_DANCER_CHEST;
                case 10220:
                    return RC_FIRE_TEMPLE_BOSS_KEY_CHEST;
                case 22596:
                    return RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST;
                case 22594:
                    return RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST;
                case 22595:
                    return RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST;
                case 22600:
                    return RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST;
                case 2090:
                    return RC_FIRE_TEMPLE_MAP_CHEST;
                case 22603:
                    return RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST;
                case 22598:
                    return RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST;
                case 23245:
                    return RC_FIRE_TEMPLE_SCARECROW_CHEST;
                case 2055:
                    return RC_FIRE_TEMPLE_COMPASS_CHEST;
                case 421:
                    return RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST;
                case 22601:
                    return RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST;
                case 9218:
                    return RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP;
                case 9217:
                    return RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM;
                case 9220:
                    return RC_FIRE_TEMPLE_GS_BOULDER_MAZE;
                case 9232:
                    return RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB;
                case 9224:
                    return RC_FIRE_TEMPLE_GS_SCARECROW_TOP;
            }
            break;
        case 5:
            switch (actorParams) {
                case 2057:
                    return RC_WATER_TEMPLE_COMPASS_CHEST;
                case 6178:
                    return RC_WATER_TEMPLE_MAP_CHEST;
                case 22592:
                    return RC_WATER_TEMPLE_CRACKED_WALL_CHEST;
                case 30785:
                    return RC_WATER_TEMPLE_TORCHES_CHEST;
                case 10213:
                    return RC_WATER_TEMPLE_BOSS_KEY_CHEST;
                case 22598:
                    return RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST;
                case 22600:
                    return RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST;
                case 295:
                    return RC_WATER_TEMPLE_LONGSHOT_CHEST;
                case 22595:
                    return RC_WATER_TEMPLE_RIVER_CHEST;
                case 22602:
                    return RC_WATER_TEMPLE_DRAGON_CHEST;
                case 9473:
                    return RC_WATER_TEMPLE_GS_BEHIND_GATE;
                case 9480:
                    return RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST;
                case 9476:
                    return RC_WATER_TEMPLE_GS_CENTRAL_PILLAR;
                case 9474:
                    return RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM;
                case 9488:
                    return RC_WATER_TEMPLE_GS_RIVER;
            }
            break;
        case 6:
            switch (actorParams) {
                case 21800:
                    return RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST;
                case -30656:
                    return RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST;
                case -32666:
                    return RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST;
                case -31444:
                    return RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST;
                case -18397:
                    return RC_SPIRIT_TEMPLE_MAP_CHEST;
                case -30655:
                    return RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST;
                case 14340:
                    return RC_SPIRIT_TEMPLE_COMPASS_CHEST;
                case 22599:
                    return RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST;
                case -30451:
                    return RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST;
                case -28786:
                    return RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST;
                case -30577:
                    return RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST;
                case -30654:
                    return RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST;
                case 22597:
                    return RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST;
                case 26900:
                    return RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST;
                case 26901:
                    return RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST;
                case 10218:
                    return RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST;
                case -29454:
                    return RC_SPIRIT_TEMPLE_TOPMOST_CHEST;
                case 9744:
                    return RC_SPIRIT_TEMPLE_GS_METAL_FENCE;
                case 9736:
                    return RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM;
                case 9729:
                    return RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM;
                case 9732:
                    return RC_SPIRIT_TEMPLE_GS_LOBBY;
                case 9730:
                    return RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM;
            }
            break;
        case 7:
            switch (actorParams) {
                case 273:
                    return RC_SHADOW_TEMPLE_FREESTANDING_KEY;
                case 6177:
                    return RC_SHADOW_TEMPLE_MAP_CHEST;
                case 5607:
                    return RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST;
                case 6147:
                    return RC_SHADOW_TEMPLE_COMPASS_CHEST;
                case 22594:
                    return RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST;
                case 22668:
                    return RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST;
                case 26998:
                    return RC_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST;
                case 22853:
                    return RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST;
                case 22662:
                    return RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST;
                case -30652:
                    return RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST;
                case 30857:
                    return RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST;
                case 26965:
                    return RC_SHADOW_TEMPLE_WIND_HINT_CHEST;
                case 30856:
                    return RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST;
                case 26708:
                    return RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST;
                case 22666:
                    return RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST;
                case 10219:
                    return RC_SHADOW_TEMPLE_BOSS_KEY_CHEST;
                case 30797:
                    return RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST;
                case 9992:
                    return RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM;
                case 9986:
                    return RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM;
                case 9985:
                    return RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT;
                case 10000:
                    return RC_SHADOW_TEMPLE_GS_NEAR_SHIP;
                case 9988:
                    return RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT;
            }
            break;
        case 8:
            switch (actorParams) {
                case 273:
                    return RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY;
                case 22600:
                    return RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST;
                case 20578:
                    return RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST;
                case 23684:
                    return RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST;
                case 22793:
                    return RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST;
                case 2049:
                    return RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST;
                case 20558:
                    return RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST;
                case 22597:
                    return RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST;
                case 21802:
                    return RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST;
                case 21836:
                    return RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST;
                case 2087:
                    return RC_BOTTOM_OF_THE_WELL_MAP_CHEST;
                case 23760:
                    return RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST;
                case 4419:
                    return RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST;
                case 27348:
                    return RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST;
                case 10244:
                    return RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM;
                case 10242:
                    return RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM;
                case 10241:
                    return RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE;
            }
            break;
        case 9:
            switch (actorParams) {
                case 2080:
                    return RC_ICE_CAVERN_MAP_CHEST;
                case 2049:
                    return RC_ICE_CAVERN_COMPASS_CHEST;
                case 5570:
                    return RC_ICE_CAVERN_IRON_BOOTS_CHEST;
                case 262:
                    return RC_ICE_CAVERN_FREESTANDING_POH;
                case 10498:
                    return RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM;
                case 10500:
                    return RC_ICE_CAVERN_GS_HEART_PIECE_ROOM;
                case 10497:
                    return RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM;
            }
            break;
        case 10:
            switch (actorParams) {
                case 10219:
                    return RC_GANONS_TOWER_BOSS_KEY_CHEST;
            }
            break;
        case 11:
            switch (actorParams) {
                case 273:
                    return RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY;
                case -30573:
                    return RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST;
                case -30393:
                    return RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST;
                case -30656:
                    return RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST;
                case 31089:
                    return RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST;
                case 31439:
                    return RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST;
                case 30862:
                    return RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST;
                case 26708:
                    return RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST;
                case 24450:
                    return RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST;
                case -30653:
                    return RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST;
                case 22596:
                    return RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST;
                case 31058:
                    return RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST;
                case 22608:
                    return RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST;
                case 23877:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST;
                case 22888:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST;
                case -30643:
                    return RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST;
                case -30655:
                    return RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST;
                case 22603:
                    return RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST;
                case 23206:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST;
                case 22986:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST;
                case 22889:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST;
                case 2860:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST;
            }
            break;
        case 12:
            switch(actorParams) {
                case 3089:
                    return RC_GF_NORTH_F1_CARPENTER;
                case 3857:
                    return RC_GF_SOUTH_F2_CARPENTER;
                case 3601:
                    return RC_GF_SOUTH_F1_CARPENTER;
                case 2577:
                    return RC_GF_NORTH_F2_CARPENTER;
            }
            break;
        case 13:
            switch (actorParams) {
                case 30857:
                    return RC_GANONS_CASTLE_FOREST_TRIAL_CHEST;
                case 24455:
                    return RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST;
                case 22790:
                    return RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST;
                case 22664:
                    return RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST;
                case 14021:
                    return RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST;
                case 22668:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST;
                case 24459:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST;
                case 22797:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST;
                case 24462:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST;
                case 22890:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST;
                case 24463:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST;
                case 30800:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST;
                case -30639:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST;
                case -29326:
                    return RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST;
                case 26964:
                    return RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST;
            }
            break;
        case 16:
            if(actorParams == 20170) return RC_MARKET_TREASURE_CHEST_GAME_REWARD;

            // RANDOTODO update logic to match 3ds rando when we implement keysanity
            // keep keys og
            if ((actorParams & 0x60) == 0x20) break;

            if (GetRandoSettingValue(RSK_SHUFFLE_CHEST_MINIGAME)) {
                if((actorParams & 0xF) < 2) return RC_MARKET_TREASURE_CHEST_GAME_ITEM_1;
                if((actorParams & 0xF) < 4) return RC_MARKET_TREASURE_CHEST_GAME_ITEM_2;
                if((actorParams & 0xF) < 6) return RC_MARKET_TREASURE_CHEST_GAME_ITEM_3;
                if((actorParams & 0xF) < 8) return RC_MARKET_TREASURE_CHEST_GAME_ITEM_4;
                if((actorParams & 0xF) < 10) return RC_MARKET_TREASURE_CHEST_GAME_ITEM_5;
            }
            break;
        case 17:
            switch (actorId) {
                case 95:
                    return RC_DEKU_TREE_QUEEN_GOHMA_HEART;
                case 93:
                    return RC_QUEEN_GOHMA;
            }
            break;
        case 18:
            switch (actorId) {
                case 10:
                    return RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST;
                case 95:
                    return RC_DODONGOS_CAVERN_KING_DODONGO_HEART;
                case 93:
                    return RC_KING_DODONGO;
            }
            break;
        case 19:
            switch (actorId) {
                case 95:
                    return RC_JABU_JABUS_BELLY_BARINADE_HEART;
                case 93:
                    return RC_BARINADE;
            }
            break;
        case 20:
            switch (actorId) {
                case 95:
                    return RC_FOREST_TEMPLE_PHANTOM_GANON_HEART;
                case 93:
                    return RC_PHANTOM_GANON;
            }
            break;
        case 21:
            switch (actorId) {
                case 95:
                    return RC_FIRE_TEMPLE_VOLVAGIA_HEART;
                case 93:
                    return RC_VOLVAGIA;
            }
            break;
        case 22:
            switch (actorId) {
                case 95:
                    return RC_WATER_TEMPLE_MORPHA_HEART;
                case 93:
                    return RC_MORPHA;
            }
            break;
        case 23:
            switch (actorId) {
                case 95:
                    return RC_SPIRIT_TEMPLE_TWINROVA_HEART;
                case 93:
                    return RC_TWINROVA;
            }
            break;
        case 24:
            switch (actorId) {
                case 95:
                    return RC_SHADOW_TEMPLE_BONGO_BONGO_HEART;
                case 93:
                    return RC_BONGO_BONGO;
            }
            break;
        case 35:
        case 36:
        case 37:
            switch (actorParams) {
                case 14342:
                    return RC_TOT_LEFT_GOSSIP_STONE;
                case 14599:
                    return RC_TOT_LEFT_CENTER_GOSSIP_STONE;
                case 14862:
                    return RC_TOT_RIGHT_CENTER_GOSSIP_STONE;
                case 15120:
                    return RC_TOT_RIGHT_GOSSIP_STONE;
            }
            break;
        case 40:
            switch (actorParams) {
                case 22944:
                    return RC_KF_MIDOS_TOP_LEFT_CHEST;
                case 22945:
                    return RC_KF_MIDOS_TOP_RIGHT_CHEST;
                case 22914:
                    return RC_KF_MIDOS_BOTTOM_LEFT_CHEST;
                case 22787:
                    return RC_KF_MIDOS_BOTTOM_RIGHT_CHEST;
            }
            break;
        case 55:
            switch (actorParams) {
                case 262:
                    return RC_KAK_IMPAS_HOUSE_FREESTANDING_POH;
            }
            break;
        case 56:
            switch (actorParams) {
                case -28152:
                    return RC_LH_GS_LAB_CRATE;
            }
            break;
        case 59:
            // using fountainType as actorParams
            switch (actorParams) {
                case 1:
                    return RC_DMT_GREAT_FAIRY_REWARD;
                case 2:
                    return RC_DMC_GREAT_FAIRY_REWARD;
                case 3:
                    return RC_OGC_GREAT_FAIRY_REWARD;
            }
            break;
        case 61:
            // using fountainType as actorParams
            switch (actorParams) {
                case 1:
                    return RC_ZF_GREAT_FAIRY_REWARD;
                case 2:
                    return RC_HC_GREAT_FAIRY_REWARD;
                case 3:
                    return RC_COLOSSUS_GREAT_FAIRY_REWARD;
            }
            break;
        case 62:
            switch (actorParams) {
                case 22988:
                    return RC_KF_STORMS_GROTTO_CHEST;
                case -22988:
                    return RC_KF_STORMS_GOSSIP_STONE;
                case 22964:
                    return RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST;
                case -22964:
                    return RC_LW_NEAR_SHORTCUTS_GOSSIP_STONE;
                case 31409:
                    return RC_SFM_WOLFOS_GROTTO_CHEST;
                case 22944:
                    return RC_HF_NEAR_MARKET_GROTTO_CHEST;
                case -22944:
                    return RC_HF_NEAR_MARKET_GOSSIP_STONE;
                case 22978:
                    return RC_HF_SOUTHEAST_GROTTO_CHEST;
                case -22978:
                    return RC_HF_SOUTHEAST_GOSSIP_STONE;
                case 22947:
                    return RC_HF_OPEN_GROTTO_CHEST;
                case -22947:
                    return RC_HF_OPEN_GROTTO_GOSSIP_STONE;
                case 22984:
                    return RC_KAK_OPEN_GROTTO_CHEST;
                case -22984:
                    return RC_KAK_OPEN_GROTTO_GOSSIP_STONE;
                case 31434:
                    return RC_KAK_REDEAD_GROTTO_CHEST;
                case 23255:
                    return RC_DMT_STORMS_GROTTO_CHEST;
                case -23255:
                    return RC_DMT_STORMS_GROTTO_GOSSIP_STONE;
                case 23802:
                    return RC_DMC_UPPER_GROTTO_CHEST;
                case -23802:
                    return RC_DMC_UPPER_GROTTO_GOSSIP_STONE;
                case 22985:
                    return RC_ZR_OPEN_GROTTO_CHEST;
                case -22985:
                    return RC_ZR_OPEN_GROTTO_GOSSIP_STONE;
                case 262:
                    return RC_HF_TEKTITE_GROTTO_FREESTANDING_POH;
                case 14363:
                    return RC_HF_COW_GROTTO_GOSSIP_STONE;
                case 14355:
                    return RC_HC_STORMS_GROTTO_GOSSIP_STONE;
                case 10753:
                    return RC_HF_GS_COW_GROTTO;
                case 10754:
                    return RC_HF_GS_NEAR_KAK_GROTTO;
                case 11778:
                    return RC_HC_GS_STORMS_GROTTO;
                default:
                    return RC_UNKNOWN_CHECK;
            }
            break;
        case 63:
            switch (actorParams) {
                case -22592:
                    return RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST;
            }
            break;
        case 64:
            switch (actorParams) {
                case 21824:
                    return RC_GRAVEYARD_SHIELD_GRAVE_CHEST;
            }
            break;
        case 65:
            switch (actorParams) {
                case -32736:
                    return RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST;
            }
            break;
        case 72:
            switch (actorParams) {
                case 4352:
                    return RC_GRAVEYARD_HOOKSHOT_CHEST;
                case 262:
                    return RC_KAK_WINDMILL_FREESTANDING_POH;
                case 1798:
                    return RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH;
            }
            break;
        case 76:
            switch (actorParams) {
                case 262:
                    return RC_LLR_FREESTANDING_POH;
            }
            break;
        case 77:
            switch (actorParams) {
                case -29176:
                    return RC_MARKET_GS_GUARD_HOUSE;
            }
            break;
        case 82:
            switch (actorId) {
                case 316:
                    if (LINK_IS_ADULT) {
                        return RC_KAK_ANJU_AS_ADULT;
                    } else {
                        return RC_KAK_ANJU_AS_CHILD;
                    }
            }
            switch (actorParams) {
                case -28640:
                    return RC_KAK_GS_TREE;
                case 20482:
                    return RC_KAK_GS_GUARDS_HOUSE;
                case 20484:
                    return RC_KAK_GS_WATCHTOWER;
                case 20496:
                    return RC_KAK_GS_SKULLTULA_HOUSE;
                case 20488:
                    return RC_KAK_GS_HOUSE_UNDER_CONSTRUCTION;
                case 20544:
                    return RC_KAK_GS_ABOVE_IMPAS_HOUSE;
            }
            break;
        case 83:
            switch (actorParams) {
                case 1030:
                    return RC_GRAVEYARD_FREESTANDING_POH;
                case 7942:
                    return RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR;
                case 14346:
                    return RC_GY_GOSSIP_STONE;
                case 28673:
                    return RC_GRAVEYARD_GS_BEAN_PATCH;
                case 20608:
                    return RC_GRAVEYARD_GS_WALL;
            }
            break;
        case 84:
            switch (actorParams) {
                case 1030:
                    return RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH;
                case 2822:
                    return RC_ZR_NEAR_DOMAIN_FREESTANDING_POH;
                case 14605:
                    return RC_ZR_NEAR_GROTTOS_GOSSIP_STONE;
                case 14860:
                    return RC_ZR_NEAR_DOMAIN_GOSSIP_STONE;
                case -28414:
                    return RC_ZR_GS_TREE;
                case 20737:
                    return RC_ZR_GS_LADDER;
                case 20752:
                    return RC_ZR_GS_NEAR_RAISED_GROTTOS;
                case 20744:
                    return RC_ZR_GS_ABOVE_BRIDGE;
            }
            break;
        case 85:
            switch (actorParams) {
                case 1248:
                    return RC_KF_KOKIRI_SWORD_CHEST;
                case 14366:
                    return RC_KF_GOSSIP_STONE;
                case 14880:
                    return RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE;
                case 14623:
                    return RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE;
                case 19458:
                    return RC_KF_GS_KNOW_IT_ALL_HOUSE;
                case 27649:
                    return RC_KF_GS_BEAN_PATCH;
                case 19460:
                    return RC_KF_GS_HOUSE_OF_TWINS;
            }
            break;
        case 86:
            switch (actorParams) {
                case 14358:
                    return RC_SFM_MAZE_LOWER_GOSSIP_STONE;
                case 14615:
                    return RC_SFM_MAZE_UPPER_GOSSIP_STONE;
                case 14876:
                    return RC_SFM_SARIA_GOSSIP_STONE;
                case 19720:
                    return RC_SFM_GS;
            }
            break;
        case 87:
            switch (actorId) {
                case 271:
                    if (LINK_IS_ADULT) {
                        return RC_LH_SUN;
                    } else {
                        return RC_LH_UNDERWATER_ITEM;
                    }
            }
            switch(actorParams) {
                case 7686:
                    return RC_LH_FREESTANDING_POH;
                case 14339:
                    return RC_LH_LAB_GOSSIP_STONE;
                case 14600:
                    return RC_LH_SOUTHWEST_GOSSIP_STONE;
                case 14863:
                    return RC_LH_SOUTHEAST_GOSSIP_STONE;
                case 29185:
                    return RC_LH_GS_BEAN_PATCH;
                case 20996:
                    return RC_LH_GS_LAB_WALL;
                case 20994:
                    return RC_LH_GS_SMALL_ISLAND;
                case 21008:
                    return RC_LH_GS_TREE;
            }
            break;
        case 88:
            switch (actorParams) {
                case -18496:
                    return RC_ZD_CHEST;
                case 14345:
                    return RC_ZD_GOSSIP_STONE;
                case 20800:
                    return RC_ZD_GS_FROZEN_WATERFALL;
            }
            break;
        case 89:
            switch (actorParams) {
                case 5126:
                    return RC_ZF_BOTTOM_FREESTANDING_POH;
                case 262:
                    return RC_ZF_ICEBERC_FREESTANDING_POH;
                case 15362:
                case 14594:
                    return RC_JABU_GOSSIP_STONE;
                case 14849:
                case 14337:
                    return RC_FAIRY_GOSSIP_STONE;
                case 20740:
                    return RC_ZF_GS_ABOVE_THE_LOG;
                case -28288:
                    return RC_ZF_GS_TREE;
                case 20768:
                    return RC_ZF_GS_HIDDEN_CAVE;
            }
            break;
        case 90:
            switch (actorParams) {
                case 23200:
                    return RC_GV_CHEST;
                case 262:
                    return RC_GV_WATERFALL_FREESTANDING_POH;
                case 518:
                    return RC_GV_CRATE_FREESTANDING_POH;
                case 14353:
                    return RC_GV_GOSSIP_STONE;
                case 21250:
                    return RC_GV_GS_SMALL_BRIDGE;
                case 29441:
                    return RC_GV_GS_BEAN_PATCH;
                case 21256:
                    return RC_GV_GS_BEHIND_TENT;
                case 21252:
                    return RC_GV_GS_PILLAR;
            }
            break;
        case 91:
            switch (actorParams) {
                case 14365:
                    return RC_LW_GOSSIP_STONE;
                case 27905:
                    return RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE;
                case 27906:
                    return RC_LW_GS_BEAN_PATCH_NEAR_THEATER;
                case 19716:
                    return RC_LW_GS_ABOVE_THEATER;
            }
            break;
        case 92:
            switch (actorParams) {
                case 1707:
                    return RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST;
                case 13673:
                    return RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST;
                case 3334:
                    return RC_COLOSSUS_FREESTANDING_POH;
                case 14362:
                    return RC_COLOSSUS_GOSSIP_STONE;
                case 29953:
                    return RC_COLOSSUS_GS_BEAN_PATCH;
                case 21768:
                    return RC_COLOSSUS_GS_TREE;
                case 21764:
                    return RC_COLOSSUS_GS_HILL;
            }
            break;
        case 93:
            switch (actorParams) {
                case 1984:
                    return RC_GF_CHEST;
                case 21506:
                    return RC_GF_GS_TOP_FLOOR;
                case 21505:
                    return RC_GF_GS_ARCHERY_RANGE;
            }
            break;
        case 94:
            switch (actorParams) {
                case -30048:
                    return RC_WASTELAND_CHEST;
                case 13570:
                    return RC_WASTELAND_GS;
            }
            break;
        case 95:
            switch (actorParams) {
                case 14610:
                    return RC_HC_MALON_GOSSIP_STONE;
                case 14347:
                    return RC_HC_ROCK_WALL_GOSSIP_STONE;
                case -29180:
                    return RC_HC_GS_TREE;
            }
            break;
        case 96:
            switch (actorParams) {
                case 7686:
                    return RC_DMT_FREESTANDING_POH;
                case 23201:
                    return RC_DMT_CHEST;
                case 14340:
                    return RC_DMT_GOSSIP_STONE;
                case 12036:
                    return RC_DMT_GS_NEAR_KAK;
                case 28418:
                    return RC_DMT_GS_BEAN_PATCH;
                case 20232:
                    return RC_DMT_GS_ABOVE_DODONGOS_CAVERN;
                case 20240:
                    return RC_DMT_GS_FALLING_ROCKS_PATH;
            }
            break;
        case 97:
            switch (actorParams) {
                case 2054:
                    return RC_DMC_VOLCANO_FREESTANDING_POH;
                case 518:
                    return RC_DMC_WALL_FREESTANDING_POH;
                case 14341:
                    return RC_DMC_GOSSIP_STONE;
                case -28800:
                    return RC_DMC_GS_CRATE;
                case 28417:
                    return RC_DMC_GS_BEAN_PATCH;
            }
            break;
        case 98:
            switch (actorParams) {
                case 23232:
                    return RC_GC_MAZE_LEFT_CHEST;
                case 23201:
                    return RC_GC_MAZE_RIGHT_CHEST;
                case 23202:
                    return RC_GC_MAZE_CENTER_CHEST;
                case 7942:
                    return RC_GC_POT_FREESTANDING_POH;
                case 14357:
                    return RC_GC_MAZE_GOSSIP_STONE;
                case 14873:
                    return RC_GC_MEDIGORON_GOSSIP_STONE;
                case 12064:
                    return RC_GC_GS_CENTER_PLATFORM;
                case -28864:
                    return RC_GC_GS_BOULDER_MAZE;
            }
            break;
        case 99:
            switch (actorParams) {
                case 11012:
                    return RC_LLR_GS_HOUSE_WINDOW;
                case -29944:
                    return RC_LLR_GS_TREE;
                case 11010:
                    return RC_LLR_GS_RAIN_SHED;
                case 11009:
                    return RC_LLR_GS_BACK_WALL;
            }
            break;
        case 100:
            switch (actorParams) {
                case 11777:
                    return RC_OGC_GS;
            }
            break;
    }

    return RC_UNKNOWN_CHECK;
}

std::thread randoThread;

void GenerateRandomizerImgui() {
    CVar_SetS32("gRandoGenerating", 1);
    CVar_Save();

    std::unordered_map<RandomizerSettingKey, u8> cvarSettings;
    cvarSettings[RSK_LOGIC_RULES] = CVar_GetS32("gRandomizeLogicRules", 0);
    cvarSettings[RSK_FOREST] = CVar_GetS32("gRandomizeForest", 0);
    cvarSettings[RSK_KAK_GATE] = CVar_GetS32("gRandomizeKakarikoGate", 0);
    cvarSettings[RSK_DOOR_OF_TIME] = CVar_GetS32("gRandomizeDoorOfTime", 0);
    cvarSettings[RSK_ZORAS_FOUNTAIN] = CVar_GetS32("gRandomizeZorasFountain", 0);
    cvarSettings[RSK_GERUDO_FORTRESS] = CVar_GetS32("gRandomizeGerudoFortress", 0);
    cvarSettings[RSK_RAINBOW_BRIDGE] = CVar_GetS32("gRandomizeRainbowBridge", 0);
    cvarSettings[RSK_RAINBOW_BRIDGE_STONE_COUNT] = CVar_GetS32("gRandomizeStoneCount", 3);
    cvarSettings[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT] = CVar_GetS32("gRandomizeMedallionCount", 6);
    cvarSettings[RSK_RAINBOW_BRIDGE_REWARD_COUNT] = CVar_GetS32("gRandomizeRewardCount", 9);
    cvarSettings[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT] = CVar_GetS32("gRandomizeDungeonCount", 8);
    cvarSettings[RSK_RAINBOW_BRIDGE_TOKEN_COUNT] = CVar_GetS32("gRandomizeTokenCount", 100);
    cvarSettings[RSK_RANDOM_TRIALS] = CVar_GetS32("gRandomizeGanonTrial", 1);
    cvarSettings[RSK_TRIAL_COUNT] = CVar_GetS32("gRandomizeGanonTrialCount", 6);
    cvarSettings[RSK_STARTING_OCARINA] = CVar_GetS32("gRandomizeStartingOcarina", 0);
    cvarSettings[RSK_SHUFFLE_OCARINA] = CVar_GetS32("gRandomizeShuffleOcarinas", 0) ||
                                        CVar_GetS32("gRandomizeStartingOcarina", 0);
    cvarSettings[RSK_STARTING_KOKIRI_SWORD] = CVar_GetS32("gRandomizeStartingKokiriSword", 0);
    cvarSettings[RSK_SHUFFLE_KOKIRI_SWORD] = CVar_GetS32("gRandomizeShuffleKokiriSword", 0) ||
                                             CVar_GetS32("gRandomizeStartingKokiriSword", 0);
    cvarSettings[RSK_STARTING_DEKU_SHIELD] = CVar_GetS32("gRandomizeStartingDekuShield", 0);
    cvarSettings[RSK_STARTING_MAPS_COMPASSES] = CVar_GetS32("gRandomizeStartingMapsCompasses", 2);
    cvarSettings[RSK_SHUFFLE_DUNGEON_REWARDS] = CVar_GetS32("gRandomizeShuffleDungeonReward", 0);
    cvarSettings[RSK_SHUFFLE_SONGS] = CVar_GetS32("gRandomizeShuffleSongs", 0);
    cvarSettings[RSK_SHUFFLE_TOKENS] = CVar_GetS32("gRandomizeShuffleTokens", 0);
    cvarSettings[RSK_SHOPSANITY] = CVar_GetS32("gRandomizeShopsanity", 0);
    cvarSettings[RSK_SHUFFLE_SCRUBS] = CVar_GetS32("gRandomizeShuffleScrubs", 0);
    cvarSettings[RSK_SHUFFLE_COWS] = CVar_GetS32("gRandomizeShuffleCows", 0);
    cvarSettings[RSK_SHUFFLE_ADULT_TRADE] = CVar_GetS32("gRandomizeShuffleAdultTrade", 0);
    cvarSettings[RSK_SHUFFLE_MAGIC_BEANS] = CVar_GetS32("gRandomizeShuffleBeans", 0);
    cvarSettings[RSK_ENABLE_BOMBCHU_DROPS] = CVar_GetS32("gRandomizeEnableBombchuDrops", 0);
    cvarSettings[RSK_BOMBCHUS_IN_LOGIC] = CVar_GetS32("gRandomizeBombchusInLogic", 0);
    cvarSettings[RSK_SKIP_CHILD_ZELDA] = CVar_GetS32("gRandomizeSkipChildZelda", 0);

    // if we skip child zelda, we start with zelda's letter, and malon starts
    // at the ranch, so we should *not* shuffle the weird egg
    cvarSettings[RSK_SHUFFLE_WEIRD_EGG] = ((CVar_GetS32("gRandomizeSkipChildZelda", 0) == 0) &&
                                            CVar_GetS32("gRandomizeShuffleWeirdEgg", 0));
    
    cvarSettings[RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD] = CVar_GetS32("gRandomizeShuffleGerudoToken", 0);
    cvarSettings[RSK_SHUFFLE_FROG_SONG_RUPEES] = CVar_GetS32("gRandomizeShuffleFrogSongRupees", 0);
    cvarSettings[RSK_ITEM_POOL] = CVar_GetS32("gRandomizeItemPool", 1);
    cvarSettings[RSK_ICE_TRAPS] = CVar_GetS32("gRandomizeIceTraps", 1);
    cvarSettings[RSK_GOSSIP_STONE_HINTS] = CVar_GetS32("gRandomizeGossipStoneHints", 1);
    cvarSettings[RSK_HINT_CLARITY] = CVar_GetS32("gRandomizeHintClarity", 2);
    cvarSettings[RSK_HINT_DISTRIBUTION] = CVar_GetS32("gRandomizeHintDistribution", 1);
    cvarSettings[RSK_BLUE_FIRE_ARROWS] = CVar_GetS32("gRandomizeBlueFireArrows", 0);
    cvarSettings[RSK_SUNLIGHT_ARROWS] = CVar_GetS32("gRandomizeSunlightArrows", 0);
    cvarSettings[RSK_KEYSANITY] = CVar_GetS32("gRandomizeKeysanity", 2);
    cvarSettings[RSK_GERUDO_KEYS] = CVar_GetS32("gRandomizeGerudoKeys", 0);
    cvarSettings[RSK_BOSS_KEYSANITY] = CVar_GetS32("gRandomizeBossKeysanity", 2);
    cvarSettings[RSK_GANONS_BOSS_KEY] = CVar_GetS32("gRandomizeShuffleGanonBossKey", 1);
    cvarSettings[RSK_STARTING_CONSUMABLES] = CVar_GetS32("gRandomizeStartingConsumables", 0);
    cvarSettings[RSK_FULL_WALLETS] = CVar_GetS32("gRandomizeFullWallets", 0);
    
    // RANDOTODO implement chest minigame shuffle with keysanity
    cvarSettings[RSK_SHUFFLE_CHEST_MINIGAME] = false;

    cvarSettings[RSK_LANGUAGE] = CVar_GetS32("gLanguages", 0);

    cvarSettings[RSK_CUCCO_COUNT] = CVar_GetS32("gRandomizeCuccosToReturn", 7);
    cvarSettings[RSK_BIG_POE_COUNT] = CVar_GetS32("gRandomizeBigPoeTargetCount", 10);

    // If we skip child zelda, skip child stealth is pointless, so this needs to be reflected in the spoiler log
    cvarSettings[RSK_SKIP_CHILD_STEALTH] =
        !CVar_GetS32("gRandomizeSkipChildZelda", 0) && CVar_GetS32("gRandomizeSkipChildStealth", 0);

    cvarSettings[RSK_SKIP_EPONA_RACE] = CVar_GetS32("gRandomizeSkipEponaRace", 0);
    cvarSettings[RSK_SKIP_TOWER_ESCAPE] = CVar_GetS32("gRandomizeSkipTowerEscape", 0);
    cvarSettings[RSK_COMPLETE_MASK_QUEST] = CVar_GetS32("gRandomizeCompleteMaskQuest", 0);
    cvarSettings[RSK_SKIP_SCARECROWS_SONG] = CVar_GetS32("gRandomizeSkipScarecrowsSong", 0);
    cvarSettings[RSK_ENABLE_GLITCH_CUTSCENES] = CVar_GetS32("gRandomizeEnableGlitchCutscenes", 0);

    cvarSettings[RSK_SKULLS_SUNS_SONG] = CVar_GetS32("gRandomizeGsExpectSunsSong", 0);
    // Link's Pocket has to have a dungeon reward if the other rewards are shuffled to end of dungeon.
    cvarSettings[RSK_LINKS_POCKET] = CVar_GetS32("gRandomizeShuffleDungeonReward", 0) != 0 ? 
                                        CVar_GetS32("gRandomizeLinksPocket", 0) : 0;

    // todo: this efficently when we build out cvar array support
    std::set<RandomizerCheck> excludedLocations;
    std::stringstream excludedLocationStringStream(CVar_GetString("gRandomizeExcludedLocations", ""));
    std::string excludedLocationString;
    while(getline(excludedLocationStringStream, excludedLocationString, ',')) {
        excludedLocations.insert((RandomizerCheck)std::stoi(excludedLocationString));
    }

    RandoMain::GenerateRando(cvarSettings, excludedLocations);

    CVar_SetS32("gRandoGenerating", 0);
    CVar_Save();
    CVar_Load();

    generated = 1;
}

void DrawRandoEditor(bool& open) {
    if (generated) {
        generated = 0;
        randoThread.join();
    }

    if (!open) {
        CVar_SetS32("gRandomizerSettingsEnabled", 0);
        return;
    }

    // Randomizer settings
    // Logic Settings
    const char* randoLogicRules[2] = { "Glitchless", "No logic" };

    // Open Settings
    const char* randoForest[3] = { "Closed", "Closed Deku", "Open" };
    const char* randoKakarikoGate[2] = { "Closed", "Open" };
    const char* randoDoorOfTime[3] = { "Closed", "Song only", "Open" };
    const char* randoZorasFountain[3] = { "Closed", "Closed as child", "Open" };
    const char* randoGerudoFortress[3] = { "Normal", "Fast", "Open" };
    const char* randoRainbowBridge[7] = { "Vanilla",         "Always open", "Stones", "Medallions",
                                          "Dungeon rewards", "Dungeons",    "Tokens" };
    const char* randoGanonsTrial[3] = { "Skip", "Set Number", "Random Number" };

    // World Settings
    const char* randoStartingAge[3] = { "Child", "Adult", "Random" };
    const char* randoShuffleEntrances[2] = { "Off", "On" };
    const char* randoShuffleDungeonsEntrances[2] = { "Off", "On" };
    const char* randoShuffleOverworldEntrances[2] = { "Off", "On" };
    const char* randoShuffleInteriorsEntrances[2] = { "Off", "On" };
    const char* randoShuffleGrottosEntrances[2] = { "Off", "On" };
    const char* randoBombchusInLogic[2] = { "Off", "On" };
    const char* randoAmmoDrops[3] = { "On + Bombchu", "Off", "On" };
    const char* randoHeartDropsAndRefills[4] = { "On", "No Drop", "No Refill", "Off" };

    // Shuffle Settings
    const char* randoShuffleDungeonRewards[4] = { "End of dungeons", "Any dungeon", "Overworld", "Anywhere" };
    const char* randoLinksPocket[4] = { "Dungeon Reward", "Advancement", "Anything", "Nothing" };
    const char* randoShuffleSongs[3] = { "Song Locations", "Dungeon Rewards", "Anywhere" };
    const char* randoShopsanity[7] = { "Off", "0 Items", "1 Item", "2 Items", "3 Items", "4 Items", "Random" };
    const char* randoTokensanity[4] = { "Off", "Dungeons", "Overworld", "All Tokens" };
    const char* randoShuffleScrubs[4] = { "Off", "Affordable", "Expensive", "Random Prices" };
    const char* randoShuffleMerchants[3] = { "Off", "On (no hints)", "On (with hints)" };

    // Shuffle Dungeon Items Settings
    const char* randoShuffleMapsAndCompasses[6] = { "Start With",  "Vanilla",   "Own Dungeon",
                                                    "Any Dungeon", "Overworld", "Anywhere" };
    const char* randoShuffleSmallKeys[6] = { "Start With",  "Vanilla",   "Own Dungeon",
                                             "Any Dungeon", "Overworld", "Anywhere" };
    const char* randoShuffleGerudoFortressKeys[4] = { "Vanilla", "Any Dungeon", "Overworld", "Anywhere" };
    const char* randoShuffleBossKeys[6] = { "Start With",  "Vanilla",   "Own Dungeon",
                                            "Any Dungeon", "Overworld", "Anywhere" };
    const char* randoShuffleGanonsBossKey[6] = { "Vanilla",     "Own dungeon", "Start with",
                                                 "Any Dungeon", "Overworld",   "Anywhere" };

    // Misc Settings
    const char* randoGossipStoneHints[4] = { "No Hints", "Need Nothing", "Mask of Truth", "Stone of Agony" };
    const char* randoHintClarity[3] = { "Obscure", "Ambiguous", "Clear" };
    const char* randoHintDistribution[4] = { "Useless", "Balanced", "Strong", "Very Strong" };
    const char* randoStartingTime[2] = { "Day", "Night" };
    const char* randoChestSizeAndColor[2] = { "Vanilla", "Match contents" };
    const char* randoRandomTrapDamage[3] = { "Basic", "Advanced", "Off" };

    // Item Pool Settings
    const char* randoItemPool[4] = { "Plentiful", "Balanced", "Scarce", "Minimal" };
    const char* randoIceTraps[5] = { "Off", "Normal", "Extra", "Mayhem", "Onslaught" };

    ImGui::SetNextWindowSize(ImVec2(920, 563), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Randomizer Editor", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    bool disableEditingRandoSettings = CVar_GetS32("gRandoGenerating", 0) || CVar_GetS32("gOnFileSelectNameEntry", 0);
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, disableEditingRandoSettings);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (disableEditingRandoSettings ? 0.5f : 1.0f));
    UIWidgets::EnhancementCheckbox("Enable Randomizer", "gRandomizer");

    if (CVar_GetS32("gRandomizer", 0)) {
        ImGui::Dummy(ImVec2(0.0f, 0.0f));
        if (ImGui::Button("Generate Seed")) {
            if (CVar_GetS32("gRandoGenerating", 0) == 0) {
                randoThread = std::thread(&GenerateRandomizerImgui);
            }
        }
        ImGui::Dummy(ImVec2(0.0f, 0.0f));
        std::string spoilerfilepath = CVar_GetString("gSpoilerLog", "");
        ImGui::Text("Spoiler File: %s", spoilerfilepath.c_str());

        // RANDOTODO settings presets
        // std::string presetfilepath = CVar_GetString("gLoadedPreset", "");
        // ImGui::Text("Settings File: %s", presetfilepath.c_str());
    }

    UIWidgets::PaddedSeparator();

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    static ImVec2 cellPadding(8.0f, 8.0f);

    if (CVar_GetS32("gRandomizer", 0) &&
        ImGui::BeginTabBar("Randomizer Settings", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("World")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (ImGui::BeginTable("tableRandoWorld", 3, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Area Access", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("World Settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Shuffle Entrances", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::TableHeadersRow();
                ImGui::PopItemFlag();
                ImGui::TableNextRow();

                // COLUMN 1 - Area Access
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildOpenSettings", ImVec2(0, -8));
                ImGui::PushItemWidth(-FLT_MIN);

                // Forest
                ImGui::Text(Settings::OpenForest.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Closed - Kokiri sword & shield are required to access "
                    "the Deku Tree, and completing the Deku Tree is required to "
                    "access the Hyrule Field exit.\n"
                    "\n"
                    "Closed Deku - Kokiri boy no longer blocks the path to Hyrule "
                    "Field but Mido still requires the Kokiri sword and Deku shield "
                    "to access the tree.\n"
                    "\n"
                    "Open - Mido no longer blocks the path to the Deku Tree. Kokiri "
                    "boy no longer blocks the path out of the forest."
                );
                UIWidgets::EnhancementCombobox("gRandomizeForest", randoForest, 3, 0);

                UIWidgets::PaddedSeparator();

                // Kakariko Gate
                ImGui::Text(Settings::OpenKakariko.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Closed - The gate will remain closed until Zelda's letter "
                    "is shown to the guard.\n"
                    "\n"
                    "Open - The gate is always open. The happy mask shop "
                    "will open immediately after obtaining Zelda's letter."
                );
                UIWidgets::EnhancementCombobox("gRandomizeKakarikoGate", randoKakarikoGate, 2, 0);

                UIWidgets::PaddedSeparator();

                // Door of Time
                ImGui::Text(Settings::OpenDoorOfTime.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Closed - The Ocarina of Time, the Song of Time and all "
                    "three spiritual stones are required to open the Door of Time.\n"
                    "\n"
                    "Song only - Play the Song of Time in front of the Door of "
                    "Time to open it.\n"
                    "\n"
                    "Open - The Door of Time is permanently open with no requirements."
                );
                UIWidgets::EnhancementCombobox("gRandomizeDoorOfTime", randoDoorOfTime, 3, 0);

                UIWidgets::PaddedSeparator();

                // Zora's Fountain
                ImGui::Text(Settings::ZorasFountain.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Closed - King Zora obstructs the way to Zora's Fountain. "
                    "Ruto's letter must be shown as child Link in order to move "
                    "him in both time periods.\n"
                    "\n"
                    "Closed as child - Ruto's Letter is only required to move King Zora "
                    "as child Link. Zora's Fountain starts open as adult.\n"
                    "\n"
                    "Open - King Zora has already mweeped out of the way in both "
                    "time periods. Ruto's Letter is removed from the item pool."
                );
                UIWidgets::EnhancementCombobox("gRandomizeZorasFountain", randoZorasFountain, 3, 0);

                UIWidgets::PaddedSeparator();

                ImGui::EndChild();

                // COLUMN 2 - World Settings
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildMiscWorldSettings", ImVec2(0,-8));
                ImGui::PushItemWidth(-FLT_MIN);

                // Gerudo Fortress
                ImGui::Text("Gerudo Fortress Carpenters");
                UIWidgets::InsertHelpHoverText(
                    "Sets the amount of carpenters required to repair the bridge "
                    "in Gerudo Valley.\n"
                    "\n"
                    "Normal - All 4 carpenters are required to be saved.\n"
                    "\n"
                    "Fast - Only the bottom left carpenter requires rescuing.\n"
                    "\n"
                    "Open - The bridge is repaired from the start."
                );
                UIWidgets::EnhancementCombobox("gRandomizeGerudoFortress", randoGerudoFortress, 3, 0);

                UIWidgets::PaddedSeparator();

                // Rainbow Bridge
                ImGui::Text("Rainbow Bridge Requirements");
                UIWidgets::InsertHelpHoverText(
                    "Alters the requirements to open the bridge to Ganon's Castle.\n"
                    "\n"
                    "Vanilla - Obtain the Shadow Medallion, Spirit Medallion and Light Arrows.\n"
                    "\n"
                    "Always open - No requirements.\n"
                    "\n"
                    "Stones - Obtain the specified amount of spiritual stones.\n"
                    "\n"
                    "Medallions - Obtain the specified amount of medallions.\n"
                    "\n"
                    "Dungeon rewards - Obtain the specified total sum of spiritual "
                    "stones or medallions.\n"
                    "\n"
                    "Dungeons - Complete the specified amount of dungeons. Dungeons "
                    "are considered complete after stepping in to the blue warp after "
                    "the boss.\n"
                    "\n"
                    "Tokens - Obtain the specified amount of Skulltula tokens."
                );

                UIWidgets::EnhancementCombobox("gRandomizeRainbowBridge", randoRainbowBridge, 7, 0);
                ImGui::PopItemWidth();
                switch (CVar_GetS32("gRandomizeRainbowBridge", 0)) {
                    case 0:
                        break;
                    case 1:
                        break;
                    case 2:
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Stone Count: %d", "##RandoStoneCount",
                                                        "gRandomizeStoneCount", 1, 3, "", 3, true);
                        break;
                    case 3:
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Medallion Count: %d", "##RandoMedallionCount",
                                                        "gRandomizeMedallionCount", 1, 6, "", 6, true);
                        break;
                    case 4:
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Reward Count: %d", "##RandoRewardCount",
                                                        "gRandomizeRewardCount", 1, 9, "", 9, true);
                        break;
                    case 5:
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Dungeon Count: %d", "##RandoDungeonCount",
                                                        "gRandomizeDungeonCount", 1, 8, "", 8, true);
                        break;
                    case 6:
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Token Count: %d", "##RandoTokenCount",
                                                        "gRandomizeTokenCount", 1, 100, "", 100, true);
                        break;
                }

                UIWidgets::PaddedSeparator();

                // Ganon's Trials
                ImGui::PushItemWidth(-FLT_MIN);
                ImGui::Text("Ganon's Trials");
                UIWidgets::InsertHelpHoverText(
                    "Sets the number of Ganon's Trials required to dispel the barrier.\n"
                    "\n"
                    "Skip - No Trials are required and the barrier is already dispelled.\n"
                    "\n"
                    "Set Number - Select a number of trials that will be required from the"
                    "slider below. Which specific trials you need to complete will be random.\n"
                    "\n"
                    "Random Number - A Random number and set of trials will be required."
                );
                UIWidgets::EnhancementCombobox("gRandomizeGanonTrial", randoGanonsTrial, 3, 1);
                ImGui::PopItemWidth();
                if (CVar_GetS32("gRandomizeGanonTrial", 1) == 1) {
                    ImGui::Dummy(ImVec2(0.0f, 0.0f));
                    UIWidgets::EnhancementSliderInt("Ganon's Trial Count: %d", "##RandoTrialCount",
                                                    "gRandomizeGanonTrialCount", 1, 6, "", 6, true);
                    UIWidgets::InsertHelpHoverText("Set the number of trials required to enter Ganon's Tower.");
                }

                UIWidgets::PaddedSeparator();

                ImGui::EndChild();

                // COLUMN 3 - Shuffle Entrances
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildShuffleEntrances", ImVec2(0, -8));
                ImGui::PushItemWidth(-FLT_MIN);

                ImGui::Text("Coming soon");

                UIWidgets::PaddedSeparator();

                ImGui::PopItemWidth();
                ImGui::EndChild();
                ImGui::EndTable();
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Items")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (ImGui::BeginTable("tableRandoStartingInventory", 3,
                                  ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Shuffle Items", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Shuffle NPCs & Merchants", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Shuffle Dungeon Items", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::TableHeadersRow();
                ImGui::PopItemFlag();
                ImGui::TableNextRow();

                // COLUMN 1 - Shuffle Items
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildShuffleItems", ImVec2(0, -8));
                ImGui::PushItemWidth(-FLT_MIN);

                // Shuffle Songs
                ImGui::Text(Settings::ShuffleSongs.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Song locations - Songs will only appear at locations that normally teach songs.\n"
                    "\n"
                    "Dungeon rewards - Songs appear after beating a major dungeon boss.\n"
                    "The 4 remaining songs are located at:\n"
                    "  - Zelda's lullaby location\n"
                    "  - Ice Cavern's Serenade of Water location\n"
                    "  - Bottom of the Well Lens of Truth location\n"
                    "  - Gerudo Training Ground's Ice Arrows location\n"
                    "\n"
                    "Anywhere - Songs can appear at any location."
                );
                UIWidgets::EnhancementCombobox("gRandomizeShuffleSongs", randoShuffleSongs, 3, 0);

                UIWidgets::PaddedSeparator();

                // Shuffle Tokens
                ImGui::Text(Settings::Tokensanity.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Shuffles Golden Skulltula Tokens into the item pool. This means "
                    "Golden Skulltulas can contain other items as well.\n"
                    "\n"
                    "Off - GS tokens will not be shuffled.\n"
                    "\n"
                    "Dungeons - Only shuffle GS tokens that are within dungeons.\n"
                    "\n"
                    "Overworld - Only shuffle GS tokens that are outside of dungeons.\n"
                    "\n"
                    "All Tokens - Shuffle all 100 GS tokens."
                );
                UIWidgets::EnhancementCombobox("gRandomizeShuffleTokens", randoTokensanity, 4, 0);

                ImGui::Dummy(ImVec2(0,0));

                UIWidgets::EnhancementCheckbox("Nighttime GS expect Sun's Song", "gRandomizeGsExpectSunsSong");
                UIWidgets::InsertHelpHoverText(
                    "All Golden Skulltulas that require nighttime to appear will only be "
                    "expected to be collected after getting Sun's Song."
                );

                UIWidgets::PaddedSeparator();

                // Shuffle Kokiri Sword
                // Disabled when Start with Kokiri Sword is active
                bool disableShuffleKokiriSword = CVar_GetS32("gRandomizeStartingKokiriSword", 0);
                const char* disableShuffleKokiriSwordText = "This option is disabled because \"Start with Kokiri Sword\" is enabled.";
                UIWidgets::EnhancementCheckbox(Settings::ShuffleKokiriSword.GetName().c_str(), "gRandomizeShuffleKokiriSword",
                                                disableShuffleKokiriSword, disableShuffleKokiriSwordText);
                UIWidgets::InsertHelpHoverText(
                    "Shuffles the Kokiri Sword into the item pool.\n"
                    "\n"
                    "This will require the use of sticks until the Kokiri Sword is found."
                );

                UIWidgets::PaddedSeparator();

                // Shuffle Ocarinas
                // Disabled when Start with Ocarina is active
                bool disableShuffleOcarinas = CVar_GetS32("gRandomizeStartingOcarina", 0);
                const char* disableShuffleOcarinasText = "This option is disabled because \"Start with Fairy Ocarina\" is enabled.";
                UIWidgets::EnhancementCheckbox(Settings::ShuffleOcarinas.GetName().c_str(), "gRandomizeShuffleOcarinas",
                                              disableShuffleOcarinas, disableShuffleOcarinasText);
                UIWidgets::InsertHelpHoverText(
                    "Enabling this shuffles the Fairy Ocarina and the Ocarina of Time into the item pool.\n"
                    "\n"
                    "This will require finding an Ocarina before being able to play songs."
                );

                UIWidgets::PaddedSeparator();

                // Shuffle Weird Egg
                // Disabled when Skip Child Zelda is active
                bool disableShuffleWeirdEgg = CVar_GetS32("gRandomizeSkipChildZelda", 0);
                const char* disableShuffleWeirdEggText = "This option is disabled because \"Skip Child Zelda\" is enabled.";
                UIWidgets::EnhancementCheckbox(Settings::ShuffleWeirdEgg.GetName().c_str(), "gRandomizeShuffleWeirdEgg",
                                              disableShuffleWeirdEgg, disableShuffleWeirdEggText);
                UIWidgets::InsertHelpHoverText(
                    "Shuffles the Weird Egg from Malon in to the item pool. Enabling "
                    "\"Skip Child Zelda\" disables this feature.\n"
                    "\n"
                    "The Weird Egg is required to unlock several events:\n"
                    "  - Zelda's Lullaby from Impa\n"
                    "  - Saria's song in Sacred Forest Meadow\n"
                    "  - Epona's song and chicken minigame at Lon Lon Ranch\n"
                    "  - Zelda's letter for Kakariko gate (if set to closed)\n"
                    "  - Happy Mask Shop sidequest\n"
                );

                UIWidgets::PaddedSeparator();

                // Shuffle Gerudo Membership Card
                UIWidgets::EnhancementCheckbox(Settings::ShuffleGerudoToken.GetName().c_str(),
                                                "gRandomizeShuffleGerudoToken");
                UIWidgets::InsertHelpHoverText(
                    "Shuffles the Gerudo Membership Card into the item pool.\n"
                    "\n"
                    "The Gerudo Card is required to enter the Gerudo Training Grounds, opening "
                    "the gate to Haunted Wasteland and the Horseback Archery minigame."
                );

                UIWidgets::PaddedSeparator();

                ImGui::PopItemWidth();
                ImGui::EndChild();

                // COLUMN 2 - Shuffle NPCs & Merchants
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildShuffleNpcs", ImVec2(0, -8));
                ImGui::PushItemWidth(-FLT_MIN);

                // Shopsanity
                ImGui::Text(Settings::Shopsanity.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Off - All shop items will be the same as vanilla.\n"
                    "\n"
                    "0 Items - Vanilla shop items will be shuffled among different shops.\n"
                    "\n"
                    "1-4 Items - Vanilla shop items will be shuffled among different shops, and each shop will contain 1-4 non-vanilla shop items.\n"
                    "\n"
                    "Random - Vanilla shop items will be shuffled among different shops, and each shop will contain a random number(1-4) of non-vanilla shop items.\n"
                );
                UIWidgets::EnhancementCombobox("gRandomizeShopsanity", randoShopsanity, 7, 0);

                UIWidgets::PaddedSeparator();

                // Shuffle Scrubs
                ImGui::Text(Settings::Scrubsanity.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Off - Scrubs will not be shuffled. The 3 Scrubs that give one-time items in the vanilla game "
                    "(PoH, Deku Nut capacity, and Deku Stick capacity) will have random items.\n"
                    "\n"
                    "Affordable - Scrubs will be shuffled and their item will cost 10 rupees.\n"
                    "\n"
                    "Expensive - Scrubs will be shuffled and their item will cost the vanilla price.\n"
                    "\n"
                    "Random - Scrubs will be shuffled and their item will cost will be between 0-95 rupees.\n");
                UIWidgets::EnhancementCombobox("gRandomizeShuffleScrubs", randoShuffleScrubs, 4, 0);

                UIWidgets::PaddedSeparator();

                // Shuffle Cows
                UIWidgets::EnhancementCheckbox(Settings::ShuffleCows.GetName().c_str(), "gRandomizeShuffleCows");
                UIWidgets::InsertHelpHoverText("Cows give a randomized item from the pool upon performing Epona's Song in front of them.");

                UIWidgets::PaddedSeparator();

                UIWidgets::EnhancementCheckbox(Settings::ShuffleMagicBeans.GetName().c_str(), "gRandomizeShuffleBeans");
                UIWidgets::InsertHelpHoverText(
                    "Enabling this adds a pack of 10 beans to the item pool and changes the Magic Bean "
                    "Salesman to sell a random item at a price of 60 rupees."
                );

                UIWidgets::PaddedSeparator();

                // Shuffle Frog Song Rupees
                UIWidgets::EnhancementCheckbox(Settings::ShuffleFrogSongRupees.GetName().c_str(), "gRandomizeShuffleFrogSongRupees");
                UIWidgets::InsertHelpHoverText(
                    "Shuffles 5 Purple Rupees into to the item pool, and allows\n"
                    "you to earn items by playing songs at the Frog Choir.\n"
                    "\n"
                    "This setting does not effect the item earned from playing\n"
                    "the Song of Storms and the frog song minigame."
                );

                UIWidgets::PaddedSeparator();

                // Shuffle Adult Trade Quest
                UIWidgets::EnhancementCheckbox(Settings::ShuffleAdultTradeQuest.GetName().c_str(),
                                                "gRandomizeShuffleAdultTrade");
                UIWidgets::InsertHelpHoverText(
                    "Adds all of the adult trade quest items into the pool, each of which "
                    "can be traded for a unique reward.\n"
                    "\n"
                    "You will be able to choose which of your owned adult trade items is visible "
                    "in the inventory by selecting the item with A and using the control stick or "
                    "D-pad.\n"
                    "\n"
                    "If disabled, only the Claim Check will be found in the pool."
                );

                UIWidgets::PaddedSeparator();

                ImGui::PopItemWidth();
                ImGui::EndChild();

                // COLUMN 3 - Shuffle Dungeon Items
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildShuffleDungeonItems", ImVec2(0, -8));
                ImGui::PushItemWidth(-FLT_MIN);

                // Shuffle Dungeon Rewards
                ImGui::Text(Settings::ShuffleRewards.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Shuffles the location of spiritual stones and medallions.\n"
                    "\n"
                    "End of dungeons - Spiritual stones and medallions will be given as rewards "
                    "for beating major dungeons. Link will always start with one stone or medallion.\n"
                    "\n"
                    "Any dungeon - Spiritual stones and medallions can be found inside any dungeon.\n"
                    "\n"
                    "Overworld - Spiritual stones and medallions can only be found outside of dungeons.\n"
                    "\n"
                    "Anywhere - Spiritual stones and medallions can appear anywhere."
                );
                UIWidgets::EnhancementCombobox("gRandomizeShuffleDungeonReward", randoShuffleDungeonRewards, 4, 0);

                UIWidgets::PaddedSeparator();

                // Maps & Compasses
                ImGui::Text(Settings::MapsAndCompasses.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Start with - You will start with Maps & Compasses from all dungeons.\n"
                    "\n"
                    "Vanilla - Maps & Compasses will appear in their vanilla locations.\n"
                    "\n"
                    "Own dungeon - Maps & Compasses can only appear in their respective dungeon.\n"
                    "\n"
                    "Any dungeon - Maps & Compasses can only appear inside of any dungon.\n"
                    "\n"
                    "Overworld - Maps & Compasses can only appear outside of dungeons.\n"
                    "\n"
                    "Anywhere - Maps & Compasses can appear anywhere in the world."
                );
                UIWidgets::EnhancementCombobox("gRandomizeStartingMapsCompasses", randoShuffleMapsAndCompasses, 6, 2);

                UIWidgets::PaddedSeparator();

                // Keysanity
                ImGui::Text(Settings::Keysanity.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Start with - You will start with all Small Keys from all dungeons.\n"
                    "\n"
                    "Vanilla - Small Keys will appear in their vanilla locations.\n"
                    "\n"
                    "Own dungeon - Small Keys can only appear in their respective dungeon.\n"
                    "\n"
                    "Any dungeon - Small Keys can only appear inside of any dungon.\n"
                    "\n"
                    "Overworld - Small Keys can only appear outside of dungeons.\n"
                    "\n"
                    "Anywhere - Small Keys can appear anywhere in the world."
                );
                UIWidgets::EnhancementCombobox("gRandomizeKeysanity", randoShuffleSmallKeys, 6, 2);

                UIWidgets::PaddedSeparator();

                // Gerudo Keys
                ImGui::Text(Settings::GerudoKeys.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Vanilla - Thieve's Hideout Keys will appear in their vanilla locations.\n"
                    "\n"
                    "Any dungeon - Thieve's Hideout Keys can only appear inside of any dungon.\n"
                    "\n"
                    "Overworld - Thieve's Hideout Keys can only appear outside of dungeons.\n"
                    "\n"
                    "Anywhere - Thieve's Hideout Keys can appear anywhere in the world."
                );
                UIWidgets::EnhancementCombobox("gRandomizeGerudoKeys", randoShuffleGerudoFortressKeys, 4, 0);

                UIWidgets::PaddedSeparator();

                // Boss Keysanity
                ImGui::Text(Settings::BossKeysanity.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Start with - You will start with Boss keys from all dungeons.\n"
                    "\n"
                    "Vanilla - Boss Keys will appear in their vanilla locations.\n"
                    "\n"
                    "Own dungeon - Boss Keys can only appear in their respective dungeon.\n"
                    "\n"
                    "Any dungeon - Boss Keys can only appear inside of any dungon.\n"
                    "\n"
                    "Overworld - Boss Keys can only appear outside of dungeons.\n"
                    "\n"
                    "Anywhere - Boss Keys can appear anywhere in the world."
                );
                UIWidgets::EnhancementCombobox("gRandomizeBossKeysanity", randoShuffleBossKeys, 6, 2);

                UIWidgets::PaddedSeparator();

                // Ganon's Boss Key
                ImGui::Text(Settings::GanonsBossKey.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Vanilla - Ganon's Boss Key will appear in the vanilla location.\n"
                    "\n"
                    "Own dungeon - Ganon's Boss Key can appear anywhere inside Ganon's Castle.\n"
                    "\n"
                    "Start with - Places Ganon's Boss Key in your starting inventory."
                    "\n"
                    "Any dungeon - Ganon's Boss Key Key can only appear inside of any dungon.\n"
                    "\n"
                    "Overworld - Ganon's Boss Key Key can only appear outside of dungeons.\n"
                    "\n"
                    "Anywhere - Ganon's Boss Key Key can appear anywhere in the world."
                );
                UIWidgets::EnhancementCombobox("gRandomizeShuffleGanonBossKey", randoShuffleGanonsBossKey, 6, 1);

                UIWidgets::PaddedSeparator();

                ImGui::PopItemWidth();
                ImGui::EndChild();
                ImGui::EndTable();
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Gameplay")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (ImGui::BeginTable("tableRandoGameplay", 3, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Timesavers", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Item Pool & Hints", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Additional Features", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::TableHeadersRow();
                ImGui::PopItemFlag();
                ImGui::TableNextRow();

                // COLUMN 1 - TIME SAVERS
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildTimeSavers", ImVec2(0, -8));

                // Cuccos to return
                UIWidgets::EnhancementSliderInt("Cuccos to return: %d", "##RandoCuccosToReturn",
                                               "gRandomizeCuccosToReturn", 0, 7, "", 7, true);
                UIWidgets::InsertHelpHoverText("The amount of cuccos needed to claim the reward from Anju the cucco lady");

                UIWidgets::PaddedSeparator();

                // Big Poe Target Count
                UIWidgets::EnhancementSliderInt("Big Poe Target Count: %d", "##RandoBigPoeTargetCount",
                                               "gRandomizeBigPoeTargetCount", 1, 10, "", 10, true);
                UIWidgets::InsertHelpHoverText("The Poe collector will give a reward for turning in this many Big Poes.");

                UIWidgets::PaddedSeparator();

                // Skip child stealth
                // Disabled when Skip Child Zelda is active
                bool disableChildStealth = CVar_GetS32("gRandomizeSkipChildZelda", 0);
                const char* disableChildStealthText = "This option is disabled because \"Skip Child Zelda\" is enabled";
                UIWidgets::EnhancementCheckbox(Settings::SkipChildStealth.GetName().c_str(), "gRandomizeSkipChildStealth", disableChildStealth, disableChildStealthText);
                UIWidgets::InsertHelpHoverText("The crawlspace into Hyrule Castle goes straight to Zelda, skipping the guards.");

                UIWidgets::PaddedSeparator();

                // Skip child zelda
                UIWidgets::EnhancementCheckbox("Skip Child Zelda", "gRandomizeSkipChildZelda");
                UIWidgets::InsertHelpHoverText("Start with Zelda's Letter and the item Impa would normally give you and skip the sequence up "
                                    "until after meeting Zelda. Disables the ability to shuffle Weird Egg.");

                UIWidgets::PaddedSeparator();

                // Skip Epona race
                UIWidgets::EnhancementCheckbox(Settings::SkipEponaRace.GetName().c_str(), "gRandomizeSkipEponaRace");
                UIWidgets::InsertHelpHoverText("Epona can be summoned with Epona's Song without needing to race Ingo.");

                UIWidgets::PaddedSeparator();

                // Skip tower escape
                UIWidgets::EnhancementCheckbox(Settings::SkipTowerEscape.GetName().c_str(), "gRandomizeSkipTowerEscape");
                UIWidgets::InsertHelpHoverText("The tower escape sequence between Ganondorf and Ganon will be skipped.");

                UIWidgets::PaddedSeparator();

                // Complete mask quest
                UIWidgets::EnhancementCheckbox(Settings::CompleteMaskQuest.GetName().c_str(),
                                              "gRandomizeCompleteMaskQuest");
                UIWidgets::InsertHelpHoverText("Once the happy mask shop is opened, all masks will be available to be borrowed.");

                UIWidgets::PaddedSeparator();

                // Skip Scarecrow Song
                UIWidgets::EnhancementCheckbox(Settings::FreeScarecrow.GetName().c_str(),
                                               "gRandomizeSkipScarecrowsSong");
                UIWidgets::InsertHelpHoverText(
                    "Start with the ability to summon Pierre the scarecrow. Pulling out an ocarina in the usual locations will automatically summon him."
                );

                UIWidgets::PaddedSeparator();

                ImGui::EndChild();

                // COLUMN 2 - Item Pool & Hint Settings
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildItemPoolHintSettings", ImVec2(0, -8));
                ImGui::PushItemWidth(-FLT_MIN);

                // Item Pool Settings
                ImGui::Text(Settings::ItemPoolValue.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Sets how many major items appear in the item pool.\n"
                    "\n"
                    "Plentiful - Extra major items are added to the pool.\n"
                    "\n"
                    "Balanced - Original item pool.\n"
                    "\n"
                    "Scarce - Some excess items are removed, including health upgrades.\n"
                    "\n"
                    "Minimal - Most excess items are removed."
                );
                UIWidgets::EnhancementCombobox("gRandomizeItemPool", randoItemPool, 4, 1);
                UIWidgets::PaddedSeparator();

                // Ice Traps
                ImGui::Text(Settings::IceTrapValue.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Sets how many items are replaced by ice traps.\n"
                    "\n"
                    "Off - No ice traps.\n"
                    "\n"
                    "Normal - Only Ice Traps from the base item pool are shuffled in.\n"
                    "\n"
                    "Extra - Chance to replace added junk items with additional ice traps.\n"
                    "\n"
                    "Mayhem - All added junk items will be Ice Traps.\n"
                    "\n"
                    "Onslaught - All junk items will be replaced by Ice Traps, even those "
                    "in the base pool."
                );
                UIWidgets::EnhancementCombobox("gRandomizeIceTraps", randoIceTraps, 5, 1);

                UIWidgets::PaddedSeparator();

                // Gossip Stone Hints
                ImGui::Text(Settings::GossipStoneHints.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Allows Gossip Stones to provide hints on item locations. Hints mentioning "
                    "\"Way of the Hero\" indicate a location that holds an item required to beat "
                    "the seed.\n"
                    "\n"
                    "No hints - No hints will be given at all.\n"
                    "\n"
                    "Need Nothing - Hints are always available from Gossip Stones.\n"
                    "\n"
                    "Need Stone of Agony - Hints are only available after obtaining the Stone of Agony.\n"
                    "\n"
                    "Need Mask of Truth - Hints are only available whilst wearing the Mask of Truth.\n");
                UIWidgets::EnhancementCombobox("gRandomizeGossipStoneHints", randoGossipStoneHints, 4, 1);
                if (CVar_GetS32("gRandomizeGossipStoneHints", 1) != 0) {
                    // Hint Clarity
                    ImGui::Dummy(ImVec2(0.0f, 0.0f));
                    ImGui::Indent();
                    ImGui::Text(Settings::ClearerHints.GetName().c_str());
                    UIWidgets::InsertHelpHoverText(
                        "Sets the difficulty of hints.\n"
                        "\n"
                        "Obscure - Hints are unique for each item, but the writing may be cryptic.\n"
                        "Ex: Kokiri Sword > a butter knife\n"
                        "\n"
                        "Ambiguous - Hints are clearly written, but may refer to more than one item.\n"
                        "Ex: Kokiri Sword > a sword\n"
                        "\n"
                        "Clear - Hints are clearly written and are unique for each item.\n"
                        "Ex: Kokiri Sword > the Kokiri Sword"
                    );
                    UIWidgets::EnhancementCombobox("gRandomizeHintClarity", randoHintClarity, 3, 2);

                    // Hint Distribution
                    ImGui::Dummy(ImVec2(0.0f, 0.0f));
                    ImGui::Text(Settings::HintDistribution.GetName().c_str());
                    UIWidgets::InsertHelpHoverText(
                        "Sets how many hints will be useful.\n"
                        "\n"
                        "Useless - Only junk hints.\n"
                        "\n"
                        "Balanced - Recommended hint spread.\n"
                        "\n"
                        "Strong - More useful hints.\n"
                        "\n"
                        "Very Strong - Many powerful hints."
                    );
                    UIWidgets::EnhancementCombobox("gRandomizeHintDistribution", randoHintDistribution, 4, 1);
                    ImGui::Unindent();
                }

                UIWidgets::PaddedSeparator();

                ImGui::PopItemWidth();
                ImGui::EndChild();

                // COLUMN 3 - Additional Features
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildAdditionalFeatures", ImVec2(0, -8));
                ImGui::PushItemWidth(-FLT_MIN);

                UIWidgets::EnhancementCheckbox("Full Wallets", "gRandomizeFullWallets");
                UIWidgets::InsertHelpHoverText("Start with a full wallet. All wallet upgrades come filled with rupees.");

                UIWidgets::PaddedSeparator();

                // Bombchus in Logic
                UIWidgets::EnhancementCheckbox(Settings::BombchusInLogic.GetName().c_str(), "gRandomizeBombchusInLogic");
                UIWidgets::InsertHelpHoverText(
                    "Bombchus are properly considered in logic.\n"
                    "\n"
                    "The first Bombchu pack will always be 20, and subsequent packs will be "
                    "5 or 10 based on how many you have.\n"
                    "Once found, they can be replenished at the Bombchu shop.\n"
                    "\n"
                    "Bombchu Bowling is opened by obtaining Bombchus."
                );

                UIWidgets::PaddedSeparator();

                // Enable Bombchu Drops
                UIWidgets::EnhancementCheckbox("Enable Bombchu Drops", "gRandomizeEnableBombchuDrops");
                UIWidgets::InsertHelpHoverText(
                    "Once you obtain bombchus for the first time, refills can be found in bushes and other places where bomb drops can normally spawn."
                );

                UIWidgets::PaddedSeparator();

                UIWidgets::EnhancementCheckbox("Blue Fire Arrows", "gRandomizeBlueFireArrows");
                UIWidgets::InsertHelpHoverText(
                    "Ice Arrows act like Blue Fire, making them able to melt red ice. "
                    "Item placement logic will respect this option, so it might be required to use this to progress."
                );

                UIWidgets::PaddedSeparator();

                UIWidgets::EnhancementCheckbox("Sunlight Arrows", "gRandomizeSunlightArrows");
                UIWidgets::InsertHelpHoverText(
                    "Light Arrows can be used to light up the sun switches instead of using the Mirror Shield. "
                    "Item placement logic will respect this option, so it might be required to use this to progress."
                );

                UIWidgets::PaddedSeparator();

                ImGui::PopItemWidth();
                ImGui::EndChild();
                ImGui::EndTable();
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }

        static bool locationsTabOpen = false;
        if (ImGui::BeginTabItem("Locations")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (!locationsTabOpen) {
                locationsTabOpen = true;
                RandomizerCheckObjects::UpdateImGuiVisibility();
                // todo: this efficently when we build out cvar array support
                std::stringstream excludedLocationStringStream(CVar_GetString("gRandomizeExcludedLocations", ""));
                std::string excludedLocationString;
                excludedLocations.clear();
                while (getline(excludedLocationStringStream, excludedLocationString, ',')) {
                    excludedLocations.insert((RandomizerCheck)std::stoi(excludedLocationString));
                }
            }

            if (ImGui::BeginTable("tableRandoLocations", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Included", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Excluded", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::TableHeadersRow();
                ImGui::PopItemFlag();
                ImGui::TableNextRow();

                // COLUMN 1 - INCLUDED LOCATIONS
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;

                static ImGuiTextFilter locationSearch;
                locationSearch.Draw();

                ImGui::BeginChild("ChildIncludedLocations", ImVec2(0, -8));
                for (auto areaIt : RandomizerCheckObjects::GetAllRCObjects()) {
                    bool hasItems = false;
                    for (auto locationIt : areaIt.second) {
                        if (locationIt.visibleInImgui && !excludedLocations.count(locationIt.rc) &&
                            locationSearch.PassFilter(locationIt.rcSpoilerName.c_str())) {

                            hasItems = true;
                            break;
                        }
                    }

                    if (hasItems) {
                        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                        if (ImGui::TreeNode(RandomizerCheckObjects::GetRCAreaName(areaIt.first).c_str())) {
                            for (auto locationIt : areaIt.second) {
                                if (locationIt.visibleInImgui && !excludedLocations.count(locationIt.rc) &&
                                    locationSearch.PassFilter(locationIt.rcSpoilerName.c_str())) {

                                    if (ImGui::ArrowButton(std::to_string(locationIt.rc).c_str(), ImGuiDir_Right)) {
                                        excludedLocations.insert(locationIt.rc);
                                        // todo: this efficently when we build out cvar array support
                                        std::string excludedLocationString = "";
                                        for (auto excludedLocationIt : excludedLocations) {
                                            excludedLocationString += std::to_string(excludedLocationIt);
                                            excludedLocationString += ",";
                                        }
                                        CVar_SetString("gRandomizeExcludedLocations", excludedLocationString.c_str());
                                        SohImGui::RequestCvarSaveOnNextTick();
                                    }
                                    ImGui::SameLine();
                                    ImGui::Text(locationIt.rcShortName.c_str());
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                }
                ImGui::EndChild();

                // COLUMN 2 - EXCLUDED LOCATIONS
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;

                ImGui::BeginChild("ChildExcludedLocations", ImVec2(0, -8));
                for (auto areaIt : RandomizerCheckObjects::GetAllRCObjects()) {
                    bool hasItems = false;
                    for (auto locationIt : areaIt.second) {
                        if (locationIt.visibleInImgui && excludedLocations.count(locationIt.rc)) {
                            hasItems = true;
                            break;
                        }
                    }

                    if (hasItems) {
                        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                        if (ImGui::TreeNode(RandomizerCheckObjects::GetRCAreaName(areaIt.first).c_str())) {
                            for (auto locationIt : areaIt.second) {
                                auto elfound = excludedLocations.find(locationIt.rc);
                                if (locationIt.visibleInImgui && elfound != excludedLocations.end()) {
                                    if (ImGui::ArrowButton(std::to_string(locationIt.rc).c_str(), ImGuiDir_Left)) {
                                        excludedLocations.erase(elfound);
                                        // todo: this efficently when we build out cvar array support
                                        std::string excludedLocationString = "";
                                        for (auto excludedLocationIt : excludedLocations) {
                                            excludedLocationString += std::to_string(excludedLocationIt);
                                            excludedLocationString += ",";
                                        }
                                        CVar_SetString("gRandomizeExcludedLocations", excludedLocationString.c_str());
                                        SohImGui::RequestCvarSaveOnNextTick();
                                    }
                                    ImGui::SameLine();
                                    ImGui::Text(locationIt.rcShortName.c_str());
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                }
                ImGui::EndChild();

                ImGui::EndTable();
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        } else {
            locationsTabOpen = false;
        }

        if (ImGui::BeginTabItem("Tricks/Glitches")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (ImGui::BeginTable("tableRandoLogic", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::PushItemWidth(170.0);
                ImGui::Text("Logic Rules");
                UIWidgets::InsertHelpHoverText(
                    "Glitchless - No glitches are required, but may require some minor tricks.\n"
                    "\n"
                    "No logic - Item placement is completely random. MAY BE IMPOSSIBLE TO BEAT."
                );
                UIWidgets::EnhancementCombobox("gRandomizeLogicRules", randoLogicRules, 2, 0);

                UIWidgets::PaddedSeparator();

                // Enable Glitch-Useful Cutscenes
                UIWidgets::EnhancementCheckbox(Settings::EnableGlitchCutscenes.GetName().c_str(),
                                              "gRandomizeEnableGlitchCutscenes");
                UIWidgets::InsertHelpHoverText(
                    "The cutscenes of the Poes in Forest Temple and Darunia in Fire Temple will not be skipped. "
                    "These cutscenes are only useful for glitched gameplay and can be safely skipped otherwise.");

                ImGui::PopItemWidth();
                ImGui::EndTable();
            }
            if (ImGui::BeginTable("tableRandoTricksGlitches", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Disabled", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Enabled", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::TableHeadersRow();
                ImGui::PopItemFlag();
                ImGui::TableNextRow();

                // COLUMN 1 - DISABLED TRICKS AND GLITCHES
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildTricksGlitchesDisabled", ImVec2(0, -8));

                ImGui::Text("Coming soon");

                ImGui::EndChild();

                // COLUMN 2 - ENABLED TRICKS AND GLITCHES
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildTricksGlitchesEnabled", ImVec2(0, -8));

                ImGui::Text("Coming soon");

                ImGui::EndChild();
                ImGui::EndTable();
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Starting Inventory")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (ImGui::BeginTable("tableRandoStartingInventory", 3,
                                  ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Starting Equipment", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Starting Items", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Starting Songs", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::TableHeadersRow();
                ImGui::PopItemFlag();
                ImGui::TableNextRow();

                // COLUMN 1 - STARTING EQUIPMENT
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildStartingEquipment", ImVec2(0, -8));
                // Don't display this option if Dungeon Rewards are Shuffled to End of Dungeon.
                // TODO: Show this but disabled when we have options for disabled Comboboxes.
                if (CVar_GetS32("gRandomizeShuffleDungeonReward", 0) != 0) {
                    ImGui::Text(Settings::LinksPocketItem.GetName().c_str());
                    UIWidgets::EnhancementCombobox("gRandomizeLinksPocket", randoLinksPocket, 4, 0);
                    UIWidgets::PaddedSeparator();
                }

                UIWidgets::EnhancementCheckbox(Settings::StartingKokiriSword.GetName().c_str(),
                                              "gRandomizeStartingKokiriSword");
                UIWidgets::PaddedSeparator();
                UIWidgets::EnhancementCheckbox(Settings::StartingDekuShield.GetName().c_str(),
                                              "gRandomizeStartingDekuShield");

                UIWidgets::PaddedSeparator();

                ImGui::EndChild();

                // COLUMN 2 - STARTING ITEMS
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildStartingItems", ImVec2(0, -8));

                UIWidgets::EnhancementCheckbox(Settings::StartingOcarina.GetName().c_str(), "gRandomizeStartingOcarina");
                UIWidgets::PaddedSeparator();
                UIWidgets::EnhancementCheckbox(Settings::StartingConsumables.GetName().c_str(),
                                              "gRandomizeStartingConsumables");
                UIWidgets::PaddedSeparator();

                ImGui::EndChild();

                // COLUMN 3 - STARTING SONGS
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildStartingSongs", ImVec2(0, -8));

                ImGui::Text("Coming soon");

                UIWidgets::PaddedSeparator();

                ImGui::EndChild();
                ImGui::EndTable();
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::PopItemFlag();
    ImGui::PopStyleVar();
    ImGui::End();
}

void CreateGetItemMessages(std::vector<GetItemMessage> messageEntries) {
    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(Randomizer::getItemMessageTableID);
    for (GetItemMessage messageEntry : messageEntries) {
        if (messageEntry.giid == RG_ICE_TRAP) {
            customMessageManager->CreateMessage(Randomizer::getItemMessageTableID, messageEntry.giid,
                                                { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, messageEntry.english,
                                                  messageEntry.german, messageEntry.french });
        } else {
            customMessageManager->CreateGetItemMessage(Randomizer::getItemMessageTableID, messageEntry.giid,
                                                       messageEntry.iid,
                                                       { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, messageEntry.english,
                                                         messageEntry.german, messageEntry.french });
        }
    }
}

void CreateRupeeMessages() {
    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(Randomizer::rupeeMessageTableID);
    const std::vector<u8> rupees = { TEXT_BLUE_RUPEE, TEXT_RED_RUPEE, TEXT_PURPLE_RUPEE, TEXT_HUGE_RUPEE };
    std::string rupeeText = " ";
    for (u8 rupee : rupees) {
        switch (rupee) {
            case TEXT_BLUE_RUPEE:
                rupeeText = "\x05\x03 5 #RUPEE#\x05\x00";
                break;
            case TEXT_RED_RUPEE:
                rupeeText = "\x05\x01 20 #RUPEE#\x05\x00";
                break;
            case TEXT_PURPLE_RUPEE:
                rupeeText = "\x05\x05 50 #RUPEE#\x05\x00";
                break;
            case TEXT_HUGE_RUPEE:
                rupeeText = "\x05\x06 200 #RUPEE#\x05\x00";
                break;
        }
        customMessageManager->CreateMessage(Randomizer::rupeeMessageTableID, rupee,
            { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM,
                "You found" + rupeeText + " !",
                "Du hast" + rupeeText + "  gefunden!",
                "Vous obtenez" + rupeeText + " !"
        });
    }
}

std::string Randomizer::RandomizeRupeeName(std::string message, int language) {
    int randomIndex;
    std::string replaceWith;
    switch (language) {
        case LANGUAGE_ENG:
            randomIndex = rand() % (sizeof(englishRupeeNames) / sizeof(englishRupeeNames[0]));
            replaceWith = englishRupeeNames[randomIndex];
            break;
        case LANGUAGE_GER:
            randomIndex = rand() % (sizeof(germanRupeeNames) / sizeof(germanRupeeNames[0]));
            replaceWith = germanRupeeNames[randomIndex];
            break;
        case LANGUAGE_FRA:
            randomIndex = rand() % (sizeof(frenchRupeeNames) / sizeof(frenchRupeeNames[0]));
            replaceWith = frenchRupeeNames[randomIndex];
            break;
    }
    std::string replaceString = "#RUPEE#";
    size_t pos = message.find(replaceString);
    size_t len = replaceString.length();
    message.replace(pos, len, replaceWith);
    CustomMessageManager::Instance->FormatCustomMessage(message);
    return message;
}

CustomMessageEntry Randomizer::GetRupeeMessage(u16 rupeeTextId) {
    CustomMessageEntry messageEntry =
        CustomMessageManager::Instance->RetrieveMessage(Randomizer::rupeeMessageTableID, rupeeTextId);
    messageEntry.english = Randomizer::RandomizeRupeeName(messageEntry.english, LANGUAGE_ENG);
    messageEntry.german = Randomizer::RandomizeRupeeName(messageEntry.german, LANGUAGE_GER);
    messageEntry.french = Randomizer::RandomizeRupeeName(messageEntry.french, LANGUAGE_FRA);
    return messageEntry;
}

CustomMessageMinimal NaviMessages[NUM_NAVI_MESSAGES] = { 
    
    { "%cMissing a small key in a dungeon?&Maybe the %rboss %chas it!", 
      "%cFehlt dir ein kleiner Schlüssel in &einem Labyrinth? Vielleicht hat ihn&ja der %rEndgegner%c!", 
      "%cIl te manque une %wPetite Clé %cdans&un donjon? C'est peut-être le %rboss&%cqui l'a!" }, 

    { "%cSometimes you can use the %rMegaton&Hammer %cinstead of bombs!", 
      "%cManchmal kannst du den %rStahlhammer&%cstatt Bomben verwenden!",
      "%cParfois, tu peux utiliser la %rMasse&des Titans %cau lieu de tes bombes!" }, 

    { "%cThere are three %gbusiness scrubs %cin &Hyrule who sell %wmysterious items%c. Do&you know where they are?",
      "%cEs gibt drei %gDeku-Händler %cin Hyrule&die mysteriöse Gegenstände&verkaufen. Weißt du wo Sie sind?",
      "%cIl y a trois %gPestes Marchandes%c en&Hyrule qui vendent des %wobjets&mystérieux%c. Tu sais où elles sont?" },

    { "%cStuck on this seed? You could &throw in the towel and check the&%wspoiler log%c...",
      "%cHängst du bei diesem Seed fest?&Du könntest die Flinte ins Korn&werfen und ins %wSpoiler Log %cschauen...",
      "%cSi tu es coincé sur cette seed,&tu peux toujours jeter l'éponge&et regader le %wSpoiler log%c..." },

    { "%cDid you know that the %yHover&Boots %ccan be used to cross&%wquicksand%c?", 
      "%cWußtest du, daß du mit den&%yGleitstiefeln %cTreibsand %wüberqueren&kannst%c?",
      "%cEst-ce que tu savais que les %rBottes&des airs %cpouvaient être utilisées&pour traverser les %wsables mouvants%c?" },

    { "%cYou can reach the back of %wImpa's&House %cby jumping from the&unfinished house with a %rcucco%c!", 
      "%cDu kannst den Balkon von %wImpas&Haus %cerreichen indem du von&der Baustelle aus mit einem %rHuhn&%cspringst!",
      "%cTu peux atteindre l'arrière de la&%wMaison d'Impa %cen sautant depuis la&maison en construction avec une&%rcocotte%c!" },

    { "%cThe %ySpirit Trial %cin %pGanon's Castle&%chas a %whidden chest %csomewhere.&Did you already know that?", 
      "%cDie %yGeister-Prüfung %cin %pGanons&Schloß %chat irgendwo eine&%wversteckte Kiste%c. Weißt du schon&wo?",
      "%cL'%yÉpreuve de l'Esprit%c dans le %pChâteau&de Ganon %ca un coffre caché quelque&part. Je suppose que tu le savais&déjà?" },

    { "%cYou know the %wBombchu Bowling&Alley%c? I heard %wonly two of the &prizes %care worthwhile. The rest &is junk!", 
      "%cKennst du die %wMinenbowlingbahn%c?&Ich habe gehört daß sich nur &%wzwei der Preise%c lohnen. Der Rest&ist Krimskrams!",
      "%cEst-ce que tu savais qu'au %wBowling&Teigneux%c, il n'y a que les %wdeux&premiers prix %cqui sont intéréssant?" },

    { "%cHave you been using %wDeku Nuts&%cenough? I've seen them blow up&a %rBombchu%c!",
      "%cBenutzt du auch genügend %wDeku&Nüsse%c? Ich habe mal gesehen daß&man damit %rKrabbelminen %cdetonieren&kann!",
      "%cTu es sûr d'utiliser tes %wNoix Mojo &%ccorrectement? J'en ai déjà vu&exploser des %rChoux-Péteurs%c!" },

    { "%cYou might end up with an %wextra&key %cfor the %bWater Temple %cor the&%rFire Temple%c. It's for your safety!", 
      "%cVielleicht verbleibt dir im&%bWassertempel %coder %rFeuertempel %cein&%wzusätzlicher Schlüssel%c. Dies&ist zu deiner Sicherheit!",
      "%cIl se peut que tu aies une %wPetite&Clé %cen trop dans le %bTemple de l'Eau&%cou le %rTemple du Feu%c. C'est pour ta&propre sécurité!" },

    { "%cIf you can't pick up a %rbomb&flower %cwith your hands, you can&still detonate it with %rfire %cor&with %warrows%c!", 
      "%cNur weil du eine %rDonnerblume&%cnicht hochheben kannst, so kannst&du sie immernoch mit %rFeuer %coder&%wPfeilen %cdetonieren!",
      "%cSi tu ne peux pas ramasser&un %rChoux-Péteur %cavec tes mains, tu&peux toujours le faire exploser&avec du %rFeu %cou avec des %wflèches%c!" },

    { "%cEven an adult can't push large&blocks without some %wextra&strength%c!", 
      "%cSelbst ein Erwachsener kann ohne&etwas %wzusätzliche Kraft %ckeine&großen Blöcke verschieben!",
      "%cMême un adulte ne peut pas pousser&de grands blocs sans un petit %wgain&de force%c!" },

    { "%cI've heard that %rFlare Dancer&%cis weak to the %wMaster Sword%c!&Have you tried it?", 
      "%cIch habe gehört daß der&%rFlammenderwisch %ceine Schwäche für&das %wMasterschwert %caufweist. Hast du&es schonmal versucht einzusetzen?",
      "%cJ'ai entendu dire que les %rDanse-&Flammes %csont faîbles face à l'%wÉpée de&Légende%c! Est-ce que tu as essayé?" },

    { "%cDon't have a weapon to kill a&%rspider%c? Try throwing a %wpot&%cat it!", 
      "%cFehlt dir die Waffe um gegen&eine %rSkulltula %czu kämpfen? Versuch&Sie mit einem %wKrug %cabzuwerfen!",
      "%cSi tu n'as pas d'arme pour tuer&une %raraignée%c, pourquoi n'essayerais&-tu pas de lui jetter une %wjarre&%cà la figure?" },

    { "%cI hear the patch of %wsoft soil&%cin %bZora's River %cis the only one&that isn't home to a %rspider%c!", 
      "%cIch habe gehört daß die Stelle&%wfeuchten Bodens %cim %bZora-Fluß %cals&einzige keine %rSkulltula %cbeherbergt.",
      "%cJ'ai entendu dire que la %wterre meuble&%cqui se trouve à la %bRivière Zora %cest&la seule qui ne contienne pas&d'%raraignée%c." },

    { "%cThe people of Hyrule sometimes&have %witems %cfor you, but they won't&like it if you're %wwearing a mask%c!", 
      "%cDie Bewohner Hyrules haben manchmal&%wGegenstände %cfür dich, aber Sie mögen&es nicht wenn du %wMasken trägst%c!",
      "%cIl se peut que les habitants d'Hyrule&aient des %wobjets %cpour toi. Par contre,&ils risquent de ne pas trop apprécier&le fait que tu %wportes un masque%c!" },

    { "%cIf you get trapped somewhere, you&might have to %wsave your game %cand&%wreset%c!", 
      "%cSolltest du irgendwo eingeschloßen&sein, mußt du vielleicht dein %wSpiel&speichern %cund %wneustarten%c!",
      "%cSi tu es coincé quelque part, tu&devrais %wsauvegarder ta partie %cet&faire un %wreset%c!" },

    { "%cSheik will meet you in a %rburning&village %conce you have %gForest%c,&%rFire%c, and %bWater %cMedallions!",
      "%cSheik wird dich in einem %rbrennenden&Dorf %ctreffen sobald du das Amulett&des %gWaldes%c, %rFeuers %cund %bWassers&%cbesitzt.",
      "%cSheik t'attendra dans un %rvillage&en feu %clorsque tu auras récupéré&les médaillons de la %gForêt%c, du %rFeu&%cet de l'%bEau%c!" },

    { "%cIf you don't have a %wsword %cas a&child, try buying %wDeku Sticks%c!&They're effective against your foes!",
      "%cSolltest du als Kind kein %wSchwert&%cbesitzen, empfehle ich %wDeku Stäbe&%czu kaufen! Diese sind effektiv gegen&Widersacher!",
      "%cSi tu n'as pas d'%wépée %cen tant&qu'enfant, pourquoi n'irais-tu pas&acheter quelques %wBâtons Mojo%c? Ils&sont efficaces contre tes ennemis!" }
};

void CreateNaviRandoMessages() {
    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(Randomizer::NaviRandoMessageTableID);
    for (u8 i = 0; i <= (NUM_NAVI_MESSAGES - 1); i++) {
        customMessageManager->CreateMessage(Randomizer::NaviRandoMessageTableID, i,
                                            { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM, NaviMessages[i].english,
                                              NaviMessages[i].german, NaviMessages[i].french });
    }
}

void Randomizer::CreateCustomMessages() {
    // RANDTODO: Translate into french and german and replace GIMESSAGE_UNTRANSLATED
    // with GIMESSAGE(getItemID, itemID, english, german, french).
    const std::vector<GetItemMessage> getItemMessages = {
        GIMESSAGE(RG_ICE_TRAP, ITEM_NONE, "\x08\x06\x30You are a %bFOWL%w!",
                  "\x08\x06\x15 Du bist ein %bDUMMKOPF%w!", "\x08\x06\x50%bIDIOT%w"),
        GIMESSAGE_NO_GERMAN(
            RG_BOTTLE_WITH_BLUE_FIRE, ITEM_BLUE_FIRE, "You got a %rBottle with Blue &Fire%w! Use it to melt Red Ice!",
            "Vous obtenez une %rBouteille avec&une Flamme Bleue%w! Utilisez-la&pour faire fondre la %rGlace&Rouge%w!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTLE_WITH_BIG_POE, ITEM_BIG_POE,
                            "You got a %rBig Poe in a Bottle%w!&Sell it to the Ghost Shop!",
                            "Vous obtenez une %rBouteille avec&une Âme%w! Vendez-la au Marchand&d'Âme"),
        GIMESSAGE_NO_GERMAN(RG_BOTTLE_WITH_BLUE_POTION, ITEM_POTION_BLUE,
                            "You got a %rBottle of Blue Potion%w!&Drink it to replenish your&%ghealth%w and %bmagic%w!",
                            "Vous obtenez une %rBouteille avec&une Potion Bleue%w! Buvez-la pour&restaurer votre "
                            "%rénergie vitale%w&ainsi que votre %gmagie%w!"),
        GIMESSAGE_NO_GERMAN(
            RG_BOTTLE_WITH_FISH, ITEM_FISH,
            "You got a %rFish in a Bottle%w!&It looks fresh and delicious!&They say Jabu-Jabu loves them!",
            "Vous obtenez une %rBouteille avec&un Poisson%w! Il a l'air délicieux!&Il paraîtrait que %bJabu-Jabu "
            "%wen&serait friand!"),
        GIMESSAGE_NO_GERMAN(
            RG_BOTTLE_WITH_BUGS, ITEM_BUG, "You got a %rBug in a Bottle%w!&They love to burrow in&dirt holes!",
            "Vous obtenez une %rBouteille avec&des Insectes%w! Ils adorent creuser&dans la terre meuble!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTLE_WITH_FAIRY, ITEM_FAIRY, "You got a %rFairy in a Bottle%w!&Use it wisely!",
                            "Vous obtenez une %rBouteille avec&une Fée%w! Faites-en bon usage!"),
        GIMESSAGE_NO_GERMAN(
            RG_BOTTLE_WITH_RED_POTION, ITEM_POTION_RED,
            "You got a %rBottle of Red Potion%w!&Drink it to replenish your&%ghealth%w!",
            "Vous obtenez une %rBouteille avec&une Potion Rouge%w! Buvez-la pour&restaurer votre %rénergie vitale%w!"),
        GIMESSAGE_NO_GERMAN(
            RG_BOTTLE_WITH_GREEN_POTION, ITEM_POTION_GREEN,
            "You got a %rBottle of Green Potion%w!&Drink it to replenish your&%bmagic%w!",
            "Vous obtenez une %rBouteille avec&une Potion Verte%w! Buvez-la pour&restaurer votre %gmagie%w!"),
        GIMESSAGE_NO_GERMAN(
            RG_BOTTLE_WITH_POE, ITEM_POE,
            "You got a %rPoe in a Bottle%w!&That creepy Ghost Shop might&be interested in this...",
            "Vous obtenez une %rBouteille avec&un Esprit%w! Ça intéresserait&peut-être le vendeur d'Âme "),

        GIMESSAGE_NO_GERMAN(RG_GERUDO_FORTRESS_SMALL_KEY, ITEM_KEY_SMALL, "You found a %yThieves Hideout &%wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %yRepaire des Voleurs%w!"),
        GIMESSAGE_NO_GERMAN(RG_FOREST_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %gForest Temple &%wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %gTemple de la Forêt%w!"),
        GIMESSAGE_NO_GERMAN(RG_FIRE_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %rFire Temple &%wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %rTemple du Feu%w!"),
        GIMESSAGE_NO_GERMAN(RG_WATER_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %bWater Temple &%wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %bTemple de l'Eau%w!"),
        GIMESSAGE_NO_GERMAN(RG_SPIRIT_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %ySpirit Temple &%wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %yTemple de l'Esprit%w!"),
        GIMESSAGE_NO_GERMAN(RG_SHADOW_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %pShadow Temple &%wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %pTemple de l'Ombre%w!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTOM_OF_THE_WELL_SMALL_KEY, ITEM_KEY_SMALL,
                            "You found a %pBottom of the &Well %wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %Puits%w!"),
        GIMESSAGE_NO_GERMAN(RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, ITEM_KEY_SMALL,
                            "You found a %yGerudo Training &Grounds %wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %yGymnase Gerudo%w!"),
        GIMESSAGE_NO_GERMAN(RG_GANONS_CASTLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %rGanon's Castle &%wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %Château de Ganon%w!"),

        GIMESSAGE_NO_GERMAN(RG_FOREST_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %gForest Temple &%wBoss Key!",
                            "Vous obtenez la %rClé d'or %wdu&%gTemple de la Forêt%w!"),
        GIMESSAGE_NO_GERMAN(RG_FIRE_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %rFire Temple &%wBoss Key!",
                            "Vous obtenez la %rClé d'or %wdu&%rTemple du Feu%w!"),
        GIMESSAGE_NO_GERMAN(RG_WATER_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %bWater Temple &%wBoss Key!",
                            "Vous obtenez la %rClé d'or %wdu&%bTemple de l'Eau%w!"),
        GIMESSAGE_NO_GERMAN(RG_SPIRIT_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %ySpirit Temple &%wBoss Key!",
                            "Vous obtenez la %rClé d'or %wdu&%yTemple de l'Esprit%w!"),
        GIMESSAGE_NO_GERMAN(RG_SHADOW_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %pShadow Temple &%wBoss Key!",
                            "Vous obtenez la %rClé d'or %wdu&%pTemple de l'Ombre%w!"),
        GIMESSAGE_NO_GERMAN(RG_GANONS_CASTLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %rGanon's Castle &%wBoss Key!",
                            "Vous obtenez la %rClé d'or %wdu&%rChâteau de Ganon%w!"),

        GIMESSAGE_NO_GERMAN(RG_DEKU_TREE_MAP, ITEM_DUNGEON_MAP, "You found the %gDeku Tree &%wMap!",
                            "Vous obtenez la %rCarte %wde&l'%gArbre Mojo%w!"),
        GIMESSAGE_NO_GERMAN(RG_DODONGOS_CAVERN_MAP, ITEM_DUNGEON_MAP, "You found the %rDodongo's Cavern &%wMap!",
                            "Vous obtenez la %rCarte %wde la&%rCaverne Dodongo%w!"),
        GIMESSAGE_NO_GERMAN(RG_JABU_JABUS_BELLY_MAP, ITEM_DUNGEON_MAP, "You found the %bJabu Jabu's Belly &%wMap!",
                            "Vous obtenez la %rCarte %wdu &%bVentre de Jabu-Jabu%w!"),
        GIMESSAGE_NO_GERMAN(RG_FOREST_TEMPLE_MAP, ITEM_DUNGEON_MAP, "You found the %gForest Temple &%wMap!",
                            "Vous obtenez la %rCarte %wdu &%gTemple de la Forêt%w!"),
        GIMESSAGE_NO_GERMAN(RG_FIRE_TEMPLE_MAP, ITEM_DUNGEON_MAP, "You found the %rFire Temple &%wMap!",
                            "Vous obtenez la %rCarte %wdu &%rTemple du Feu%w!"),
        GIMESSAGE_NO_GERMAN(RG_WATER_TEMPLE_MAP, ITEM_DUNGEON_MAP, "You found the %bWater Temple &%wMap!",
                            "Vous obtenez la %rCarte %wdu &%bTemple de l'Eau%w!"),
        GIMESSAGE_NO_GERMAN(RG_SPIRIT_TEMPLE_MAP, ITEM_DUNGEON_MAP, "You found the %ySpirit Temple &%wMap!",
                            "Vous obtenez la %rCarte %wdu &%yTemple de l'Esprit%w!"),
        GIMESSAGE_NO_GERMAN(RG_SHADOW_TEMPLE_MAP, ITEM_DUNGEON_MAP, "You found the %pShadow Temple &%wMap!",
                            "Vous obtenez la %rCarte %wdu &%pTemple de l'Ombre%w!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTOM_OF_THE_WELL_MAP, ITEM_DUNGEON_MAP, "You found the %pBottom of the &Well %wMap!",
                            "Vous obtenez la %rCarte %wdu &%pPuits%w!"),
        GIMESSAGE_NO_GERMAN(RG_ICE_CAVERN_MAP, ITEM_DUNGEON_MAP, "You found the %cIce Cavern &%wMap!",
                            "Vous obtenez la %rCarte %wde &la %cCaverne Polaire%w!"),

        GIMESSAGE_NO_GERMAN(RG_DEKU_TREE_COMPASS, ITEM_COMPASS, "You found the %gDeku Tree &%wCompass!",
                            "Vous obtenez la %rBoussole %wde&l'%gArbre Mojo%w!"),
        GIMESSAGE_NO_GERMAN(RG_DODONGOS_CAVERN_COMPASS, ITEM_COMPASS, "You found the %rDodongo's Cavern &%wCompass!",
                            "Vous obtenez la %rBoussole %wde la&%rCaverne Dodongo%w!"),
        GIMESSAGE_NO_GERMAN(RG_JABU_JABUS_BELLY_COMPASS, ITEM_COMPASS, "You found the %bJabu Jabu's Belly &%wCompass!",
                            "Vous obtenez la %rBoussole %wdu &%bVentre de Jabu-Jabu%w!"),
        GIMESSAGE_NO_GERMAN(RG_FOREST_TEMPLE_COMPASS, ITEM_COMPASS, "You found the %gForest Temple &%wCompass!",
                            "Vous obtenez la %rBoussole %wdu &%gTemple de la Forêt%w!"),
        GIMESSAGE_NO_GERMAN(RG_FIRE_TEMPLE_COMPASS, ITEM_COMPASS, "You found the %rFire Temple &%wCompass!",
                            "Vous obtenez la %rBoussole %wdu &%rTemple du Feu%w!"),
        GIMESSAGE_NO_GERMAN(RG_WATER_TEMPLE_COMPASS, ITEM_COMPASS, "You found the %bWater Temple &%wCompass!",
                            "Vous obtenez la %rBoussole %wdu &%bTemple de l'Eau%w!"),
        GIMESSAGE_NO_GERMAN(RG_SPIRIT_TEMPLE_COMPASS, ITEM_COMPASS, "You found the %ySpirit Temple &%wCompass!",
                            "Vous obtenez la %rBoussole %wdu &%yTemple de l'Esprit%w!"),
        GIMESSAGE_NO_GERMAN(RG_SHADOW_TEMPLE_COMPASS, ITEM_COMPASS, "You found the %pShadow Temple &%wCompass!",
                            "Vous obtenez la %rBoussole %wdu &%pTemple de l'Ombre%w!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTOM_OF_THE_WELL_COMPASS, ITEM_COMPASS,
                            "You found the %pBottom of the &Well %wCompass!",
                            "Vous obtenez la %rBoussole %wdu &%pPuits%w!"),
        GIMESSAGE_NO_GERMAN(RG_ICE_CAVERN_COMPASS, ITEM_COMPASS, "You found the %cIce Cavern &%wCompass!",
                            "Vous obtenez la %rBoussole %wde &la %cCaverne Polaire%w!"),
        GIMESSAGE(RG_MAGIC_BEAN_PACK, ITEM_BEAN,
                            "You got a %rPack of Magic Beans%w!&Find a suitable spot for a garden&and plant them. Then, wait for&something fun to happen!",
                            "Du hast eine %rPackung&Magic Beans%w! Finde&einen geeigneten Platz fur einen&Garten und pflanze sie. Dann^warte auf etwas Lustiges passiert!",
                            "Vous avez un %rPack de&haricots magiques%w ! Trouvez&un endroit convenable pour un&jardin et plantez-les.^Ensuite, attendez quelque&chose d'amusant doit arriver !"),
        GIMESSAGE_NO_GERMAN(RG_TYCOON_WALLET, ITEM_WALLET_GIANT,
                            "You got a %rTycoon's Wallet%w!&It's gigantic! Now you can carry&up to %y999 rupees%w!",
                            "Vous obtenez la %rBourse de Magnat%w!&Elle peut contenir jusqu'à %y999 rubis%w!&C'est gigantesque!")
    };
    CreateGetItemMessages(getItemMessages);
    CreateRupeeMessages();
    CreateNaviRandoMessages();
}

class ExtendedVanillaTableInvalidItemIdException: public std::exception {
    private:
    s16 itemID;

    public:
      ExtendedVanillaTableInvalidItemIdException(s16 itemID): itemID(itemID) {}
      std::string what() {
        return itemID + " is not a valid ItemID for the extendedVanillaGetItemTable. If you are adding a new"
        "item, try adding it to randoGetItemTable instead.";
      }
};

void InitRandoItemTable() {
    // These entries have ItemIDs from vanilla, but not GetItemIDs or entries in the old sGetItemTable
    GetItemEntry extendedVanillaGetItemTable[] = {
        GET_ITEM(ITEM_MEDALLION_LIGHT, OBJECT_GI_MEDAL, GID_MEDALLION_LIGHT, 0x40, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_LIGHT_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_FOREST, OBJECT_GI_MEDAL, GID_MEDALLION_FOREST, 0x3E, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_FOREST_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_FIRE, OBJECT_GI_MEDAL, GID_MEDALLION_FIRE, 0x3C, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_FIRE_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_WATER, OBJECT_GI_MEDAL, GID_MEDALLION_WATER, 0x3D, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_WATER_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_SHADOW, OBJECT_GI_MEDAL, GID_MEDALLION_SHADOW, 0x41, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SHADOW_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_SPIRIT, OBJECT_GI_MEDAL, GID_MEDALLION_SPIRIT, 0x3F, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SPIRIT_MEDALLION),

        GET_ITEM(ITEM_KOKIRI_EMERALD, OBJECT_GI_JEWEL, GID_KOKIRI_EMERALD, 0x80, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_KOKIRI_EMERALD),
        GET_ITEM(ITEM_GORON_RUBY, OBJECT_GI_JEWEL, GID_GORON_RUBY, 0x81, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_GORON_RUBY),
        GET_ITEM(ITEM_ZORA_SAPPHIRE, OBJECT_GI_JEWEL, GID_ZORA_SAPPHIRE, 0x82, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_ZORA_SAPPHIRE),

        GET_ITEM(ITEM_SONG_LULLABY, OBJECT_GI_MELODY, GID_SONG_ZELDA, 0xD4, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_ZELDAS_LULLABY),
        GET_ITEM(ITEM_SONG_SUN, OBJECT_GI_MELODY, GID_SONG_SUN, 0xD3, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SUNS_SONG),
        GET_ITEM(ITEM_SONG_EPONA, OBJECT_GI_MELODY, GID_SONG_EPONA, 0xD2, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_EPONAS_SONG),
        GET_ITEM(ITEM_SONG_STORMS, OBJECT_GI_MELODY, GID_SONG_STORM, 0xD6, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SONG_OF_STORMS),
        GET_ITEM(ITEM_SONG_TIME, OBJECT_GI_MELODY, GID_SONG_TIME, 0xD5, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SONG_OF_TIME),
        GET_ITEM(ITEM_SONG_SARIA, OBJECT_GI_MELODY, GID_SONG_SARIA, 0xD1, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SARIAS_SONG),

        GET_ITEM(ITEM_SONG_MINUET, OBJECT_GI_MELODY, GID_SONG_MINUET, 0x73, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_MINUET_OF_FOREST),
        GET_ITEM(ITEM_SONG_BOLERO, OBJECT_GI_MELODY, GID_SONG_BOLERO, 0x74, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_BOLERO_OF_FIRE),
        GET_ITEM(ITEM_SONG_SERENADE, OBJECT_GI_MELODY, GID_SONG_SERENADE, 0x75, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SERENADE_OF_WATER),
        GET_ITEM(ITEM_SONG_NOCTURNE, OBJECT_GI_MELODY, GID_SONG_NOCTURNE, 0x77, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_NOCTURNE_OF_SHADOW),
        GET_ITEM(ITEM_SONG_REQUIEM, OBJECT_GI_MELODY, GID_SONG_REQUIEM, 0x76, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_REQUIEM_OF_SPIRIT),
        GET_ITEM(ITEM_SONG_PRELUDE, OBJECT_GI_MELODY, GID_SONG_PRELUDE, 0x78, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_PRELUDE_OF_LIGHT),
    };

    // These do not have ItemIDs or GetItemIDs from vanilla, so I'm using their
    // RandomizerGet enum values for both.
    GetItemEntry randoGetItemTable[] = {
        GET_ITEM(RG_ICE_TRAP, OBJECT_GI_RUPY, GID_RUPEE_GOLD, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_ICE_TRAP),
        GET_ITEM(RG_MAGIC_SINGLE, OBJECT_GI_MAGICPOT, GID_MAGIC_SMALL, 0xE4, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_MAGIC_SINGLE),
        GET_ITEM(RG_MAGIC_DOUBLE, OBJECT_GI_MAGICPOT, GID_MAGIC_LARGE, 0xE8, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_MAGIC_DOUBLE),
        GET_ITEM(RG_DOUBLE_DEFENSE, OBJECT_GI_HEARTS, GID_HEART_CONTAINER, 0xE9, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_DOUBLE_DEFENSE),
        GET_ITEM(RG_BOTTLE_WITH_RED_POTION, OBJECT_GI_LIQUID, GID_POTION_RED, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_RED_POTION),
        GET_ITEM(RG_BOTTLE_WITH_GREEN_POTION, OBJECT_GI_LIQUID, GID_POTION_GREEN, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_GREEN_POTION),
        GET_ITEM(RG_BOTTLE_WITH_BLUE_POTION, OBJECT_GI_LIQUID, GID_POTION_BLUE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_BLUE_POTION),
        GET_ITEM(RG_BOTTLE_WITH_FAIRY, OBJECT_GI_BOTTLE, GID_BOTTLE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_FAIRY),
        GET_ITEM(RG_BOTTLE_WITH_FISH, OBJECT_GI_FISH, GID_FISH, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_FISH),
        GET_ITEM(RG_BOTTLE_WITH_BLUE_FIRE, OBJECT_GI_FIRE, GID_BLUE_FIRE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_BLUE_FIRE),
        GET_ITEM(RG_BOTTLE_WITH_BUGS, OBJECT_GI_INSECT, GID_BUG, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_BUGS),
        GET_ITEM(RG_BOTTLE_WITH_POE, OBJECT_GI_GHOST, GID_POE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_POE),
        GET_ITEM(RG_BOTTLE_WITH_BIG_POE, OBJECT_GI_GHOST, GID_BIG_POE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_BIG_POE),
        GET_ITEM(RG_GERUDO_FORTRESS_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_GERUDO_FORTRESS_SMALL_KEY),
        GET_ITEM(RG_FOREST_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_FOREST_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_FIRE_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_FIRE_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_WATER_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_WATER_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_SPIRIT_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_SHADOW_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_SHADOW_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_BOTTOM_OF_THE_WELL_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_BOTTOM_OF_THE_WELL_SMALL_KEY),
        GET_ITEM(RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY),
        GET_ITEM(RG_GANONS_CASTLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_GANONS_CASTLE_SMALL_KEY),
        GET_ITEM(RG_FOREST_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FOREST_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_FIRE_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FIRE_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_WATER_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_WATER_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_SPIRIT_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_SHADOW_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SHADOW_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_GANONS_CASTLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_GANONS_CASTLE_BOSS_KEY),
        GET_ITEM(RG_DEKU_TREE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_DEKU_TREE_MAP),
        GET_ITEM(RG_DODONGOS_CAVERN_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_DODONGOS_CAVERN_MAP),
        GET_ITEM(RG_JABU_JABUS_BELLY_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_JABU_JABUS_BELLY_MAP),
        GET_ITEM(RG_FOREST_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FOREST_TEMPLE_MAP),
        GET_ITEM(RG_FIRE_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FIRE_TEMPLE_MAP),
        GET_ITEM(RG_WATER_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_WATER_TEMPLE_MAP),
        GET_ITEM(RG_SPIRIT_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_MAP),
        GET_ITEM(RG_SHADOW_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SHADOW_TEMPLE_MAP),
        GET_ITEM(RG_BOTTOM_OF_THE_WELL_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTOM_OF_THE_WELL_MAP),
        GET_ITEM(RG_ICE_CAVERN_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_ICE_CAVERN_MAP),
        GET_ITEM(RG_DEKU_TREE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_DEKU_TREE_COMPASS),
        GET_ITEM(RG_DODONGOS_CAVERN_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_DODONGOS_CAVERN_COMPASS),
        GET_ITEM(RG_JABU_JABUS_BELLY_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_JABU_JABUS_BELLY_COMPASS),
        GET_ITEM(RG_FOREST_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FOREST_TEMPLE_COMPASS),
        GET_ITEM(RG_FIRE_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FIRE_TEMPLE_COMPASS),
        GET_ITEM(RG_WATER_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_WATER_TEMPLE_COMPASS),
        GET_ITEM(RG_SPIRIT_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_COMPASS),
        GET_ITEM(RG_SHADOW_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SHADOW_TEMPLE_COMPASS),
        GET_ITEM(RG_BOTTOM_OF_THE_WELL_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTOM_OF_THE_WELL_COMPASS),
        GET_ITEM(RG_ICE_CAVERN_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_ICE_CAVERN_COMPASS),
        GET_ITEM(RG_MAGIC_BEAN_PACK, OBJECT_GI_BEAN, GID_BEAN, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_MAGIC_BEAN_PACK),
        GET_ITEM(RG_TYCOON_WALLET, OBJECT_GI_PURSE, GID_WALLET_GIANT, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_TYCOON_WALLET),
    };
    ItemTableManager::Instance->AddItemTable(MOD_RANDOMIZER);
    for (int i = 0; i < ARRAY_COUNT(extendedVanillaGetItemTable); i++) {
        ItemTableManager::Instance->AddItemEntry(MOD_RANDOMIZER, extendedVanillaGetItemTable[i].getItemId, extendedVanillaGetItemTable[i]);
    }
    for (int i = 0; i < ARRAY_COUNT(randoGetItemTable); i++) {
        if (randoGetItemTable[i].itemId >= RG_FOREST_TEMPLE_SMALL_KEY && randoGetItemTable[i].itemId <= RG_GANONS_CASTLE_SMALL_KEY
            && randoGetItemTable[i].itemId != RG_GERUDO_FORTRESS_SMALL_KEY) {
            randoGetItemTable[i].drawFunc = (CustomDrawFunc)Randomizer_DrawSmallKey;
        } else if (randoGetItemTable[i].itemId >= RG_FOREST_TEMPLE_BOSS_KEY && randoGetItemTable[i].itemId <= RG_GANONS_CASTLE_BOSS_KEY) {
            randoGetItemTable[i].drawFunc = (CustomDrawFunc)Randomizer_DrawBossKey;
        } else if (randoGetItemTable[i].itemId == RG_DOUBLE_DEFENSE) {
            randoGetItemTable[i].drawFunc = (CustomDrawFunc)Randomizer_DrawDoubleDefense;
        }
        ItemTableManager::Instance->AddItemEntry(MOD_RANDOMIZER, randoGetItemTable[i].itemId, randoGetItemTable[i]);
    }
}


void InitRando() {
    SohImGui::AddWindow("Randomizer", "Randomizer Settings", DrawRandoEditor);
    Randomizer::CreateCustomMessages();
    InitRandoItemTable();
}

extern "C" {

void Rando_Init(void) {
    InitRando();
}

}
