#include "randomizer.h"
#include <nlohmann/json.hpp>
#include "3drando/settings.hpp"
#include <fstream>
#include <variables.h>
#include <macros.h>
#include <objects/gameplay_keep/gameplay_keep.h>
#include <functions.h>
#include <Cvar.h>
#include <textures/icon_item_static/icon_item_static.h>
#include <textures/icon_item_24_static/icon_item_24_static.h>
#include <ImGuiImpl.h>
#include <thread>
#include "3drando/rando_main.hpp"
#include "3drando/random.hpp"
#include "../../UIWidgets.hpp"
#include <ImGui/imgui_internal.h>
#include "../custom-message/CustomMessageTypes.h"
#include "../item-tables/ItemTableManager.h"
#include "../presets.h"
#include "../../../src/overlays/actors/ovl_En_GirlA/z_en_girla.h"
#include <stdexcept>
#include "randomizer_check_objects.h"
#include <sstream>
#include <tuple>
#include <functional>
#include "draw.h"
#include "rando_hash.h"

extern "C" uint32_t ResourceMgr_IsGameMasterQuest();
extern "C" uint32_t ResourceMgr_IsSceneMasterQuest(s16 sceneNum);

using json = nlohmann::json;
using namespace std::literals::string_literals;

std::unordered_map<std::string, RandomizerCheck> SpoilerfileCheckNameToEnum;
std::unordered_map<std::string, RandomizerGet> SpoilerfileGetNameToEnum;
std::multimap<std::tuple<s16, s16, s32>, RandomizerCheckObject> checkFromActorMultimap;
std::set<RandomizerCheck> excludedLocations;

u8 generated;

const std::string Randomizer::getItemMessageTableID = "Randomizer";
const std::string Randomizer::hintMessageTableID = "RandomizerHints";
const std::string Randomizer::merchantMessageTableID = "RandomizerMerchants";
const std::string Randomizer::rupeeMessageTableID = "RandomizerRupees";
const std::string Randomizer::NaviRandoMessageTableID = "RandomizerNavi";
const std::string Randomizer::IceTrapRandoMessageTableID = "RandomizerIceTrap";

static const char* englishRupeeNames[81] = {
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
    "Store Credit", "Social Credit", "Cocoa Beans",  "Strawbs"
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
    for (auto [randomizerCheck, rcObject] : RandomizerCheckObjects::GetAllRCObjects()) {
        SpoilerfileCheckNameToEnum[rcObject.rcSpoilerName] = rcObject.rc;
        checkFromActorMultimap.emplace(std::make_tuple((s16)rcObject.actorId, (s16)rcObject.sceneId, rcObject.actorParams), rcObject);
    }
    SpoilerfileCheckNameToEnum["Invalid Location"] = RC_UNKNOWN_CHECK;
    SpoilerfileCheckNameToEnum["Link's Pocket"] = RC_LINKS_POCKET;

    for (auto& item: *RandoMain::GetFullItemTable()) {
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

std::unordered_map<std::string, SceneID> spoilerFileDungeonToScene = {
    { "Deku Tree", SCENE_YDAN },
    { "Dodongo's Cavern", SCENE_DDAN },
    { "Jabu Jabu's Belly", SCENE_BDAN },
    { "Forest Temple", SCENE_BMORI1 },
    { "Fire Temple", SCENE_HIDAN },
    { "Water Temple", SCENE_MIZUSIN },
    { "Spirit Temple", SCENE_JYASINZOU },
    { "Shadow Temple", SCENE_HAKADAN },
    { "Bottom of the Well", SCENE_HAKADANCH },
    { "Ice Cavern", SCENE_ICE_DOUKUTO },
    { "Gerudo Training Grounds", SCENE_MEN },
    { "Ganon's Castle", SCENE_GANONTIKA }
};

std::unordered_map<s16, s16>
    getItemIdToItemId = { { GI_BOW, ITEM_BOW },
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
                          { GI_CLAIM_CHECK, ITEM_CLAIM_CHECK } };

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
    { "Shuffle Settings:Link's Pocket", RSK_LINKS_POCKET},
    { "Shuffle Settings:Shuffle Gerudo Card", RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD },
    { "Shuffle Settings:Shopsanity", RSK_SHOPSANITY },
    { "Shuffle Settings:Scrub Shuffle", RSK_SHUFFLE_SCRUBS },
    { "Shuffle Settings:Shuffle Cows", RSK_SHUFFLE_COWS },
    { "Shuffle Settings:Tokensanity", RSK_SHUFFLE_TOKENS },
    { "Shuffle Settings:Shuffle Adult Trade", RSK_SHUFFLE_ADULT_TRADE },
    { "Shuffle Settings:Shuffle Magic Beans", RSK_SHUFFLE_MAGIC_BEANS },
    { "Shuffle Settings:Shuffle Kokiri Sword", RSK_SHUFFLE_KOKIRI_SWORD },
    { "Shuffle Settings:Shuffle Weird Egg", RSK_SHUFFLE_WEIRD_EGG },
    { "Shuffle Settings:Shuffle Frog Song Rupees", RSK_SHUFFLE_FROG_SONG_RUPEES },
    { "Shuffle Settings:Shuffle Merchants", RSK_SHUFFLE_MERCHANTS },
    { "Start with Deku Shield", RSK_STARTING_DEKU_SHIELD },
    { "Start with Kokiri Sword", RSK_STARTING_KOKIRI_SWORD },
    { "Start with Fairy Ocarina", RSK_STARTING_OCARINA },
    { "Shuffle Dungeon Items:Maps/Compasses", RSK_STARTING_MAPS_COMPASSES },
    { "Shuffle Dungeon Items:Small Keys", RSK_KEYSANITY },
    { "Shuffle Dungeon Items:Gerudo Fortress Keys", RSK_GERUDO_KEYS },
    { "Shuffle Dungeon Items:Boss Keys", RSK_BOSS_KEYSANITY },
    { "Shuffle Dungeon Items:Ganon's Boss Key", RSK_GANONS_BOSS_KEY },
    { "Shuffle Dungeon Items:Medallion Count", RSK_LACS_MEDALLION_COUNT },
    { "Shuffle Dungeon Items:Stone Count", RSK_LACS_STONE_COUNT },
    { "Shuffle Dungeon Items:Reward Count", RSK_LACS_REWARD_COUNT },
    { "Shuffle Dungeon Items:Dungeon Count", RSK_LACS_DUNGEON_COUNT },
    { "Shuffle Dungeon Items:Token Count", RSK_LACS_TOKEN_COUNT },
    { "World Settings:Starting Age", RSK_STARTING_AGE },
    { "World Settings:Ammo Drops", RSK_ENABLE_BOMBCHU_DROPS },
    { "World Settings:Bombchus in Logic", RSK_BOMBCHUS_IN_LOGIC },
    { "World Settings:Shuffle Entrances", RSK_SHUFFLE_ENTRANCES },
    { "World Settings:Dungeon Entrances", RSK_SHUFFLE_DUNGEON_ENTRANCES },
    { "World Settings:Overworld Entrances", RSK_SHUFFLE_OVERWORLD_ENTRANCES },
    { "World Settings:Interior Entrances", RSK_SHUFFLE_INTERIOR_ENTRANCES },
    { "World Settings:Grottos Entrances", RSK_SHUFFLE_GROTTO_ENTRANCES },
    { "World Settings:Owl Drops", RSK_SHUFFLE_OWL_DROPS },
    { "World Settings:Warp Songs", RSK_SHUFFLE_WARP_SONGS },
    { "World Settings:Overworld Spawns", RSK_SHUFFLE_OVERWORLD_SPAWNS },
    { "World Settings:Mixed Entrance Pools", RSK_MIXED_ENTRANCE_POOLS },
    { "World Settings:Mix Dungeons", RSK_MIX_DUNGEON_ENTRANCES },
    { "World Settings:Mix Overworld", RSK_MIX_OVERWORLD_ENTRANCES },
    { "World Settings:Mix Interiors", RSK_MIX_INTERIOR_ENTRANCES },
    { "World Settings:Mix Grottos", RSK_MIX_GROTTO_ENTRANCES },
    { "World Settings:Decouple Entrances", RSK_DECOUPLED_ENTRANCES },
    { "Misc Settings:Gossip Stone Hints", RSK_GOSSIP_STONE_HINTS },
    { "Misc Settings:Hint Clarity", RSK_HINT_CLARITY },
    { "Misc Settings:Hint Distribution", RSK_HINT_DISTRIBUTION },
    { "Misc Settings:Blue Fire Arrows", RSK_BLUE_FIRE_ARROWS },
    { "Misc Settings:Sunlight Arrows", RSK_SUNLIGHT_ARROWS },
    { "Skip Child Zelda", RSK_SKIP_CHILD_ZELDA },
    { "Start with Consumables", RSK_STARTING_CONSUMABLES },
    { "Start with Max Rupees", RSK_FULL_WALLETS },
    { "Gold Skulltula Tokens", RSK_STARTING_SKULLTULA_TOKEN },
    { "Timesaver Settings:Cuccos to return", RSK_CUCCO_COUNT },
    { "Timesaver Settings:Big Poe Target Count", RSK_BIG_POE_COUNT },
    { "Timesaver Settings:Skip Child Stealth", RSK_SKIP_CHILD_STEALTH },
    { "Timesaver Settings:Skip Epona Race", RSK_SKIP_EPONA_RACE },
    { "Timesaver Settings:Skip Tower Escape", RSK_SKIP_TOWER_ESCAPE },
    { "Timesaver Settings:Complete Mask Quest", RSK_COMPLETE_MASK_QUEST },
    { "Timesaver Settings:Skip Scarecrow's Song", RSK_SKIP_SCARECROWS_SONG },
    { "Timesaver Settings:Enable Glitch-Useful Cutscenes", RSK_ENABLE_GLITCH_CUTSCENES },
    { "World Settings:MQ Dungeon Count", RSK_MQ_DUNGEON_COUNT }
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

    CustomMessageManager::Instance->CreateMessage(Randomizer::hintMessageTableID, TEXT_WARP_RANDOM_REPLACED_TEXT,
        { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM,
        "Warp to&{{location}}?\x1B&%gOK&No%w\x02",
        "Zu {{location}}?\x1B&%gOK&No%w\x02",
        "Se téléporter vers&{{location}}?\x1B&%gOK!&Non%w\x02" });

    CustomMessageManager::Instance->CreateMessage(Randomizer::hintMessageTableID, TEXT_LAKE_HYLIA_WATER_SWITCH_SIGN,
        { TEXTBOX_TYPE_WOODEN, TEXTBOX_POS_BOTTOM,
            "Water level control system.&Keep away!",
            "Wasserstand Kontrollsystem&Finger weg!",
            "Système de contrôle du niveau&d'eau.&Ne pas toucher!"
        });
    CustomMessageManager::Instance->CreateMessage(Randomizer::hintMessageTableID, TEXT_LAKE_HYLIA_WATER_SWITCH_NAVI,
        { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM,
            "%cThis switch is rustier than you think.^%cSomething must be wrong with the&pipe system in the %bWater Temple%c.",
            "%cDieser Schalter scheint rostiger zu&sein als er aussieht.^%cEtwas muss mit dem Leitungssystem&im %bWassertempel%c nicht stimmen.",
            "%cCet interrupteur est très rouillé.^%cIl doit y avoir un problème avec&la tuyauterie du %bTemple de l'Eau%c."
        });
}

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
    CustomMessageManager::Instance->ClearMessageTable(Randomizer::merchantMessageTableID);
    CustomMessageManager::Instance->AddCustomMessageTable(Randomizer::merchantMessageTableID);

    // Prices have a chance of being 0, and the "sell" message below doesn't really make sense for a free item, so adding a "free" variation here
    CustomMessageManager::Instance->CreateMessage(Randomizer::merchantMessageTableID, TEXT_SCRUB_RANDOM_FREE,
        { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM,
            "\x12\x38\x82" "All right! You win! In return for&sparing me, I will give you a&%g{{item}}%w!&Please, take it!\x07\x10\xA3",
            "\x12\x38\x82" "In Ordnung! Du gewinnst! Im Austausch&dafür, dass du mich verschont hast,&werde ich dir einen &%g{{item}}%w geben!\x07\x10\xA3",
            "\x12\x38\x82" "J'me rends! Laisse-moi partir et en&échange, je te donne un &%g{{item}}%w! Vas-y prends le!\x07\x10\xA3",
        });
    CustomMessageManager::Instance->CreateMessage(Randomizer::merchantMessageTableID, TEXT_SCRUB_RANDOM,
        { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM,
            "\x12\x38\x82" "All right! You win! In return for&sparing me, I will sell you a&%g{{item}}%w!&%r{{price}} Rupees%w it is!\x07\x10\xA3",
            "\x12\x38\x82" "Aufgeben! Ich verkaufe dir einen&%g{{item}}%w&für %r{{price}} Rubine%w!\x07\x10\xA3",
            "\x12\x38\x82" "J'abandonne! Tu veux bien m'acheter&un %g{{item}}%w?&Ça fera %r{{price}} Rubis%w!\x07\x10\xA3"
        });
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


    //Setup for merchant text boxes
    //Medigoron
    //RANDOTODO: Implement obscure/ambiguous hints
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::merchantMessageTableID, TEXT_MEDIGORON,
        {
            TEXTBOX_TYPE_BLACK,
            TEXTBOX_POS_BOTTOM,
            "How about buying %r&{{item}}%w for %g200 rupees%w?\x1B&%gYes&No%w",
            "Wie wäre es mit %r&{{item}}%w für %g200 Rubine?%w\x1B&%gJa!&Nein!%w",
            "Veux-tu acheter %r&{{item}}%w pour %g200 rubis?%w\x1B&%gOui&Non&w" 
        });
    
    //Carpet Salesman
    //RANDOTODO: Implement obscure/ambiguous hints
    std::vector<std::string> cgBoxTwoText;
    if (Randomizer::GetRandoSettingValue(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_ON_HINT) {
        cgBoxTwoText = {
            "!%w&It's real, I promise!&A lonely man such as myself&wouldn't %rlie%w to you, hmm?^",
            "!%w&Ich kann versichern es ist ein&aufrichtiges Angebot!^Ein einsamer Mann wie ich würde dich&doch nicht %ranlügen%w, oder?^",
            "!%w&C'est vrai! J'te jure!&Un gars comme moi ne te %rmentirai%w pas&tu ne crois pas?^"
        };
    } else {
        cgBoxTwoText = {
            "!%w&I won't tell you what it is until I see&the money...^",
            "!%w&Erst kommt das Geld, dann die Ware...^",
            "!%w&Je ne te dirai pas ce que c'est avant&d'être payé rubis sur l'ongle...^"
            };
        }
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::merchantMessageTableID, TEXT_CARPET_SALESMAN_1,
        {
            TEXTBOX_TYPE_BLACK,
            TEXTBOX_POS_BOTTOM,
            "Welcome!^I am selling stuff, strange and rare, &from all over the world to everybody.&Today's special is...^%r{{item}}" + cgBoxTwoText[0] +
            "How about %g200 Rupees?%w\x1B&&%gYes&No%w",
            "Sei gegrüßt!^Ich verkaufe allerlei Kuriorisäten.&Stets sonderliche und seltene Ware&aus aller Welt für jedermann.&Das heutige Angebot bleibt...^%r{{item}}" + 
            cgBoxTwoText[1] + "Wie wäre es mit %g200 Rubinen?%w\x1B&&%gJa!&Nein!%w",
            "Bienvenue!^Je vends des trucs étranges et rares,&de partout dans le monde et à tout le&monde! L'objet du jour est...^%r{{item}}" + 
            cgBoxTwoText[2] + "Alors, marché conclu pour %g200 rubis?%w\x1B&&%gOui&Non%w"
        }
    );

    CustomMessageManager::Instance->CreateMessage(
        Randomizer::merchantMessageTableID, TEXT_CARPET_SALESMAN_2,
        {
            TEXTBOX_TYPE_BLACK,
            TEXTBOX_POS_TOP,
            "Finally! Now I can go back to being &an %rarms dealer%w!",
            "Endlich! Schon bald kann ich wieder &%rKrabbelminen-Händler%w sein!",
            "Squalala! Je vais enfin pouvoir &%rprendre des vacances%w!"
        }
    );

    // Each shop item has two messages, one for when the cursor is over it, and one for when you select it and are
    // prompted buy/don't buy
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::merchantMessageTableID, TEXT_SHOP_ITEM_RANDOM, { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_VARIABLE,
            "\x08%r{{item}}  {{price}} Rupees&%wSpecial deal! ONE LEFT!&Get it while it lasts!\x0A\x02",
            "\x08%r{{item}}  {{price}} Rubine&%wSonderangebot! NUR NOCH EINES VERFÜGBAR!&Beeilen Sie sich!\x0A\x02",
            "\x08%r{{item}}  {{price}} Rubis&%wOffre spéciale! DERNIER EN STOCK!&Faites vite!\x0A\x02",
        });
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::merchantMessageTableID, TEXT_SHOP_ITEM_RANDOM_CONFIRM, { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_VARIABLE,
            "\x08{{item}}  {{price}} Rupees\x09&&\x1B%gBuy&Don't buy%w\x09\x02",
            "\x08{{item}}  {{price}} Rubine\x09&&\x1B%gKaufen&Nicht kaufen%w\x09\x02",
            "\x08{{item}}  {{price}} Rubis\x09&&\x1B%gAcheter&Ne pas acheter%w\x09\x02",
        });
}

void Randomizer::LoadItemLocations(const char* spoilerFileName, bool silent) {
    if (strcmp(spoilerFileName, "") != 0) {
        ParseItemLocationsFile(spoilerFileName, silent);
    }

    for (auto itemLocation : gSaveContext.itemLocations) {
        this->itemLocations[itemLocation.check] = itemLocation.get;
    }

    itemLocations[RC_UNKNOWN_CHECK].rgID = itemLocations[RC_UNKNOWN_CHECK].fakeRgID = RG_NONE;
}

void Randomizer::LoadRequiredTrials(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        ParseRequiredTrialsFile(spoilerFileName);
    }
}

void Randomizer::LoadEntranceOverrides(const char* spoilerFileName, bool silent){
    if (strcmp(spoilerFileName, "") != 0) {
        ParseEntranceDataFile(spoilerFileName, silent);
    }
}

void Randomizer::LoadMasterQuestDungeons(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        ParseMasterQuestDungeonsFile(spoilerFileName);
    }
    gSaveContext.mqDungeonCount = this->masterQuestDungeons.size();
}

