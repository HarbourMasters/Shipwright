#include "settings.h"

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

Settings::Settings() {
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
    mOptions[RSK_GANONS_TRIALS] = Option::Bool("Ganon's Trials", {"Skip", "Set Number", "Random Number"}, OptionCategory::Setting, RO_GANONS_TRIALS_SKIP);
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
    mOptions[RSK_ENABLE_BOMBCHU_DROPS] = Option::Bool("Bombchu Drops");
    // TODO: AmmoDrops and/or HeartDropRefill, combine with/separate Ammo Drops from Bombchu Drops?
    mOptions[RSK_TRIFORCE_HUNT] = Option::Bool("Triforce Hunt");
    mOptions[RSK_TRIFORCE_HUNT_PIECES_TOTAL] = Option::U8("Triforce Hunt Total Pieces", {NumOpts(0, 100)});
    mOptions[RSK_TRIFORCE_HUNT_PIECES_REQUIRED] = Option::U8("Triforce Hunt Required Pieces", {NumOpts(0, 100)});
    mOptions[RSK_MQ_DUNGEON_COUNT] = Option::U8("MQ Dungeon Count", {MultiVecOpts({NumOpts(0, 12), {"Random"}, {"Selection"}})});
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
    mOptions[RSK_DAMAGE_MULTIPLIER] = Option::U8("Damange Multiplier", {"x1/2", "x1", "x2", "x4", "x8", "x16", "OHKO"}, OptionCategory::Setting, RO_DAMAGE_MULTIPLIER_DEFAULT)
    // clang-format on

    mExcludeLocationsOptionsGroups.reserve(SPOILER_COLLECTION_GROUP_COUNT);

    trickOptions[RT_VISIBLE_COLLISION] = Option::LogicTrick(std::to_string(RT_VISIBLE_COLLISION));
    trickOptions[RT_GROTTOS_WITHOUT_AGONY] = Option::LogicTrick(std::to_string(RT_GROTTOS_WITHOUT_AGONY));
    trickOptions[RT_FEWER_TUNIC_REQUIREMENTS] = Option::LogicTrick(std::to_string(RT_FEWER_TUNIC_REQUIREMENTS));
    trickOptions[RT_RUSTED_SWITCHES] = Option::LogicTrick(std::to_string(RT_RUSTED_SWITCHES));
    trickOptions[RT_FLAMING_CHESTS] = Option::LogicTrick(std::to_string(RT_FLAMING_CHESTS));
    trickOptions[RT_BUNNY_HOOD_JUMPS] = Option::LogicTrick(std::to_string(RT_BUNNY_HOOD_JUMPS));
    trickOptions[RT_DAMAGE_BOOST_SIMPLE] = Option::LogicTrick(std::to_string(RT_DAMAGE_BOOST_SIMPLE));
    trickOptions[RT_HOVER_BOOST_SIMPLE] = Option::LogicTrick(std::to_string(RT_HOVER_BOOST_SIMPLE));
    trickOptions[RT_KF_ADULT_GS] = Option::LogicTrick(std::to_string(RT_KF_ADULT_GS));
    trickOptions[RT_LW_BRIDGE] = Option::LogicTrick(std::to_string(RT_LW_BRIDGE));
    trickOptions[RT_LW_MIDO_BACKFLIP] = Option::LogicTrick(std::to_string(RT_LW_MIDO_BACKFLIP));
    trickOptions[RT_LW_GS_BEAN] = Option::LogicTrick(std::to_string(RT_LW_GS_BEAN));
    trickOptions[RT_HC_STORMS_GS] = Option::LogicTrick(std::to_string(RT_HC_STORMS_GS));
    trickOptions[RT_KAK_MAN_ON_ROOF] = Option::LogicTrick(std::to_string(RT_KAK_MAN_ON_ROOF));
    trickOptions[RT_KAK_TOWER_GS] = Option::LogicTrick(std::to_string(RT_KAK_TOWER_GS));
    trickOptions[RT_KAK_ADULT_WINDMILL_POH] = Option::LogicTrick(std::to_string(RT_KAK_ADULT_WINDMILL_POH));
    trickOptions[RT_KAK_CHILD_WINDMILL_POH] = Option::LogicTrick(std::to_string(RT_KAK_CHILD_WINDMILL_POH));
    trickOptions[RT_KAK_ROOFTOP_GS] = Option::LogicTrick(std::to_string(RT_KAK_ROOFTOP_GS));
    trickOptions[RT_GY_POH] = Option::LogicTrick(std::to_string(RT_GY_POH));
    trickOptions[RT_GY_CHILD_DAMPE_RACE_POH] = Option::LogicTrick(std::to_string(RT_GY_CHILD_DAMPE_RACE_POH));
    trickOptions[RT_GY_SHADOW_FIRE_ARROWS] = Option::LogicTrick(std::to_string(RT_GY_SHADOW_FIRE_ARROWS));
    trickOptions[RT_DMT_SOIL_GS] = Option::LogicTrick(std::to_string(RT_DMT_SOIL_GS));
    trickOptions[RT_DMT_BOMBABLE] = Option::LogicTrick(std::to_string(RT_DMT_BOMBABLE));
    trickOptions[RT_DMT_HOOKSHOT_LOWER_GS] = Option::LogicTrick(std::to_string(RT_DMT_HOOKSHOT_LOWER_GS));
    trickOptions[RT_DMT_HOVERS_LOWER_GS] = Option::LogicTrick(std::to_string(RT_DMT_HOVERS_LOWER_GS));
    trickOptions[RT_DMT_BEAN_LOWER_GS] = Option::LogicTrick(std::to_string(RT_DMT_BEAN_LOWER_GS));
    trickOptions[RT_DMT_JS_LOWER_GS] = Option::LogicTrick(std::to_string(RT_DMT_JS_LOWER_GS));
    trickOptions[RT_DMT_CLIMB_HOVERS] = Option::LogicTrick(std::to_string(RT_DMT_CLIMB_HOVERS));
    trickOptions[RT_DMT_UPPER_GS] = Option::LogicTrick(std::to_string(RT_DMT_UPPER_GS));
    trickOptions[RT_DMT_BOLERO_BIGGORON] = Option::LogicTrick(std::to_string(RT_DMT_BOLERO_BIGGORON));
    trickOptions[RT_GC_POT] = Option::LogicTrick(std::to_string(RT_GC_POT));
    trickOptions[RT_GC_POT_STRENGTH] = Option::LogicTrick(std::to_string(RT_GC_POT_STRENGTH));
    trickOptions[RT_GC_ROLLING_STRENGTH] = Option::LogicTrick(std::to_string(RT_GC_ROLLING_STRENGTH));
    trickOptions[RT_GC_LEFTMOST] = Option::LogicTrick(std::to_string(RT_GC_LEFTMOST));
    trickOptions[RT_GC_GROTTO] = Option::LogicTrick(std::to_string(RT_GC_GROTTO));
    trickOptions[RT_GC_LINK_GORON_DINS] = Option::LogicTrick(std::to_string(RT_GC_LINK_GORON_DINS));
    trickOptions[RT_DMC_HOVER_BEAN_POH] = Option::LogicTrick(std::to_string(RT_DMC_HOVER_BEAN_POH));
    trickOptions[RT_DMC_BOLERO_JUMP] = Option::LogicTrick(std::to_string(RT_DMC_BOLERO_JUMP));
    trickOptions[RT_DMC_BOULDER_JS] = Option::LogicTrick(std::to_string(RT_DMC_BOULDER_JS));
    trickOptions[RT_DMC_BOULDER_SKIP] = Option::LogicTrick(std::to_string(RT_DMC_BOULDER_SKIP));
    trickOptions[RT_ZR_LOWER] = Option::LogicTrick(std::to_string(RT_ZR_LOWER));
    trickOptions[RT_ZR_UPPER] = Option::LogicTrick(std::to_string(RT_ZR_UPPER));
    trickOptions[RT_ZR_HOVERS] = Option::LogicTrick(std::to_string(RT_ZR_HOVERS));
    trickOptions[RT_ZR_CUCCO] = Option::LogicTrick(std::to_string(RT_ZR_CUCCO));
    trickOptions[RT_ZD_KING_ZORA_SKIP] = Option::LogicTrick(std::to_string(RT_ZD_KING_ZORA_SKIP));
    trickOptions[RT_ZD_GS] = Option::LogicTrick(std::to_string(RT_ZD_GS));
    trickOptions[RT_LH_LAB_WALL_GS] = Option::LogicTrick(std::to_string(RT_LH_LAB_WALL_GS));
    trickOptions[RT_LH_LAB_DIVING] = Option::LogicTrick(std::to_string(RT_LH_LAB_DIVING));
    trickOptions[RT_LH_WATER_HOOKSHOT] = Option::LogicTrick(std::to_string(RT_LH_WATER_HOOKSHOT));
    trickOptions[RT_GV_CRATE_HOVERS] = Option::LogicTrick(std::to_string(RT_GV_CRATE_HOVERS));
    trickOptions[RT_GF_KITCHEN] = Option::LogicTrick(std::to_string(RT_GF_KITCHEN));
    trickOptions[RT_GF_JUMP] = Option::LogicTrick(std::to_string(RT_GF_JUMP));
    trickOptions[RT_HW_BUNNY_CROSSING] = Option::LogicTrick(std::to_string(RT_HW_BUNNY_CROSSING));
    trickOptions[RT_HW_CROSSING] = Option::LogicTrick(std::to_string(RT_HW_CROSSING));
    trickOptions[RT_LENS_HW] = Option::LogicTrick(std::to_string(RT_LENS_HW));
    trickOptions[RT_HW_REVERSE] = Option::LogicTrick(std::to_string(RT_HW_REVERSE));
    trickOptions[RT_COLOSSUS_GS] = Option::LogicTrick(std::to_string(RT_COLOSSUS_GS));
    trickOptions[RT_DEKU_BASEMENT_GS] = Option::LogicTrick(std::to_string(RT_DEKU_BASEMENT_GS));
    trickOptions[RT_DEKU_B1_SKIP] = Option::LogicTrick(std::to_string(RT_DEKU_B1_SKIP));
    trickOptions[RT_DEKU_B1_BOW_WEBS] = Option::LogicTrick(std::to_string(RT_DEKU_B1_BOW_WEBS));
    trickOptions[RT_DEKU_MQ_COMPASS_GS] = Option::LogicTrick(std::to_string(RT_DEKU_MQ_COMPASS_GS));
    trickOptions[RT_DEKU_MQ_LOG] = Option::LogicTrick(std::to_string(RT_DEKU_MQ_LOG));
    trickOptions[RT_DC_SCARECROW_GS] = Option::LogicTrick(std::to_string(RT_DC_SCARECROW_GS));
    trickOptions[RT_DC_VINES_GS] = Option::LogicTrick(std::to_string(RT_DC_VINES_GS));
    trickOptions[RT_DC_STAIRCASE] = Option::LogicTrick(std::to_string(RT_DC_STAIRCASE));
    trickOptions[RT_DC_SLINGSHOT_SKIP] = Option::LogicTrick(std::to_string(RT_DC_SLINGSHOT_SKIP));
    trickOptions[RT_DC_SCRUB_ROOM] = Option::LogicTrick(std::to_string(RT_DC_SCRUB_ROOM));
    trickOptions[RT_DC_JUMP] = Option::LogicTrick(std::to_string(RT_DC_JUMP));
    trickOptions[RT_DC_HAMMER_FLOOR] = Option::LogicTrick(std::to_string(RT_DC_HAMMER_FLOOR));
    trickOptions[RT_DC_MQ_CHILD_BOMBS] = Option::LogicTrick(std::to_string(RT_DC_MQ_CHILD_BOMBS));
    trickOptions[RT_DC_MQ_CHILD_EYES] = Option::LogicTrick(std::to_string(RT_DC_MQ_CHILD_EYES));
    trickOptions[RT_DC_MQ_ADULT_EYES] = Option::LogicTrick(std::to_string(RT_DC_MQ_ADULT_EYES));
    trickOptions[RT_JABU_ALCOVE_JUMP_DIVE] = Option::LogicTrick(std::to_string(RT_JABU_ALCOVE_JUMP_DIVE));
    trickOptions[RT_JABU_BOSS_HOVER] = Option::LogicTrick(std::to_string(RT_JABU_BOSS_HOVER));
    trickOptions[RT_JABU_NEAR_BOSS_RANGED] = Option::LogicTrick(std::to_string(RT_JABU_NEAR_BOSS_RANGED));
    trickOptions[RT_JABU_NEAR_BOSS_EXPLOSIVES] = Option::LogicTrick(std::to_string(RT_JABU_NEAR_BOSS_EXPLOSIVES));
    trickOptions[RT_LENS_JABU_MQ] = Option::LogicTrick(std::to_string(RT_LENS_JABU_MQ));
    trickOptions[RT_JABU_MQ_RANG_JUMP] = Option::LogicTrick(std::to_string(RT_JABU_MQ_RANG_JUMP));
    trickOptions[RT_JABU_MQ_SOT_GS] = Option::LogicTrick(std::to_string(RT_JABU_MQ_SOT_GS));
    trickOptions[RT_LENS_BOTW] = Option::LogicTrick(std::to_string(RT_LENS_BOTW));
    trickOptions[RT_BOTW_CHILD_DEADHAND] = Option::LogicTrick(std::to_string(RT_BOTW_CHILD_DEADHAND));
    trickOptions[RT_BOTW_BASEMENT] = Option::LogicTrick(std::to_string(RT_BOTW_BASEMENT));
    trickOptions[RT_BOTW_MQ_PITS] = Option::LogicTrick(std::to_string(RT_BOTW_MQ_PITS));
    trickOptions[RT_BOTW_MQ_DEADHAND_KEY] = Option::LogicTrick(std::to_string(RT_BOTW_MQ_DEADHAND_KEY));
    trickOptions[RT_FOREST_FIRST_GS] = Option::LogicTrick(std::to_string(RT_FOREST_FIRST_GS));
    trickOptions[RT_FOREST_OUTDOORS_EAST_GS] = Option::LogicTrick(std::to_string(RT_FOREST_OUTDOORS_EAST_GS));
    trickOptions[RT_FOREST_VINES] = Option::LogicTrick(std::to_string(RT_FOREST_VINES));
    trickOptions[RT_FOREST_OUTDOORS_LEDGE] = Option::LogicTrick(std::to_string(RT_FOREST_OUTDOORS_LEDGE));
    trickOptions[RT_FOREST_DOORFRAME] = Option::LogicTrick(std::to_string(RT_FOREST_DOORFRAME));
    trickOptions[RT_FOREST_OUTSIDE_BACKDOOR] = Option::LogicTrick(std::to_string(RT_FOREST_OUTSIDE_BACKDOOR));
    trickOptions[RT_FOREST_MQ_WELL_SWIM] = Option::LogicTrick(std::to_string(RT_FOREST_MQ_WELL_SWIM));
    trickOptions[RT_FOREST_MQ_BLOCK_PUZZLE] = Option::LogicTrick(std::to_string(RT_FOREST_MQ_BLOCK_PUZZLE));
    trickOptions[RT_FOREST_MQ_JS_HALLWAY_SWITCH] = Option::LogicTrick(std::to_string(RT_FOREST_MQ_JS_HALLWAY_SWITCH));
    trickOptions[RT_FOREST_MQ_HOOKSHOT_HALLWAY_SWITCH] = Option::LogicTrick(std::to_string(RT_FOREST_MQ_HOOKSHOT_HALLWAY_SWITCH));
    trickOptions[RT_FOREST_MQ_RANG_HALLWAY_SWITCH] = Option::LogicTrick(std::to_string(RT_FOREST_MQ_RANG_HALLWAY_SWITCH));
    trickOptions[RT_FIRE_BOSS_DOOR_JUMP] = Option::LogicTrick(std::to_string(RT_FIRE_BOSS_DOOR_JUMP));
    trickOptions[RT_FIRE_SOT] = Option::LogicTrick(std::to_string(RT_FIRE_SOT));
    trickOptions[RT_FIRE_STRENGTH] = Option::LogicTrick(std::to_string(RT_FIRE_STRENGTH));
    trickOptions[RT_FIRE_SCARECROW] = Option::LogicTrick(std::to_string(RT_FIRE_SCARECROW));
    trickOptions[RT_FIRE_FLAME_MAZE] = Option::LogicTrick(std::to_string(RT_FIRE_FLAME_MAZE));
    trickOptions[RT_FIRE_MQ_NEAR_BOSS] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_NEAR_BOSS));
    trickOptions[RT_FIRE_MQ_BLOCKED_CHEST] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_BLOCKED_CHEST));
    trickOptions[RT_FIRE_MQ_BK_CHEST] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_BK_CHEST));
    trickOptions[RT_FIRE_MQ_CLIMB] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_CLIMB));
    trickOptions[RT_FIRE_MQ_MAZE_SIDE_ROOM] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_MAZE_SIDE_ROOM));
    trickOptions[RT_FIRE_MQ_MAZE_HOVERS] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_MAZE_HOVERS));
    trickOptions[RT_FIRE_MQ_MAZE_JUMP] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_MAZE_JUMP));
    trickOptions[RT_FIRE_MQ_ABOVE_MAZE_GS] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_ABOVE_MAZE_GS));
    trickOptions[RT_FIRE_MQ_FLAME_MAZE] = Option::LogicTrick(std::to_string(RT_FIRE_MQ_FLAME_MAZE));
    trickOptions[RT_WATER_LONGSHOT_TORCH] = Option::LogicTrick(std::to_string(RT_WATER_LONGSHOT_TORCH));
    trickOptions[RT_WATER_CRACKED_WALL_HOVERS] = Option::LogicTrick(std::to_string(RT_WATER_CRACKED_WALL_HOVERS));
    trickOptions[RT_WATER_CRACKED_WALL] = Option::LogicTrick(std::to_string(RT_WATER_CRACKED_WALL));
    trickOptions[RT_WATER_BK_REGION] = Option::LogicTrick(std::to_string(RT_WATER_BK_REGION));
    trickOptions[RT_WATER_NORTH_BASEMENT_LEDGE_JUMP] = Option::LogicTrick(std::to_string(RT_WATER_NORTH_BASEMENT_LEDGE_JUMP));
    trickOptions[RT_WATER_BK_JUMP_DIVE] = Option::LogicTrick(std::to_string(RT_WATER_BK_JUMP_DIVE));
    trickOptions[RT_WATER_FW_CENTRAL_GS] = Option::LogicTrick(std::to_string(RT_WATER_FW_CENTRAL_GS));
    trickOptions[RT_WATER_IRONS_CENTRAL_GS] = Option::LogicTrick(std::to_string(RT_WATER_IRONS_CENTRAL_GS));
    trickOptions[RT_WATER_CENTRAL_BOW] = Option::LogicTrick(std::to_string(RT_WATER_CENTRAL_BOW));
    trickOptions[RT_WATER_HOOKSHOT_FALLING_PLATFORM_GS] = Option::LogicTrick(std::to_string(RT_WATER_HOOKSHOT_FALLING_PLATFORM_GS));
    trickOptions[RT_WATER_RANG_FALLING_PLATFORM_GS] = Option::LogicTrick(std::to_string(RT_WATER_RANG_FALLING_PLATFORM_GS));
    trickOptions[RT_WATER_RIVER_GS] = Option::LogicTrick(std::to_string(RT_WATER_RIVER_GS));
    trickOptions[RT_WATER_DRAGON_JUMP_DIVE] = Option::LogicTrick(std::to_string(RT_WATER_DRAGON_JUMP_DIVE));
    trickOptions[RT_WATER_ADULT_DRAGON] = Option::LogicTrick(std::to_string(RT_WATER_ADULT_DRAGON));
    trickOptions[RT_WATER_CHILD_DRAGON] = Option::LogicTrick(std::to_string(RT_WATER_CHILD_DRAGON));
    trickOptions[RT_WATER_MQ_CENTRAL_PILLAR] = Option::LogicTrick(std::to_string(RT_WATER_MQ_CENTRAL_PILLAR));
    trickOptions[RT_WATER_MQ_LOCKED_GS] = Option::LogicTrick(std::to_string(RT_WATER_MQ_LOCKED_GS));
    trickOptions[RT_LENS_SHADOW] = Option::LogicTrick(std::to_string(RT_LENS_SHADOW));
    trickOptions[RT_LENS_SHADOW_PLATFORM] = Option::LogicTrick(std::to_string(RT_LENS_SHADOW_PLATFORM));
    trickOptions[RT_LENS_BONGO] = Option::LogicTrick(std::to_string(RT_LENS_BONGO));
    trickOptions[RT_SHADOW_UMBRELLA] = Option::LogicTrick(std::to_string(RT_SHADOW_UMBRELLA));
    trickOptions[RT_SHADOW_UMBRELLA_GS] = Option::LogicTrick(std::to_string(RT_SHADOW_UMBRELLA_GS));
    trickOptions[RT_SHADOW_FREESTANDING_KEY] = Option::LogicTrick(std::to_string(RT_SHADOW_FREESTANDING_KEY));
    trickOptions[RT_SHADOW_STATUE] = Option::LogicTrick(std::to_string(RT_SHADOW_STATUE));
    trickOptions[RT_SHADOW_BONGO] = Option::LogicTrick(std::to_string(RT_SHADOW_BONGO));
    trickOptions[RT_LENS_SHADOW_MQ] = Option::LogicTrick(std::to_string(RT_LENS_SHADOW_MQ));
    trickOptions[RT_LENS_SHADOW_MQ_INVISIBLE_BLADES] = Option::LogicTrick(std::to_string(RT_LENS_SHADOW_MQ_INVISIBLE_BLADES));
    trickOptions[RT_LENS_SHADOW_MQ_PLATFORM] = Option::LogicTrick(std::to_string(RT_LENS_SHADOW_MQ_PLATFORM));
    trickOptions[RT_LENS_SHADOW_MQ_DEADHAND] = Option::LogicTrick(std::to_string(RT_LENS_SHADOW_MQ_DEADHAND));
    trickOptions[RT_SHADOW_MQ_GAP] = Option::LogicTrick(std::to_string(RT_SHADOW_MQ_GAP));
    trickOptions[RT_SHADOW_MQ_INVISIBLE_BLADES] = Option::LogicTrick(std::to_string(RT_SHADOW_MQ_INVISIBLE_BLADES));
    trickOptions[RT_SHADOW_MQ_HUGE_PIT] = Option::LogicTrick(std::to_string(RT_SHADOW_MQ_HUGE_PIT));
    trickOptions[RT_SHADOW_MQ_WINDY_WALKWAY] = Option::LogicTrick(std::to_string(RT_SHADOW_MQ_WINDY_WALKWAY));
    trickOptions[RT_LENS_SPIRIT] = Option::LogicTrick(std::to_string(RT_LENS_SPIRIT));
    trickOptions[RT_SPIRIT_CHILD_CHU] = Option::LogicTrick(std::to_string(RT_SPIRIT_CHILD_CHU));
    trickOptions[RT_SPIRIT_LOBBY_GS] = Option::LogicTrick(std::to_string(RT_SPIRIT_LOBBY_GS));
    trickOptions[RT_SPIRIT_LOWER_ADULT_SWITCH] = Option::LogicTrick(std::to_string(RT_SPIRIT_LOWER_ADULT_SWITCH));
    trickOptions[RT_SPIRIT_LOBBY_JUMP] = Option::LogicTrick(std::to_string(RT_SPIRIT_LOBBY_JUMP));
    trickOptions[RT_SPIRIT_PLATFORM_HOOKSHOT] = Option::LogicTrick(std::to_string(RT_SPIRIT_PLATFORM_HOOKSHOT));
    trickOptions[RT_SPIRIT_MAP_CHEST] = Option::LogicTrick(std::to_string(RT_SPIRIT_MAP_CHEST));
    trickOptions[RT_SPIRIT_SUN_CHEST] = Option::LogicTrick(std::to_string(RT_SPIRIT_SUN_CHEST));
    trickOptions[RT_SPIRIT_WALL] = Option::LogicTrick(std::to_string(RT_SPIRIT_WALL));
    trickOptions[RT_LENS_SPIRIT_MQ] = Option::LogicTrick(std::to_string(RT_LENS_SPIRIT_MQ));
    trickOptions[RT_SPIRIT_MQ_SUN_BLOCK_SOT] = Option::LogicTrick(std::to_string(RT_SPIRIT_MQ_SUN_BLOCK_SOT));
    trickOptions[RT_SPIRIT_MQ_SUN_BLOCK_GS] = Option::LogicTrick(std::to_string(RT_SPIRIT_MQ_SUN_BLOCK_GS));
    trickOptions[RT_SPIRIT_MQ_LOWER_ADULT] = Option::LogicTrick(std::to_string(RT_SPIRIT_MQ_LOWER_ADULT));
    trickOptions[RT_SPIRIT_MQ_FROZEN_EYE] = Option::LogicTrick(std::to_string(RT_SPIRIT_MQ_FROZEN_EYE));
    trickOptions[RT_ICE_BLOCK_GS] = Option::LogicTrick(std::to_string(RT_ICE_BLOCK_GS));
    trickOptions[RT_ICE_MQ_RED_ICE_GS] = Option::LogicTrick(std::to_string(RT_ICE_MQ_RED_ICE_GS));
    trickOptions[RT_ICE_MQ_SCARECROW] = Option::LogicTrick(std::to_string(RT_ICE_MQ_SCARECROW));
    trickOptions[RT_LENS_GTG] = Option::LogicTrick(std::to_string(RT_LENS_GTG));
    trickOptions[RT_GTG_WITHOUT_HOOKSHOT] = Option::LogicTrick(std::to_string(RT_GTG_WITHOUT_HOOKSHOT));
    trickOptions[RT_GTG_FAKE_WALL] = Option::LogicTrick(std::to_string(RT_GTG_FAKE_WALL));
    trickOptions[RT_LENS_GTG_MQ] = Option::LogicTrick(std::to_string(RT_LENS_GTG_MQ));
    trickOptions[RT_GTG_MQ_WITH_HOOKSHOT] = Option::LogicTrick(std::to_string(RT_GTG_MQ_WITH_HOOKSHOT));
    trickOptions[RT_GTG_MQ_WIHTOUT_HOOKSHOT] = Option::LogicTrick(std::to_string(RT_GTG_MQ_WIHTOUT_HOOKSHOT));
    trickOptions[RT_LENS_GANON] = Option::LogicTrick(std::to_string(RT_LENS_GANON));
    trickOptions[RT_GANON_SPIRIT_TRIAL_HOOKSHOT] = Option::LogicTrick(std::to_string(RT_GANON_SPIRIT_TRIAL_HOOKSHOT));
    trickOptions[RT_LENS_GANON_MQ] = Option::LogicTrick(std::to_string(RT_LENS_GANON_MQ));
    trickOptions[RT_GANON_MQ_FIRE_TRIAL] = Option::LogicTrick(std::to_string(RT_GANON_MQ_FIRE_TRIAL));
    trickOptions[RT_GANON_MQ_SHADOW_TRIAL] = Option::LogicTrick(std::to_string(RT_GANON_MQ_SHADOW_TRIAL));
    trickOptions[RT_GANON_MQ_LIGHT_TRIAL] = Option::LogicTrick(std::to_string(RT_GANON_MQ_LIGHT_TRIAL));
}

Option& Settings::Setting(RandomizerSettingKey key) {
    return mOptions[key];
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
}