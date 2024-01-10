#include "randomizer.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <variables.h>
#include <macros.h>
#include <objects/gameplay_keep/gameplay_keep.h>
#include <functions.h>
#include <libultraship/libultraship.h>
#include <textures/icon_item_static/icon_item_static.h>
#include <textures/icon_item_24_static/icon_item_24_static.h>
#include "3drando/rando_main.hpp"
#include "3drando/random.hpp"
#include "../../UIWidgets.hpp"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include "../item-tables/ItemTableManager.h"
#include "../presets.h"
#include "../../../src/overlays/actors/ovl_En_GirlA/z_en_girla.h"
#include <stdexcept>
#include "randomizer_check_objects.h"
#include "randomizer_check_tracker.h"
#include <sstream>
#include <tuple>
#include <functional>
#include "draw.h"
#include "static_data.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include <boost_custom/container_hash/hash_32.hpp>
#include "randomizer_settings_window.h"
#include "savefile.h"
#include "entrance.h"
#include "dungeon.h"
#include "trial.h"
#include "settings.h"
#include "soh/util.h"
#include "fishsanity.h"

extern "C" uint32_t ResourceMgr_IsGameMasterQuest();
extern "C" uint32_t ResourceMgr_IsSceneMasterQuest(s16 sceneNum);

extern std::map<RandomizerCheckArea, std::string> rcAreaNames;
extern std::array<std::string, HINT_TYPE_MAX> hintTypeNames;

using json = nlohmann::json;
using namespace std::literals::string_literals;

std::unordered_map<std::string, RandomizerCheck> SpoilerfileCheckNameToEnum;
std::unordered_map<std::string, RandomizerGet> SpoilerfileGetNameToEnum;
std::unordered_map<std::string, RandomizerCheckArea> SpoilerfileAreaNameToEnum;
std::unordered_map<std::string, HintType> SpoilerfileHintTypeNameToEnum;
std::multimap<std::tuple<s16, s16, s32>, RandomizerCheck> checkFromActorMultimap;
std::set<RandomizerCheck> excludedLocations;
std::set<RandomizerCheck> spoilerExcludedLocations;
std::set<RandomizerTrick> enabledTricks;
std::set<RandomizerTrick> enabledGlitches;

std::set<std::map<RandomizerCheck, RandomizerCheckTrackerData>> checkTrackerStates;

u8 generated;
char* seedString;

const std::string Randomizer::getItemMessageTableID = "Randomizer";
const std::string Randomizer::hintMessageTableID = "RandomizerHints";
const std::string Randomizer::merchantMessageTableID = "RandomizerMerchants";
const std::string Randomizer::rupeeMessageTableID = "RandomizerRupees";
const std::string Randomizer::triforcePieceMessageTableID = "RandomizerTriforcePiece";
const std::string Randomizer::NaviRandoMessageTableID = "RandomizerNavi";
const std::string Randomizer::IceTrapRandoMessageTableID = "RandomizerIceTrap";
const std::string Randomizer::randoMiscHintsTableID = "RandomizerMiscHints";

static const char* englishRupeeNames[171] = {
    "[P]",               "Bad RNG Rolls",    "Bananas",          "Beanbean Coins",   "Beans",
    "Beli",              "Bells",            "Berries",          "Bison Dollars",    "Bitcoin",
    "Blue Essence",      "Bolts",            "Bones",            "Boondollars",      "Bottle Caps",
    "Bratwürste",        "Bucks",            "BugFrags",         "Canadian Dollars", "Cards",
    "Chaos Orbs",        "Clams",            "Coal",             "Cocoa Beans",      "Coins",
    "Cookies",           "Copper",           "Cor",              "Cornflakes",       "Credits",
    "Crimebucks",        "Crystal Shards",   "Cubits",           "Cucumbers",        "Dalmations",
    "Dampécoin",         "Dark Elixir",      "Darseks",          "Dead Memes",       "Diamonds",
    "DNA",               "Doge",             "Dogecoin",         "Doll Hairs",       "Dollars",
    "Dollarydoos",       "Dosh",             "Doubloons",        "Dwarfbucks",       "Emeralds",
    "Energon",           "Eris",             "Ether",            "Euro",             "Experience",
    "Extinction Points", "Floopies",         "Flurbos",          "FPS",              "Friends",
    "Frog Coins",        "Gald",             "Gekz",             "Gems",             "Geo",
    "Gil",               "Glimmer",          "Glitches",         "Gold",             "Gold Dragons",
    "Goober Dollars",    "Green Herbs",      "Greg Siblings",    "Gummybears",       "Hell",
    "Hylian Loaches",    "Ice Traps",        "ISK",              "Jiggies",          "KF7 Ammo",
    "Kinstones",         "Kremcoins",        "Kroner",           "Leaves ",          "Lemmings",
    "Lien",              "Lira",             "Lumber",           "Lungmen Dollars",  "Macca",
    "Mana",              "Mann Co. Keys",    "Meat",             "Meat Stacks",      "Medaparts",
    "Meseta",            "Mesetas",          "Minerals",         "Monopoly Money",   "Moons",
    "Mora",              "Mumbo Tokens",     "Munny",            "Mushrooms",        "Mysteries",
    "Neopoints",         "Notes",            "Nuyen",            "Orbs",             "Pix",
    "Pixels",            "Platinum",         "Pokédollars",      "Pokémon",          "Poko",
    "Pokos",             "Potch",            "Pounds",           "Power Pellets",    "Primogems",
    "Réals",             "Refined Metal",    "Remote Mines",     "Retweets",         "Rhinu",
    "Rings",             "Riot Points",      "Robux",            "Rubies",           "Rubles",
    "Runite Ore",        "Rupees",           "Saint Quartz",     "Septims",          "Shekels",
    "Shillings",         "Silver",           "Simoleons",        "Smackaroos",       "Social Credit",
    "Souls",             "Spent Casings",    "Spice",            "Spondulicks",      "Spoons",
    "Star Bits",         "Star Chips",       "Stars",            "Stones of Jordan", "Store Credit",
    "Strawbs",           "Studs",            "Super Sea Snails", "Talent",           "Teef",
    "Telecrystals",      "Tiberium",         "TokKul",           "Toys",             "Turnips",
    "Upvotes",           "V-Bucks",          "Vespene Gas",      "Watts",            "Widgets",
    "Woolongs",          "World Dollars",    "Wumpa Fruit",      "Yen",              "Zenny",
    "Zorkmids"
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
    Rando::StaticData::InitItemTable();
    Rando::StaticData::InitLocationTable();
    for (auto& location : Rando::StaticData::GetLocationTable()) {
        SpoilerfileCheckNameToEnum[location.GetName()] = location.GetRandomizerCheck();
        checkFromActorMultimap.emplace(std::make_tuple((s16)location.GetActorID(), (s16)location.GetScene(), location.GetActorParams()), location.GetRandomizerCheck());
    }
    SpoilerfileCheckNameToEnum["Invalid Location"] = RC_UNKNOWN_CHECK;
    SpoilerfileCheckNameToEnum["Link's Pocket"] = RC_LINKS_POCKET;

    for (auto& item: Rando::StaticData::GetItemTable()) {
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
    for (auto area : rcAreaNames) {
        SpoilerfileAreaNameToEnum[area.second] = area.first;
    }
    SpoilerfileAreaNameToEnum["Inside Ganon's Castle"] = RCAREA_GANONS_CASTLE;
    SpoilerfileAreaNameToEnum["the Lost Woods"] = RCAREA_LOST_WOODS;
    SpoilerfileAreaNameToEnum["the Market"] = RCAREA_MARKET;
    SpoilerfileAreaNameToEnum["the Graveyard"] = RCAREA_GRAVEYARD;
    SpoilerfileAreaNameToEnum["Haunted Wasteland"] = RCAREA_WASTELAND;
    SpoilerfileAreaNameToEnum["outside Ganon's Castle"] = RCAREA_HYRULE_CASTLE;
    for (int c = 0; c < hintTypeNames.size(); c++) {
        SpoilerfileHintTypeNameToEnum[hintTypeNames[c]] = (HintType)c;
    }
}

Randomizer::~Randomizer() {
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
    { "Deku Tree", SCENE_DEKU_TREE },
    { "Dodongo's Cavern", SCENE_DODONGOS_CAVERN },
    { "Jabu Jabu's Belly", SCENE_JABU_JABU },
    { "Forest Temple", SCENE_FOREST_TEMPLE },
    { "Fire Temple", SCENE_FIRE_TEMPLE },
    { "Water Temple", SCENE_WATER_TEMPLE },
    { "Spirit Temple", SCENE_SPIRIT_TEMPLE },
    { "Shadow Temple", SCENE_SHADOW_TEMPLE },
    { "Bottom of the Well", SCENE_BOTTOM_OF_THE_WELL },
    { "Ice Cavern", SCENE_ICE_CAVERN },
    { "Gerudo Training Grounds", SCENE_GERUDO_TRAINING_GROUND },
    { "Ganon's Castle", SCENE_INSIDE_GANONS_CASTLE }
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

std::string sanitize(std::string stringValue) {
    // Add backslashes.
    for (auto i = stringValue.begin();;) {
        auto const pos = std::find_if(i, stringValue.end(), [](char const c) { return '\\' == c || '\'' == c || '"' == c; });
        if (pos == stringValue.end()) {
            break;
        }
        i = std::next(stringValue.insert(pos, '\\'), 2);
    }

    // Removes others.
    stringValue.erase(std::remove_if(stringValue.begin(), stringValue.end(), [](char const c) {
        return '\n' == c || '\r' == c || '\0' == c || '\x1A' == c; }), stringValue.end());

    return stringValue;
}

#pragma optimize("", off)
#pragma GCC push_options
#pragma GCC optimize ("O0")
bool Randomizer::SpoilerFileExists(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        std::ifstream spoilerFileStream(SohUtils::Sanitize(spoilerFileName));
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

void Randomizer::LoadHintMessages() {
    auto ctx = Rando::Context::GetInstance();

    CustomMessageManager::Instance->ClearMessageTable(Randomizer::hintMessageTableID);
    CustomMessageManager::Instance->AddCustomMessageTable(Randomizer::hintMessageTableID);

    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_ALTAR_CHILD,
        CustomMessage(ctx->GetHint(RH_ALTAR_CHILD)->GetText(), TEXTBOX_TYPE_BLUE));
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_ALTAR_ADULT,
        CustomMessage(ctx->GetHint(RH_ALTAR_ADULT)->GetText(), TEXTBOX_TYPE_BLUE));
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_GANONDORF,
        CustomMessage(ctx->GetHint(RH_GANONDORF_HINT)->GetText()));
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_GANONDORF_NOHINT,
        CustomMessage(ctx->GetHint(RH_SARIA)->GetText()));//RANDOTODO: Change to RH_BLANK or remove {{message}} replacment
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_SHEIK_NEED_HOOK,
        CustomMessage("{{message}}", "{{message}}", "{{message}}"));
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_SHEIK_HAVE_HOOK,
        CustomMessage("{{message}}", "{{message}}", "{{message}}"));
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_SARIAS_SONG_FACE_TO_FACE,
        CustomMessage(ctx->GetHint(RH_SARIA)->GetText(), TEXTBOX_TYPE_BLUE));

    for (int i : Rando::StaticData::gossipStoneLocations) {
        RandomizerHintKey rhk = RandomizerHintKey(i - RC_COLOSSUS_GOSSIP_STONE + 1);
        CustomMessageManager::Instance->CreateMessage(
            Randomizer::hintMessageTableID, i, CustomMessage(ctx->GetHint(rhk)->GetText()));
    }

    //Extra Hints
    CustomMessageManager::Instance->ClearMessageTable(Randomizer::randoMiscHintsTableID);
    CustomMessageManager::Instance->AddCustomMessageTable(Randomizer::randoMiscHintsTableID);

    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_SKULLTULA_PEOPLE_IM_CURSED,
            CustomMessage("Yeaaarrgh! I'm cursed!!^Please save me by destroying&%r{{params}} Spiders of the Curse%w&and I will give you my&%g{{item1}}%w!",
                "Yeaaarrgh! Ich bin verflucht!^Bitte rette mich, indem du %r{{params}} Skulltulas&%wzerstörst und ich werde dir dafür&%g{{item1}} %wgeben!",
                "Yeaaarrgh! Je suis maudit!^Détruit encore %r{{params}} Araignées de&la Malédiction%w et j'aurai quelque&chose à te donner!&%g({{item1}})")
        );
    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_DAMPES_DIARY,
            CustomMessage(ctx->GetHint(RH_DAMPES_DIARY)->GetText())
        );
    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_CHEST_GAME_PROCEED,
            CustomMessage(ctx->GetHint(RH_GREG_RUPEE)->GetText())
        );
    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_FROGS_UNDERWATER,
            CustomMessage(ctx->GetHint(RH_FROGS)->GetText().GetEnglish(),
                ctx->GetHint(RH_FROGS)->GetText().GetGerman(),
                ctx->GetHint(RH_FROGS)->GetText().GetFrench(), TEXTBOX_TYPE_BLUE)
        );
    CustomMessageManager::Instance->CreateMessage(Randomizer::randoMiscHintsTableID, TEXT_WARP_MINUET_OF_FOREST,
        CustomMessage(ctx->GetHint(RH_MINUET_WARP_LOC)->GetText().GetEnglish(),
            ctx->GetHint(RH_MINUET_WARP_LOC)->GetText().GetGerman(),
            ctx->GetHint(RH_MINUET_WARP_LOC)->GetText().GetFrench()));
    CustomMessageManager::Instance->CreateMessage(Randomizer::randoMiscHintsTableID, TEXT_WARP_BOLERO_OF_FIRE,
        CustomMessage(ctx->GetHint(RH_BOLERO_WARP_LOC)->GetText().GetEnglish(),
            ctx->GetHint(RH_BOLERO_WARP_LOC)->GetText().GetGerman(),
            ctx->GetHint(RH_BOLERO_WARP_LOC)->GetText().GetFrench()));
    CustomMessageManager::Instance->CreateMessage(Randomizer::randoMiscHintsTableID, TEXT_WARP_SERENADE_OF_WATER,
        CustomMessage(ctx->GetHint(RH_SERENADE_WARP_LOC)->GetText().GetEnglish(),
            ctx->GetHint(RH_SERENADE_WARP_LOC)->GetText().GetGerman(),
            ctx->GetHint(RH_SERENADE_WARP_LOC)->GetText().GetFrench()));
    CustomMessageManager::Instance->CreateMessage(Randomizer::randoMiscHintsTableID, TEXT_WARP_REQUIEM_OF_SPIRIT,
        CustomMessage(ctx->GetHint(RH_REQUIEM_WARP_LOC)->GetText().GetEnglish(),
            ctx->GetHint(RH_REQUIEM_WARP_LOC)->GetText().GetGerman(),
            ctx->GetHint(RH_REQUIEM_WARP_LOC)->GetText().GetFrench()));

    CustomMessageManager::Instance->CreateMessage(
        Randomizer::randoMiscHintsTableID, TEXT_SARIAS_SONG_FOREST_SOUNDS,
            CustomMessage("{{message}}", "{{message}}", "{{message}}", TEXTBOX_TYPE_BLUE)
        );

    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_BIGGORON_BETTER_AT_SMITHING,
            CustomMessage("Arrrrrre you here to claim my finest&%g{{item1}}%w?&Shoooooow me your %rClaim Check.%w",
            "",
            "")
        );

    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_GHOST_SHOP_CARD_HAS_POINTS,
            CustomMessage("You have %g\x1E\x01%r Poe Points%w!&Reach 1000 and you'll get a&%g{{item1}}%w!",
            "",
            "")
        );

    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_ANJU_PLEASE_BRING_MY_CUCCOS_BACK,
            CustomMessage("You! Please!&Bring my Cucco's back to my pen!&I'll give you my %g{{item1}}%w!",
            "",
            "")
        );

    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_MALON_OBSTICLE_COURSE,
            CustomMessage("How about trying the %rObsticle Course?%w&If you beat my time I'll let you keep&my favourite cow Elsie and&her toy %g{{item1}}%w!^"
            "Challenge the %rObsticle Course?&\x1B&%gLet's go&No thanks%w",
            "",
            "")
        );

    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_MALON_HOW_IS_EPONA_DOING,
            CustomMessage("@! You should come back &with Epona and try to beat my time&on the %rObsticle Course%w!^If you beat my time, I'll give you&my favourite %rcow%w Elsie and&her toy %g{{item1}}%w!",
            "",
            "")
        );

    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_MALON_EVERYONE_TURNING_EVIL,
            CustomMessage("@? Is that you? ^If I ran the ranch, I'd build an &%rObsticle Course%w, and whoever gets&the best time would win a %rcow%w!^Elsie loves sharing her %g{{item1}}%w&with new people, It'll be fun!^...But Ingo won't let me...",
            "",
            "")
        );

    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_MALON_INGO_MUST_HAVE_BEEN_TEMPTED,
            CustomMessage("@! You should come back in&the morning and try to beat my time&on the %rObsticle Course%w!^If you beat my time, I'll give you&my favourite %rcow%w Elsie and&her toy %g{{item1}}%w!",
            "",
            "")
        );

    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_HBA_NOT_ON_HORSE,
            CustomMessage("Hey, rookie!&Come back on your %rhorse%w&and take on the&%rHorseback Archery%w challenge!^Impress me with a high score of 1000&to win a %g{{item1}}%w&or score 1500 for my&%g{{item2}}%w!",
            "",
            "")
        );

    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_HBA_INITIAL_EXPLAINATION,
            CustomMessage("Hey, rookie!&Want to take on the&%rHorseback Archery%w challenge?^Impress me with a high score of 1000&to win a %g{{item1}}%w&or score 1500 for my&%g{{item2}}%w!\x0B",
            "",
            "")
        );

    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_HBA_ALREADY_HAVE_1000,
            CustomMessage("Hey, newcomer!&Want to take on the&%rHorseback Archery%w challenge?^Prove yourself to be a horsemaster&by scoring 1500 points to win &my %g{{item1}}%w!\x0B",
            "",
            "")
        );

    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_GF_HBA_SIGN,
            CustomMessage("%rHorseback Archery%w Range Prizes:&1000: %g{{item1}}%w&1500: %g{{item2}}%w^@'s Record: %g\x1E\x00%w",
            "",
            "")
        );

    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_MALON_OBSTICLE_COURSE,
            CustomMessage("How about trying your skill on the %rObsticle Course?%w& If you beat my time I'll let you keep my favourite cow Elsie and& her toy %g{{item1}}%w!&x1B&%gLet's go&No thanks%w",
            "",
            "")
        );

    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_MALON_EVERYONE_TURNING_EVIL,
            CustomMessage("@? Is that you? &If I ran the ranch, I'd build an %rObsticle Course%w, and whoever gets the best time would win a cow!& Elsie loves sharing her %g{{item1}}%w&with new people, It'll be fun!&...But Ingo won't let me...",
            "",
            "")
        );

    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_MALON_INGO_MUST_HAVE_BEEN_TEMPTED,
            CustomMessage("@! You should come back in the morning and try to beat my time on the %rObsticle Course%w!&If you beat my time, I'll let you keep Elsie and& her toy %g{{item1}}%w!",
            "",
            "")
        );

    CustomMessageManager::Instance->CreateMessage(Randomizer::randoMiscHintsTableID, TEXT_WARP_NOCTURNE_OF_SHADOW,
        CustomMessage(ctx->GetHint(RH_NOCTURNE_WARP_LOC)->GetText().GetEnglish(),
            ctx->GetHint(RH_NOCTURNE_WARP_LOC)->GetText().GetGerman(),
            ctx->GetHint(RH_NOCTURNE_WARP_LOC)->GetText().GetFrench()));
    CustomMessageManager::Instance->CreateMessage(Randomizer::randoMiscHintsTableID, TEXT_WARP_PRELUDE_OF_LIGHT,
        CustomMessage(ctx->GetHint(RH_PRELUDE_WARP_LOC)->GetText().GetEnglish(),
            ctx->GetHint(RH_PRELUDE_WARP_LOC)->GetText().GetGerman(),
            ctx->GetHint(RH_PRELUDE_WARP_LOC)->GetText().GetFrench()));

    // Bow Shooting Gallery reminder
    CustomMessageManager::Instance->CreateMessage(Randomizer::hintMessageTableID, TEXT_SHOOTING_GALLERY_MAN_COME_BACK_WITH_BOW,
        CustomMessage("Come back when you have your own&bow and you'll get a %rdifferent prize%w!",
        "Komm wieder sobald du deinen eigenen&Bogen hast, um einen %rspeziellen Preis%w zu&erhalten!",
        "J'aurai %rune autre récompense%w pour toi&lorsque tu auras ton propre arc."));

    // Fishing pond pole hint
    CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_FISHING_POND_START,
            CustomMessage(ctx->GetHint(RH_FISHING_POLE)->GetText().GetEnglish(),
                ctx->GetHint(RH_FISHING_POLE)->GetText().GetEnglish(),
                ctx->GetHint(RH_FISHING_POLE)->GetText().GetFrench())
        );
        CustomMessageManager::Instance->CreateMessage(
            Randomizer::randoMiscHintsTableID, TEXT_FISHING_POND_START_MET,
            CustomMessage(ctx->GetHint(RH_FISHING_POLE)->GetText().GetEnglish(),
                ctx->GetHint(RH_FISHING_POLE)->GetText().GetEnglish(),
                ctx->GetHint(RH_FISHING_POLE)->GetText().GetFrench())
        );

    // Lake Hylia water level system
    CustomMessageManager::Instance->CreateMessage(Randomizer::hintMessageTableID, TEXT_LAKE_HYLIA_WATER_SWITCH_SIGN,
        CustomMessage("Water level control system.&Keep away!",
            "Wasserstand Kontrollsystem&Finger weg!",
            "Système de contrôle du niveau&d'eau.&Ne pas toucher!"));
    CustomMessageManager::Instance->CreateMessage(Randomizer::hintMessageTableID, TEXT_LAKE_HYLIA_WATER_SWITCH_NAVI,
        CustomMessage("%cThis switch is rustier than you think.^%cSomething must be wrong with the&pipe system in the %bWater Temple%c.",
            "%cDieser Schalter scheint rostiger zu&sein als er aussieht.^%cEtwas muss mit dem Leitungssystem&im %bWassertempel%c nicht stimmen.",
            "%cCet interrupteur est très rouillé.^%cIl doit y avoir un problème avec&la tuyauterie du %bTemple de l'Eau%c."));
}