void Randomizer::ParseRandomizerSettingsFile(const char* spoilerFileName) {
    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream)
        return;

    bool success = false;

    try {
        // clear out existing settings
        for(size_t i = 0; i < RSK_MAX; i++) {
            gSaveContext.randoSettings[i].key = RSK_NONE;
            gSaveContext.randoSettings[i].value = 0;
        }

        json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;
        json settingsJson = spoilerFileJson["settings"];

        for (auto it = settingsJson.begin(); it != settingsJson.end(); ++it) {
            // todo load into cvars for UI
            
            std::string numericValueString;
            if(SpoilerfileSettingNameToEnum.count(it.key())) {
                RandomizerSettingKey index = SpoilerfileSettingNameToEnum[it.key()];
                gSaveContext.randoSettings[index].key = SpoilerfileSettingNameToEnum[it.key()];
                // this is annoying but the same strings are used in different orders
                // and i don't want the spoilerfile to just have numbers instead of
                // human readable settings values so it'll have to do for now
                switch(gSaveContext.randoSettings[index].key) {
                    case RSK_FOREST:
                        if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = RO_FOREST_CLOSED;            
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = RO_FOREST_OPEN;
                        } else if(it.value() == "Closed Deku") {
                            gSaveContext.randoSettings[index].value = RO_FOREST_CLOSED_DEKU;
                        }
                        break;
                    case RSK_KAK_GATE:
                        if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = RO_KAK_GATE_CLOSED;            
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = RO_KAK_GATE_OPEN;
                        }
                        break;
                    case RSK_DOOR_OF_TIME:
                        if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = RO_DOOROFTIME_OPEN;            
                        } else if(it.value() == "Song only") {
                            gSaveContext.randoSettings[index].value = RO_DOOROFTIME_SONGONLY;
                        } else if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = RO_DOOROFTIME_CLOSED;
                        }
                        break;
                    case RSK_ZORAS_FOUNTAIN:
                        if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = RO_ZF_CLOSED;            
                        } else if(it.value() == "Closed as child") {
                            gSaveContext.randoSettings[index].value = RO_ZF_CLOSED_CHILD;
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = RO_ZF_OPEN;
                        }
                        break;
                    case RSK_STARTING_AGE:
                        if(it.value() == "Child") {
                            gSaveContext.randoSettings[index].value = RO_AGE_CHILD;
                        } else if (it.value() == "Adult") {
                            gSaveContext.randoSettings[index].value = RO_AGE_ADULT;
                        }
                        break;
                    case RSK_GERUDO_FORTRESS:
                        if(it.value() == "Normal") {
                            gSaveContext.randoSettings[index].value = RO_GF_NORMAL;            
                        } else if(it.value() == "Fast") {
                            gSaveContext.randoSettings[index].value = RO_GF_FAST;
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = RO_GF_OPEN;
                        }
                        break;
                    case RSK_RAINBOW_BRIDGE:
                        if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = RO_BRIDGE_VANILLA;
                        } else if(it.value() == "Always open") {
                            gSaveContext.randoSettings[index].value = RO_BRIDGE_ALWAYS_OPEN;            
                        } else if(it.value() == "Stones") {
                            gSaveContext.randoSettings[index].value = RO_BRIDGE_STONES;
                        } else if(it.value() == "Medallions") {
                            gSaveContext.randoSettings[index].value = RO_BRIDGE_MEDALLIONS;
                        } else if(it.value() == "Dungeon rewards") {
                            gSaveContext.randoSettings[index].value = RO_BRIDGE_DUNGEON_REWARDS;
                        } else if(it.value() == "Dungeons") {
                            gSaveContext.randoSettings[index].value = RO_BRIDGE_DUNGEONS;
                        } else if(it.value() == "Tokens") {
                            gSaveContext.randoSettings[index].value = RO_BRIDGE_TOKENS;
                        }
                        break;
                    case RSK_RAINBOW_BRIDGE_STONE_COUNT:
                    case RSK_RAINBOW_BRIDGE_MEDALLION_COUNT:
                    case RSK_RAINBOW_BRIDGE_REWARD_COUNT:
                    case RSK_RAINBOW_BRIDGE_DUNGEON_COUNT:
                    case RSK_RAINBOW_BRIDGE_TOKEN_COUNT:
                    case RSK_LACS_STONE_COUNT:
                    case RSK_LACS_MEDALLION_COUNT:
                    case RSK_LACS_REWARD_COUNT:
                    case RSK_LACS_DUNGEON_COUNT:
                    case RSK_LACS_TOKEN_COUNT:
                    case RSK_BIG_POE_COUNT:
                    case RSK_CUCCO_COUNT:
                    case RSK_STARTING_SKULLTULA_TOKEN:
                        numericValueString = it.value();
                        gSaveContext.randoSettings[index].value = std::stoi(numericValueString);
                        break;
                    case RSK_SHOPSANITY:
                        if(it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = RO_SHOPSANITY_OFF;         
                        } else if(it.value() == "0 Items") {
                            gSaveContext.randoSettings[index].value = RO_SHOPSANITY_ZERO_ITEMS;
                        } else if(it.value() == "1 Item") {
                            gSaveContext.randoSettings[index].value = RO_SHOPSANITY_ONE_ITEM;
                        } else if(it.value() == "2 Items") {
                            gSaveContext.randoSettings[index].value = RO_SHOPSANITY_TWO_ITEMS;
                        } else if(it.value() == "3 Items") {
                            gSaveContext.randoSettings[index].value = RO_SHOPSANITY_THREE_ITEMS;
                        } else if(it.value() == "4 Items") {
                            gSaveContext.randoSettings[index].value = RO_SHOPSANITY_FOUR_ITEMS;
                        } else if(it.value() == "Random") {
                            gSaveContext.randoSettings[index].value = RO_SHOPSANITY_RANDOM;
                        }
                        break;
                    case RSK_SHUFFLE_SCRUBS:
                        if(it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = RO_SCRUBS_OFF;            
                        } else if(it.value() == "Affordable") {
                            gSaveContext.randoSettings[index].value = RO_SCRUBS_AFFORDABLE;
                        } else if(it.value() == "Expensive") {
                            gSaveContext.randoSettings[index].value = RO_SCRUBS_EXPENSIVE;
                        } else if(it.value() == "Random Prices") {
                            gSaveContext.randoSettings[index].value = RO_SCRUBS_RANDOM;
                        }
                        break;
                    case RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD:
                    case RSK_SHUFFLE_COWS:
                    case RSK_SHUFFLE_ADULT_TRADE:
                    case RSK_SHUFFLE_MAGIC_BEANS:
                    case RSK_SHUFFLE_KOKIRI_SWORD:
                    case RSK_SHUFFLE_WEIRD_EGG:
                    case RSK_SHUFFLE_FROG_SONG_RUPEES:
                    case RSK_RANDOM_MQ_DUNGEONS:
                    case RSK_STARTING_DEKU_SHIELD:
                    case RSK_STARTING_KOKIRI_SWORD:
                    case RSK_COMPLETE_MASK_QUEST:
                    case RSK_SKIP_SCARECROWS_SONG:
                    case RSK_ENABLE_GLITCH_CUTSCENES:
                    case RSK_BLUE_FIRE_ARROWS:
                    case RSK_SUNLIGHT_ARROWS:
                    case RSK_BOMBCHUS_IN_LOGIC:
                    case RSK_SHUFFLE_ENTRANCES:
                    case RSK_SHUFFLE_OVERWORLD_ENTRANCES:
                    case RSK_SHUFFLE_GROTTO_ENTRANCES:
                    case RSK_SHUFFLE_OWL_DROPS:
                    case RSK_SHUFFLE_WARP_SONGS:
                    case RSK_SHUFFLE_OVERWORLD_SPAWNS:
                    case RSK_MIXED_ENTRANCE_POOLS:
                    case RSK_MIX_DUNGEON_ENTRANCES:
                    case RSK_MIX_OVERWORLD_ENTRANCES:
                    case RSK_MIX_INTERIOR_ENTRANCES:
                    case RSK_MIX_GROTTO_ENTRANCES:
                    case RSK_DECOUPLED_ENTRANCES:
                        if(it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = RO_GENERIC_OFF;            
                        } else if(it.value() == "On") {
                            gSaveContext.randoSettings[index].value = RO_GENERIC_ON;
                        }
                        break;
                    case RSK_SHUFFLE_MERCHANTS:
                        if(it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = RO_SHUFFLE_MERCHANTS_OFF;
                        } else if (it.value() == "On (No Hints)") {
                            gSaveContext.randoSettings[index].value = RO_SHUFFLE_MERCHANTS_ON_NO_HINT;
                        } else if (it.value() == "On (With Hints)") {
                            gSaveContext.randoSettings[index].value = RO_SHUFFLE_MERCHANTS_ON_HINT;
                        }
                        break;
                    // Uses Ammo Drops option for now. "Off" not yet implemented
                    case RSK_ENABLE_BOMBCHU_DROPS:
                        if (it.value() == "On") {
                            gSaveContext.randoSettings[index].value = RO_AMMO_DROPS_ON;
                        } else if (it.value() == "On + Bombchu") {
                            gSaveContext.randoSettings[index].value = RO_AMMO_DROPS_ON_PLUS_BOMBCHU;
                        } else if (it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = RO_AMMO_DROPS_OFF;
                        }
                        break;
                    case RSK_STARTING_MAPS_COMPASSES:
                        if(it.value() == "Start With") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_STARTWITH;            
                        } else if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_VANILLA;
                        } else if(it.value() == "Own Dungeon") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_OWN_DUNGEON;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_ANY_DUNGEON;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_OVERWORLD;
                        } else if(it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_ANYWHERE;
                        }
                        break;
                    case RSK_STARTING_OCARINA:
                        if(it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = RO_STARTING_OCARINA_OFF;            
                        } else if(it.value() == "Fairy Ocarina") {
                            gSaveContext.randoSettings[index].value = RO_STARTING_OCARINA_FAIRY;
                        }
                        break;
                    case RSK_ITEM_POOL:
                        if(it.value() == "Plentiful") {
                            gSaveContext.randoSettings[index].value = RO_ITEM_POOL_PLENTIFUL;
                        } else if(it.value() == "Balanced") {
                            gSaveContext.randoSettings[index].value = RO_ITEM_POOL_BALANCED;            
                        } else if(it.value() == "Scarce") {
                            gSaveContext.randoSettings[index].value = RO_ITEM_POOL_SCARCE;
                        } else if(it.value() == "Minimal") {
                            gSaveContext.randoSettings[index].value = RO_ITEM_POOL_MINIMAL;
                        }
                    case RSK_ICE_TRAPS:
                        if(it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = RO_ICE_TRAPS_OFF;
                        } else if(it.value() == "Normal") {
                            gSaveContext.randoSettings[index].value = RO_ICE_TRAPS_NORMAL;            
                        } else if(it.value() == "Extra") {
                            gSaveContext.randoSettings[index].value = RO_ICE_TRAPS_EXTRA;
                        } else if(it.value() == "Mayhem") {
                            gSaveContext.randoSettings[index].value = RO_ICE_TRAPS_MAYHEM;
                        } else if(it.value() == "Onslaught") {
                            gSaveContext.randoSettings[index].value = RO_ICE_TRAPS_ONSLAUGHT;
                        }
                    case RSK_GOSSIP_STONE_HINTS:
                        if(it.value() == "No Hints") {
                            gSaveContext.randoSettings[index].value = RO_GOSSIP_STONES_NONE;            
                        } else if(it.value() == "Need Nothing") {
                            gSaveContext.randoSettings[index].value = RO_GOSSIP_STONES_NEED_NOTHING;
                        } else if(it.value() == "Mask of Truth") {
                            gSaveContext.randoSettings[index].value = RO_GOSSIP_STONES_NEED_TRUTH;
                        } else if(it.value() == "Stone of Agony") {
                            gSaveContext.randoSettings[index].value = RO_GOSSIP_STONES_NEED_STONE;
                        }
                        break;
                    case RSK_HINT_CLARITY:
                        if(it.value() == "Obscure") {
                            gSaveContext.randoSettings[index].value = RO_HINT_CLARITY_OBSCURE;            
                        } else if(it.value() == "Ambiguous") {
                            gSaveContext.randoSettings[index].value = RO_HINT_CLARITY_AMBIGUOUS;
                        } else if(it.value() == "Clear") {
                            gSaveContext.randoSettings[index].value = RO_HINT_CLARITY_CLEAR;
                        }
                        break;
                    case RSK_HINT_DISTRIBUTION:
                        if(it.value() == "Useless") {
                            gSaveContext.randoSettings[index].value = RO_HINT_DIST_USELESS;            
                        } else if(it.value() == "Balanced") {
                            gSaveContext.randoSettings[index].value = RO_HINT_DIST_BALANCED;
                        } else if(it.value() == "Strong") {
                            gSaveContext.randoSettings[index].value = RO_HINT_DIST_STRONG;
                        } else if(it.value() == "Very Strong") {
                            gSaveContext.randoSettings[index].value = RO_HINT_DIST_VERY_STRONG;
                        }
                        break;
                    case RSK_GERUDO_KEYS:
                        if (it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = RO_GERUDO_KEYS_VANILLA;
                        } else if (it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = RO_GERUDO_KEYS_ANY_DUNGEON;
                        } else if (it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = RO_GERUDO_KEYS_OVERWORLD;
                        } else if (it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = RO_GERUDO_KEYS_ANYWHERE;
                        }
                        break;
                    case RSK_KEYSANITY:
                        if(it.value() == "Start With") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_STARTWITH;            
                        } else if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_VANILLA;
                        } else if(it.value() == "Own Dungeon") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_OWN_DUNGEON;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_ANY_DUNGEON;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_OVERWORLD;
                        } else if(it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_ANYWHERE;
                        }
                        break;
                    case RSK_BOSS_KEYSANITY:
                        if(it.value() == "Start With") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_STARTWITH;            
                        } else if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_VANILLA;
                        } else if(it.value() == "Own Dungeon") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_OWN_DUNGEON;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_ANY_DUNGEON;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_OVERWORLD;
                        } else if(it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ITEM_LOC_ANYWHERE;
                        }
                        break;
                    case RSK_GANONS_BOSS_KEY:
                        if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = RO_GANON_BOSS_KEY_VANILLA;
                        } else if(it.value() == "Own dungeon") {
                            gSaveContext.randoSettings[index].value = RO_GANON_BOSS_KEY_OWN_DUNGEON;
                        } else if(it.value() == "Start with") {
                            gSaveContext.randoSettings[index].value = RO_GANON_BOSS_KEY_STARTWITH;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = RO_GANON_BOSS_KEY_ANY_DUNGEON;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = RO_GANON_BOSS_KEY_OVERWORLD;
                        } else if(it.value() == "Anywhere") {                         
                            gSaveContext.randoSettings[index].value = RO_GANON_BOSS_KEY_ANYWHERE;
                        } else if(it.value() == "LACS-Vanilla") {
                            gSaveContext.randoSettings[index].value = RO_GANON_BOSS_KEY_LACS_VANILLA;
                        } else if(it.value() == "LACS-Medallions") {
                            gSaveContext.randoSettings[index].value = RO_GANON_BOSS_KEY_LACS_MEDALLIONS;
                        } else if(it.value() == "LACS-Stones") {
                            gSaveContext.randoSettings[index].value = RO_GANON_BOSS_KEY_LACS_STONES;
                        } else if(it.value() == "LACS-Rewards") {
                            gSaveContext.randoSettings[index].value = RO_GANON_BOSS_KEY_LACS_REWARDS;
                        } else if(it.value() == "LACS-Dungeons") {
                            gSaveContext.randoSettings[index].value = RO_GANON_BOSS_KEY_LACS_DUNGEONS;
                        } else if(it.value() == "LACS-Tokens") {
                            gSaveContext.randoSettings[index].value = RO_GANON_BOSS_KEY_LACS_TOKENS;
                        }
                        break;
                    case RSK_SKIP_CHILD_ZELDA:
                        gSaveContext.randoSettings[index].value = it.value();
                        break;
                    case RSK_STARTING_CONSUMABLES:
                    case RSK_FULL_WALLETS:
                        if(it.value() == "No") {
                            gSaveContext.randoSettings[index].value = RO_GENERIC_NO;            
                        } else if(it.value() == "Yes") {
                            gSaveContext.randoSettings[index].value = RO_GENERIC_YES;
                        }
                        break;
                    case RSK_SKIP_CHILD_STEALTH:
                    case RSK_SKIP_EPONA_RACE:
                    case RSK_SKIP_TOWER_ESCAPE:
                        if(it.value() == "Don't Skip") {
                            gSaveContext.randoSettings[index].value = RO_GENERIC_DONT_SKIP;
                        } else if (it.value() == "Skip") {
                            gSaveContext.randoSettings[index].value = RO_GENERIC_SKIP;
                        }
                        break;
                    case RSK_SHUFFLE_TOKENS:
                        if (it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = RO_TOKENSANITY_OFF;
                        } else if (it.value() == "Dungeons") {
                            gSaveContext.randoSettings[index].value = RO_TOKENSANITY_DUNGEONS;
                        } else if (it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = RO_TOKENSANITY_OVERWORLD;
                        } else if (it.value() == "All Tokens") {
                            gSaveContext.randoSettings[index].value = RO_TOKENSANITY_ALL;
                        }
                        break;
                    case RSK_LINKS_POCKET:
                        if (it.value() == "Dungeon Reward") {
                            gSaveContext.randoSettings[index].value = RO_LINKS_POCKET_DUNGEON_REWARD;
                        } else if (it.value() == "Advancement") {
                            gSaveContext.randoSettings[index].value = RO_LINKS_POCKET_ADVANCEMENT;
                        } else if (it.value() == "Anything") {
                            gSaveContext.randoSettings[index].value = RO_LINKS_POCKET_ANYTHING;
                        } else if (it.value() == "Nothing") {
                            gSaveContext.randoSettings[index].value = RO_LINKS_POCKET_NOTHING;
                        }
                        break;
                    case RSK_MQ_DUNGEON_COUNT:
                        if (it.value() == "Random") {
                            gSaveContext.randoSettings[index].value = 13;
                            break;
                        }
                        numericValueString = it.value();
                        gSaveContext.randoSettings[index].value = std::stoi(numericValueString);
                        break;
                    case RSK_SHUFFLE_DUNGEON_ENTRANCES:
                        if (it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ENTRANCE_SHUFFLE_OFF;
                        } else if (it.value() == "On") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ENTRANCE_SHUFFLE_ON;
                        } else if (it.value() == "On + Ganon") {
                            gSaveContext.randoSettings[index].value = RO_DUNGEON_ENTRANCE_SHUFFLE_ON_PLUS_GANON;
                        }
                        break;
                    case RSK_SHUFFLE_INTERIOR_ENTRANCES:
                        if (it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = RO_INTERIOR_ENTRANCE_SHUFFLE_OFF;
                        } else if (it.value() == "Simple") {
                            gSaveContext.randoSettings[index].value = RO_INTERIOR_ENTRANCE_SHUFFLE_SIMPLE;
                        } else if (it.value() == "All") {
                            gSaveContext.randoSettings[index].value = RO_INTERIOR_ENTRANCE_SHUFFLE_ALL;
                        }
                        break;
                }
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

    // Have all these use strncpy so that the null terminator is copied 
    // and also set the last index to null for safety
    try {
        json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;

        std::string childAltarJsonText = spoilerFileJson["childAltarText"].get<std::string>();
        std::string formattedChildAltarText = FormatJsonHintText(childAltarJsonText);
        strncpy(gSaveContext.childAltarText, formattedChildAltarText.c_str(), sizeof(gSaveContext.childAltarText) - 1);
        gSaveContext.childAltarText[sizeof(gSaveContext.childAltarText) - 1] = 0;

        std::string adultAltarJsonText = spoilerFileJson["adultAltarText"].get<std::string>();
        std::string formattedAdultAltarText = FormatJsonHintText(adultAltarJsonText);
        strncpy(gSaveContext.adultAltarText, formattedAdultAltarText.c_str(), sizeof(gSaveContext.adultAltarText) - 1);
        gSaveContext.adultAltarText[sizeof(gSaveContext.adultAltarText) - 1] = 0;

        std::string ganonHintJsonText = spoilerFileJson["ganonHintText"].get<std::string>();
        std::string formattedGanonHintJsonText = FormatJsonHintText(ganonHintJsonText);
        strncpy(gSaveContext.ganonHintText, formattedGanonHintJsonText.c_str(), sizeof(gSaveContext.ganonHintText) - 1);
        gSaveContext.ganonHintText[sizeof(gSaveContext.ganonHintText) - 1] = 0;

        std::string ganonJsonText = spoilerFileJson["ganonText"].get<std::string>();
        std::string formattedGanonJsonText = FormatJsonHintText(ganonJsonText);
        strncpy(gSaveContext.ganonText, formattedGanonJsonText.c_str(), sizeof(gSaveContext.ganonText) - 1);
        gSaveContext.ganonText[sizeof(gSaveContext.ganonText) - 1] = 0;

        std::string warpMinuetJsonText = spoilerFileJson["warpMinuetText"].get<std::string>();
        strncpy(gSaveContext.warpMinuetText, warpMinuetJsonText.c_str(), sizeof(gSaveContext.warpMinuetText) - 1);
        gSaveContext.warpMinuetText[sizeof(gSaveContext.warpMinuetText) - 1] = 0;

        std::string warpBoleroJsonText = spoilerFileJson["warpBoleroText"].get<std::string>();
        strncpy(gSaveContext.warpBoleroText, warpBoleroJsonText.c_str(), sizeof(gSaveContext.warpBoleroText) - 1);
        gSaveContext.warpBoleroText[sizeof(gSaveContext.warpBoleroText) - 1] = 0;

        std::string warpSerenadeJsonText = spoilerFileJson["warpSerenadeText"].get<std::string>();
        strncpy(gSaveContext.warpSerenadeText, warpSerenadeJsonText.c_str(), sizeof(gSaveContext.warpSerenadeText) - 1);
        gSaveContext.warpSerenadeText[sizeof(gSaveContext.warpSerenadeText) - 1] = 0;

        std::string warpRequiemJsonText = spoilerFileJson["warpRequiemText"].get<std::string>();
        strncpy(gSaveContext.warpRequiemText, warpRequiemJsonText.c_str(), sizeof(gSaveContext.warpRequiemText) - 1);
        gSaveContext.warpRequiemText[sizeof(gSaveContext.warpRequiemText) - 1] = 0;

        std::string warpNocturneJsonText = spoilerFileJson["warpNocturneText"].get<std::string>();
        strncpy(gSaveContext.warpNocturneText, warpNocturneJsonText.c_str(), sizeof(gSaveContext.warpNocturneText) - 1);
        gSaveContext.warpNocturneText[sizeof(gSaveContext.warpNocturneText) - 1] = 0;

        std::string warpPreludeJsonText = spoilerFileJson["warpPreludeText"].get<std::string>();
        strncpy(gSaveContext.warpPreludeText, warpPreludeJsonText.c_str(), sizeof(gSaveContext.warpPreludeText) - 1);
        gSaveContext.warpPreludeText[sizeof(gSaveContext.warpPreludeText) - 1] = 0;

        json hintsJson = spoilerFileJson["hints"];
        int index = 0;
        for (auto it = hintsJson.begin(); it != hintsJson.end(); ++it) {
            gSaveContext.hintLocations[index].check = SpoilerfileCheckNameToEnum[it.key()];

            std::string hintMessage = FormatJsonHintText(it.value());
            size_t maxHintTextSize = sizeof(gSaveContext.hintLocations[index].hintText);
            strncpy(gSaveContext.hintLocations[index].hintText, hintMessage.c_str(), maxHintTextSize - 1);
            gSaveContext.hintLocations[index].hintText[maxHintTextSize - 1] = 0;

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

void Randomizer::ParseMasterQuestDungeonsFile(const char* spoilerFileName) {
    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream) {
        return;
    }

    this->masterQuestDungeons.clear();

    try {
        json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;
        json mqDungeonsJson = spoilerFileJson["masterQuestDungeons"];

        for (auto it = mqDungeonsJson.begin(); it != mqDungeonsJson.end(); it++) {
            this->masterQuestDungeons.emplace(spoilerFileDungeonToScene[it.value()]);
        }
    } catch (const std::exception& e) {
        return;
    }
}

int16_t Randomizer::GetVanillaMerchantPrice(RandomizerCheck check) {
    switch (check) { 
        case RC_HF_DEKU_SCRUB_GROTTO:
            return 10;    
        case RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT:
        case RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS:
        case RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR:
            return 15;    
        case RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT:
        case RC_LH_DEKU_SCRUB_GROTTO_LEFT:
        case RC_GC_DEKU_SCRUB_GROTTO_LEFT:
        case RC_DMC_DEKU_SCRUB_GROTTO_LEFT:
        case RC_LLR_DEKU_SCRUB_GROTTO_LEFT:
        case RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT:
        case RC_JABU_JABUS_BELLY_DEKU_SCRUB:
        case RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT:
            return 20;
        case RC_LW_DEKU_SCRUB_NEAR_BRIDGE:
        case RC_LW_DEKU_SCRUB_GROTTO_REAR:
        case RC_LW_DEKU_SCRUB_GROTTO_FRONT:
        case RC_SFM_DEKU_SCRUB_GROTTO_REAR:
        case RC_SFM_DEKU_SCRUB_GROTTO_FRONT:
        case RC_LH_DEKU_SCRUB_GROTTO_RIGHT:
        case RC_LH_DEKU_SCRUB_GROTTO_CENTER:
        case RC_GV_DEKU_SCRUB_GROTTO_REAR:
        case RC_GV_DEKU_SCRUB_GROTTO_FRONT:
        case RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR:
        case RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT:
        case RC_GC_DEKU_SCRUB_GROTTO_RIGHT:
        case RC_GC_DEKU_SCRUB_GROTTO_CENTER:
        case RC_DMC_DEKU_SCRUB:
        case RC_DMC_DEKU_SCRUB_GROTTO_RIGHT:
        case RC_DMC_DEKU_SCRUB_GROTTO_CENTER:
        case RC_ZR_DEKU_SCRUB_GROTTO_REAR:
        case RC_ZR_DEKU_SCRUB_GROTTO_FRONT:
        case RC_LLR_DEKU_SCRUB_GROTTO_RIGHT:
        case RC_LLR_DEKU_SCRUB_GROTTO_CENTER:
        case RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT:
        case RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT:
        case RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS:
        case RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT:
        case RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT:
        case RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT:
        case RC_GANONS_CASTLE_DEKU_SCRUB_LEFT:
        case RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT:
        case RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER:
        case RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT:
        case RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT:
            return 40;
        case RC_DEKU_TREE_MQ_DEKU_SCRUB:
        case RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY:
        case RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE:
            return 50;
        default:
            // we check for -1 when calling this to know if we don't have a price
            return -1;
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

        for (auto it = locationsJson.begin(); it != locationsJson.end(); ++it) {
            RandomizerCheck randomizerCheck = SpoilerfileCheckNameToEnum[it.key()];
            if (it->is_structured()) {
                json itemJson = *it;
                for (auto itemit = itemJson.begin(); itemit != itemJson.end(); ++itemit) {
                    if (itemit.key() == "item") {
                        gSaveContext.itemLocations[randomizerCheck].check = randomizerCheck;
                        gSaveContext.itemLocations[randomizerCheck].get.rgID = SpoilerfileGetNameToEnum[itemit.value()];
                    } else if (itemit.key() == "price") {
                        merchantPrices[gSaveContext.itemLocations[randomizerCheck].check] = itemit.value();
                    } else if (itemit.key() == "model") {
                        gSaveContext.itemLocations[randomizerCheck].get.fakeRgID =
                            SpoilerfileGetNameToEnum[itemit.value()];
                    } else if (itemit.key() == "trickName") {
                        strncpy(gSaveContext.itemLocations[randomizerCheck].get.trickName,
                                std::string(itemit.value()).c_str(), MAX_TRICK_NAME_SIZE);
                    }
                }
            } else {
                gSaveContext.itemLocations[randomizerCheck].check = SpoilerfileCheckNameToEnum[it.key()];
                gSaveContext.itemLocations[randomizerCheck].get.rgID = SpoilerfileGetNameToEnum[it.value()];
                gSaveContext.itemLocations[randomizerCheck].get.fakeRgID = RG_NONE;
                int16_t price = GetVanillaMerchantPrice(randomizerCheck);
                if (price != -1) {
                    merchantPrices[gSaveContext.itemLocations[randomizerCheck].check] = price;
                }
            }
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

void Randomizer::ParseEntranceDataFile(const char* spoilerFileName, bool silent) {
    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream) {
        return;
    }

    // set all the entrances to be 0 to indicate an unshuffled entrance
    for (auto &entranceOveride : gSaveContext.entranceOverrides) {
        entranceOveride.index = 0;
        entranceOveride.destination = 0;
        entranceOveride.blueWarp = 0;
        entranceOveride.override = 0;
        entranceOveride.overrideDestination = 0;
    }

    try {
        json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;
        json EntrancesJson = spoilerFileJson["entrances"];

        size_t i = 0;
        for (auto it = EntrancesJson.begin(); it != EntrancesJson.end(); ++it, i++) {
            json entranceJson = *it;

            for (auto entranceIt = entranceJson.begin(); entranceIt != entranceJson.end(); ++entranceIt) {
                if (entranceIt.key() == "index") {
                    gSaveContext.entranceOverrides[i].index = entranceIt.value();
                } else if (entranceIt.key() == "destination") {
                    gSaveContext.entranceOverrides[i].destination = entranceIt.value();
                } else if (entranceIt.key() == "blueWarp") {
                    gSaveContext.entranceOverrides[i].blueWarp = entranceIt.value();
                } else if (entranceIt.key() == "override") {
                    gSaveContext.entranceOverrides[i].override = entranceIt.value();
                } else if (entranceIt.key() == "overrideDestination") {
                    gSaveContext.entranceOverrides[i].overrideDestination = entranceIt.value();
                }
            }
        }
    } catch (const std::exception& e) {
        return;
    }
}

bool Randomizer::IsTrialRequired(RandomizerInf trial) {
    return this->trialsRequired.contains(trial);
}

RandomizerGetData Randomizer::GetRandomizerGetDataFromActor(s16 actorId, s16 sceneNum, s16 actorParams) {
    return this->itemLocations[GetCheckFromActor(actorId, sceneNum, actorParams)];
}

RandomizerGetData Randomizer::GetRandomizerGetDataFromKnownCheck(RandomizerCheck randomizerCheck) {
    return this->itemLocations[randomizerCheck];
}

GetItemEntry Randomizer::GetItemFromActor(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogItemId, bool checkObtainability) {
    RandomizerGetData rgData = this->itemLocations[GetCheckFromActor(actorId, sceneNum, actorParams)];
    return GetItemEntryFromRGData(rgData, ogItemId, checkObtainability);
}

ItemObtainability Randomizer::GetItemObtainabilityFromRandomizerCheck(RandomizerCheck randomizerCheck) {
    return GetItemObtainabilityFromRandomizerGet(GetRandomizerGetDataFromKnownCheck(randomizerCheck).rgID);
}

ItemObtainability Randomizer::GetItemObtainabilityFromRandomizerGet(RandomizerGet randoGet) {

    // Shopsanity with at least one item shuffled allows for a third wallet upgrade.
    // This is needed since Plentiful item pool also adds a third progressive wallet
    // but we should *not* get Tycoon's Wallet in that mode.
    u8 numWallets = GetRandoSettingValue(RSK_SHOPSANITY) > RO_SHOPSANITY_ZERO_ITEMS ? 3 : 2;
    switch (randoGet) {
        case RG_NONE:
        case RG_TRIFORCE:
        case RG_HINT:
        case RG_MAX:
        case RG_SOLD_OUT:
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
            return !gSaveContext.isDoubleDefenseAcquired ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
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
        case RG_FOREST_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_BMORI1] < FOREST_TEMPLE_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_HIDAN] < FIRE_TEMPLE_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_WATER_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_MIZUSIN] < WATER_TEMPLE_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SPIRIT_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_JYASINZOU] < SPIRIT_TEMPLE_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SHADOW_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_HAKADAN] < SHADOW_TEMPLE_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BOTTOM_OF_THE_WELL_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_HAKADANCH] < BOTTOM_OF_THE_WELL_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_MEN] < GERUDO_TRAINING_GROUNDS_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GERUDO_FORTRESS_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_GERUDOWAY] < GERUDO_FORTRESS_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GANONS_CASTLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_GANONTIKA] < GANONS_CASTLE_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_TREASURE_GAME_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_TAKARAYA] < TREASURE_GAME_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

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
            if (GetRandoSettingValue(RSK_STARTING_MAPS_COMPASSES) == RO_DUNGEON_ITEM_LOC_STARTWITH ||
                GetRandoSettingValue(RSK_STARTING_MAPS_COMPASSES) == RO_DUNGEON_ITEM_LOC_VANILLA ||
                GetRandoSettingValue(RSK_STARTING_MAPS_COMPASSES) == RO_DUNGEON_ITEM_LOC_OWN_DUNGEON) {
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
            if (GetRandoSettingValue(RSK_STARTING_MAPS_COMPASSES) == RO_DUNGEON_ITEM_LOC_STARTWITH ||
                GetRandoSettingValue(RSK_STARTING_MAPS_COMPASSES) == RO_DUNGEON_ITEM_LOC_VANILLA ||
                GetRandoSettingValue(RSK_STARTING_MAPS_COMPASSES) == RO_DUNGEON_ITEM_LOC_OWN_DUNGEON) {
                return GI_COMPASS;
            } else {
                return (GetItemID)randoGet;
            }

        case RG_FOREST_TEMPLE_BOSS_KEY:
        case RG_FIRE_TEMPLE_BOSS_KEY:
        case RG_WATER_TEMPLE_BOSS_KEY:
        case RG_SPIRIT_TEMPLE_BOSS_KEY:
        case RG_SHADOW_TEMPLE_BOSS_KEY:
            if (GetRandoSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_STARTWITH ||
                GetRandoSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_VANILLA ||
                GetRandoSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_OWN_DUNGEON) {
                return GI_KEY_BOSS;
            } else {
                return (GetItemID)randoGet;
            }
        case RG_GANONS_CASTLE_BOSS_KEY:
            if (GetRandoSettingValue(RSK_GANONS_BOSS_KEY) == RO_GANON_BOSS_KEY_VANILLA ||
                GetRandoSettingValue(RSK_GANONS_BOSS_KEY) == RO_GANON_BOSS_KEY_OWN_DUNGEON ||
                GetRandoSettingValue(RSK_GANONS_BOSS_KEY) == RO_GANON_BOSS_KEY_STARTWITH) {
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
            if (GetRandoSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_STARTWITH ||
                GetRandoSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_VANILLA ||
                GetRandoSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_OWN_DUNGEON) {
                return GI_KEY_SMALL;
            } else {
                return (GetItemID)randoGet;
            }
        case RG_GERUDO_FORTRESS_SMALL_KEY:
            if (GetRandoSettingValue(RSK_GERUDO_KEYS) == RO_GERUDO_KEYS_VANILLA) {
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
            if (GetRandoSettingValue(RSK_GERUDO_KEYS) != RO_GERUDO_KEYS_VANILLA) {
                return false;
            }
            return true;
        case RG_FOREST_TEMPLE_BOSS_KEY:
        case RG_FIRE_TEMPLE_BOSS_KEY:
        case RG_WATER_TEMPLE_BOSS_KEY:
        case RG_SPIRIT_TEMPLE_BOSS_KEY:
        case RG_SHADOW_TEMPLE_BOSS_KEY:
            if (GetRandoSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_STARTWITH ||
                GetRandoSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_VANILLA ||
                GetRandoSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_OWN_DUNGEON) {
                return true;
            }
            return false;
        case RG_GANONS_CASTLE_BOSS_KEY:
            if (GetRandoSettingValue(RSK_GANONS_BOSS_KEY) == RO_GANON_BOSS_KEY_VANILLA ||
                GetRandoSettingValue(RSK_GANONS_BOSS_KEY) == RO_GANON_BOSS_KEY_OWN_DUNGEON ||
                GetRandoSettingValue(RSK_GANONS_BOSS_KEY) == RO_GANON_BOSS_KEY_STARTWITH) {
                return true;
            }
            return false;
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
            if (GetRandoSettingValue(RSK_STARTING_MAPS_COMPASSES) == RO_DUNGEON_ITEM_LOC_STARTWITH ||
                GetRandoSettingValue(RSK_STARTING_MAPS_COMPASSES) == RO_DUNGEON_ITEM_LOC_VANILLA ||
                GetRandoSettingValue(RSK_STARTING_MAPS_COMPASSES) == RO_DUNGEON_ITEM_LOC_OWN_DUNGEON) {
                return true;
            }
            return false;
        default:
            return false;
    }
}

bool Randomizer::CheckContainsVanillaItem(RandomizerCheck randoCheck) {
    RandomizerGet randoGet = this->itemLocations[randoCheck].rgID;
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

// There has been some talk about potentially just using the RC identifier to store flags rather than randomizer inf, so
// for now we're not going to store randomzierInf in the randomizer check objects, we're just going to map them 1:1 here
std::map<RandomizerCheck, RandomizerInf> rcToRandomizerInf = {
    { RC_KF_LINKS_HOUSE_COW,                                          RAND_INF_COWS_MILKED_KF_LINKS_HOUSE_COW },
    { RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT,                       RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT },
    { RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT,                        RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT },
    { RC_LW_DEKU_SCRUB_NEAR_BRIDGE,                                   RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_BRIDGE },
    { RC_LW_DEKU_SCRUB_GROTTO_REAR,                                   RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_GROTTO_REAR },
    { RC_LW_DEKU_SCRUB_GROTTO_FRONT,                                  RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_GROTTO_FRONT },
    { RC_SFM_DEKU_SCRUB_GROTTO_REAR,                                  RAND_INF_SCRUBS_PURCHASED_SFM_DEKU_SCRUB_GROTTO_REAR },
    { RC_SFM_DEKU_SCRUB_GROTTO_FRONT,                                 RAND_INF_SCRUBS_PURCHASED_SFM_DEKU_SCRUB_GROTTO_FRONT },
    { RC_HF_DEKU_SCRUB_GROTTO,                                        RAND_INF_SCRUBS_PURCHASED_HF_DEKU_SCRUB_GROTTO },
    { RC_HF_COW_GROTTO_COW,                                           RAND_INF_COWS_MILKED_HF_COW_GROTTO_COW },
    { RC_LH_DEKU_SCRUB_GROTTO_LEFT,                                   RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_LEFT },
    { RC_LH_DEKU_SCRUB_GROTTO_RIGHT,                                  RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_RIGHT },
    { RC_LH_DEKU_SCRUB_GROTTO_CENTER,                                 RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_CENTER },
    { RC_GV_DEKU_SCRUB_GROTTO_REAR,                                   RAND_INF_SCRUBS_PURCHASED_GV_DEKU_SCRUB_GROTTO_REAR },
    { RC_GV_DEKU_SCRUB_GROTTO_FRONT,                                  RAND_INF_SCRUBS_PURCHASED_GV_DEKU_SCRUB_GROTTO_FRONT },
    { RC_GV_COW,                                                      RAND_INF_COWS_MILKED_GV_COW },
    { RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR,                             RAND_INF_SCRUBS_PURCHASED_COLOSSUS_DEKU_SCRUB_GROTTO_REAR },
    { RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT,                            RAND_INF_SCRUBS_PURCHASED_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT },
    { RC_KAK_IMPAS_HOUSE_COW,                                         RAND_INF_COWS_MILKED_KAK_IMPAS_HOUSE_COW },
    { RC_DMT_COW_GROTTO_COW,                                          RAND_INF_COWS_MILKED_DMT_COW_GROTTO_COW },
    { RC_GC_DEKU_SCRUB_GROTTO_LEFT,                                   RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_LEFT },
    { RC_GC_DEKU_SCRUB_GROTTO_RIGHT,                                  RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_RIGHT },
    { RC_GC_DEKU_SCRUB_GROTTO_CENTER,                                 RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_CENTER },
    { RC_DMC_DEKU_SCRUB,                                              RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB },
    { RC_DMC_DEKU_SCRUB_GROTTO_LEFT,                                  RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_LEFT },
    { RC_DMC_DEKU_SCRUB_GROTTO_RIGHT,                                 RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_RIGHT },
    { RC_DMC_DEKU_SCRUB_GROTTO_CENTER,                                RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_CENTER },
    { RC_ZR_DEKU_SCRUB_GROTTO_REAR,                                   RAND_INF_SCRUBS_PURCHASED_ZR_DEKU_SCRUB_GROTTO_REAR },
    { RC_ZR_DEKU_SCRUB_GROTTO_FRONT,                                  RAND_INF_SCRUBS_PURCHASED_ZR_DEKU_SCRUB_GROTTO_FRONT },
    { RC_LLR_DEKU_SCRUB_GROTTO_LEFT,                                  RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_LEFT },
    { RC_LLR_DEKU_SCRUB_GROTTO_RIGHT,                                 RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_RIGHT },
    { RC_LLR_DEKU_SCRUB_GROTTO_CENTER,                                RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_CENTER },
    { RC_LLR_STABLES_LEFT_COW,                                        RAND_INF_COWS_MILKED_LLR_STABLES_LEFT_COW },
    { RC_LLR_STABLES_RIGHT_COW,                                       RAND_INF_COWS_MILKED_LLR_STABLES_RIGHT_COW },
    { RC_LLR_TOWER_LEFT_COW,                                          RAND_INF_COWS_MILKED_LLR_TOWER_LEFT_COW },
    { RC_LLR_TOWER_RIGHT_COW,                                         RAND_INF_COWS_MILKED_LLR_TOWER_RIGHT_COW },
    { RC_DEKU_TREE_MQ_DEKU_SCRUB,                                     RAND_INF_SCRUBS_PURCHASED_DEKU_TREE_MQ_DEKU_SCRUB },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,               RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS,          RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT,              RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY,                            RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR,                    RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT,                   RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE,                     RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS, RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS },
    { RC_JABU_JABUS_BELLY_DEKU_SCRUB,                                 RAND_INF_SCRUBS_PURCHASED_JABU_JABUS_BELLY_DEKU_SCRUB },
    { RC_JABU_JABUS_BELLY_MQ_COW,                                     RAND_INF_COWS_MILKED_JABU_JABUS_BELLY_MQ_COW },
    { RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT,                        RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT },
    { RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT,                       RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT },
    { RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT,                              RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_RIGHT },
    { RC_GANONS_CASTLE_DEKU_SCRUB_LEFT,                               RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_LEFT },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT,                           RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT,                     RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER,                          RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT,                    RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT,                            RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT },
    { RC_KF_SHOP_ITEM_1,                                              RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1 },
    { RC_KF_SHOP_ITEM_2,                                              RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_2 },
    { RC_KF_SHOP_ITEM_3,                                              RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_3 },
    { RC_KF_SHOP_ITEM_4,                                              RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_4 },
    { RC_KF_SHOP_ITEM_5,                                              RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_5 },
    { RC_KF_SHOP_ITEM_6,                                              RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_6 },
    { RC_KF_SHOP_ITEM_7,                                              RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_7 },
    { RC_KF_SHOP_ITEM_8,                                              RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_8 },
    { RC_GC_SHOP_ITEM_1,                                              RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_1 },
    { RC_GC_SHOP_ITEM_2,                                              RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_2 },
    { RC_GC_SHOP_ITEM_3,                                              RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_3 },
    { RC_GC_SHOP_ITEM_4,                                              RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_4 },
    { RC_GC_SHOP_ITEM_5,                                              RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_5 },
    { RC_GC_SHOP_ITEM_6,                                              RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_6 },
    { RC_GC_SHOP_ITEM_7,                                              RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_7 },
    { RC_GC_SHOP_ITEM_8,                                              RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_8 },
    { RC_ZD_SHOP_ITEM_1,                                              RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_1 },
    { RC_ZD_SHOP_ITEM_2,                                              RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_2 },
    { RC_ZD_SHOP_ITEM_3,                                              RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_3 },
    { RC_ZD_SHOP_ITEM_4,                                              RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_4 },
    { RC_ZD_SHOP_ITEM_5,                                              RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_5 },
    { RC_ZD_SHOP_ITEM_6,                                              RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_6 },
    { RC_ZD_SHOP_ITEM_7,                                              RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_7 },
    { RC_ZD_SHOP_ITEM_8,                                              RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_8 },
    { RC_KAK_BAZAAR_ITEM_1,                                           RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_1 },
    { RC_KAK_BAZAAR_ITEM_2,                                           RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_2 },
    { RC_KAK_BAZAAR_ITEM_3,                                           RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_3 },
    { RC_KAK_BAZAAR_ITEM_4,                                           RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_4 },
    { RC_KAK_BAZAAR_ITEM_5,                                           RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_5 },
    { RC_KAK_BAZAAR_ITEM_6,                                           RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_6 },
    { RC_KAK_BAZAAR_ITEM_7,                                           RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_7 },
    { RC_KAK_BAZAAR_ITEM_8,                                           RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_8 },
    { RC_KAK_POTION_SHOP_ITEM_1,                                      RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_1 },
    { RC_KAK_POTION_SHOP_ITEM_2,                                      RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_2 },
    { RC_KAK_POTION_SHOP_ITEM_3,                                      RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_3 },
    { RC_KAK_POTION_SHOP_ITEM_4,                                      RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_4 },
    { RC_KAK_POTION_SHOP_ITEM_5,                                      RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_5 },
    { RC_KAK_POTION_SHOP_ITEM_6,                                      RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_6 },
    { RC_KAK_POTION_SHOP_ITEM_7,                                      RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_7 },
    { RC_KAK_POTION_SHOP_ITEM_8,                                      RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_8 },
    { RC_MARKET_BAZAAR_ITEM_1,                                        RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_1 },
    { RC_MARKET_BAZAAR_ITEM_2,                                        RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_2 },
    { RC_MARKET_BAZAAR_ITEM_3,                                        RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_3 },
    { RC_MARKET_BAZAAR_ITEM_4,                                        RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_4 },
    { RC_MARKET_BAZAAR_ITEM_5,                                        RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_5 },
    { RC_MARKET_BAZAAR_ITEM_6,                                        RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_6 },
    { RC_MARKET_BAZAAR_ITEM_7,                                        RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_7 },
    { RC_MARKET_BAZAAR_ITEM_8,                                        RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_8 },
    { RC_MARKET_POTION_SHOP_ITEM_1,                                   RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_1 },
    { RC_MARKET_POTION_SHOP_ITEM_2,                                   RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_2 },
    { RC_MARKET_POTION_SHOP_ITEM_3,                                   RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_3 },
    { RC_MARKET_POTION_SHOP_ITEM_4,                                   RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_4 },
    { RC_MARKET_POTION_SHOP_ITEM_5,                                   RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_5 },
    { RC_MARKET_POTION_SHOP_ITEM_6,                                   RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_6 },
    { RC_MARKET_POTION_SHOP_ITEM_7,                                   RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_7 },
    { RC_MARKET_POTION_SHOP_ITEM_8,                                   RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_8 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_1,                                  RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_1 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_2,                                  RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_2 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_3,                                  RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_3 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_4,                                  RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_4 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_5,                                  RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_5 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_6,                                  RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_6 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_7,                                  RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_7 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_8,                                  RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_8 },
    { RC_GC_MEDIGORON,                                                RAND_INF_MERCHANTS_MEDIGORON                   },
    { RC_WASTELAND_BOMBCHU_SALESMAN,                                  RAND_INF_MERCHANTS_CARPET_SALESMAN              },
    { RC_LW_TRADE_COJIRO,                                             RAND_INF_ADULT_TRADES_LW_TRADE_COJIRO },
    { RC_GV_TRADE_SAW,                                                RAND_INF_ADULT_TRADES_GV_TRADE_SAW },
    { RC_DMT_TRADE_BROKEN_SWORD,                                      RAND_INF_ADULT_TRADES_DMT_TRADE_BROKEN_SWORD },
    { RC_LH_TRADE_FROG,                                               RAND_INF_ADULT_TRADES_LH_TRADE_FROG },
    { RC_DMT_TRADE_EYEDROPS,                                          RAND_INF_ADULT_TRADES_DMT_TRADE_EYEDROPS },

};

