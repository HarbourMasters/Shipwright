#include "settings.h"
#include "trial.h"
#include "dungeon.h"
#include "context.h"

#include "soh/OTRGlobals.h"

#include <spdlog/spdlog.h>

namespace Rando {
std::vector<std::string> NumOpts(int min, int max, int step = 1, std::string textBefore = {}, std::string textAfter = {}) {
    std::vector<std::string> options;
    options.reserve((max - min) / step + 1);
    for (int i = min; i <= max; i += step) {
        options.push_back(textBefore + std::to_string(i) + textAfter);
    }
    return options;
}

std::vector<std::string> MultiVecOpts(std::vector<std::vector<std::string>> optionsVector) {
    uint32_t totalSize = 0;
    for (auto vector: optionsVector) {
        totalSize += vector.size();
    }
    std::vector<std::string> options;
    options.reserve(totalSize);
    for (auto vector : optionsVector) {
        for (auto op : vector) {
            options.push_back(op);
        }
    }
    return options;
}

Settings::Settings() : mExcludeLocationsOptionsGroups(SPOILER_COLLECTION_GROUP_COUNT) {
}

void Settings::CreateOptions() {
    CreateOptionDescriptions();
    // clang-format off
    mOptions[RSK_FOREST] = Option::U8("Forest", {"Closed", "Closed Deku", "Open"}, OptionCategory::Setting, "gRandomizeForest", mOptionDescriptions[RSK_FOREST], WidgetType::Combobox, RO_FOREST_CLOSED);
    mOptions[RSK_KAK_GATE] = Option::U8("Kakariko Gate", {"Close", "Open"}, OptionCategory::Setting, "gRandomizeKakarikoGate", mOptionDescriptions[RSK_KAK_GATE]);
    mOptions[RSK_DOOR_OF_TIME] = Option::U8("Door of Time", {"Closed", "Song only", "Open"}, OptionCategory::Setting, "gRandomizeDoorOfTime", mOptionDescriptions[RSK_DOOR_OF_TIME], WidgetType::Combobox);
    mOptions[RSK_ZORAS_FOUNTAIN] = Option::U8("Zora's Fountain", {"Closed", "Closed as child", "Open"}, OptionCategory::Setting, "gRandomizeZorasFountain", mOptionDescriptions[RSK_ZORAS_FOUNTAIN]);
    mOptions[RSK_GERUDO_FORTRESS] = Option::U8("Gerudo Fortress", {"Normal", "Fast", "Open"}, OptionCategory::Setting, "gRandomizeGerudoFortress", mOptionDescriptions[RSK_GERUDO_FORTRESS]);
    mOptions[RSK_RAINBOW_BRIDGE] = Option::U8("Rainbow Bridge", {"Vanilla", "Always open", "Stones", "Medallions", "Dungeon rewards", "Dungeons", "Tokens", "Greg"}, OptionCategory::Setting, "gRandomizeRainbowBridge", mOptionDescriptions[RSK_RAINBOW_BRIDGE], WidgetType::Combobox, RO_BRIDGE_VANILLA);
    mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT] = Option::U8("Stone Count", {NumOpts(0, 4)}, OptionCategory::Setting, "gRandomizeStoneCount", "", WidgetType::Slider, 3, true);
    mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT] = Option::U8("Medallion Count", {NumOpts(0, 7)}, OptionCategory::Setting, "gRandomizeMedallionCount", "", WidgetType::Slider, 6, true);
    mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT] = Option::U8("Reward Count", {NumOpts(0, 10)}, OptionCategory::Setting, "gRandomizeRewardCount", "", WidgetType::Slider, 9, true);
    mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT] = Option::U8("Dungeon Count", {NumOpts(0, 9)}, OptionCategory::Setting, "gRandomizeDungeonCount", "", WidgetType::Slider, 8, true);
    mOptions[RSK_RAINBOW_BRIDGE_TOKEN_COUNT] = Option::U8("Token Count", {NumOpts(0, 100)}, OptionCategory::Setting, "gRandomizeTokenCount", "", WidgetType::Slider, 100, true);
    mOptions[RSK_BRIDGE_OPTIONS] = Option::U8("Bridge Reward Options", {"Standard Rewards", "Greg as Reward", "Greg as Wildcard"}, OptionCategory::Setting, "gRandomizeBridgeRewardOptions", mOptionDescriptions[RSK_BRIDGE_OPTIONS], WidgetType::Combobox);
    mOptions[RSK_GANONS_TRIALS] = Option::U8("Ganon's Trials", {"Skip", "Set Number", "Random Number"}, OptionCategory::Setting, "gRandomizeGanonTrial", mOptionDescriptions[RSK_GANONS_TRIALS], WidgetType::Combobox, RO_GANONS_TRIALS_SET_NUMBER);
    mOptions[RSK_TRIAL_COUNT] = Option::U8("Ganon's Trials Count", {NumOpts(0, 6)}, OptionCategory::Setting, "gRandomizeGanonTrialCount", mOptionDescriptions[RSK_TRIAL_COUNT], WidgetType::Slider, 6, true);
    mOptions[RSK_STARTING_AGE] = Option::U8("Starting Age", {"Child", "Adult", "Random"}, OptionCategory::Setting, "gRandomizeStartingAge", mOptionDescriptions[RSK_STARTING_AGE], WidgetType::Combobox, RO_AGE_CHILD);
    mOptions[RSK_SHUFFLE_ENTRANCES] = Option::Bool("Shuffle Entrances");
    mOptions[RSK_SHUFFLE_DUNGEON_ENTRANCES] = Option::U8("Dungeon Entrances", {"Off", "On", "On + Ganon"}, OptionCategory::Setting, "gRandomizeShuffleDungeonsEntrances", mOptionDescriptions[RSK_SHUFFLE_DUNGEON_ENTRANCES], WidgetType::Combobox, RO_DUNGEON_ENTRANCE_SHUFFLE_OFF);
    mOptions[RSK_SHUFFLE_BOSS_ENTRANCES] = Option::U8("Boss Entrances", {"Off", "Age Restricted", "Full"}, OptionCategory::Setting, "gRandomizeShuffleBossEntrances", mOptionDescriptions[RSK_SHUFFLE_BOSS_ENTRANCES], WidgetType::Combobox, RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF);
    mOptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES] = Option::Bool("Overworld Entrances", "gRandomizeShuffleOverworldEntrances", mOptionDescriptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES]);
    mOptions[RSK_SHUFFLE_INTERIOR_ENTRANCES] = Option::U8("Interior Entrances", {"Off", "Simple", "All"}, OptionCategory::Setting, "gRandomizeShuffleInteriorsEntrances", mOptionDescriptions[RSK_SHUFFLE_INTERIOR_ENTRANCES], WidgetType::Combobox, RO_INTERIOR_ENTRANCE_SHUFFLE_OFF);
    mOptions[RSK_SHUFFLE_GROTTO_ENTRANCES] = Option::Bool("Grottos Entrances", "gRandomizeShuffleGrottosEntrances", mOptionDescriptions[RSK_SHUFFLE_GROTTO_ENTRANCES]);
    mOptions[RSK_SHUFFLE_OWL_DROPS] = Option::Bool("Owl Drops", "gRandomizeShuffleOwlDrops", mOptionDescriptions[RSK_SHUFFLE_OWL_DROPS]);
    mOptions[RSK_SHUFFLE_WARP_SONGS] = Option::Bool("Warp Songs", "gRandomizeShuffleWarpSongs", mOptionDescriptions[RSK_SHUFFLE_WARP_SONGS]);
    mOptions[RSK_SHUFFLE_OVERWORLD_SPAWNS] = Option::Bool("Overworld Spawns", "gRandomizeShuffleOverworldSpanws", mOptionDescriptions[RSK_SHUFFLE_OVERWORLD_SPAWNS]);
    mOptions[RSK_MIXED_ENTRANCE_POOLS] = Option::Bool("Mixed Entrance Pools", "gRandomizeMixedEntrances", mOptionDescriptions[RSK_MIXED_ENTRANCE_POOLS]);
    mOptions[RSK_MIX_DUNGEON_ENTRANCES] = Option::Bool("Mix Dungeons", "gRandomizeMixDungeons", mOptionDescriptions[RSK_MIX_DUNGEON_ENTRANCES]);
    mOptions[RSK_MIX_OVERWORLD_ENTRANCES] = Option::Bool("Mix Overworld", "gRandomizeMixOverworld", mOptionDescriptions[RSK_MIX_OVERWORLD_ENTRANCES]);
    mOptions[RSK_MIX_INTERIOR_ENTRANCES] = Option::Bool("Mix Interiors", "gRandomizeMixInteriors", mOptionDescriptions[RSK_MIX_INTERIOR_ENTRANCES]);
    mOptions[RSK_MIX_GROTTO_ENTRANCES] = Option::Bool("Mix Grottos", "gRandomizeMixGrottos", mOptionDescriptions[RSK_MIX_GROTTO_ENTRANCES]);
    mOptions[RSK_DECOUPLED_ENTRANCES] = Option::Bool("Decouple Entrances", "gRandomizeDecoupleEntrances", mOptionDescriptions[RSK_DECOUPLED_ENTRANCES]);
    mOptions[RSK_BOMBCHUS_IN_LOGIC] = Option::Bool("Bombchus in Logic", "gRandomizeBombchusInLogic");
    mOptions[RSK_ENABLE_BOMBCHU_DROPS] = Option::U8("Bombchu Drops", {"No", "Yes"}, OptionCategory::Setting, "gRandomizeEnableBombchuDrops", "", WidgetType::Combobox, RO_AMMO_DROPS_ON);
    // TODO: AmmoDrops and/or HeartDropRefill, combine with/separate Ammo Drops from Bombchu Drops?
    mOptions[RSK_TRIFORCE_HUNT] = Option::Bool("Triforce Hunt", "gRandomizeTriforceHunt", mOptionDescriptions[RSK_TRIFORCE_HUNT]);
    mOptions[RSK_TRIFORCE_HUNT_PIECES_TOTAL] = Option::U8("Triforce Hunt Total Pieces", {NumOpts(1, 100)}, OptionCategory::Setting, "gRandomizeTriforceHuntTotalPieces", mOptionDescriptions[RSK_TRIFORCE_HUNT_PIECES_TOTAL], WidgetType::Slider, 29);
    mOptions[RSK_TRIFORCE_HUNT_PIECES_REQUIRED] = Option::U8("Triforce Hunt Required Pieces", {NumOpts(1, 100)}, OptionCategory::Setting, "gRandomizeTriforceHuntRequiredPieces", mOptionDescriptions[RSK_TRIFORCE_HUNT_PIECES_REQUIRED], WidgetType::Slider, 19);
    mOptions[RSK_MQ_DUNGEON_RANDOM] = Option::U8("MQ Dungeon Setting", {"None", "Set Number", "Random", "Selection Only"}, OptionCategory::Setting, "gRandomizeMqDungeons", mOptionDescriptions[RSK_MQ_DUNGEON_RANDOM], WidgetType::Combobox, RO_MQ_DUNGEONS_NONE, true);
    mOptions[RSK_MQ_DUNGEON_COUNT] = Option::U8("MQ Dungeon Count", {NumOpts(0, 12)}, OptionCategory::Setting, "gRandomizeMqDungeonCount", "", WidgetType::Slider, 12, true);
    mOptions[RSK_MQ_DUNGEON_SET] = Option::Bool("Set Dungeon Quests", {"Off", "On"}, OptionCategory::Setting, "gRandomizeMqDungeonsSelection", mOptionDescriptions[RSK_MQ_DUNGEON_SET]);
    mOptions[RSK_MQ_DEKU_TREE] = Option::U8("Deku Tree", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, "gRandomizeMqDungeonsDekuTree", "", WidgetType::Combobox, RO_MQ_SET_VANILLA);
    mOptions[RSK_MQ_DODONGOS_CAVERN] = Option::U8("Dodongo's Cavern", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, "gRandomizeMqDungeonsDodongosCavern", "", WidgetType::Combobox, RO_MQ_SET_VANILLA);
    mOptions[RSK_MQ_JABU_JABU] = Option::U8("Jabu-Jabu's Belly", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, "gRandomizeMqDungeonsJabuJabu", "", WidgetType::Combobox, RO_MQ_SET_VANILLA);
    mOptions[RSK_MQ_FOREST_TEMPLE] = Option::U8("Forest Temple", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, "gRandomizeMqDungeonsForestTemple", "", WidgetType::Combobox, RO_MQ_SET_VANILLA);
    mOptions[RSK_MQ_FIRE_TEMPLE] = Option::U8("Fire Temple", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, "gRandomizeMqDungeonsFireTemple", "", WidgetType::Combobox, RO_MQ_SET_VANILLA);
    mOptions[RSK_MQ_WATER_TEMPLE] = Option::U8("Water Temple", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, "gRandomizeMqDungeonsWaterTemple", "", WidgetType::Combobox, RO_MQ_SET_VANILLA);
    mOptions[RSK_MQ_SPIRIT_TEMPLE] = Option::U8("Spirit Temple", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, "gRandomizeMqDungeonsSpiritTemple", "", WidgetType::Combobox, RO_MQ_SET_VANILLA);
    mOptions[RSK_MQ_SHADOW_TEMPLE] = Option::U8("Shadow Temple", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, "gRandomizeMqDungeonsShadowTemple", "", WidgetType::Combobox, RO_MQ_SET_VANILLA);
    mOptions[RSK_MQ_BOTTOM_OF_THE_WELL] = Option::U8("Bottom of the Well", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, "gRandomizeMqDungeonsBottomOfTheWell", "", WidgetType::Combobox, RO_MQ_SET_VANILLA);
    mOptions[RSK_MQ_ICE_CAVERN] = Option::U8("Ice Cavern", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, "gRandomizeMqDungeonsIceCavern", "", WidgetType::Combobox, RO_MQ_SET_VANILLA);
    mOptions[RSK_MQ_GTG] = Option::U8("Gerudo Training Grounds", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, "gRandomizeMqDungeonsGTG", "", WidgetType::Combobox, RO_MQ_SET_VANILLA);
    mOptions[RSK_MQ_GANONS_CASTLE] = Option::U8("Ganon's Castle", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, "gRandomizeMqDungeonsGanonsCastle", "", WidgetType::Combobox, RO_MQ_SET_VANILLA);
    mOptions[RSK_SHUFFLE_DUNGEON_REWARDS] = Option::U8("Shuffle Dungeon Rewards", {"End of Dungeons", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, "gRandomizeShuffleDungeonReward", "", WidgetType::Combobox, RO_DUNGEON_REWARDS_END_OF_DUNGEON);
    mOptions[RSK_LINKS_POCKET] = Option::U8("Link's Pocket", {"Dungeon Reward", "Advancement", "Anything", "Nothing"}, OptionCategory::Setting, "gRandomizeLinksPocket", "", WidgetType::Combobox, RO_LINKS_POCKET_DUNGEON_REWARD, true);
    mOptions[RSK_SHUFFLE_SONGS] = Option::U8("Shuffle Songs", {"Song Locations", "Dungeon Rewards", "Anywhere"}, OptionCategory::Setting, "gRandomizeShuffleSongs", "", WidgetType::Combobox, RO_SONG_SHUFFLE_SONG_LOCATIONS);
    mOptions[RSK_SHOPSANITY] = Option::U8("Shopsanity", {"Off", "0 Items", "1 Item", "2 Items", "3 Items", "4 Items", "Random"}, OptionCategory::Setting, "gRandomizeShopsanity", "", WidgetType::Combobox, RO_SHOPSANITY_OFF);
    mOptions[RSK_SHOPSANITY_PRICES] = Option::U8("Shopsanity Prices", {"Balanced", "Starting Wallet", "Adult Wallet", "Giant's Wallet", "Tycoon's Wallet"}, OptionCategory::Setting, "gRandomizeShopsanityPrices", "", WidgetType::Combobox, RO_SHOPSANITY_PRICE_BALANCED);
    mOptions[RSK_SHOPSANITY_PRICES_AFFORDABLE] = Option::Bool("Affordable Prices", "gRandomizeShopsanityPricesAffordable"); // TODO: Can this be added as a selection for above option?
    mOptions[RSK_SHUFFLE_TOKENS] = Option::U8("Tokensanity", {"Off", "Dungeons", "Overworld", "All Tokens"}, OptionCategory::Setting, "gRandomizeShuffleTokens", "", WidgetType::Combobox, RO_TOKENSANITY_OFF);
    mOptions[RSK_SHUFFLE_SCRUBS] = Option::U8("Scrub Shuffle", {"Off", "Affordable", "Expensive", "Random Prices"}, OptionCategory::Setting, "gRandomizeShuffleScrubs", "", WidgetType::Combobox, RO_SCRUBS_OFF);
    mOptions[RSK_SHUFFLE_COWS] = Option::Bool("Shuffle Cows", "gRandomizeShuffleCows");
    mOptions[RSK_SHUFFLE_KOKIRI_SWORD] = Option::Bool("Shuffle Kokiri Sword", "gRandomizeShuffleKokiriSword");
    mOptions[RSK_SHUFFLE_MASTER_SWORD] = Option::Bool("Shuffle Master Sword", "gRandomizeShuffleMasterSword");
    mOptions[RSK_SHUFFLE_OCARINA] = Option::Bool("Shuffle Ocarinas", "gRandomizeShuffleOcarinas");
    mOptions[RSK_SHUFFLE_WEIRD_EGG] = Option::Bool("Shuffle Weird Egg", "gRandomizeShuffleWeirdEgg");
    mOptions[RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD] = Option::Bool("Shuffle Gerudo Membership Card", "gRandomizeShuffleGerudoToken");
    mOptions[RSK_SHUFFLE_MAGIC_BEANS] = Option::Bool("Shuffle Magic Beans", "gRandomizeShuffleBeans");
    mOptions[RSK_SHUFFLE_MERCHANTS] = Option::U8("Shuffle Merchants", {"Off", "On (No Hints)", "On (With Hints)"}, OptionCategory::Setting, "gRandomizeShuffleMerchants", "", WidgetType::Combobox, RO_SHUFFLE_MERCHANTS_OFF);
    mOptions[RSK_SHUFFLE_FROG_SONG_RUPEES] = Option::Bool("Shuffle Frog Song Rupees", "gRandomizeShuffleFrogSongRupees");
    mOptions[RSK_SHUFFLE_ADULT_TRADE] = Option::Bool("Shuffle Adult Trade", "gRandomizeShuffleAdultTrade");
    mOptions[RSK_SHUFFLE_CHEST_MINIGAME] = Option::U8("Shuffle Chest Minigame", {"Off", "On (Separate)", "On (Pack)"});
    mOptions[RSK_SHUFFLE_100_GS_REWARD] = Option::Bool("Shuffle 100 GS Reward", "gRandomizeShuffle100GSReward", "", WidgetType::Checkbox, RO_GENERIC_OFF);
    mOptions[RSK_SHUFFLE_MAPANDCOMPASS] = Option::U8("Maps/Compasses", {"Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, "gRandomizeStartingMapsCompasses", "", WidgetType::Combobox, RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
    mOptions[RSK_KEYSANITY] = Option::U8("Small Keys", {"Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, "gRandomizeKeysanity", "", WidgetType::Combobox, RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
    mOptions[RSK_GERUDO_KEYS] = Option::U8("Gerudo Fortress Keys", {"Vanilla", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, "gRandomizeGerudoKeys", "", WidgetType::Combobox, RO_GERUDO_KEYS_VANILLA);
    mOptions[RSK_BOSS_KEYSANITY] = Option::U8("Boss Keys", {"Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, "gRandomizeBossKeysanity", "", WidgetType::Combobox, RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
    mOptions[RSK_GANONS_BOSS_KEY] = Option::U8("Ganon's Boss Key", {"Vanilla", "Own Dungeon", "Start With", "Any Dungeon", "Overworld", "Anywhere", "LACS-Vanilla", "LACS-Stones", "LACS-Medallions", "LACS-Rewards", "LACS-Dungeons", "LACS-Tokens", "Triforce Hunt"}, OptionCategory::Setting, "gRandomizeShuffleGanonBossKey", "", WidgetType::Combobox, RO_GANON_BOSS_KEY_VANILLA);
    mOptions[RSK_LACS_STONE_COUNT] = Option::U8("Stone Count", {NumOpts(0, 4)}, OptionCategory::Setting, "gRandomizeLacsStoneCount", "", WidgetType::Slider, 3, true);
    mOptions[RSK_LACS_MEDALLION_COUNT] = Option::U8("Medallion Count", {NumOpts(0, 7)}, OptionCategory::Setting, "gRandomizeLacsMedallionCount", "", WidgetType::Slider, 6, true);
    mOptions[RSK_LACS_REWARD_COUNT] = Option::U8("Reward Count", {NumOpts(0, 10)}, OptionCategory::Setting, "gRandomizeLacsRewardCount", "", WidgetType::Slider, 9, true);
    mOptions[RSK_LACS_DUNGEON_COUNT] = Option::U8("Dungeon Count", {NumOpts(0, 9)}, OptionCategory::Setting, "gRandomizeLacsDungeonCount", "", WidgetType::Slider, 8, true);
    mOptions[RSK_LACS_TOKEN_COUNT] = Option::U8("Token Count", {NumOpts(0, 100)}, OptionCategory::Setting, "gRandomizeLacsTokenCount", "", WidgetType::Slider, 100, true);
    mOptions[RSK_LACS_OPTIONS] = Option::U8("LACS Reward Options", {"Standard Reward", "Greg as Reward", "Greg as Wildcard"}, OptionCategory::Setting, "gRandomizeLacsRewardOptions", "", WidgetType::Combobox, RO_LACS_STANDARD_REWARD);
    mOptions[RSK_KEYRINGS] = Option::U8("Key Rings", {"Off", "Random", "Count", "Selection"}, OptionCategory::Setting, "gRandomizeShuffleKeyRings", "", WidgetType::Combobox, RO_KEYRINGS_OFF);
    mOptions[RSK_KEYRINGS_RANDOM_COUNT] = Option::U8("Keyring Dungeon Count", {NumOpts(0, 9)}, OptionCategory::Setting, "gRandomizeShuffleKeyRingsRandomCount", "", WidgetType::Slider, 8);
    mOptions[RSK_KEYRINGS_GERUDO_FORTRESS] = Option::Bool("Gerudo Fortress", "gRandomizeShuffleKeyRingsGerudoFortress");
    mOptions[RSK_KEYRINGS_FOREST_TEMPLE] = Option::Bool("Forest Temple", "gRandomizeShuffleKeyRingsForestTemple");
    mOptions[RSK_KEYRINGS_FIRE_TEMPLE] = Option::Bool("Fire Temple", "gRandomizeShuffleKeyRingsFireTemple");
    mOptions[RSK_KEYRINGS_WATER_TEMPLE] = Option::Bool("Water Temple", "gRandomizeShuffleKeyRingsWaterTemple");
    mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE] = Option::Bool("Spirit Temple", "gRandomizeShuffleKeyRingsSpiritTemple");
    mOptions[RSK_KEYRINGS_SHADOW_TEMPLE] = Option::Bool("Shadow Temple", "gRandomizeShuffleKeyRingsShadowTemple");
    mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL] = Option::Bool("Bottom of the Well", "gRandomizeShuffleKeyRingsBottomOfTheWell");
    mOptions[RSK_KEYRINGS_GTG] = Option::Bool("Gerudo Training Grounds", "gRandomizeShuffleKeyRingsGTG");
    mOptions[RSK_KEYRINGS_GANONS_CASTLE] = Option::Bool("Ganon's Castle", "gRandomizeShuffleKeyRingsGanonsCastle");
    mOptions[RSK_SKIP_CHILD_STEALTH] = Option::Bool("Skip Child Stealth", {"Don't Skip", "Skip"}, OptionCategory::Setting, "gRandomizeSkipChildZelda", "", WidgetType::Checkbox, RO_GENERIC_DONT_SKIP);
    mOptions[RSK_SKIP_CHILD_ZELDA] = Option::Bool("Skip Child Zelda", {"Don't Skip", "Skip"}, OptionCategory::Setting, "gRandomizeSkipChildZelda", "", WidgetType::Checkbox, RO_GENERIC_DONT_SKIP);
    mOptions[RSK_SKIP_TOWER_ESCAPE] = Option::Bool("Skip Tower Escape", {"Don't Skip", "Skip"}, OptionCategory::Setting, "gRandomizeSkipTowerEscape", "", WidgetType::Checkbox, RO_GENERIC_DONT_SKIP);
    mOptions[RSK_SKIP_EPONA_RACE] = Option::Bool("Skip Epona Race", {"Don't Skip", "Skip"}, OptionCategory::Setting, "gRandomizeSkipEponaRace", "", WidgetType::Checkbox, RO_GENERIC_DONT_SKIP);
    mOptions[RSK_SKIP_SCARECROWS_SONG] = Option::Bool("Skip Scarecrow's Song", "gRandomizeSkipScarecrowsSong");
    mOptions[RSK_BIG_POE_COUNT] = Option::U8("Big Poe Target Count", {NumOpts(1, 10)}, OptionCategory::Setting, "gRandomizeBigPoeTargetCount", "The Poe collector will give a reward for turning in this many Big Poes.", WidgetType::Slider, 9);
    mOptions[RSK_CUCCO_COUNT] = Option::U8("Cuccos to return", {NumOpts(0, 7)}, OptionCategory::Setting, "gRandomizeCuccosToReturn", "", WidgetType::Checkbox, 7);
    mOptions[RSK_COMPLETE_MASK_QUEST] = Option::Bool("Complete Mask Quest", "gRandomizeCompleteMaskQuest");
    mOptions[RSK_ENABLE_GLITCH_CUTSCENES] = Option::Bool("Enable Glitch-Useful Cutscenes", "gRandomizeEnableGlitchCutscenes");
    mOptions[RSK_GOSSIP_STONE_HINTS] = Option::U8("Gossip Stone Hints", {"No Hints", "Need Nothing", "Mask of Truth", "Stone of Agony"}, OptionCategory::Setting, "gRandomizeGossipStoneHints", "", WidgetType::Combobox, RO_GOSSIP_STONES_NEED_NOTHING);
    mOptions[RSK_HINT_CLARITY] = Option::U8("Hint Clarity", {"Obscure", "Ambiguous", "Clear"}, OptionCategory::Setting, "gRandomizeHintClarity", "", WidgetType::Combobox, RO_HINT_CLARITY_CLEAR);
    mOptions[RSK_HINT_DISTRIBUTION] = Option::U8("Hint Distribution", {"Useless", "Balanced", "Strong", "Very Strong"}, OptionCategory::Setting, "gRandomizeHintDistribution", "", WidgetType::Combobox, RO_HINT_DIST_BALANCED);
    mOptions[RSK_TOT_ALTAR_HINT] = Option::Bool("ToT Altar Hint", {"Off", "On"}, OptionCategory::Setting, "gRandomizeAltarHint", "", WidgetType::Checkbox, RO_GENERIC_ON);
    mOptions[RSK_LIGHT_ARROWS_HINT] = Option::Bool("Light Arrow Hint", {"Off", "On"}, OptionCategory::Setting, "gRandomizeLAHint", "", WidgetType::Checkbox, RO_GENERIC_ON);
    mOptions[RSK_DAMPES_DIARY_HINT] = Option::Bool("Dampe's Diary Hint", "gRandomizeDampeHint");
    mOptions[RSK_GREG_HINT] = Option::Bool("Greg the Green Rupee Hint", "gRandomizeGregHint");
    mOptions[RSK_SARIA_HINT] = Option::Bool("Saria's Hint", "gRandomizeSariaHint");
    mOptions[RSK_FROGS_HINT] = Option::Bool("Frog Ocarina Game Hint", "gRandomizeFrogsHint");
    mOptions[RSK_WARP_SONG_HINTS] = Option::Bool("Warp Song Hints", "gRandomizeWarpSongText", "", WidgetType::Checkbox, RO_GENERIC_ON);
    mOptions[RSK_KAK_10_SKULLS_HINT] = Option::Bool("10 GS Hint", "gRandomize10GSHint");
    mOptions[RSK_KAK_20_SKULLS_HINT] = Option::Bool("20 GS Hint", "gRandomize20GSHint");
    mOptions[RSK_KAK_30_SKULLS_HINT] = Option::Bool("30 GS Hint", "gRandomize30GSHint");
    mOptions[RSK_KAK_40_SKULLS_HINT] = Option::Bool("40 GS Hint", "gRandomize40GSHint");
    mOptions[RSK_KAK_50_SKULLS_HINT] = Option::Bool("50 GS Hint", "gRandomize50GSHint");
    mOptions[RSK_SCRUB_TEXT_HINT] = Option::Bool("Scrub Hint Text", "gRandomizeScrubText");
    // TODO: Compasses show rewards/woth, maps show dungeon mode
    mOptions[RSK_BLUE_FIRE_ARROWS] = Option::Bool("Blue Fire Arrows", "gRandomizeBlueFireArrows");
    mOptions[RSK_SUNLIGHT_ARROWS] = Option::Bool("Sunlight Arrows", "gRandomizeSunlightArrows");
    mOptions[RSK_ITEM_POOL] = Option::U8("Item Pool", {"Plentiful", "Balanced", "Scarce", "Minimal"}, OptionCategory::Setting, "gRandomizeItemPool", "", WidgetType::Combobox, RO_ITEM_POOL_BALANCED);
    mOptions[RSK_ICE_TRAPS] = Option::U8("Ice Traps", {"Off", "Normal", "Extra", "Mayhem", "Onslaught"}, OptionCategory::Setting, "gRandomizeIceTraps", "", WidgetType::Combobox, RO_ICE_TRAPS_NORMAL);
    // TODO: Remove Double Defense, Progressive Goron Sword
    mOptions[RSK_STARTING_OCARINA] = Option::U8("Start with Ocarina", {"Off", "Fairy Ocarina", "Ocarina of Time"}, OptionCategory::Setting, "gRandomizeStartingOcarina", "", WidgetType::Combobox, RO_STARTING_OCARINA_OFF);
    mOptions[RSK_STARTING_DEKU_SHIELD] = Option::Bool("Start with Deku Shield", "gRandomizeStartingDekuShield");
    mOptions[RSK_STARTING_KOKIRI_SWORD] = Option::Bool("Start with Kokiri Sword", "gRandomizeStartingKokiriSword");
    mOptions[RSK_STARTING_MASTER_SWORD] = Option::Bool("Start with Master Sword", "gRandomizeStartingMasterSword");
    mOptions[RSK_STARTING_CONSUMABLES] = Option::Bool("Start with Consumables", {"No", "Yes"}, OptionCategory::Setting, "gRandomizeStartingConsumables", "", WidgetType::Checkbox, RO_GENERIC_OFF);
    mOptions[RSK_FULL_WALLETS] = Option::Bool("Full Wallets", {"No", "Yes"}, OptionCategory::Setting, "gRandomizeFullWallets", "", WidgetType::Checkbox, RO_GENERIC_OFF);
    mOptions[RSK_STARTING_ZELDAS_LULLABY] = Option::Bool("Start with Zelda's Lullaby", "gRandomizeStartingZeldasLullaby");
    mOptions[RSK_STARTING_EPONAS_SONG] = Option::Bool("Start with Epona's Song", "gRandomizeStartingEponasSong");
    mOptions[RSK_STARTING_SARIAS_SONG] = Option::Bool("Start with Epona's Song", "gRandomizeStartingSariasSong");
    mOptions[RSK_STARTING_SUNS_SONG] = Option::Bool("Start with Sun's Song", "gRandomizeStartingSunsSong");
    mOptions[RSK_STARTING_SONG_OF_TIME] = Option::Bool("Start with Song of Time", "gRandomizeStartingSongOfTime");
    mOptions[RSK_STARTING_SONG_OF_STORMS] = Option::Bool("Start with Song of Storms", "gRandomizeStartingSongOfStorms");
    mOptions[RSK_STARTING_MINUET_OF_FOREST] = Option::Bool("Start with Minuet of Forest", "gRandomizeStartingMinuetOfForest");
    mOptions[RSK_STARTING_BOLERO_OF_FIRE] = Option::Bool("Start with Bolero of Fire", "gRandomizeStartingBoleroOfFire");
    mOptions[RSK_STARTING_SERENADE_OF_WATER] = Option::Bool("Start with Serenade of Water", "gRandomizeStartingSerenadeOfWater");
    mOptions[RSK_STARTING_REQUIEM_OF_SPIRIT] = Option::Bool("Start with Requiem of Spirit", "gRandomizeStartingRequiemOfSpirit");
    mOptions[RSK_STARTING_NOCTURNE_OF_SHADOW] = Option::Bool("Start with Nocturne of Shadow", "gRandomizeStartingNocturneOfShadow");
    mOptions[RSK_STARTING_PRELUDE_OF_LIGHT] = Option::Bool("Start with Prelude of Light", "gRandomizeStartingPreludeOfLight");
    mOptions[RSK_STARTING_SKULLTULA_TOKEN] = Option::U8("Gold Skulltula Tokens", {NumOpts(0, 100)}, OptionCategory::Setting, "", "gRandomizeStartingSkulltulaToken", WidgetType::Slider);
    mOptions[RSK_STARTING_HEARTS] = Option::U8("Hearts", {NumOpts(1, 20)}, OptionCategory::Setting, "", "", WidgetType::Slider, 2);
    // TODO: Remainder of Starting Items
    mOptions[RSK_LOGIC_RULES] = Option::U8("Logic", {"Glitchless", "Glitched", "No Logic", "Vanilla"}, OptionCategory::Setting, "gRandomizeLogicRules", "", WidgetType::Combobox, RO_LOGIC_GLITCHLESS);
    mOptions[RSK_ALL_LOCATIONS_REACHABLE] = Option::Bool("All Locations Reachable", {"Off", "On"}, OptionCategory::Setting, "gRandomizeAllLocationsReachable", "", WidgetType::Checkbox, RO_GENERIC_ON);
    mOptions[RSK_SKULLS_SUNS_SONG] = Option::Bool("Night Skulltula's Expect Sun's Song", "gRandomizeGsExpectSunsSong");
    mOptions[RSK_DAMAGE_MULTIPLIER] = Option::U8("Damage Multiplier", {"x1/2", "x1", "x2", "x4", "x8", "x16", "OHKO"}, OptionCategory::Setting, "", "", WidgetType::Slider, RO_DAMAGE_MULTIPLIER_DEFAULT);

    mOptions[RSK_LANGUAGE] = Option::U8("Language", {"English", "German", "French"}, OptionCategory::Setting, "gLanguages", "", WidgetType::Combobox, LANGUAGE_ENG);
    // clang-format on

    mExcludeLocationsOptionsGroups.reserve(SPOILER_COLLECTION_GROUP_COUNT);

    mTrickOptions[RT_VISIBLE_COLLISION] = Option::LogicTrick(std::to_string(RT_VISIBLE_COLLISION));
    mTrickOptions[RT_GROTTOS_WITHOUT_AGONY] = Option::LogicTrick(std::to_string(RT_GROTTOS_WITHOUT_AGONY));
    mTrickOptions[RT_FEWER_TUNIC_REQUIREMENTS] = Option::LogicTrick(std::to_string(RT_FEWER_TUNIC_REQUIREMENTS));
    mTrickOptions[RT_RUSTED_SWITCHES] = Option::LogicTrick(std::to_string(RT_RUSTED_SWITCHES));
    mTrickOptions[RT_FLAMING_CHESTS] = Option::LogicTrick(std::to_string(RT_FLAMING_CHESTS));
    mTrickOptions[RT_BUNNY_HOOD_JUMPS] = Option::LogicTrick(std::to_string(RT_BUNNY_HOOD_JUMPS));
    mTrickOptions[RT_DAMAGE_BOOST_SIMPLE] = Option::LogicTrick(std::to_string(RT_DAMAGE_BOOST_SIMPLE));
    mTrickOptions[RT_HOVER_BOOST_SIMPLE] = Option::LogicTrick(std::to_string(RT_HOVER_BOOST_SIMPLE));
    mTrickOptions[RT_KF_ADULT_GS] = Option::LogicTrick(std::to_string(RT_KF_ADULT_GS));
    mTrickOptions[RT_LW_BRIDGE] = Option::LogicTrick(std::to_string(RT_LW_BRIDGE));
    mTrickOptions[RT_LW_MIDO_BACKFLIP] = Option::LogicTrick(std::to_string(RT_LW_MIDO_BACKFLIP));
    mTrickOptions[RT_LW_GS_BEAN] = Option::LogicTrick(std::to_string(RT_LW_GS_BEAN));
    mTrickOptions[RT_HC_STORMS_GS] = Option::LogicTrick(std::to_string(RT_HC_STORMS_GS));
    mTrickOptions[RT_KAK_MAN_ON_ROOF] = Option::LogicTrick(std::to_string(RT_KAK_MAN_ON_ROOF));
    mTrickOptions[RT_KAK_TOWER_GS] = Option::LogicTrick(std::to_string(RT_KAK_TOWER_GS));
    mTrickOptions[RT_KAK_ADULT_WINDMILL_POH] = Option::LogicTrick(std::to_string(RT_KAK_ADULT_WINDMILL_POH));
    mTrickOptions[RT_KAK_CHILD_WINDMILL_POH] = Option::LogicTrick(std::to_string(RT_KAK_CHILD_WINDMILL_POH));
    mTrickOptions[RT_KAK_ROOFTOP_GS] = Option::LogicTrick(std::to_string(RT_KAK_ROOFTOP_GS));
    mTrickOptions[RT_GY_POH] = Option::LogicTrick(std::to_string(RT_GY_POH));
    mTrickOptions[RT_GY_CHILD_DAMPE_RACE_POH] = Option::LogicTrick(std::to_string(RT_GY_CHILD_DAMPE_RACE_POH));
    mTrickOptions[RT_GY_SHADOW_FIRE_ARROWS] = Option::LogicTrick(std::to_string(RT_GY_SHADOW_FIRE_ARROWS));
    mTrickOptions[RT_DMT_SOIL_GS] = Option::LogicTrick(std::to_string(RT_DMT_SOIL_GS));
    mTrickOptions[RT_DMT_BOMBABLE] = Option::LogicTrick(std::to_string(RT_DMT_BOMBABLE));
    mTrickOptions[RT_DMT_HOOKSHOT_LOWER_GS] = Option::LogicTrick(std::to_string(RT_DMT_HOOKSHOT_LOWER_GS));
    mTrickOptions[RT_DMT_HOVERS_LOWER_GS] = Option::LogicTrick(std::to_string(RT_DMT_HOVERS_LOWER_GS));
    mTrickOptions[RT_DMT_BEAN_LOWER_GS] = Option::LogicTrick(std::to_string(RT_DMT_BEAN_LOWER_GS));
    mTrickOptions[RT_DMT_JS_LOWER_GS] = Option::LogicTrick(std::to_string(RT_DMT_JS_LOWER_GS));
    mTrickOptions[RT_DMT_CLIMB_HOVERS] = Option::LogicTrick(std::to_string(RT_DMT_CLIMB_HOVERS));
    mTrickOptions[RT_DMT_UPPER_GS] = Option::LogicTrick(std::to_string(RT_DMT_UPPER_GS));
    mTrickOptions[RT_DMT_BOLERO_BIGGORON] = Option::LogicTrick(std::to_string(RT_DMT_BOLERO_BIGGORON));
    mTrickOptions[RT_GC_POT] = Option::LogicTrick(std::to_string(RT_GC_POT));
    mTrickOptions[RT_GC_POT_STRENGTH] = Option::LogicTrick(std::to_string(RT_GC_POT_STRENGTH));
    mTrickOptions[RT_GC_ROLLING_STRENGTH] = Option::LogicTrick(std::to_string(RT_GC_ROLLING_STRENGTH));
    mTrickOptions[RT_GC_LEFTMOST] = Option::LogicTrick(std::to_string(RT_GC_LEFTMOST));
    mTrickOptions[RT_GC_GROTTO] = Option::LogicTrick(std::to_string(RT_GC_GROTTO));
    mTrickOptions[RT_GC_LINK_GORON_DINS] = Option::LogicTrick(std::to_string(RT_GC_LINK_GORON_DINS));
    mTrickOptions[RT_DMC_HOVER_BEAN_POH] = Option::LogicTrick(std::to_string(RT_DMC_HOVER_BEAN_POH));
    mTrickOptions[RT_DMC_BOLERO_JUMP] = Option::LogicTrick(std::to_string(RT_DMC_BOLERO_JUMP));
    mTrickOptions[RT_DMC_BOULDER_JS] = Option::LogicTrick(std::to_string(RT_DMC_BOULDER_JS));
    mTrickOptions[RT_DMC_BOULDER_SKIP] = Option::LogicTrick(std::to_string(RT_DMC_BOULDER_SKIP));
    mTrickOptions[RT_ZR_LOWER] = Option::LogicTrick(std::to_string(RT_ZR_LOWER));
    mTrickOptions[RT_ZR_UPPER] = Option::LogicTrick(std::to_string(RT_ZR_UPPER));
    mTrickOptions[RT_ZR_HOVERS] = Option::LogicTrick(std::to_string(RT_ZR_HOVERS));
    mTrickOptions[RT_ZR_CUCCO] = Option::LogicTrick(std::to_string(RT_ZR_CUCCO));
    mTrickOptions[RT_ZD_KING_ZORA_SKIP] = Option::LogicTrick(std::to_string(RT_ZD_KING_ZORA_SKIP));
    mTrickOptions[RT_ZD_GS] = Option::LogicTrick(std::to_string(RT_ZD_GS));
    mTrickOptions[RT_LH_LAB_WALL_GS] = Option::LogicTrick(std::to_string(RT_LH_LAB_WALL_GS));
    mTrickOptions[RT_LH_LAB_DIVING] = Option::LogicTrick(std::to_string(RT_LH_LAB_DIVING));
    mTrickOptions[RT_LH_WATER_HOOKSHOT] = Option::LogicTrick(std::to_string(RT_LH_WATER_HOOKSHOT));
    mTrickOptions[RT_GV_CRATE_HOVERS] = Option::LogicTrick(std::to_string(RT_GV_CRATE_HOVERS));
    mTrickOptions[RT_GF_KITCHEN] = Option::LogicTrick(std::to_string(RT_GF_KITCHEN));
    mTrickOptions[RT_GF_JUMP] = Option::LogicTrick(std::to_string(RT_GF_JUMP));
    mTrickOptions[RT_HW_BUNNY_CROSSING] = Option::LogicTrick(std::to_string(RT_HW_BUNNY_CROSSING));
    mTrickOptions[RT_HW_CROSSING] = Option::LogicTrick(std::to_string(RT_HW_CROSSING));
    mTrickOptions[RT_LENS_HW] = Option::LogicTrick(std::to_string(RT_LENS_HW));
    mTrickOptions[RT_HW_REVERSE] = Option::LogicTrick(std::to_string(RT_HW_REVERSE));
    mTrickOptions[RT_COLOSSUS_GS] = Option::LogicTrick(std::to_string(RT_COLOSSUS_GS));
    mTrickOptions[RT_DEKU_BASEMENT_GS] = Option::LogicTrick(std::to_string(RT_DEKU_BASEMENT_GS));
    mTrickOptions[RT_DEKU_B1_SKIP] = Option::LogicTrick(std::to_string(RT_DEKU_B1_SKIP));
    mTrickOptions[RT_DEKU_B1_BOW_WEBS] = Option::LogicTrick(std::to_string(RT_DEKU_B1_BOW_WEBS));
    mTrickOptions[RT_DEKU_MQ_COMPASS_GS] = Option::LogicTrick(std::to_string(RT_DEKU_MQ_COMPASS_GS));
    mTrickOptions[RT_DEKU_MQ_LOG] = Option::LogicTrick(std::to_string(RT_DEKU_MQ_LOG));
    mTrickOptions[RT_DC_SCARECROW_GS] = Option::LogicTrick(std::to_string(RT_DC_SCARECROW_GS));
    mTrickOptions[RT_DC_VINES_GS] = Option::LogicTrick(std::to_string(RT_DC_VINES_GS));
    mTrickOptions[RT_DC_STAIRCASE] = Option::LogicTrick(std::to_string(RT_DC_STAIRCASE));
    mTrickOptions[RT_DC_SLINGSHOT_SKIP] = Option::LogicTrick(std::to_string(RT_DC_SLINGSHOT_SKIP));
    mTrickOptions[RT_DC_SCRUB_ROOM] = Option::LogicTrick(std::to_string(RT_DC_SCRUB_ROOM));
    mTrickOptions[RT_DC_JUMP] = Option::LogicTrick(std::to_string(RT_DC_JUMP));
    mTrickOptions[RT_DC_HAMMER_FLOOR] = Option::LogicTrick(std::to_string(RT_DC_HAMMER_FLOOR));
    mTrickOptions[RT_DC_MQ_CHILD_BOMBS] = Option::LogicTrick(std::to_string(RT_DC_MQ_CHILD_BOMBS));
    mTrickOptions[RT_DC_MQ_CHILD_EYES] = Option::LogicTrick(std::to_string(RT_DC_MQ_CHILD_EYES));
    mTrickOptions[RT_DC_MQ_ADULT_EYES] = Option::LogicTrick(std::to_string(RT_DC_MQ_ADULT_EYES));
    mTrickOptions[RT_JABU_ALCOVE_JUMP_DIVE] = Option::LogicTrick(std::to_string(RT_JABU_ALCOVE_JUMP_DIVE));
    mTrickOptions[RT_JABU_BOSS_HOVER] = Option::LogicTrick(std::to_string(RT_JABU_BOSS_HOVER));
    mTrickOptions[RT_JABU_NEAR_BOSS_RANGED] = Option::LogicTrick(std::to_string(RT_JABU_NEAR_BOSS_RANGED));
    mTrickOptions[RT_JABU_NEAR_BOSS_EXPLOSIVES] = Option::LogicTrick(std::to_string(RT_JABU_NEAR_BOSS_EXPLOSIVES));
    mTrickOptions[RT_LENS_JABU_MQ] = Option::LogicTrick(std::to_string(RT_LENS_JABU_MQ));
    mTrickOptions[RT_JABU_MQ_RANG_JUMP] = Option::LogicTrick(std::to_string(RT_JABU_MQ_RANG_JUMP));
    mTrickOptions[RT_JABU_MQ_SOT_GS] = Option::LogicTrick(std::to_string(RT_JABU_MQ_SOT_GS));
    mTrickOptions[RT_LENS_BOTW] = Option::LogicTrick(std::to_string(RT_LENS_BOTW));
    mTrickOptions[RT_BOTW_CHILD_DEADHAND] = Option::LogicTrick(std::to_string(RT_BOTW_CHILD_DEADHAND));
    mTrickOptions[RT_BOTW_BASEMENT] = Option::LogicTrick(std::to_string(RT_BOTW_BASEMENT));
    mTrickOptions[RT_BOTW_MQ_PITS] = Option::LogicTrick(std::to_string(RT_BOTW_MQ_PITS));
    mTrickOptions[RT_BOTW_MQ_DEADHAND_KEY] = Option::LogicTrick(std::to_string(RT_BOTW_MQ_DEADHAND_KEY));
    mTrickOptions[RT_FOREST_FIRST_GS] = Option::LogicTrick(std::to_string(RT_FOREST_FIRST_GS));
    mTrickOptions[RT_FOREST_OUTDOORS_EAST_GS] = Option::LogicTrick(std::to_string(RT_FOREST_OUTDOORS_EAST_GS));
    mTrickOptions[RT_FOREST_VINES] = Option::LogicTrick(std::to_string(RT_FOREST_VINES));
    mTrickOptions[RT_FOREST_OUTDOORS_LEDGE] = Option::LogicTrick(std::to_string(RT_FOREST_OUTDOORS_LEDGE));
    mTrickOptions[RT_FOREST_DOORFRAME] = Option::LogicTrick(std::to_string(RT_FOREST_DOORFRAME));
    mTrickOptions[RT_FOREST_OUTSIDE_BACKDOOR] = Option::LogicTrick(std::to_string(RT_FOREST_OUTSIDE_BACKDOOR));
    mTrickOptions[RT_FOREST_MQ_WELL_SWIM] = Option::LogicTrick(std::to_string(RT_FOREST_MQ_WELL_SWIM));
    mTrickOptions[RT_FOREST_MQ_BLOCK_PUZZLE] = Option::LogicTrick(std::to_string(RT_FOREST_MQ_BLOCK_PUZZLE));
    mTrickOptions[RT_FOREST_MQ_JS_HALLWAY_SWITCH] = Option::LogicTrick(std::to_string(RT_FOREST_MQ_JS_HALLWAY_SWITCH));
    mTrickOptions[RT_FOREST_MQ_HOOKSHOT_HALLWAY_SWITCH] = Option::LogicTrick(std::to_string(RT_FOREST_MQ_HOOKSHOT_HALLWAY_SWITCH));
    mTrickOptions[RT_FOREST_MQ_RANG_HALLWAY_SWITCH] = Option::LogicTrick(std::to_string(RT_FOREST_MQ_RANG_HALLWAY_SWITCH));
    mTrickOptions[RT_FIRE_BOSS_DOOR_JUMP] = Option::LogicTrick(std::to_string(RT_FIRE_BOSS_DOOR_JUMP));
    mTrickOptions[RT_FIRE_SOT] = Option::LogicTrick(std::to_string(RT_FIRE_SOT));
    mTrickOptions[RT_FIRE_STRENGTH] = Option::LogicTrick(std::to_string(RT_FIRE_STRENGTH));
    mTrickOptions[RT_FIRE_SCARECROW] = Option::LogicTrick(std::to_string(RT_FIRE_SCARECROW));
    mTrickOptions[RT_FIRE_FLAME_MAZE] = Option::LogicTrick(std::to_string(RT_FIRE_FLAME_MAZE));
    mTrickOptions[RT_FIRE_MQ_NEAR_BOSS] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_NEAR_BOSS));
    mTrickOptions[RT_FIRE_MQ_BLOCKED_CHEST] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_BLOCKED_CHEST));
    mTrickOptions[RT_FIRE_MQ_BK_CHEST] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_BK_CHEST));
    mTrickOptions[RT_FIRE_MQ_CLIMB] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_CLIMB));
    mTrickOptions[RT_FIRE_MQ_MAZE_SIDE_ROOM] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_MAZE_SIDE_ROOM));
    mTrickOptions[RT_FIRE_MQ_MAZE_HOVERS] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_MAZE_HOVERS));
    mTrickOptions[RT_FIRE_MQ_MAZE_JUMP] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_MAZE_JUMP));
    mTrickOptions[RT_FIRE_MQ_ABOVE_MAZE_GS] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_ABOVE_MAZE_GS));
    mTrickOptions[RT_FIRE_MQ_FLAME_MAZE] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_FLAME_MAZE));
    mTrickOptions[RT_WATER_LONGSHOT_TORCH] = Option::LogicTrick(std::to_string(RT_WATER_LONGSHOT_TORCH));
    mTrickOptions[RT_WATER_CRACKED_WALL_HOVERS] = Option::LogicTrick(std::to_string(RT_WATER_CRACKED_WALL_HOVERS));
    mTrickOptions[RT_WATER_CRACKED_WALL] = Option::LogicTrick(std::to_string(RT_WATER_CRACKED_WALL));
    mTrickOptions[RT_WATER_BK_REGION] = Option::LogicTrick(std::to_string(RT_WATER_BK_REGION));
    mTrickOptions[RT_WATER_NORTH_BASEMENT_LEDGE_JUMP] = Option::LogicTrick(std::to_string(RT_WATER_NORTH_BASEMENT_LEDGE_JUMP));
    mTrickOptions[RT_WATER_BK_JUMP_DIVE] = Option::LogicTrick(std::to_string(RT_WATER_BK_JUMP_DIVE));
    mTrickOptions[RT_WATER_FW_CENTRAL_GS] = Option::LogicTrick(std::to_string(RT_WATER_FW_CENTRAL_GS));
    mTrickOptions[RT_WATER_IRONS_CENTRAL_GS] = Option::LogicTrick(std::to_string(RT_WATER_IRONS_CENTRAL_GS));
    mTrickOptions[RT_WATER_CENTRAL_BOW] = Option::LogicTrick(std::to_string(RT_WATER_CENTRAL_BOW));
    mTrickOptions[RT_WATER_HOOKSHOT_FALLING_PLATFORM_GS] = Option::LogicTrick(std::to_string(RT_WATER_HOOKSHOT_FALLING_PLATFORM_GS));
    mTrickOptions[RT_WATER_RANG_FALLING_PLATFORM_GS] = Option::LogicTrick(std::to_string(RT_WATER_RANG_FALLING_PLATFORM_GS));
    mTrickOptions[RT_WATER_RIVER_GS] = Option::LogicTrick(std::to_string(RT_WATER_RIVER_GS));
    mTrickOptions[RT_WATER_DRAGON_JUMP_DIVE] = Option::LogicTrick(std::to_string(RT_WATER_DRAGON_JUMP_DIVE));
    mTrickOptions[RT_WATER_ADULT_DRAGON] = Option::LogicTrick(std::to_string(RT_WATER_ADULT_DRAGON));
    mTrickOptions[RT_WATER_CHILD_DRAGON] = Option::LogicTrick(std::to_string(RT_WATER_CHILD_DRAGON));
    mTrickOptions[RT_WATER_MQ_CENTRAL_PILLAR] = Option::LogicTrick(std::to_string(RT_WATER_MQ_CENTRAL_PILLAR));
    mTrickOptions[RT_WATER_MQ_LOCKED_GS] = Option::LogicTrick(std::to_string(RT_WATER_MQ_LOCKED_GS));
    mTrickOptions[RT_LENS_SHADOW] = Option::LogicTrick(std::to_string(RT_LENS_SHADOW));
    mTrickOptions[RT_LENS_SHADOW_PLATFORM] = Option::LogicTrick(std::to_string(RT_LENS_SHADOW_PLATFORM));
    mTrickOptions[RT_LENS_BONGO] = Option::LogicTrick(std::to_string(RT_LENS_BONGO));
    mTrickOptions[RT_SHADOW_UMBRELLA] = Option::LogicTrick(std::to_string(RT_SHADOW_UMBRELLA));
    mTrickOptions[RT_SHADOW_UMBRELLA_GS] = Option::LogicTrick(std::to_string(RT_SHADOW_UMBRELLA_GS));
    mTrickOptions[RT_SHADOW_FREESTANDING_KEY] = Option::LogicTrick(std::to_string(RT_SHADOW_FREESTANDING_KEY));
    mTrickOptions[RT_SHADOW_STATUE] = Option::LogicTrick(std::to_string(RT_SHADOW_STATUE));
    mTrickOptions[RT_SHADOW_BONGO] = Option::LogicTrick(std::to_string(RT_SHADOW_BONGO));
    mTrickOptions[RT_LENS_SHADOW_MQ] = Option::LogicTrick(std::to_string(RT_LENS_SHADOW_MQ));
    mTrickOptions[RT_LENS_SHADOW_MQ_INVISIBLE_BLADES] = Option::LogicTrick(std::to_string(RT_LENS_SHADOW_MQ_INVISIBLE_BLADES));
    mTrickOptions[RT_LENS_SHADOW_MQ_PLATFORM] = Option::LogicTrick(std::to_string(RT_LENS_SHADOW_MQ_PLATFORM));
    mTrickOptions[RT_LENS_SHADOW_MQ_DEADHAND] = Option::LogicTrick(std::to_string(RT_LENS_SHADOW_MQ_DEADHAND));
    mTrickOptions[RT_SHADOW_MQ_GAP] = Option::LogicTrick(std::to_string(RT_SHADOW_MQ_GAP));
    mTrickOptions[RT_SHADOW_MQ_INVISIBLE_BLADES] = Option::LogicTrick(std::to_string(RT_SHADOW_MQ_INVISIBLE_BLADES));
    mTrickOptions[RT_SHADOW_MQ_HUGE_PIT] = Option::LogicTrick(std::to_string(RT_SHADOW_MQ_HUGE_PIT));
    mTrickOptions[RT_SHADOW_MQ_WINDY_WALKWAY] = Option::LogicTrick(std::to_string(RT_SHADOW_MQ_WINDY_WALKWAY));
    mTrickOptions[RT_LENS_SPIRIT] = Option::LogicTrick(std::to_string(RT_LENS_SPIRIT));
    mTrickOptions[RT_SPIRIT_CHILD_CHU] = Option::LogicTrick(std::to_string(RT_SPIRIT_CHILD_CHU));
    mTrickOptions[RT_SPIRIT_LOBBY_GS] = Option::LogicTrick(std::to_string(RT_SPIRIT_LOBBY_GS));
    mTrickOptions[RT_SPIRIT_LOWER_ADULT_SWITCH] = Option::LogicTrick(std::to_string(RT_SPIRIT_LOWER_ADULT_SWITCH));
    mTrickOptions[RT_SPIRIT_LOBBY_JUMP] = Option::LogicTrick(std::to_string(RT_SPIRIT_LOBBY_JUMP));
    mTrickOptions[RT_SPIRIT_PLATFORM_HOOKSHOT] = Option::LogicTrick(std::to_string(RT_SPIRIT_PLATFORM_HOOKSHOT));
    mTrickOptions[RT_SPIRIT_MAP_CHEST] = Option::LogicTrick(std::to_string(RT_SPIRIT_MAP_CHEST));
    mTrickOptions[RT_SPIRIT_SUN_CHEST] = Option::LogicTrick(std::to_string(RT_SPIRIT_SUN_CHEST));
    mTrickOptions[RT_SPIRIT_WALL] = Option::LogicTrick(std::to_string(RT_SPIRIT_WALL));
    mTrickOptions[RT_LENS_SPIRIT_MQ] = Option::LogicTrick(std::to_string(RT_LENS_SPIRIT_MQ));
    mTrickOptions[RT_SPIRIT_MQ_SUN_BLOCK_SOT] = Option::LogicTrick(std::to_string(RT_SPIRIT_MQ_SUN_BLOCK_SOT));
    mTrickOptions[RT_SPIRIT_MQ_SUN_BLOCK_GS] = Option::LogicTrick(std::to_string(RT_SPIRIT_MQ_SUN_BLOCK_GS));
    mTrickOptions[RT_SPIRIT_MQ_LOWER_ADULT] = Option::LogicTrick(std::to_string(RT_SPIRIT_MQ_LOWER_ADULT));
    mTrickOptions[RT_SPIRIT_MQ_FROZEN_EYE] = Option::LogicTrick(std::to_string(RT_SPIRIT_MQ_FROZEN_EYE));
    mTrickOptions[RT_ICE_BLOCK_GS] = Option::LogicTrick(std::to_string(RT_ICE_BLOCK_GS));
    mTrickOptions[RT_ICE_MQ_RED_ICE_GS] = Option::LogicTrick(std::to_string(RT_ICE_MQ_RED_ICE_GS));
    mTrickOptions[RT_ICE_MQ_SCARECROW] = Option::LogicTrick(std::to_string(RT_ICE_MQ_SCARECROW));
    mTrickOptions[RT_LENS_GTG] = Option::LogicTrick(std::to_string(RT_LENS_GTG));
    mTrickOptions[RT_GTG_WITHOUT_HOOKSHOT] = Option::LogicTrick(std::to_string(RT_GTG_WITHOUT_HOOKSHOT));
    mTrickOptions[RT_GTG_FAKE_WALL] = Option::LogicTrick(std::to_string(RT_GTG_FAKE_WALL));
    mTrickOptions[RT_LENS_GTG_MQ] = Option::LogicTrick(std::to_string(RT_LENS_GTG_MQ));
    mTrickOptions[RT_GTG_MQ_WITH_HOOKSHOT] = Option::LogicTrick(std::to_string(RT_GTG_MQ_WITH_HOOKSHOT));
    mTrickOptions[RT_GTG_MQ_WIHTOUT_HOOKSHOT] = Option::LogicTrick(std::to_string(RT_GTG_MQ_WIHTOUT_HOOKSHOT));
    mTrickOptions[RT_LENS_GANON] = Option::LogicTrick(std::to_string(RT_LENS_GANON));
    mTrickOptions[RT_GANON_SPIRIT_TRIAL_HOOKSHOT] = Option::LogicTrick(std::to_string(RT_GANON_SPIRIT_TRIAL_HOOKSHOT));
    mTrickOptions[RT_LENS_GANON_MQ] = Option::LogicTrick(std::to_string(RT_LENS_GANON_MQ));
    mTrickOptions[RT_GANON_MQ_FIRE_TRIAL] = Option::LogicTrick(std::to_string(RT_GANON_MQ_FIRE_TRIAL));
    mTrickOptions[RT_GANON_MQ_SHADOW_TRIAL] = Option::LogicTrick(std::to_string(RT_GANON_MQ_SHADOW_TRIAL));
    mTrickOptions[RT_GANON_MQ_LIGHT_TRIAL] = Option::LogicTrick(std::to_string(RT_GANON_MQ_LIGHT_TRIAL));

    mOptionGroups[RSG_LOGIC] = OptionGroup::SubGroup("Logic Options", {
        &mOptions[RSK_LOGIC_RULES],
        &mOptions[RSK_ALL_LOCATIONS_REACHABLE],
        &mOptions[RSK_SKULLS_SUNS_SONG],
    });
    // TODO: Exclude Locations Menus
    mOptionGroups[RSG_TRICKS] = OptionGroup::SubGroup("Logical Tricks", {
        &mTrickOptions[RT_VISIBLE_COLLISION],
        &mTrickOptions[RT_GROTTOS_WITHOUT_AGONY],
        &mTrickOptions[RT_FEWER_TUNIC_REQUIREMENTS],
        &mTrickOptions[RT_RUSTED_SWITCHES],
        &mTrickOptions[RT_FLAMING_CHESTS],
        &mTrickOptions[RT_BUNNY_HOOD_JUMPS],
        &mTrickOptions[RT_DAMAGE_BOOST_SIMPLE],
        &mTrickOptions[RT_HOVER_BOOST_SIMPLE],
        &mTrickOptions[RT_KF_ADULT_GS],
        &mTrickOptions[RT_LW_BRIDGE],
        &mTrickOptions[RT_LW_MIDO_BACKFLIP],
        &mTrickOptions[RT_LW_GS_BEAN],
        &mTrickOptions[RT_HC_STORMS_GS],
        &mTrickOptions[RT_KAK_MAN_ON_ROOF],
        &mTrickOptions[RT_KAK_TOWER_GS],
        &mTrickOptions[RT_KAK_ADULT_WINDMILL_POH],
        &mTrickOptions[RT_KAK_CHILD_WINDMILL_POH],
        &mTrickOptions[RT_KAK_ROOFTOP_GS],
        &mTrickOptions[RT_GY_POH],
        &mTrickOptions[RT_GY_CHILD_DAMPE_RACE_POH],
        &mTrickOptions[RT_GY_SHADOW_FIRE_ARROWS],
        &mTrickOptions[RT_DMT_SOIL_GS],
        &mTrickOptions[RT_DMT_BOMBABLE],
        &mTrickOptions[RT_DMT_HOOKSHOT_LOWER_GS],
        &mTrickOptions[RT_DMT_HOVERS_LOWER_GS],
        &mTrickOptions[RT_DMT_BEAN_LOWER_GS],
        &mTrickOptions[RT_DMT_JS_LOWER_GS],
        &mTrickOptions[RT_DMT_CLIMB_HOVERS],
        &mTrickOptions[RT_DMT_UPPER_GS],
        &mTrickOptions[RT_DMT_BOLERO_BIGGORON],
        &mTrickOptions[RT_GC_POT],
        &mTrickOptions[RT_GC_POT_STRENGTH],
        &mTrickOptions[RT_GC_ROLLING_STRENGTH],
        &mTrickOptions[RT_GC_LEFTMOST],
        &mTrickOptions[RT_GC_GROTTO],
        &mTrickOptions[RT_GC_LINK_GORON_DINS],
        &mTrickOptions[RT_DMC_HOVER_BEAN_POH],
        &mTrickOptions[RT_DMC_BOLERO_JUMP],
        &mTrickOptions[RT_DMC_BOULDER_JS],
        &mTrickOptions[RT_DMC_BOULDER_SKIP],
        &mTrickOptions[RT_ZR_LOWER],
        &mTrickOptions[RT_ZR_UPPER],
        &mTrickOptions[RT_ZR_HOVERS],
        &mTrickOptions[RT_ZR_CUCCO],
        &mTrickOptions[RT_ZD_KING_ZORA_SKIP],
        &mTrickOptions[RT_ZD_GS],
        &mTrickOptions[RT_LH_LAB_WALL_GS],
        &mTrickOptions[RT_LH_LAB_DIVING],
        &mTrickOptions[RT_LH_WATER_HOOKSHOT],
        &mTrickOptions[RT_GV_CRATE_HOVERS],
        &mTrickOptions[RT_GF_KITCHEN],
        &mTrickOptions[RT_GF_JUMP],
        &mTrickOptions[RT_HW_BUNNY_CROSSING],
        &mTrickOptions[RT_HW_CROSSING],
        &mTrickOptions[RT_LENS_HW],
        &mTrickOptions[RT_HW_REVERSE],
        &mTrickOptions[RT_COLOSSUS_GS],
        &mTrickOptions[RT_DEKU_BASEMENT_GS],
        &mTrickOptions[RT_DEKU_B1_SKIP],
        &mTrickOptions[RT_DEKU_B1_BOW_WEBS],
        &mTrickOptions[RT_DEKU_MQ_COMPASS_GS],
        &mTrickOptions[RT_DEKU_MQ_LOG],
        &mTrickOptions[RT_DC_SCARECROW_GS],
        &mTrickOptions[RT_DC_VINES_GS],
        &mTrickOptions[RT_DC_STAIRCASE],
        &mTrickOptions[RT_DC_SLINGSHOT_SKIP],
        &mTrickOptions[RT_DC_SCRUB_ROOM],
        &mTrickOptions[RT_DC_JUMP],
        &mTrickOptions[RT_DC_HAMMER_FLOOR],
        &mTrickOptions[RT_DC_MQ_CHILD_BOMBS],
        &mTrickOptions[RT_DC_MQ_CHILD_EYES],
        &mTrickOptions[RT_DC_MQ_ADULT_EYES],
        &mTrickOptions[RT_JABU_ALCOVE_JUMP_DIVE],
        &mTrickOptions[RT_JABU_BOSS_HOVER],
        &mTrickOptions[RT_JABU_NEAR_BOSS_RANGED],
        &mTrickOptions[RT_JABU_NEAR_BOSS_EXPLOSIVES],
        &mTrickOptions[RT_LENS_JABU_MQ],
        &mTrickOptions[RT_JABU_MQ_RANG_JUMP],
        &mTrickOptions[RT_JABU_MQ_SOT_GS],
        &mTrickOptions[RT_LENS_BOTW],
        &mTrickOptions[RT_BOTW_CHILD_DEADHAND],
        &mTrickOptions[RT_BOTW_BASEMENT],
        &mTrickOptions[RT_BOTW_MQ_PITS],
        &mTrickOptions[RT_BOTW_MQ_DEADHAND_KEY],
        &mTrickOptions[RT_FOREST_FIRST_GS],
        &mTrickOptions[RT_FOREST_OUTDOORS_EAST_GS],
        &mTrickOptions[RT_FOREST_VINES],
        &mTrickOptions[RT_FOREST_OUTDOORS_LEDGE],
        &mTrickOptions[RT_FOREST_DOORFRAME],
        &mTrickOptions[RT_FOREST_OUTSIDE_BACKDOOR],
        &mTrickOptions[RT_FOREST_MQ_WELL_SWIM],
        &mTrickOptions[RT_FOREST_MQ_BLOCK_PUZZLE],
        &mTrickOptions[RT_FOREST_MQ_JS_HALLWAY_SWITCH],
        &mTrickOptions[RT_FOREST_MQ_HOOKSHOT_HALLWAY_SWITCH],
        &mTrickOptions[RT_FOREST_MQ_RANG_HALLWAY_SWITCH],
        &mTrickOptions[RT_FIRE_BOSS_DOOR_JUMP],
        &mTrickOptions[RT_FIRE_SOT],
        &mTrickOptions[RT_FIRE_STRENGTH],
        &mTrickOptions[RT_FIRE_SCARECROW],
        &mTrickOptions[RT_FIRE_FLAME_MAZE],
        &mTrickOptions[RT_FIRE_MQ_NEAR_BOSS],
        &mTrickOptions[RT_FIRE_MQ_BLOCKED_CHEST],
        &mTrickOptions[RT_FIRE_MQ_BK_CHEST],
        &mTrickOptions[RT_FIRE_MQ_CLIMB],
        &mTrickOptions[RT_FIRE_MQ_MAZE_SIDE_ROOM],
        &mTrickOptions[RT_FIRE_MQ_MAZE_HOVERS],
        &mTrickOptions[RT_FIRE_MQ_MAZE_JUMP],
        &mTrickOptions[RT_FIRE_MQ_ABOVE_MAZE_GS],
        &mTrickOptions[RT_FIRE_MQ_FLAME_MAZE],
        &mTrickOptions[RT_WATER_LONGSHOT_TORCH],
        &mTrickOptions[RT_WATER_CRACKED_WALL_HOVERS],
        &mTrickOptions[RT_WATER_CRACKED_WALL],
        &mTrickOptions[RT_WATER_BK_REGION],
        &mTrickOptions[RT_WATER_NORTH_BASEMENT_LEDGE_JUMP],
        &mTrickOptions[RT_WATER_BK_JUMP_DIVE],
        &mTrickOptions[RT_WATER_FW_CENTRAL_GS],
        &mTrickOptions[RT_WATER_IRONS_CENTRAL_GS],
        &mTrickOptions[RT_WATER_CENTRAL_BOW],
        &mTrickOptions[RT_WATER_HOOKSHOT_FALLING_PLATFORM_GS],
        &mTrickOptions[RT_WATER_RANG_FALLING_PLATFORM_GS],
        &mTrickOptions[RT_WATER_RIVER_GS],
        &mTrickOptions[RT_WATER_DRAGON_JUMP_DIVE],
        &mTrickOptions[RT_WATER_ADULT_DRAGON],
        &mTrickOptions[RT_WATER_CHILD_DRAGON],
        &mTrickOptions[RT_WATER_MQ_CENTRAL_PILLAR],
        &mTrickOptions[RT_WATER_MQ_LOCKED_GS],
        &mTrickOptions[RT_LENS_SHADOW],
        &mTrickOptions[RT_LENS_SHADOW_PLATFORM],
        &mTrickOptions[RT_LENS_BONGO],
        &mTrickOptions[RT_SHADOW_UMBRELLA],
        &mTrickOptions[RT_SHADOW_UMBRELLA_GS],
        &mTrickOptions[RT_SHADOW_FREESTANDING_KEY],
        &mTrickOptions[RT_SHADOW_STATUE],
        &mTrickOptions[RT_SHADOW_BONGO],
        &mTrickOptions[RT_LENS_SHADOW_MQ],
        &mTrickOptions[RT_LENS_SHADOW_MQ_INVISIBLE_BLADES],
        &mTrickOptions[RT_LENS_SHADOW_MQ_PLATFORM],
        &mTrickOptions[RT_LENS_SHADOW_MQ_DEADHAND],
        &mTrickOptions[RT_SHADOW_MQ_GAP],
        &mTrickOptions[RT_SHADOW_MQ_INVISIBLE_BLADES],
        &mTrickOptions[RT_SHADOW_MQ_HUGE_PIT],
        &mTrickOptions[RT_SHADOW_MQ_WINDY_WALKWAY],
        &mTrickOptions[RT_LENS_SPIRIT],
        &mTrickOptions[RT_SPIRIT_CHILD_CHU],
        &mTrickOptions[RT_SPIRIT_LOBBY_GS],
        &mTrickOptions[RT_SPIRIT_LOWER_ADULT_SWITCH],
        &mTrickOptions[RT_SPIRIT_LOBBY_JUMP],
        &mTrickOptions[RT_SPIRIT_PLATFORM_HOOKSHOT],
        &mTrickOptions[RT_SPIRIT_MAP_CHEST],
        &mTrickOptions[RT_SPIRIT_SUN_CHEST],
        &mTrickOptions[RT_SPIRIT_WALL],
        &mTrickOptions[RT_LENS_SPIRIT_MQ],
        &mTrickOptions[RT_SPIRIT_MQ_SUN_BLOCK_SOT],
        &mTrickOptions[RT_SPIRIT_MQ_SUN_BLOCK_GS],
        &mTrickOptions[RT_SPIRIT_MQ_LOWER_ADULT],
        &mTrickOptions[RT_SPIRIT_MQ_FROZEN_EYE],
        &mTrickOptions[RT_ICE_BLOCK_GS],
        &mTrickOptions[RT_ICE_MQ_RED_ICE_GS],
        &mTrickOptions[RT_ICE_MQ_SCARECROW],
        &mTrickOptions[RT_LENS_GTG],
        &mTrickOptions[RT_GTG_WITHOUT_HOOKSHOT],
        &mTrickOptions[RT_GTG_FAKE_WALL],
        &mTrickOptions[RT_LENS_GTG_MQ],
        &mTrickOptions[RT_GTG_MQ_WITH_HOOKSHOT],
        &mTrickOptions[RT_GTG_MQ_WIHTOUT_HOOKSHOT],
        &mTrickOptions[RT_LENS_GANON],
        &mTrickOptions[RT_GANON_SPIRIT_TRIAL_HOOKSHOT],
        &mTrickOptions[RT_LENS_GANON_MQ],
        &mTrickOptions[RT_GANON_MQ_FIRE_TRIAL],
        &mTrickOptions[RT_GANON_MQ_SHADOW_TRIAL],
        &mTrickOptions[RT_GANON_MQ_LIGHT_TRIAL],
    }, false);
    // TODO: Glitches
    mOptionGroups[RSG_OPEN] = OptionGroup("Open Settings", {
        &mOptions[RSK_FOREST],
        &mOptions[RSK_KAK_GATE],
        &mOptions[RSK_DOOR_OF_TIME],
        &mOptions[RSK_ZORAS_FOUNTAIN],
        &mOptions[RSK_GERUDO_FORTRESS],
        &mOptions[RSK_RAINBOW_BRIDGE],
        &mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT],
        &mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT],
        &mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT],
        &mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT],
        &mOptions[RSK_RAINBOW_BRIDGE_TOKEN_COUNT],
        &mOptions[RSK_BRIDGE_OPTIONS],
        &mOptions[RSK_GANONS_TRIALS],
        &mOptions[RSK_TRIAL_COUNT],
    });
    mOptionGroups[RSG_WORLD] = OptionGroup("World Settings", {
        &mOptions[RSK_STARTING_AGE],
        &mOptions[RSK_SHUFFLE_ENTRANCES],
        &mOptions[RSK_SHUFFLE_DUNGEON_ENTRANCES],
        &mOptions[RSK_SHUFFLE_BOSS_ENTRANCES],
        &mOptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES],
        &mOptions[RSK_SHUFFLE_INTERIOR_ENTRANCES],
        &mOptions[RSK_SHUFFLE_GROTTO_ENTRANCES],
        &mOptions[RSK_SHUFFLE_OWL_DROPS],
        &mOptions[RSK_SHUFFLE_WARP_SONGS],
        &mOptions[RSK_SHUFFLE_OVERWORLD_SPAWNS],
        &mOptions[RSK_MIXED_ENTRANCE_POOLS],
        &mOptions[RSK_MIX_DUNGEON_ENTRANCES],
        &mOptions[RSK_MIX_OVERWORLD_ENTRANCES],
        &mOptions[RSK_MIX_INTERIOR_ENTRANCES],
        &mOptions[RSK_MIX_GROTTO_ENTRANCES],
        &mOptions[RSK_DECOUPLED_ENTRANCES],
        &mOptions[RSK_BOMBCHUS_IN_LOGIC],
        &mOptions[RSK_ENABLE_BOMBCHU_DROPS],
        &mOptions[RSK_TRIFORCE_HUNT],
        &mOptions[RSK_TRIFORCE_HUNT_PIECES_TOTAL],
        &mOptions[RSK_TRIFORCE_HUNT_PIECES_REQUIRED],
        &mOptions[RSK_MQ_DUNGEON_RANDOM],
        &mOptions[RSK_MQ_DUNGEON_COUNT],
        &mOptions[RSK_MQ_DUNGEON_SET],
    });
    mOptionGroups[RSG_SHUFFLE_DUNGEON_QUESTS] = OptionGroup::SubGroup("Shuffle Dungeon Quest", {
        &mOptions[RSK_MQ_DEKU_TREE],
        &mOptions[RSK_MQ_DODONGOS_CAVERN],
        &mOptions[RSK_MQ_JABU_JABU],
        &mOptions[RSK_MQ_FOREST_TEMPLE],
        &mOptions[RSK_MQ_FIRE_TEMPLE],
        &mOptions[RSK_MQ_WATER_TEMPLE],
        &mOptions[RSK_MQ_SPIRIT_TEMPLE],
        &mOptions[RSK_MQ_SHADOW_TEMPLE],
        &mOptions[RSK_MQ_BOTTOM_OF_THE_WELL],
        &mOptions[RSK_MQ_ICE_CAVERN],
        &mOptions[RSK_MQ_GTG],
        &mOptions[RSK_MQ_GANONS_CASTLE]
    });
    mOptionGroups[RSG_SHUFFLE] = OptionGroup("Shuffle Settings", {
        &mOptions[RSK_SHUFFLE_DUNGEON_REWARDS],
        &mOptions[RSK_LINKS_POCKET],
        &mOptions[RSK_SHUFFLE_SONGS],
        &mOptions[RSK_SHOPSANITY],
        &mOptions[RSK_SHOPSANITY_PRICES],
        &mOptions[RSK_SHOPSANITY_PRICES_AFFORDABLE],
        &mOptions[RSK_SHUFFLE_TOKENS],
        &mOptions[RSK_SHUFFLE_SCRUBS],
        &mOptions[RSK_SHUFFLE_COWS],
        &mOptions[RSK_SHUFFLE_KOKIRI_SWORD],
        &mOptions[RSK_SHUFFLE_OCARINA],
        &mOptions[RSK_SHUFFLE_WEIRD_EGG],
        &mOptions[RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD],
        &mOptions[RSK_SHUFFLE_MAGIC_BEANS],
        &mOptions[RSK_SHUFFLE_MERCHANTS],
        &mOptions[RSK_SHUFFLE_FROG_SONG_RUPEES],
        &mOptions[RSK_SHUFFLE_ADULT_TRADE],
        &mOptions[RSK_SHUFFLE_CHEST_MINIGAME],
        &mOptions[RSK_SHUFFLE_100_GS_REWARD],
    });
    mOptionGroups[RSG_SHUFFLE_DUNGEON_ITEMS] = OptionGroup("Shuffle Dungeon Items", {
        &mOptions[RSK_SHUFFLE_MAPANDCOMPASS],
        &mOptions[RSK_KEYSANITY],
        &mOptions[RSK_GERUDO_KEYS],
        &mOptions[RSK_BOSS_KEYSANITY],
        &mOptions[RSK_GANONS_BOSS_KEY],
        &mOptions[RSK_LACS_STONE_COUNT],
        &mOptions[RSK_LACS_MEDALLION_COUNT],
        &mOptions[RSK_LACS_DUNGEON_COUNT],
        &mOptions[RSK_LACS_REWARD_COUNT],
        &mOptions[RSK_LACS_TOKEN_COUNT],
        &mOptions[RSK_LACS_OPTIONS],
        &mOptions[RSK_KEYRINGS],
        &mOptions[RSK_KEYRINGS_RANDOM_COUNT],
        &mOptions[RSK_KEYRINGS_GERUDO_FORTRESS],
        &mOptions[RSK_KEYRINGS_FOREST_TEMPLE],
        &mOptions[RSK_KEYRINGS_FIRE_TEMPLE],
        &mOptions[RSK_KEYRINGS_WATER_TEMPLE],
        &mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE],
        &mOptions[RSK_KEYRINGS_SHADOW_TEMPLE],
        &mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL],
        &mOptions[RSK_KEYRINGS_GTG],
        &mOptions[RSK_KEYRINGS_GANONS_CASTLE],
    });
    mOptionGroups[RSG_STARTING_ITEMS] = OptionGroup::SubGroup("Items", {
        &mOptions[RSK_STARTING_OCARINA],
        &mOptions[RSK_STARTING_KOKIRI_SWORD],
        &mOptions[RSK_STARTING_DEKU_SHIELD]
    }, false);
    mOptionGroups[RSG_STARTING_SONGS] = OptionGroup::SubGroup("Ocarina Songs", {
        &mOptions[RSK_STARTING_ZELDAS_LULLABY],
        &mOptions[RSK_STARTING_EPONAS_SONG],
        &mOptions[RSK_STARTING_SARIAS_SONG],
        &mOptions[RSK_STARTING_SUNS_SONG],
        &mOptions[RSK_STARTING_SONG_OF_TIME],
        &mOptions[RSK_STARTING_SONG_OF_STORMS],
        &mOptions[RSK_STARTING_SONG_OF_TIME],
        &mOptions[RSK_STARTING_MINUET_OF_FOREST],
        &mOptions[RSK_STARTING_BOLERO_OF_FIRE],
        &mOptions[RSK_STARTING_SERENADE_OF_WATER],
        &mOptions[RSK_STARTING_REQUIEM_OF_SPIRIT],
        &mOptions[RSK_STARTING_NOCTURNE_OF_SHADOW],
        &mOptions[RSK_STARTING_PRELUDE_OF_LIGHT],
    }, false);
    mOptionGroups[RSG_STARTING_OTHER] = OptionGroup::SubGroup("Other", {
        &mOptions[RSK_STARTING_CONSUMABLES],
        &mOptions[RSK_FULL_WALLETS],
        &mOptions[RSK_STARTING_SKULLTULA_TOKEN],
    }, false);
    mOptionGroups[RSG_STARTING_INVENTORY] = OptionGroup("Starting Inventory", {
        &mOptionGroups[RSG_STARTING_ITEMS],
        &mOptionGroups[RSG_STARTING_SONGS],
        &mOptionGroups[RSG_STARTING_OTHER],
    }, OptionGroupType::DEFAULT, false);
    mOptionGroups[RSG_TIMESAVERS] = OptionGroup("Timesaver Settings", {
        &mOptions[RSK_SKIP_CHILD_STEALTH],
        &mOptions[RSK_SKIP_CHILD_ZELDA],
        &mOptions[RSK_SKIP_EPONA_RACE],
        &mOptions[RSK_SKIP_SCARECROWS_SONG],
        &mOptions[RSK_BIG_POE_COUNT],
        &mOptions[RSK_CUCCO_COUNT],
        &mOptions[RSK_COMPLETE_MASK_QUEST],
        &mOptions[RSK_ENABLE_GLITCH_CUTSCENES],
    });
    mOptionGroups[RSG_MISC] = OptionGroup("Miscellaneous Settings", {
        &mOptions[RSK_GOSSIP_STONE_HINTS],
        &mOptions[RSK_HINT_CLARITY],
        &mOptions[RSK_HINT_DISTRIBUTION],
        &mOptions[RSK_TOT_ALTAR_HINT],
        &mOptions[RSK_LIGHT_ARROWS_HINT],
        &mOptions[RSK_DAMPES_DIARY_HINT],
        &mOptions[RSK_GREG_HINT],
        &mOptions[RSK_SARIA_HINT],
        &mOptions[RSK_FROGS_HINT],
        &mOptions[RSK_WARP_SONG_HINTS],
        &mOptions[RSK_KAK_10_SKULLS_HINT],
        &mOptions[RSK_KAK_20_SKULLS_HINT],
        &mOptions[RSK_KAK_30_SKULLS_HINT],
        &mOptions[RSK_KAK_40_SKULLS_HINT],
        &mOptions[RSK_KAK_50_SKULLS_HINT],
        &mOptions[RSK_SCRUB_TEXT_HINT],
        // TODO: Compasses show Reward/WOTH, Maps show Dungeon Mode, Starting Time
        &mOptions[RSK_DAMAGE_MULTIPLIER],
        &mOptions[RSK_BLUE_FIRE_ARROWS],
        &mOptions[RSK_SUNLIGHT_ARROWS],
    });
    // TODO: For some reason this group and only this group is an ambiguous constructor call, despite
    // the initializer list only having Option* in it. For now explictly declare and initialize
    // a vector of Option* and construct the group with that.
    std::vector<Option*> itemPoolOptions = {&mOptions[RSK_ITEM_POOL], &mOptions[RSK_ICE_TRAPS]};
    mOptionGroups[RSG_ITEM_POOL] = OptionGroup("Item Pool Settings", itemPoolOptions);
    // TODO: Progressive Goron Sword, Remove Double Defense
    mOptionGroups[RSG_EXCLUDES_KOKIRI_FOREST] = OptionGroup::SubGroup("Kokiri Forest", mExcludeLocationsOptionsGroups[GROUP_KOKIRI_FOREST], false);
    mOptionGroups[RSG_EXCLUDES_LOST_WOODS] = OptionGroup::SubGroup("Lost Woods", mExcludeLocationsOptionsGroups[GROUP_LOST_WOODS], false);
    mOptionGroups[RSG_EXCLUDES_DEKU_TREE] = OptionGroup::SubGroup("Deku Tree", mExcludeLocationsOptionsGroups[GROUP_DUNGEON_DEKU_TREE], false);
    mOptionGroups[RSG_EXCLUDES_FOREST_TEMPLE] = OptionGroup::SubGroup("Forest Temple", mExcludeLocationsOptionsGroups[GROUP_DUNGEON_FOREST_TEMPLE], false);
    mOptionGroups[RSG_EXCLUDES_KAKARIKO_VILLAGE] = OptionGroup::SubGroup("Kakariko Village", mExcludeLocationsOptionsGroups[GROUP_KAKARIKO], false);
    mOptionGroups[RSG_EXCLUDES_BOTTOM_OF_THE_WELL] = OptionGroup::SubGroup("Bottom of the Well", mExcludeLocationsOptionsGroups[GROUP_DUNGEON_BOTTOM_OF_THE_WELL], false);
    mOptionGroups[RSG_EXCLUDES_SHADOW_TEMPLE] = OptionGroup::SubGroup("Shadow Temple", mExcludeLocationsOptionsGroups[GROUP_DUNGEON_SHADOW_TEMPLE], false);
    mOptionGroups[RSG_EXCLUDES_DEATH_MOUNTAIN] = OptionGroup::SubGroup("Death Mountain", mExcludeLocationsOptionsGroups[GROUP_DEATH_MOUNTAIN], false);
    mOptionGroups[RSG_EXCLUDES_GORON_CITY] = OptionGroup::SubGroup("Goron City", mExcludeLocationsOptionsGroups[GROUP_GORON_CITY], false);
    mOptionGroups[RSG_EXCLUDES_DODONGOS_CAVERN] = OptionGroup::SubGroup("Dodongo's Cavern", mExcludeLocationsOptionsGroups[GROUP_DUNGEON_DODONGOS_CAVERN], false);
    mOptionGroups[RSG_EXCLUDES_FIRE_TEMPLE] = OptionGroup::SubGroup("Fire Temple", mExcludeLocationsOptionsGroups[GROUP_DUNGEON_FIRE_TEMPLE], false);
    mOptionGroups[RSG_EXCLUDES_ZORAS_RIVER] = OptionGroup::SubGroup("Zora's River", mExcludeLocationsOptionsGroups[GROUP_ZORAS_RIVER], false);
    mOptionGroups[RSG_EXCLUDES_ZORAS_DOMAIN] = OptionGroup::SubGroup("Zora's Domain", mExcludeLocationsOptionsGroups[GROUP_ZORAS_DOMAIN], false);
    mOptionGroups[RSG_EXCLUDES_JABU_JABU] = OptionGroup::SubGroup("Jabu Jabu's Belly", mExcludeLocationsOptionsGroups[GROUP_DUNGEON_JABUJABUS_BELLY], false);
    mOptionGroups[RSG_EXCLUDES_ICE_CAVERN] = OptionGroup::SubGroup("Ice Cavern", mExcludeLocationsOptionsGroups[GROUP_DUNGEON_ICE_CAVERN], false);
    mOptionGroups[RSG_EXCLUDES_HYRULE_FIELD] = OptionGroup::SubGroup("Hyrule Field", mExcludeLocationsOptionsGroups[GROUP_HYRULE_FIELD], false);
    mOptionGroups[RSG_EXCLUDES_LON_LON_RANCH] = OptionGroup::SubGroup("Lon Lon Ranch", mExcludeLocationsOptionsGroups[GROUP_LON_LON_RANCH], false);
    mOptionGroups[RSG_EXCLUDES_LAKE_HYLIA] = OptionGroup::SubGroup("Lake Hylia", mExcludeLocationsOptionsGroups[GROUP_LAKE_HYLIA], false);
    mOptionGroups[RSG_EXCLUDES_WATER_TEMPLE] = OptionGroup::SubGroup("Water Temple", mExcludeLocationsOptionsGroups[GROUP_DUNGEON_WATER_TEMPLE], false);
    mOptionGroups[RSG_EXCLUDES_GERUDO_VALLEY] = OptionGroup::SubGroup("Gerudo Valley", mExcludeLocationsOptionsGroups[GROUP_GERUDO_VALLEY], false);
    mOptionGroups[RSG_EXCLUDES_GERUDO_TRAINING_GROUNDS] = OptionGroup::SubGroup("Gerudo Training Grounds", mExcludeLocationsOptionsGroups[GROUP_GERUDO_TRAINING_GROUND], false);
    mOptionGroups[RSG_EXCLUDES_SPIRIT_TEMPLE] = OptionGroup::SubGroup("Spirit Temple", mExcludeLocationsOptionsGroups[GROUP_DUNGEON_SPIRIT_TEMPLE], false);
    mOptionGroups[RSG_EXCLUDES_HYRULE_CASTLE] = OptionGroup::SubGroup("Hyrule Castle", mExcludeLocationsOptionsGroups[GROUP_HYRULE_CASTLE], false);
    mOptionGroups[RSG_EXCLUDES_GANONS_CASTLE] = OptionGroup::SubGroup("Ganon's Castle", mExcludeLocationsOptionsGroups[GROUP_DUNGEON_GANONS_CASTLE], false);
    mOptionGroups[RSG_EXCLUDES] = OptionGroup::SubGroup("Exclude Locations", {
        &mOptionGroups[RSG_EXCLUDES_KOKIRI_FOREST],
        &mOptionGroups[RSG_EXCLUDES_LOST_WOODS],
        &mOptionGroups[RSG_EXCLUDES_DEKU_TREE],
        &mOptionGroups[RSG_EXCLUDES_FOREST_TEMPLE],
        &mOptionGroups[RSG_EXCLUDES_KAKARIKO_VILLAGE],
        &mOptionGroups[RSG_EXCLUDES_BOTTOM_OF_THE_WELL],
        &mOptionGroups[RSG_EXCLUDES_SHADOW_TEMPLE],
        &mOptionGroups[RSG_EXCLUDES_DEATH_MOUNTAIN],
        &mOptionGroups[RSG_EXCLUDES_GORON_CITY],
        &mOptionGroups[RSG_EXCLUDES_DODONGOS_CAVERN],
        &mOptionGroups[RSG_EXCLUDES_FIRE_TEMPLE],
        &mOptionGroups[RSG_EXCLUDES_ZORAS_RIVER],
        &mOptionGroups[RSG_EXCLUDES_ZORAS_DOMAIN],
        &mOptionGroups[RSG_EXCLUDES_JABU_JABU],
        &mOptionGroups[RSG_EXCLUDES_ICE_CAVERN],
        &mOptionGroups[RSG_EXCLUDES_HYRULE_FIELD],
        &mOptionGroups[RSG_EXCLUDES_LON_LON_RANCH],
        &mOptionGroups[RSG_EXCLUDES_LAKE_HYLIA],
        &mOptionGroups[RSG_EXCLUDES_WATER_TEMPLE],
        &mOptionGroups[RSG_EXCLUDES_GERUDO_VALLEY],
        &mOptionGroups[RSG_EXCLUDES_GERUDO_TRAINING_GROUNDS],
        &mOptionGroups[RSG_EXCLUDES_SPIRIT_TEMPLE],
        &mOptionGroups[RSG_EXCLUDES_HYRULE_CASTLE],
        &mOptionGroups[RSG_EXCLUDES_GANONS_CASTLE],
    }, false);
    mOptionGroups[RSG_DETAILED_LOGIC] = OptionGroup("Detailed Logic Settings", {
        &mOptionGroups[RSG_LOGIC],
        &mOptionGroups[RSG_TRICKS],
        &mOptionGroups[RSG_EXCLUDES]
    });

    VanillaLogicDefaults = {
        &mOptions[RSK_LINKS_POCKET],
        &mOptions[RSK_SHUFFLE_DUNGEON_REWARDS],
        &mOptions[RSK_SHUFFLE_SONGS],
        &mOptions[RSK_SHOPSANITY],
        &mOptions[RSK_SHOPSANITY_PRICES],
        &mOptions[RSK_SHOPSANITY_PRICES_AFFORDABLE],
        &mOptions[RSK_SHUFFLE_SCRUBS],
        &mOptions[RSK_SHUFFLE_COWS],
        &mOptions[RSK_SHUFFLE_MAGIC_BEANS],
        &mOptions[RSK_SHUFFLE_MERCHANTS],
        &mOptions[RSK_SHUFFLE_FROG_SONG_RUPEES],
        &mOptions[RSK_SHUFFLE_ADULT_TRADE],
        &mOptions[RSK_SHUFFLE_100_GS_REWARD],
        &mOptions[RSK_GOSSIP_STONE_HINTS],
    };

    mSpoilerfileSettingNameToEnum = {
        { "Logic Options:Logic", RSK_LOGIC_RULES },
        { "Logic Options:Night Skulltula's Expect Sun's Song", RSK_SKULLS_SUNS_SONG },
        { "Logic Options:All Locations Reachable", RSK_ALL_LOCATIONS_REACHABLE },
        { "Item Pool Settings:Item Pool", RSK_ITEM_POOL },
        { "Item Pool Settings:Ice Traps", RSK_ICE_TRAPS },
        { "Open Settings:Forest", RSK_FOREST },
        { "Open Settings:Kakariko Gate", RSK_KAK_GATE },
        { "Open Settings:Door of Time", RSK_DOOR_OF_TIME },
        { "Open Settings:Zora's Fountain", RSK_ZORAS_FOUNTAIN },
        { "Open Settings:Gerudo Fortress", RSK_GERUDO_FORTRESS },
        { "Open Settings:Rainbow Bridge", RSK_RAINBOW_BRIDGE },
        { "Open Settings:Ganon's Trials", RSK_GANONS_TRIALS },
        { "Open Settings:Ganon's Trials Count", RSK_TRIAL_COUNT },
        { "Open Settings:Stone Count", RSK_RAINBOW_BRIDGE_STONE_COUNT },
        { "Open Settings:Medallion Count", RSK_RAINBOW_BRIDGE_MEDALLION_COUNT },
        { "Open Settings:Reward Count", RSK_RAINBOW_BRIDGE_REWARD_COUNT },
        { "Open Settings:Dungeon Count", RSK_RAINBOW_BRIDGE_DUNGEON_COUNT },
        { "Open Settings:Token Count", RSK_RAINBOW_BRIDGE_TOKEN_COUNT },
        { "Open Settings:Bridge Reward Options", RSK_BRIDGE_OPTIONS },
        { "Shuffle Settings:Shuffle Dungeon Rewards", RSK_SHUFFLE_DUNGEON_REWARDS },
        { "Shuffle Settings:Link's Pocket", RSK_LINKS_POCKET },
        { "Shuffle Settings:Shuffle Songs", RSK_SHUFFLE_SONGS },
        { "Shuffle Settings:Shuffle Gerudo Membership Card", RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD },
        { "Shuffle Settings:Shopsanity", RSK_SHOPSANITY },
        { "Shuffle Settings:Shopsanity Prices", RSK_SHOPSANITY_PRICES },
        { "Shuffle Settings:Affordable Prices", RSK_SHOPSANITY_PRICES_AFFORDABLE },
        { "Shuffle Settings:Scrub Shuffle", RSK_SHUFFLE_SCRUBS },
        { "Shuffle Settings:Shuffle Cows", RSK_SHUFFLE_COWS },
        { "Shuffle Settings:Tokensanity", RSK_SHUFFLE_TOKENS },
        { "Shuffle Settings:Shuffle Ocarinas", RSK_SHUFFLE_OCARINA },
        { "Shuffle Settings:Shuffle Adult Trade", RSK_SHUFFLE_ADULT_TRADE },
        { "Shuffle Settings:Shuffle Magic Beans", RSK_SHUFFLE_MAGIC_BEANS },
        { "Shuffle Settings:Shuffle Kokiri Sword", RSK_SHUFFLE_KOKIRI_SWORD },
        { "Shuffle Settings:Shuffle Master Sword", RSK_SHUFFLE_MASTER_SWORD },
        { "Shuffle Settings:Shuffle Weird Egg", RSK_SHUFFLE_WEIRD_EGG },
        { "Shuffle Settings:Shuffle Frog Song Rupees", RSK_SHUFFLE_FROG_SONG_RUPEES },
        { "Shuffle Settings:Shuffle Merchants", RSK_SHUFFLE_MERCHANTS },
        { "Shuffle Settings:Shuffle 100 GS Reward", RSK_SHUFFLE_100_GS_REWARD },
        { "Start with Deku Shield", RSK_STARTING_DEKU_SHIELD },
        { "Start with Kokiri Sword", RSK_STARTING_KOKIRI_SWORD },
        { "Start with Fairy Ocarina", RSK_STARTING_OCARINA },
        { "Start with Zelda's Lullaby", RSK_STARTING_ZELDAS_LULLABY },
        { "Start with Epona's Song", RSK_STARTING_EPONAS_SONG },
        { "Start with Saria's Song", RSK_STARTING_SARIAS_SONG },
        { "Start with Sun's Song", RSK_STARTING_SUNS_SONG },
        { "Start with Song of Time", RSK_STARTING_SONG_OF_TIME },
        { "Start with Song of Storms", RSK_STARTING_SONG_OF_STORMS },
        { "Start with Minuet of Forest", RSK_STARTING_MINUET_OF_FOREST },
        { "Start with Bolero of Fire", RSK_STARTING_BOLERO_OF_FIRE },
        { "Start with Serenade of Water", RSK_STARTING_SERENADE_OF_WATER },
        { "Start with Requiem of Spirit", RSK_STARTING_REQUIEM_OF_SPIRIT },
        { "Start with Nocturne of Shadow", RSK_STARTING_NOCTURNE_OF_SHADOW },
        { "Start with Prelude of Light", RSK_STARTING_PRELUDE_OF_LIGHT },
        { "Shuffle Dungeon Items:Maps/Compasses", RSK_SHUFFLE_MAPANDCOMPASS },
        { "Shuffle Dungeon Items:Small Keys", RSK_KEYSANITY },
        { "Shuffle Dungeon Items:Gerudo Fortress Keys", RSK_GERUDO_KEYS },
        { "Shuffle Dungeon Items:Boss Keys", RSK_BOSS_KEYSANITY },
        { "Shuffle Dungeon Items:Ganon's Boss Key", RSK_GANONS_BOSS_KEY },
        { "Shuffle Dungeon Items:Stone Count", RSK_LACS_STONE_COUNT },
        { "Shuffle Dungeon Items:Medallion Count", RSK_LACS_MEDALLION_COUNT },
        { "Shuffle Dungeon Items:Reward Count", RSK_LACS_REWARD_COUNT },
        { "Shuffle Dungeon Items:Dungeon Count", RSK_LACS_DUNGEON_COUNT },
        { "Shuffle Dungeon Items:Token Count", RSK_LACS_TOKEN_COUNT },
        { "Shuffle Dungeon Items:LACS Reward Options", RSK_LACS_OPTIONS },
        { "Shuffle Dungeon Items:Key Rings", RSK_KEYRINGS },
        { "Shuffle Dungeon Items:Keyring Dungeon Count", RSK_KEYRINGS_RANDOM_COUNT },
        { "Shuffle Dungeon Items:Gerudo Fortress", RSK_KEYRINGS_GERUDO_FORTRESS },
        { "Shuffle Dungeon Items:Forest Temple", RSK_KEYRINGS_FOREST_TEMPLE },
        { "Shuffle Dungeon Items:Fire Temple", RSK_KEYRINGS_FIRE_TEMPLE },
        { "Shuffle Dungeon Items:Water Temple", RSK_KEYRINGS_WATER_TEMPLE },
        { "Shuffle Dungeon Items:Spirit Temple", RSK_KEYRINGS_SPIRIT_TEMPLE },
        { "Shuffle Dungeon Items:Shadow Temple", RSK_KEYRINGS_SHADOW_TEMPLE },
        { "Shuffle Dungeon Items:Bottom of the Well", RSK_KEYRINGS_BOTTOM_OF_THE_WELL },
        { "Shuffle Dungeon Items:GTG", RSK_KEYRINGS_GTG },
        { "Shuffle Dungeon Items:Ganon's Castle", RSK_KEYRINGS_GANONS_CASTLE },
        { "World Settings:Starting Age", RSK_STARTING_AGE },
        // TODO: Ammo Drop settings
        { "World Settings:Bombchu Drops", RSK_ENABLE_BOMBCHU_DROPS },
        { "World Settings:Bombchus in Logic", RSK_BOMBCHUS_IN_LOGIC },
        { "World Settings:Shuffle Entrances", RSK_SHUFFLE_ENTRANCES },
        { "World Settings:Dungeon Entrances", RSK_SHUFFLE_DUNGEON_ENTRANCES },
        { "World Settings:Boss Entrances", RSK_SHUFFLE_BOSS_ENTRANCES },
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
        { "World Settings:Triforce Hunt", RSK_TRIFORCE_HUNT },
        { "World Settings:Triforce Hunt Total Pieces", RSK_TRIFORCE_HUNT_PIECES_TOTAL },
        { "World Settings:Triforce Hunt Required Pieces", RSK_TRIFORCE_HUNT_PIECES_REQUIRED },
        { "Miscellaneous Settings:Gossip Stone Hints", RSK_GOSSIP_STONE_HINTS },
        { "Miscellaneous Settings:Hint Clarity", RSK_HINT_CLARITY },
        { "Miscellaneous Settings:ToT Altar Hint", RSK_TOT_ALTAR_HINT },
        { "Miscellaneous Settings:Light Arrow Hint", RSK_LIGHT_ARROWS_HINT },
        { "Miscellaneous Settings:Dampe's Diary Hint", RSK_DAMPES_DIARY_HINT },
        { "Miscellaneous Settings:Greg the Rupee Hint", RSK_GREG_HINT },
        { "Miscellaneous Settings:Saria's Hint", RSK_SARIA_HINT },
        { "Miscellaneous Settings:Frog Ocarina Game Hint", RSK_FROGS_HINT },
        { "Miscellaneous Settings:10 GS Hint", RSK_KAK_10_SKULLS_HINT },
        { "Miscellaneous Settings:20 GS Hint", RSK_KAK_20_SKULLS_HINT },
        { "Miscellaneous Settings:30 GS Hint", RSK_KAK_30_SKULLS_HINT },
        { "Miscellaneous Settings:40 GS Hint", RSK_KAK_40_SKULLS_HINT },
        { "Miscellaneous Settings:50 GS Hint", RSK_KAK_50_SKULLS_HINT },
        { "Miscellaneous Settings:Warp Song Hints", RSK_WARP_SONG_HINTS },
        { "Miscellaneous Settings:Scrub Hint Text", RSK_SCRUB_TEXT_HINT },
        { "Miscellaneous Settings:Hint Distribution", RSK_HINT_DISTRIBUTION },
        { "Miscellaneous Settings:Blue Fire Arrows", RSK_BLUE_FIRE_ARROWS },
        { "Miscellaneous Settings:Sunlight Arrows", RSK_SUNLIGHT_ARROWS },
        { "Timesaver Settings:Skip Child Zelda", RSK_SKIP_CHILD_ZELDA },
        { "Start with Consumables", RSK_STARTING_CONSUMABLES },
        { "Full Wallets", RSK_FULL_WALLETS },
        { "Gold Skulltula Tokens", RSK_STARTING_SKULLTULA_TOKEN },
        { "Timesaver Settings:Cuccos to return", RSK_CUCCO_COUNT },
        { "Timesaver Settings:Big Poe Target Count", RSK_BIG_POE_COUNT },
        { "Timesaver Settings:Skip Child Stealth", RSK_SKIP_CHILD_STEALTH },
        { "Timesaver Settings:Skip Epona Race", RSK_SKIP_EPONA_RACE },
        { "Timesaver Settings:Skip Tower Escape", RSK_SKIP_TOWER_ESCAPE },
        { "Timesaver Settings:Complete Mask Quest", RSK_COMPLETE_MASK_QUEST },
        { "Timesaver Settings:Skip Scarecrow's Song", RSK_SKIP_SCARECROWS_SONG },
        { "Timesaver Settings:Enable Glitch-Useful Cutscenes", RSK_ENABLE_GLITCH_CUTSCENES },
        { "World Settings:MQ Dungeon Setting", RSK_MQ_DUNGEON_RANDOM },
        { "World Settings:MQ Dungeon Count", RSK_MQ_DUNGEON_COUNT },
        { "World Settings:Set Dungeon Quests", RSK_MQ_DUNGEON_SET },
        { "Shuffle Dungeon Quest:Forest Temple", RSK_MQ_FOREST_TEMPLE },
        { "Shuffle Dungeon Quest:Fire Temple", RSK_MQ_FIRE_TEMPLE },
        { "Shuffle Dungeon Quest:Water Temple", RSK_MQ_WATER_TEMPLE },
        { "Shuffle Dungeon Quest:Spirit Temple", RSK_MQ_SPIRIT_TEMPLE },
        { "Shuffle Dungeon Quest:Shadow Temple", RSK_MQ_SHADOW_TEMPLE },
        { "Shuffle Dungeon Quest:Bottom of the Well", RSK_MQ_BOTTOM_OF_THE_WELL },
        { "Shuffle Dungeon Quest:Ice Cavern", RSK_MQ_ICE_CAVERN },
        { "Shuffle Dungeon Quest:GTG", RSK_MQ_GTG },
        { "Shuffle Dungeon Quest:Ganon's Castle", RSK_MQ_GANONS_CASTLE },
    };
}