// Reference soh/src/overlays/actors/ovl_En_GirlA/z_en_girla.h
std::unordered_map<RandomizerGet, EnGirlAShopItem> randomizerGetToEnGirlShopItem = {
    { RG_BUY_DEKU_NUTS_5,   SI_DEKU_NUTS_5 },
    { RG_BUY_ARROWS_30,     SI_ARROWS_30 },
    { RG_BUY_ARROWS_50,     SI_ARROWS_50 },
    { RG_BUY_BOMBS_525,     SI_BOMBS_5_R25 },
    { RG_BUY_DEKU_NUTS_10,  SI_DEKU_NUTS_10 },
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

void Randomizer::LoadMerchantMessages() {
    auto ctx = Rando::Context::GetInstance();
    CustomMessageManager::Instance->ClearMessageTable(Randomizer::merchantMessageTableID);
    CustomMessageManager::Instance->AddCustomMessageTable(Randomizer::merchantMessageTableID);

    // Prices have a chance of being 0, and the "sell" message below doesn't really make sense for a free item, so adding a "free" variation here
    CustomMessageManager::Instance->CreateMessage(Randomizer::merchantMessageTableID, TEXT_SCRUB_RANDOM_FREE,
        CustomMessage("\x12\x38\x82" "All right! You win! In return for&sparing me, I will give you a&%g{{item}}%w!&Please, take it!\x07\x10\xA3",
            "\x12\x38\x82" "In Ordnung! Du gewinnst! Im Austausch&dafür, dass du mich verschont hast,&werde ich dir einen &%g{{item}}%w geben!\x07\x10\xA3",
            "\x12\x38\x82" "J'me rends! Laisse-moi partir et en&échange, je te donne un &%g{{item}}%w! Vas-y prends le!\x07\x10\xA3"));
    CustomMessageManager::Instance->CreateMessage(Randomizer::merchantMessageTableID, TEXT_SCRUB_RANDOM,
        CustomMessage("\x12\x38\x82" "All right! You win! In return for&sparing me, I will sell you a&%g{{item}}%w!&%r{{price}} Rupees%w it is!\x07\x10\xA3",
            "\x12\x38\x82" "Aufgeben! Ich verkaufe dir einen&%g{{item}}%w&für %r{{price}} Rubine%w!\x07\x10\xA3",
            "\x12\x38\x82" "J'abandonne! Tu veux bien m'acheter&un %g{{item}}%w?&Ça fera %r{{price}} Rubis%w!\x07\x10\xA3"));
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::merchantMessageTableID, TEXT_BEAN_SALESMAN_BUY_FOR_10,
        CustomMessage(ctx->GetHint(RH_BEAN_SALESMAN)->GetText().GetEnglish(),
            ctx->GetHint(RH_BEAN_SALESMAN)->GetText().GetGerman(),
            ctx->GetHint(RH_BEAN_SALESMAN)->GetText().GetFrench()));


    //Setup for merchant text boxes
    //Medigoron
    //RANDOTODO: Implement obscure/ambiguous hints
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::merchantMessageTableID, TEXT_MEDIGORON,
        CustomMessage(ctx->GetHint(RH_MEDIGORON)->GetText().GetEnglish(),
            ctx->GetHint(RH_MEDIGORON)->GetText().GetGerman(),
            ctx->GetHint(RH_MEDIGORON)->GetText().GetFrench()));

    //Granny Shopy
    //RANDOTODO: Implement obscure/ambiguous hints
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::merchantMessageTableID, TEXT_GRANNYS_SHOP,
        CustomMessage(ctx->GetHint(RH_GRANNYS_SHOP)->GetText().GetEnglish(),
            ctx->GetHint(RH_GRANNYS_SHOP)->GetText().GetGerman(),
            ctx->GetHint(RH_GRANNYS_SHOP)->GetText().GetFrench()));

    //Carpet Salesman
    //RANDOTODO: Implement obscure/ambiguous hints
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::merchantMessageTableID, TEXT_CARPET_SALESMAN_1,
        CustomMessage(ctx->GetHint(RH_WASTELAND_BOMBCHU_SALESMAN)->GetText().GetEnglish(),
            ctx->GetHint(RH_WASTELAND_BOMBCHU_SALESMAN)->GetText().GetGerman(),
            ctx->GetHint(RH_WASTELAND_BOMBCHU_SALESMAN)->GetText().GetFrench()));
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::merchantMessageTableID, TEXT_CARPET_SALESMAN_2,
        CustomMessage(ctx->GetHint(RH_WASTELAND_BOMBCHU_SALESMAN_POST)->GetText().GetEnglish(),
            ctx->GetHint(RH_WASTELAND_BOMBCHU_SALESMAN_POST)->GetText().GetGerman(),
            ctx->GetHint(RH_WASTELAND_BOMBCHU_SALESMAN_POST)->GetText().GetFrench()));

        // Each shop item has two messages, one for when the cursor is over it, and one for when you select it and are
        // prompted buy/don't buy
        CustomMessageManager::Instance->CreateMessage(
            Randomizer::merchantMessageTableID, TEXT_SHOP_ITEM_RANDOM,
            CustomMessage("\x08%r{{item}}  {{price}} Rupees&%wSpecial deal! ONE LEFT!&Get it while it lasts!\x0A\x02",
                "\x08%r{{item}}  {{price}} Rubine&%wSonderangebot! NUR NOCH EINES VERFÜGBAR!&Beeilen Sie sich!\x0A\x02",
                "\x08%r{{item}}  {{price}} Rubis&%wOffre spéciale! DERNIER EN STOCK!&Faites vite!\x0A\x02"));
        CustomMessageManager::Instance->CreateMessage(
            Randomizer::merchantMessageTableID, TEXT_SHOP_ITEM_RANDOM_CONFIRM,
            CustomMessage("\x08{{item}}  {{price}} Rupees\x09&&\x1B%gBuy&Don't buy%w\x09\x02",
                "\x08{{item}}  {{price}} Rubine\x09&&\x1B%gKaufen&Nicht kaufen%w\x09\x02",
                "\x08{{item}}  {{price}} Rubis\x09&&\x1B%gAcheter&Ne pas acheter%w\x09\x02"));
}

bool Randomizer::IsTrialRequired(RandomizerInf trial) {
    return Rando::Context::GetInstance()->GetTrial(trial - RAND_INF_TRIALS_DONE_LIGHT_TRIAL)->IsRequired();
}

GetItemEntry Randomizer::GetItemFromActor(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogItemId,
                                          bool checkObtainability) {
    return Rando::Context::GetInstance()->GetFinalGIEntry(GetCheckFromActor(actorId, sceneNum, actorParams),
                                                          checkObtainability, ogItemId);
}

ItemObtainability Randomizer::GetItemObtainabilityFromRandomizerCheck(RandomizerCheck randomizerCheck) {
    return GetItemObtainabilityFromRandomizerGet(
        Rando::Context::GetInstance()->GetItemLocation(randomizerCheck)->GetPlacedRandomizerGet());
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
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_KOKIRI) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_MASTER_SWORD:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BIGGORON_SWORD:
            return !gSaveContext.bgsFlag ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DEKU_SHIELD:
        case RG_BUY_DEKU_SHIELD:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_SHIELD, EQUIP_INV_SHIELD_DEKU) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_HYLIAN_SHIELD:
        case RG_BUY_HYLIAN_SHIELD:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_SHIELD, EQUIP_INV_SHIELD_HYLIAN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_MIRROR_SHIELD:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_SHIELD, EQUIP_INV_SHIELD_MIRROR) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GORON_TUNIC:
        case RG_BUY_GORON_TUNIC:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_GORON) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_ZORA_TUNIC:
        case RG_BUY_ZORA_TUNIC:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_ZORA) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_IRON_BOOTS:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_BOOTS, EQUIP_INV_BOOTS_IRON) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_HOVER_BOOTS:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_BOOTS, EQUIP_INV_BOOTS_HOVER) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

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
        case RG_FISH:
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
        case RG_FISHING_POLE:
            return !Flags_GetRandomizerInf(RAND_INF_FISHING_POLE_FOUND) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

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
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_DEKU_TREE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DODONGOS_CAVERN_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_DODONGOS_CAVERN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_JABU_JABUS_BELLY_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_JABU_JABU) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FOREST_TEMPLE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_FOREST_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_TEMPLE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_FIRE_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_WATER_TEMPLE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_WATER_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SPIRIT_TEMPLE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_SPIRIT_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SHADOW_TEMPLE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_SHADOW_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BOTTOM_OF_THE_WELL_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_BOTTOM_OF_THE_WELL) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_ICE_CAVERN_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_ICE_CAVERN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DEKU_TREE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_DEKU_TREE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DODONGOS_CAVERN_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_DODONGOS_CAVERN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_JABU_JABUS_BELLY_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_JABU_JABU) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FOREST_TEMPLE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_FOREST_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_TEMPLE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_FIRE_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_WATER_TEMPLE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_WATER_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SPIRIT_TEMPLE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_SPIRIT_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SHADOW_TEMPLE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_SHADOW_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BOTTOM_OF_THE_WELL_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_BOTTOM_OF_THE_WELL) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_ICE_CAVERN_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_ICE_CAVERN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FOREST_TEMPLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_FOREST_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_TEMPLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_FIRE_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_WATER_TEMPLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_WATER_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SPIRIT_TEMPLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_SPIRIT_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SHADOW_TEMPLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_SHADOW_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GANONS_CASTLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_GANONS_TOWER) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FOREST_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_FOREST_TEMPLE] < FOREST_TEMPLE_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_FIRE_TEMPLE] < FIRE_TEMPLE_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_WATER_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_WATER_TEMPLE] < WATER_TEMPLE_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SPIRIT_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_SPIRIT_TEMPLE] < SPIRIT_TEMPLE_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SHADOW_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_SHADOW_TEMPLE] < SHADOW_TEMPLE_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BOTTOM_OF_THE_WELL_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_BOTTOM_OF_THE_WELL] < BOTTOM_OF_THE_WELL_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_GERUDO_TRAINING_GROUND] < GERUDO_TRAINING_GROUNDS_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GERUDO_FORTRESS_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_THIEVES_HIDEOUT] < GERUDO_FORTRESS_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GANONS_CASTLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_INSIDE_GANONS_CASTLE] < GANONS_CASTLE_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_TREASURE_GAME_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_TREASURE_BOX_SHOP] < TREASURE_GAME_SMALL_KEY_MAX ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

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

        // Ocarina Buttons
        case RG_OCARINA_A_BUTTON:
            return Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_A) ? CANT_OBTAIN_ALREADY_HAVE : CAN_OBTAIN;
        case RG_OCARINA_C_LEFT_BUTTON:
            return Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_LEFT) ? CANT_OBTAIN_ALREADY_HAVE : CAN_OBTAIN;
        case RG_OCARINA_C_RIGHT_BUTTON:
            return Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_RIGHT) ? CANT_OBTAIN_ALREADY_HAVE : CAN_OBTAIN;
        case RG_OCARINA_C_UP_BUTTON:
            return Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_UP) ? CANT_OBTAIN_ALREADY_HAVE : CAN_OBTAIN;
        case RG_OCARINA_C_DOWN_BUTTON:
            return Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_DOWN) ? CANT_OBTAIN_ALREADY_HAVE : CAN_OBTAIN;

        case RG_RECOVERY_HEART:
        case RG_GREEN_RUPEE:
        case RG_GREG_RUPEE:
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
        case RG_BUY_DEKU_NUTS_5:
        case RG_BUY_DEKU_NUTS_10:
        case RG_BUY_DEKU_STICK_1:
        case RG_BUY_HEART:
        case RG_TRIFORCE_PIECE:
        default:
            return CAN_OBTAIN;
    }
}