RandomizerCheckObject Randomizer::GetCheckObjectFromActor(s16 actorId, s16 sceneNum, s32 actorParams = 0x00) {
    RandomizerCheck specialRc = RC_UNKNOWN_CHECK;
    // TODO: Migrate these special cases into table, or at least document why they are special
    switch(sceneNum) {
        case SCENE_TAKARAYA:
            if(actorParams == 20170) specialRc = RC_MARKET_TREASURE_CHEST_GAME_REWARD;

            // RANDOTODO update logic to match 3ds rando when we implement keysanity
            // keep keys og
            if ((actorParams & 0x60) == 0x20) break;

            if (GetRandoSettingValue(RSK_SHUFFLE_CHEST_MINIGAME)) {
                if((actorParams & 0xF) < 2) specialRc = RC_MARKET_TREASURE_CHEST_GAME_ITEM_1;
                if((actorParams & 0xF) < 4) specialRc = RC_MARKET_TREASURE_CHEST_GAME_ITEM_2;
                if((actorParams & 0xF) < 6) specialRc = RC_MARKET_TREASURE_CHEST_GAME_ITEM_3;
                if((actorParams & 0xF) < 8) specialRc = RC_MARKET_TREASURE_CHEST_GAME_ITEM_4;
                if((actorParams & 0xF) < 10) specialRc = RC_MARKET_TREASURE_CHEST_GAME_ITEM_5;
            }
            break;
        case SCENE_SHRINE:
        case SCENE_SHRINE_N:
        case SCENE_SHRINE_R:
            switch (actorParams) {
                case 14342:
                    specialRc = RC_TOT_LEFT_GOSSIP_STONE;
                    break;
                case 14599:
                    specialRc = RC_TOT_LEFT_CENTER_GOSSIP_STONE;
                    break;
                case 14862:
                    specialRc = RC_TOT_RIGHT_CENTER_GOSSIP_STONE;
                    break;
                case 15120:
                    specialRc = RC_TOT_RIGHT_GOSSIP_STONE;
                    break;
            }
            break;
        case SCENE_SPOT01:
            switch (actorId) {
                case ACTOR_EN_NIW_LADY:
                    if (LINK_IS_ADULT) {
                        specialRc = RC_KAK_ANJU_AS_ADULT;
                    } else {
                        specialRc = RC_KAK_ANJU_AS_CHILD;
                    }
            }
            break;
        case SCENE_SPOT06:
            switch (actorId) {
                case ACTOR_ITEM_ETCETERA:
                    if (LINK_IS_ADULT) {
                        specialRc = RC_LH_SUN;
                    } else {
                        specialRc = RC_LH_UNDERWATER_ITEM;
                    }
            }
            break;
        case SCENE_SPOT08:
            switch (actorParams) {
                case 15362:
                case 14594:
                    specialRc = RC_JABU_GOSSIP_STONE;
                    break;
                case 14849:
                case 14337:
                    specialRc = RC_FAIRY_GOSSIP_STONE;
                    break;
            }
            break;
        case SCENE_DDAN:
            // special case for MQ DC Gossip Stone
            if (actorId == ACTOR_EN_GS && actorParams == 15892 && ResourceMgr_IsGameMasterQuest()) {
                specialRc = RC_DODONGOS_CAVERN_GOSSIP_STONE;
            }
            break;
    }

    if (specialRc != RC_UNKNOWN_CHECK) {
        return RandomizerCheckObjects::GetAllRCObjects()[specialRc];
    }

    auto range = checkFromActorMultimap.equal_range(std::make_tuple(actorId, sceneNum, actorParams));

    for (auto it = range.first; it != range.second; ++it) {
        if (
            it->second.vOrMQ == RCVORMQ_BOTH ||
            (it->second.vOrMQ == RCVORMQ_VANILLA && !ResourceMgr_IsGameMasterQuest()) ||
            (it->second.vOrMQ == RCVORMQ_MQ && ResourceMgr_IsGameMasterQuest())
        ) {
            return it->second;
        }
    }

    return RandomizerCheckObjects::GetAllRCObjects()[RC_UNKNOWN_CHECK];
}