Option& Settings::Setting(RandomizerSettingKey key) {
    return mOptions[key];
}

Option& Settings::GetTrickOption(RandomizerTrick key) {
    return mTrickOptions[key];
}

const std::array<Option, RSK_MAX>& Settings::GetAllOptions() const {
    return mOptions;
}

std::vector<Option *>& Settings::GetExcludeOptionsForGroup(SpoilerCollectionCheckGroup group) {
    return mExcludeLocationsOptionsGroups[group];
}

const std::vector<std::vector<Option *>>& Settings::GetExcludeLocationsOptions() const {
    return mExcludeLocationsOptionsGroups;
}

RandoOptionStartingAge Settings::ResolvedStartingAge() const {
    return mResolvedStartingAge;
}

RandoOptionLACSCondition Settings::LACSCondition() const {
    return mLACSCondition;
}

std::string Settings::GetHash() const {
    return mHash;
}

void Settings::SetHash(std::string hash) {
    mHash = hash;
}

const std::string& Settings::GetSeedString() const {
    return mSeedString;
}

void Settings::SetSeedString(std::string seedString) {
    mSeedString = seedString;
}

const uint32_t Settings::GetSeed() const {
    return mFinalSeed;
}

void Settings::SetSeed(uint32_t seed) {
    mFinalSeed = seed;
}