GetItemID Randomizer::GetItemIdFromRandomizerGet(RandomizerGet randoGet, GetItemID ogItemId) {
    // Shopsanity with at least one item shuffled allows for a third wallet upgrade.
    // This is needed since Plentiful item pool also adds a third progressive wallet
    // but we should *not* get Tycoon's Wallet in that mode.
    u8 numWallets = GetRandoSettingValue(RSK_SHOPSANITY) > RO_SHOPSANITY_ZERO_ITEMS ? 3 : 2;
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
                case 3:
                    return GI_STICK_UPGRADE_30;
            }
        case RG_PROGRESSIVE_NUT_UPGRADE:
            switch (CUR_UPG_VALUE(UPG_NUTS)) {
                case 0:
                case 1:
                    return GI_NUT_UPGRADE_30;
                case 2:
                case 3:
                    return GI_NUT_UPGRADE_40;
            }
        case RG_PROGRESSIVE_BOMB_BAG:
            switch (CUR_UPG_VALUE(UPG_BOMB_BAG)) {
                case 0:
                    return GI_BOMB_BAG_20;
                case 1:
                    return GI_BOMB_BAG_30;
                case 2:
                case 3:
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
                case 3:
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
                case 3:
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
                case ITEM_OCARINA_TIME:
                    return GI_OCARINA_OOT;
            }
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
                case ITEM_LONGSHOT:
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
        case RG_FISH:
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
                case 3:
                    return GI_GAUNTLETS_GOLD;
            }
        case RG_PROGRESSIVE_WALLET:
            switch (CUR_UPG_VALUE(UPG_WALLET)) {
                case 0:
                    return GI_WALLET_ADULT;
                case 1:
                    return GI_WALLET_GIANT;
                case 2:
                case 3:
                    return numWallets == 3 ? (GetItemID)RG_TYCOON_WALLET : GI_WALLET_GIANT;
            }
        case RG_PROGRESSIVE_SCALE:
            switch (CUR_UPG_VALUE(UPG_SCALE)) {
                case 0:
                    return GI_SCALE_SILVER;
                case 1:
                case 2:
                    return GI_SCALE_GOLD;
            }
        case RG_PROGRESSIVE_MAGIC_METER:
            switch (gSaveContext.magicLevel) {
                case 0:
                    return (GetItemID)RG_MAGIC_SINGLE;
                case 1:
                case 2:
                    return (GetItemID)RG_MAGIC_DOUBLE;
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
        case RG_BUY_DEKU_NUTS_5:
            return GI_NUTS_5;
        case RG_DEKU_NUTS_10:
        case RG_BUY_DEKU_NUTS_10:
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

        //Ocarina Buttons
        case RG_OCARINA_A_BUTTON:
            return (GetItemID)RG_OCARINA_A_BUTTON;
        case RG_OCARINA_C_LEFT_BUTTON:
            return (GetItemID)RG_OCARINA_C_LEFT_BUTTON;
        case RG_OCARINA_C_RIGHT_BUTTON:
            return (GetItemID)RG_OCARINA_C_RIGHT_BUTTON;
        case RG_OCARINA_C_UP_BUTTON:
            return (GetItemID)RG_OCARINA_C_UP_BUTTON;
        case RG_OCARINA_C_DOWN_BUTTON:
            return (GetItemID)RG_OCARINA_C_DOWN_BUTTON;

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
        case RG_FISH:
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
        case RG_BUY_DEKU_NUTS_5:
        case RG_BUY_ARROWS_30:
        case RG_BUY_ARROWS_50:
        case RG_BUY_BOMBS_525:
        case RG_BUY_DEKU_NUTS_10:
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
        default:
            return false;
    }
}

bool Randomizer::CheckContainsVanillaItem(RandomizerCheck randoCheck) {
    RandomizerGet randoGet = Rando::Context::GetInstance()->GetItemLocation(randoCheck)->GetPlacedRandomizerGet();
    return IsItemVanilla(randoGet);
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
    { RC_TOT_MASTER_SWORD,                                            RAND_INF_TOT_MASTER_SWORD                      },
    { RC_GC_MEDIGORON,                                                RAND_INF_MERCHANTS_MEDIGORON                   },
    { RC_KAK_GRANNYS_SHOP,                                            RAND_INF_MERCHANTS_GRANNYS_SHOP                },
    { RC_WASTELAND_BOMBCHU_SALESMAN,                                  RAND_INF_MERCHANTS_CARPET_SALESMAN             },
    { RC_LW_TRADE_COJIRO,                                             RAND_INF_ADULT_TRADES_LW_TRADE_COJIRO },
    { RC_GV_TRADE_SAW,                                                RAND_INF_ADULT_TRADES_GV_TRADE_SAW },
    { RC_DMT_TRADE_BROKEN_SWORD,                                      RAND_INF_ADULT_TRADES_DMT_TRADE_BROKEN_SWORD },
    { RC_LH_TRADE_FROG,                                               RAND_INF_ADULT_TRADES_LH_TRADE_FROG },
    { RC_DMT_TRADE_EYEDROPS,                                          RAND_INF_ADULT_TRADES_DMT_TRADE_EYEDROPS },
    { RC_LH_CHILD_FISHING,                                            RAND_INF_CHILD_FISHING },
    { RC_LH_ADULT_FISHING,                                            RAND_INF_ADULT_FISHING },
    { RC_MARKET_10_BIG_POES,                                          RAND_INF_10_BIG_POES },
    { RC_KAK_100_GOLD_SKULLTULA_REWARD,                               RAND_INF_KAK_100_GOLD_SKULLTULA_REWARD },
    { RC_LH_CHILD_FISH_1,                                             RAND_INF_CHILD_FISH_1 },
    { RC_LH_CHILD_FISH_2,                                             RAND_INF_CHILD_FISH_2 },
    { RC_LH_CHILD_FISH_3,                                             RAND_INF_CHILD_FISH_3 },
    { RC_LH_CHILD_FISH_4,                                             RAND_INF_CHILD_FISH_4 },
    { RC_LH_CHILD_FISH_5,                                             RAND_INF_CHILD_FISH_5 },
    { RC_LH_CHILD_FISH_6,                                             RAND_INF_CHILD_FISH_6 },
    { RC_LH_CHILD_FISH_7,                                             RAND_INF_CHILD_FISH_7 },
    { RC_LH_CHILD_FISH_8,                                             RAND_INF_CHILD_FISH_8 },
    { RC_LH_CHILD_FISH_9,                                             RAND_INF_CHILD_FISH_9 },
    { RC_LH_CHILD_FISH_10,                                            RAND_INF_CHILD_FISH_10 },
    { RC_LH_CHILD_FISH_11,                                            RAND_INF_CHILD_FISH_11 },
    { RC_LH_CHILD_FISH_12,                                            RAND_INF_CHILD_FISH_12 },
    { RC_LH_CHILD_FISH_13,                                            RAND_INF_CHILD_FISH_13 },
    { RC_LH_CHILD_FISH_14,                                            RAND_INF_CHILD_FISH_14 },
    { RC_LH_CHILD_FISH_15,                                            RAND_INF_CHILD_FISH_15 },
    { RC_LH_CHILD_LOACH_1,                                            RAND_INF_CHILD_LOACH_1 },
    { RC_LH_CHILD_LOACH_2,                                            RAND_INF_CHILD_LOACH_2 },
    { RC_LH_ADULT_FISH_1,                                             RAND_INF_ADULT_FISH_1 },
    { RC_LH_ADULT_FISH_2,                                             RAND_INF_ADULT_FISH_2 },
    { RC_LH_ADULT_FISH_3,                                             RAND_INF_ADULT_FISH_3 },
    { RC_LH_ADULT_FISH_4,                                             RAND_INF_ADULT_FISH_4 },
    { RC_LH_ADULT_FISH_5,                                             RAND_INF_ADULT_FISH_5 },
    { RC_LH_ADULT_FISH_6,                                             RAND_INF_ADULT_FISH_6 },
    { RC_LH_ADULT_FISH_7,                                             RAND_INF_ADULT_FISH_7 },
    { RC_LH_ADULT_FISH_8,                                             RAND_INF_ADULT_FISH_8 },
    { RC_LH_ADULT_FISH_9,                                             RAND_INF_ADULT_FISH_9 },
    { RC_LH_ADULT_FISH_10,                                            RAND_INF_ADULT_FISH_10 },
    { RC_LH_ADULT_FISH_11,                                            RAND_INF_ADULT_FISH_11 },
    { RC_LH_ADULT_FISH_12,                                            RAND_INF_ADULT_FISH_12 },
    { RC_LH_ADULT_FISH_13,                                            RAND_INF_ADULT_FISH_13 },
    { RC_LH_ADULT_FISH_14,                                            RAND_INF_ADULT_FISH_14 },
    { RC_LH_ADULT_FISH_15,                                            RAND_INF_ADULT_FISH_15 },
    { RC_LH_ADULT_LOACH,                                              RAND_INF_ADULT_LOACH },
    { RC_ZR_OPEN_GROTTO_FISH,                                         RAND_INF_GROTTO_FISH_ZR_OPEN_GROTTO },
    { RC_DMC_UPPER_GROTTO_FISH,                                       RAND_INF_GROTTO_FISH_DMC_UPPER_GROTTO },
    { RC_DMT_STORMS_GROTTO_FISH,                                      RAND_INF_GROTTO_FISH_DMT_STORMS_GROTTO },
    { RC_KAK_OPEN_GROTTO_FISH,                                        RAND_INF_GROTTO_FISH_KAK_OPEN_GROTTO },
    { RC_HF_NEAR_MARKET_GROTTO_FISH,                                  RAND_INF_GROTTO_FISH_HF_NEAR_MARKET_GROTTO },
    { RC_HF_OPEN_GROTTO_FISH,                                         RAND_INF_GROTTO_FISH_HF_OPEN_GROTTO },
    { RC_HF_SOUTHEAST_GROTTO_FISH,                                    RAND_INF_GROTTO_FISH_HF_SOUTHEAST_GROTTO },
    { RC_LW_NEAR_SHORTCUTS_GROTTO_FISH,                               RAND_INF_GROTTO_FISH_LW_NEAR_SHORTCUTS_GROTTO },
    { RC_KF_STORMS_GROTTO_FISH,                                       RAND_INF_GROTTO_FISH_KF_STORMS_GROTTO },
    { RC_ZD_FISH_1,                                                   RAND_INF_ZD_FISH_1 },
    { RC_ZD_FISH_2,                                                   RAND_INF_ZD_FISH_2 },
    { RC_ZD_FISH_3,                                                   RAND_INF_ZD_FISH_3 },
    { RC_ZD_FISH_4,                                                   RAND_INF_ZD_FISH_4 },
    { RC_ZD_FISH_5,                                                   RAND_INF_ZD_FISH_5 },
};