ScrubIdentity Randomizer::IdentifyScrub(s32 sceneNum, s32 actorParams, s32 respawnData) {
    struct ScrubIdentity scrubIdentity;

    scrubIdentity.randomizerInf = RAND_INF_MAX;
    scrubIdentity.randomizerCheck = RC_UNKNOWN_CHECK;
    scrubIdentity.getItemId = GI_NONE;
    scrubIdentity.itemPrice = -1;
    scrubIdentity.isShuffled = false;

    // Scrubs that are 0x06 are loaded as 0x03 when child, switching from selling arrows to seeds
    if (actorParams == 0x06) actorParams = 0x03;

    if (sceneNum == SCENE_KAKUSIANA) {
        actorParams = TWO_ACTOR_PARAMS(actorParams, respawnData);
    }

    RandomizerCheckObject rcObject = GetCheckObjectFromActor(ACTOR_EN_DNS, sceneNum, actorParams);

    if (rcObject.rc != RC_UNKNOWN_CHECK) {
        scrubIdentity.randomizerInf = rcToRandomizerInf[rcObject.rc];
        scrubIdentity.randomizerCheck = rcObject.rc;
        scrubIdentity.getItemId = rcObject.ogItemId;
        scrubIdentity.isShuffled = GetRandoSettingValue(RSK_SHUFFLE_SCRUBS) != RO_SCRUBS_OFF;

        if (rcObject.rc == RC_HF_DEKU_SCRUB_GROTTO || rcObject.rc == RC_LW_DEKU_SCRUB_GROTTO_FRONT || rcObject.rc == RC_LW_DEKU_SCRUB_NEAR_BRIDGE) {
            scrubIdentity.isShuffled = true;
        }

        if (merchantPrices.find(scrubIdentity.randomizerCheck) != merchantPrices.end()) {
            scrubIdentity.itemPrice = merchantPrices[scrubIdentity.randomizerCheck];
        }
    }

    return scrubIdentity;
}

ShopItemIdentity Randomizer::IdentifyShopItem(s32 sceneNum, u8 slotIndex) {
    ShopItemIdentity shopItemIdentity;

    shopItemIdentity.randomizerInf = RAND_INF_MAX;
    shopItemIdentity.randomizerCheck = RC_UNKNOWN_CHECK;
    shopItemIdentity.ogItemId = GI_NONE;
    shopItemIdentity.itemPrice = -1;
    shopItemIdentity.enGirlAShopItem = 0x32;

    RandomizerCheckObject rcObject = GetCheckObjectFromActor(ACTOR_EN_GIRLA, 
        // Bazaar (SHOP1) scene is reused, so if entering from Kak use debug scene to identify
        (sceneNum == SCENE_SHOP1 && gSaveContext.entranceIndex == 0xB7) ? SCENE_TEST01 : sceneNum, slotIndex);

    if (rcObject.rc != RC_UNKNOWN_CHECK) {
        shopItemIdentity.randomizerInf = rcToRandomizerInf[rcObject.rc];
        shopItemIdentity.randomizerCheck = rcObject.rc;
        shopItemIdentity.ogItemId = rcObject.ogItemId;

        RandomizerGetData randoGet = GetRandomizerGetDataFromKnownCheck(shopItemIdentity.randomizerCheck);
        if (randomizerGetToEnGirlShopItem.find(randoGet.rgID) != randomizerGetToEnGirlShopItem.end()) {
            shopItemIdentity.enGirlAShopItem = randomizerGetToEnGirlShopItem[randoGet.rgID];
        }

        if (merchantPrices.find(shopItemIdentity.randomizerCheck) != merchantPrices.end()) {
            shopItemIdentity.itemPrice = merchantPrices[shopItemIdentity.randomizerCheck];
        }
    }

    return shopItemIdentity;
}

CowIdentity Randomizer::IdentifyCow(s32 sceneNum, s32 posX, s32 posZ) {
    struct CowIdentity cowIdentity;

    cowIdentity.randomizerInf = RAND_INF_MAX;
    cowIdentity.randomizerCheck = RC_UNKNOWN_CHECK;

    s32 actorParams = 0x00;
    // Only need to pass params if in a scene with two cows
    if (sceneNum == SCENE_KAKUSIANA || sceneNum == SCENE_MALON_STABLE || sceneNum == SCENE_SOUKO) {
        actorParams = TWO_ACTOR_PARAMS(posX, posZ);
    }

    RandomizerCheckObject rcObject = GetCheckObjectFromActor(ACTOR_EN_COW, sceneNum, actorParams);

    if (rcObject.rc != RC_UNKNOWN_CHECK) {
        cowIdentity.randomizerInf = rcToRandomizerInf[rcObject.rc];
        cowIdentity.randomizerCheck = rcObject.rc;
    }

    return cowIdentity;
}

u8 Randomizer::GetRandoSettingValue(RandomizerSettingKey randoSettingKey) {
    return this->randoSettings[randoSettingKey];
}

GetItemEntry Randomizer::GetItemEntryFromRGData(RandomizerGetData rgData, GetItemID ogItemId, bool checkObtainability) {
    // Go ahead and early return the ogItemId's entry if we somehow get RG_NONE.
    if (rgData.rgID == RG_NONE) {
        return ItemTableManager::Instance->RetrieveItemEntry(MOD_NONE, ogItemId);
    }
    if (checkObtainability && OTRGlobals::Instance->gRandomizer->GetItemObtainabilityFromRandomizerGet(rgData.rgID) != CAN_OBTAIN) {
        return ItemTableManager::Instance->RetrieveItemEntry(MOD_NONE, GI_RUPEE_BLUE);
    }
    // Can't get RG_ICE_TRAP if the rgID corresponds to a vanilla item
    if (IsItemVanilla(rgData.rgID)) {
        return ItemTableManager::Instance->RetrieveItemEntry(MOD_NONE, GetItemIdFromRandomizerGet(rgData.rgID, ogItemId));
    }
    // After this point we can assume we are dealing with a randomizer exclusive item.
    GetItemEntry giEntry = ItemTableManager::Instance->RetrieveItemEntry(
        MOD_RANDOMIZER, GetItemIdFromRandomizerGet(rgData.rgID, ogItemId));
    // If we have an ice trap, we want to change the GID and DrawFunc to the fakeRgID's values.
    if (rgData.rgID == RG_ICE_TRAP) {
        ModIndex modIndex;
        if (IsItemVanilla(rgData.fakeRgID)) {
            modIndex = MOD_NONE;
        } else {
            modIndex = MOD_RANDOMIZER;
        }
        GetItemEntry fakeGiEntry = ItemTableManager::Instance->RetrieveItemEntry(modIndex, GetItemIdFromRandomizerGet(rgData.fakeRgID, ogItemId));
        giEntry.gid = fakeGiEntry.gid;
        giEntry.gi = fakeGiEntry.gi;
        giEntry.drawFunc = fakeGiEntry.drawFunc;
    }
    return giEntry;
}

GetItemEntry Randomizer::GetItemFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogItemId, bool checkObtainability) {
    RandomizerGetData rgData = this->itemLocations[randomizerCheck];
    return GetItemEntryFromRGData(rgData, ogItemId, checkObtainability);
}

RandomizerCheck Randomizer::GetCheckFromActor(s16 actorId, s16 sceneNum, s16 actorParams) {
    return GetCheckObjectFromActor(actorId, sceneNum, actorParams).rc;
}

RandomizerInf Randomizer::GetRandomizerInfFromCheck(RandomizerCheck rc) {
    auto rcIt = rcToRandomizerInf.find(rc);
    if (rcIt == rcToRandomizerInf.end())
        return RAND_INF_MAX;
    
    return rcIt->second;
}

RandomizerCheck Randomizer::GetCheckFromRandomizerInf(RandomizerInf randomizerInf) {
    for (auto const& [key, value] : rcToRandomizerInf) {
        if (value == randomizerInf) return key;
    }

    return RC_UNKNOWN_CHECK;
}

std::thread randoThread;