const std::array<OptionGroup, RSG_MAX>& Settings::GetOptionGroups() {
    return mOptionGroups;
}

const OptionGroup& Settings::GetOptionGroup(RandomizerSettingGroupKey key) {
    return mOptionGroups[key];
}

void Settings::SetAllFromCVar() {
    for (auto& option : mOptions) {
        option.SetFromCVar();
    }
}

void Settings::UpdateSettings(std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks) {
    auto ctx = Rando::Context::GetInstance();
    if (!ctx->IsSpoilerLoaded()) {
        // If we've loaded a spoiler file, the settings have already been populated, so we
        // only need to do things like resolve the starting age or determine MQ dungeons.
        // Any logic dependent on cvarSettings should go in this if statement
        
        // if we skip child zelda, we start with zelda's letter, and malon starts
        // at the ranch, so we should *not* shuffle the weird egg
        if (mOptions[RSK_SKIP_CHILD_ZELDA]) {
            mOptions[RSK_SHUFFLE_WEIRD_EGG].SetSelectedIndex(0);
        }

        // Force 100 GS Shuffle if that's where Ganon's Boss Key is
        if (mOptions[RSK_GANONS_BOSS_KEY].Is(RO_GANON_BOSS_KEY_KAK_TOKENS)) {
            mOptions[RSK_SHUFFLE_100_GS_REWARD].SetSelectedIndex(1);
        }

        if (mOptions[RSK_TRIFORCE_HUNT]) {
            mOptions[RSK_GANONS_BOSS_KEY].SetSelectedIndex(RO_GANON_BOSS_KEY_TRIFORCE_HUNT);
        }

        if (OTRGlobals::Instance->HasMasterQuest() && !OTRGlobals::Instance->HasOriginal()) {
            mOptions[RSK_MQ_DUNGEON_RANDOM].SetSelectedIndex(RO_MQ_DUNGEONS_SET_NUMBER);
            mOptions[RSK_MQ_DUNGEON_COUNT].SetSelectedIndex(12);
        }

        if (OTRGlobals::Instance->HasOriginal() && !OTRGlobals::Instance->HasMasterQuest()) {
            mOptions[RSK_MQ_DUNGEON_RANDOM].SetSelectedIndex(RO_MQ_DUNGEONS_NONE);
            mOptions[RSK_MQ_DUNGEON_COUNT].SetSelectedIndex(0);
        }

        if (mOptions[RSK_SHUFFLE_DUNGEON_ENTRANCES].IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF)
            || mOptions[RSK_SHUFFLE_BOSS_ENTRANCES].IsNot(RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF)
            || mOptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES]
            || mOptions[RSK_SHUFFLE_INTERIOR_ENTRANCES].IsNot(RO_INTERIOR_ENTRANCE_SHUFFLE_OFF)
            || mOptions[RSK_SHUFFLE_GROTTO_ENTRANCES] || mOptions[RSK_SHUFFLE_OWL_DROPS]
            || mOptions[RSK_SHUFFLE_WARP_SONGS] || mOptions[RSK_SHUFFLE_OVERWORLD_SPAWNS]) {
            mOptions[RSK_SHUFFLE_ENTRANCES].SetSelectedIndex(RO_GENERIC_ON);
        }

        // ImGui has a slider which returns the actual number, which is off by one since
        // the ImGui slider can't go down to 0 (the first index aka 1 big poe).
        // TODO: Revisit Slider mOptions[RSK_BIG_POE_COUNT].SetSelectedIndex(cvarSettings[RSK_BIG_POE_COUNT] - 1);
        ctx->AddExcludedOptions();
        for (auto locationKey : ctx->everyPossibleLocation) {
            auto location = ctx->GetItemLocation(locationKey);
            if (excludedLocations.count(location->GetRandomizerCheck())) {
                location->GetExcludedOption()->SetSelectedIndex(1);
            } else {
                location->GetExcludedOption()->SetSelectedIndex(0);
            }
        }
        // Tricks
        for (auto randomizerTrick : enabledTricks) {
            mTrickOptions[randomizerTrick].SetSelectedIndex(1);
        }
    }

    // RANDOTODO implement chest shuffle with keysanity
    // ShuffleChestMinigame.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_CHEST_MINIGAME]);
    //TODO: RandomizeAllSettings(true) when implementing the ability to randomize the options themselves.
    std::array<DungeonInfo*, 12> dungeons = ctx->GetDungeons()->GetDungeonList();
    std::array<bool, 12> dungeonModesKnown;
    std::vector<Option*> dungeonOptions = {
        &mOptions[RSK_MQ_DEKU_TREE],
        &mOptions[RSK_MQ_DODONGOS_CAVERN],
        &mOptions[RSK_MQ_JABU_JABU],
        &mOptions[RSK_MQ_FOREST_TEMPLE],
        &mOptions[RSK_MQ_FIRE_TEMPLE],
        &mOptions[RSK_MQ_WATER_TEMPLE],
        &mOptions[RSK_MQ_SPIRIT_TEMPLE],
        &mOptions[RSK_MQ_SHADOW_TEMPLE],
        &mOptions[RSK_MQ_BOTTOM_OF_THE_WELL],
        &mOptions[RSK_MQ_ICE_CAVERN],
        &mOptions[RSK_MQ_GTG],
        &mOptions[RSK_MQ_GANONS_CASTLE]
    };
    uint8_t mqSet = mOptions[RSK_MQ_DUNGEON_COUNT].Value<uint8_t>();
    if (mOptions[RSK_MQ_DUNGEON_RANDOM].Is(RO_MQ_DUNGEONS_SELECTION)) {
        mqSet = 0;
    }
    uint8_t dungeonCount = 0;
    std::vector<uint8_t> randMQOption = {};
    if (mOptions[RSK_MQ_DUNGEON_SET]) {
        for (size_t i = 0; i < dungeons.size(); i++) {
            dungeons[i]->ClearMQ();
            dungeonModesKnown[i] = true;
            switch (dungeonOptions[i]->Value<uint8_t>()) {
                case 1:
                    dungeons[i]->SetMQ();
                    dungeonCount++;
                    break;
                case 2:
                    randMQOption.push_back(i);
                    dungeonModesKnown[i] = false;
                    break;
            }
        }
        Shuffle(randMQOption);
        if (mOptions[RSK_MQ_DUNGEON_RANDOM].Is(RO_MQ_DUNGEONS_RANDOM_NUMBER)) {
            mqSet = dungeonCount + Random(0, randMQOption.size() + 1);
        }
        for (uint8_t i = 0; dungeonCount < mqSet; i++) {
            if (i > randMQOption.size()) {
                // This can happen if the amount of MQ Dungeons is specifically
                // set to a higher number than the amount of Dungeons specifically set to MQ or Random,
                // break out of the loop and just have fewer MQ dungeons than the Set Count.
                break;
            }
            dungeons[randMQOption[i]]->SetMQ();
            dungeonCount++;
        }
    } else {
        Shuffle(dungeons);
        for (size_t i = 0; i < dungeons.size(); i++) {
            dungeons[i]->ClearMQ();
        }
        bool allDungeonModesKnown = mqSet == 0 || mqSet == dungeons.size();
        for (uint8_t i = 0; i < sizeof(dungeonModesKnown); ++i) {
            dungeonModesKnown[i] = allDungeonModesKnown;
        }
        if (mOptions[RSK_MQ_DUNGEON_RANDOM].Is(RO_MQ_DUNGEONS_RANDOM_NUMBER)) {
            mqSet = Random(0, 13);
        }
        for (uint8_t i = 0; i < mqSet; i++) {
            dungeons[i]->SetMQ();
        }
    }

    // Set key ring for each dungeon
    for (size_t i = 0; i < dungeons.size(); i++) {
        dungeons[i]->ClearKeyRing();
    }

    std::vector<Option*> keyRingOptions = {
        &mOptions[RSK_KEYRINGS_FOREST_TEMPLE],
        &mOptions[RSK_KEYRINGS_FIRE_TEMPLE],
        &mOptions[RSK_KEYRINGS_WATER_TEMPLE],
        &mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE],
        &mOptions[RSK_KEYRINGS_SHADOW_TEMPLE],
        &mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL],
        &mOptions[RSK_KEYRINGS_GTG],
        &mOptions[RSK_KEYRINGS_GANONS_CASTLE],
    };

    if (mOptions[RSK_KEYRINGS]) {
        // Random Key Rings
        if (mOptions[RSK_KEYRINGS].Is(RO_KEYRINGS_RANDOM) || mOptions[RSK_KEYRINGS].Is(RO_KEYRINGS_COUNT)) {
            auto keyrings = keyRingOptions;
            if (mOptions[RSK_GERUDO_FORTRESS].Is(RO_GF_NORMAL) && mOptions[RSK_GERUDO_KEYS].IsNot(RO_GERUDO_KEYS_VANILLA)) {
                keyrings.push_back(&mOptions[RSK_KEYRINGS_GERUDO_FORTRESS]);
            }
            int keyRingCount = mOptions[RSK_KEYRINGS].Is(RO_KEYRINGS_COUNT) ? mOptions[RSK_KEYRINGS_RANDOM_COUNT].Value<uint8_t>() : Random(0, keyrings.size());
            Shuffle(keyrings);
            for (size_t i = 0; i < keyRingCount; i++) {
                keyrings[i]->SetSelectedIndex(RO_GENERIC_ON);
            }
        }
        if (mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL]) {
            ctx->GetDungeon(BOTTOM_OF_THE_WELL)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_FOREST_TEMPLE]) {
            ctx->GetDungeon(FOREST_TEMPLE)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_FIRE_TEMPLE]) {
            ctx->GetDungeon(FIRE_TEMPLE)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_WATER_TEMPLE]) {
            ctx->GetDungeon(WATER_TEMPLE)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE]) {
            ctx->GetDungeon(SPIRIT_TEMPLE)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_SHADOW_TEMPLE]) {
            ctx->GetDungeon(SHADOW_TEMPLE)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_GTG]) {
            ctx->GetDungeon(GERUDO_TRAINING_GROUNDS)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_GANONS_CASTLE]) {
            ctx->GetDungeon(GANONS_CASTLE)->SetKeyRing();
        }
    }
    auto trials = ctx->GetTrials()->GetTrialList();
    Shuffle(trials);
    for (auto& trial : trials) {
        trial->SetAsSkipped();
    }
    if (mOptions[RSK_GANONS_TRIALS].Is(RO_GANONS_TRIALS_RANDOM_NUMBER)) {
        mOptions[RSK_TRIAL_COUNT].SetSelectedIndex(Random(0, mOptions[RSK_TRIAL_COUNT].GetOptionCount()));
    }
    for (uint8_t i = 0; i < mOptions[RSK_TRIAL_COUNT].Value<uint8_t>(); i++) {
        trials[i]->SetAsRequired();
    }

    if (mOptions[RSK_FOREST].Is(RO_FOREST_CLOSED) &&
        (mOptions[RSK_SHUFFLE_INTERIOR_ENTRANCES].Is(RO_INTERIOR_ENTRANCE_SHUFFLE_ALL) ||
         mOptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES] || mOptions[RSK_SHUFFLE_OVERWORLD_SPAWNS] ||
         mOptions[RSK_DECOUPLED_ENTRANCES] || mOptions[RSK_MIXED_ENTRANCE_POOLS])) {
        mOptions[RSK_FOREST].SetSelectedIndex(RO_FOREST_CLOSED_DEKU);
    }

    if (mOptions[RSK_STARTING_AGE].Is(RO_AGE_RANDOM)) {
        int choice = Random(0, 2);
        if (choice == 0) {
            mResolvedStartingAge = RO_AGE_CHILD;
        } else {
            mResolvedStartingAge = RO_AGE_ADULT;
        }
    } else {
        mResolvedStartingAge = static_cast<RandoOptionStartingAge>(mOptions[RSK_STARTING_AGE].Value<uint8_t>());
    }

    // TODO: Random Starting Time

    if (mOptions[RSK_GANONS_BOSS_KEY].Is(RO_GANON_BOSS_KEY_LACS_STONES)) {
        mLACSCondition = RO_LACS_STONES;
    } else if (mOptions[RSK_GANONS_BOSS_KEY].Is(RO_GANON_BOSS_KEY_LACS_MEDALLIONS)) {
        mLACSCondition = RO_LACS_MEDALLIONS;
    } else if (mOptions[RSK_GANONS_BOSS_KEY].Is(RO_GANON_BOSS_KEY_LACS_REWARDS)) {
        mLACSCondition = RO_LACS_REWARDS;
    } else if (mOptions[RSK_GANONS_BOSS_KEY].Is(RO_GANON_BOSS_KEY_LACS_DUNGEONS)) {
        mLACSCondition = RO_LACS_DUNGEONS;
    } else if (mOptions[RSK_GANONS_BOSS_KEY].Is(RO_GANON_BOSS_KEY_LACS_TOKENS)) {
        mLACSCondition = RO_LACS_TOKENS;
    } else {
        mLACSCondition = RO_LACS_VANILLA;
    }

    if (mOptions[RSK_LOGIC_RULES].Is(RO_LOGIC_VANILLA)) {
        for (Option* setting : VanillaLogicDefaults) {
            setting->SetDelayedOption();
            setting->SetSelectedIndex(0);
        }
        mOptions[RSK_KEYSANITY].SetDelayedOption();
        mOptions[RSK_KEYSANITY].SetSelectedIndex(3);
    }
}
void Settings::ParseJson(nlohmann::json spoilerFileJson) {
    mSeedString = spoilerFileJson["seed"].template get<std::string>();
    mFinalSeed = spoilerFileJson["finalSeed"].get<uint32_t>();
    nlohmann::json settingsJson = spoilerFileJson["settings"];
    for (auto it = settingsJson.begin(); it != settingsJson.end(); ++it) {
        // todo load into cvars for UI

        std::string numericValueString;
        if (mSpoilerfileSettingNameToEnum.count(it.key())) {
            RandomizerSettingKey index = mSpoilerfileSettingNameToEnum[it.key()];
            // this is annoying but the same strings are used in different orders
            // and i don't want the spoilerfile to just have numbers instead of
            // human readable settings values so it'll have to do for now
            switch (index) {
                case RSK_LOGIC_RULES:
                    if (it.value() == "Glitchless") {
                        mOptions[index].SetSelectedIndex(RO_LOGIC_GLITCHLESS);
                    } else if (it.value() == "No Logic") {
                        mOptions[index].SetSelectedIndex(RO_LOGIC_NO_LOGIC);
                    }
                    break;
                case RSK_FOREST:
                    if (it.value() == "Closed") {
                        mOptions[index].SetSelectedIndex(RO_FOREST_CLOSED);
                    } else if (it.value() == "Open") {
                        mOptions[index].SetSelectedIndex(RO_FOREST_OPEN);
                    } else if (it.value() == "Closed Deku") {
                        mOptions[index].SetSelectedIndex(RO_FOREST_CLOSED_DEKU);
                    }
                    break;
                case RSK_KAK_GATE:
                    if (it.value() == "Closed") {
                        mOptions[index].SetSelectedIndex(RO_KAK_GATE_CLOSED);
                    } else if (it.value() == "Open") {
                        mOptions[index].SetSelectedIndex(RO_KAK_GATE_OPEN);
                    }
                    break;
                case RSK_DOOR_OF_TIME:
                    if (it.value() == "Open") {
                        mOptions[index].SetSelectedIndex(RO_DOOROFTIME_OPEN);
                    } else if (it.value() == "Song only") {
                        mOptions[index].SetSelectedIndex(RO_DOOROFTIME_SONGONLY);
                    } else if (it.value() == "Closed") {
                        mOptions[index].SetSelectedIndex(RO_DOOROFTIME_CLOSED);
                    }
                    break;
                case RSK_ZORAS_FOUNTAIN:
                    if (it.value() == "Closed") {
                        mOptions[index].SetSelectedIndex(RO_ZF_CLOSED);
                    } else if (it.value() == "Closed as child") {
                        mOptions[index].SetSelectedIndex(RO_ZF_CLOSED_CHILD);
                    } else if (it.value() == "Open") {
                        mOptions[index].SetSelectedIndex(RO_ZF_OPEN);
                    }
                    break;
                case RSK_STARTING_AGE:
                    if (it.value() == "Child") {
                        mOptions[index].SetSelectedIndex(RO_AGE_CHILD);
                    } else if (it.value() == "Adult") {
                        mOptions[index].SetSelectedIndex(RO_AGE_ADULT);
                    }
                    break;
                case RSK_GERUDO_FORTRESS:
                    if (it.value() == "Normal") {
                        mOptions[index].SetSelectedIndex(RO_GF_NORMAL);
                    } else if (it.value() == "Fast") {
                        mOptions[index].SetSelectedIndex(RO_GF_FAST);
                    } else if (it.value() == "Open") {
                        mOptions[index].SetSelectedIndex(RO_GF_OPEN);
                    }
                    break;
                case RSK_RAINBOW_BRIDGE:
                    if (it.value() == "Vanilla") {
                        mOptions[index].SetSelectedIndex(RO_BRIDGE_VANILLA);
                    } else if (it.value() == "Always open") {
                        mOptions[index].SetSelectedIndex(RO_BRIDGE_ALWAYS_OPEN);
                    } else if (it.value() == "Stones") {
                        mOptions[index].SetSelectedIndex(RO_BRIDGE_STONES);
                    } else if (it.value() == "Medallions") {
                        mOptions[index].SetSelectedIndex(RO_BRIDGE_MEDALLIONS);
                    } else if (it.value() == "Dungeon rewards") {
                        mOptions[index].SetSelectedIndex(RO_BRIDGE_DUNGEON_REWARDS);
                    } else if (it.value() == "Dungeons") {
                        mOptions[index].SetSelectedIndex(RO_BRIDGE_DUNGEONS);
                    } else if (it.value() == "Tokens") {
                        mOptions[index].SetSelectedIndex(RO_BRIDGE_TOKENS);
                    } else if (it.value() == "Greg") {
                        mOptions[index].SetSelectedIndex(RO_BRIDGE_GREG);
                    }
                    break;
                case RSK_BRIDGE_OPTIONS:
                    if (it.value() == "Standard Rewards") {
                        mOptions[index].SetSelectedIndex(RO_BRIDGE_STANDARD_REWARD);
                    } else if (it.value() == "Greg as Reward") {
                        mOptions[index].SetSelectedIndex(RO_BRIDGE_GREG_REWARD);
                    } else if (it.value() == "Greg as Wildcard") {
                        mOptions[index].SetSelectedIndex(RO_BRIDGE_WILDCARD_REWARD);
                    }
                    break;
                case RSK_LACS_OPTIONS:
                    if (it.value() == "Standard Reward") {
                        mOptions[index].SetSelectedIndex(RO_LACS_STANDARD_REWARD);
                    } else if (it.value() == "Greg as Reward") {
                        mOptions[index].SetSelectedIndex(RO_LACS_GREG_REWARD);
                    } else if (it.value() == "Greg as Wildcard") {
                        mOptions[index].SetSelectedIndex(RO_LACS_WILDCARD_REWARD);
                    }
                    break;
                case RSK_DAMAGE_MULTIPLIER:
                    if (it.value() == "x1/2") {
                        mOptions[index].SetSelectedIndex(RO_DAMAGE_MULTIPLIER_HALF);
                    } else if (it.value() == "x1") {
                        mOptions[index].SetSelectedIndex(RO_DAMAGE_MULTIPLIER_DEFAULT);
                    } else if (it.value() == "x2") {
                        mOptions[index].SetSelectedIndex(RO_DAMAGE_MULTIPLIER_DOUBLE);
                    } else if (it.value() == "x4") {
                        mOptions[index].SetSelectedIndex(RO_DAMAGE_MULTIPLIER_QUADRUPLE);
                    } else if (it.value() == "x8") {
                        mOptions[index].SetSelectedIndex(RO_DAMAGE_MULTIPLIER_OCTUPLE);
                    } else if (it.value() == "x16") {
                        mOptions[index].SetSelectedIndex(RO_DAMAGE_MULTIPLIER_SEXDECUPLE);
                    } else if (it.value() == "OHKO") {
                        mOptions[index].SetSelectedIndex(RO_DAMAGE_MULTIPLIER_OHKO);
                    }
                    break;
                case RSK_RAINBOW_BRIDGE_STONE_COUNT:
                case RSK_RAINBOW_BRIDGE_MEDALLION_COUNT:
                case RSK_RAINBOW_BRIDGE_REWARD_COUNT:
                case RSK_RAINBOW_BRIDGE_DUNGEON_COUNT:
                case RSK_RAINBOW_BRIDGE_TOKEN_COUNT:
                case RSK_TRIAL_COUNT:
                case RSK_LACS_STONE_COUNT:
                case RSK_LACS_MEDALLION_COUNT:
                case RSK_LACS_REWARD_COUNT:
                case RSK_LACS_DUNGEON_COUNT:
                case RSK_LACS_TOKEN_COUNT:
                case RSK_KEYRINGS_RANDOM_COUNT:
                case RSK_BIG_POE_COUNT:
                case RSK_CUCCO_COUNT:
                case RSK_STARTING_SKULLTULA_TOKEN:
                case RSK_TRIFORCE_HUNT_PIECES_TOTAL:
                case RSK_TRIFORCE_HUNT_PIECES_REQUIRED:
                case RSK_STARTING_HEARTS:
                    numericValueString = it.value();
                    mOptions[index].SetSelectedIndex(std::stoi(numericValueString));
                    break;
                case RSK_GANONS_TRIALS:
                    if (it.value() == "Skip") {
                        mOptions[index].SetSelectedIndex(RO_GANONS_TRIALS_SKIP);
                    } else if (it.value() == "Set Number") {
                        mOptions[index].SetSelectedIndex(RO_GANONS_TRIALS_SET_NUMBER);
                    } else if (it.value() == "Random Number") {
                        mOptions[index].SetSelectedIndex(RO_GANONS_TRIALS_RANDOM_NUMBER);
                    }
                case RSK_SHOPSANITY:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_SHOPSANITY_OFF);
                    } else if (it.value() == "0 Items") {
                        mOptions[index].SetSelectedIndex(RO_SHOPSANITY_ZERO_ITEMS);
                    } else if (it.value() == "1 Item") {
                        mOptions[index].SetSelectedIndex(RO_SHOPSANITY_ONE_ITEM);
                    } else if (it.value() == "2 Items") {
                        mOptions[index].SetSelectedIndex(RO_SHOPSANITY_TWO_ITEMS);
                    } else if (it.value() == "3 Items") {
                        mOptions[index].SetSelectedIndex(RO_SHOPSANITY_THREE_ITEMS);
                    } else if (it.value() == "4 Items") {
                        mOptions[index].SetSelectedIndex(RO_SHOPSANITY_FOUR_ITEMS);
                    } else if (it.value() == "Random") {
                        mOptions[index].SetSelectedIndex(RO_SHOPSANITY_RANDOM);
                    }
                    break;
                case RSK_SHOPSANITY_PRICES:
                    if (it.value() == "Random") {
                        mOptions[index].SetSelectedIndex(RO_SHOPSANITY_PRICE_BALANCED);
                    } else if (it.value() == "Starter Wallet") {
                        mOptions[index].SetSelectedIndex(RO_SHOPSANITY_PRICE_STARTER);
                    } else if (it.value() == "Adult's Wallet") {
                        mOptions[index].SetSelectedIndex(RO_SHOPSANITY_PRICE_ADULT);
                    } else if (it.value() == "Giant's Wallet") {
                        mOptions[index].SetSelectedIndex(RO_SHOPSANITY_PRICE_GIANT);
                    } else if (it.value() == "Tycoon's Wallet") {
                        mOptions[index].SetSelectedIndex(RO_SHOPSANITY_PRICE_TYCOON);
                    }
                case RSK_SHUFFLE_SCRUBS:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_SCRUBS_OFF);
                    } else if (it.value() == "Affordable") {
                        mOptions[index].SetSelectedIndex(RO_SCRUBS_AFFORDABLE);
                    } else if (it.value() == "Expensive") {
                        mOptions[index].SetSelectedIndex(RO_SCRUBS_EXPENSIVE);
                    } else if (it.value() == "Random Prices") {
                        mOptions[index].SetSelectedIndex(RO_SCRUBS_RANDOM);
                    }
                    break;
                case RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD:
                case RSK_SHUFFLE_COWS:
                case RSK_SHUFFLE_ADULT_TRADE:
                case RSK_SHUFFLE_MAGIC_BEANS:
                case RSK_SHUFFLE_KOKIRI_SWORD:
                case RSK_SHUFFLE_WEIRD_EGG:
                case RSK_SHUFFLE_FROG_SONG_RUPEES:
                case RSK_SHUFFLE_100_GS_REWARD:
                case RSK_SHUFFLE_OCARINA:
                case RSK_STARTING_DEKU_SHIELD:
                case RSK_STARTING_KOKIRI_SWORD:
                case RSK_STARTING_ZELDAS_LULLABY:
                case RSK_STARTING_EPONAS_SONG:
                case RSK_STARTING_SARIAS_SONG:
                case RSK_STARTING_SUNS_SONG:
                case RSK_STARTING_SONG_OF_TIME:
                case RSK_STARTING_SONG_OF_STORMS:
                case RSK_STARTING_MINUET_OF_FOREST:
                case RSK_STARTING_BOLERO_OF_FIRE:
                case RSK_STARTING_SERENADE_OF_WATER:
                case RSK_STARTING_REQUIEM_OF_SPIRIT:
                case RSK_STARTING_NOCTURNE_OF_SHADOW:
                case RSK_STARTING_PRELUDE_OF_LIGHT:
                case RSK_COMPLETE_MASK_QUEST:
                case RSK_SKIP_SCARECROWS_SONG:
                case RSK_ENABLE_GLITCH_CUTSCENES:
                case RSK_SKULLS_SUNS_SONG:
                case RSK_BLUE_FIRE_ARROWS:
                case RSK_SUNLIGHT_ARROWS:
                case RSK_BOMBCHUS_IN_LOGIC:
                case RSK_TOT_ALTAR_HINT:
                case RSK_LIGHT_ARROWS_HINT:
                case RSK_DAMPES_DIARY_HINT:
                case RSK_GREG_HINT:
                case RSK_SARIA_HINT:
                case RSK_FROGS_HINT:
                case RSK_KAK_10_SKULLS_HINT:
                case RSK_KAK_20_SKULLS_HINT:
                case RSK_KAK_30_SKULLS_HINT:
                case RSK_KAK_40_SKULLS_HINT:
                case RSK_KAK_50_SKULLS_HINT:
                case RSK_WARP_SONG_HINTS:
                case RSK_SCRUB_TEXT_HINT:
                case RSK_KEYRINGS_GERUDO_FORTRESS:
                case RSK_KEYRINGS_FOREST_TEMPLE:
                case RSK_KEYRINGS_FIRE_TEMPLE:
                case RSK_KEYRINGS_WATER_TEMPLE:
                case RSK_KEYRINGS_SHADOW_TEMPLE:
                case RSK_KEYRINGS_SPIRIT_TEMPLE:
                case RSK_KEYRINGS_BOTTOM_OF_THE_WELL:
                case RSK_KEYRINGS_GTG:
                case RSK_KEYRINGS_GANONS_CASTLE:
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
                case RSK_SHOPSANITY_PRICES_AFFORDABLE:
                case RSK_ALL_LOCATIONS_REACHABLE:
                case RSK_TRIFORCE_HUNT:
                case RSK_MQ_DUNGEON_SET:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_GENERIC_OFF);
                    } else if (it.value() == "On") {
                        mOptions[index].SetSelectedIndex(RO_GENERIC_ON);
                    }
                    break;
                case RSK_KEYRINGS:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_KEYRINGS_OFF);
                    } else if (it.value() == "Random") {
                        mOptions[index].SetSelectedIndex(RO_KEYRINGS_RANDOM);
                    } else if (it.value() == "Count") {
                        mOptions[index].SetSelectedIndex(RO_KEYRINGS_COUNT);
                    } else if (it.value() == "Selection") {
                        mOptions[index].SetSelectedIndex(RO_KEYRINGS_SELECTION);
                    }
                    break;
                case RSK_SHUFFLE_MERCHANTS:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_SHUFFLE_MERCHANTS_OFF);
                    } else if (it.value() == "On (No Hints)") {
                        mOptions[index].SetSelectedIndex(RO_SHUFFLE_MERCHANTS_ON_NO_HINT);
                    } else if (it.value() == "On (With Hints)") {
                        mOptions[index].SetSelectedIndex(RO_SHUFFLE_MERCHANTS_ON_HINT);
                    }
                    break;
                // Uses Ammo Drops option for now. "Off" not yet implemented
                case RSK_ENABLE_BOMBCHU_DROPS:
                    if (it.value() == "On") {
                        mOptions[index].SetSelectedIndex(RO_AMMO_DROPS_ON);
                    } else if (it.value() == "On + Bombchu") {
                        mOptions[index].SetSelectedIndex(RO_AMMO_DROPS_ON_PLUS_BOMBCHU);
                    } else if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_AMMO_DROPS_OFF);
                    }
                    break;
                case RSK_STARTING_OCARINA:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_STARTING_OCARINA_OFF);
                    } else if (it.value() == "Fairy Ocarina") {
                        mOptions[index].SetSelectedIndex(RO_STARTING_OCARINA_FAIRY);
                    }
                    break;
                case RSK_ITEM_POOL:
                    if (it.value() == "Plentiful") {
                        mOptions[index].SetSelectedIndex(RO_ITEM_POOL_PLENTIFUL);
                    } else if (it.value() == "Balanced") {
                        mOptions[index].SetSelectedIndex(RO_ITEM_POOL_BALANCED);
                    } else if (it.value() == "Scarce") {
                        mOptions[index].SetSelectedIndex(RO_ITEM_POOL_SCARCE);
                    } else if (it.value() == "Minimal") {
                        mOptions[index].SetSelectedIndex(RO_ITEM_POOL_MINIMAL);
                    }
                    break;
                case RSK_ICE_TRAPS:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_ICE_TRAPS_OFF);
                    } else if (it.value() == "Normal") {
                        mOptions[index].SetSelectedIndex(RO_ICE_TRAPS_NORMAL);
                    } else if (it.value() == "Extra") {
                        mOptions[index].SetSelectedIndex(RO_ICE_TRAPS_EXTRA);
                    } else if (it.value() == "Mayhem") {
                        mOptions[index].SetSelectedIndex(RO_ICE_TRAPS_MAYHEM);
                    } else if (it.value() == "Onslaught") {
                        mOptions[index].SetSelectedIndex(RO_ICE_TRAPS_ONSLAUGHT);
                    }
                    break;
                case RSK_GOSSIP_STONE_HINTS:
                    if (it.value() == "No Hints") {
                        mOptions[index].SetSelectedIndex(RO_GOSSIP_STONES_NONE);
                    } else if (it.value() == "Need Nothing") {
                        mOptions[index].SetSelectedIndex(RO_GOSSIP_STONES_NEED_NOTHING);
                    } else if (it.value() == "Mask of Truth") {
                        mOptions[index].SetSelectedIndex(RO_GOSSIP_STONES_NEED_TRUTH);
                    } else if (it.value() == "Stone of Agony") {
                        mOptions[index].SetSelectedIndex(RO_GOSSIP_STONES_NEED_STONE);
                    }
                    break;
                case RSK_HINT_CLARITY:
                    if (it.value() == "Obscure") {
                        mOptions[index].SetSelectedIndex(RO_HINT_CLARITY_OBSCURE);
                    } else if (it.value() == "Ambiguous") {
                        mOptions[index].SetSelectedIndex(RO_HINT_CLARITY_AMBIGUOUS);
                    } else if (it.value() == "Clear") {
                        mOptions[index].SetSelectedIndex(RO_HINT_CLARITY_CLEAR);
                    }
                    break;
                case RSK_HINT_DISTRIBUTION:
                    if (it.value() == "Useless") {
                        mOptions[index].SetSelectedIndex(RO_HINT_DIST_USELESS);
                    } else if (it.value() == "Balanced") {
                        mOptions[index].SetSelectedIndex(RO_HINT_DIST_BALANCED);
                    } else if (it.value() == "Strong") {
                        mOptions[index].SetSelectedIndex(RO_HINT_DIST_STRONG);
                    } else if (it.value() == "Very Strong") {
                        mOptions[index].SetSelectedIndex(RO_HINT_DIST_VERY_STRONG);
                    }
                    break;
                case RSK_GERUDO_KEYS:
                    if (it.value() == "Vanilla") {
                        mOptions[index].SetSelectedIndex(RO_GERUDO_KEYS_VANILLA);
                    } else if (it.value() == "Any Dungeon") {
                        mOptions[index].SetSelectedIndex(RO_GERUDO_KEYS_ANY_DUNGEON);
                    } else if (it.value() == "Overworld") {
                        mOptions[index].SetSelectedIndex(RO_GERUDO_KEYS_OVERWORLD);
                    } else if (it.value() == "Anywhere") {
                        mOptions[index].SetSelectedIndex(RO_GERUDO_KEYS_ANYWHERE);
                    }
                    break;
                case RSK_KEYSANITY:
                case RSK_BOSS_KEYSANITY:
                case RSK_SHUFFLE_MAPANDCOMPASS:
                    if (it.value() == "Start With") {
                        mOptions[index].SetSelectedIndex(RO_DUNGEON_ITEM_LOC_STARTWITH);
                    } else if (it.value() == "Vanilla") {
                        mOptions[index].SetSelectedIndex(RO_DUNGEON_ITEM_LOC_VANILLA);
                    } else if (it.value() == "Own Dungeon") {
                        mOptions[index].SetSelectedIndex(RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
                    } else if (it.value() == "Any Dungeon") {
                        mOptions[index].SetSelectedIndex(RO_DUNGEON_ITEM_LOC_ANY_DUNGEON);
                    } else if (it.value() == "Overworld") {
                        mOptions[index].SetSelectedIndex(RO_DUNGEON_ITEM_LOC_OVERWORLD);
                    } else if (it.value() == "Anywhere") {
                        mOptions[index].SetSelectedIndex(RO_DUNGEON_ITEM_LOC_ANYWHERE);
                    }
                    break;
                case RSK_GANONS_BOSS_KEY:
                    if (it.value() == "Vanilla") {
                        mOptions[index].SetSelectedIndex(RO_GANON_BOSS_KEY_VANILLA);
                    } else if (it.value() == "Own dungeon") {
                        mOptions[index].SetSelectedIndex(RO_GANON_BOSS_KEY_OWN_DUNGEON);
                    } else if (it.value() == "Start with") {
                        mOptions[index].SetSelectedIndex(RO_GANON_BOSS_KEY_STARTWITH);
                    } else if (it.value() == "Any Dungeon") {
                        mOptions[index].SetSelectedIndex(RO_GANON_BOSS_KEY_ANY_DUNGEON);
                    } else if (it.value() == "Overworld") {
                        mOptions[index].SetSelectedIndex(RO_GANON_BOSS_KEY_OVERWORLD);
                    } else if (it.value() == "Anywhere") {
                        mOptions[index].SetSelectedIndex(RO_GANON_BOSS_KEY_ANYWHERE);
                    } else if (it.value() == "LACS-Vanilla") {
                        mOptions[index].SetSelectedIndex(RO_GANON_BOSS_KEY_LACS_VANILLA);
                    } else if (it.value() == "LACS-Stones") {
                        mOptions[index].SetSelectedIndex(RO_GANON_BOSS_KEY_LACS_STONES);
                    } else if (it.value() == "LACS-Medallions") {
                        mOptions[index].SetSelectedIndex(RO_GANON_BOSS_KEY_LACS_MEDALLIONS);
                    } else if (it.value() == "LACS-Rewards") {
                        mOptions[index].SetSelectedIndex(RO_GANON_BOSS_KEY_LACS_REWARDS);
                    } else if (it.value() == "LACS-Dungeons") {
                        mOptions[index].SetSelectedIndex(RO_GANON_BOSS_KEY_LACS_DUNGEONS);
                    } else if (it.value() == "LACS-Tokens") {
                        mOptions[index].SetSelectedIndex(RO_GANON_BOSS_KEY_LACS_TOKENS);
                    } else if (it.value() == "100 GS Reward") {
                        mOptions[index].SetSelectedIndex(RO_GANON_BOSS_KEY_KAK_TOKENS);
                    } else if (it.value() == "Triforce Hunt") {
                        mOptions[index].SetSelectedIndex(RO_GANON_BOSS_KEY_TRIFORCE_HUNT);
                    }
                    break;
                case RSK_MQ_DUNGEON_RANDOM:
                    if (it.value() == "None") {
                        mOptions[index].SetSelectedIndex(RO_MQ_DUNGEONS_NONE);
                    } else if (it.value() == "Random Number") {
                        mOptions[index].SetSelectedIndex(RO_MQ_DUNGEONS_RANDOM_NUMBER);
                    } else if (it.value() == "Set Number") {
                        mOptions[index].SetSelectedIndex(RO_MQ_DUNGEONS_SET_NUMBER);
                    } else if (it.value() == "Selection Only") {
                        mOptions[index].SetSelectedIndex(RO_MQ_DUNGEONS_SELECTION);
                    }
                    break;
                case RSK_STARTING_CONSUMABLES:
                case RSK_FULL_WALLETS:
                    if (it.value() == "No") {
                        mOptions[index].SetSelectedIndex(RO_GENERIC_NO);
                    } else if (it.value() == "Yes") {
                        mOptions[index].SetSelectedIndex(RO_GENERIC_YES);
                    }
                    break;
                case RSK_SKIP_CHILD_ZELDA:
                case RSK_SKIP_CHILD_STEALTH:
                case RSK_SKIP_EPONA_RACE:
                case RSK_SKIP_TOWER_ESCAPE:
                    if (it.value() == "Don't Skip") {
                        mOptions[index].SetSelectedIndex(RO_GENERIC_DONT_SKIP);
                    } else if (it.value() == "Skip") {
                        mOptions[index].SetSelectedIndex(RO_GENERIC_SKIP);
                    }
                    break;
                case RSK_SHUFFLE_DUNGEON_REWARDS:
                    if (it.value() == "End of dungeons") {
                        mOptions[index].SetSelectedIndex(RO_DUNGEON_REWARDS_END_OF_DUNGEON);
                    } else if (it.value() == "Any dungeon") {
                        mOptions[index].SetSelectedIndex(RO_DUNGEON_REWARDS_ANY_DUNGEON);
                    } else if (it.value() == "Overworld") {
                        mOptions[index].SetSelectedIndex(RO_DUNGEON_REWARDS_OVERWORLD);
                    } else if (it.value() == "Anywhere") {
                        mOptions[index].SetSelectedIndex(RO_DUNGEON_REWARDS_ANYWHERE);
                    }
                    break;
                case RSK_SHUFFLE_SONGS:
                    if (it.value() == "Song locations") {
                        mOptions[index].SetSelectedIndex(RO_SONG_SHUFFLE_SONG_LOCATIONS);
                    } else if (it.value() == "Dungeon rewards") {
                        mOptions[index].SetSelectedIndex(RO_SONG_SHUFFLE_DUNGEON_REWARDS);
                    } else if (it.value() == "Anywhere") {
                        mOptions[index].SetSelectedIndex(RO_SONG_SHUFFLE_ANYWHERE);
                    }
                    break;
                case RSK_SHUFFLE_TOKENS:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_TOKENSANITY_OFF);
                    } else if (it.value() == "Dungeons") {
                        mOptions[index].SetSelectedIndex(RO_TOKENSANITY_DUNGEONS);
                    } else if (it.value() == "Overworld") {
                        mOptions[index].SetSelectedIndex(RO_TOKENSANITY_OVERWORLD);
                    } else if (it.value() == "All Tokens") {
                        mOptions[index].SetSelectedIndex(RO_TOKENSANITY_ALL);
                    }
                    break;
                case RSK_LINKS_POCKET:
                    if (it.value() == "Dungeon Reward") {
                        mOptions[index].SetSelectedIndex(RO_LINKS_POCKET_DUNGEON_REWARD);
                    } else if (it.value() == "Advancement") {
                        mOptions[index].SetSelectedIndex(RO_LINKS_POCKET_ADVANCEMENT);
                    } else if (it.value() == "Anything") {
                        mOptions[index].SetSelectedIndex(RO_LINKS_POCKET_ANYTHING);
                    } else if (it.value() == "Nothing") {
                        mOptions[index].SetSelectedIndex(RO_LINKS_POCKET_NOTHING);
                    }
                    break;
                case RSK_MQ_DUNGEON_COUNT:
                    numericValueString = it.value();
                    mOptions[index].SetSelectedIndex(std::stoi(numericValueString));
                    break;
                case RSK_SHUFFLE_DUNGEON_ENTRANCES:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF);
                    } else if (it.value() == "On") {
                        mOptions[index].SetSelectedIndex(RO_DUNGEON_ENTRANCE_SHUFFLE_ON);
                    } else if (it.value() == "On + Ganon") {
                        mOptions[index].SetSelectedIndex(RO_DUNGEON_ENTRANCE_SHUFFLE_ON_PLUS_GANON);
                    }
                    break;
                case RSK_SHUFFLE_BOSS_ENTRANCES:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF);
                    } else if (it.value() == "Age Restricted") {
                        mOptions[index].SetSelectedIndex(RO_BOSS_ROOM_ENTRANCE_SHUFFLE_AGE_RESTRICTED);
                    } else if (it.value() == "Full") {
                        mOptions[index].SetSelectedIndex(RO_BOSS_ROOM_ENTRANCE_SHUFFLE_FULL);
                    }
                    break;
                case RSK_SHUFFLE_INTERIOR_ENTRANCES:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_INTERIOR_ENTRANCE_SHUFFLE_OFF);
                    } else if (it.value() == "Simple") {
                        mOptions[index].SetSelectedIndex(RO_INTERIOR_ENTRANCE_SHUFFLE_SIMPLE);
                    } else if (it.value() == "All") {
                        mOptions[index].SetSelectedIndex(RO_INTERIOR_ENTRANCE_SHUFFLE_ALL);
                    }
                    break;
                case RSK_SHUFFLE_CHEST_MINIGAME:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_CHEST_GAME_OFF);
                    } else if (it.value() == "On (Separate)") {
                        mOptions[index].SetSelectedIndex(RO_CHEST_GAME_SINGLE_KEYS);
                    } else if (it.value() == "On (Pack)") {
                        mOptions[index].SetSelectedIndex(RO_CHEST_GAME_PACK);
                    }
                    break;
                case RSK_MQ_DEKU_TREE:
                case RSK_MQ_DODONGOS_CAVERN:
                case RSK_MQ_JABU_JABU:
                case RSK_MQ_FOREST_TEMPLE:
                case RSK_MQ_FIRE_TEMPLE:
                case RSK_MQ_WATER_TEMPLE:
                case RSK_MQ_SPIRIT_TEMPLE:
                case RSK_MQ_SHADOW_TEMPLE:
                case RSK_MQ_BOTTOM_OF_THE_WELL:
                case RSK_MQ_ICE_CAVERN:
                case RSK_MQ_GTG:
                case RSK_MQ_GANONS_CASTLE:
                    if (it.value() == "Vanilla") {
                        mOptions[index].SetSelectedIndex(RO_MQ_SET_VANILLA);
                    } else if (it.value() == "Master Quest") {
                        mOptions[index].SetSelectedIndex(RO_MQ_SET_MQ);
                    } else if (it.value() == "Random") {
                        mOptions[index].SetSelectedIndex(RO_MQ_SET_RANDOM);
                    }
                    break;
                default:
                    SPDLOG_DEBUG("No string to enum conversion for option: %s", it.key());
                    break;
            }
        }
    }

    nlohmann::json jsonExcludedLocations = spoilerFileJson["excludedLocations"];
    auto ctx = Rando::Context::GetInstance();

    ctx->AddExcludedOptions();
    for (auto it = jsonExcludedLocations.begin(); it != jsonExcludedLocations.end(); it++) {
        RandomizerCheck rc = ctx->mSpoilerfileCheckNameToEnum[it.value()];
        ctx->GetItemLocation(rc)->GetExcludedOption()->SetSelectedIndex(RO_GENERIC_ON);
    }

    nlohmann::json enabledTricksJson = spoilerFileJson["enabledTricks"];
    for (auto it = enabledTricksJson.begin(); it != enabledTricksJson.end(); it++) {
        std::string numericValueString = it.value();
        RandomizerTrick rt = (RandomizerTrick)std::stoi(numericValueString);
        GetTrickOption(rt).SetSelectedIndex(RO_GENERIC_ON);
    }
}
} // namespace Rando