Rando::Location* Randomizer::GetCheckObjectFromActor(s16 actorId, s16 sceneNum, s32 actorParams = 0x00) {
    auto fs = OTRGlobals::Instance->gRandoContext->GetFishsanity();
    RandomizerCheck specialRc = RC_UNKNOWN_CHECK;
    // TODO: Migrate these special cases into table, or at least document why they are special
    switch(sceneNum) {
        case SCENE_TREASURE_BOX_SHOP:
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
        case SCENE_SACRED_FOREST_MEADOW:
            if (actorId == ACTOR_EN_SA) {
                specialRc = RC_SONG_FROM_SARIA;
            }
            break;
        case SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY:
        case SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT:
        case SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS:
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
        case SCENE_HOUSE_OF_SKULLTULA:
            if (actorId == ACTOR_EN_SSH) {
                switch (actorParams) { // actor params are used to differentiate between textboxes
                    case 1: specialRc = RC_KAK_10_GOLD_SKULLTULA_REWARD; break;
                    case 2: specialRc = RC_KAK_20_GOLD_SKULLTULA_REWARD; break;
                    case 3: specialRc = RC_KAK_30_GOLD_SKULLTULA_REWARD; break;
                    case 4: specialRc = RC_KAK_40_GOLD_SKULLTULA_REWARD; break;
                    case 5: specialRc = RC_KAK_50_GOLD_SKULLTULA_REWARD; break;
                }
            }
            break;
        case SCENE_KAKARIKO_VILLAGE:
            switch (actorId) {
                case ACTOR_EN_NIW_LADY:
                    if (LINK_IS_ADULT) {
                        specialRc = RC_KAK_ANJU_AS_ADULT;
                    } else {
                        specialRc = RC_KAK_ANJU_AS_CHILD;
                    }
            }
            break;
        case SCENE_LAKE_HYLIA:
            switch (actorId) {
                case ACTOR_ITEM_ETCETERA:
                    if (LINK_IS_ADULT) {
                        specialRc = RC_LH_SUN;
                    } else {
                        specialRc = RC_LH_UNDERWATER_ITEM;
                    }
            }
            break;
        case SCENE_ZORAS_FOUNTAIN:
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
        case SCENE_GERUDOS_FORTRESS:
            // GF chest as child has different params and gives odd mushroom
            // set it to the GF chest check for both ages
            if (actorId == ACTOR_EN_BOX) {
                specialRc = RC_GF_CHEST;
            }
            break;
        case SCENE_DODONGOS_CAVERN:
            // special case for MQ DC Gossip Stone
            if (actorId == ACTOR_EN_GS && actorParams == 15892 && ResourceMgr_IsGameMasterQuest()) {
                specialRc = RC_DODONGOS_CAVERN_GOSSIP_STONE;
            }
            break;
        case SCENE_GROTTOS:
            // Grotto fish are identified by respawn data
            if (actorId == ACTOR_EN_FISH && actorParams == 1) {
                int8_t data = gSaveContext.respawn[RESPAWN_MODE_RETURN].data;
                if (Rando::StaticData::randomizerGrottoFishMap.contains(data)) {
                    specialRc = Rando::StaticData::randomizerGrottoFishMap[data];
                }
            }
            break;
    }

    if (specialRc != RC_UNKNOWN_CHECK) {
        return Rando::StaticData::GetLocation(specialRc);
    }

    auto range = checkFromActorMultimap.equal_range(std::make_tuple(actorId, sceneNum, actorParams));

    for (auto it = range.first; it != range.second; ++it) {
        if (
            Rando::StaticData::GetLocation(it->second)->GetQuest() == RCQUEST_BOTH ||
            (Rando::StaticData::GetLocation(it->second)->GetQuest() == RCQUEST_VANILLA && !ResourceMgr_IsGameMasterQuest()) ||
            (Rando::StaticData::GetLocation(it->second)->GetQuest() == RCQUEST_MQ && ResourceMgr_IsGameMasterQuest())
        ) {
            return Rando::StaticData::GetLocation(it->second);
        }
    }

    return Rando::StaticData::GetLocation(RC_UNKNOWN_CHECK);
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

    if (sceneNum == SCENE_GROTTOS) {
        actorParams = TWO_ACTOR_PARAMS(actorParams, respawnData);
    }

    Rando::Location* location = GetCheckObjectFromActor(ACTOR_EN_DNS, sceneNum, actorParams);

    if (location->GetRandomizerCheck() != RC_UNKNOWN_CHECK) {
        scrubIdentity.randomizerInf = rcToRandomizerInf[location->GetRandomizerCheck()];
        scrubIdentity.randomizerCheck = location->GetRandomizerCheck();
        scrubIdentity.getItemId = (GetItemID)Rando::StaticData::RetrieveItem(location->GetVanillaItem()).GetItemID();
        scrubIdentity.isShuffled = GetRandoSettingValue(RSK_SHUFFLE_SCRUBS) != RO_SCRUBS_OFF;

        if (location->GetRandomizerCheck() == RC_HF_DEKU_SCRUB_GROTTO || location->GetRandomizerCheck() == RC_LW_DEKU_SCRUB_GROTTO_FRONT || location->GetRandomizerCheck() == RC_LW_DEKU_SCRUB_NEAR_BRIDGE) {
            scrubIdentity.isShuffled = true;
        }

        scrubIdentity.itemPrice = OTRGlobals::Instance->gRandoContext->GetItemLocation(scrubIdentity.randomizerCheck)->GetPrice();
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

    Rando::Location* location = GetCheckObjectFromActor(ACTOR_EN_GIRLA,
        // Bazaar (SHOP1) scene is reused, so if entering from Kak use debug scene to identify
        (sceneNum == SCENE_BAZAAR && gSaveContext.entranceIndex == ENTR_BAZAAR_0) ? SCENE_TEST01 : sceneNum, slotIndex);

    if (location->GetRandomizerCheck() != RC_UNKNOWN_CHECK) {
        shopItemIdentity.randomizerInf = rcToRandomizerInf[location->GetRandomizerCheck()];
        shopItemIdentity.randomizerCheck = location->GetRandomizerCheck();
        shopItemIdentity.ogItemId = (GetItemID)Rando::StaticData::RetrieveItem(location->GetVanillaItem()).GetItemID();

        RandomizerGet randoGet =
            Rando::Context::GetInstance()->GetItemLocation(shopItemIdentity.randomizerCheck)->GetPlacedRandomizerGet();
        if (randomizerGetToEnGirlShopItem.find(randoGet) != randomizerGetToEnGirlShopItem.end()) {
            shopItemIdentity.enGirlAShopItem = randomizerGetToEnGirlShopItem[randoGet];
        }

        shopItemIdentity.itemPrice = OTRGlobals::Instance->gRandoContext->GetItemLocation(shopItemIdentity.randomizerCheck)->GetPrice();
    }

    return shopItemIdentity;
}

CowIdentity Randomizer::IdentifyCow(s32 sceneNum, s32 posX, s32 posZ) {
    struct CowIdentity cowIdentity;

    cowIdentity.randomizerInf = RAND_INF_MAX;
    cowIdentity.randomizerCheck = RC_UNKNOWN_CHECK;

    s32 actorParams = 0x00;
    // Only need to pass params if in a scene with two cows
    if (sceneNum == SCENE_GROTTOS || sceneNum == SCENE_STABLE || sceneNum == SCENE_LON_LON_BUILDINGS) {
        actorParams = TWO_ACTOR_PARAMS(posX, posZ);
    }

    Rando::Location* location = GetCheckObjectFromActor(ACTOR_EN_COW, sceneNum, actorParams);

    if (location->GetRandomizerCheck() != RC_UNKNOWN_CHECK) {
        cowIdentity.randomizerInf = rcToRandomizerInf[location->GetRandomizerCheck()];
        cowIdentity.randomizerCheck = location->GetRandomizerCheck();
    }

    return cowIdentity;
}

FishIdentity Randomizer::IdentifyFish(s32 sceneNum, s32 actorParams) {
    struct FishIdentity fishIdentity;

    fishIdentity.randomizerInf = RAND_INF_MAX;
    fishIdentity.randomizerCheck = RC_UNKNOWN_CHECK;

    // Fishsanity will determine what the identity of the fish should be
    if (sceneNum == SCENE_FISHING_POND) {
        return OTRGlobals::Instance->gRandoContext->GetFishsanity()->IdentifyPondFish(actorParams);
    }

    Rando::Location* location = GetCheckObjectFromActor(ACTOR_EN_FISH, sceneNum, actorParams);

    if (location->GetRandomizerCheck() != RC_UNKNOWN_CHECK) {
        fishIdentity.randomizerInf = rcToRandomizerInf[location->GetRandomizerCheck()];
        fishIdentity.randomizerCheck = location->GetRandomizerCheck();
    }

    return fishIdentity;
}

u8 Randomizer::GetRandoSettingValue(RandomizerSettingKey randoSettingKey) {
    return Rando::Context::GetInstance()->GetOption(randoSettingKey).GetSelectedOptionIndex();
}

GetItemEntry Randomizer::GetItemFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogItemId, bool checkObtainability) {
    return Rando::Context::GetInstance()->GetFinalGIEntry(randomizerCheck, checkObtainability);
}

