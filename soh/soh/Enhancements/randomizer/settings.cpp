#include "settings.h"
#include "3drando/trial.hpp"
#include "dungeon.h"
#include "context.h"

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
    //clang-format off
    mOptions[RSK_FOREST] = Option::U8("Forest", {"Closed", "Closed Deku", "Open"}, OptionCategory::Setting, RO_FOREST_CLOSED);
    mOptions[RSK_KAK_GATE] = Option::U8("Kakariko Gate", {"Close", "Open"});
    mOptions[RSK_DOOR_OF_TIME] = Option::U8("Door of Time", {"Closed", "Song only", "Open"});
    mOptions[RSK_ZORAS_FOUNTAIN] = Option::U8("Zora's Fountain", {"Closed", "Closed as child", "Open"});
    mOptions[RSK_GERUDO_FORTRESS] = Option::U8("Gerudo Fortress", {"Normal", "Fast", "Open"});
    mOptions[RSK_RAINBOW_BRIDGE] = Option::U8("Rainbow Bridge", {"Vanilla", "Always open", "Stones", "Medallions", "Dungeons", "Tokens", "Greg"}, OptionCategory::Setting, RO_BRIDGE_VANILLA);
    mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT] = Option::U8("Stone Count", {NumOpts(0, 4)}, OptionCategory::Setting, 1, true);
    mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT] = Option::U8("Medallion Count", {NumOpts(0, 7)}, OptionCategory::Setting, 1, true);
    mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT] = Option::U8("Reward Count", {NumOpts(0, 10)}, OptionCategory::Setting, 1, true);
    mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT] = Option::U8("Dungeon Count", {NumOpts(0, 9)}, OptionCategory::Setting, 1, true);
    mOptions[RSK_RAINBOW_BRIDGE_TOKEN_COUNT] = Option::U8("Token Count", {NumOpts(0, 100)}, OptionCategory::Setting, 1, true);
    mOptions[RSK_BRIDGE_OPTIONS] = Option::U8("Bridge Reward Options", {"Standard Rewards", "Greg as Reward", "Greg as Wildcard"});
    mOptions[RSK_GANONS_TRIALS] = Option::U8("Ganon's Trials", {"Skip", "Set Number", "Random Number"}, OptionCategory::Setting, RO_GANONS_TRIALS_SKIP);
    mOptions[RSK_TRIAL_COUNT] = Option::U8("Ganon's Trials Count", {NumOpts(1, 6)}, OptionCategory::Setting, 1, true);
    mOptions[RSK_STARTING_AGE] = Option::U8("Starting Age", {"Child", "Adult", "Random"}, OptionCategory::Setting, RO_AGE_CHILD);
    mOptions[RSK_SHUFFLE_ENTRANCES] = Option::Bool("Shuffle Entrances");
    mOptions[RSK_SHUFFLE_DUNGEON_ENTRANCES] = Option::U8("Dungeon Entrances", {"Off", "On", "On + Ganon"});
    mOptions[RSK_SHUFFLE_BOSS_ENTRANCES] = Option::U8("Boss Entrances", {"Off", "Age Restricted", "Full"});
    mOptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES] = Option::Bool("Overworld Entrances");
    mOptions[RSK_SHUFFLE_INTERIOR_ENTRANCES] = Option::U8("Interior Entrances", {"Off", "Simple", "All"});
    mOptions[RSK_SHUFFLE_GROTTO_ENTRANCES] = Option::Bool("Grottos Entrances");
    mOptions[RSK_SHUFFLE_OWL_DROPS] = Option::Bool("Owl Drops");
    mOptions[RSK_SHUFFLE_WARP_SONGS] = Option::Bool("Warp Songs");
    mOptions[RSK_SHUFFLE_OVERWORLD_SPAWNS] = Option::Bool("Overworld Spawns");
    mOptions[RSK_MIXED_ENTRANCE_POOLS] = Option::Bool("Mixed Entrance Pools");
    mOptions[RSK_MIX_DUNGEON_ENTRANCES] = Option::Bool("Mix Dungeons");
    mOptions[RSK_MIX_OVERWORLD_ENTRANCES] = Option::Bool("Mix Overworld");
    mOptions[RSK_MIX_INTERIOR_ENTRANCES] = Option::Bool("Mix Interiors");
    mOptions[RSK_MIX_GROTTO_ENTRANCES] = Option::Bool("Mix Grottos");
    mOptions[RSK_DECOUPLED_ENTRANCES] = Option::Bool("Decouple Entrances");
    mOptions[RSK_BOMBCHUS_IN_LOGIC] = Option::Bool("Bombchus in Logic");
    mOptions[RSK_ENABLE_BOMBCHU_DROPS] = Option::U8("Bombchu Drops", {"No", "Yes"});
    // TODO: AmmoDrops and/or HeartDropRefill, combine with/separate Ammo Drops from Bombchu Drops?
    mOptions[RSK_TRIFORCE_HUNT] = Option::Bool("Triforce Hunt");
    mOptions[RSK_TRIFORCE_HUNT_PIECES_TOTAL] = Option::U8("Triforce Hunt Total Pieces", {NumOpts(0, 100)});
    mOptions[RSK_TRIFORCE_HUNT_PIECES_REQUIRED] = Option::U8("Triforce Hunt Required Pieces", {NumOpts(0, 100)});
    mOptions[RSK_MQ_DUNGEON_RANDOM] = Option::U8("MQ Dungeon Setting", {"None", "Set Number", "Random", "Selection Only"});
    mOptions[RSK_MQ_DUNGEON_COUNT] = Option::U8("MQ Dungeon Count", {NumOpts(0, 12)});
    mOptions[RSK_MQ_DUNGEON_SET] = Option::Bool("Set Dungeon Quests");
    mOptions[RSK_MQ_DEKU_TREE] = Option::U8("Deku Tree", {"Vanilla", "Master Quest", "Random"});
    mOptions[RSK_MQ_DODONGOS_CAVERN] = Option::U8("Dodongo's Cavern", {"Vanilla", "Master Quest", "Random"});
    mOptions[RSK_MQ_JABU_JABU] = Option::U8("Jabu-Jabu's Belly", {"Vanilla", "Master Quest", "Random"});
    mOptions[RSK_MQ_FOREST_TEMPLE] = Option::U8("Forest Temple", {"Vanilla", "Master Quest", "Random"});
    mOptions[RSK_MQ_FIRE_TEMPLE] = Option::U8("Fire Temple", {"Vanilla", "Master Quest", "Random"});
    mOptions[RSK_MQ_WATER_TEMPLE] = Option::U8("Water Temple", {"Vanilla", "Master Quest", "Random"});
    mOptions[RSK_MQ_SPIRIT_TEMPLE] = Option::U8("Spirit Temple", {"Vanilla", "Master Quest", "Random"});
    mOptions[RSK_MQ_SHADOW_TEMPLE] = Option::U8("Shadow Temple", {"Vanilla", "Master Quest", "Random"});
    mOptions[RSK_MQ_BOTTOM_OF_THE_WELL] = Option::U8("Bottom of the Well", {"Vanilla", "Master Quest", "Random"});
    mOptions[RSK_MQ_ICE_CAVERN] = Option::U8("Ice Cavern", {"Vanilla", "Master Quest", "Random"});
    mOptions[RSK_MQ_GTG] = Option::U8("Gerudo Training Grounds", {"Vanilla", "Master Quest", "Random"});
    mOptions[RSK_MQ_GANONS_CASTLE] = Option::U8("Ganon's Castle", {"Vanilla", "Master Quest", "Random"});
    mOptions[RSK_SHUFFLE_DUNGEON_REWARDS] = Option::U8("Shuffle Dungeon Rewards", {"End of Dungeons", "Any Dungeon", "Overworld", "Anywhere"});
    mOptions[RSK_LINKS_POCKET] = Option::U8("Link's Pocket", {"Dungeon Reward", "Advancement", "Anything", "Nothing"});
    mOptions[RSK_SHUFFLE_SONGS] = Option::U8("Shuffle Songs", {"Song Locations", "Dungeon Rewards", "Anywhere"});
    mOptions[RSK_SHOPSANITY] = Option::U8("Shopsanity", {"Off", "0 Items", "1 Item", "2 Items", "3 Items", "4 Items", "Random"});
    mOptions[RSK_SHOPSANITY_PRICES] = Option::U8("Shopsanity Prices", {"Balanced", "Starting Wallet", "Adult Wallet", "Giant's Wallet", "Tycoon's Wallet"});
    mOptions[RSK_SHOPSANITY_PRICES_AFFORDABLE] = Option::Bool("Affordable Prices"); // TODO: Can this be added as a selection for above option?
    mOptions[RSK_SHUFFLE_TOKENS] = Option::U8("Tokensanity", {"Off", "Dungeons", "Overworld", "All Tokens"});
    mOptions[RSK_SHUFFLE_SCRUBS] = Option::U8("Scrub Shuffle", {"Off", "Affordable", "Expensive", "Random Prices"});
    mOptions[RSK_SHUFFLE_COWS] = Option::Bool("Shuffle Cows");
    mOptions[RSK_SHUFFLE_KOKIRI_SWORD] = Option::Bool("Shuffle Kokiri Sword");
    mOptions[RSK_SHUFFLE_OCARINA] = Option::Bool("Shuffle Ocarinas");
    mOptions[RSK_SHUFFLE_WEIRD_EGG] = Option::Bool("Shuffle Weird Egg");
    mOptions[RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD] = Option::Bool("Shuffle Gerudo Membership Card");
    mOptions[RSK_SHUFFLE_MAGIC_BEANS] = Option::Bool("Shuffle Magic Beans");
    mOptions[RSK_SHUFFLE_MERCHANTS] = Option::U8("Shuffle Merchants", {"Off", "On (No Hints)", "On (With Hints)"});
    mOptions[RSK_SHUFFLE_FROG_SONG_RUPEES] = Option::Bool("Shuffle Frog Song Rupees");
    mOptions[RSK_SHUFFLE_ADULT_TRADE] = Option::Bool("Shuffle Adult Trade");
    mOptions[RSK_SHUFFLE_CHEST_MINIGAME] = Option::U8("Shuffle Chest Minigame", {"Off", "On (Separate)", "On (Pack)"});
    mOptions[RSK_SHUFFLE_100_GS_REWARD] = Option::Bool("Shuffle 100 GS Reward");
    mOptions[RSK_SHUFFLE_MAPANDCOMPASS] = Option::U8("Maps/Compasses", {"Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
    mOptions[RSK_KEYSANITY] = Option::U8("Small Keys", {"Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
    mOptions[RSK_GERUDO_KEYS] = Option::U8("Gerudo Fortress Keys", {"Vanilla", "Any Dungeon", "Overworld", "Anywhere"});
    mOptions[RSK_BOSS_KEYSANITY] = Option::U8("Boss Keys", {"Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
    mOptions[RSK_GANONS_BOSS_KEY] = Option::U8("Ganon's Boss Key", {"Vanilla", "Own Dungeon", "Start With", "Any Dungeon", "Overworld", "Anywhere", "LACS-Vanilla", "LACS-Stones", "LACS-Medallions", "LACS-Rewards", "LACS-Dungeons", "LACS-Tokens", "Triforce Hunt"}, OptionCategory::Setting, RO_GANON_BOSS_KEY_VANILLA);
    mOptions[RSK_LACS_STONE_COUNT] = Option::U8("Stone Count", {NumOpts(0, 4)}, OptionCategory::Setting, 1, true);
    mOptions[RSK_LACS_MEDALLION_COUNT] = Option::U8("Medallion Count", {NumOpts(0, 7)}, OptionCategory::Setting, 1, true);
    mOptions[RSK_LACS_REWARD_COUNT] = Option::U8("Reward Count", {NumOpts(0, 10)}, OptionCategory::Setting, 1, true);
    mOptions[RSK_LACS_DUNGEON_COUNT] = Option::U8("Dungeon Count", {NumOpts(0, 9)}, OptionCategory::Setting, 1, true);
    mOptions[RSK_LACS_TOKEN_COUNT] = Option::U8("Token Count", {NumOpts(0, 100)}, OptionCategory::Setting, 1, true);
    mOptions[RSK_LACS_OPTIONS] = Option::U8("LACS Reward Options", {"Standard Reward", "Greg as Reward", "Greg as Wildcard"});
    mOptions[RSK_KEYRINGS] = Option::U8("Key Rings", {"Off", "Random", "Count", "Selection"});
    mOptions[RSK_KEYRINGS_RANDOM_COUNT] = Option::U8("Keyring Dungeon Count", {NumOpts(0, 9)}, OptionCategory::Setting, 1);
    mOptions[RSK_KEYRINGS_GERUDO_FORTRESS] = Option::Bool("Gerudo Fortress");
    mOptions[RSK_KEYRINGS_FOREST_TEMPLE] = Option::Bool("Forest Temple");
    mOptions[RSK_KEYRINGS_FIRE_TEMPLE] = Option::Bool("Fire Temple");
    mOptions[RSK_KEYRINGS_WATER_TEMPLE] = Option::Bool("Water Temple");
    mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE] = Option::Bool("Spirit Temple");
    mOptions[RSK_KEYRINGS_SHADOW_TEMPLE] = Option::Bool("Shadow Temple");
    mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL] = Option::Bool("Bottom of the Well");
    mOptions[RSK_KEYRINGS_GTG] = Option::Bool("Gerudo Training Grounds");
    mOptions[RSK_KEYRINGS_GANONS_CASTLE] = Option::Bool("Ganon's Castle");
    mOptions[RSK_SKIP_CHILD_STEALTH] = Option::Bool("Skip Child Stealth", {"Don't Skip", "Skip"}, OptionCategory::Setting, RO_GENERIC_SKIP);
    mOptions[RSK_SKIP_CHILD_ZELDA] = Option::Bool("Skip Child Zelda", {"Don't Skip", "Skip"}, OptionCategory::Setting, RO_GENERIC_DONT_SKIP);
    mOptions[RSK_SKIP_TOWER_ESCAPE] = Option::Bool("Skip Tower Escape", {"Don't Skip", "Skip"});
    mOptions[RSK_SKIP_EPONA_RACE] = Option::Bool("Skip Epona Race", {"Don't Skip", "Skip"});
    mOptions[RSK_SKIP_SCARECROWS_SONG] = Option::Bool("Skip Scarecrow's Song");
    mOptions[RSK_BIG_POE_COUNT] = Option::U8("Big Poe Target Count", {NumOpts(1, 10)});
    mOptions[RSK_CUCCO_COUNT] = Option::U8("Cuccos to return", {NumOpts(0, 7)});
    mOptions[RSK_COMPLETE_MASK_QUEST] = Option::Bool("Complete Mask Quest");
    mOptions[RSK_ENABLE_GLITCH_CUTSCENES] = Option::Bool("Enable Glitch-Useful Cutscenes");
    mOptions[RSK_GOSSIP_STONE_HINTS] = Option::U8("Gossip Stone Hints", {"No Hints", "Need Nothing", "Mask of Truth", "Stone of Agony"}, OptionCategory::Setting, RO_GOSSIP_STONES_NEED_NOTHING);
    mOptions[RSK_HINT_CLARITY] = Option::U8("Hint Clarity", {"Obscure", "Ambiguous", "Clear"});
    mOptions[RSK_HINT_DISTRIBUTION] = Option::U8("Hint Distribution", {"Useless", "Balanced", "Strong", "Very Strong"}, OptionCategory::Setting, RO_HINT_DIST_BALANCED);
    mOptions[RSK_TOT_ALTAR_HINT] = Option::Bool("ToT Altar Hint", {"Off", "On"}, OptionCategory::Setting, RO_GENERIC_ON);
    mOptions[RSK_LIGHT_ARROWS_HINT] = Option::Bool("Light Arrow Hint", {"Off", "On"}, OptionCategory::Setting, RO_GENERIC_ON);
    mOptions[RSK_DAMPES_DIARY_HINT] = Option::Bool("Dampe's Diary Hint");
    mOptions[RSK_GREG_HINT] = Option::Bool("Greg the Green Rupee Hint");
    mOptions[RSK_SARIA_HINT] = Option::Bool("Saria's Hint");
    mOptions[RSK_FROGS_HINT] = Option::Bool("Frog Ocarina Game Hint");
    mOptions[RSK_WARP_SONG_HINTS] = Option::Bool("Warp Song Hints");
    mOptions[RSK_KAK_10_SKULLS_HINT] = Option::Bool("10 GS Hint");
    mOptions[RSK_KAK_20_SKULLS_HINT] = Option::Bool("20 GS Hint");
    mOptions[RSK_KAK_30_SKULLS_HINT] = Option::Bool("30 GS Hint");
    mOptions[RSK_KAK_40_SKULLS_HINT] = Option::Bool("40 GS Hint");
    mOptions[RSK_KAK_50_SKULLS_HINT] = Option::Bool("50 GS Hint");
    mOptions[RSK_SCRUB_TEXT_HINT] = Option::Bool("Scrub Hint Text");
    // TODO: Compasses show rewards/woth, maps show dungeon mode
    mOptions[RSK_BLUE_FIRE_ARROWS] = Option::Bool("Blue Fire Arrows");
    mOptions[RSK_SUNLIGHT_ARROWS] = Option::Bool("Sunlight Arrows");
    mOptions[RSK_ITEM_POOL] = Option::U8("Item Pool", {"Plentiful", "Balanced", "Scarce", "Minimal"}, OptionCategory::Setting, RO_ITEM_POOL_BALANCED);
    mOptions[RSK_ICE_TRAPS] = Option::U8("Ice Traps", {"Off", "Normal", "Extra", "Mayhem", "Onslaught"}, OptionCategory::Setting, RO_ICE_TRAPS_NORMAL);
    // TODO: Remove Double Defense, Progressive Goron Sword
    mOptions[RSK_STARTING_OCARINA] = Option::U8("Start with Ocarina", {"Off", "Fairy Ocarina", "Ocarina of Time"});
    mOptions[RSK_STARTING_DEKU_SHIELD] = Option::Bool("Start with Deku Shield");
    mOptions[RSK_STARTING_KOKIRI_SWORD] = Option::Bool("Start with Kokiri Sword");
    mOptions[RSK_STARTING_CONSUMABLES] = Option::Bool("Start with Consumables", {"No", "Yes"});
    mOptions[RSK_FULL_WALLETS] = Option::Bool("Full Wallets", {"No", "Yes"});
    mOptions[RSK_STARTING_ZELDAS_LULLABY] = Option::Bool("Start with Zelda's Lullaby");
    mOptions[RSK_STARTING_EPONAS_SONG] = Option::Bool("Start with Epona's Song");
    mOptions[RSK_STARTING_SARIAS_SONG] = Option::Bool("Start with Epona's Song");
    mOptions[RSK_STARTING_SUNS_SONG] = Option::Bool("Start with Sun's Song");
    mOptions[RSK_STARTING_SONG_OF_TIME] = Option::Bool("Start with Song of Time");
    mOptions[RSK_STARTING_SONG_OF_STORMS] = Option::Bool("Start with Song of Storms");
    mOptions[RSK_STARTING_MINUET_OF_FOREST] = Option::Bool("Start with Minuet of Forest");
    mOptions[RSK_STARTING_BOLERO_OF_FIRE] = Option::Bool("Start with Bolero of Fire");
    mOptions[RSK_STARTING_SERENADE_OF_WATER] = Option::Bool("Start with Serenade of Water");
    mOptions[RSK_STARTING_REQUIEM_OF_SPIRIT] = Option::Bool("Start with Requiem of Spirit");
    mOptions[RSK_STARTING_NOCTURNE_OF_SHADOW] = Option::Bool("Start with Nocturne of Shadow");
    mOptions[RSK_STARTING_PRELUDE_OF_LIGHT] = Option::Bool("Start with Prelude of Light");
    mOptions[RSK_STARTING_SKULLTULA_TOKEN] = Option::U8("Gold Skulltula Tokens", {NumOpts(0, 100)});
    mOptions[RSK_STARTING_HEARTS] = Option::U8("Hearts", {NumOpts(1, 20)}, OptionCategory::Setting, 2);
    // TODO: Remainder of Starting Items
    mOptions[RSK_LOGIC_RULES] = Option::U8("Logic", {"Glitchless", "Glitched", "No Logic", "Vanilla"});
    mOptions[RSK_ALL_LOCATIONS_REACHABLE] = Option::Bool("All Locations Reachable", {"Off", "On"}, OptionCategory::Setting, RO_GENERIC_ON);
    mOptions[RSK_SKULLS_SUNS_SONG] = Option::Bool("Night Skulltula's Expect Sun's Song");
    mOptions[RSK_DAMAGE_MULTIPLIER] = Option::U8("Damage Multiplier", {"x1/2", "x1", "x2", "x4", "x8", "x16", "OHKO"}, OptionCategory::Setting, RO_DAMAGE_MULTIPLIER_DEFAULT);
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
    return mSeed;
}

void Settings::SetSeed(uint32_t seed) {
    mSeed = seed;
}

const std::array<OptionGroup, RSG_MAX>& Settings::GetOptionGroups() {
    return mOptionGroups;
}

const OptionGroup& Settings::GetOptionGroup(RandomizerSettingGroupKey key) {
    return mOptionGroups[key];
}

void Settings::UpdateSettings(std::unordered_map<RandomizerSettingKey, uint8_t> cvarSettings, std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks) {
    auto ctx = Rando::Context::GetInstance();
    if (!ctx->IsSpoilerLoaded()) {
        // If we've loaded a spoiler file, the settings have already been populated, so we
        // only need to do things like resolve the starting age or determine MQ dungeons.
        // Any logic dependent on cvarSettings should go in this if statement
        for (auto [key, value] : cvarSettings) {
            mOptions[key].SetSelectedIndex(value);
        }
        switch (cvarSettings[RSK_LOGIC_RULES]) {
            case RO_LOGIC_GLITCHLESS:
                mOptions[RSK_LOGIC_RULES].SetSelectedIndex(RO_LOGIC_GLITCHLESS);
                break;
            case RO_LOGIC_NO_LOGIC:
                mOptions[RSK_LOGIC_RULES].SetSelectedIndex(RO_LOGIC_NO_LOGIC);
                break;
        }
        
        // if we skip child zelda, we start with zelda's letter, and malon starts
        // at the ranch, so we should *not* shuffle the weird egg
        if (cvarSettings[RSK_SKIP_CHILD_ZELDA]) {
            mOptions[RSK_SKIP_CHILD_ZELDA].SetSelectedIndex(true);
            mOptions[RSK_SHUFFLE_WEIRD_EGG].SetSelectedIndex(0);
        } else {
            mOptions[RSK_SKIP_CHILD_ZELDA].SetSelectedIndex(false);
            mOptions[RSK_SHUFFLE_WEIRD_EGG].SetSelectedIndex(cvarSettings[RSK_SHUFFLE_WEIRD_EGG]);
        }
        // Force 100 GS Shuffle if that's where Ganon's Boss Key is
        if (cvarSettings[RSK_GANONS_BOSS_KEY] == RO_GANON_BOSS_KEY_KAK_TOKENS) {
            mOptions[RSK_SHUFFLE_100_GS_REWARD].SetSelectedIndex(1);
        } else {
            mOptions[RSK_SHUFFLE_100_GS_REWARD].SetSelectedIndex(cvarSettings[RSK_SHUFFLE_100_GS_REWARD]);
        }

        if (cvarSettings[RSK_TRIFORCE_HUNT]) {
            mOptions[RSK_GANONS_BOSS_KEY].SetSelectedIndex(RO_GANON_BOSS_KEY_TRIFORCE_HUNT);
        } else {
            mOptions[RSK_GANONS_BOSS_KEY].SetSelectedIndex(cvarSettings[RSK_GANONS_BOSS_KEY]);
        }
        // ImGui has a slider which returns the actual number, which is off by one since
        // the ImGui slider can't go down to 0 (the first index aka 1 big poe).
        mOptions[RSK_BIG_POE_COUNT].SetSelectedIndex(cvarSettings[RSK_BIG_POE_COUNT] - 1);
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
    auto trials = Trial::trialList;
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
}