void GenerateRandomizerImgui() {
    CVar_SetS32("gRandoGenerating", 1);
    CVar_Save();

    std::unordered_map<RandomizerSettingKey, u8> cvarSettings;
    cvarSettings[RSK_LOGIC_RULES] = CVar_GetS32("gRandomizeLogicRules", RO_LOGIC_GLITCHLESS);
    cvarSettings[RSK_ALL_LOCATIONS_REACHABLE] = CVar_GetS32("gRandomizeAllLocationsReachable", RO_GENERIC_ON);
    cvarSettings[RSK_FOREST] = CVar_GetS32("gRandomizeForest", RO_FOREST_CLOSED);
    cvarSettings[RSK_KAK_GATE] = CVar_GetS32("gRandomizeKakarikoGate", RO_KAK_GATE_CLOSED);
    cvarSettings[RSK_DOOR_OF_TIME] = CVar_GetS32("gRandomizeDoorOfTime", RO_DOOROFTIME_CLOSED);
    cvarSettings[RSK_ZORAS_FOUNTAIN] = CVar_GetS32("gRandomizeZorasFountain", 0);
    cvarSettings[RSK_STARTING_AGE] = CVar_GetS32("gRandomizeStartingAge", RO_AGE_CHILD);
    cvarSettings[RSK_GERUDO_FORTRESS] = CVar_GetS32("gRandomizeGerudoFortress", RO_GF_NORMAL);
    cvarSettings[RSK_RAINBOW_BRIDGE] = CVar_GetS32("gRandomizeRainbowBridge", RO_BRIDGE_VANILLA);
    cvarSettings[RSK_RAINBOW_BRIDGE_STONE_COUNT] = CVar_GetS32("gRandomizeStoneCount", 3);
    cvarSettings[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT] = CVar_GetS32("gRandomizeMedallionCount", 6);
    cvarSettings[RSK_RAINBOW_BRIDGE_REWARD_COUNT] = CVar_GetS32("gRandomizeRewardCount", 9);
    cvarSettings[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT] = CVar_GetS32("gRandomizeDungeonCount", 8);
    cvarSettings[RSK_RAINBOW_BRIDGE_TOKEN_COUNT] = CVar_GetS32("gRandomizeTokenCount", 100);
    cvarSettings[RSK_GANONS_TRIALS] = CVar_GetS32("gRandomizeGanonTrial", RO_GANONS_TRIALS_SET_NUMBER);
    cvarSettings[RSK_TRIAL_COUNT] = CVar_GetS32("gRandomizeGanonTrialCount", 6);
    cvarSettings[RSK_STARTING_OCARINA] = CVar_GetS32("gRandomizeStartingOcarina", 0);
    cvarSettings[RSK_SHUFFLE_OCARINA] = CVar_GetS32("gRandomizeShuffleOcarinas", 0) ||
                                        CVar_GetS32("gRandomizeStartingOcarina", 0);
    cvarSettings[RSK_STARTING_KOKIRI_SWORD] = CVar_GetS32("gRandomizeStartingKokiriSword", 0);
    cvarSettings[RSK_SHUFFLE_KOKIRI_SWORD] = CVar_GetS32("gRandomizeShuffleKokiriSword", 0) ||
                                             CVar_GetS32("gRandomizeStartingKokiriSword", 0);
    cvarSettings[RSK_STARTING_DEKU_SHIELD] = CVar_GetS32("gRandomizeStartingDekuShield", 0);
    cvarSettings[RSK_STARTING_SKULLTULA_TOKEN] = CVar_GetS32("gRandomizeStartingSkulltulaToken", 0);
    cvarSettings[RSK_STARTING_MAPS_COMPASSES] = CVar_GetS32("gRandomizeStartingMapsCompasses", RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
    cvarSettings[RSK_SHUFFLE_DUNGEON_REWARDS] = CVar_GetS32("gRandomizeShuffleDungeonReward", RO_DUNGEON_REWARDS_END_OF_DUNGEON);
    cvarSettings[RSK_SHUFFLE_SONGS] = CVar_GetS32("gRandomizeShuffleSongs", RO_SONG_SHUFFLE_SONG_LOCATIONS);
    cvarSettings[RSK_SHUFFLE_TOKENS] = CVar_GetS32("gRandomizeShuffleTokens", RO_TOKENSANITY_OFF);
    cvarSettings[RSK_SHOPSANITY] = CVar_GetS32("gRandomizeShopsanity", RO_SHOPSANITY_OFF);
    cvarSettings[RSK_SHUFFLE_SCRUBS] = CVar_GetS32("gRandomizeShuffleScrubs", RO_SCRUBS_OFF);
    cvarSettings[RSK_SHUFFLE_COWS] = CVar_GetS32("gRandomizeShuffleCows", 0);
    cvarSettings[RSK_SHUFFLE_ADULT_TRADE] = CVar_GetS32("gRandomizeShuffleAdultTrade", 0);
    cvarSettings[RSK_SHUFFLE_MAGIC_BEANS] = CVar_GetS32("gRandomizeShuffleBeans", 0);
    cvarSettings[RSK_SHUFFLE_MERCHANTS] = CVar_GetS32("gRandomizeShuffleMerchants", RO_SHUFFLE_MERCHANTS_OFF);
    cvarSettings[RSK_ENABLE_BOMBCHU_DROPS] = CVar_GetS32("gRandomizeEnableBombchuDrops", 0);
    cvarSettings[RSK_BOMBCHUS_IN_LOGIC] = CVar_GetS32("gRandomizeBombchusInLogic", 0);
    cvarSettings[RSK_SKIP_CHILD_ZELDA] = CVar_GetS32("gRandomizeSkipChildZelda", 0);

    // if we skip child zelda, we start with zelda's letter, and malon starts
    // at the ranch, so we should *not* shuffle the weird egg
    cvarSettings[RSK_SHUFFLE_WEIRD_EGG] = ((CVar_GetS32("gRandomizeSkipChildZelda", 0) == 0) &&
                                            CVar_GetS32("gRandomizeShuffleWeirdEgg", 0));
    
    cvarSettings[RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD] = CVar_GetS32("gRandomizeShuffleGerudoToken", 0);
    cvarSettings[RSK_SHUFFLE_FROG_SONG_RUPEES] = CVar_GetS32("gRandomizeShuffleFrogSongRupees", 0);
    cvarSettings[RSK_ITEM_POOL] = CVar_GetS32("gRandomizeItemPool", RO_ITEM_POOL_BALANCED);
    cvarSettings[RSK_ICE_TRAPS] = CVar_GetS32("gRandomizeIceTraps", RO_ICE_TRAPS_NORMAL);
    cvarSettings[RSK_GOSSIP_STONE_HINTS] = CVar_GetS32("gRandomizeGossipStoneHints", RO_GOSSIP_STONES_NEED_NOTHING);
    cvarSettings[RSK_HINT_CLARITY] = CVar_GetS32("gRandomizeHintClarity", RO_HINT_CLARITY_CLEAR);
    cvarSettings[RSK_HINT_DISTRIBUTION] = CVar_GetS32("gRandomizeHintDistribution", RO_HINT_DIST_BALANCED);
    cvarSettings[RSK_BLUE_FIRE_ARROWS] = CVar_GetS32("gRandomizeBlueFireArrows", 0);
    cvarSettings[RSK_SUNLIGHT_ARROWS] = CVar_GetS32("gRandomizeSunlightArrows", 0);
    cvarSettings[RSK_KEYSANITY] = CVar_GetS32("gRandomizeKeysanity", RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
    cvarSettings[RSK_GERUDO_KEYS] = CVar_GetS32("gRandomizeGerudoKeys", RO_GERUDO_KEYS_VANILLA);
    cvarSettings[RSK_KEYRINGS] = CVar_GetS32("gRandomizeShuffleKeyRings", RO_KEYRINGS_OFF);
    cvarSettings[RSK_KEYRINGS_RANDOM_COUNT] = CVar_GetS32("gRandomizeShuffleKeyRingsRandomCount", 8);
    cvarSettings[RSK_KEYRINGS_FOREST_TEMPLE] = CVar_GetS32("gRandomizeShuffleKeyRingsForestTemple", 0);
    cvarSettings[RSK_KEYRINGS_FIRE_TEMPLE] = CVar_GetS32("gRandomizeShuffleKeyRingsFireTemple", 0);
    cvarSettings[RSK_KEYRINGS_WATER_TEMPLE] = CVar_GetS32("gRandomizeShuffleKeyRingsWaterTemple", 0);
    cvarSettings[RSK_KEYRINGS_SPIRIT_TEMPLE] = CVar_GetS32("gRandomizeShuffleKeyRingsSpiritTemple", 0);
    cvarSettings[RSK_KEYRINGS_SHADOW_TEMPLE] = CVar_GetS32("gRandomizeShuffleKeyRingsShadowTemple", 0);
    cvarSettings[RSK_KEYRINGS_BOTTOM_OF_THE_WELL] = CVar_GetS32("gRandomizeShuffleKeyRingsBottomOfTheWell", 0);
    cvarSettings[RSK_KEYRINGS_GTG] = CVar_GetS32("gRandomizeShuffleKeyRingsGTG", 0);
    cvarSettings[RSK_KEYRINGS_GANONS_CASTLE] = CVar_GetS32("gRandomizeShuffleKeyRingsGanonsCastle", 0);
    cvarSettings[RSK_BOSS_KEYSANITY] = CVar_GetS32("gRandomizeBossKeysanity", RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
    cvarSettings[RSK_GANONS_BOSS_KEY] = CVar_GetS32("gRandomizeShuffleGanonBossKey", RO_GANON_BOSS_KEY_VANILLA);
    cvarSettings[RSK_LACS_STONE_COUNT] = CVar_GetS32("gRandomizeLacsStoneCount", 3);
    cvarSettings[RSK_LACS_MEDALLION_COUNT] = CVar_GetS32("gRandomizeLacsMedallionCount", 6);
    cvarSettings[RSK_LACS_REWARD_COUNT] = CVar_GetS32("gRandomizeLacsRewardCount", 9);
    cvarSettings[RSK_LACS_DUNGEON_COUNT] = CVar_GetS32("gRandomizeLacsDungeonCount", 8);
    cvarSettings[RSK_LACS_TOKEN_COUNT] = CVar_GetS32("gRandomizeLacsTokenCount", 100);
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
    cvarSettings[RSK_LINKS_POCKET] = CVar_GetS32("gRandomizeShuffleDungeonReward", RO_DUNGEON_REWARDS_END_OF_DUNGEON) != RO_DUNGEON_REWARDS_END_OF_DUNGEON ? 
                                        CVar_GetS32("gRandomizeLinksPocket", RO_LINKS_POCKET_DUNGEON_REWARD) :
                                        RO_LINKS_POCKET_DUNGEON_REWARD;

    if (OTRGlobals::Instance->HasMasterQuest() && OTRGlobals::Instance->HasOriginal()) {
        // If both OTRs are loaded.
        cvarSettings[RSK_RANDOM_MQ_DUNGEONS] = CVar_GetS32("gRandomizeMqDungeons", RO_MQ_DUNGEONS_NONE);
        cvarSettings[RSK_MQ_DUNGEON_COUNT] = CVar_GetS32("gRandomizeMqDungeonCount", 12);
    } else if (OTRGlobals::Instance->HasMasterQuest()) {
        // If only Master Quest is loaded.
        cvarSettings[RSK_RANDOM_MQ_DUNGEONS] = RO_MQ_DUNGEONS_SET_NUMBER;
        cvarSettings[RSK_MQ_DUNGEON_COUNT] = 12;
    } else {
        // If only Original Quest is loaded.
        cvarSettings[RSK_RANDOM_MQ_DUNGEONS] = RO_MQ_DUNGEONS_NONE;
        cvarSettings[RSK_MQ_DUNGEON_COUNT] = 0;
    }

    // Enable if any of the entrance rando options are enabled.
    cvarSettings[RSK_SHUFFLE_ENTRANCES] = CVar_GetS32("gRandomizeShuffleDungeonsEntrances", RO_DUNGEON_ENTRANCE_SHUFFLE_OFF) ||
                                          CVar_GetS32("gRandomizeShuffleOverworldEntrances", RO_GENERIC_OFF) ||
                                          CVar_GetS32("gRandomizeShuffleInteriorsEntrances", RO_INTERIOR_ENTRANCE_SHUFFLE_OFF) ||
                                          CVar_GetS32("gRandomizeShuffleGrottosEntrances", RO_GENERIC_OFF) ||
                                          CVar_GetS32("gRandomizeShuffleOwlDrops", RO_GENERIC_OFF) ||
                                          CVar_GetS32("gRandomizeShuffleWarpSongs", RO_GENERIC_OFF) ||
                                          CVar_GetS32("gRandomizeShuffleOverworldSpawns", RO_GENERIC_OFF);

    cvarSettings[RSK_SHUFFLE_DUNGEON_ENTRANCES] = CVar_GetS32("gRandomizeShuffleDungeonsEntrances", RO_DUNGEON_ENTRANCE_SHUFFLE_OFF);
    cvarSettings[RSK_SHUFFLE_OVERWORLD_ENTRANCES] = CVar_GetS32("gRandomizeShuffleOverworldEntrances", RO_GENERIC_OFF);
    cvarSettings[RSK_SHUFFLE_INTERIOR_ENTRANCES] = CVar_GetS32("gRandomizeShuffleInteriorsEntrances", RO_INTERIOR_ENTRANCE_SHUFFLE_OFF);
    cvarSettings[RSK_SHUFFLE_GROTTO_ENTRANCES] = CVar_GetS32("gRandomizeShuffleGrottosEntrances", RO_GENERIC_OFF);
    cvarSettings[RSK_SHUFFLE_OWL_DROPS] = CVar_GetS32("gRandomizeShuffleOwlDrops", RO_GENERIC_OFF);
    cvarSettings[RSK_SHUFFLE_WARP_SONGS] = CVar_GetS32("gRandomizeShuffleWarpSongs", RO_GENERIC_OFF);
    cvarSettings[RSK_SHUFFLE_OVERWORLD_SPAWNS] = CVar_GetS32("gRandomizeShuffleOverworldSpawns", RO_GENERIC_OFF);
    cvarSettings[RSK_MIXED_ENTRANCE_POOLS] = CVar_GetS32("gRandomizeMixedEntrances", RO_GENERIC_OFF);
    cvarSettings[RSK_MIX_DUNGEON_ENTRANCES] = CVar_GetS32("gRandomizeMixDungeons", RO_GENERIC_OFF);
    cvarSettings[RSK_MIX_OVERWORLD_ENTRANCES] = CVar_GetS32("gRandomizeMixOverworld", RO_GENERIC_OFF);
    cvarSettings[RSK_MIX_INTERIOR_ENTRANCES] = CVar_GetS32("gRandomizeMixInteriors", RO_GENERIC_OFF);
    cvarSettings[RSK_MIX_GROTTO_ENTRANCES] = CVar_GetS32("gRandomizeMixGrottos", RO_GENERIC_OFF);
    cvarSettings[RSK_DECOUPLED_ENTRANCES] = CVar_GetS32("gRandomizeDecoupleEntrances", RO_GENERIC_OFF);

    // todo: this efficently when we build out cvar array support
    std::set<RandomizerCheck> excludedLocations;
    std::stringstream excludedLocationStringStream(CVar_GetString("gRandomizeExcludedLocations", ""));
    std::string excludedLocationString;
    while (getline(excludedLocationStringStream, excludedLocationString, ',')) {
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
    static const char* randoLogicRules[2] = { "Glitchless", "No logic" };

    // Open Settings
    static const char* randoForest[3] = { "Closed", "Closed Deku", "Open" };
    static const char* randoKakarikoGate[2] = { "Closed", "Open" };
    static const char* randoDoorOfTime[3] = { "Closed", "Song only", "Open" };
    static const char* randoZorasFountain[3] = { "Closed", "Closed as child", "Open" };
    static const char* randoGerudoFortress[3] = { "Normal", "Fast", "Open" };
    static const char* randoRainbowBridge[7] = { "Vanilla",         "Always open", "Stones", "Medallions",
                                          "Dungeon rewards", "Dungeons",    "Tokens" };
    static const char* randoGanonsTrial[3] = { "Skip", "Set Number", "Random Number" };
    static const char* randoMqDungeons[3] = { "None", "Set Number", "Random Number" };

    // World Settings
    static const char* randoStartingAge[3] = { "Child", "Adult", "Random" };
    static const char* randoShuffleDungeonsEntrances[3] = { "Off", "On", "On + Ganon" };
    static const char* randoShuffleInteriorsEntrances[3] = { "Off", "Simple", "All" };
    static const char* randoBombchusInLogic[2] = { "Off", "On" };
    static const char* randoAmmoDrops[3] = { "On + Bombchu", "Off", "On" };
    static const char* randoHeartDropsAndRefills[4] = { "On", "No Drop", "No Refill", "Off" };

    // Shuffle Settings
    static const char* randoShuffleDungeonRewards[4] = { "End of dungeons", "Any dungeon", "Overworld", "Anywhere" };
    static const char* randoLinksPocket[4] = { "Dungeon Reward", "Advancement", "Anything", "Nothing" };
    static const char* randoShuffleSongs[3] = { "Song Locations", "Dungeon Rewards", "Anywhere" };
    static const char* randoShopsanity[7] = { "Off", "0 Items", "1 Item", "2 Items", "3 Items", "4 Items", "Random" };
    static const char* randoTokensanity[4] = { "Off", "Dungeons", "Overworld", "All Tokens" };
    static const char* randoShuffleScrubs[4] = { "Off", "Affordable", "Expensive", "Random Prices" };
    static const char* randoShuffleMerchants[3] = { "Off", "On (no hints)", "On (with hints)" };

    // Shuffle Dungeon Items Settings
    static const char* randoShuffleMapsAndCompasses[6] = { "Start With",  "Vanilla",   "Own Dungeon",
                                                    "Any Dungeon", "Overworld", "Anywhere" };
    static const char* randoShuffleSmallKeys[6] = { "Start With",  "Vanilla",   "Own Dungeon",
                                             "Any Dungeon", "Overworld", "Anywhere" };
    static const char* randoShuffleGerudoFortressKeys[4] = { "Vanilla", "Any Dungeon", "Overworld", "Anywhere" };
    static const char* randoShuffleBossKeys[6] = { "Start With",  "Vanilla",   "Own Dungeon",
                                            "Any Dungeon", "Overworld", "Anywhere" };
    static const char* randoShuffleGanonsBossKey[12] = {"Vanilla", "Own dungeon", "Start with", 
                                                "Any Dungeon", "Overworld", "Anywhere", 
                                                "LACS-Vanilla", "LACS-Medallions", "LACS-Stones", 
                                                "LACS-Rewards", "LACS-Dungeons", "LACS-Tokens"};
    static const char* randoShuffleKeyRings[4] = { "Off", "Random", "Count", "Selection" };

    // Misc Settings
    static const char* randoGossipStoneHints[4] = { "No Hints", "Need Nothing", "Mask of Truth", "Stone of Agony" };
    static const char* randoHintClarity[3] = { "Obscure", "Ambiguous", "Clear" };
    static const char* randoHintDistribution[4] = { "Useless", "Balanced", "Strong", "Very Strong" };
    static const char* randoStartingTime[2] = { "Day", "Night" };
    static const char* randoRandomTrapDamage[3] = { "Basic", "Advanced", "Off" };

    // Item Pool Settings
    static const char* randoItemPool[4] = { "Plentiful", "Balanced", "Scarce", "Minimal" };
    static const char* randoIceTraps[5] = { "Off", "Normal", "Extra", "Mayhem", "Onslaught" };

    ImGui::SetNextWindowSize(ImVec2(920, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Randomizer Editor", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    DrawPresetSelector(PRESET_TYPE_RANDOMIZER);

    bool disableEditingRandoSettings = CVar_GetS32("gRandoGenerating", 0) || CVar_GetS32("gOnFileSelectNameEntry", 0);
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, disableEditingRandoSettings);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (disableEditingRandoSettings ? 0.5f : 1.0f));

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

    UIWidgets::PaddedSeparator();

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    static ImVec2 cellPadding(8.0f, 8.0f);

    if (ImGui::BeginTabBar("Randomizer Settings", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
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
                UIWidgets::EnhancementCombobox("gRandomizeForest", randoForest, RO_FOREST_MAX, RO_FOREST_CLOSED);

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
                UIWidgets::EnhancementCombobox("gRandomizeKakarikoGate", randoKakarikoGate, RO_KAK_GATE_MAX, RO_KAK_GATE_CLOSED);

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
                UIWidgets::EnhancementCombobox("gRandomizeDoorOfTime", randoDoorOfTime, RO_DOOROFTIME_MAX, RO_DOOROFTIME_CLOSED);

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
                UIWidgets::EnhancementCombobox("gRandomizeZorasFountain", randoZorasFountain, RO_ZF_MAX, RO_ZF_CLOSED);

                UIWidgets::PaddedSeparator();

                ImGui::EndChild();

                // COLUMN 2 - World Settings
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildMiscWorldSettings", ImVec2(0,-8));
                ImGui::PushItemWidth(-FLT_MIN);

                //Starting Age
                //Disabled when Forest is set to Closed or under very specific conditions
                //RANDOTODO: Replace magic number checks with enums
                bool disableRandoStartingAge =  (CVar_GetS32("gRandomizeLogicRules", RO_LOGIC_GLITCHLESS) == RO_LOGIC_GLITCHLESS) &&
                ((CVar_GetS32("gRandomizeForest", RO_FOREST_CLOSED) == RO_FOREST_CLOSED) ||
                 ((CVar_GetS32("gRandomizeDoorOfTime", RO_DOOROFTIME_CLOSED) == RO_DOOROFTIME_CLOSED) &&
                 (CVar_GetS32("gRandomizeShuffleOcarinas", 0) == 0)));  // ocarinas not shuffled
                    
                static const char* disableRandoStartingAgeText = "This option is disabled due to other options making the game unbeatable.";
                ImGui::Text(Settings::StartingAge.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Choose which age Link will start as.\n\n"
                    "Starting as adult means you start with the Master Sword in your inventory.\n"
                    "The child option is forcefully set if it would conflict with other options."    
                );
               if (disableRandoStartingAge) {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                }    
                UIWidgets::EnhancementCombobox("gRandomizeStartingAge", randoStartingAge, RO_AGE_MAX, RO_AGE_CHILD);
                if (disableRandoStartingAge) {
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                       ImGui::SetTooltip("%s", disableRandoStartingAgeText);
                    }
                    CVar_SetS32("gRandomizeStartingAge", RO_AGE_CHILD);
                    ImGui::PopStyleVar(1);
                    ImGui::PopItemFlag();
                }                
                
                UIWidgets::PaddedSeparator();

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
                UIWidgets::EnhancementCombobox("gRandomizeGerudoFortress", randoGerudoFortress, RO_GF_MAX, RO_GF_NORMAL);

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

                UIWidgets::EnhancementCombobox("gRandomizeRainbowBridge", randoRainbowBridge, RO_BRIDGE_MAX, RO_BRIDGE_VANILLA);
                ImGui::PopItemWidth();
                switch (CVar_GetS32("gRandomizeRainbowBridge", RO_BRIDGE_VANILLA)) {
                    case RO_BRIDGE_ALWAYS_OPEN:
                        break;
                    case RO_BRIDGE_VANILLA:
                        break;
                    case RO_BRIDGE_STONES:
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Stone Count: %d", "##RandoStoneCount",
                                                        "gRandomizeStoneCount", 1, 3, "", 3, true);
                        break;
                    case RO_BRIDGE_MEDALLIONS:
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Medallion Count: %d", "##RandoMedallionCount",
                                                        "gRandomizeMedallionCount", 1, 6, "", 6, true);
                        break;
                    case RO_BRIDGE_DUNGEON_REWARDS:
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Reward Count: %d", "##RandoRewardCount",
                                                        "gRandomizeRewardCount", 1, 9, "", 9, true);
                        break;
                    case RO_BRIDGE_DUNGEONS:
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Dungeon Count: %d", "##RandoDungeonCount",
                                                        "gRandomizeDungeonCount", 1, 8, "", 8, true);
                        break;
                    case RO_BRIDGE_TOKENS:
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
                UIWidgets::EnhancementCombobox("gRandomizeGanonTrial", randoGanonsTrial, RO_GANONS_TRIALS_MAX, RO_GANONS_TRIALS_SET_NUMBER);
                ImGui::PopItemWidth();
                if (CVar_GetS32("gRandomizeGanonTrial", RO_GANONS_TRIALS_SET_NUMBER) == RO_GANONS_TRIALS_SET_NUMBER) {
                    ImGui::Dummy(ImVec2(0.0f, 0.0f));
                    UIWidgets::EnhancementSliderInt("Ganon's Trial Count: %d", "##RandoTrialCount",
                                                    "gRandomizeGanonTrialCount", 1, 6, "", 6, true);
                    UIWidgets::InsertHelpHoverText("Set the number of trials required to enter Ganon's Tower.");
                }

                UIWidgets::PaddedSeparator();

                if (OTRGlobals::Instance->HasMasterQuest() && OTRGlobals::Instance->HasOriginal()) {
                    ImGui::PushItemWidth(-FLT_MIN);
                    ImGui::Text("Master Quest Dungeons");
                    UIWidgets::InsertHelpHoverText(
                        "Sets the number of Master Quest Dungeons that are shuffled into the pool.\n"
                        "\n"
                        "None - All Dungeons will be their Vanilla versions.\n"
                        "\n"
                        "Set Number - Select a number of dungeons that will be their Master Quest versions "
                        "using the slider below. Which dungeons are set to be the Master Quest variety will be random.\n"
                        "\n"
                        "Random Number - A Random number and set of dungeons will be their Master Quest varieties."
                    );
                    UIWidgets::EnhancementCombobox("gRandomizeMqDungeons", randoMqDungeons, RO_MQ_DUNGEONS_MAX, RO_MQ_DUNGEONS_NONE);
                    ImGui::PopItemWidth();
                    if (CVar_GetS32("gRandomizeMqDungeons", RO_MQ_DUNGEONS_NONE) == RO_MQ_DUNGEONS_SET_NUMBER) {
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Master Quest Dungeon Count: %d", "##RandoMqDungeonCount",
                            "gRandomizeMqDungeonCount", 1, 12, "", CVar_GetS32("gRandomizeMqDungeonCount", 12), true);
                    }
                }

                ImGui::EndChild();

                // COLUMN 3 - Shuffle Entrances
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::BeginChild("ChildShuffleEntrances", ImVec2(0, -8));
                ImGui::PushItemWidth(-FLT_MIN);

                // Shuffle Dungeon Entrances
                ImGui::Text("Shuffle Dungeon Entrances");
                UIWidgets::InsertHelpHoverText(
                    "Shuffle the pool of dungeon entrances, including Bottom of the Well, Ice Cavern and Gerudo Training Grounds.\n"
                    "\n"
                    "Shuffling Ganon's Castle can be enabled separately.\n"
                    "\n"
                    "Additionally, the entrances of Deku Tree, Fire Temple, Bottom of the Well and Gerudo Training Ground are opened for both child and adult.\n"
                    "\n"
                    "- Deku Tree will be open for adult after Mido has seen child Link with a sword and shield.\n"
                    "- Bottom of the Well will be open for adult after playing Song of Storms to the Windmill guy as child.\n"
                    "- Gerudo Training Ground will be open for child after adult has paid to open the gate once."
                );
                UIWidgets::EnhancementCombobox("gRandomizeShuffleDungeonsEntrances", randoShuffleDungeonsEntrances, RO_DUNGEON_ENTRANCE_SHUFFLE_MAX, RO_DUNGEON_ENTRANCE_SHUFFLE_OFF);

                UIWidgets::PaddedSeparator();

                // Shuffle Overworld Entrances
                UIWidgets::EnhancementCheckbox("Shuffle Overworld Entrances", "gRandomizeShuffleOverworldEntrances");
                UIWidgets::InsertHelpHoverText(
                    "Shuffle the pool of Overworld entrances, which corresponds to almost all loading zones between overworld areas.\n"
                    "\n"
                    "Some entrances are unshuffled to avoid issues:\n"
                    "- Hyrule Castle Courtyard and Garden entrance\n"
                    "- Both Market Back Alley entrances\n"
                    "- Gerudo Valley to Lake Hylia (unless entrances are decoupled)"
                );

                UIWidgets::PaddedSeparator();

                // Shuffle Interior Entrances
                ImGui::Text("Shuffle Interior Entrances");
                UIWidgets::InsertHelpHoverText(
                    "Shuffle the pool of interior entrances which contains most Houses and all Great Fairies.\n"
                    "\n"
                    "All - An extended version of 'Simple' with some extra places:\n"
                    "- Windmill\n"
                    "- Link's House\n"
                    "- Temple of Time\n"
                    "- Kakariko Potion Shop"
                );
                UIWidgets::EnhancementCombobox("gRandomizeShuffleInteriorsEntrances", randoShuffleInteriorsEntrances, RO_INTERIOR_ENTRANCE_SHUFFLE_MAX, RO_INTERIOR_ENTRANCE_SHUFFLE_OFF);

                UIWidgets::PaddedSeparator();

                // Shuffle Grotto Entrances
                UIWidgets::EnhancementCheckbox("Shuffle Grotto Entrances", "gRandomizeShuffleGrottosEntrances");
                UIWidgets::InsertHelpHoverText(
                    "Shuffle the pool of grotto entrances, including all graves, small Fairy fountains and the Deku Theatre."
                );

                UIWidgets::PaddedSeparator();

                // Shuffle Owl Drops
                UIWidgets::EnhancementCheckbox("Shuffle Owl Drops", "gRandomizeShuffleOwlDrops");
                UIWidgets::InsertHelpHoverText(
                    "Randomize where Kaepora Gaebora (the Owl) drops you at when you talk "
                    "to him at Lake Hylia or at the top of Death Mountain Trail."
                );

                UIWidgets::PaddedSeparator();

                // Shuffle Warp Songs
                UIWidgets::EnhancementCheckbox("Shuffle Warp Songs", "gRandomizeShuffleWarpSongs");
                UIWidgets::InsertHelpHoverText(
                    "Randomize where each of the 6 warp songs leads to."
                );

                UIWidgets::PaddedSeparator();

                // Shuffle Overworld Spawns
                UIWidgets::EnhancementCheckbox("Shuffle Overworld Spawns", "gRandomizeShuffleOverworldSpawns");
                UIWidgets::InsertHelpHoverText(
                    "Randomize where you start as Child or Adult when loading a save in the Overworld. This "
                    "means you may not necessarily spawn inside Link's House or Temple of Time.\n"
                    "\n"
                    "This stays consistent after saving and loading the game again.\n"
                    "\n"
                    "Keep in mind you may need to temporarily disable the \"Remember Save Location\" time saver to "
                    "be able use the spawn positions, especially if they are the only logical way to get to certain areas."
                );

                UIWidgets::PaddedSeparator();

                // Decouple Entrances
                UIWidgets::EnhancementCheckbox("Decouple Entrances", "gRandomizeDecoupleEntrances");
                UIWidgets::InsertHelpHoverText(
                    "Decouple entrances when shuffling them. This means you are no longer guaranteed "
                    "to end up back where you came from when you go back through an entrance.\n"
                    "\n"
                    "This also adds the one-way entrance from Gerudo Valley to Lake Hylia in the pool of "
                    "overworld entrances when they are shuffled."
                );

                UIWidgets::PaddedSeparator();

                // Mixed Entrance Pools
                UIWidgets::EnhancementCheckbox("Mixed Entrance Pools", "gRandomizeMixedEntrances");
                UIWidgets::InsertHelpHoverText(
                    "Shuffle entrances into a mixed pool instead of separate ones.\n"
                    "\n"
                    "For example, enabling the settings to shuffle grotto, dungeon, and overworld entrances and "
                    "selecting grotto and dungeon entrances here will allow a dungeon to be inside a grotto or "
                    "vice versa, while overworld entrances are shuffled in their own separate pool and indoors stay vanilla."
                );

                if (CVar_GetS32("gRandomizeMixedEntrances", RO_GENERIC_OFF)) {
                    if (CVar_GetS32("gRandomizeShuffleDungeonsEntrances", RO_GENERIC_OFF)) {
                        UIWidgets::Spacer(0);
                        ImGui::SetCursorPosX(20);
                        UIWidgets::EnhancementCheckbox("Mix Dungeons", "gRandomizeMixDungeons");
                        UIWidgets::InsertHelpHoverText("Dungeon entrances will be part of the mixed pool");
                    }
                    if (CVar_GetS32("gRandomizeShuffleOverworldEntrances", RO_GENERIC_OFF)) {
                        UIWidgets::Spacer(0);
                        ImGui::SetCursorPosX(20);
                        UIWidgets::EnhancementCheckbox("Mix Overworld", "gRandomizeMixOverworld");
                        UIWidgets::InsertHelpHoverText("Overworld entrances will be part of the mixed pool");
                    }
                    if (CVar_GetS32("gRandomizeShuffleInteriorsEntrances", RO_GENERIC_OFF)) {
                        UIWidgets::Spacer(0);
                        ImGui::SetCursorPosX(20);
                        UIWidgets::EnhancementCheckbox("Mix Interiors", "gRandomizeMixInteriors");
                        UIWidgets::InsertHelpHoverText("Interior entrances will be part of the mixed pool");
                    }
                    if (CVar_GetS32("gRandomizeShuffleGrottosEntrances", RO_GENERIC_OFF)) {
                        UIWidgets::Spacer(0);
                        ImGui::SetCursorPosX(20);
                        UIWidgets::EnhancementCheckbox("Mix Grottos", "gRandomizeMixGrottos");
                        UIWidgets::InsertHelpHoverText("Grotto entrances will be part of the mixed pool");
                    }
                }

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
                UIWidgets::EnhancementCombobox("gRandomizeShuffleSongs", randoShuffleSongs, RO_SONG_SHUFFLE_MAX, RO_SONG_SHUFFLE_SONG_LOCATIONS);

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
                UIWidgets::EnhancementCombobox("gRandomizeShuffleTokens", randoTokensanity, RO_TOKENSANITY_MAX, RO_TOKENSANITY_OFF);

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
                static const char* disableShuffleKokiriSwordText = "This option is disabled because \"Start with Kokiri Sword\" is enabled.";
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
                static const char* disableShuffleOcarinasText = "This option is disabled because \"Start with Fairy Ocarina\" is enabled.";
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
                static const char* disableShuffleWeirdEggText = "This option is disabled because \"Skip Child Zelda\" is enabled.";
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
                UIWidgets::EnhancementCombobox("gRandomizeShopsanity", randoShopsanity, RO_SHOPSANITY_MAX, RO_SHOPSANITY_OFF);

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
                UIWidgets::EnhancementCombobox("gRandomizeShuffleScrubs", randoShuffleScrubs, RO_SCRUBS_MAX, RO_SCRUBS_OFF);

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

                // Shuffle Merchants
                ImGui::Text(Settings::ShuffleMerchants.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Enabling this adds a Giant's Knife and a pack of Bombchus to the item pool "
                    "and changes both Medigoron and the Haunted Wasteland Carpet Salesman to sell "
                    "a random item once at the price of 200 rupees.\n\n"
                    "On (no hints) - Salesmen will be included but won't tell you what you'll get.\n"
                    "On (with hints) - Salesmen will be included and you'll know what you're buying."
                );
                UIWidgets::EnhancementCombobox("gRandomizeShuffleMerchants", randoShuffleMerchants, RO_SHUFFLE_MERCHANTS_MAX, RO_SHUFFLE_MERCHANTS_OFF);

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
                UIWidgets::EnhancementCombobox("gRandomizeShuffleDungeonReward", randoShuffleDungeonRewards, RO_DUNGEON_REWARDS_MAX, RO_DUNGEON_REWARDS_END_OF_DUNGEON);

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
                UIWidgets::EnhancementCombobox("gRandomizeStartingMapsCompasses", randoShuffleMapsAndCompasses, RO_DUNGEON_ITEM_LOC_MAX, RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);

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
                UIWidgets::EnhancementCombobox("gRandomizeKeysanity", randoShuffleSmallKeys, RO_DUNGEON_ITEM_LOC_MAX, RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);

                UIWidgets::PaddedSeparator();

                 // Key Rings
                ImGui::Text(Settings::KeyRings.GetName().c_str());
                UIWidgets::InsertHelpHoverText(
                    "Keyrings will replace all small keys from a particular dungeon with a single keyring that awards all keys for it's associated dungeon\n"
                    "\n"
                    "Off - No dungeons will have their keys replaced with keyrings.\n"
                    "\n"
                    "Random - A random amount of dungeons(0-8) will have their keys replaced with keyrings.\n"
                    "\n"
                    "Count - A specified amount of randomly selected dungeons will have their keys replaced with keyrings.\n"
                    "\n"
                    "Selection - Hand select which dungeons will have their keys replaced with keyrings."
                );
                UIWidgets::EnhancementCombobox("gRandomizeShuffleKeyRings", randoShuffleKeyRings, RO_KEYRINGS_MAX, RO_KEYRINGS_OFF);
                ImGui::PopItemWidth();
                switch (CVar_GetS32("gRandomizeShuffleKeyRings", RO_KEYRINGS_OFF)) {
                    case RO_KEYRINGS_COUNT:
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Key Ring Count: %d", "##RandomizeShuffleKeyRingsRandomCount",
                                                        "gRandomizeShuffleKeyRingsRandomCount", 1, 8, "", 8, true);
                        break;
                    case RO_KEYRINGS_SELECTION:
                        UIWidgets::EnhancementCheckbox("Forest Temple##RandomizeShuffleKeyRings", "gRandomizeShuffleKeyRingsForestTemple");
                        UIWidgets::EnhancementCheckbox("Fire Temple##RandomizeShuffleKeyRings", "gRandomizeShuffleKeyRingsFireTemple");
                        UIWidgets::EnhancementCheckbox("Water Temple##RandomizeShuffleKeyRings", "gRandomizeShuffleKeyRingsWaterTemple");
                        UIWidgets::EnhancementCheckbox("Spirit Temple##RandomizeShuffleKeyRings", "gRandomizeShuffleKeyRingsSpiritTemple");
                        UIWidgets::EnhancementCheckbox("Shadow Temple##RandomizeShuffleKeyRings", "gRandomizeShuffleKeyRingsShadowTemple");
                        UIWidgets::EnhancementCheckbox("Bottom of the Well##RandomizeShuffleKeyRings", "gRandomizeShuffleKeyRingsBottomOfTheWell");
                        UIWidgets::EnhancementCheckbox("GTG##RandomizeShuffleKeyRings", "gRandomizeShuffleKeyRingsGTG");
                        UIWidgets::EnhancementCheckbox("Ganon's Castle##RandomizeShuffleKeyRings", "gRandomizeShuffleKeyRingsGanonsCastle");
                        break;
                    default:
                        break;
                }
                ImGui::PushItemWidth(-FLT_MIN);

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
                UIWidgets::EnhancementCombobox("gRandomizeGerudoKeys", randoShuffleGerudoFortressKeys, RO_GERUDO_KEYS_MAX, RO_GERUDO_KEYS_VANILLA);

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
                UIWidgets::EnhancementCombobox("gRandomizeBossKeysanity", randoShuffleBossKeys, RO_DUNGEON_ITEM_LOC_MAX, RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);

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
                    "Anywhere - Ganon's Boss Key Key can appear anywhere in the world.\n"
                    "\n"
                    "LACS - These settings put the boss key on the Light Arrow Cutscene location, from Zelda in Temple of Time as adult, with differing requirements:\n"
                    "- Vanilla: Obtain the Shadow Medallion and Spirit Medallion\n"
                    "- Medallions: Obtain the specified amount of medallions.\n"
                    "- Stones: Obtain the specified amount of spiritual stones.\n"
                    "- Dungeon rewards: Obtain the specified total sum of spiritual stones or medallions.\n"
                    "- Dungeons: Complete the specified amount of dungeons. Dungeons are considered complete after stepping in to the blue warp after the boss.\n"
                    "- Tokens: Obtain the specified amount of Skulltula tokens."
                );
                UIWidgets::EnhancementCombobox("gRandomizeShuffleGanonBossKey", randoShuffleGanonsBossKey, RO_GANON_BOSS_KEY_MAX, RO_GANON_BOSS_KEY_VANILLA);
                ImGui::PopItemWidth();
                switch (CVar_GetS32("gRandomizeShuffleGanonBossKey", RO_GANON_BOSS_KEY_VANILLA)) {
                    case RO_GANON_BOSS_KEY_LACS_MEDALLIONS:
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Medallion Count: %d", "##RandoLacsMedallionCount",
                                                        "gRandomizeLacsMedallionCount", 1, 6, "", 6, true);
                        break;
                    case RO_GANON_BOSS_KEY_LACS_STONES:
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Stone Count: %d", "##RandoLacsStoneCount",
                                                        "gRandomizeLacsStoneCount", 1, 3, "", 3, true);
                        break;
                    case RO_GANON_BOSS_KEY_LACS_REWARDS:
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Reward Count: %d", "##RandoLacsRewardCount",
                                                        "gRandomizeLacsRewardCount", 1, 9, "", 9, true);
                        break;
                    case RO_GANON_BOSS_KEY_LACS_DUNGEONS:
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Dungeon Count: %d", "##RandoLacsDungeonCount",
                                                        "gRandomizeLacsDungeonCount", 1, 8, "", 8, true);
                        break;
                    case RO_GANON_BOSS_KEY_LACS_TOKENS:
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        UIWidgets::EnhancementSliderInt("Token Count: %d", "##RandoLacsTokenCount",
                                                        "gRandomizeLacsTokenCount", 1, 100, "", 100, true);
                        break;
                    default:
                        break;
                }
                ImGui::PushItemWidth(-FLT_MIN);

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
                static const char* disableChildStealthText = "This option is disabled because \"Skip Child Zelda\" is enabled";
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
                UIWidgets::EnhancementCombobox("gRandomizeItemPool", randoItemPool, RO_ITEM_POOL_MAX, RO_ITEM_POOL_BALANCED);
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
                UIWidgets::EnhancementCombobox("gRandomizeIceTraps", randoIceTraps, RO_ICE_TRAPS_MAX, RO_ICE_TRAPS_NORMAL);

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
                UIWidgets::EnhancementCombobox("gRandomizeGossipStoneHints", randoGossipStoneHints, RO_GOSSIP_STONES_MAX, RO_GOSSIP_STONES_NEED_NOTHING);
                if (CVar_GetS32("gRandomizeGossipStoneHints", RO_GOSSIP_STONES_NEED_NOTHING) != RO_GOSSIP_STONES_NONE) {
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
                    UIWidgets::EnhancementCombobox("gRandomizeHintClarity", randoHintClarity, RO_HINT_CLARITY_MAX, RO_HINT_CLARITY_CLEAR);

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
                    UIWidgets::EnhancementCombobox("gRandomizeHintDistribution", randoHintDistribution, RO_HINT_DIST_MAX, RO_HINT_DIST_BALANCED);
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
                for (auto [rcArea, rcObjects] : RandomizerCheckObjects::GetAllRCObjectsByArea()) {
                    bool hasItems = false;
                    for (auto [randomizerCheck, rcObject] : rcObjects) {
                        if (rcObject.visibleInImgui && !excludedLocations.count(rcObject.rc) &&
                            locationSearch.PassFilter(rcObject.rcSpoilerName.c_str())) {

                            hasItems = true;
                            break;
                        }
                    }

                    if (hasItems) {
                        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                        if (ImGui::TreeNode(RandomizerCheckObjects::GetRCAreaName(rcArea).c_str())) {
                            for (auto [randomizerCheck, rcObject] : rcObjects) {
                                if (rcObject.visibleInImgui && !excludedLocations.count(rcObject.rc) &&
                                    locationSearch.PassFilter(rcObject.rcSpoilerName.c_str())) {

                                    if (ImGui::ArrowButton(std::to_string(rcObject.rc).c_str(), ImGuiDir_Right)) {
                                        excludedLocations.insert(rcObject.rc);
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
                                    ImGui::Text(rcObject.rcShortName.c_str());
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
                for (auto [rcArea, rcObjects] : RandomizerCheckObjects::GetAllRCObjectsByArea()) {
                    bool hasItems = false;
                    for (auto [randomizerCheck, rcObject] : rcObjects) {
                        if (rcObject.visibleInImgui && excludedLocations.count(rcObject.rc)) {
                            hasItems = true;
                            break;
                        }
                    }

                    if (hasItems) {
                        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                        if (ImGui::TreeNode(RandomizerCheckObjects::GetRCAreaName(rcArea).c_str())) {
                            for (auto [randomizerCheck, rcObject] : rcObjects) {
                                auto elfound = excludedLocations.find(rcObject.rc);
                                if (rcObject.visibleInImgui && elfound != excludedLocations.end()) {
                                    if (ImGui::ArrowButton(std::to_string(rcObject.rc).c_str(), ImGuiDir_Left)) {
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
                                    ImGui::Text(rcObject.rcShortName.c_str());
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
                UIWidgets::EnhancementCombobox("gRandomizeLogicRules", randoLogicRules, RO_LOGIC_MAX, RO_LOGIC_GLITCHLESS);
                if (CVar_GetS32("gRandomizeLogicRules", RO_LOGIC_GLITCHLESS) == RO_LOGIC_GLITCHLESS) {
                    ImGui::SameLine();
                    UIWidgets::EnhancementCheckbox(Settings::LocationsReachable.GetName().c_str(), "gRandomizeAllLocationsReachable", false, "", UIWidgets::CheckboxGraphics::Cross, RO_GENERIC_ON);
                    UIWidgets::InsertHelpHoverText(
                        "When this options is enabled, the randomizer will "
                        "guarantee that every item is obtainable and every "
                        "location is reachable. When disabled, only "
                        "required items and locations to beat the game "
                        "will be guaranteed reachable."
                    );
                }

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
                if (CVar_GetS32("gRandomizeShuffleDungeonReward", RO_DUNGEON_REWARDS_END_OF_DUNGEON) != RO_DUNGEON_REWARDS_END_OF_DUNGEON) {
                    ImGui::Text(Settings::LinksPocketItem.GetName().c_str());
                    UIWidgets::EnhancementCombobox("gRandomizeLinksPocket", randoLinksPocket, RO_LINKS_POCKET_MAX, RO_LINKS_POCKET_DUNGEON_REWARD);
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
                UIWidgets::EnhancementSliderInt("Gold Skulltula Tokens: %d", "##RandoStartingSkulltulaToken", "gRandomizeStartingSkulltulaToken", 0, 100, "", 0, true);
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

CustomMessageEntry Randomizer::GetWarpSongMessage(u16 textId, bool mysterious) {
    CustomMessageEntry messageEntry = CustomMessageManager::Instance->RetrieveMessage(
        Randomizer::hintMessageTableID, TEXT_WARP_RANDOM_REPLACED_TEXT);
    if (mysterious) {
        std::vector<std::string> locationName ={
            "a mysterious place",
            "ein mysteriöser Ort",
            "un endroit mystérieux",
        };

        CustomMessageManager::ReplaceStringInMessage(messageEntry, "{{location}}", locationName[0],
            locationName[1], locationName[2]);
        return messageEntry;
    }

    std::string locationName;
    switch (textId) {
        case TEXT_WARP_MINUET_OF_FOREST:
            locationName = std::string(gSaveContext.warpMinuetText);
            break;
        case TEXT_WARP_BOLERO_OF_FIRE:
            locationName = std::string(gSaveContext.warpBoleroText);
            break;
        case TEXT_WARP_SERENADE_OF_WATER:
            locationName = std::string(gSaveContext.warpSerenadeText);
            break;
        case TEXT_WARP_REQUIEM_OF_SPIRIT:
            locationName = std::string(gSaveContext.warpRequiemText);
            break;
        case TEXT_WARP_NOCTURNE_OF_SHADOW:
            locationName = std::string(gSaveContext.warpNocturneText);
            break;
        case TEXT_WARP_PRELUDE_OF_LIGHT:
            locationName = std::string(gSaveContext.warpPreludeText);
            break;
    }

    CustomMessageManager::ReplaceStringInMessage(messageEntry, "{{location}}", locationName);
    return messageEntry;
}

CustomMessageEntry Randomizer::GetMerchantMessage(RandomizerInf randomizerInf, u16 textId, bool mysterious) {
    CustomMessageEntry messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::merchantMessageTableID, textId);
    RandomizerCheck rc = GetCheckFromRandomizerInf(randomizerInf);
    RandomizerGet shopItemGet = this->itemLocations[rc].rgID;
    std::vector<std::string> shopItemName;
    if (mysterious) {
        shopItemName = {
            "mysterious item",
            "mysteriösen Gegenstand",
            "objet mystérieux"
        };
    // TODO: This should eventually be replaced with a full fledged trick model & trick name system
    } else if (shopItemGet == RG_ICE_TRAP) {
        shopItemGet = this->itemLocations[rc].fakeRgID;
        shopItemName = {
            std::string(this->itemLocations[rc].trickName),
            std::string(this->itemLocations[rc].trickName),
            std::string(this->itemLocations[rc].trickName)
        };
    } else { 
        shopItemName = EnumToSpoilerfileGetName[shopItemGet];
    }
    u16 shopItemPrice = merchantPrices[rc];

    if (textId == TEXT_SCRUB_RANDOM && shopItemPrice == 0) {
        messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::merchantMessageTableID, TEXT_SCRUB_RANDOM_FREE);
    }

    CustomMessageManager::ReplaceStringInMessage(messageEntry, "{{item}}", shopItemName[0], shopItemName[1], shopItemName[2]);
    CustomMessageManager::ReplaceStringInMessage(messageEntry, "{{price}}", std::to_string(shopItemPrice));
    return messageEntry;
}

const char* mapGetItemHints[3][2] = {
    { " It's ordinary.", " It's masterful!" },
    { "&Sieht aus wie immer.", " &Man kann darauf die Worte&%r\"Master Quest\"%w entziffern..." },
    { "&Elle vous semble %rordinaire%w.", "&Étrange... les mots %r\"Master&Quest\"%w sont gravés dessus." },
};

CustomMessageEntry Randomizer::GetMapGetItemMessageWithHint(GetItemEntry itemEntry) {
    CustomMessageEntry messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::getItemMessageTableID, itemEntry.getItemId);
    int sceneNum;
    switch (itemEntry.getItemId) {
        case RG_DEKU_TREE_MAP:
            sceneNum = SCENE_YDAN;
            break;
        case RG_DODONGOS_CAVERN_MAP:
            sceneNum = SCENE_DDAN;
            break;
        case RG_JABU_JABUS_BELLY_MAP:
            sceneNum = SCENE_BDAN;
            break;
        case RG_FOREST_TEMPLE_MAP:
            sceneNum = SCENE_BMORI1;
            break;
        case RG_FIRE_TEMPLE_MAP:
            sceneNum = SCENE_HIDAN;
            break;
        case RG_WATER_TEMPLE_MAP:
            sceneNum = SCENE_MIZUSIN;
            break;
        case RG_SPIRIT_TEMPLE_MAP:
            sceneNum = SCENE_JYASINZOU;
            break;
        case RG_SHADOW_TEMPLE_MAP:
            sceneNum = SCENE_HAKADAN;
            break;
        case RG_BOTTOM_OF_THE_WELL_MAP:
            sceneNum = SCENE_HAKADANCH;
            break;
        case RG_ICE_CAVERN_MAP:
            sceneNum = SCENE_ICE_DOUKUTO;
            break;
    }

    if (this->masterQuestDungeons.empty() || this->masterQuestDungeons.size() >= 12) {
        CustomMessageManager::ReplaceStringInMessage(messageEntry, "{{typeHint}}", "");
    } else if (ResourceMgr_IsSceneMasterQuest(sceneNum)) {
        CustomMessageManager::ReplaceStringInMessage(messageEntry, "{{typeHint}}", mapGetItemHints[0][1], mapGetItemHints[1][1], mapGetItemHints[2][1]);
    } else {
        CustomMessageManager::ReplaceStringInMessage(messageEntry, "{{typeHint}}", mapGetItemHints[0][0], mapGetItemHints[1][0], mapGetItemHints[2][0]);
    }

    return messageEntry;
}

void CreateGetItemMessages(std::vector<GetItemMessage> messageEntries) {
    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(Randomizer::getItemMessageTableID);
    for (const GetItemMessage& messageEntry : messageEntries) {
        customMessageManager->CreateGetItemMessage(Randomizer::getItemMessageTableID, messageEntry.giid, messageEntry.iid,
                                                    { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, messageEntry.english,
                                                        messageEntry.german, messageEntry.french });
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
                rupeeText = "\x05\x03 5 {{rupee}}\x05\x00";
                break;
            case TEXT_RED_RUPEE:
                rupeeText = "\x05\x01 20 {{rupee}}\x05\x00";
                break;
            case TEXT_PURPLE_RUPEE:
                rupeeText = "\x05\x05 50 {{rupee}}\x05\x00";
                break;
            case TEXT_HUGE_RUPEE:
                rupeeText = "\x05\x06 200 {{rupee}}\x05\x00";
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

CustomMessageEntry Randomizer::GetRupeeMessage(u16 rupeeTextId) {
    CustomMessageEntry messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::rupeeMessageTableID, rupeeTextId);
    std::string englishName = RandomElement(englishRupeeNames);
    std::string germanName = RandomElement(germanRupeeNames);
    std::string frenchName = RandomElement(frenchRupeeNames);
    CustomMessageManager::ReplaceStringInMessage(messageEntry, "{{rupee}}", englishName, germanName, frenchName);
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
    for (unsigned int i = 0; i <= (NUM_NAVI_MESSAGES - 1); i++) {
        customMessageManager->CreateMessage(Randomizer::NaviRandoMessageTableID, i,
                                            { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM, NaviMessages[i].english,
                                              NaviMessages[i].german, NaviMessages[i].french });
    }
}

CustomMessageMinimal IceTrapMessages[NUM_ICE_TRAP_MESSAGES] = {
    { "You are a %bFOOL%w!",
      "Du bist ein %bDUMMKOPF%w!",
      "%bPauvre fou%w..." },

    { "You are a %bFOWL%w!",
      "Du bist eine %bFrostbeule%w!",
      "Tu es un %bglaçon%w, Harry!" },

    { "%bFOOL%w!",
      "%bDUMMKOPF%w!",
      "%bSot%w que tu es." },

    { "You just got %bPUNKED%w!",
      "Du wurdest %beiskalt%w erwischt!",
      "Ça me %bglace%w le sang!" },

    { "Stay %bfrosty%w, @.",
      "Es läuft dir %beiskalt%w den Rücken&hinunter, @.",
      "%bReste au frais%w, @." },

    { "Take a %bchill pill%w, @.",
      "Bleib %bcool%w, @.",
      "Et c'est la douche %bfroide%w!" },

    { "%bWinter%w is coming.",
      "Der %bWinter%w naht.",
      "L'%bhiver%w vient." },
    
    { "%bICE%w to see you, @.",
      "Alles %bcool%w im Pool?",
      "%bGlacier%w!" },

    { "Feeling a little %rhot%w under the collar?&%bLet's fix that%w.",
      "%bAbkühlung gefällig%w?",
      "%Ça en jette un %bfroid%w." },

    { "It's a %bcold day%w in the Evil Realm.",
      "Es ist ein %kalter%w Tag im Herzen&von Hyrule.",
      "Est-ce que tu as déjà eu des sueurs&%bfroides%w?" },

    { "Getting %bcold feet%w?",
      "Bekommst du etwa %bkalte%w Füße?",
      "La vengeance est un plat qui se mange&%bfroid%w!" },

    { "Say hello to the %bZoras%w for me!",
      "Sag den %bZoras%w viele Grüße von mir!",
      "Dit bonjour aux %bZoras%w pour moi!" },

    { "Can you keep a %bcool head%w?",
      "Bewahre einen %bkühlen%w! Kopf.",
      "Il faut parfois savoir garder la tête&%bfroide%w!" },

    { "Ganondorf used %bIce Trap%w!&It's super effective!",
      "Ganondorf setzt %bEisstrahl%w ein.&Das ist sehr effektiv!",
      "Ganondorf utilise %bPiège de Glace%w!&C'est super efficace!" },

    { "Allow me to break the %bice%w!",
      "Ein Lächeln ist der beste Weg,&um das %bEis%w zu brechen!",
      "Laisse moi briser la %bglace%w!" },

    { "%bCold pun%w.",
      "%bEiskalt%w lässt du meine Seele&erfrier'n.",
      "Balance man...,&Cadence man...,&Trace la %bglace%w...,&c'est le Cooooolllll Rasta!" },

    { "The %bTitanic%w would be scared of you,&@.",
      "Die %bTitanic%w hätte Angst vor dir,&@.",
      "Le %bTitanic%w aurait peur de toi,&@." },

    { "Oh no!",
      "Oh nein!",
      "Oh non!" },
      
    { "What killed the dinosaurs?&The %bICE%w age!",
      "Was die Dinosaurier getötet hat?&Die %bEiszeit%w!",
      "Qu'est-ce qui a tué les dinosaures?&L'ère %bglacière%w!" },

    { "Knock knock. Who's there? Ice. Ice&who? Ice see that you're a %bFOOL%w.",
      "Nachts ist es %bkälter%w als draußen.",
      "L'imbécile réfléchit uniquement quand il&s'observe dans la %bglace%w." },

    { "Never gonna %bgive you up%w. Never&gonna %blet you down%w. Never gonna&run around and %bhurt you%w.",
      "Never gonna %bgive you up%w. Never&gonna %blet you down%w. Never gonna&run around and %bhurt you%w.",
      "Never gonna %bgive you up%w. Never&gonna %blet you down%w. Never gonna&run around and %bhurt you%w." },

    { "Thank you %b@%w!&But your item is in another castle!",
      "Danke %b@%w!&Aber der Gegenstand ist in&einem anderem Schloss!",
      "Merci %b@%w!&Mais ton objet est dans un autre&château!" },

    { "%bFREEZE%w! Don't move!",
      "	Kalt. Kalt. Kälter. %bEISKALT%w!",
      "J'espère que ça ne te fait ni chaud, ni&%bfroid%w." },
      
};

void CreateIceTrapRandoMessages() {
    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(Randomizer::IceTrapRandoMessageTableID);
    for (u8 i = 0; i <= (NUM_ICE_TRAP_MESSAGES - 1); i++) {
        customMessageManager->CreateMessage(Randomizer::IceTrapRandoMessageTableID, i,
                                            { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM, IceTrapMessages[i].english,
                                              IceTrapMessages[i].german, IceTrapMessages[i].french });
    }

    // We only use this ice trap message for christmas, so we don't want it in the normal ice trap messages rotation
    customMessageManager->CreateMessage(Randomizer::IceTrapRandoMessageTableID, NUM_ICE_TRAP_MESSAGES + 1,
                                            { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM,
                                              "This year for Christmas, all&you get is %BCOAL%w!",
                                              "This year for Christmas, all&you get is %BCOAL%w!",
                                              "Pour Noël, cette année, tu&n'auras que du %BCHARBON!&%rJoyeux Noël%w!" });
}

void Randomizer::CreateCustomMessages() {
    // RANDTODO: Translate into french and german and replace GIMESSAGE_UNTRANSLATED
    // with GIMESSAGE(getItemID, itemID, english, german, french).
    const std::vector<GetItemMessage> getItemMessages = {
        GIMESSAGE(RG_BOTTLE_WITH_BLUE_FIRE, ITEM_BLUE_FIRE, 
			"You got a %rBottle with Blue &Fire%w! Use it to melt Red Ice!",
			"Du erhältst eine %rFlasche mit&blauem Feuer%w! Nutze es um&%rRotes Eis%w zu schmelzen!",
            "Vous obtenez une %rBouteille avec&une Flamme Bleue%w! Utilisez-la&pour faire fondre la %rGlace&Rouge%w!"),
        GIMESSAGE(RG_BOTTLE_WITH_BIG_POE, ITEM_BIG_POE,
			"You got a %rBig Poe in a Bottle%w!&Sell it to the Ghost Shop!",
			"Du hast einen %rNachtschwärmer%w&in einer Flasche gefangen!&Gehe zum %rGespenstermarkt%w&und verkaufe ihn!",
			"Vous obtenez une %rBouteille avec&une Âme%w! Vendez-la au Marchand&d'Âme"),
        GIMESSAGE(RG_BOTTLE_WITH_BLUE_POTION, ITEM_POTION_BLUE,
			"You got a %rBottle of Blue Potion%w!&Drink it to replenish your&%ghealth%w and %bmagic%w!",
			"Du erhältst ein %rBlaues Elexier%w!&Nutze es, um deine %rMagie- und&Energieleiste%w komplett&aufzufüllen!",
			"Vous obtenez une %rBouteille avec&une Potion Bleue%w! Buvez-la pour&restaurer votre %rénergie vitale%w&ainsi que votre %gmagie%w!"),
        GIMESSAGE(RG_BOTTLE_WITH_FISH, ITEM_FISH,
            "You got a %rFish in a Bottle%w!&It looks fresh and delicious!&They say Jabu-Jabu loves them!",
			"Du hast jetzt einen %rFisch in&einer Flasche%w! Er sieht richtig&frisch aus! Man sagt,&Lord Jabu-Jabu liebt Fische!",
            "Vous obtenez une %rBouteille avec&un Poisson%w! Il a l'air délicieux!&Il paraîtrait que %bJabu-Jabu %wen&serait friand!"),
        GIMESSAGE(RG_BOTTLE_WITH_BUGS, ITEM_BUG,
			"You got a %rBug in a Bottle%w!&They love to burrow in&dirt holes!",
			"Du hast jetzt %rKäfer in einer&Flasche&%w!&Sie graben gerne&in Erdlöchern.",
            "Vous obtenez une %rBouteille avec&des Insectes%w! Ils adorent creuser&dans la terre meuble!"),
        GIMESSAGE(RG_BOTTLE_WITH_FAIRY, ITEM_FAIRY,
			"You got a %rFairy in a Bottle%w!&Use it wisely!",
			"Du hast jetzt eine %rFee in einer&Flasche%w! Nutze sie weise!",
			"Vous obtenez une %rBouteille avec&une Fée%w! Faites-en bon usage!"),
        GIMESSAGE(RG_BOTTLE_WITH_RED_POTION, ITEM_POTION_RED,
            "You got a %rBottle of Red Potion%w!&Drink it to replenish your&%ghealth%w!",
			"Du erhältst ein %rRotes Elexier%w!&Nutze es, um deine %rEnergieleiste&%weinmalig komplett aufzufüllen!",
            "Vous obtenez une %rBouteille avec&une Potion Rouge%w! Buvez-la pour&restaurer votre %rénergie vitale%w!"),
        GIMESSAGE(RG_BOTTLE_WITH_GREEN_POTION, ITEM_POTION_GREEN,
            "You got a %rBottle of Green Potion%w!&Drink it to replenish your&%bmagic%w!",
			"Du erhältst ein %rGrünes Elexier%w!&Nutze es, um deine %bMagieleiste&%weinmalig komplett aufzufüllen!",
            "Vous obtenez une %rBouteille avec&une Potion Verte%w! Buvez-la pour&restaurer votre %gmagie%w!"),
        GIMESSAGE(RG_BOTTLE_WITH_POE, ITEM_POE,
            "You got a %rPoe in a Bottle%w!&That creepy Ghost Shop might&be interested in this...",
			"Du hast jetzt ein %rIrrlicht in einer&Flasche%w! Der %rGespenstermarkt%w&interessiert sich für vielleicht&dafür...",
            "Vous obtenez une %rBouteille avec&un Esprit%w! Ça intéresserait&peut-être le vendeur d'Âme "),

        GIMESSAGE(RG_GERUDO_FORTRESS_SMALL_KEY, ITEM_KEY_SMALL,
			"You found a %yThieves Hideout &%wSmall Key!",
			"Du erhältst einen %rKleinen&Schlüssel%w für das %yDiebesversteck%w!",
			"Vous obtenez une %rPetite Clé %w&du %yRepaire des Voleurs%w!"),
        GIMESSAGE(RG_FOREST_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL,
			"You found a %gForest Temple &%wSmall Key!",
			"Du erhältst einen %rKleinen&Schlüssel%w für den %gWaldtempel%w!",
			"Vous obtenez une %rPetite Clé %w&du %gTemple de la Forêt%w!"),
        GIMESSAGE(RG_FIRE_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL,
			"You found a %rFire Temple &%wSmall Key!",
			"Du erhältst einen %rKleinen&Schlüssel%w für den %rFeuertempel%w!",
			"Vous obtenez une %rPetite Clé %w&du %rTemple du Feu%w!"),
        GIMESSAGE(RG_WATER_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL,
			"You found a %bWater Temple &%wSmall Key!",
			"Du erhältst einen %rKleinen&Schlüssel%w für den %bWassertempel%w!",
			"Vous obtenez une %rPetite Clé %w&du %bTemple de l'Eau%w!"),
        GIMESSAGE(RG_SPIRIT_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL,
			"You found a %ySpirit Temple &%wSmall Key!",
			"Du erhältst einen %rKleinen&Schlüssel%w für den %yGeistertempel%w!",
			"Vous obtenez une %rPetite Clé %w&du %yTemple de l'Esprit%w!"),
        GIMESSAGE(RG_SHADOW_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL,
			"You found a %pShadow Temple &%wSmall Key!",
			"Du erhältst einen %rKleinen&Schlüssel%w für den %pSchattentempel%w!",
			"Vous obtenez une %rPetite Clé %w&du %pTemple de l'Ombre%w!"),
        GIMESSAGE(RG_BOTTOM_OF_THE_WELL_SMALL_KEY, ITEM_KEY_SMALL,
			"You found a %pBottom of the &Well %wSmall Key!",
			"Du erhältst einen %rKleinen&Schlüssel%w für den %pGrund des Brunnens%w!",
			"Vous obtenez une %rPetite Clé %w&du %pPuits%w!"),
        GIMESSAGE(RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, ITEM_KEY_SMALL,
			"You found a %yGerudo Training &Grounds %wSmall Key!",
			"Du erhältst einen %rKleinen&Schlüssel%w für die %yGerudo&Trainingsarena%w!",
			"Vous obtenez une %rPetite Clé %w&du %yGymnase Gerudo%w!"),
        GIMESSAGE(RG_GANONS_CASTLE_SMALL_KEY, ITEM_KEY_SMALL,
			"You found a %rGanon's Castle &%wSmall Key!",
			"Du erhältst einen %rKleinen&Schlüssel%w für die %rGanons Schloß%w!",
			"Vous obtenez une %rPetite Clé %w&du %rChâteau de Ganon%w!"),

        GIMESSAGE(RG_GERUDO_FORTRESS_KEY_RING, ITEM_KEY_SMALL,
			"You found a %yThieves Hideout &%wKeyring!",
			"Du erhältst ein %rSchlüsselbund%w&für das %yDiebesversteck%w!",
			"Vous obtenez un trousseau de&clés du %yRepaire des Voleurs%w!"),
        GIMESSAGE(RG_FOREST_TEMPLE_KEY_RING, ITEM_KEY_SMALL,
			"You found a %gForest Temple &%wKeyring!",
			"Du erhältst ein %rSchlüsselbund%w&für den %gWaldtempel%w!",
			"Vous obtenez un trousseau de&clés du %gTemple de la Forêt%w!"),
        GIMESSAGE(RG_FIRE_TEMPLE_KEY_RING, ITEM_KEY_SMALL,
			"You found a %rFire Temple &%wKeyring!",
			"Du erhältst ein %rSchlüsselbund%w&für den %rFeuertempel%w!",
			"Vous obtenez un trousseau de&clés du %rTemple du Feu%w!"),
        GIMESSAGE(RG_WATER_TEMPLE_KEY_RING, ITEM_KEY_SMALL,
			"You found a %bWater Temple &%wKeyring!",
			"Du erhältst ein %rSchlüsselbund%w&für den %bWassertempel%w!",
			"Vous obtenez un trousseau de&clés du %bTemple de l'Eau%w!"),
        GIMESSAGE(RG_SPIRIT_TEMPLE_KEY_RING, ITEM_KEY_SMALL,
			"You found a %ySpirit Temple &%wKeyring!",
			"Du erhältst ein %rSchlüsselbund%w&für den %yGeistertempel%w!",
			"Vous obtenez un trousseau de&clés du %yTemple de l'Esprit%w!"),
        GIMESSAGE(RG_SHADOW_TEMPLE_KEY_RING, ITEM_KEY_SMALL,
			"You found a %pShadow Temple &%wKeyring!",
			"Du erhältst ein %rSchlüsselbund%w&für den %pSchattentempel%w!",
			"Vous obtenez un trousseau de&clés du %pTemple de l'Ombre%w!"),
        GIMESSAGE(RG_BOTTOM_OF_THE_WELL_KEY_RING, ITEM_KEY_SMALL,
			"You found a %pBottom of the &Well %wKeyring!",
			"Du erhältst ein %rSchlüsselbund%w&für den %pGrund des Brunnens%w!",
			"Vous obtenez un trousseau de&clés du %pPuits%w!"),
        GIMESSAGE(RG_GERUDO_TRAINING_GROUNDS_KEY_RING, ITEM_KEY_SMALL,
			"You found a %yGerudo Training &Grounds %wKeyring!",
			"Du erhältst ein %rSchlüsselbund%w&für die %yGerudo Trainingsarena%w!",
			"Vous obtenez un trousseau de&clés du %yGymnase Gerudo%w!"),
        GIMESSAGE(RG_GANONS_CASTLE_KEY_RING, ITEM_KEY_SMALL, 
			"You found a %rGanon's Castle &%wKeyring!",
			"Du erhältst ein %rSchlüsselbund%w&für %rGanons Schloß%w!",
			"Vous obtenez un trousseau de&clés du %rChâteau de Ganon%w!"),

        GIMESSAGE(RG_FOREST_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS,
			"You found the %gForest Temple &%wBoss Key!",
			"Du erhältst den %rMaster-Schlüssel%w&für den %gWaldtempel%w!",
			"Vous obtenez la %rClé d'or %wdu&%gTemple de la Forêt%w!"),
        GIMESSAGE(RG_FIRE_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS,
			"You found the %rFire Temple &%wBoss Key!",
			"Du erhältst den %rMaster-Schlüssel%w&für den %rFeuertempel%w!",
			"Vous obtenez la %rClé d'or %wdu&%rTemple du Feu%w!"),
        GIMESSAGE(RG_WATER_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS,
			"You found the %bWater Temple &%wBoss Key!",
			"Du erhältst den %rMaster-Schlüssel%w&für den %bWassertempel%w!",
			"Vous obtenez la %rClé d'or %wdu&%bTemple de l'Eau%w!"),
        GIMESSAGE(RG_SPIRIT_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS,
			"You found the %ySpirit Temple &%wBoss Key!",
			"Du erhältst den %rMaster-Schlüssel%w&für den %yGeistertempel%w!",
			"Vous obtenez la %rClé d'or %wdu&%yTemple de l'Esprit%w!"),
        GIMESSAGE(RG_SHADOW_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS,
			"You found the %pShadow Temple &%wBoss Key!",
			"Du erhältst den %rMaster-Schlüssel%w&für den %pSchattentempel%w!",
			"Vous obtenez la %rClé d'or %wdu&%pTemple de l'Ombre%w!"),
        GIMESSAGE(RG_GANONS_CASTLE_BOSS_KEY, ITEM_KEY_BOSS,
			"You found the %rGanon's Castle &%wBoss Key!",
			"Du erhältst den %rMaster-Schlüssel%w&für %rGanons Schloß%w!",
			"Vous obtenez la %rClé d'or %wdu&%rChâteau de Ganon%w!"),

        GIMESSAGE(RG_DEKU_TREE_MAP, ITEM_DUNGEON_MAP,
			"You found the %gDeku Tree &%wMap!{{typeHint}}",
			"Du erhältst die %rKarte%w für den&%gDeku-Baum%w!{{typeHint}}",
			"Vous obtenez la %rCarte %wde&l'%gArbre Mojo%w!{{typeHint}}"),
        GIMESSAGE(RG_DODONGOS_CAVERN_MAP, ITEM_DUNGEON_MAP,
			"You found the %rDodongo's Cavern &%wMap!{{typeHint}}",
			"Du erhältst die %rKarte%w für&%rDodongos Höhle%w!{{typeHint}}",
			"Vous obtenez la %rCarte %wde la&%rCaverne Dodongo%w!{{typeHint}}"),
        GIMESSAGE(RG_JABU_JABUS_BELLY_MAP, ITEM_DUNGEON_MAP,
			"You found the %bJabu Jabu's Belly &%wMap!{{typeHint}}",
			"Du erhältst die %rKarte%w für&%bJabu-Jabus Bauch%w!{{typeHint}}",
			"Vous obtenez la %rCarte %wdu &%bVentre de Jabu-Jabu%w!{{typeHint}}"),
        GIMESSAGE(RG_FOREST_TEMPLE_MAP, ITEM_DUNGEON_MAP,
			"You found the %gForest Temple &%wMap!{{typeHint}}",
			"Du erhältst die %rKarte%w für den&%gWaldtempel%w!{{typeHint}}",
			"Vous obtenez la %rCarte %wdu &%gTemple de la Forêt%w!{{typeHint}}"),
        GIMESSAGE(RG_FIRE_TEMPLE_MAP, ITEM_DUNGEON_MAP,
			"You found the %rFire Temple &%wMap!{{typeHint}}",
			"Du erhältst die %rKarte%w für den&%rFeuertempel%w!{{typeHint}}",
			"Vous obtenez la %rCarte %wdu &%rTemple du Feu%w!{{typeHint}}"),
        GIMESSAGE(RG_WATER_TEMPLE_MAP, ITEM_DUNGEON_MAP,
			"You found the %bWater Temple &%wMap!{{typeHint}}",
			"Du erhältst die %rKarte%w für den&%bWassertempel%w!{{typeHint}}",
			"Vous obtenez la %rCarte %wdu &%bTemple de l'Eau%w!{{typeHint}}"),
        GIMESSAGE(RG_SPIRIT_TEMPLE_MAP, ITEM_DUNGEON_MAP,
			"You found the %ySpirit Temple &%wMap!{{typeHint}}",
			"Du erhältst die %rKarte%w für den&%yGeistertempel%w!{{typeHint}}",
			"Vous obtenez la %rCarte %wdu &%yTemple de l'Esprit%w!{{typeHint}}"),
        GIMESSAGE(RG_SHADOW_TEMPLE_MAP, ITEM_DUNGEON_MAP,
			"You found the %pShadow Temple &%wMap!{{typeHint}}",
			"Du erhältst die %rKarte%w für den&%pSchattentempel%w!{{typeHint}}",
			"Vous obtenez la %rCarte %wdu &%pTemple de l'Ombre%w!{{typeHint}}"),
        GIMESSAGE(RG_BOTTOM_OF_THE_WELL_MAP, ITEM_DUNGEON_MAP,
			"You found the %pBottom of the &Well %wMap!{{typeHint}}",
			"Du erhältst die %rKarte%w für den&%pGrund des Brunnens%w!{{typeHint}}",
			"Vous obtenez la %rCarte %wdu &%pPuits%w!{{typeHint}}"),
        GIMESSAGE(RG_ICE_CAVERN_MAP, ITEM_DUNGEON_MAP,
			"You found the %cIce Cavern &%wMap!{{typeHint}}",
			"Du erhältst die %rKarte%w für die&%cEishöhle%w!{{typeHint}}",
			"Vous obtenez la %rCarte %wde &la %cCaverne Polaire%w!{{typeHint}}"),

        GIMESSAGE(RG_DEKU_TREE_COMPASS, ITEM_COMPASS,
			"You found the %gDeku Tree &%wCompass!",
			"Du erhältst den %rKompaß%w für den&%gDeku-Baum%w!",
			"Vous obtenez la %rBoussole %wde&l'%gArbre Mojo%w!"),
        GIMESSAGE(RG_DODONGOS_CAVERN_COMPASS, ITEM_COMPASS,
			"You found the %rDodongo's Cavern &%wCompass!",
			"Du erhältst den %rKompaß%w für&%rDodongos Höhle%w!",
			"Vous obtenez la %rBoussole %wde la&%rCaverne Dodongo%w!"),
        GIMESSAGE(RG_JABU_JABUS_BELLY_COMPASS, ITEM_COMPASS,
			"You found the %bJabu Jabu's Belly &%wCompass!",
			"Du erhältst den %rKompaß%w für den&%bJabu-Jabus Bauch%w!",
			"Vous obtenez la %rBoussole %wdu &%bVentre de Jabu-Jabu%w!"),
        GIMESSAGE(RG_FOREST_TEMPLE_COMPASS, ITEM_COMPASS,
			"You found the %gForest Temple &%wCompass!",
			"Du erhältst den %rKompaß%w für den&%gWaldtempel%w!",
			"Vous obtenez la %rBoussole %wdu &%gTemple de la Forêt%w!"),
        GIMESSAGE(RG_FIRE_TEMPLE_COMPASS, ITEM_COMPASS,
			"You found the %rFire Temple &%wCompass!",
			"Du erhältst den %rKompaß%w für den&%rFeuertempel%w!",
			"Vous obtenez la %rBoussole %wdu &%rTemple du Feu%w!"),
        GIMESSAGE(RG_WATER_TEMPLE_COMPASS, ITEM_COMPASS,
			"You found the %bWater Temple &%wCompass!",
			"Du erhältst den %rKompaß%w für den&%bWassertempel%w!",
			"Vous obtenez la %rBoussole %wdu &%bTemple de l'Eau%w!"),
        GIMESSAGE(RG_SPIRIT_TEMPLE_COMPASS, ITEM_COMPASS,
			"You found the %ySpirit Temple &%wCompass!",
			"Du erhältst den %rKompaß%w für den&%yGeistertempel%w!",
			"Vous obtenez la %rBoussole %wdu &%yTemple de l'Esprit%w!"),
        GIMESSAGE(RG_SHADOW_TEMPLE_COMPASS, ITEM_COMPASS,
			"You found the %pShadow Temple &%wCompass!",
			"Du erhältst den %rKompaß%w für den&%pSchattentempel%w!",
			"Vous obtenez la %rBoussole %wdu &%pTemple de l'Ombre%w!"),
        GIMESSAGE(RG_BOTTOM_OF_THE_WELL_COMPASS, ITEM_COMPASS,
			"You found the %pBottom of the &Well %wCompass!",
			"Du erhältst den %rKompaß%w für den&%pGrund des Brunnens%w!",
			"Vous obtenez la %rBoussole %wdu &%pPuits%w!"),
        GIMESSAGE(RG_ICE_CAVERN_COMPASS, ITEM_COMPASS,
			"You found the %cIce Cavern &%wCompass!",
			"Du erhältst den %rKompaß%w für die&%cEishöhle%w!",
			"Vous obtenez la %rBoussole %wde &la %cCaverne Polaire%w!"),

        GIMESSAGE(RG_MAGIC_BEAN_PACK, ITEM_BEAN,
			"You got a %rPack of Magic Beans%w!&Find a suitable spot for a garden&and plant them. Then, wait for&something fun to happen!",
			"Du erhältst eine %rPackung&Wundererbsen%w! Suche nach einer&Stelle um sie einzupflanzen.&Warte ab, was passiert!",
			"Vous obtenez un %rPaquet de&Haricots Magiques%w! Trouvez&un endroit approprié pour un&jardin et plantez-les.^Attendez ensuite que quelque&chose d'amusant se produise!"),
        GIMESSAGE(RG_TYCOON_WALLET, ITEM_WALLET_GIANT,
			"You got a %rTycoon's Wallet%w!&It's gigantic! Now you can carry&up to %y999 rupees%w!",
			"Du erhältst die %rGoldene&Geldbörse%w! Die größte aller&Geldbörsen! Jetzt kannst Du bis&zu %y999 Rubine%w mit dir führen!",
			"Vous obtenez la %rBourse de Magnat%w!&Elle peut contenir jusqu'à %y999 rubis%w!&C'est gigantesque!")
    };
    CreateGetItemMessages(getItemMessages);
    CreateRupeeMessages();
    CreateNaviRandoMessages();
    CreateIceTrapRandoMessages();
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
    static GetItemEntry extendedVanillaGetItemTable[] = {
        GET_ITEM(ITEM_MEDALLION_LIGHT,                 OBJECT_GI_MEDAL,    GID_MEDALLION_LIGHT,  0x40,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_LIGHT_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_FOREST,                OBJECT_GI_MEDAL,    GID_MEDALLION_FOREST, 0x3E,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_FOREST_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_FIRE,                  OBJECT_GI_MEDAL,    GID_MEDALLION_FIRE,   0x3C,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_FIRE_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_WATER,                 OBJECT_GI_MEDAL,    GID_MEDALLION_WATER,  0x3D,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_WATER_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_SHADOW,                OBJECT_GI_MEDAL,    GID_MEDALLION_SHADOW, 0x41,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_SHADOW_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_SPIRIT,                OBJECT_GI_MEDAL,    GID_MEDALLION_SPIRIT, 0x3F,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_SPIRIT_MEDALLION),

        GET_ITEM(ITEM_KOKIRI_EMERALD,                  OBJECT_GI_JEWEL,    GID_KOKIRI_EMERALD,   0x80,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_KOKIRI_EMERALD),
        GET_ITEM(ITEM_GORON_RUBY,                      OBJECT_GI_JEWEL,    GID_GORON_RUBY,       0x81,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_GORON_RUBY),
        GET_ITEM(ITEM_ZORA_SAPPHIRE,                   OBJECT_GI_JEWEL,    GID_ZORA_SAPPHIRE,    0x82,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_ZORA_SAPPHIRE),

        GET_ITEM(ITEM_SONG_LULLABY,                    OBJECT_GI_MELODY,   GID_SONG_ZELDA,       0xD4,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_ZELDAS_LULLABY),
        GET_ITEM(ITEM_SONG_SUN,                        OBJECT_GI_MELODY,   GID_SONG_SUN,         0xD3,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_SUNS_SONG),
        GET_ITEM(ITEM_SONG_EPONA,                      OBJECT_GI_MELODY,   GID_SONG_EPONA,       0xD2,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_EPONAS_SONG),
        GET_ITEM(ITEM_SONG_STORMS,                     OBJECT_GI_MELODY,   GID_SONG_STORM,       0xD6,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_SONG_OF_STORMS),
        GET_ITEM(ITEM_SONG_TIME,                       OBJECT_GI_MELODY,   GID_SONG_TIME,        0xD5,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_SONG_OF_TIME),
        GET_ITEM(ITEM_SONG_SARIA,                      OBJECT_GI_MELODY,   GID_SONG_SARIA,       0xD1,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_SARIAS_SONG),

        GET_ITEM(ITEM_SONG_MINUET,                     OBJECT_GI_MELODY,   GID_SONG_MINUET,      0x73,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_MINUET_OF_FOREST),
        GET_ITEM(ITEM_SONG_BOLERO,                     OBJECT_GI_MELODY,   GID_SONG_BOLERO,      0x74,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_BOLERO_OF_FIRE),
        GET_ITEM(ITEM_SONG_SERENADE,                   OBJECT_GI_MELODY,   GID_SONG_SERENADE,    0x75,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_SERENADE_OF_WATER),
        GET_ITEM(ITEM_SONG_NOCTURNE,                   OBJECT_GI_MELODY,   GID_SONG_NOCTURNE,    0x77,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_NOCTURNE_OF_SHADOW),
        GET_ITEM(ITEM_SONG_REQUIEM,                    OBJECT_GI_MELODY,   GID_SONG_REQUIEM,     0x76,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_REQUIEM_OF_SPIRIT),
        GET_ITEM(ITEM_SONG_PRELUDE,                    OBJECT_GI_MELODY,   GID_SONG_PRELUDE,     0x78,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR, MOD_NONE,       RG_PRELUDE_OF_LIGHT),
    };

    // These do not have ItemIDs or GetItemIDs from vanilla, so I'm using their
    // RandomizerGet enum values for both.
    static GetItemEntry randoGetItemTable[] = {
        GET_ITEM(RG_ICE_TRAP,                          OBJECT_GI_RUPY,     GID_RUPEE_GOLD,       TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,     MOD_RANDOMIZER, RG_ICE_TRAP),
        GET_ITEM(RG_MAGIC_SINGLE,                      OBJECT_GI_MAGICPOT, GID_MAGIC_SMALL,      0xE4,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,     MOD_RANDOMIZER, RG_MAGIC_SINGLE),
        GET_ITEM(RG_MAGIC_DOUBLE,                      OBJECT_GI_MAGICPOT, GID_MAGIC_LARGE,      0xE8,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_MAGIC_DOUBLE),
        GET_ITEM(RG_DOUBLE_DEFENSE,                    OBJECT_GI_HEARTS,   GID_HEART_CONTAINER,  0xE9,                        0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_DOUBLE_DEFENSE),
        GET_ITEM(RG_BOTTLE_WITH_RED_POTION,            OBJECT_GI_LIQUID,   GID_POTION_RED,       TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,     MOD_RANDOMIZER, RG_BOTTLE_WITH_RED_POTION),
        GET_ITEM(RG_BOTTLE_WITH_GREEN_POTION,          OBJECT_GI_LIQUID,   GID_POTION_GREEN,     TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,     MOD_RANDOMIZER, RG_BOTTLE_WITH_GREEN_POTION),
        GET_ITEM(RG_BOTTLE_WITH_BLUE_POTION,           OBJECT_GI_LIQUID,   GID_POTION_BLUE,      TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,     MOD_RANDOMIZER, RG_BOTTLE_WITH_BLUE_POTION),
        GET_ITEM(RG_BOTTLE_WITH_FAIRY,                 OBJECT_GI_BOTTLE,   GID_BOTTLE,           TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,     MOD_RANDOMIZER, RG_BOTTLE_WITH_FAIRY),
        GET_ITEM(RG_BOTTLE_WITH_FISH,                  OBJECT_GI_FISH,     GID_FISH,             TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,     MOD_RANDOMIZER, RG_BOTTLE_WITH_FISH),
        GET_ITEM(RG_BOTTLE_WITH_BLUE_FIRE,             OBJECT_GI_FIRE,     GID_BLUE_FIRE,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,     MOD_RANDOMIZER, RG_BOTTLE_WITH_BLUE_FIRE),
        GET_ITEM(RG_BOTTLE_WITH_BUGS,                  OBJECT_GI_INSECT,   GID_BUG,              TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,     MOD_RANDOMIZER, RG_BOTTLE_WITH_BUGS),
        GET_ITEM(RG_BOTTLE_WITH_POE,                   OBJECT_GI_GHOST,    GID_POE,              TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,     MOD_RANDOMIZER, RG_BOTTLE_WITH_POE),
        GET_ITEM(RG_BOTTLE_WITH_BIG_POE,               OBJECT_GI_GHOST,    GID_BIG_POE,          TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,     MOD_RANDOMIZER, RG_BOTTLE_WITH_BIG_POE),
        GET_ITEM(RG_GERUDO_FORTRESS_SMALL_KEY,         OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_GERUDO_FORTRESS_SMALL_KEY),
        GET_ITEM(RG_FOREST_TEMPLE_SMALL_KEY,           OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_FOREST_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_FIRE_TEMPLE_SMALL_KEY,             OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_FIRE_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_WATER_TEMPLE_SMALL_KEY,            OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_WATER_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_SPIRIT_TEMPLE_SMALL_KEY,           OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_SHADOW_TEMPLE_SMALL_KEY,           OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_SHADOW_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_BOTTOM_OF_THE_WELL_SMALL_KEY,      OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_BOTTOM_OF_THE_WELL_SMALL_KEY),
        GET_ITEM(RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY),
        GET_ITEM(RG_GANONS_CASTLE_SMALL_KEY,           OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_GANONS_CASTLE_SMALL_KEY),
        GET_ITEM(RG_GERUDO_FORTRESS_KEY_RING,          OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_GERUDO_FORTRESS_KEY_RING),
        GET_ITEM(RG_FOREST_TEMPLE_KEY_RING,            OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_FOREST_TEMPLE_KEY_RING),
        GET_ITEM(RG_FIRE_TEMPLE_KEY_RING,              OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_FIRE_TEMPLE_KEY_RING),
        GET_ITEM(RG_WATER_TEMPLE_KEY_RING,             OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_WATER_TEMPLE_KEY_RING),
        GET_ITEM(RG_SPIRIT_TEMPLE_KEY_RING,            OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_KEY_RING),
        GET_ITEM(RG_SHADOW_TEMPLE_KEY_RING,            OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_SHADOW_TEMPLE_KEY_RING),
        GET_ITEM(RG_BOTTOM_OF_THE_WELL_KEY_RING,       OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_BOTTOM_OF_THE_WELL_KEY_RING),
        GET_ITEM(RG_GERUDO_TRAINING_GROUNDS_KEY_RING,  OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_GERUDO_TRAINING_GROUNDS_KEY_RING),
        GET_ITEM(RG_GANONS_CASTLE_KEY_RING,            OBJECT_GI_KEY,      GID_KEY_SMALL,        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER, RG_GANONS_CASTLE_KEY_RING),
        GET_ITEM(RG_FOREST_TEMPLE_BOSS_KEY,            OBJECT_GI_BOSSKEY,  GID_KEY_BOSS,         TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_BOSS_KEY,  MOD_RANDOMIZER, RG_FOREST_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_FIRE_TEMPLE_BOSS_KEY,              OBJECT_GI_BOSSKEY,  GID_KEY_BOSS,         TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_BOSS_KEY,  MOD_RANDOMIZER, RG_FIRE_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_WATER_TEMPLE_BOSS_KEY,             OBJECT_GI_BOSSKEY,  GID_KEY_BOSS,         TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_BOSS_KEY,  MOD_RANDOMIZER, RG_WATER_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_SPIRIT_TEMPLE_BOSS_KEY,            OBJECT_GI_BOSSKEY,  GID_KEY_BOSS,         TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_BOSS_KEY,  MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_SHADOW_TEMPLE_BOSS_KEY,            OBJECT_GI_BOSSKEY,  GID_KEY_BOSS,         TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_BOSS_KEY,  MOD_RANDOMIZER, RG_SHADOW_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_GANONS_CASTLE_BOSS_KEY,            OBJECT_GI_BOSSKEY,  GID_KEY_BOSS,         TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_BOSS_KEY,  MOD_RANDOMIZER, RG_GANONS_CASTLE_BOSS_KEY),
        GET_ITEM(RG_DEKU_TREE_MAP,                     OBJECT_GI_MAP,      GID_DUNGEON_MAP,      TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_DEKU_TREE_MAP),
        GET_ITEM(RG_DODONGOS_CAVERN_MAP,               OBJECT_GI_MAP,      GID_DUNGEON_MAP,      TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_DODONGOS_CAVERN_MAP),
        GET_ITEM(RG_JABU_JABUS_BELLY_MAP,              OBJECT_GI_MAP,      GID_DUNGEON_MAP,      TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_JABU_JABUS_BELLY_MAP),
        GET_ITEM(RG_FOREST_TEMPLE_MAP,                 OBJECT_GI_MAP,      GID_DUNGEON_MAP,      TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_FOREST_TEMPLE_MAP),
        GET_ITEM(RG_FIRE_TEMPLE_MAP,                   OBJECT_GI_MAP,      GID_DUNGEON_MAP,      TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_FIRE_TEMPLE_MAP),
        GET_ITEM(RG_WATER_TEMPLE_MAP,                  OBJECT_GI_MAP,      GID_DUNGEON_MAP,      TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_WATER_TEMPLE_MAP),
        GET_ITEM(RG_SPIRIT_TEMPLE_MAP,                 OBJECT_GI_MAP,      GID_DUNGEON_MAP,      TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_MAP),
        GET_ITEM(RG_SHADOW_TEMPLE_MAP,                 OBJECT_GI_MAP,      GID_DUNGEON_MAP,      TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_SHADOW_TEMPLE_MAP),
        GET_ITEM(RG_BOTTOM_OF_THE_WELL_MAP,            OBJECT_GI_MAP,      GID_DUNGEON_MAP,      TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_BOTTOM_OF_THE_WELL_MAP),
        GET_ITEM(RG_ICE_CAVERN_MAP,                    OBJECT_GI_MAP,      GID_DUNGEON_MAP,      TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_ICE_CAVERN_MAP),
        GET_ITEM(RG_DEKU_TREE_COMPASS,                 OBJECT_GI_COMPASS,  GID_COMPASS,          TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_DEKU_TREE_COMPASS),
        GET_ITEM(RG_DODONGOS_CAVERN_COMPASS,           OBJECT_GI_COMPASS,  GID_COMPASS,          TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_DODONGOS_CAVERN_COMPASS),
        GET_ITEM(RG_JABU_JABUS_BELLY_COMPASS,          OBJECT_GI_COMPASS,  GID_COMPASS,          TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_JABU_JABUS_BELLY_COMPASS),
        GET_ITEM(RG_FOREST_TEMPLE_COMPASS,             OBJECT_GI_COMPASS,  GID_COMPASS,          TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_FOREST_TEMPLE_COMPASS),
        GET_ITEM(RG_FIRE_TEMPLE_COMPASS,               OBJECT_GI_COMPASS,  GID_COMPASS,          TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_FIRE_TEMPLE_COMPASS),
        GET_ITEM(RG_WATER_TEMPLE_COMPASS,              OBJECT_GI_COMPASS,  GID_COMPASS,          TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_WATER_TEMPLE_COMPASS),
        GET_ITEM(RG_SPIRIT_TEMPLE_COMPASS,             OBJECT_GI_COMPASS,  GID_COMPASS,          TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_COMPASS),
        GET_ITEM(RG_SHADOW_TEMPLE_COMPASS,             OBJECT_GI_COMPASS,  GID_COMPASS,          TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_SHADOW_TEMPLE_COMPASS),
        GET_ITEM(RG_BOTTOM_OF_THE_WELL_COMPASS,        OBJECT_GI_COMPASS,  GID_COMPASS,          TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_BOTTOM_OF_THE_WELL_COMPASS),
        GET_ITEM(RG_ICE_CAVERN_COMPASS,                OBJECT_GI_COMPASS,  GID_COMPASS,          TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_ICE_CAVERN_COMPASS),
        GET_ITEM(RG_MAGIC_BEAN_PACK,                   OBJECT_GI_BEAN,     GID_BEAN,             TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,     MOD_RANDOMIZER, RG_MAGIC_BEAN_PACK),
        GET_ITEM(RG_TYCOON_WALLET,                     OBJECT_GI_PURSE,    GID_WALLET_GIANT,     TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,    MOD_RANDOMIZER, RG_TYCOON_WALLET),
    };
    ItemTableManager::Instance->AddItemTable(MOD_RANDOMIZER);
    for (int i = 0; i < ARRAY_COUNT(extendedVanillaGetItemTable); i++) {
        ItemTableManager::Instance->AddItemEntry(MOD_RANDOMIZER, extendedVanillaGetItemTable[i].getItemId, extendedVanillaGetItemTable[i]);
    }
    for (int i = 0; i < ARRAY_COUNT(randoGetItemTable); i++) {
        if (randoGetItemTable[i].itemId >= RG_FOREST_TEMPLE_SMALL_KEY && randoGetItemTable[i].itemId <= RG_GANONS_CASTLE_SMALL_KEY
            && randoGetItemTable[i].itemId != RG_GERUDO_FORTRESS_SMALL_KEY) {
            randoGetItemTable[i].drawFunc = (CustomDrawFunc)Randomizer_DrawSmallKey;
        } else if (randoGetItemTable[i].itemId >= RG_FOREST_TEMPLE_KEY_RING && randoGetItemTable[i].itemId <= RG_GANONS_CASTLE_KEY_RING) {
            randoGetItemTable[i].drawFunc = (CustomDrawFunc)Randomizer_DrawKeyRing;
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