RandomizerCheck Randomizer::GetCheckFromActor(s16 actorId, s16 sceneNum, s16 actorParams) {
    return GetCheckObjectFromActor(actorId, sceneNum, actorParams)->GetRandomizerCheck();
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

void GenerateRandomizerImgui(std::string seed = "") {
    CVarSetInteger("gRandoGenerating", 1);
    CVarSave();
    auto ctx = Rando::Context::GetInstance();
    if (!ctx->IsSpoilerLoaded()) {
        // We use the settings from the spoiler rather than CVars.
        ctx->GetSettings()->SetAllFromCVar();
    }
    // todo: this efficently when we build out cvar array support
    std::set<RandomizerCheck> excludedLocations;
    std::stringstream excludedLocationStringStream(CVarGetString("gRandomizeExcludedLocations", ""));
    std::string excludedLocationString;
    while (getline(excludedLocationStringStream, excludedLocationString, ',')) {
        excludedLocations.insert((RandomizerCheck)std::stoi(excludedLocationString));
    }

    // todo: better way to sort out linking tricks rather than name
    
    std::set<RandomizerTrick> enabledTricks;
    std::stringstream enabledTrickStringStream(CVarGetString("gRandomizeEnabledTricks", ""));
    std::string enabledTrickString;
    while (getline(enabledTrickStringStream, enabledTrickString, ',')) {
        enabledTricks.insert((RandomizerTrick)std::stoi(enabledTrickString));
    }
    

    // Update the visibilitiy before removing conflicting excludes (in case the locations tab wasn't viewed)
    RandomizerCheckObjects::UpdateImGuiVisibility();

    // Remove excludes for locations that are no longer allowed to be excluded
    for (auto& location : Rando::StaticData::GetLocationTable()) {
        auto elfound = excludedLocations.find(location.GetRandomizerCheck());
        if (!ctx->GetItemLocation(location.GetRandomizerCheck())->IsVisible() && elfound != excludedLocations.end()) {
            excludedLocations.erase(elfound);
        }
    }

    RandoMain::GenerateRando(excludedLocations, enabledTricks, seed);

    CVarSetInteger("gRandoGenerating", 0);
    CVarSave();
    CVarLoad();

    generated = 1;
}

bool GenerateRandomizer(std::string seed /*= ""*/) {
    if (generated) {
        generated = 0;
        randoThread.join();
    }
    if (CVarGetInteger("gRandoGenerating", 0) == 0) {
        randoThread = std::thread(&GenerateRandomizerImgui, seed);
        return true;
    }
    return false;
}

void RandomizerSettingsWindow::DrawElement() {
    auto ctx = Rando::Context::GetInstance();
    if (generated) {
        generated = 0;
        randoThread.join();
    }

    ImGui::SetNextWindowSize(ImVec2(920, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Randomizer Editor", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    bool disableEditingRandoSettings = CVarGetInteger("gRandoGenerating", 0) || CVarGetInteger("gOnFileSelectNameEntry", 0);
    if (disableEditingRandoSettings) {
        UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
    }

    ImGui::BeginDisabled(CVarGetInteger("gDisableChangingSettings", 0));
    DrawPresetSelector(PRESET_TYPE_RANDOMIZER);
    ImGui::EndDisabled();

    UIWidgets::Spacer(0);
    UIWidgets::EnhancementCheckbox("Manual seed entry", "gRandoManualSeedEntry", false, "");
    if (CVarGetInteger("gRandoManualSeedEntry", 0)) {
        ImGui::Text("Seed");
        ImGui::InputText("##RandomizerSeed", seedString, MAX_SEED_STRING_SIZE, ImGuiInputTextFlags_CallbackCharFilter, UIWidgets::TextFilters::FilterAlphaNum);
        UIWidgets::Tooltip(
            "Characters from a-z, A-Z, and 0-9 are supported.\n"
            "Character limit is 1023, after which the seed will be truncated.\n"
        );
        ImGui::SameLine();
        if (ImGui::Button("New Seed")) {
            SohUtils::CopyStringToCharArray(seedString, std::to_string(rand() & 0xFFFFFFFF), MAX_SEED_STRING_SIZE);
        }
        UIWidgets::Tooltip("Creates a new random seed value to be used when generating a randomizer");
        ImGui::SameLine();
        if (ImGui::Button("Clear Seed")) {
            memset(seedString, 0, MAX_SEED_STRING_SIZE);
        }
    }

    UIWidgets::Spacer(0);
    ImGui::BeginDisabled(CVarGetInteger("gRandomizerDontGenerateSpoiler", 0) && gSaveContext.gameMode != GAMEMODE_FILE_SELECT);
    if (ImGui::Button("Generate Randomizer")) {
        ctx->SetSpoilerLoaded(false);
        GenerateRandomizer(CVarGetInteger("gRandoManualSeedEntry", 0) ? seedString : "");
    }
    UIWidgets::Tooltip("You can also press L on the Quest Select screen to generate a new seed");
    ImGui::EndDisabled();

    UIWidgets::Spacer(0);
    if (!CVarGetInteger("gRandomizerDontGenerateSpoiler", 0)) {
        std::string spoilerfilepath = CVarGetString("gSpoilerLog", "");
        ImGui::Text("Spoiler File: %s", spoilerfilepath.c_str());
    }

    // RANDOTODO settings presets
    // std::string presetfilepath = CVarGetString("gLoadedPreset", "");
    // ImGui::Text("Settings File: %s", presetfilepath.c_str());

    UIWidgets::PaddedSeparator();

    ImGui::BeginDisabled(CVarGetInteger("gDisableChangingSettings", 0));

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    static ImVec2 cellPadding(8.0f, 8.0f);

    if (ImGui::BeginTabBar("Randomizer Settings", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("World")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (mSettings->GetOptionGroup(RSG_WORLD_IMGUI_TABLE).RenderImGui()) {
                mNeedsUpdate = true;
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }

        ImGui::BeginDisabled(CVarGetInteger("gRandomizeLogicRules", RO_LOGIC_GLITCHLESS) == RO_LOGIC_VANILLA);
        if (ImGui::BeginTabItem("Items")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            ImGui::BeginDisabled(CVarGetInteger("gRandomizeLogicRules", RO_LOGIC_GLITCHLESS) == RO_LOGIC_VANILLA);
            if (mSettings->GetOptionGroup(RSG_ITEMS_IMGUI_TABLE).RenderImGui()) {
                mNeedsUpdate = true;
            }
            ImGui::EndDisabled();
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled(CVarGetInteger("gRandomizeLogicRules", RO_LOGIC_GLITCHLESS) == RO_LOGIC_VANILLA);
        if (ImGui::BeginTabItem("Gameplay")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (mSettings->GetOptionGroup(RSG_GAMEPLAY_IMGUI_TABLE).RenderImGui()) {
                mNeedsUpdate = true;
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled(CVarGetInteger("gRandomizeLogicRules", RO_LOGIC_GLITCHLESS) == RO_LOGIC_VANILLA);
        static bool locationsTabOpen = false;
        if (ImGui::BeginTabItem("Locations")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (!locationsTabOpen) {
                locationsTabOpen = true;
                RandomizerCheckObjects::UpdateImGuiVisibility();
                // todo: this efficently when we build out cvar array support
                std::stringstream excludedLocationStringStream(CVarGetString("gRandomizeExcludedLocations", ""));
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
                for (auto& [rcArea, locations] : RandomizerCheckObjects::GetAllRCObjectsByArea()) {
                    bool hasItems = false;
                    for (RandomizerCheck rc : locations) {
                        if (ctx->GetItemLocation(rc)->IsVisible() && !excludedLocations.count(rc) &&
                            locationSearch.PassFilter(Rando::StaticData::GetLocation(rc)->GetName().c_str())) {

                            hasItems = true;
                            break;
                        }
                    }

                    if (hasItems) {
                        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                        if (ImGui::TreeNode(RandomizerCheckObjects::GetRCAreaName(rcArea).c_str())) {
                            for (auto& location : locations) {
                                if (ctx->GetItemLocation(location)->IsVisible() && !excludedLocations.count(location) &&
                                    locationSearch.PassFilter(Rando::StaticData::GetLocation(location)->GetName().c_str())) {

                                    if (ImGui::ArrowButton(std::to_string(location).c_str(), ImGuiDir_Right)) {
                                        excludedLocations.insert(location);
                                        // todo: this efficently when we build out cvar array support
                                        std::string excludedLocationString = "";
                                        for (auto excludedLocationIt : excludedLocations) {
                                            excludedLocationString += std::to_string(excludedLocationIt);
                                            excludedLocationString += ",";
                                        }
                                        CVarSetString("gRandomizeExcludedLocations", excludedLocationString.c_str());
                                        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                                    }
                                    ImGui::SameLine();
                                    ImGui::Text("%s", Rando::StaticData::GetLocation(location)->GetShortName().c_str());
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
                for (auto& [rcArea, locations] : RandomizerCheckObjects::GetAllRCObjectsByArea()) {
                    bool hasItems = false;
                    for (RandomizerCheck rc : locations) {
                        if (ctx->GetItemLocation(rc)->IsVisible() && excludedLocations.count(rc)) {
                            hasItems = true;
                            break;
                        }
                    }

                    if (hasItems) {
                        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                        if (ImGui::TreeNode(RandomizerCheckObjects::GetRCAreaName(rcArea).c_str())) {
                            for (auto& location : locations) {
                                auto elfound = excludedLocations.find(location);
                                if (ctx->GetItemLocation(location)->IsVisible() && elfound != excludedLocations.end()) {
                                    if (ImGui::ArrowButton(std::to_string(location).c_str(), ImGuiDir_Left)) {
                                        excludedLocations.erase(elfound);
                                        // todo: this efficently when we build out cvar array support
                                        std::string excludedLocationString = "";
                                        for (auto excludedLocationIt : excludedLocations) {
                                            excludedLocationString += std::to_string(excludedLocationIt);
                                            excludedLocationString += ",";
                                        }
                                        CVarSetString("gRandomizeExcludedLocations", excludedLocationString.c_str());
                                        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                                    }
                                    ImGui::SameLine();
                                    ImGui::Text("%s", Rando::StaticData::GetLocation(location)->GetShortName().c_str());
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
        ImGui::EndDisabled();

        static bool tricksTabOpen = false;
        if (ImGui::BeginTabItem("Tricks/Glitches")) {
            if (!tricksTabOpen) {
                tricksTabOpen = true;
                //RandomizerTricks::UpdateImGuiVisibility();
                // todo: this efficently when we build out cvar array support
                std::stringstream enabledTrickStringStream(CVarGetString("gRandomizeEnabledTricks", ""));
                std::string enabledTrickString;
                enabledTricks.clear();
                while (getline(enabledTrickStringStream, enabledTrickString, ',')) {
                    enabledTricks.insert((RandomizerTrick)std::stoi(enabledTrickString));
                }
                std::stringstream enabledGlitchStringStream(CVarGetString("gRandomizeEnabledGlitches", ""));
                std::string enabledGlitchString;
                enabledGlitches.clear();
                while (getline(enabledGlitchStringStream, enabledGlitchString, ',')) {
                    enabledGlitches.insert((RandomizerTrick)std::stoi(enabledGlitchString));
                }
            }
            
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (ImGui::BeginTable("tableRandoLogic", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::PushItemWidth(170.0);
                if (mSettings->GetOption(RSK_LOGIC_RULES).RenderImGui()) {
                    mNeedsUpdate = true;
                }
                // RANDOTODO: Implement Disalbling of Options for Vanilla Logic
                if (CVarGetInteger("gRandomizeLogicRules", RO_LOGIC_GLITCHLESS) == RO_LOGIC_GLITCHLESS) {
                    ImGui::SameLine();
                    if (mSettings->GetOption(RSK_ALL_LOCATIONS_REACHABLE).RenderImGui()) {
                        mNeedsUpdate = true;
                    }
                }
                if (CVarGetInteger("gRandomizeLogicRules", RO_LOGIC_GLITCHLESS) == RO_LOGIC_VANILLA) {
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Heads up! This will disable all rando settings except for entrance shuffle and starter items");
                }

                UIWidgets::PaddedSeparator();

                // Enable Glitch-Useful Cutscenes
                if (mSettings->GetOption(RSK_ENABLE_GLITCH_CUTSCENES).RenderImGui()) {
                    mNeedsUpdate = true;
                }
                ImGui::PopItemWidth();
                ImGui::EndTable();
            }

            ImGui::BeginDisabled(CVarGetInteger("gRandomizeLogicRules", RO_LOGIC_GLITCHLESS) == RO_LOGIC_VANILLA);

            // Tricks
            static std::unordered_map<RandomizerArea, bool> areaTreeDisabled {
                {RA_NONE, true},
                {RA_KOKIRI_FOREST, true},
                {RA_THE_LOST_WOODS, true},
                {RA_SACRED_FOREST_MEADOW, true},
                {RA_HYRULE_FIELD, true},
                {RA_LAKE_HYLIA, true},
                {RA_GERUDO_VALLEY, true},
                {RA_GERUDO_FORTRESS, true},
                {RA_HAUNTED_WASTELAND, true},
                {RA_DESERT_COLOSSUS, true},
                {RA_THE_MARKET, true},
                {RA_HYRULE_CASTLE, true},
                {RA_KAKARIKO_VILLAGE, true},
                {RA_THE_GRAVEYARD, true},
                {RA_DEATH_MOUNTAIN_TRAIL, true},
                {RA_GORON_CITY, true},
                {RA_DEATH_MOUNTAIN_CRATER, true},
                {RA_ZORAS_RIVER, true},
                {RA_ZORAS_DOMAIN, true},
                {RA_ZORAS_FOUNTAIN, true},
                {RA_LON_LON_RANCH, true},
                {RA_DEKU_TREE, true},
                {RA_DODONGOS_CAVERN, true},
                {RA_JABU_JABUS_BELLY, true},
                {RA_FOREST_TEMPLE, true},
                {RA_FIRE_TEMPLE, true},
                {RA_WATER_TEMPLE, true},
                {RA_SPIRIT_TEMPLE, true},
                {RA_SHADOW_TEMPLE, true},
                {RA_BOTTOM_OF_THE_WELL, true},
                {RA_ICE_CAVERN, true},
                {RA_GERUDO_TRAINING_GROUND, true},
                {RA_GANONS_CASTLE, true}
            };
            static std::unordered_map<RandomizerArea, bool> areaTreeEnabled {
                {RA_NONE, true},
                {RA_KOKIRI_FOREST, true},
                {RA_THE_LOST_WOODS, true},
                {RA_SACRED_FOREST_MEADOW, true},
                {RA_HYRULE_FIELD, true},
                {RA_LAKE_HYLIA, true},
                {RA_GERUDO_VALLEY, true},
                {RA_GERUDO_FORTRESS, true},
                {RA_HAUNTED_WASTELAND, true},
                {RA_DESERT_COLOSSUS, true},
                {RA_THE_MARKET, true},
                {RA_HYRULE_CASTLE, true},
                {RA_KAKARIKO_VILLAGE, true},
                {RA_THE_GRAVEYARD, true},
                {RA_DEATH_MOUNTAIN_TRAIL, true},
                {RA_GORON_CITY, true},
                {RA_DEATH_MOUNTAIN_CRATER, true},
                {RA_ZORAS_RIVER, true},
                {RA_ZORAS_DOMAIN, true},
                {RA_ZORAS_FOUNTAIN, true},
                {RA_LON_LON_RANCH, true},
                {RA_DEKU_TREE, true},
                {RA_DODONGOS_CAVERN, true},
                {RA_JABU_JABUS_BELLY, true},
                {RA_FOREST_TEMPLE, true},
                {RA_FIRE_TEMPLE, true},
                {RA_WATER_TEMPLE, true},
                {RA_SPIRIT_TEMPLE, true},
                {RA_SHADOW_TEMPLE, true},
                {RA_BOTTOM_OF_THE_WELL, true},
                {RA_ICE_CAVERN, true},
                {RA_GERUDO_TRAINING_GROUND, true},
                {RA_GANONS_CASTLE, true}
            };

            static std::unordered_map<Rando::Tricks::Tag, bool> showTag {
                {Rando::Tricks::Tag::NOVICE,true},
                {Rando::Tricks::Tag::INTERMEDIATE,true},
                {Rando::Tricks::Tag::ADVANCED,true},
                {Rando::Tricks::Tag::EXPERT,true},
                {Rando::Tricks::Tag::EXTREME,true}
            };
            static ImGuiTextFilter trickSearch;
            trickSearch.Draw("Filter (inc,-exc)", 490.0f);
            if (CVarGetInteger("gRandomizeLogicRules", RO_LOGIC_GLITCHLESS) != RO_LOGIC_NO_LOGIC) {
                ImGui::SameLine();
                if (ImGui::Button("Disable All")) {
                    for (int i = 0; i < RT_MAX; i++) {
                        auto etfound = enabledTricks.find(static_cast<RandomizerTrick>(i));
                        if (!ctx->GetTrickOption(static_cast<RandomizerTrick>(i)).IsGlitch() && etfound != enabledTricks.end()) {
                            enabledTricks.erase(etfound);
                        }
                    }
                    std::string enabledTrickString = "";
                    for (auto enabledTrickIt : enabledTricks) {
                        enabledTrickString += std::to_string(enabledTrickIt);
                        enabledTrickString += ",";
                    }
                    CVarSetString("gRandomizeEnabledTricks", enabledTrickString.c_str());
                    LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                }
                ImGui::SameLine();
                if (ImGui::Button("Enable All")) {
                    for (int i = 0; i < RT_MAX; i++) {
                        if (!ctx->GetTrickOption(static_cast<RandomizerTrick>(i)).IsGlitch() && !enabledTricks.count(static_cast<RandomizerTrick>(i))) {
                            enabledTricks.insert(static_cast<RandomizerTrick>(i));
                        }
                    }
                    std::string enabledTrickString = "";
                    for (auto enabledTrickIt : enabledTricks) {
                        enabledTrickString += std::to_string(enabledTrickIt);
                        enabledTrickString += ",";
                    }
                    CVarSetString("gRandomizeEnabledTricks", enabledTrickString.c_str());
                    LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                }
            }
            if (ImGui::BeginTable("trickTags", showTag.size(), ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders)) {  
                for (auto [rtTag, isShown] : showTag) {
                    ImGui::TableNextColumn();
                    ImGui::PushStyleColor(ImGuiCol_Header, Rando::Tricks::GetRTTagColor(rtTag));
                    ImGui::Selectable(Rando::Tricks::GetRTTagName(rtTag).c_str(), &showTag[rtTag]);
                    ImGui::PopStyleColor(1);
                }
                ImGui::EndTable();
            }

            if (ImGui::BeginTable("tableRandoTricks", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Disabled Tricks", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Enabled Tricks", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::TableHeadersRow();
                ImGui::PopItemFlag();
                ImGui::TableNextRow();
                
                if (CVarGetInteger("gRandomizeLogicRules", RO_LOGIC_GLITCHLESS) != RO_LOGIC_NO_LOGIC) {

                    // COLUMN 1 - DISABLED TRICKS
                    ImGui::TableNextColumn();
                    window->DC.CurrLineTextBaseOffset = 0.0f;
                    
                    if (ImGui::Button("Collapse All##disabled")) {
                        for (int i = 0; i < RT_MAX; i++) {
                            auto option = mSettings->GetTrickOption(static_cast<RandomizerTrick>(i));
                            if (!option.IsHidden() && !enabledTricks.count(static_cast<RandomizerTrick>(i)) &&
                                !option.IsGlitch()) {
                                areaTreeDisabled[option.GetArea()] = false;
                            }
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Open All##disabled")) {
                        for (int i = 0; i < RT_MAX; i++) {
                            auto option = mSettings->GetTrickOption(static_cast<RandomizerTrick>(i));
                            if (option.IsHidden() && !enabledTricks.count(static_cast<RandomizerTrick>(i)) &&
                                !option.IsGlitch()) {
                                areaTreeDisabled[option.GetArea()] = false;
                            }
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Enable Visible")) {
                        for (int i = 0; i < RT_MAX; i++) {
                            auto option = mSettings->GetTrickOption(static_cast<RandomizerTrick>(i));
                            if (!option.IsGlitch() && !enabledTricks.count(static_cast<RandomizerTrick>(i)) &&
                                trickSearch.PassFilter(option.GetName().c_str()) &&
                                areaTreeDisabled[option.GetArea()] &&
                                Rando::Tricks::CheckRTTags(showTag, option.GetTags())) {
                                enabledTricks.insert(static_cast<RandomizerTrick>(i));
                            }
                        }
                        std::string enabledTrickString = "";
                        for (auto enabledTrickIt : enabledTricks) {
                            enabledTrickString += std::to_string(enabledTrickIt);
                            enabledTrickString += ",";
                        }
                        CVarSetString("gRandomizeEnabledTricks", enabledTrickString.c_str());
                        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                    }
                    
                    ImGui::BeginChild("ChildTricksDisabled", ImVec2(0, -8), false, ImGuiWindowFlags_HorizontalScrollbar);

                    for (auto [area, trickIds] : mSettings->mTricksByArea) {
                        bool hasTricks = false;
                        for (auto rt : trickIds) {
                            auto option = mSettings->GetTrickOption(rt);
                            if (!option.IsHidden() && trickSearch.PassFilter(option.GetName().c_str()) &&
                                !enabledTricks.count(rt) && Rando::Tricks::CheckRTTags(showTag, option.GetTags()) &&
                                !option.IsGlitch()) {
                                hasTricks = true;
                                break;
                            }
                        }
                        if (hasTricks) {
                            ImGui::TreeNodeSetOpen(ImGui::GetID(Rando::Tricks::GetRTAreaName(area).c_str()), areaTreeDisabled[area]);
                            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                            if (ImGui::TreeNode(Rando::Tricks::GetRTAreaName(area).c_str())) {
                                for (auto rt : trickIds) {
                                    auto option = mSettings->GetTrickOption(rt);
                                    if (!option.IsHidden() && trickSearch.PassFilter(option.GetName().c_str()) &&
                                        !enabledTricks.count(rt) && Rando::Tricks::CheckRTTags(showTag, option.GetTags()) &&
                                        !option.IsGlitch()) {
                                        ImGui::TreeNodeSetOpen(ImGui::GetID(Rando::Tricks::GetRTAreaName(option.GetArea()).c_str()), areaTreeDisabled[option.GetArea()]);
                                        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                                        if (ImGui::ArrowButton(std::to_string(rt).c_str(), ImGuiDir_Right)) {
                                            enabledTricks.insert(rt);
                                            std::string enabledTrickString = "";
                                            for (auto enabledTrickIt : enabledTricks) {
                                                enabledTrickString += std::to_string(enabledTrickIt);
                                                enabledTrickString += ",";
                                            }
                                            CVarSetString("gRandomizeEnabledTricks", enabledTrickString.c_str());
                                            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                                        }
                                        Rando::Tricks::DrawTagChips(option.GetTags());
                                        ImGui::SameLine();
                                        ImGui::Text("%s", option.GetName().c_str());
                                        UIWidgets::InsertHelpHoverText(option.GetDescription().c_str());
                                    }
                                }
                                areaTreeDisabled[area] = true;
                                ImGui::TreePop();
                            } else {
                                areaTreeDisabled[area] = false;
                            }
                        }
                    }
                    ImGui::EndChild();

                    

                    // COLUMN 2 - ENABLED TRICKS
                    ImGui::TableNextColumn();
                    window->DC.CurrLineTextBaseOffset = 0.0f;

                    if (ImGui::Button("Collapse All##enabled")) {
                        for (int i = 0; i < RT_MAX; i++) {
                            auto option = mSettings->GetTrickOption(static_cast<RandomizerTrick>(i));
                            if (!option.IsHidden() && enabledTricks.count(static_cast<RandomizerTrick>(i)) &&
                                !option.IsGlitch()) {
                                areaTreeDisabled[option.GetArea()] = false;
                            }
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Open All##enabled")) {
                        for (int i = 0; i < RT_MAX; i++) {
                            auto option = mSettings->GetTrickOption(static_cast<RandomizerTrick>(i));
                            if (option.IsHidden() && enabledTricks.count(static_cast<RandomizerTrick>(i)) &&
                                !option.IsGlitch()) {
                                areaTreeDisabled[option.GetArea()] = false;
                            }
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Disable Visible")) {
                        for (int i = 0; i < RT_MAX; i++) {
                            auto option = mSettings->GetTrickOption(static_cast<RandomizerTrick>(i));
                            if (!option.IsGlitch() && enabledTricks.count(static_cast<RandomizerTrick>(i)) &&
                                trickSearch.PassFilter(option.GetName().c_str()) &&
                                areaTreeEnabled[option.GetArea()] &&
                                Rando::Tricks::CheckRTTags(showTag, option.GetTags())) {
                                enabledTricks.insert(static_cast<RandomizerTrick>(i));
                            }
                        }
                        std::string enabledTrickString = "";
                        for (auto enabledTrickIt : enabledTricks) {
                            enabledTrickString += std::to_string(enabledTrickIt);
                            enabledTrickString += ",";
                        }
                        CVarSetString("gRandomizeEnabledTricks", enabledTrickString.c_str());
                        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                    }
                    
                    ImGui::BeginChild("ChildTricksEnabled", ImVec2(0, -8), false, ImGuiWindowFlags_HorizontalScrollbar);

                    for (auto [area, trickIds] : mSettings->mTricksByArea) {
                        bool hasTricks = false;
                        for (auto rt : trickIds) {
                            auto option = mSettings->GetTrickOption(rt);
                            if (!option.IsHidden() && trickSearch.PassFilter(option.GetName().c_str()) &&
                                enabledTricks.count(rt) && Rando::Tricks::CheckRTTags(showTag, option.GetTags()) &&
                                !option.IsGlitch()) {
                                hasTricks = true;
                                break;
                            }
                        }
                        if (hasTricks) {
                            ImGui::TreeNodeSetOpen(ImGui::GetID(Rando::Tricks::GetRTAreaName(area).c_str()), areaTreeDisabled[area]);
                            ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                            if (ImGui::TreeNode(Rando::Tricks::GetRTAreaName(area).c_str())) {
                                for (auto rt : trickIds) {
                                    auto option = mSettings->GetTrickOption(rt);
                                    if (!option.IsHidden() && trickSearch.PassFilter(option.GetName().c_str()) &&
                                        enabledTricks.count(rt) && Rando::Tricks::CheckRTTags(showTag, option.GetTags()) &&
                                        !option.IsGlitch()) {
                                        ImGui::TreeNodeSetOpen(ImGui::GetID(Rando::Tricks::GetRTAreaName(option.GetArea()).c_str()), areaTreeDisabled[option.GetArea()]);
                                        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                                        if (ImGui::ArrowButton(std::to_string(rt).c_str(), ImGuiDir_Left)) {
                                            enabledTricks.insert(rt);
                                            std::string enabledTrickString = "";
                                            for (auto enabledTrickIt : enabledTricks) {
                                                enabledTrickString += std::to_string(enabledTrickIt);
                                                enabledTrickString += ",";
                                        }
                                        CVarSetString("gRandomizeEnabledTricks", enabledTrickString.c_str());
                                        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                                    }
                                    Rando::Tricks::DrawTagChips(option.GetTags());
                                    ImGui::SameLine();
                                    ImGui::Text("%s", option.GetName().c_str());
                                    UIWidgets::InsertHelpHoverText(option.GetDescription().c_str());
                                    }
                                }
                                areaTreeEnabled[area] = true;
                                ImGui::TreePop();
                            } else {
                                areaTreeEnabled[area] = false;
                            }
                        }
                    }

                    ImGui::EndChild();
                } else {
                    ImGui::TableNextColumn();
                    ImGui::BeginChild("ChildTrickAreas", ImVec2(0, -8));
                    ImGui::Text("Requires Logic Turned On.");
                    ImGui::EndChild();
                    ImGui::TableNextColumn();
                    ImGui::BeginChild("ChildTricksDisabled", ImVec2(0, -8));
                    ImGui::Text("Requires Logic Turned On.");
                    ImGui::EndChild();
                    ImGui::TableNextColumn();
                    ImGui::BeginChild("ChildTricksEnabled", ImVec2(0, -8));
                    ImGui::Text("Requires Logic Turned On.");
                    ImGui::EndChild();
                }
                ImGui::EndTable();
            }
            ImGui::EndDisabled();
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        } else {
            tricksTabOpen = false;
        }

        if (ImGui::BeginTabItem("Starting Inventory")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (mSettings->GetOptionGroup(RSG_STARTING_INVENTORY_IMGUI_TABLE).RenderImGui()) {
                mNeedsUpdate = true;
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::EndDisabled();

    if (disableEditingRandoSettings) {
        UIWidgets::ReEnableComponent("");
    }
    ImGui::End();
}

void RandomizerSettingsWindow::UpdateElement() {
    if (mNeedsUpdate) {
        mSettings->UpdateOptionProperties();
    }
}

CustomMessage Randomizer::ReplaceWithItemName(CustomMessage message, std::string&& toReplace, RandomizerCheck hintedCheck){
    auto ctx = Rando::Context::GetInstance();
    RandomizerGet targetRG = ctx->GetItemLocation(hintedCheck)->GetPlacedRandomizerGet();
    std::array<std::string, LANGUAGE_MAX> itemName;
        if (targetRG == RG_ICE_TRAP) {
            targetRG = ctx->overrides[hintedCheck].LooksLike();
            itemName = {
                ctx->overrides[hintedCheck].GetTrickName().english,
                ctx->overrides[hintedCheck].GetTrickName().french,
                ctx->overrides[hintedCheck].GetTrickName().english
            };
        } else {
            itemName = EnumToSpoilerfileGetName[targetRG];
        }
    message.Replace(std::move(toReplace), std::move(itemName[0]), std::move(itemName[1]), std::move(itemName[2]));
    return message;
}


CustomMessage Randomizer::GetMiscHintMessage(TextIDs textToGet, RandomizerCheck hintedCheck, RandomizerCheck otherCheck) {
    CustomMessage messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::randoMiscHintsTableID, textToGet);
    messageEntry = ReplaceWithItemName(messageEntry, "{{item1}}", hintedCheck);
    if (otherCheck != RC_UNKNOWN_CHECK){
        messageEntry = ReplaceWithItemName(messageEntry, "{{item2}}", otherCheck);
    }
    return messageEntry;
}

CustomMessage Randomizer::GetCursedSkullMessage(s16 params, RandomizerCheck hintedCheck) {
    auto ctx = Rando::Context::GetInstance();
    CustomMessage messageEntry = GetMiscHintMessage(TEXT_SKULLTULA_PEOPLE_IM_CURSED, hintedCheck);
    messageEntry.Replace("{{params}}", std::to_string(params*10));
    return messageEntry;
}

CustomMessage Randomizer::GetSheikMessage(s16 scene, u16 originalTextId) {
    auto ctx = Rando::Context::GetInstance();
    CustomMessage messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::hintMessageTableID, originalTextId);
    switch (scene) {
        case SCENE_TEMPLE_OF_TIME:
            if (originalTextId == TEXT_SHEIK_NEED_HOOK) {
                messageEntry.Replace("{{message}}", 
                "@,&meet me at %gGanon's Castle%w&once you obtain the %rkey to his lair%w.",
                "@, wir treffen uns bei %gGanons Schloß%w,&sobald Du den %rSchlüssel zu&seinem Verließ%w hast.",
                "Retrouve-moi au %gChâteau de Ganon%w une&fois que tu auras obtenu la&Mrclé de son repaire%w.");
            } else {
                messageEntry.Replace("{{message}}",
                "The time has come. Prepare yourself.",
                "Die Zeit ist gekommen.&Mach Dich bereit.",
                "Le moment est venu @.&Tu ferais bien de te préparer.");
            }
            break;
        case SCENE_INSIDE_GANONS_CASTLE:
            if (originalTextId == TEXT_SHEIK_NEED_HOOK) {
                //If MS shuffle is on, Sheik will hint both MS and LA as long as Link doesn't have both, to prevent hint lockout.
                //Otherwise, she'll only give LA hint so only LA is required to move on.
                bool needRequirements = GetRandoSettingValue(RSK_SHUFFLE_MASTER_SWORD) ? 
                  (!CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER) || INV_CONTENT(ITEM_ARROW_LIGHT) != ITEM_ARROW_LIGHT) :
                  (INV_CONTENT(ITEM_ARROW_LIGHT) != ITEM_ARROW_LIGHT);
                if (needRequirements) {
                    messageEntry.Replace("{{message}}", ctx->GetHint(RH_SHEIK_LIGHT_ARROWS)->GetText().GetEnglish().c_str(), ctx->GetHint(RH_SHEIK_LIGHT_ARROWS)->GetText().GetEnglish().c_str(), ctx->GetHint(RH_SHEIK_LIGHT_ARROWS)->GetText().GetFrench().c_str());
                } else {
                    messageEntry.Replace("{{message}}", "You are still ill-equipped to&face %rGanondorf%w."
                    "^Seek out the %cMaster Sword%w,&%rsomething to hold your arrows%w,&and %gmagic%w to summon the %ylight%w.",
                    "Du bist noch nicht gewappnet um Dich&%rGanondorf%w stellen zu können.^"
                    "Begib Dich auf die Suche nach dem&%cMaster-Schwert%w, %retwas um deine Pfeilen&einen Sinn zu geben%w,^sowie %gdie Magie%w, um das %yLicht%w&herauf beschwören zu können.",
                    "@, tu n'es toujours pas prêt à affronter&%rGanondorf%w.^"
                    "Cherche l'%cÉpée de Légende%w,&%rquelque chose pour ranger tes flèches%w&et de la %gmagie%w pour invoquer la&%ylumière%w.");
                }                   
            } else {
                if (!Flags_GetEventChkInf(EVENTCHKINF_DISPELLED_GANONS_TOWER_BARRIER)) {
                    messageEntry.Replace("{{message}}",
                    "You may have what you need to defeat&%rthe Evil King%w, but the %cbarrier%w still&stands.^Complete the remaining %gtrials%w&to destroy it."
                    );

                } else {
                    messageEntry.Replace("{{message}}",
                    "If you're ready, then proceed.^Good luck.",
                    "Wenn Du bereit bist, so schreite&voran.^Viel Glück.",
                    "Si tu es prêt, tu peux y aller.^Bonne chance.");
                }
            }
            break;
    }
        return messageEntry;
}

CustomMessage Randomizer::GetSariaMessage(u16 originalTextId) {
    if (originalTextId == TEXT_SARIA_SFM || (originalTextId == TEXT_SARIAS_SONG_FOREST_SOUNDS && originalTextId == TEXT_SARIAS_SONG_FOREST_TEMPLE)) {
        CustomMessage messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::hintMessageTableID, TEXT_SARIAS_SONG_FACE_TO_FACE);
        CustomMessage messageEntry2 = messageEntry;
        std::string code = originalTextId == TEXT_SARIA_SFM ? "" : "\x0B";
        messageEntry2.Replace("$C", std::move(code));
        return messageEntry2;
    }
}

CustomMessage Randomizer::GetFishingPondOwnerMessage(u16 originalTextId) {
    CustomMessage hintMessageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::randoMiscHintsTableID, TEXT_FISHING_POND_START);
    CustomMessage messageEntry = CustomMessage(
      "Sorry, but the pond is closed.&I've lost my good %rfishing pole%w...&Can't go fishing without it!",
      "",
      ""
    );

    if (Rando::Context::GetInstance()->GetOption(RSK_FISHING_POLE_HINT)) {
        messageEntry = messageEntry + hintMessageEntry;
    }

    // if the fishing pond guy doesnt remember me i will cry :(
    if (originalTextId == TEXT_FISHING_POND_START_MET) {
        messageEntry = CustomMessage(
            "Hey, mister! I remember you!&It's been a long time!^",
            "",
            ""
        ) + messageEntry;
    }

    messageEntry.Format();

    return messageEntry;
}

CustomMessage Randomizer::GetMerchantMessage(RandomizerInf randomizerInf, u16 textId, bool mysterious) {
    auto ctx = Rando::Context::GetInstance();
    CustomMessage messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::merchantMessageTableID, textId);
    RandomizerCheck rc = GetCheckFromRandomizerInf(randomizerInf);
    RandomizerGet shopItemGet = ctx->GetItemLocation(rc)->GetPlacedRandomizerGet();
    std::array<std::string, LANGUAGE_MAX> shopItemName;
    if (mysterious) {
        shopItemName = {
            "mysterious item",
            "mysteriösen Gegenstand",
            "objet mystérieux"
        };
    // TODO: This should eventually be replaced with a full fledged trick model & trick name system
    } else if (shopItemGet == RG_ICE_TRAP) {
        shopItemGet = ctx->overrides[rc].LooksLike();
        shopItemName = {
            std::string(ctx->overrides[rc].GetTrickName().english),
            std::string(ctx->overrides[rc].GetTrickName().french),
            std::string(ctx->overrides[rc].GetTrickName().english)
        };
    } else { 
        shopItemName = EnumToSpoilerfileGetName[shopItemGet];
    }
    u16 shopItemPrice = ctx->GetItemLocation(rc)->GetPrice();

    if (textId == TEXT_SCRUB_RANDOM && shopItemPrice == 0) {
        messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::merchantMessageTableID, TEXT_SCRUB_RANDOM_FREE);
    } else if (textId == TEXT_GRANNYS_SHOP) {
        // Capitalize the first letter for the item in Granny's text as the item is the first word presented
        for (auto &itemName : shopItemName) {
            itemName[0] = std::toupper(itemName[0]);
        }
    }

    messageEntry.Replace("{{item}}", std::move(shopItemName[0]), std::move(shopItemName[1]), std::move(shopItemName[2]));
    messageEntry.Replace("{{price}}", std::to_string(shopItemPrice));
    return messageEntry;
}

static const char* mapGetItemHints[3][2] = {
    { " It's ordinary.", " It's masterful!" },
    { "&Sieht aus wie immer.", " &Man kann darauf die Worte&%r\"Master Quest\"%w entziffern..." },
    { "&Elle vous semble %rordinaire%w.", "&Étrange... les mots %r\"Master&Quest\"%w sont gravés dessus." },
};

CustomMessage Randomizer::GetMapGetItemMessageWithHint(GetItemEntry itemEntry) {
    CustomMessage messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::getItemMessageTableID, itemEntry.getItemId);
    int sceneNum;
    switch (itemEntry.getItemId) {
        case RG_DEKU_TREE_MAP:
            sceneNum = SCENE_DEKU_TREE;
            break;
        case RG_DODONGOS_CAVERN_MAP:
            sceneNum = SCENE_DODONGOS_CAVERN;
            break;
        case RG_JABU_JABUS_BELLY_MAP:
            sceneNum = SCENE_JABU_JABU;
            break;
        case RG_FOREST_TEMPLE_MAP:
            sceneNum = SCENE_FOREST_TEMPLE;
            break;
        case RG_FIRE_TEMPLE_MAP:
            sceneNum = SCENE_FIRE_TEMPLE;
            break;
        case RG_WATER_TEMPLE_MAP:
            sceneNum = SCENE_WATER_TEMPLE;
            break;
        case RG_SPIRIT_TEMPLE_MAP:
            sceneNum = SCENE_SPIRIT_TEMPLE;
            break;
        case RG_SHADOW_TEMPLE_MAP:
            sceneNum = SCENE_SHADOW_TEMPLE;
            break;
        case RG_BOTTOM_OF_THE_WELL_MAP:
            sceneNum = SCENE_BOTTOM_OF_THE_WELL;
            break;
        case RG_ICE_CAVERN_MAP:
            sceneNum = SCENE_ICE_CAVERN;
            break;
    }

    if (GetRandoSettingValue(RSK_MQ_DUNGEON_RANDOM) == RO_MQ_DUNGEONS_NONE ||
        (GetRandoSettingValue(RSK_MQ_DUNGEON_RANDOM) == RO_MQ_DUNGEONS_SET_NUMBER &&
         GetRandoSettingValue(RSK_MQ_DUNGEON_COUNT) == 12)
       ) {
        messageEntry.Replace("{{typeHint}}", "");
    } else if (ResourceMgr_IsSceneMasterQuest(sceneNum)) {
        messageEntry.Replace("{{typeHint}}", mapGetItemHints[0][1], mapGetItemHints[1][1], mapGetItemHints[2][1]);
    } else {
        messageEntry.Replace("{{typeHint}}", mapGetItemHints[0][0], mapGetItemHints[1][0], mapGetItemHints[2][0]);
    }

    return messageEntry;
}

template<size_t N>
void CreateGetItemMessages(const std::array<GetItemMessage, N>* messageEntries) {
    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(Randomizer::getItemMessageTableID);
    for (const GetItemMessage& messageEntry : *messageEntries) {
        customMessageManager->CreateGetItemMessage(
            Randomizer::getItemMessageTableID, messageEntry.giid, messageEntry.iid,
            CustomMessage(messageEntry.english, messageEntry.german, messageEntry.french, TEXTBOX_TYPE_BLUE,
                          TEXTBOX_POS_BOTTOM));
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
        customMessageManager->CreateMessage(
            Randomizer::rupeeMessageTableID, rupee,
            CustomMessage("You found" + rupeeText + " !", "Du hast" + rupeeText + "  gefunden!",
                          "Vous obtenez" + rupeeText + " !", TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM));
    }
}

CustomMessage Randomizer::GetRupeeMessage(u16 rupeeTextId) {
    CustomMessage messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::rupeeMessageTableID, rupeeTextId);
    messageEntry.Replace("{{rupee}}", RandomElement(englishRupeeNames),
                                                 RandomElement(germanRupeeNames), RandomElement(frenchRupeeNames));
    return messageEntry;
}

void CreateTriforcePieceMessages() {
    CustomMessage TriforcePieceMessages[NUM_TRIFORCE_PIECE_MESSAGES] = {

        { "You found a %yTriforce Piece%w!&%g{{current}}%w down, %c{{remaining}}%w to go. It's a start!",
          "Ein %yTriforce-Splitter%w! Du hast&%g{{current}}%w von %c{{required}}%w gefunden. Es ist ein&Anfang!",
          "Vous trouvez un %yFragment de la&Triforce%w! Vous en avez %g{{current}}%w, il en&reste %c{{remaining}}%w à trouver. C'est un début!" },

        { "You found a %yTriforce Piece%w!&%g{{current}}%w down, %c{{remaining}}%w to go. Progress!",
          "Ein %yTriforce-Splitter%w! Du hast&%g{{current}}%w von %c{{required}}%w gefunden. Es geht voran!",
          "Vous trouvez un %yFragment de la&Triforce%w! Vous en avez %g{{current}}%w, il en&reste %c{{remaining}}%w à trouver. Ça avance!" },

        { "You found a %yTriforce Piece%w!&%g{{current}}%w down, %c{{remaining}}%w to go. Over half-way&there!",
          "Ein %yTriforce-Splitter%w! Du hast&schon %g{{current}}%w von %c{{required}}%w gefunden. Schon&über die Hälfte!",
          "Vous trouvez un %yFragment de la&Triforce%w! Vous en avez %g{{current}}%w, il en&reste %c{{remaining}}%w à trouver. Il en reste un&peu moins que la moitié!" },

        { "You found a %yTriforce Piece%w!&%g{{current}}%w down, %c{{remaining}}%w to go. Almost done!",
          "Ein %yTriforce-Splitter%w! Du hast&schon %g{{current}}%w von %c{{required}}%w gefunden. Fast&geschafft!",
          "Vous trouvez un %yFragment de la&Triforce%w! Vous en avez %g{{current}}%w, il en&reste %c{{remaining}}%w à trouver. C'est presque&terminé!" },

        { "You completed the %yTriforce of&Courage%w! %gGG%w!",
          "Das %yTriforce des Mutes%w! Du hast&alle Splitter gefunden. %gGut gemacht%w!",
          "Vous avez complété la %yTriforce&du Courage%w! %gFélicitations%w!" },

        { "You found a spare %yTriforce Piece%w!&You only needed %c{{required}}%w, but you have %g{{current}}%w!",
          "Ein übriger %yTriforce-Splitter%w! Du&hast nun %g{{current}}%w von %c{{required}}%w nötigen gefunden.",
          "Vous avez trouvé un %yFragment de&Triforce%w en plus! Vous n'aviez besoin&que de %c{{required}}%w, mais vous en avez %g{{current}}%w en&tout!" },
    };
    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(Randomizer::triforcePieceMessageTableID);
    for (unsigned int i = 0; i <= (NUM_TRIFORCE_PIECE_MESSAGES - 1); i++) {
        customMessageManager->CreateMessage(Randomizer::triforcePieceMessageTableID, i, TriforcePieceMessages[i]);
    }
}

CustomMessage Randomizer::GetTriforcePieceMessage() {
    // Item is only given after the textbox, so reflect that inside the textbox.
    uint8_t current = gSaveContext.triforcePiecesCollected + 1;
    uint8_t required = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_TRIFORCE_HUNT_PIECES_REQUIRED) + 1;
    uint8_t remaining = required - current;
    float percentageCollected = (float)current / (float)required;
    uint8_t messageIndex;

    if (percentageCollected <= 0.25) {
        messageIndex = TH_MESSAGE_START;
    } else if (percentageCollected <= 0.5) {
        messageIndex = TH_MESSAGE_PROGRESS;
    } else if (percentageCollected <= 0.75) {
        messageIndex = TH_MESSAGE_HALFWAY;
    } else if (percentageCollected < 1) {
        messageIndex = TH_MESSAGE_ALMOSTDONE;
    } else if (current == required) {
        messageIndex = TH_MESSAGE_FINISHED;
    } else {
        messageIndex = TH_MESSAGE_SURPLUS;
    }

    CustomMessage messageEntry =
        CustomMessageManager::Instance->RetrieveMessage(Randomizer::triforcePieceMessageTableID, messageIndex);
    messageEntry.Replace("{{current}}", std::to_string(current), std::to_string(current), std::to_string(current));
    messageEntry.Replace("{{remaining}}", std::to_string(remaining), std::to_string(remaining), std::to_string(remaining));
    messageEntry.Replace("{{required}}", std::to_string(required), std::to_string(required), std::to_string(required));
    return messageEntry;
}

void CreateNaviRandoMessages() {
    CustomMessage NaviMessages[NUM_NAVI_MESSAGES] = {

        { "%cMissing a small key in a dungeon?&Maybe the %rboss %chas it!",
          "%cFehlt dir ein kleiner Schlüssel in &einem Labyrinth? Vielleicht hat ihn&ja der %rEndgegner%c!",
          "%cIl te manque une %wPetite Clé %cdans&un donjon? C'est peut-être le %rboss&%cqui l'a!" },

        { "%cSometimes you can use the %rMegaton&Hammer %cinstead of bombs!",
          "%cManchmal kannst du den %rStahlhammer&%cstatt Bomben verwenden!",
          "%cParfois, tu peux utiliser la %rMasse&des Titans %cau lieu de tes bombes!" },

        { "%cThere are three %gbusiness scrubs %cin &Hyrule who sell %wmysterious items%c. Do&you know where they are?",
          "%cEs gibt drei %gDeku-Händler %cin Hyrule&die mysteriöse Gegenstände&verkaufen. Weißt du wo Sie sind?",
          "%cIl y a trois %gPestes Marchandes%c en&Hyrule qui vendent des %wobjets&mystérieux%c. Tu sais où elles "
          "sont?" },

        { "%cStuck on this seed? You could &throw in the towel and check the&%wspoiler log%c...",
          "%cHängst du bei diesem Seed fest?&Du könntest die Flinte ins Korn&werfen und ins %wSpoiler Log %cschauen...",
          "%cSi tu es coincé sur cette seed,&tu peux toujours jeter l'éponge&et regader le %wSpoiler log%c..." },

        { "%cDid you know that the %yHover&Boots %ccan be used to cross&%wquicksand%c?",
          "%cWußtest du, daß du mit den&%yGleitstiefeln %cTreibsand %wüberqueren&kannst%c?",
          "%cEst-ce que tu savais que les %rBottes&des airs %cpouvaient être utilisées&pour traverser les %wsables "
          "mouvants%c?" },

        { "%cYou can reach the back of %wImpa's&House %cby jumping from the&unfinished house with a %rcucco%c!",
          "%cDu kannst den Balkon von %wImpas&Haus %cerreichen indem du von&der Baustelle aus mit einem "
          "%rHuhn&%cspringst!",
          "%cTu peux atteindre l'arrière de la&%wMaison d'Impa %cen sautant depuis la&maison en construction avec "
          "une&%rcocotte%c!" },

        { "%cThe %ySpirit Trial %cin %pGanon's Castle&%chas a %whidden chest %csomewhere.&Did you already know that?",
          "%cDie %yGeister-Prüfung %cin %pGanons&Schloß %chat irgendwo eine&%wversteckte Kiste%c. Weißt du schon&wo?",
          "%cL'%yÉpreuve de l'Esprit%c dans le %pChâteau&de Ganon %ca un coffre caché quelque&part. Je suppose que tu "
          "le savais&déjà?" },

        { "%cYou know the %wBombchu Bowling&Alley%c? I heard %wonly two of the &prizes %care worthwhile. The rest &is "
          "junk!",
          "%cKennst du die %wMinenbowlingbahn%c?&Ich habe gehört daß sich nur &%wzwei der Preise%c lohnen. Der "
          "Rest&ist Krimskrams!",
          "%cEst-ce que tu savais qu'au %wBowling&Teigneux%c, il n'y a que les %wdeux&premiers prix %cqui sont "
          "intéréssant?" },

        { "%cHave you been using %wDeku Nuts&%cenough? I've seen them blow up&a %rBombchu%c!",
          "%cBenutzt du auch genügend %wDeku&Nüsse%c? Ich habe mal gesehen daß&man damit %rKrabbelminen "
          "%cdetonieren&kann!",
          "%cTu es sûr d'utiliser tes %wNoix Mojo &%ccorrectement? J'en ai déjà vu&exploser des %rChoux-Péteurs%c!" },

        { "%cYou might end up with an %wextra&key %cfor the %bWater Temple %cor the&%rFire Temple%c. It's for your "
          "safety!",
          "%cVielleicht verbleibt dir im&%bWassertempel %coder %rFeuertempel %cein&%wzusätzlicher Schlüssel%c. "
          "Dies&ist zu deiner Sicherheit!",
          "%cIl se peut que tu aies une %wPetite&Clé %cen trop dans le %bTemple de l'Eau&%cou le %rTemple du Feu%c. "
          "C'est pour ta&propre sécurité!" },

        { "%cIf you can't pick up a %rbomb&flower %cwith your hands, you can&still detonate it with %rfire %cor&with "
          "%warrows%c!",
          "%cNur weil du eine %rDonnerblume&%cnicht hochheben kannst, so kannst&du sie immernoch mit %rFeuer "
          "%coder&%wPfeilen %cdetonieren!",
          "%cSi tu ne peux pas ramasser&un %rChoux-Péteur %cavec tes mains, tu&peux toujours le faire exploser&avec du "
          "%rFeu %cou avec des %wflèches%c!" },

        { "%cEven an adult can't push large&blocks without some %wextra&strength%c!",
          "%cSelbst ein Erwachsener kann ohne&etwas %wzusätzliche Kraft %ckeine&großen Blöcke verschieben!",
          "%cMême un adulte ne peut pas pousser&de grands blocs sans un petit %wgain&de force%c!" },

        { "%cI've heard that %rFlare Dancer&%cis weak to the %wMaster Sword%c!&Have you tried it?",
          "%cIch habe gehört daß der&%rFlammenderwisch %ceine Schwäche für&das %wMasterschwert %caufweist. Hast du&es "
          "schonmal versucht einzusetzen?",
          "%cJ'ai entendu dire que les %rDanse-&Flammes %csont faîbles face à l'%wÉpée de&Légende%c! Est-ce que tu as "
          "essayé?" },

        { "%cDon't have a weapon to kill a&%rspider%c? Try throwing a %wpot&%cat it!",
          "%cFehlt dir die Waffe um gegen&eine %rSkulltula %czu kämpfen? Versuch&Sie mit einem %wKrug %cabzuwerfen!",
          "%cSi tu n'as pas d'arme pour tuer&une %raraignée%c, pourquoi n'essayerais&-tu pas de lui jetter une "
          "%wjarre&%cà la figure?" },

        { "%cI hear the patch of %wsoft soil&%cin %bZora's River %cis the only one&that isn't home to a %rspider%c!",
          "%cIch habe gehört daß die Stelle&%wfeuchten Bodens %cim %bZora-Fluß %cals&einzige keine %rSkulltula "
          "%cbeherbergt.",
          "%cJ'ai entendu dire que la %wterre meuble&%cqui se trouve à la %bRivière Zora %cest&la seule qui ne "
          "contienne pas&d'%raraignée%c." },

        { "%cThe people of Hyrule sometimes&have %witems %cfor you, but they won't&like it if you're %wwearing a "
          "mask%c!",
          "%cDie Bewohner Hyrules haben manchmal&%wGegenstände %cfür dich, aber Sie mögen&es nicht wenn du %wMasken "
          "trägst%c!",
          "%cIl se peut que les habitants d'Hyrule&aient des %wobjets %cpour toi. Par contre,&ils risquent de ne pas "
          "trop apprécier&le fait que tu %wportes un masque%c!" },

        { "%cIf you get trapped somewhere, you&might have to %wsave your game %cand&%wreset%c!",
          "%cSolltest du irgendwo eingeschloßen&sein, mußt du vielleicht dein %wSpiel&speichern %cund %wneustarten%c!",
          "%cSi tu es coincé quelque part, tu&devrais %wsauvegarder ta partie %cet&faire un %wreset%c!" },

        { "%cSheik will meet you in a %rburning&village %conce you have %gForest%c,&%rFire%c, and %bWater "
          "%cMedallions!",
          "%cShiek wird dich in einem %rbrennenden&Dorf %ctreffen sobald du das Amulett&des %gWaldes%c, %rFeuers %cund "
          "%bWassers&%cbesitzt.",
          "%cSheik t'attendra dans un %rvillage&en feu %clorsque tu auras récupéré&les médaillons de la %gForêt%c, du "
          "%rFeu&%cet de l'%bEau%c!" },

        { "%cIf you don't have a %wsword %cas a&child, try buying %wDeku Sticks%c!&They're effective against your "
          "foes!",
          "%cSolltest du als Kind kein %wSchwert&%cbesitzen, empfehle ich %wDeku Stäbe&%czu kaufen! Diese sind "
          "effektiv gegen&Widersacher!",
          "%cSi tu n'as pas d'%wépée %cen tant&qu'enfant, pourquoi n'irais-tu pas&acheter quelques %wBâtons Mojo%c? "
          "Ils&sont efficaces contre tes ennemis!" }
    };
    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(Randomizer::NaviRandoMessageTableID);
    for (unsigned int i = 0; i <= (NUM_NAVI_MESSAGES - 1); i++) {
        customMessageManager->CreateMessage(
            Randomizer::NaviRandoMessageTableID, i,
            NaviMessages[i]);
    }
}

void CreateIceTrapRandoMessages() {
    CustomMessage IceTrapMessages[NUM_ICE_TRAP_MESSAGES] = {
        { "You are a %bFOOL%w!", "Du bist ein %bDUMMKOPF%w!", "%bPauvre fou%w..." },

        { "You are a %bFOWL%w!", "Du bist eine %bFrostbeule%w!", "Tu es un %bglaçon%w, Harry!" },

        { "%bFOOL%w!", "%bDUMMKOPF%w!", "%bSot%w que tu es." },

        { "You just got %bPUNKED%w!", "Du wurdest %beiskalt%w erwischt!", "Ça me %bglace%w le sang!" },

        { "Stay %bfrosty%w, @.", "Es läuft dir %beiskalt%w den Rücken&hinunter, @.", "%bReste au frais%w, @." },

        { "Take a %bchill pill%w, @.", "Bleib %bcool%w, @.", "Et c'est la douche %bfroide%w!" },

        { "%bWinter%w is coming.", "Der %bWinter%w naht.", "L'%bhiver%w vient." },

        { "%bICE%w to see you, @.", "Alles %bcool%w im Pool?", "%bGlacier%w!" },

        { "Feeling a little %rhot%w under the collar?&%bLet's fix that%w.", "%bAbkühlung gefällig%w?",
          "%Ça en jette un %bfroid%w." },

        { "It's a %bcold day%w in the Evil Realm.", "Es ist ein %kalter%w Tag im Herzen&von Hyrule.",
          "Est-ce que tu as déjà eu des sueurs&%bfroides%w?" },

        { "Getting %bcold feet%w?", "Bekommst du etwa %bkalte%w Füße?",
          "La vengeance est un plat qui se mange&%bfroid%w!" },

        { "Say hello to the %bZoras%w for me!", "Sag den %bZoras%w viele Grüße von mir!",
          "Dit bonjour aux %bZoras%w pour moi!" },

        { "Can you keep a %bcool head%w?", "Bewahre einen %bkühlen%w! Kopf.",
          "Il faut parfois savoir garder la tête&%bfroide%w!" },

        { "Ganondorf used %bIce Trap%w!&It's super effective!",
          "Ganondorf setzt %bEisstrahl%w ein.&Das ist sehr effektiv!",
          "Ganondorf utilise %bPiège de Glace%w!&C'est super efficace!" },

        { "Allow me to break the %bice%w!", "Ein Lächeln ist der beste Weg,&um das %bEis%w zu brechen!",
          "Laisse moi briser la %bglace%w!" },

        { "%bCold pun%w.", "%bEiskalt%w lässt du meine Seele&erfrier'n.",
          "Balance man...,&Cadence man...,&Trace la %bglace%w...,&c'est le Cooooolllll Rasta!" },

        { "The %bTitanic%w would be scared of you,&@.", "Die %bTitanic%w hätte Angst vor dir,&@.",
          "Le %bTitanic%w aurait peur de toi,&@." },

        { "Oh no!", "Oh nein!", "Oh non!" },

        { "What killed the dinosaurs?&The %bICE%w age!", "Was die Dinosaurier getötet hat?&Die %bEiszeit%w!",
          "Qu'est-ce qui a tué les dinosaures?&L'ère %bglacière%w!" },

        { "Knock knock. Who's there? Ice. Ice&who? Ice see that you're a %bFOOL%w.",
          "Nachts ist es %bkälter%w als draußen.",
          "L'imbécile réfléchit uniquement quand il&s'observe dans la %bglace%w." },

        { "Never gonna %bgive you up%w. Never&gonna %blet you down%w. Never gonna&run around and %bdesert you%w.",
          "Never gonna %bgive you up%w. Never&gonna %blet you down%w. Never gonna&run around and %bdesert you%w.",
          "Never gonna %bgive you up%w. Never&gonna %blet you down%w. Never gonna&run around and %bdesert you%w." },

        { "Thank you %b@%w!&But your item is in another castle!",
          "Danke %b@%w!&Aber der Gegenstand ist in&einem anderem Schloss!",
          "Merci %b@%w!&Mais ton objet est dans un autre&château!" },

        { "%bFREEZE%w! Don't move!", "	Kalt. Kalt. Kälter. %bEISKALT%w!",
          "J'espère que ça ne te fait ni chaud, ni&%bfroid%w." },

    };
    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(Randomizer::IceTrapRandoMessageTableID);
    for (u8 i = 0; i <= (NUM_ICE_TRAP_MESSAGES - 1); i++) {
        customMessageManager->CreateMessage(Randomizer::IceTrapRandoMessageTableID, i,
                                            IceTrapMessages[i]);
    }

    // We only use this ice trap message for christmas, so we don't want it in the normal ice trap messages rotation
    customMessageManager->CreateMessage(
        Randomizer::IceTrapRandoMessageTableID, NUM_ICE_TRAP_MESSAGES + 1,
        CustomMessage("This year for Christmas, all&you get is %BCOAL%w!",
                      "This year for Christmas, all&you get is %BCOAL%w!",
                      "Pour Noël, cette année, tu&n'auras que du %BCHARBON!&%rJoyeux Noël%w!"));
}

static int goronIDs[9] = { 0x3052, 0x3069, 0x306A, 0x306B, 0x306C, 0x306D, 0x306E, 0x306F, 0x3070 };

void CreateFireTempleGoronMessages() {
    CustomMessage FireTempleGoronMessages[NUM_GORON_MESSAGES] = {
        {
            "Are you the one they call %g@%w?^You look really weird for %rDarunia's kid.%w&Are you adopted?",
            "Du bist also der, den sie @ nennen?^Du siehst nicht aus als wärst du&%rDarunias Kind.%w Bist du "
            "adoptiert?",
            "C'est toi qu'on appelle %g@%w?^Tu es vraiment bizarre pour être&le %rfils du Chef%w. Tu as été adopté?",
        },
        {
            "Thank Hylia! I was so worried about&when my teacher would let me get&out of detention.^I gotta go home "
            "and see my parents.",
            "Ich wollte nur dieses Ding hier wieder&in seine Truhe zurücklegen, weil...^...gehört mir ja eigentlich "
            "nicht,&weißt du?^Doch dann ging plötzlich dieses&Tor hinter mir zu.&Danke für die Rettung.",
            "Par les déesses!&Mon Frère?!&C'est bien toi?&Comment ça on ne se connaît pas?^Tu trouves vraiment que "
            "je&ressemble à n'importe quel Goron?",
        },
        {
            "How long has it been, do you know?^%r{{days}}%w days!?^Oh no, and it's %r\x1F%w?&I have to check on my "
            "cake!!",
            "Weißt du zufällig, wie viele Tage&vergangen sind?^%r{{days}}%w Tage!?^Oh je, und es ist %r\x1F%w Uhr? "
            "Ich&muss dringend nach meinem Kuchen&sehen!!!",
            "Cela fait combien de temps que&je suis enfermé ici?&Non mais je ne vais pas crier.^COMBIEN?! %r{{days}}%w "
            "JOURS!?^En plus il est %r\x1F%w...&Il faut vraiment que je rentre...",
        },
        {
            // 0x39C7 - ganon laugh
            "\x12\x39\xC7You fell into my %rtrap!%w&Foolish boy, it was me, Ganondorf!!!^...whoa, where am I?&What "
            "happened?^Weird.",
            "\x12\x39\xC7"
            "Du bist mir in die %rFalle%w gegangen!&Du Narr, ich bin es, %rGanondorf%w!!!^...Huch? Wo bin ich? Was ist "
            "passiert?^Seltsam...",
            "\x12\x39\xC7Tu es tombé dans mon %rpiège%w!&Tu croyais que j'étais un Goron mais,&c'était moi! "
            "%rGanondorf%w!^...Hein? Où suis-je?&Que s'est-il passé?",
        },
        {
            "Thanks, but I don't know if I wanna go&just yet...^Hmm...^...^...^...^...^...maybe I can come back "
            "later.&Bye bye.",
            "Danke für die Rettung, aber&eigentlich finde ich es hier ganz&nett...^Hmm...^...^...^...^...^...Naja, ich "
            "kann ja jederzeit&wiederkommen. Man sieht sich.",
            "Merci, mais je me sens plus en&sécurité ici...^...^...^...^...^Hmm...^...Tout compte fait, je vais y "
            "aller.&A plus tard.",
        },
        { "Do you know about %b\x9f%w?&It's this weird symbol that's been&in my dreams lately...^Apparently, you "
          "pressed it %b{{a_btn}}%w times.^Wow.",
          "Weißt du über %b\x9f%w bescheid?&Es sind Symbole, die mir&in letzter Zeit öfter in&meinen Träumen "
          "erschienen sind...^Es scheint, dass du sie schon&%b{{a_btn}}%w mal betätigt hast.^Faszinierend...",
          "Tu as déjà entendu parler du&symbole %b\x9f%w?&C'est un symbole bizarre qui est&apparu dans mes rêves "
          "dernièrement...^Apparemment, tu as appuyé dessus&%b{{a_btn}}%w fois.^Wow..." },
        {
            "\x13\x1A"
            "Boy, you must be hot!&Get yourself a bottle of&%rLon Lon Milk%w right away and cool&down, for only %g30%w "
            "rupees!",
            "\x13\x1A"
            "Hey, ist dir nicht zu warm?&Besorg dir doch eine Flasche&%rLon Lon-Milch%w, um dich&abzukühlen.^Kostet "
            "dich auch nur %g30%w Rubine!",
            "\x13\x1A"
            "Woah! Tu dois avoir chaud!&Tu savais que tu pouvais acheter&du %rLait de Lon Lon%w pour&seulement %g30 "
            "rubis%w?^Il n'y a rien de mieux pour s'hydrater!",
        },
        {
            "In that case, I'll help you out!^They say that %rthe thing you're&looking for%w can only be found%g "
            "when&you're not looking for it.%w^Hope that helps!",
            "Pass auf, ich geb dir einen Tipp!^Man sagt, man findet %rdas was&man sucht%w nur, und wirklich nur&dann, "
            "%gwenn man gerade nicht danach&sucht%w.^Du kannst mich jederzeit wieder für&mehr hilfreiche Tipps "
            "aufsuchen!",
            "Dans ce cas, je vais t'aider!&On dit que l'objet que tu cherches&ne peut être trouvé que lorsque&tu ne le "
            "cherches pas.",
        },
        {
            "I dunno why I was thrown in here,&truth be told.&I'm just a %g\"PR\"%w person.",
            "Wat weiß'n ich, wieso ich hier&eingepfercht wurd. Ich mach&doch nur %g\"Pull&Requests\"%w.",
            "Je ne sais pas comment on m'a jeté&ici. Il faut croire que je dors comme&une pierre.",
        },
    };

    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(customMessageTableID);
    for (u8 i = 0; i <= NUM_GORON_MESSAGES - 1; i++) {
        customMessageManager->CreateMessage(customMessageTableID, goronIDs[i], FireTempleGoronMessages[i]);
    }
}

CustomMessage Randomizer::GetGoronMessage(u16 index) {
    CustomMessage messageEntry = CustomMessageManager::Instance->RetrieveMessage(customMessageTableID, goronIDs[index]);
    messageEntry.Replace("{{days}}", std::to_string(gSaveContext.totalDays));
    messageEntry.Replace("{{a_btn}}", std::to_string(gSaveContext.sohStats.count[COUNT_BUTTON_PRESSES_A]));
    return messageEntry;
}

void Randomizer::CreateCustomMessages() {
    // RANDTODO: Translate into french and german and replace GIMESSAGE_UNTRANSLATED
    // with GIMESSAGE(getItemID, itemID, english, german, french).
    const std::array<GetItemMessage, 72> getItemMessages = {{
        GIMESSAGE(RG_GREG_RUPEE, ITEM_MASK_GORON, 
			"You found %gGreg%w!",
			"%gGreg%w! Du hast ihn wirklich gefunden!",
            "Félicitation! Vous avez trouvé %gGreg%w!"),
        GIMESSAGE(RG_MASTER_SWORD, ITEM_SWORD_MASTER, 
            "You found the %gMaster Sword%w!",
            "Du erhältst dem %gMaster-Schwert%w!",
            "Vous obtenez %gl'Épée de Légende%w!"),
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
			"Vous obtenez la %rBourse de Magnat%w!&Elle peut contenir jusqu'à %y999 rubis%w!&C'est gigantesque!"),

        GIMESSAGE_UNTRANSLATED(RG_GOHMA_SOUL, ITEM_BIG_POE, "You found the soul for %gGohma%w!"),
        GIMESSAGE_UNTRANSLATED(RG_KING_DODONGO_SOUL, ITEM_BIG_POE, "You found the soul for %rKing&Dodongo%w!"),
        GIMESSAGE_UNTRANSLATED(RG_BARINADE_SOUL, ITEM_BIG_POE, "You found the soul for %bBarinade%w!"),
        GIMESSAGE_UNTRANSLATED(RG_PHANTOM_GANON_SOUL, ITEM_BIG_POE, "You found the soul for %gPhantom&Ganon%w!"),
        GIMESSAGE_UNTRANSLATED(RG_VOLVAGIA_SOUL, ITEM_BIG_POE, "You found the soul for %rVolvagia%w!"),
        GIMESSAGE_UNTRANSLATED(RG_MORPHA_SOUL, ITEM_BIG_POE, "You found the soul for %bMorpha%w!"),
        GIMESSAGE_UNTRANSLATED(RG_BONGO_BONGO_SOUL, ITEM_BIG_POE, "You found the soul for %pBongo&Bongo%w!"),
        GIMESSAGE_UNTRANSLATED(RG_TWINROVA_SOUL, ITEM_BIG_POE, "You found the soul for %yTwinrova%w!"),
        GIMESSAGE_UNTRANSLATED(RG_GANON_SOUL, ITEM_BIG_POE, "You found the soul for %cGanon%w!"),

        GIMESSAGE(RG_OCARINA_A_BUTTON, ITEM_OCARINA_TIME,
            "You got the %b\x9f%r button for the&Ocarina%w! You can now use it&while playing songs!",
			"Der %b\x9f%r Knopf%w!&Du kannst ihn nun zum Spielen&von Liedern auf der %rOkarina%w&verwenden!",
			"Vous trouvez la %rtouche %b\x9f%r de&l'Ocarina%w! Vous pouvez&maintenant l'utiliser lorsque&vous en jouez!"),
        GIMESSAGE(RG_OCARINA_C_LEFT_BUTTON, ITEM_OCARINA_TIME,
            "You got the %y\xa7%r button for the&Ocarina%w! You can now use it&while playing songs!",
			"Der %y\xa7%r Knopf%w!&Du kannst ihn nun zum Spielen&von Liedern auf der %rOkarina%w&verwenden!",
			"Vous trouvez la %rtouche %y\xa7%r de&l'Ocarina%w! Vous pouvez&maintenant l'utiliser lorsque&vous en jouez!"),
        GIMESSAGE(RG_OCARINA_C_RIGHT_BUTTON, ITEM_OCARINA_TIME,
            "You got the %y\xa8%r button for the&Ocarina%w! You can now use it&while playing songs!",
			"Der %y\xa8%r Knopf%w!&Du kannst ihn nun zum Spielen&von Liedern auf der %rOkarina%w&verwenden!",
			"Vous trouvez la %rtouche %y\xa8%r de&l'Ocarina%w! Vous pouvez&maintenant l'utiliser lorsque&vous en jouez!"),
        GIMESSAGE(RG_OCARINA_C_UP_BUTTON, ITEM_OCARINA_TIME,
            "You got the %y\xa5%r button for the&Ocarina%w! You can now use it&while playing songs!",
			"Der %y\xa5%r Knopf%w!&Du kannst ihn nun zum Spielen&von Liedern auf der %rOkarina%w&verwenden!",
			"Vous trouvez la %rtouche %y\xa5%r de&l'Ocarina%w! Vous pouvez&maintenant l'utiliser lorsque&vous en jouez!"),
        GIMESSAGE(RG_OCARINA_C_DOWN_BUTTON, ITEM_OCARINA_TIME,
            "You got the %y\xa6%r button for the&Ocarina%w! You can now use it&while playing songs!",
			"Der %y\xa6%r Knopf%w!&Du kannst ihn nun zum Spielen&von Liedern auf der %rOkarina%w&verwenden!",
			"Vous trouvez la %rtouche %y\xa6%r de&l'Ocarina%w! Vous pouvez&maintenant l'utiliser lorsque&vous en jouez!"),
        GIMESSAGE_UNTRANSLATED(RG_FISHING_POLE, ITEM_FISHING_POLE, "You found a lost %rFishing Pole%w!&Time to hit the pond!"),
    }};
    CreateGetItemMessages(&getItemMessages);
    CreateRupeeMessages();
    CreateTriforcePieceMessages();
    CreateNaviRandoMessages();
    CreateIceTrapRandoMessages();
    CreateFireTempleGoronMessages();
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

void RandomizerSettingsWindow::InitElement() {
    mSettings = Rando::Context::GetInstance()->GetSettings();
    Randomizer::CreateCustomMessages();
    seedString = (char*)calloc(MAX_SEED_STRING_SIZE, sizeof(char));
    Rando::Context::GetInstance()->GetSettings()->UpdateOptionProperties();
}
