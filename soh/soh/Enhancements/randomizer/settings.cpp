#include "settings.h"
#include "trial.h"
#include "dungeon.h"
#include "context.h"

#include "soh/OTRGlobals.h"

#include <spdlog/spdlog.h>

#include <utility>

#include "consolevariablebridge.h"

namespace Rando {
std::vector<std::string> NumOpts(const int min, const int max, const int step = 1, const std::string& textBefore = {}, const std::string& textAfter = {}) {
    std::vector<std::string> options;
    options.reserve((max - min) / step + 1);
    for (int i = min; i <= max; i += step) {
        options.push_back(textBefore + std::to_string(i) += textAfter);
    }
    return options;
}

std::vector<std::string> MultiVecOpts(const std::vector<std::vector<std::string>>& optionsVector) {
    uint32_t totalSize = 0;
    for (const auto& vector: optionsVector) {
        totalSize += vector.size();
    }
    std::vector<std::string> options;
    options.reserve(totalSize);
    for (const auto& vector : optionsVector) {
        for (const auto& op : vector) {
            options.push_back(op);
        }
    }
    return options;
}

void Settings::HandleShopsanityPriceUI(){
    bool isTycoon = CVarGetInteger(CVAR_RANDOMIZER_SETTING("IncludeTycoonWallet"), RO_GENERIC_OFF);
    mOptions[RSK_SHOPSANITY].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
    mOptions[RSK_SHOPSANITY_PRICES].Unhide();
    switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShopsanityPrices"), RO_PRICE_VANILLA)){
        case RO_PRICE_FIXED:
            mOptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE].Unhide();
            mOptions[RSK_SHOPSANITY_PRICES_RANGE_1].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_RANGE_2].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_NO_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_CHILD_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_ADULT_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_GIANT_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_TYCOON_WALLET_WEIGHT].Hide();
            if (isTycoon ? mOptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE].GetOptionCount() == 501 : mOptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE].GetOptionCount() == 1000) {
                mOptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE].ChangeOptions(isTycoon ? NumOpts(0, 999) : NumOpts(0, 500));
            }
            mOptions[RSK_SHOPSANITY_PRICES_AFFORDABLE].Hide();
            break;
        case RO_PRICE_RANGE:
            mOptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_RANGE_1].Unhide();
            mOptions[RSK_SHOPSANITY_PRICES_RANGE_2].Unhide();
            mOptions[RSK_SHOPSANITY_PRICES_NO_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_CHILD_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_ADULT_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_GIANT_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_TYCOON_WALLET_WEIGHT].Hide();
            if (isTycoon ? mOptions[RSK_SHOPSANITY_PRICES_RANGE_1].GetOptionCount() == 101 : mOptions[RSK_SHOPSANITY_PRICES_RANGE_1].GetOptionCount() == 200) {
                mOptions[RSK_SHOPSANITY_PRICES_RANGE_1].ChangeOptions(isTycoon ? NumOpts(0, 995, 5) : NumOpts(0, 500, 5));
                mOptions[RSK_SHOPSANITY_PRICES_RANGE_2].ChangeOptions(isTycoon ? NumOpts(0, 995, 5) : NumOpts(0, 500, 5));
            }
            mOptions[RSK_SHOPSANITY_PRICES_AFFORDABLE].Unhide();
            break;
        case RO_PRICE_SET_BY_WALLET:
            mOptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_RANGE_1].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_RANGE_2].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_NO_WALLET_WEIGHT].Unhide();
            mOptions[RSK_SHOPSANITY_PRICES_CHILD_WALLET_WEIGHT].Unhide();
            mOptions[RSK_SHOPSANITY_PRICES_ADULT_WALLET_WEIGHT].Unhide();
            mOptions[RSK_SHOPSANITY_PRICES_GIANT_WALLET_WEIGHT].Unhide();
            if (isTycoon){
                mOptions[RSK_SHOPSANITY_PRICES_TYCOON_WALLET_WEIGHT].Unhide();
            } else {
                mOptions[RSK_SHOPSANITY_PRICES_TYCOON_WALLET_WEIGHT].Hide();
            }
            mOptions[RSK_SHOPSANITY_PRICES_AFFORDABLE].Unhide();
            break;
        default:
            mOptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_RANGE_1].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_RANGE_2].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_NO_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_CHILD_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_ADULT_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_GIANT_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_TYCOON_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_AFFORDABLE].Unhide();
            break;
    }
}

Settings::Settings() : mExcludeLocationsOptionsAreas(RCAREA_INVALID) {
}

void Settings::CreateOptions() {
    CreateOptionDescriptions();
    // clang-format off
    mOptions[RSK_FOREST] = Option::U8("Forest", {"Closed", "Closed Deku", "Open"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("Forest"), mOptionDescriptions[RSK_FOREST], WidgetType::Combobox, RO_FOREST_CLOSED);
    mOptions[RSK_KAK_GATE] = Option::U8("Kakariko Gate", {"Closed", "Open"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("KakarikoGate"), mOptionDescriptions[RSK_KAK_GATE]);
    mOptions[RSK_DOOR_OF_TIME] = Option::U8("Door of Time", {"Closed", "Song only", "Open"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("DoorOfTime"), mOptionDescriptions[RSK_DOOR_OF_TIME], WidgetType::Combobox);
    mOptions[RSK_ZORAS_FOUNTAIN] = Option::U8("Zora's Fountain", {"Closed", "Closed as child", "Open"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ZorasFountain"), mOptionDescriptions[RSK_ZORAS_FOUNTAIN]);
    mOptions[RSK_GERUDO_FORTRESS] = Option::U8("Gerudo Fortress", {"Normal", "Fast", "Open"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GerudoFortress"), mOptionDescriptions[RSK_GERUDO_FORTRESS]);
    mOptions[RSK_RAINBOW_BRIDGE] = Option::U8("Rainbow Bridge", {"Vanilla", "Always open", "Stones", "Medallions", "Dungeon rewards", "Dungeons", "Tokens", "Greg"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("RainbowBridge"), mOptionDescriptions[RSK_RAINBOW_BRIDGE], WidgetType::Combobox, RO_BRIDGE_VANILLA, false, IMFLAG_NONE);
    mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT] = Option::U8("Stone Count", {NumOpts(0, 4)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StoneCount"), "", WidgetType::Slider, 3, true);
    mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT] = Option::U8("Medallion Count", {NumOpts(0, 7)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MedallionCount"), "", WidgetType::Slider, 6, true);
    mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT] = Option::U8("Reward Count", {NumOpts(0, 10)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("RewardCount"), "", WidgetType::Slider, 9, true);
    mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT] = Option::U8("Dungeon Count", {NumOpts(0, 9)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("DungeonCount"), "", WidgetType::Slider, 8, true);
    mOptions[RSK_RAINBOW_BRIDGE_TOKEN_COUNT] = Option::U8("Token Count", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("TokenCount"), "", WidgetType::Slider, 100, true);
    mOptions[RSK_BRIDGE_OPTIONS] = Option::U8("Bridge Reward Options", {"Standard Rewards", "Greg as Reward", "Greg as Wildcard"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("BridgeRewardOptions"), mOptionDescriptions[RSK_BRIDGE_OPTIONS], WidgetType::Combobox, RO_BRIDGE_STANDARD_REWARD, false, IMFLAG_NONE);
    mOptions[RSK_GANONS_TRIALS] = Option::U8("Ganon's Trials", {"Skip", "Set Number", "Random Number"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GanonTrial"), mOptionDescriptions[RSK_GANONS_TRIALS], WidgetType::Combobox, RO_GANONS_TRIALS_SET_NUMBER);
    mOptions[RSK_TRIAL_COUNT] = Option::U8("Ganon's Trials Count", {NumOpts(0, 6)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GanonTrialCount"), mOptionDescriptions[RSK_TRIAL_COUNT], WidgetType::Slider, 6, true);
    mOptions[RSK_STARTING_AGE] = Option::U8("Starting Age", {"Child", "Adult", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingAge"), mOptionDescriptions[RSK_STARTING_AGE], WidgetType::Combobox, RO_AGE_CHILD);
    mOptions[RSK_SHUFFLE_ENTRANCES] = Option::Bool("Shuffle Entrances");
    mOptions[RSK_SHUFFLE_DUNGEON_ENTRANCES] = Option::U8("Dungeon Entrances", {"Off", "On", "On + Ganon"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleDungeonsEntrances"), mOptionDescriptions[RSK_SHUFFLE_DUNGEON_ENTRANCES], WidgetType::Combobox, RO_DUNGEON_ENTRANCE_SHUFFLE_OFF);
    mOptions[RSK_SHUFFLE_BOSS_ENTRANCES] = Option::U8("Boss Entrances", {"Off", "Age Restricted", "Full"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleBossEntrances"), mOptionDescriptions[RSK_SHUFFLE_BOSS_ENTRANCES], WidgetType::Combobox, RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF);
    mOptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES] = Option::Bool("Overworld Entrances", CVAR_RANDOMIZER_SETTING("ShuffleOverworldEntrances"), mOptionDescriptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES]);
    mOptions[RSK_SHUFFLE_INTERIOR_ENTRANCES] = Option::U8("Interior Entrances", {"Off", "Simple", "All"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleInteriorsEntrances"), mOptionDescriptions[RSK_SHUFFLE_INTERIOR_ENTRANCES], WidgetType::Combobox, RO_INTERIOR_ENTRANCE_SHUFFLE_OFF);
    mOptions[RSK_SHUFFLE_GROTTO_ENTRANCES] = Option::Bool("Grottos Entrances", CVAR_RANDOMIZER_SETTING("ShuffleGrottosEntrances"), mOptionDescriptions[RSK_SHUFFLE_GROTTO_ENTRANCES]);
    mOptions[RSK_SHUFFLE_OWL_DROPS] = Option::Bool("Owl Drops", CVAR_RANDOMIZER_SETTING("ShuffleOwlDrops"), mOptionDescriptions[RSK_SHUFFLE_OWL_DROPS]);
    mOptions[RSK_SHUFFLE_WARP_SONGS] = Option::Bool("Warp Songs", CVAR_RANDOMIZER_SETTING("ShuffleWarpSongs"), mOptionDescriptions[RSK_SHUFFLE_WARP_SONGS]);
    mOptions[RSK_SHUFFLE_OVERWORLD_SPAWNS] = Option::Bool("Overworld Spawns", CVAR_RANDOMIZER_SETTING("ShuffleOverworldSpawns"), mOptionDescriptions[RSK_SHUFFLE_OVERWORLD_SPAWNS]);
    mOptions[RSK_MIXED_ENTRANCE_POOLS] = Option::Bool("Mixed Entrance Pools", CVAR_RANDOMIZER_SETTING("MixedEntrances"), mOptionDescriptions[RSK_MIXED_ENTRANCE_POOLS]);
    mOptions[RSK_MIX_DUNGEON_ENTRANCES] = Option::Bool("Mix Dungeons", CVAR_RANDOMIZER_SETTING("MixDungeons"), mOptionDescriptions[RSK_MIX_DUNGEON_ENTRANCES], IMFLAG_NONE);
    mOptions[RSK_MIX_BOSS_ENTRANCES] = Option::Bool("Mix Bosses", CVAR_RANDOMIZER_SETTING("MixBosses"), mOptionDescriptions[RSK_MIX_BOSS_ENTRANCES], IMFLAG_NONE);
    mOptions[RSK_MIX_OVERWORLD_ENTRANCES] = Option::Bool("Mix Overworld", CVAR_RANDOMIZER_SETTING("MixOverworld"), mOptionDescriptions[RSK_MIX_OVERWORLD_ENTRANCES], IMFLAG_NONE);
    mOptions[RSK_MIX_INTERIOR_ENTRANCES] = Option::Bool("Mix Interiors", CVAR_RANDOMIZER_SETTING("MixInteriors"), mOptionDescriptions[RSK_MIX_INTERIOR_ENTRANCES], IMFLAG_NONE);
    mOptions[RSK_MIX_GROTTO_ENTRANCES] = Option::Bool("Mix Grottos", CVAR_RANDOMIZER_SETTING("MixGrottos"), mOptionDescriptions[RSK_MIX_GROTTO_ENTRANCES]);
    mOptions[RSK_DECOUPLED_ENTRANCES] = Option::Bool("Decouple Entrances", CVAR_RANDOMIZER_SETTING("DecoupleEntrances"), mOptionDescriptions[RSK_DECOUPLED_ENTRANCES]);
    mOptions[RSK_BOMBCHUS_IN_LOGIC] = Option::Bool("Bombchus in Logic", CVAR_RANDOMIZER_SETTING("BombchusInLogic"), mOptionDescriptions[RSK_BOMBCHUS_IN_LOGIC]);
    mOptions[RSK_ENABLE_BOMBCHU_DROPS] = Option::U8("Bombchu Drops", {"No", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("EnableBombchuDrops"), mOptionDescriptions[RSK_ENABLE_BOMBCHU_DROPS], WidgetType::Combobox, RO_AMMO_DROPS_ON);
    // TODO: AmmoDrops and/or HeartDropRefill, combine with/separate Ammo Drops from Bombchu Drops?
    mOptions[RSK_TRIFORCE_HUNT] = Option::Bool("Triforce Hunt", CVAR_RANDOMIZER_SETTING("TriforceHunt"), mOptionDescriptions[RSK_TRIFORCE_HUNT], IMFLAG_NONE);
    mOptions[RSK_TRIFORCE_HUNT_PIECES_TOTAL] = Option::U8("Triforce Hunt Total Pieces", {NumOpts(1, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("TriforceHuntTotalPieces"), mOptionDescriptions[RSK_TRIFORCE_HUNT_PIECES_TOTAL], WidgetType::Slider, 29, false, IMFLAG_NONE);
    mOptions[RSK_TRIFORCE_HUNT_PIECES_REQUIRED] = Option::U8("Triforce Hunt Required Pieces", {NumOpts(1, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("TriforceHuntRequiredPieces"), mOptionDescriptions[RSK_TRIFORCE_HUNT_PIECES_REQUIRED], WidgetType::Slider, 19);
    mOptions[RSK_MQ_DUNGEON_RANDOM] = Option::U8("MQ Dungeon Setting", {"None", "Set Number", "Random", "Selection Only"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeons"), mOptionDescriptions[RSK_MQ_DUNGEON_RANDOM], WidgetType::Combobox, RO_MQ_DUNGEONS_NONE, true, IMFLAG_NONE);
    mOptions[RSK_MQ_DUNGEON_COUNT] = Option::U8("MQ Dungeon Count", {NumOpts(0, 12)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonCount"), "", WidgetType::Slider, 12, true, IMFLAG_NONE);
    mOptions[RSK_MQ_DUNGEON_SET] = Option::Bool("Set Dungeon Quests", {"Off", "On"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsSelection"), mOptionDescriptions[RSK_MQ_DUNGEON_SET], WidgetType::Checkbox, false, false, IMFLAG_NONE);
    mOptions[RSK_MQ_DEKU_TREE] = Option::U8("Deku Tree", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsDekuTree"), "", WidgetType::Combobox, RO_MQ_SET_VANILLA, false, IMFLAG_NONE);
    mOptions[RSK_MQ_DODONGOS_CAVERN] = Option::U8("Dodongo's Cavern", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsDodongosCavern"), "", WidgetType::Combobox, RO_MQ_SET_VANILLA, false, IMFLAG_NONE);
    mOptions[RSK_MQ_JABU_JABU] = Option::U8("Jabu-Jabu's Belly", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsJabuJabu"), "", WidgetType::Combobox, RO_MQ_SET_VANILLA, false, IMFLAG_NONE);
    mOptions[RSK_MQ_FOREST_TEMPLE] = Option::U8("Forest Temple", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsForestTemple"), "", WidgetType::Combobox, RO_MQ_SET_VANILLA, false, IMFLAG_NONE);
    mOptions[RSK_MQ_FIRE_TEMPLE] = Option::U8("Fire Temple", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsFireTemple"), "", WidgetType::Combobox, RO_MQ_SET_VANILLA, false, IMFLAG_NONE);
    mOptions[RSK_MQ_WATER_TEMPLE] = Option::U8("Water Temple", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsWaterTemple"), "", WidgetType::Combobox, RO_MQ_SET_VANILLA, false, IMFLAG_NONE);
    mOptions[RSK_MQ_SPIRIT_TEMPLE] = Option::U8("Spirit Temple", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsSpiritTemple"), "", WidgetType::Combobox, RO_MQ_SET_VANILLA, false, IMFLAG_NONE);
    mOptions[RSK_MQ_SHADOW_TEMPLE] = Option::U8("Shadow Temple", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsShadowTemple"), "", WidgetType::Combobox, RO_MQ_SET_VANILLA, false, IMFLAG_NONE);
    mOptions[RSK_MQ_BOTTOM_OF_THE_WELL] = Option::U8("Bottom of the Well", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsBottomOfTheWell"), "", WidgetType::Combobox, RO_MQ_SET_VANILLA, false, IMFLAG_NONE);
    mOptions[RSK_MQ_ICE_CAVERN] = Option::U8("Ice Cavern", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsIceCavern"), "", WidgetType::Combobox, RO_MQ_SET_VANILLA, false, IMFLAG_NONE);
    mOptions[RSK_MQ_GTG] = Option::U8("Gerudo Training Grounds", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsGTG"), "", WidgetType::Combobox, RO_MQ_SET_VANILLA, false, IMFLAG_NONE);
    mOptions[RSK_MQ_GANONS_CASTLE] = Option::U8("Ganon's Castle", {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsGanonsCastle"), "", WidgetType::Combobox, RO_MQ_SET_VANILLA);
    mOptions[RSK_SHUFFLE_DUNGEON_REWARDS] = Option::U8("Shuffle Dungeon Rewards", {"End of Dungeons", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleDungeonReward"), mOptionDescriptions[RSK_SHUFFLE_DUNGEON_REWARDS], WidgetType::Combobox, RO_DUNGEON_REWARDS_END_OF_DUNGEON);
    mOptions[RSK_LINKS_POCKET] = Option::U8("Link's Pocket", {"Dungeon Reward", "Advancement", "Anything", "Nothing"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("LinksPocket"), "", WidgetType::Combobox, RO_LINKS_POCKET_DUNGEON_REWARD);
    mOptions[RSK_SHUFFLE_SONGS] = Option::U8("Shuffle Songs", {"Song Locations", "Dungeon Rewards", "Anywhere"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleSongs"), mOptionDescriptions[RSK_SHUFFLE_SONGS], WidgetType::Combobox, RO_SONG_SHUFFLE_SONG_LOCATIONS);
    mOptions[RSK_SHOPSANITY] = Option::U8("Shopsanity", {"Off", "Specific Count", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("Shopsanity"), mOptionDescriptions[RSK_SHOPSANITY], WidgetType::Combobox, RO_SHOPSANITY_OFF);
    mOptions[RSK_SHOPSANITY_COUNT] = Option::U8("Shopsanity Item Count", {NumOpts(0, 7/*8*/)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityCount"), mOptionDescriptions[RSK_SHOPSANITY_COUNT], WidgetType::Slider, 0, false, IMFLAG_NONE);
    mOptions[RSK_SHOPSANITY_PRICES] = Option::U8("Shopsanity Prices", {"Vanilla", "Cheap Balanced", "Balanced", "Fixed", "Range", "Set By Wallet"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityPrices"), mOptionDescriptions[RSK_SHOPSANITY_PRICES], WidgetType::Combobox, RO_PRICE_VANILLA, false, IMFLAG_NONE);
    mOptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE] = Option::U8("Fixed Price", {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityFixedPrice"), mOptionDescriptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE], WidgetType::Slider, 10, true);
    mOptions[RSK_SHOPSANITY_PRICES_RANGE_1] = Option::U8("Lower Bound", {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityPriceRange1"), mOptionDescriptions[RSK_SHOPSANITY_PRICES_RANGE_1], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_SHOPSANITY_PRICES_RANGE_2] = Option::U8("Upper Bound", {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityPriceRange2"), mOptionDescriptions[RSK_SHOPSANITY_PRICES_RANGE_2], WidgetType::Slider, 100, true, IMFLAG_NONE);
    mOptions[RSK_SHOPSANITY_PRICES_NO_WALLET_WEIGHT] = Option::U8("No Wallet Weight", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityNoWalletWeight"), mOptionDescriptions[RSK_SHOPSANITY_PRICES_NO_WALLET_WEIGHT], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_SHOPSANITY_PRICES_CHILD_WALLET_WEIGHT] = Option::U8("Child Wallet Weight", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityChildWalletWeight"), mOptionDescriptions[RSK_SHOPSANITY_PRICES_CHILD_WALLET_WEIGHT], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_SHOPSANITY_PRICES_ADULT_WALLET_WEIGHT] = Option::U8("Adult Wallet Weight", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityAdultWalletWeight"), mOptionDescriptions[RSK_SHOPSANITY_PRICES_ADULT_WALLET_WEIGHT], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_SHOPSANITY_PRICES_GIANT_WALLET_WEIGHT] = Option::U8("Giant Wallet Weight", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityGiantWalletWeight"), mOptionDescriptions[RSK_SHOPSANITY_PRICES_GIANT_WALLET_WEIGHT], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_SHOPSANITY_PRICES_TYCOON_WALLET_WEIGHT] = Option::U8("Tycoon Wallet Weight", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityTycoonWalletWeight"), mOptionDescriptions[RSK_SHOPSANITY_PRICES_TYCOON_WALLET_WEIGHT], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_SHOPSANITY_PRICES_AFFORDABLE] = Option::Bool("Shops Affordable Prices", CVAR_RANDOMIZER_SETTING("ShopsanityPricesAffordable"), mOptionDescriptions[RSK_SHOPSANITY_PRICES_AFFORDABLE]);
    mOptions[RSK_SHUFFLE_TOKENS] = Option::U8("Tokensanity", {"Off", "Dungeons", "Overworld", "All Tokens"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleTokens"), mOptionDescriptions[RSK_SHUFFLE_TOKENS], WidgetType::Combobox, RO_TOKENSANITY_OFF);
    mOptions[RSK_SHUFFLE_SCRUBS] = Option::U8("Scrub Shuffle", {"Off", "One-Time Only", "All"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleScrubs"), mOptionDescriptions[RSK_SHUFFLE_SCRUBS], WidgetType::Combobox, RO_SCRUBS_OFF);
    mOptions[RSK_SCRUBS_PRICES] = Option::U8("Scrub Prices", {"Vanilla", "Cheap Balanced", "Balanced", "Fixed", "Range", "Set By Wallet"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsPrices"), mOptionDescriptions[RSK_SCRUBS_PRICES], WidgetType::Combobox, RO_PRICE_VANILLA, false, IMFLAG_NONE);
    mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE] = Option::U8("Fixed Price", {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsFixedPrice"), mOptionDescriptions[RSK_SCRUBS_PRICES_FIXED_PRICE], WidgetType::Slider, 10, true);
    mOptions[RSK_SCRUBS_PRICES_RANGE_1] = Option::U8("Lower Bound", {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsPriceRange1"), mOptionDescriptions[RSK_SCRUBS_PRICES_RANGE_1], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_SCRUBS_PRICES_RANGE_2] = Option::U8("Upper Bound", {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsPriceRange2"), mOptionDescriptions[RSK_SCRUBS_PRICES_RANGE_2], WidgetType::Slider, 100, true, IMFLAG_NONE);
    mOptions[RSK_SCRUBS_PRICES_NO_WALLET_WEIGHT] = Option::U8("No Wallet Weight", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsNoWalletWeight"), mOptionDescriptions[RSK_SCRUBS_PRICES_NO_WALLET_WEIGHT], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_SCRUBS_PRICES_CHILD_WALLET_WEIGHT] = Option::U8("Child Wallet Weight", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsChildWalletWeight"), mOptionDescriptions[RSK_SCRUBS_PRICES_CHILD_WALLET_WEIGHT], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_SCRUBS_PRICES_ADULT_WALLET_WEIGHT] = Option::U8("Adult Wallet Weight", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsAdultWalletWeight"), mOptionDescriptions[RSK_SCRUBS_PRICES_ADULT_WALLET_WEIGHT], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_SCRUBS_PRICES_GIANT_WALLET_WEIGHT] = Option::U8("Giant Wallet Weight", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsGiantWalletWeight"), mOptionDescriptions[RSK_SCRUBS_PRICES_GIANT_WALLET_WEIGHT], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT] = Option::U8("Tycoon Wallet Weight", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsTycoonWalletWeight"), mOptionDescriptions[RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_SCRUBS_PRICES_AFFORDABLE] = Option::Bool("Scrubs Affordable Prices", CVAR_RANDOMIZER_SETTING("ScrubsPricesAffordable"), mOptionDescriptions[RSK_SCRUBS_PRICES_AFFORDABLE]);
    mOptions[RSK_SHUFFLE_BEEHIVES] = Option::Bool("Shuffle Beehives", CVAR_RANDOMIZER_SETTING("ShuffleBeehives"), mOptionDescriptions[RSK_SHUFFLE_BEEHIVES]);
    mOptions[RSK_SHUFFLE_COWS] = Option::Bool("Shuffle Cows", CVAR_RANDOMIZER_SETTING("ShuffleCows"), mOptionDescriptions[RSK_SHUFFLE_COWS]);
    mOptions[RSK_SHUFFLE_KOKIRI_SWORD] = Option::Bool("Shuffle Kokiri Sword", CVAR_RANDOMIZER_SETTING("ShuffleKokiriSword"), mOptionDescriptions[RSK_SHUFFLE_KOKIRI_SWORD]);
    mOptions[RSK_SHUFFLE_MASTER_SWORD] = Option::Bool("Shuffle Master Sword", CVAR_RANDOMIZER_SETTING("ShuffleMasterSword"), mOptionDescriptions[RSK_SHUFFLE_MASTER_SWORD]);
    mOptions[RSK_SHUFFLE_CHILD_WALLET] = Option::Bool("Shuffle Child's Wallet", CVAR_RANDOMIZER_SETTING("ShuffleChildWallet"), mOptionDescriptions[RSK_SHUFFLE_CHILD_WALLET], IMFLAG_NONE);
    mOptions[RSK_INCLUDE_TYCOON_WALLET] = Option::Bool("Include Tycoon Wallet", CVAR_RANDOMIZER_SETTING("IncludeTycoonWallet"), mOptionDescriptions[RSK_INCLUDE_TYCOON_WALLET]);
    mOptions[RSK_SHUFFLE_OCARINA] = Option::Bool("Shuffle Ocarinas", CVAR_RANDOMIZER_SETTING("ShuffleOcarinas"), mOptionDescriptions[RSK_SHUFFLE_OCARINA]);
    mOptions[RSK_SHUFFLE_OCARINA_BUTTONS] = Option::Bool("Shuffle Ocarina Buttons", CVAR_RANDOMIZER_SETTING("ShuffleOcarinaButtons"), mOptionDescriptions[RSK_SHUFFLE_OCARINA_BUTTONS]);
    mOptions[RSK_SHUFFLE_SWIM] = Option::Bool("Shuffle Swim", CVAR_RANDOMIZER_SETTING("ShuffleSwim"), mOptionDescriptions[RSK_SHUFFLE_SWIM]);
    mOptions[RSK_SHUFFLE_WEIRD_EGG] = Option::Bool("Shuffle Weird Egg", CVAR_RANDOMIZER_SETTING("ShuffleWeirdEgg"), mOptionDescriptions[RSK_SHUFFLE_WEIRD_EGG]);
    mOptions[RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD] = Option::Bool("Shuffle Gerudo Membership Card", CVAR_RANDOMIZER_SETTING("ShuffleGerudoToken"), mOptionDescriptions[RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD]);
    mOptions[RSK_SHUFFLE_FISHING_POLE] = Option::Bool("Shuffle Fishing Pole", CVAR_RANDOMIZER_SETTING("ShuffleFishingPole"), mOptionDescriptions[RSK_SHUFFLE_FISHING_POLE]);
    mOptions[RSK_SHUFFLE_MERCHANTS] = Option::U8("Shuffle Merchants", {"Off", "Bean Merchant Only", "All But Beans", "All"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleMerchants"), mOptionDescriptions[RSK_SHUFFLE_MERCHANTS], WidgetType::Combobox, RO_SHUFFLE_MERCHANTS_OFF, IMFLAG_NONE);
    mOptions[RSK_MERCHANT_PRICES] = Option::U8("Merchant Prices", {"Vanilla", "Cheap Balanced", "Balanced", "Fixed", "Range", "Set By Wallet"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantPrices"), mOptionDescriptions[RSK_MERCHANT_PRICES], WidgetType::Combobox, RO_PRICE_VANILLA, false, IMFLAG_NONE);
    mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE] = Option::U8("Fixed Price", {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantFixedPrice"), mOptionDescriptions[RSK_MERCHANT_PRICES_FIXED_PRICE], WidgetType::Slider, 10, true);
    mOptions[RSK_MERCHANT_PRICES_RANGE_1] = Option::U8("Lower Bound", {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantPriceRange1"), mOptionDescriptions[RSK_MERCHANT_PRICES_RANGE_1], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_MERCHANT_PRICES_RANGE_2] = Option::U8("Upper Bound", {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantPriceRange2"), mOptionDescriptions[RSK_MERCHANT_PRICES_RANGE_2], WidgetType::Slider, 100, true, IMFLAG_NONE);
    mOptions[RSK_MERCHANT_PRICES_NO_WALLET_WEIGHT] = Option::U8("No Wallet Weight", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantNoWalletWeight"), mOptionDescriptions[RSK_MERCHANT_PRICES_NO_WALLET_WEIGHT], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_MERCHANT_PRICES_CHILD_WALLET_WEIGHT] = Option::U8("Child Wallet Weight", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantChildWalletWeight"), mOptionDescriptions[RSK_MERCHANT_PRICES_CHILD_WALLET_WEIGHT], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_MERCHANT_PRICES_ADULT_WALLET_WEIGHT] = Option::U8("Adult Wallet Weight", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantAdultWalletWeight"), mOptionDescriptions[RSK_MERCHANT_PRICES_ADULT_WALLET_WEIGHT], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_MERCHANT_PRICES_GIANT_WALLET_WEIGHT] = Option::U8("Giant Wallet Weight", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantGiantWalletWeight"), mOptionDescriptions[RSK_MERCHANT_PRICES_GIANT_WALLET_WEIGHT], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT] = Option::U8("Tycoon Wallet Weight", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantTycoonWalletWeight"), mOptionDescriptions[RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT], WidgetType::Slider, 10, true, IMFLAG_NONE);
    mOptions[RSK_MERCHANT_PRICES_AFFORDABLE] = Option::Bool("Merchant Affordable Prices", CVAR_RANDOMIZER_SETTING("MerchantPricesAffordable"), mOptionDescriptions[RSK_MERCHANT_PRICES_AFFORDABLE]);
    mOptions[RSK_SHUFFLE_FROG_SONG_RUPEES] = Option::Bool("Shuffle Frog Song Rupees", CVAR_RANDOMIZER_SETTING("ShuffleFrogSongRupees"), mOptionDescriptions[RSK_SHUFFLE_FROG_SONG_RUPEES]);
    mOptions[RSK_SHUFFLE_ADULT_TRADE] = Option::Bool("Shuffle Adult Trade", CVAR_RANDOMIZER_SETTING("ShuffleAdultTrade"), mOptionDescriptions[RSK_SHUFFLE_ADULT_TRADE]);
    mOptions[RSK_SHUFFLE_CHEST_MINIGAME] = Option::U8("Shuffle Chest Minigame", {"Off", "On (Separate)", "On (Pack)"});
    mOptions[RSK_SHUFFLE_100_GS_REWARD] = Option::Bool("Shuffle 100 GS Reward", CVAR_RANDOMIZER_SETTING("Shuffle100GSReward"), mOptionDescriptions[RSK_SHUFFLE_100_GS_REWARD], IMFLAG_SEPARATOR_BOTTOM, WidgetType::Checkbox, RO_GENERIC_OFF);
    mOptions[RSK_SHUFFLE_BOSS_SOULS] = Option::U8("Shuffle Boss Souls", {"Off", "On", "On + Ganon"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleBossSouls"), mOptionDescriptions[RSK_SHUFFLE_BOSS_SOULS], WidgetType::Combobox);
    mOptions[RSK_SHUFFLE_DEKU_STICK_BAG] = Option::Bool("Shuffle Deku Stick Bag", CVAR_RANDOMIZER_SETTING("ShuffleDekuStickBag"), mOptionDescriptions[RSK_SHUFFLE_DEKU_STICK_BAG], IMFLAG_SEPARATOR_BOTTOM, WidgetType::Checkbox, RO_GENERIC_OFF);
    mOptions[RSK_SHUFFLE_DEKU_NUT_BAG] = Option::Bool("Shuffle Deku Nut Bag", CVAR_RANDOMIZER_SETTING("ShuffleDekuNutBag"), mOptionDescriptions[RSK_SHUFFLE_DEKU_NUT_BAG], IMFLAG_SEPARATOR_BOTTOM, WidgetType::Checkbox, RO_GENERIC_OFF);
    mOptions[RSK_FISHSANITY] = Option::U8("Fishsanity", {"Off", "Shuffle only Hyrule Loach", "Shuffle Fishing Pond", "Shuffle Overworld Fish", "Shuffle Both"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("Fishsanity"), mOptionDescriptions[RSK_FISHSANITY], WidgetType::Combobox, RO_FISHSANITY_OFF);
    mOptions[RSK_FISHSANITY_POND_COUNT] = Option::U8("Pond Fish Count", {NumOpts(0,17,1)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("FishsanityPondCount"), mOptionDescriptions[RSK_FISHSANITY_POND_COUNT], WidgetType::Slider, 0, true, IMFLAG_NONE);
    mOptions[RSK_FISHSANITY_AGE_SPLIT] = Option::Bool("Pond Age Split", CVAR_RANDOMIZER_SETTING("FishsanityAgeSplit"), mOptionDescriptions[RSK_FISHSANITY_AGE_SPLIT]);
    mOptions[RSK_SHUFFLE_MAPANDCOMPASS] = Option::U8("Maps/Compasses", {"Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingMapsCompasses"), mOptionDescriptions[RSK_SHUFFLE_MAPANDCOMPASS], WidgetType::Combobox, RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
    mOptions[RSK_KEYSANITY] = Option::U8("Small Keys", {"Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("Keysanity"), mOptionDescriptions[RSK_KEYSANITY], WidgetType::Combobox, RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
    mOptions[RSK_GERUDO_KEYS] = Option::U8("Gerudo Fortress Keys", {"Vanilla", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GerudoKeys"), mOptionDescriptions[RSK_GERUDO_KEYS], WidgetType::Combobox, RO_GERUDO_KEYS_VANILLA);
    mOptions[RSK_BOSS_KEYSANITY] = Option::U8("Boss Keys", {"Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("BossKeysanity"), mOptionDescriptions[RSK_BOSS_KEYSANITY], WidgetType::Combobox, RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
    mOptions[RSK_GANONS_BOSS_KEY] = Option::U8("Ganon's Boss Key", {"Vanilla", "Own Dungeon", "Start With", "Any Dungeon", "Overworld", "Anywhere", "LACS-Vanilla", "LACS-Stones", "LACS-Medallions", "LACS-Rewards", "LACS-Dungeons", "LACS-Tokens", "100 GS Reward", "Triforce Hunt"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleGanonBossKey"), mOptionDescriptions[RSK_GANONS_BOSS_KEY], WidgetType::Combobox, RO_GANON_BOSS_KEY_VANILLA);
    mOptions[RSK_LACS_STONE_COUNT] = Option::U8("Stone Count", {NumOpts(0, 4)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("LacsStoneCount"), "", WidgetType::Slider, 3, true);
    mOptions[RSK_LACS_MEDALLION_COUNT] = Option::U8("Medallion Count", {NumOpts(0, 7)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("LacsMedallionCount"), "", WidgetType::Slider, 6, true);
    mOptions[RSK_LACS_REWARD_COUNT] = Option::U8("Reward Count", {NumOpts(0, 10)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("LacsRewardCount"), "", WidgetType::Slider, 9, true);
    mOptions[RSK_LACS_DUNGEON_COUNT] = Option::U8("Dungeon Count", {NumOpts(0, 9)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("LacsDungeonCount"), "", WidgetType::Slider, 8, true);
    mOptions[RSK_LACS_TOKEN_COUNT] = Option::U8("Token Count", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("LacsTokenCount"), "", WidgetType::Slider, 100, true);
    mOptions[RSK_LACS_OPTIONS] = Option::U8("LACS Reward Options", {"Standard Reward", "Greg as Reward", "Greg as Wildcard"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("LacsRewardOptions"), "", WidgetType::Combobox, RO_LACS_STANDARD_REWARD);
    mOptions[RSK_KEYRINGS] = Option::U8("Key Rings", {"Off", "Random", "Count", "Selection"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRings"), mOptionDescriptions[RSK_KEYRINGS], WidgetType::Combobox, RO_KEYRINGS_OFF);
    mOptions[RSK_KEYRINGS_RANDOM_COUNT] = Option::U8("Keyring Dungeon Count", {NumOpts(0, 9)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsRandomCount"), "", WidgetType::Slider, 8);
    mOptions[RSK_KEYRINGS_GERUDO_FORTRESS] = Option::U8("Gerudo Fortress Keyring", {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsGerudoFortress"), "", WidgetType::TristateCheckbox, 0);
    mOptions[RSK_KEYRINGS_FOREST_TEMPLE] = Option::U8("Forest Temple Keyring", {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsForestTemple"), "", WidgetType::TristateCheckbox, 0);
    mOptions[RSK_KEYRINGS_FIRE_TEMPLE] = Option::U8("Fire Temple Keyring", {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsFireTemple"), "", WidgetType::TristateCheckbox, 0);
    mOptions[RSK_KEYRINGS_WATER_TEMPLE] = Option::U8("Water Temple Keyring", {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsWaterTemple"), "", WidgetType::TristateCheckbox, 0);
    mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE] = Option::U8("Spirit Temple Keyring", {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsSpiritTemple"), "", WidgetType::TristateCheckbox, 0);
    mOptions[RSK_KEYRINGS_SHADOW_TEMPLE] = Option::U8("Shadow Temple Keyring", {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsShadowTemple"), "", WidgetType::TristateCheckbox, 0);
    mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL] = Option::U8("Bottom of the Well Keyring", {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsBottomOfTheWell"), "", WidgetType::TristateCheckbox, 0);
    mOptions[RSK_KEYRINGS_GTG] = Option::U8("Gerudo Training Grounds Keyring", {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsGTG"), "", WidgetType::TristateCheckbox, 0);
    mOptions[RSK_KEYRINGS_GANONS_CASTLE] = Option::U8("Ganon's Castle Keyring", {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsGanonsCastle"), "", WidgetType::TristateCheckbox, 0);
    mOptions[RSK_SKIP_CHILD_STEALTH] = Option::Bool("Skip Child Stealth", {"Don't Skip", "Skip"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("SkipChildStealth"), mOptionDescriptions[RSK_SKIP_CHILD_STEALTH], WidgetType::Checkbox, RO_GENERIC_DONT_SKIP);
    mOptions[RSK_SKIP_CHILD_ZELDA] = Option::Bool("Skip Child Zelda", {"Don't Skip", "Skip"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("SkipChildZelda"), mOptionDescriptions[RSK_SKIP_CHILD_ZELDA], WidgetType::Checkbox, RO_GENERIC_DONT_SKIP);
    mOptions[RSK_SKIP_EPONA_RACE] = Option::Bool("Skip Epona Race", {"Don't Skip", "Skip"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("SkipEponaRace"), mOptionDescriptions[RSK_SKIP_EPONA_RACE], WidgetType::Checkbox, RO_GENERIC_DONT_SKIP);
    mOptions[RSK_SKIP_SCARECROWS_SONG] = Option::Bool("Skip Scarecrow's Song", CVAR_RANDOMIZER_SETTING("SkipScarecrowsSong"), mOptionDescriptions[RSK_SKIP_SCARECROWS_SONG]);
    mOptions[RSK_BIG_POE_COUNT] = Option::U8("Big Poe Target Count", {NumOpts(1, 10)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("BigPoeTargetCount"), mOptionDescriptions[RSK_BIG_POE_COUNT], WidgetType::Slider, 9);
    mOptions[RSK_CUCCO_COUNT] = Option::U8("Cuccos to return", {NumOpts(0, 7)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("CuccosToReturn"), mOptionDescriptions[RSK_CUCCO_COUNT], WidgetType::Slider, 7);
    mOptions[RSK_COMPLETE_MASK_QUEST] = Option::Bool("Complete Mask Quest", CVAR_RANDOMIZER_SETTING("CompleteMaskQuest"), mOptionDescriptions[RSK_COMPLETE_MASK_QUEST]);
    mOptions[RSK_ENABLE_GLITCH_CUTSCENES] = Option::Bool("Enable Glitch-Useful Cutscenes", CVAR_RANDOMIZER_SETTING("EnableGlitchCutscenes"), mOptionDescriptions[RSK_ENABLE_GLITCH_CUTSCENES]);
    mOptions[RSK_GOSSIP_STONE_HINTS] = Option::U8("Gossip Stone Hints", {"No Hints", "Need Nothing", "Mask of Truth", "Stone of Agony"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GossipStoneHints"), mOptionDescriptions[RSK_GOSSIP_STONE_HINTS], WidgetType::Combobox, RO_GOSSIP_STONES_NEED_NOTHING, false, IMFLAG_NONE);
    mOptions[RSK_HINT_CLARITY] = Option::U8("Hint Clarity", {"Obscure", "Ambiguous", "Clear"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("HintClarity"), mOptionDescriptions[RSK_HINT_CLARITY], WidgetType::Combobox, RO_HINT_CLARITY_CLEAR, true, IMFLAG_INDENT);
    mOptions[RSK_HINT_DISTRIBUTION] = Option::U8("Hint Distribution", {"Useless", "Balanced", "Strong", "Very Strong"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("HintDistribution"), mOptionDescriptions[RSK_HINT_DISTRIBUTION], WidgetType::Combobox, RO_HINT_DIST_BALANCED, true, IMFLAG_UNINDENT);
    mOptions[RSK_TOT_ALTAR_HINT] = Option::Bool("ToT Altar Hint", {"Off", "On"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("AltarHint"), mOptionDescriptions[RSK_TOT_ALTAR_HINT], WidgetType::Checkbox, RO_GENERIC_ON, false, IMFLAG_INDENT);
    mOptions[RSK_GANONDORF_HINT] = Option::Bool("Ganondorf Hint", {"Off", "On"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GanondorfHint"), mOptionDescriptions[RSK_GANONDORF_HINT], WidgetType::Checkbox, RO_GENERIC_ON, false, IMFLAG_NONE);
    mOptions[RSK_SHEIK_LA_HINT] = Option::Bool("Sheik Light Arrow Hint", {"Off", "On"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("SheikLAHint"), mOptionDescriptions[RSK_SHEIK_LA_HINT], WidgetType::Checkbox, RO_GENERIC_ON, false, IMFLAG_NONE);
    mOptions[RSK_DAMPES_DIARY_HINT] = Option::Bool("Dampe's Diary Hint", CVAR_RANDOMIZER_SETTING("DampeHint"), mOptionDescriptions[RSK_DAMPES_DIARY_HINT], IMFLAG_NONE);
    mOptions[RSK_GREG_HINT] = Option::Bool("Greg the Green Rupee Hint", CVAR_RANDOMIZER_SETTING("GregHint"), mOptionDescriptions[RSK_GREG_HINT], IMFLAG_NONE);
    mOptions[RSK_LOACH_HINT] = Option::Bool("Hyrule Loach Hint", CVAR_RANDOMIZER_SETTING("LoachHint"), mOptionDescriptions[RSK_LOACH_HINT], IMFLAG_NONE);
    mOptions[RSK_SARIA_HINT] = Option::Bool("Saria's Hint", CVAR_RANDOMIZER_SETTING("SariaHint"), mOptionDescriptions[RSK_SARIA_HINT], IMFLAG_NONE);
    mOptions[RSK_FISHING_POLE_HINT] = Option::Bool("Fishing Pole Hint", CVAR_RANDOMIZER_SETTING("FishingPoleHint"), mOptionDescriptions[RSK_FISHING_POLE_HINT], IMFLAG_NONE);
    mOptions[RSK_FROGS_HINT] = Option::Bool("Frog Ocarina Game Hint", CVAR_RANDOMIZER_SETTING("FrogsHint"), mOptionDescriptions[RSK_FROGS_HINT], IMFLAG_NONE);
    mOptions[RSK_OOT_HINT] = Option::Bool("Ocarina of Time Hint", CVAR_RANDOMIZER_SETTING("OoTHint"), mOptionDescriptions[RSK_OOT_HINT], IMFLAG_NONE);
    mOptions[RSK_BIGGORON_HINT] = Option::Bool("Biggoron's Hint", CVAR_RANDOMIZER_SETTING("BiggoronHint"), mOptionDescriptions[RSK_BIGGORON_HINT], IMFLAG_NONE);
    mOptions[RSK_BIG_POES_HINT] = Option::Bool("Big Poes Hint", CVAR_RANDOMIZER_SETTING("BigPoesHint"), mOptionDescriptions[RSK_BIG_POES_HINT], IMFLAG_NONE);
    mOptions[RSK_CHICKENS_HINT] = Option::Bool("Chickens Hint", CVAR_RANDOMIZER_SETTING("ChickensHint"), mOptionDescriptions[RSK_CHICKENS_HINT], IMFLAG_NONE);
    mOptions[RSK_MALON_HINT] = Option::Bool("Malon Hint", CVAR_RANDOMIZER_SETTING("MalonHint"), mOptionDescriptions[RSK_MALON_HINT], IMFLAG_NONE);
    mOptions[RSK_HBA_HINT] = Option::Bool("Horseback Archery Hint", CVAR_RANDOMIZER_SETTING("HBAHint"), mOptionDescriptions[RSK_HBA_HINT], IMFLAG_NONE);
    mOptions[RSK_WARP_SONG_HINTS] = Option::Bool("Warp Song Hints", CVAR_RANDOMIZER_SETTING("WarpSongText"), mOptionDescriptions[RSK_WARP_SONG_HINTS], IMFLAG_NONE, WidgetType::Checkbox, RO_GENERIC_ON);
    mOptions[RSK_SCRUB_TEXT_HINT] = Option::Bool("Scrub Hint Text", CVAR_RANDOMIZER_SETTING("ScrubText"), mOptionDescriptions[RSK_SCRUB_TEXT_HINT], IMFLAG_NONE);
    mOptions[RSK_MERCHANT_TEXT_HINT] = Option::Bool("Merchant Hint Text", CVAR_RANDOMIZER_SETTING("MerchantText"), mOptionDescriptions[RSK_MERCHANT_TEXT_HINT], IMFLAG_NONE);
    mOptions[RSK_KAK_10_SKULLS_HINT] = Option::Bool("10 GS Hint", CVAR_RANDOMIZER_SETTING("10GSHint"), mOptionDescriptions[RSK_KAK_10_SKULLS_HINT], IMFLAG_NONE);
    mOptions[RSK_KAK_20_SKULLS_HINT] = Option::Bool("20 GS Hint", CVAR_RANDOMIZER_SETTING("20GSHint"), mOptionDescriptions[RSK_KAK_20_SKULLS_HINT], IMFLAG_NONE);
    mOptions[RSK_KAK_30_SKULLS_HINT] = Option::Bool("30 GS Hint", CVAR_RANDOMIZER_SETTING("30GSHint"), mOptionDescriptions[RSK_KAK_30_SKULLS_HINT], IMFLAG_NONE);
    mOptions[RSK_KAK_40_SKULLS_HINT] = Option::Bool("40 GS Hint", CVAR_RANDOMIZER_SETTING("40GSHint"), mOptionDescriptions[RSK_KAK_40_SKULLS_HINT], IMFLAG_NONE);
    mOptions[RSK_KAK_50_SKULLS_HINT] = Option::Bool("50 GS Hint", CVAR_RANDOMIZER_SETTING("50GSHint"), mOptionDescriptions[RSK_KAK_50_SKULLS_HINT], IMFLAG_NONE);
    mOptions[RSK_KAK_100_SKULLS_HINT] = Option::Bool("100 GS Hint", CVAR_RANDOMIZER_SETTING("100GSHint"), mOptionDescriptions[RSK_KAK_100_SKULLS_HINT], IMFLAG_NONE);
    mOptions[RSK_MASK_SHOP_HINT] = Option::Bool("Mask Shop Hint", CVAR_RANDOMIZER_SETTING("MaskShopHint"), mOptionDescriptions[RSK_MASK_SHOP_HINT]);
    // TODO: Compasses show rewards/woth, maps show dungeon mode
    mOptions[RSK_BLUE_FIRE_ARROWS] = Option::Bool("Blue Fire Arrows", CVAR_RANDOMIZER_SETTING("BlueFireArrows"), mOptionDescriptions[RSK_BLUE_FIRE_ARROWS]);
    mOptions[RSK_SUNLIGHT_ARROWS] = Option::Bool("Sunlight Arrows", CVAR_RANDOMIZER_SETTING("SunlightArrows"), mOptionDescriptions[RSK_SUNLIGHT_ARROWS]);
    mOptions[RSK_INFINITE_UPGRADES] = Option::U8("Infinite Upgrades", {"Off", "Progressive", "Condensed Progressive"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("InfiniteUpgrades"), mOptionDescriptions[RSK_INFINITE_UPGRADES]);
    mOptions[RSK_SKELETON_KEY] = Option::Bool("Skeleton Key", CVAR_RANDOMIZER_SETTING("SkeletonKey"), mOptionDescriptions[RSK_SKELETON_KEY]);
    mOptions[RSK_ITEM_POOL] = Option::U8("Item Pool", {"Plentiful", "Balanced", "Scarce", "Minimal"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ItemPool"), mOptionDescriptions[RSK_ITEM_POOL], WidgetType::Combobox, RO_ITEM_POOL_BALANCED);
    mOptions[RSK_ICE_TRAPS] = Option::U8("Ice Traps", {"Off", "Normal", "Extra", "Mayhem", "Onslaught"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("IceTraps"), mOptionDescriptions[RSK_ICE_TRAPS], WidgetType::Combobox, RO_ICE_TRAPS_NORMAL);
    // TODO: Remove Double Defense, Progressive Goron Sword
    mOptions[RSK_STARTING_OCARINA] = Option::U8("Start with Ocarina", {"Off", "Fairy Ocarina", "Ocarina of Time"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingOcarina"), "", WidgetType::Combobox, RO_STARTING_OCARINA_OFF);
    mOptions[RSK_STARTING_DEKU_SHIELD] = Option::Bool("Start with Deku Shield", CVAR_RANDOMIZER_SETTING("StartingDekuShield"));
    mOptions[RSK_STARTING_KOKIRI_SWORD] = Option::Bool("Start with Kokiri Sword", CVAR_RANDOMIZER_SETTING("StartingKokiriSword"));
    mOptions[RSK_STARTING_MASTER_SWORD] = Option::Bool("Start with Master Sword", CVAR_RANDOMIZER_SETTING("StartingMasterSword"));
    mOptions[RSK_STARTING_CONSUMABLES] = Option::Bool("Start with Consumables", {"No", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingConsumables"), "", WidgetType::Checkbox, RO_GENERIC_OFF);
    mOptions[RSK_FULL_WALLETS] = Option::Bool("Full Wallets", {"No", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("FullWallets"), mOptionDescriptions[RSK_FULL_WALLETS], WidgetType::Checkbox, RO_GENERIC_OFF);
    mOptions[RSK_STARTING_ZELDAS_LULLABY] = Option::Bool("Start with Zelda's Lullaby", CVAR_RANDOMIZER_SETTING("StartingZeldasLullaby"), "", IMFLAG_NONE);
    mOptions[RSK_STARTING_EPONAS_SONG] = Option::Bool("Start with Epona's Song", CVAR_RANDOMIZER_SETTING("StartingEponasSong"), "", IMFLAG_NONE);
    mOptions[RSK_STARTING_SARIAS_SONG] = Option::Bool("Start with Saria's Song", CVAR_RANDOMIZER_SETTING("StartingSariasSong"), "", IMFLAG_NONE);
    mOptions[RSK_STARTING_SUNS_SONG] = Option::Bool("Start with Sun's Song", CVAR_RANDOMIZER_SETTING("StartingSunsSong"), "", IMFLAG_NONE);
    mOptions[RSK_STARTING_SONG_OF_TIME] = Option::Bool("Start with Song of Time", CVAR_RANDOMIZER_SETTING("StartingSongOfTime"), "", IMFLAG_NONE);
    mOptions[RSK_STARTING_SONG_OF_STORMS] = Option::Bool("Start with Song of Storms", CVAR_RANDOMIZER_SETTING("StartingSongOfStorms"), "", IMFLAG_NONE);
    mOptions[RSK_STARTING_MINUET_OF_FOREST] = Option::Bool("Start with Minuet of Forest", CVAR_RANDOMIZER_SETTING("StartingMinuetOfForest"), "", IMFLAG_NONE);
    mOptions[RSK_STARTING_BOLERO_OF_FIRE] = Option::Bool("Start with Bolero of Fire", CVAR_RANDOMIZER_SETTING("StartingBoleroOfFire"), "", IMFLAG_NONE);
    mOptions[RSK_STARTING_SERENADE_OF_WATER] = Option::Bool("Start with Serenade of Water", CVAR_RANDOMIZER_SETTING("StartingSerenadeOfWater"), "", IMFLAG_NONE);
    mOptions[RSK_STARTING_REQUIEM_OF_SPIRIT] = Option::Bool("Start with Requiem of Spirit", CVAR_RANDOMIZER_SETTING("StartingRequiemOfSpirit"), "", IMFLAG_NONE);
    mOptions[RSK_STARTING_NOCTURNE_OF_SHADOW] = Option::Bool("Start with Nocturne of Shadow", CVAR_RANDOMIZER_SETTING("StartingNocturneOfShadow"), "", IMFLAG_NONE);
    mOptions[RSK_STARTING_PRELUDE_OF_LIGHT] = Option::Bool("Start with Prelude of Light", CVAR_RANDOMIZER_SETTING("StartingPreludeOfLight"));
    mOptions[RSK_STARTING_SKULLTULA_TOKEN] = Option::U8("Gold Skulltula Tokens", {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingSkulltulaToken"), "", WidgetType::Slider);
    mOptions[RSK_STARTING_HEARTS] = Option::U8("Hearts", {NumOpts(1, 20)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingHearts"), "", WidgetType::Slider, 2);
    // TODO: Remainder of Starting Items
    mOptions[RSK_LOGIC_RULES] = Option::U8("Logic", {"Glitchless", "Glitched", "No Logic", "Vanilla"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("LogicRules"), mOptionDescriptions[RSK_LOGIC_RULES], WidgetType::Combobox, RO_LOGIC_GLITCHLESS);
    mOptions[RSK_ALL_LOCATIONS_REACHABLE] = Option::Bool("All Locations Reachable", {"Off", "On"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("AllLocationsReachable"), mOptionDescriptions[RSK_ALL_LOCATIONS_REACHABLE], WidgetType::Checkbox, RO_GENERIC_ON);
    mOptions[RSK_SKULLS_SUNS_SONG] = Option::Bool("Night Skulltula's Expect Sun's Song", CVAR_RANDOMIZER_SETTING("GsExpectSunsSong"), mOptionDescriptions[RSK_SKULLS_SUNS_SONG]);
    mOptions[RSK_DAMAGE_MULTIPLIER] = Option::U8("Damage Multiplier", {"x1/2", "x1", "x2", "x4", "x8", "x16", "OHKO"}, OptionCategory::Setting, "", "", WidgetType::Slider, RO_DAMAGE_MULTIPLIER_DEFAULT);
    // clang-format on

    mExcludeLocationsOptionsAreas.reserve(RCAREA_INVALID);

    mTrickOptions[RT_ACUTE_ANGLE_CLIP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, true, "Acute angle clip", "Enables locations requiring jumpslash clips through walls which meet at an acute angle.");
    mTrickOptions[RT_ADVANCED_CLIPS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, true, "Advanced clips", "Enables locations requiring clips through walls and objects requiring precise jumps or other tricks.");
    mTrickOptions[RT_BLANK_A] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, true, "Blank A", "Enables locations requiring blank A button; NOTE: this requires the 'Quick Putaway' restoration.");
    mTrickOptions[RT_DOOM_JUMP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, true, "Doom Jump", "Enables locations requiring doom jumps.");
    mTrickOptions[RT_EPG] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, true, "EPG", "Enables locations requiring use of the Entrance Point Glitch.");
    mTrickOptions[RT_EQUIP_SWAP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, true, "Equip Swap", "Enables locations requiring use of equip swap; NOTE: this may expect the 'Allow cursor to be over any slot' enhancement to be turned off.");
    mTrickOptions[RT_EQUIP_SWAP_EXPECTS_DINS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, true, "Equip Swap Require's Din's Fire", "Enables locations requiring use of equip swap once din's fire is found.");
    mTrickOptions[RT_FLAME_STORAGE] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, true, "Flame Storage", "Enables locations requiring flame storage.");
    mTrickOptions[RT_GROUND_CLIP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, true, "Ground Clip", "Enables locations requiring ground clips.");
    mTrickOptions[RT_GROUND_JUMP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, true, "Ground Jump", "Enables locations requiring ground jumps.");
    mTrickOptions[RT_HESS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, true, "HESS", "Enables locations requiring a Hyper Extended Super Slide.");
    mTrickOptions[RT_HOOKSHOT_CLIP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, true, "Hookshot Clip", "Enables locations requiring hookshot clips.");
    mTrickOptions[RT_HOOKSHOT_JUMP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, true, "Hookshot Jump", "Enables locations requiring hookshot jumps.");
    mTrickOptions[RT_ISG] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, true, "ISG", "Enables locations requiring use of the infinite sword glitch.");
    mTrickOptions[RT_VISIBLE_COLLISION] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::NOVICE}, false, "Pass Through Visible One-Way Collision", "Allows climbing through the platform to reach Impa's House Back as adult with no items and going through the Kakariko Village Gate as child when coming from the Mountain Trail side.");
    mTrickOptions[RT_GROTTOS_WITHOUT_AGONY] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::NOVICE}, false, "Hidden Grottos without Stone of Agony", "Allows entering hidden grottos without the Stone of Agony.");
    mTrickOptions[RT_FEWER_TUNIC_REQUIREMENTS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::INTERMEDIATE}, false, "Fewer Tunic Requirements", "Allows the following possible without Tunics:\n- Enter Water Temple. The area below the center pillar still requires Zora Tunic. Applies to MQ also.\n- Enter Fire Temple. Volvagia still requires Goron tunic. Applies to MQ also, and includes child access to first floor with dungeon shuffle.");
    mTrickOptions[RT_RUSTED_SWITCHES] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::NOVICE}, false, "Hammer Rusted Switches Through Walls", "Applies to: - Fire Temple Highest Goron Chest. - MQ Fire Temple Lizalfos Maze. - MQ Spirit Trial.");
    mTrickOptions[RT_FLAMING_CHESTS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::INTERMEDIATE}, false, "Flaming Chests", "The chests encircled in flames in Gerudo Training Grounds and in Spirit Temple can be opened by running into the flames while Link is invincible after taking damage.");
    // mTrickOptions[RT_BUNNY_HOOD_JUMPS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, false, "Bunny Hood Jumps", "Allows reaching locations using Bunny Hood's extended jumps.");
    // mTrickOptions[RT_DAMAGE_BOOST_SIMPLE] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, false, "Simple damage boosts", "Allows damage boosts to reach Hyrule Castle guards, the bomb bag area in DC and the Gerudo Valley crate Piece of Heart. Can be combined with \"Simple hover boosts\" for reaching far distances.");
    // mTrickOptions[RT_HOVER_BOOST_SIMPLE] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::ADVANCED}, false, "Simple hover boosts", "Allows equipping of hover boots when link is moving at high speeds to extend distance covered. Can be combined with \"Simple damage boosts\" for greater uses.");
    mTrickOptions[RT_BOMBCHU_BEEHIVES] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_NONE, {Tricks::Tag::NOVICE}, false, "Bombchu Beehives", "Allows exploding beehives with bombchus"),
    mTrickOptions[RT_KF_ADULT_GS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_KOKIRI_FOREST, {Tricks::Tag::NOVICE}, false, "Adult Kokiri Forest GS with Hover Boots", "Can be obtained without Hookshot by using the Hover Boots off of one of the roots.");
    mTrickOptions[RT_LW_BRIDGE] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_THE_LOST_WOODS, {Tricks::Tag::EXPERT}, false, "Jump onto the Lost Woods Bridge as Adult with Nothing", "With very precise movement it's possible for adult to jump onto the bridge without needing Longshot, Hover Boots, or Bean.");
    mTrickOptions[RT_LW_MIDO_BACKFLIP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_THE_LOST_WOODS, {Tricks::Tag::NOVICE}, false, "Backflip over Mido as Adult", "With a specific position and angle, you can backflip over Mido.");
    mTrickOptions[RT_LW_GS_BEAN] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_THE_LOST_WOODS, {Tricks::Tag::INTERMEDIATE}, false, "Lost Woods Adult GS without Bean", "You can collect the token with a precise Hookshot use, as long as you can kill the Skulltula somehow first. It can be killed using Longshot, Bow, Bombchus or Din's Fire.");
    mTrickOptions[RT_HC_STORMS_GS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_HYRULE_CASTLE, {Tricks::Tag::INTERMEDIATE}, false, "Hyrule Castle Storms Grotto GS with Just Boomerang", "With precise throws, the Boomerang alone can kill the Skulltula and collect the token, without first needing to blow up the wall.");
    mTrickOptions[RT_KAK_MAN_ON_ROOF] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_KAKARIKO_VILLAGE, {Tricks::Tag::NOVICE}, false, "Man on Roof without Hookshot", "Can be reached by side-hopping off the watchtower as either age, or by jumping onto the potion shop's roof from the ledge as adult.");
    mTrickOptions[RT_KAK_TOWER_GS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_KAKARIKO_VILLAGE, {Tricks::Tag::INTERMEDIATE}, false, "Kakariko Tower GS with Jump Slash", "Climb the tower as high as you can without touching the Gold Skulltula, then let go and jump slash immediately. By jump-slashing from as low on the ladder as possible to still hit the Skulltula, this trick can be done without taking fall damage.");
    mTrickOptions[RT_KAK_ADULT_WINDMILL_POH] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_KAKARIKO_VILLAGE, {Tricks::Tag::NOVICE}, false, "Windmill PoH as Adult with Nothing", "Can jump up to the spinning platform from below as adult.");
    // mTrickOptions[RT_KAK_CHILD_WINDMILL_POH] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_KAKARIKO_VILLAGE, {Tricks::Tag::EXTREME}, false, "Windmill PoH as Child with Precise Jump Slash", "Can jump up to the spinning platform from below as child with a precise jumpslash timed with the platforms rotation.");
    mTrickOptions[RT_KAK_ROOFTOP_GS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_KAKARIKO_VILLAGE, {Tricks::Tag::ADVANCED}, false, "Kakariko Rooftop GS with Hover Boots", "Take the Hover Boots from the entrance to Impa's House over to the rooftop of Skulltula House. From there, a precise Hover Boots backwalk with backflip can be used to get onto a hill above the side of the village. And then from there you can Hover onto Impa's rooftop to kill the Skulltula and backflip into the token.");
    mTrickOptions[RT_GY_POH] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_THE_GRAVEYARD, {Tricks::Tag::INTERMEDIATE}, false, "Graveyard Freestanding PoH with Boomerang", "Using a precise moving setup you can obtain the Piece of Heart by having the Boomerang interact with it along the return path.");
    mTrickOptions[RT_GY_CHILD_DAMPE_RACE_POH] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_THE_GRAVEYARD, {Tricks::Tag::NOVICE}, false, "Second Dampe Race as Child", "It is possible to complete the second dampe race as child in under a minute, but it is a strict time limit.");
    mTrickOptions[RT_GY_SHADOW_FIRE_ARROWS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_THE_GRAVEYARD, {Tricks::Tag::EXPERT}, false, "Shadow Temple Entry with Fire Arrows", "It is possible to light all of the torches to open the Shadow Temple entrance with just Fire Arrows, but you must be very quick, precise, and strategic with how you take your shots.");
    mTrickOptions[RT_DMT_SOIL_GS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, {Tricks::Tag::INTERMEDIATE}, false, "Death Mountain Trail Soil GS without Destroying Boulder", "Bugs will go into the soft soil even while the boulder is still blocking the entrance. Then, using a precise moving setup you can kill the Gold Skulltula and obtain the token by having the Boomerang interact with it along the return path.");
    mTrickOptions[RT_DMT_BOMBABLE] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, {Tricks::Tag::INTERMEDIATE}, false, "Death Mountain Trail Chest with Strength", "Child Link can blow up the wall using a nearby bomb flower. You must backwalk with the flower and then quickly throw it toward the wall.");
    mTrickOptions[RT_DMT_HOOKSHOT_LOWER_GS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, {Tricks::Tag::INTERMEDIATE}, false, "Death Mountain Trail Lower Red Rock GS with Hookshot", "After killing the Skulltula, the token can be fished out of the rock without needing to destroy it, by using the Hookshot in the correct way.");
    mTrickOptions[RT_DMT_HOVERS_LOWER_GS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, {Tricks::Tag::ADVANCED}, false, "Death Mountain Trail Lower Red Rock GS with Hover Boots", "After killing the Skulltula, the token can be collected without needing to destroy the rock by backflipping down onto it with the Hover Boots. First use the Hover Boots to stand on a nearby fence, and go for the Skulltula Token from there.");
    mTrickOptions[RT_DMT_BEAN_LOWER_GS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, {Tricks::Tag::EXPERT}, false, "Death Mountain Trail Lower Red Rock GS with Magic Bean", "After killing the Skulltula, the token can be collected without needing to destroy the rock by jumping down onto it from the bean plant, midflight, with precise timing and positioning.");
    mTrickOptions[RT_DMT_JS_LOWER_GS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, {Tricks::Tag::INTERMEDIATE}, false, "Death Mountain Trail Lower Red Rock GS with Jump Slash", "After killing the Skulltula, the token can be collected without needing to destroy the rock by jump slashing from a precise angle.");
    mTrickOptions[RT_DMT_CLIMB_HOVERS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, {Tricks::Tag::ADVANCED}, false, "Death Mountain Trail Climb with Hover Boots", "It is possible to use the Hover Boots to bypass needing to destroy the boulders blocking the path to the top of Death Mountain.");
    mTrickOptions[RT_DMT_UPPER_GS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, {Tricks::Tag::NOVICE}, false, "Death Mountain Trail Upper Red Rock GS without Hammer", "After killing the Skulltula, the token can be collected by backflipping into the rock at the correct angle.");
    // mTrickOptions[RT_DMT_BOLERO_BIGGORON] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, {Tricks::Tag::INTERMEDIATE}, false, "Deliver Eye Drops with Bolero of Fire", "Playing a warp song normally causes a trade item to spoil immediately, however, it is possible use Bolero to reach Biggoron and still deliver the Eye Drops before they spoil. If you do not wear the Goron Tunic, the heat timer inside the crater will override the trade item\'s timer. When you exit to Death Mountain Trail you will have one second to show the Eye Drops before they expire. You can get extra time to show the Eye Drops if you warp immediately upon receiving them. If you don't have many hearts, you may have to reset the heat timer by quickly dipping in and out of Darunia\'s chamber or quickly equipping and unequipping the Goron Tunic. This trick does not apply if \"Randomize Warp Song Destinations\" is enabled, or if the settings are such that trade items do not need to be delivered within a time limit.");
    mTrickOptions[RT_GC_POT] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_GORON_CITY, {Tricks::Tag::ADVANCED}, false, "Goron City Spinning Pot PoH with Bombchu", "A Bombchu can be used to stop the spinning pot, but it can be quite finicky to get it to work.");
    mTrickOptions[RT_GC_POT_STRENGTH] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_GORON_CITY, {Tricks::Tag::INTERMEDIATE}, false, "Goron City Spinning Pot PoH with Strength", "Allows for stopping the Goron City Spinning Pot using a bomb flower alone, requiring strength in lieu of inventory explosives.");
    mTrickOptions[RT_GC_ROLLING_STRENGTH] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_GORON_CITY, {Tricks::Tag::INTERMEDIATE}, false, "Rolling Goron (Hot Rodder Goron) as Child with Strength", "Use the bombflower on the stairs or near Medigoron. Timing is tight, especially without backwalking.");
    mTrickOptions[RT_GC_LEFTMOST] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_GORON_CITY, {Tricks::Tag::ADVANCED}, false, "Goron City Maze Left Chest with Hover Boots", "A precise backwalk starting from on top of the crate and ending with a precisely-timed backflip can reach this chest without needing either the Hammer or Silver Gauntlets.");
    mTrickOptions[RT_GC_GROTTO] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_GORON_CITY, {Tricks::Tag::ADVANCED}, false, "Goron City Grotto with Hookshot While Taking Damage", "It is possible to reach the Goron City Grotto by quickly using the Hookshot while in the midst of taking damage from the lava floor.");
    mTrickOptions[RT_GC_LINK_GORON_DINS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_GORON_CITY, {Tricks::Tag::NOVICE}, false, "Stop Link the Goron with Din\'s Fire", "The timing is quite awkward.");
    mTrickOptions[RT_DMC_HOVER_BEAN_POH] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DEATH_MOUNTAIN_CRATER, {Tricks::Tag::NOVICE}, false, "Crater\'s Bean PoH with Hover Boots", "Hover from the base of the bridge near Goron City and walk up the very steep slope.");
    mTrickOptions[RT_DMC_BOLERO_JUMP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DEATH_MOUNTAIN_CRATER, {Tricks::Tag::EXTREME}, false, "Death Mountain Crater Jump to Bolero", "As Adult , using a shield to drop a pot while you have the perfect speed and position, the pot can push you that little extra distance you need to jump across the gap in the bridge.");
    mTrickOptions[RT_DMC_BOULDER_JS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DEATH_MOUNTAIN_CRATER, {Tricks::Tag::NOVICE}, false, "Death Mountain Crater Upper to Lower with Hammer", "With the Hammer, you can jump slash the rock twice in the same jump in order to destroy it before you fall into the lava.");
    mTrickOptions[RT_DMC_BOULDER_SKIP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DEATH_MOUNTAIN_CRATER, {Tricks::Tag::INTERMEDIATE}, false, "Death Mountain Crater Upper to Lower Boulder Skip", "As adult, With careful positioning, you can jump to the ledge where the boulder is, then use repeated ledge grabs to shimmy to a climbable ledge. This trick supersedes \"Death Mountain Crater Upper to Lower with Hammer\".");
    mTrickOptions[RT_ZR_LOWER] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_ZORAS_RIVER, {Tricks::Tag::INTERMEDIATE}, false, "Zora\'s River Lower Freestanding PoH as Adult with Nothing", "Adult can reach this PoH with a precise jump, no Hover Boots required.");
    mTrickOptions[RT_ZR_UPPER] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_ZORAS_RIVER, {Tricks::Tag::INTERMEDIATE}, false, "Zora\'s River Upper Freestanding PoH as Adult with Nothing", "Adult can reach this PoH with a precise jump, no Hover Boots required.");
    mTrickOptions[RT_ZR_HOVERS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_ZORAS_RIVER, {Tricks::Tag::NOVICE}, false, "Zora\'s Domain Entry with Hover Boots", "Can hover behind the waterfall as adult.");
    mTrickOptions[RT_ZR_CUCCO] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_ZORAS_RIVER, {Tricks::Tag::NOVICE}, false, "Zora\'s Domain Entry with Cucco", "You can fly behind the waterfall with a Cucco as child.");
    mTrickOptions[RT_ZD_KING_ZORA_SKIP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_ZORAS_DOMAIN, {Tricks::Tag::INTERMEDIATE}, false, "Skip King Zora as Adult with Nothing", "With a precise jump as adult, it is possible to get on the fence next to King Zora from the front to access Zora's Fountain.");
    mTrickOptions[RT_ZD_GS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_ZORAS_DOMAIN, {Tricks::Tag::INTERMEDIATE}, false, "Zora\'s Domain GS with No Additional Items", "A precise jump slash can kill the Skulltula and recoil back onto the top of the frozen waterfall. To kill it, the logic normally guarantees one of Hookshot, Bow, or Magic.");
    mTrickOptions[RT_LH_LAB_WALL_GS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_LAKE_HYLIA, {Tricks::Tag::NOVICE}, false, "Lake Hylia Lab Wall GS with Jump Slash", "The jump slash to actually collect the token is somewhat precise.");
    mTrickOptions[RT_LH_LAB_DIVING] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_LAKE_HYLIA, {Tricks::Tag::NOVICE}, false, "Lake Hylia Lab Dive without Gold Scale", "Remove the Iron Boots in the midst of Hookshotting the underwater crate.");
    mTrickOptions[RT_LH_WATER_HOOKSHOT] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_LAKE_HYLIA, {Tricks::Tag::INTERMEDIATE}, false, "Water Temple Entry without Iron Boots using Hookshot", "When entering Water Temple using Gold Scale instead of Iron Boots, the Longshot is usually used to be able to hit the switch and open the gate. But, by standing in a particular spot, the switch can be hit with only the reach of the Hookshot.");
    mTrickOptions[RT_GV_CRATE_HOVERS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_GERUDO_VALLEY, {Tricks::Tag::INTERMEDIATE}, false, "Gerudo Valley Crate PoH as Adult with Hover Boots", "From the far side of Gerudo Valley, a precise Hover Boots movement and jump-slash recoil can allow adult to reach the ledge with the crate PoH without needing Longshot. You will take fall damage.");
    mTrickOptions[RT_GF_KITCHEN] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_GERUDO_FORTRESS, {Tricks::Tag::NOVICE}, false, "Thieves\' Hideout \"Kitchen\" with No Additional Items", "Allows passing through the kitchen by avoiding being seen by the guards. The logic normally guarantees Bow or Hookshot to stun them from a distance, or Hover Boots to cross the room without needing to deal with the guards.");
    mTrickOptions[RT_GF_JUMP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_GERUDO_FORTRESS, {Tricks::Tag::NOVICE}, false, "Gerudo\'s Fortress Ledge Jumps", "Adult can jump onto the top roof of the fortress without going through the interior of the hideout.");
    // mTrickOptions[RT_HW_BUNNY_CROSSING] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_HAUNTED_WASTELAND, {Tricks::Tag::NOVICE}, false, "Wasteland Crossing with Bunny Hood", "You can beat the quicksand by using the increased speed of the Bunny Hood. Note that jumping to the carpet merchant as child typically requires a fairly precise jump slash.");
    mTrickOptions[RT_HW_CROSSING] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_HAUNTED_WASTELAND, {Tricks::Tag::INTERMEDIATE}, false, "Wasteland Crossing without Hover Boots or Longshot", "You can beat the quicksand by backwalking across it in a specific way. Note that jumping to the carpet merchant as child typically requires a fairly precise jump slash.");
    mTrickOptions[RT_LENS_HW] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_HAUNTED_WASTELAND, {Tricks::Tag::INTERMEDIATE}, false, "Lensless Wasteland", "By memorizing the path, you can travel through the Wasteland without using the Lens of Truth to see the Poe. The equivalent trick for going in reverse through the Wasteland is \"Reverse Wasteland\".");
    mTrickOptions[RT_HW_REVERSE] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_HAUNTED_WASTELAND, {Tricks::Tag::INTERMEDIATE}, false, "Reverse Wasteland", "By memorizing the path, you can travel through the Wasteland in reverse. Note that jumping to the carpet merchant as child typically requires a fairly precise jump slash. The equivalent trick for going forward through the Wasteland is \"Lensless Wasteland\". To cross the river of sand with no additional items, be sure to also enable \"Wasteland Crossing without Hover Boots or Longshot\". Unless all overworld entrances are randomized, child Link will not be expected to do anything at Gerudo's Fortress.");
    mTrickOptions[RT_COLOSSUS_GS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DESERT_COLOSSUS, {Tricks::Tag::NOVICE}, false, "Colossus Hill GS with Hookshot", "Somewhat precise. If you kill enough Leevers you can get enough of a break to take some time to aim more carefully.");
    mTrickOptions[RT_DEKU_BASEMENT_GS] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_DEKU_TREE, {Tricks::Tag::NOVICE}, false, "Deku Tree Basement Vines GS with Jump Slash", "Can be defeated by doing a precise jump slash.");
    mTrickOptions[RT_DEKU_B1_SKIP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DEKU_TREE, {Tricks::Tag::INTERMEDIATE}, false, "Deku Tree Basement without Slingshot", "A precise jump can be used to skip needing to use the Slingshot to go around B1 of the Deku Tree. If used with the \"Closed Forest\" setting, a Slingshot will not be guaranteed to exist somewhere inside the Forest. This trick applies to both Vanilla and Master Quest.");
    mTrickOptions[RT_DEKU_B1_BOW_WEBS] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_DEKU_TREE, {Tricks::Tag::NOVICE}, false, "Deku Tree Basement Web to Gohma with Bow", "All spider web walls in the Deku Tree basement can be burnt as adult with just a bow by shooting through torches. This trick only applies to the circular web leading to Gohma; the two vertical webs are always in logic. Backflip onto the chest near the torch at the bottom of the vine wall. With precise positioning you can shoot through the torch to the right edge of the circular web. This allows completion of adult Deku Tree with no fire source.");
    mTrickOptions[RT_DEKU_B1_BACKFLIP_OVER_SPIKED_LOG] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_DEKU_TREE, {Tricks::Tag::NOVICE}, false, "Deku Tree Basement Backflip over Spiked Log", "Allows backflipping over the spiked log in the deku tree basement in vanilla. Only relevant if \"Shuffle Swim\" is enabled.");
    mTrickOptions[RT_DEKU_MQ_COMPASS_GS] = TrickOption::LogicTrick(RCQUEST_MQ, RA_DEKU_TREE, {Tricks::Tag::NOVICE}, false, "Deku Tree MQ Compass Room GS Boulders with Just Hammer", "Climb to the top of the vines, then let go and jump slash immediately to destroy the boulders using the Hammer, without needing to spawn a Song of Time block.");
    mTrickOptions[RT_DEKU_MQ_LOG] = TrickOption::LogicTrick(RCQUEST_MQ, RA_DEKU_TREE, {Tricks::Tag::NOVICE}, false, "Deku Tree MQ Roll Under the Spiked Log", "You can get past the spiked log by rolling to briefly shrink your hitbox. As adult, the timing is a bit more precise.");
    mTrickOptions[RT_DC_SCARECROW_GS] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_DODONGOS_CAVERN, {Tricks::Tag::NOVICE}, false, "Dodongo\'s Cavern Scarecrow GS with Armos Statue", "You can jump off an Armos Statue to reach the alcove with the Gold Skulltula. It takes quite a long time to pull the statue the entire way. The jump to the alcove can be a bit picky when done as child.");
    mTrickOptions[RT_DC_VINES_GS] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_DODONGOS_CAVERN, {Tricks::Tag::NOVICE}, false, "Dodongo\'s Cavern Vines GS from Below with Longshot", "The vines upon which this Skulltula rests are one- sided collision. You can use the Longshot to get it from below, by shooting it through the vines, bypassing the need to lower the staircase.");
    mTrickOptions[RT_DC_STAIRCASE] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_DODONGOS_CAVERN, {Tricks::Tag::NOVICE}, false, "Dodongo\'s Cavern Staircase with Bow", "The Bow can be used to knock down the stairs with two well-timed shots.");
    mTrickOptions[RT_DC_SLINGSHOT_SKIP] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_DODONGOS_CAVERN, {Tricks::Tag::EXPERT}, false, "Dodongo\'s Cavern Child Slingshot Skips", "With precise platforming, child can cross the platforms while the flame circles are there. When enabling this trick, it's recommended that you also enable the Adult variant: \"Dodongo's Cavern Spike Trap Room Jump without Hover Boots\".");
    mTrickOptions[RT_DC_SCRUB_ROOM] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_DODONGOS_CAVERN, {Tricks::Tag::NOVICE}, false, "Dodongo\'s Cavern Two Scrub Room with Strength", "With help from a conveniently-positioned block, Adult can quickly carry a bomb flower over to destroy the mud wall blocking the room with two Deku Scrubs.");
    mTrickOptions[RT_DC_JUMP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DODONGOS_CAVERN, {Tricks::Tag::NOVICE}, false, "Dodongo\'s Cavern Spike Trap Room Jump without Hover Boots", "The jump is adult Link only. Applies to both Vanilla and MQ.");
    mTrickOptions[RT_DC_HAMMER_FLOOR] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_DODONGOS_CAVERN, {Tricks::Tag::NOVICE}, false, "Dodongo\'s Cavern Smash the Boss Lobby Floor", "The bombable floor before King Dodongo can be destroyed with Hammer if hit in the very center. This is only relevant with Shuffle Boss Entrances or if Dodongo's Cavern is MQ and either variant of \"Dodongo's Cavern MQ Light the Eyes with Strength\" is on.");
    mTrickOptions[RT_DC_MQ_CHILD_BOMBS] = TrickOption::LogicTrick(RCQUEST_MQ, RA_DODONGOS_CAVERN, {Tricks::Tag::ADVANCED}, false, "Dodongo\'s Cavern MQ Early Bomb Bag Area as Child", "With a precise jump slash from above, you can reach the Bomb Bag area as only child without needing a Slingshot. You will take fall damage.");
    mTrickOptions[RT_DC_MQ_CHILD_EYES] = TrickOption::LogicTrick(RCQUEST_MQ, RA_DODONGOS_CAVERN, {Tricks::Tag::EXPERT}, false, "Dodongo\'s Cavern MQ Light the Eyes with Strength as Child", "If you move very quickly, it is possible to use the bomb flower at the top of the room to light the eyes. To perform this trick as child is significantly more difficult than adult. The player is also expected to complete the DC back area without explosives, including getting past the Armos wall to the switch for the boss door.");
    mTrickOptions[RT_DC_MQ_ADULT_EYES] = TrickOption::LogicTrick(RCQUEST_MQ, RA_DODONGOS_CAVERN, {Tricks::Tag::ADVANCED}, false, "Dodongo\'s Cavern MQ Light the Eyes with Strength as Adult", "If you move very quickly, it is possible to use the bomb flower at the top of the room to light the eyes.");
    mTrickOptions[RT_JABU_ALCOVE_JUMP_DIVE] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_JABU_JABUS_BELLY, {Tricks::Tag::NOVICE}, false, "Jabu Underwater Alcove as Adult with Jump Dive", "Standing above the underwater tunnel leading to the scrub, jump down and swim through the tunnel. This allows adult to access the alcove with no Scale or Iron Boots. In vanilla Jabu, this alcove has a business scrub. In MQ Jabu, it has the compass chest and a door switch for the main floor.");
    mTrickOptions[RT_JABU_BOSS_HOVER] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_JABU_JABUS_BELLY, {Tricks::Tag::INTERMEDIATE}, false, "Jabu Near Boss Room with Hover Boots", "A box for the blue switch can be carried over by backwalking with one while the elevator is at its peak. Alternatively, you can skip transporting a box by quickly rolling from the switch and opening the door before it closes. However, the timing for this is very tight.");
    mTrickOptions[RT_JABU_NEAR_BOSS_RANGED] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_JABU_JABUS_BELLY, {Tricks::Tag::NOVICE}, false, "Jabu Near Boss Ceiling Switch/GS without Boomerang or Explosives", "Vanilla Jabu: From near the entrance into the room, you can hit the switch that opens the door to the boss room using a precisely-aimed use of the Slingshot, Bow, or Longshot. As well, if you climb to the top of the vines you can stand on the right edge of the platform and shoot around the glass. From this distance, even the Hookshot can reach the switch. This trick is only relevant if \"Shuffle Boss Entrances\" is enabled. MQ Jabu: A Gold Skulltula Token can be collected with the Hookshot or Longshot using the same methods as hitting the switch in vanilla. This MQ trick is not currently relevant in logic.");
    mTrickOptions[RT_JABU_NEAR_BOSS_EXPLOSIVES] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_JABU_JABUS_BELLY, {Tricks::Tag::INTERMEDIATE}, false, "Jabu Near Boss Ceiling Switch with Explosives", "You can hit the switch that opens the door to the boss room using a precisely-aimed Bombchu. Also, using the Hover Boots, adult can throw a Bomb at the switch. This trick is only relevant if \"Shuffle Boss Entrances\" is enabled.");
    mTrickOptions[RT_LENS_JABU_MQ] = TrickOption::LogicTrick(RCQUEST_MQ, RA_JABU_JABUS_BELLY, {Tricks::Tag::NOVICE}, false, "Jabu MQ without Lens of Truth", "Removes the requirements for the Lens of Truth in Jabu MQ.");
    // mTrickOptions[RT_JABU_MQ_RANG_JUMP] = TrickOption::LogicTrick(RCQUEST_MQ, RA_JABU_JABUS_BELLY, {Tricks::Tag::ADVANCED}, false, "Jabu MQ Compass Chest with Boomerang", "Boomerang can reach the cow switch to spawn the chest by targeting the cow, jumping off of the ledge where the chest spawns, and throwing the Boomerang in midair. This is only relevant with Jabu Jabu's Belly dungeon shortcuts enabled.");
    mTrickOptions[RT_JABU_MQ_SOT_GS] = TrickOption::LogicTrick(RCQUEST_MQ, RA_JABU_JABUS_BELLY, {Tricks::Tag::INTERMEDIATE}, false, "Jabu MQ Song of Time Block GS with Boomerang", "Allow the Boomerang to return to you through the Song of Time block to grab the token.");
    mTrickOptions[RT_LENS_BOTW] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_BOTTOM_OF_THE_WELL, {Tricks::Tag::NOVICE}, false, "Bottom of the Well without Lens of Truth", "Removes the requirements for the Lens of Truth in Bottom of the Well.");
    mTrickOptions[RT_BOTW_CHILD_DEADHAND] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_BOTTOM_OF_THE_WELL, {Tricks::Tag::NOVICE}, false, "Child Dead Hand without Kokiri Sword", "Requires 9 sticks or 5 jump slashes.");
    mTrickOptions[RT_BOTW_BASEMENT] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_BOTTOM_OF_THE_WELL, {Tricks::Tag::NOVICE}, false, "Bottom of the Well Map Chest with Strength & Sticks", "The chest in the basement can be reached with strength by doing a jump slash with a lit stick to access the bomb flowers.");
    mTrickOptions[RT_BOTW_MQ_PITS] = TrickOption::LogicTrick(RCQUEST_MQ, RA_BOTTOM_OF_THE_WELL, {Tricks::Tag::NOVICE}, false, "Bottom of the Well MQ Jump Over the Pits", "While the pits in Bottom of the Well don't allow you to jump just by running straight at them, you can still get over them by side-hopping or backflipping across. With explosives, this allows you to access the central areas without Zelda's Lullaby. With Zelda's Lullaby, it allows you to access the west inner room without explosives.");
    mTrickOptions[RT_BOTW_MQ_DEADHAND_KEY] = TrickOption::LogicTrick(RCQUEST_MQ, RA_BOTTOM_OF_THE_WELL, {Tricks::Tag::NOVICE}, false, "Bottom of the Well MQ Dead Hand Freestanding Key with Boomerang", "Boomerang can fish the item out of the rubble without needing explosives to blow it up.");
    mTrickOptions[RT_FOREST_FIRST_GS] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_FOREST_TEMPLE, {Tricks::Tag::NOVICE}, false, "Forest Temple First Room GS with Difficult-to-Use Weapons", "Allows killing this Skulltula with Sword or Sticks by jump slashing it as you let go from the vines. You can avoid taking fall damage by recoiling onto the tree. Also allows killing it as Child with a Bomb throw. It's much more difficult to use a Bomb as child due to Child Link's shorter height.");
    mTrickOptions[RT_FOREST_OUTDOORS_EAST_GS] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_FOREST_TEMPLE, {Tricks::Tag::NOVICE}, false, "Forest Temple East Courtyard GS with Boomerang", "Precise Boomerang throws can allow child to kill the Skulltula and collect the token.");
    mTrickOptions[RT_FOREST_VINES] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_FOREST_TEMPLE, {Tricks::Tag::NOVICE}, false, "Forest Temple East Courtyard Vines with Hookshot", "The vines in Forest Temple leading to where the well drain switch is in the standard form can be barely reached with just the Hookshot. Applies to MQ also.");
    mTrickOptions[RT_FOREST_OUTDOORS_LEDGE] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_FOREST_TEMPLE, {Tricks::Tag::NOVICE}, false, "Forest Temple NE Outdoors Ledge with Hover Boots", "With precise Hover Boots movement you can fall down to this ledge from upper balconies. If done precisely enough, it is not necessary to take fall damage. In MQ, this skips a Longshot requirement. In Vanilla, this can skip a Hookshot requirement in entrance randomizer.");
    mTrickOptions[RT_FOREST_DOORFRAME] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_FOREST_TEMPLE, {Tricks::Tag::ADVANCED}, false, "Forest Temple East Courtyard Door Frame with Hover Boots", "A precise Hover Boots movement from the upper balconies in this courtyard can be used to get on top of the door frame. Applies to both Vanilla and Master Quest. In Vanilla, from on top the door frame you can summon Pierre, allowing you to access the falling ceiling room early. In Master Quest, this allows you to obtain the GS on the door frame as adult without Hookshot or Song of Time.");
    mTrickOptions[RT_FOREST_OUTSIDE_BACKDOOR] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_FOREST_TEMPLE, {Tricks::Tag::ADVANCED}, false, "Forest Temple Outside Backdoor with Jump Slash", "A jump slash recoil can be used to reach the ledge in the block puzzle room that leads to the west courtyard. This skips a potential Hover Boots requirement in vanilla, and it can sometimes apply in MQ as well. This trick can be performed as both ages.");
    mTrickOptions[RT_FOREST_MQ_WELL_SWIM] = TrickOption::LogicTrick(RCQUEST_MQ, RA_FOREST_TEMPLE, {Tricks::Tag::ADVANCED}, false, "Swim Through Forest Temple MQ Well with Hookshot", "Shoot the vines in the well as low and as far to the right as possible, and then immediately swim under the ceiling to the right. This can only be required if Forest Temple is in its Master Quest form.");
    mTrickOptions[RT_FOREST_MQ_BLOCK_PUZZLE] = TrickOption::LogicTrick(RCQUEST_MQ, RA_FOREST_TEMPLE, {Tricks::Tag::NOVICE}, false, "Skip Forest Temple MQ Block Puzzle with Bombchu", "Send the Bombchu straight up the center of the wall directly to the left upon entering the room.");
    //Child with hovers cannot do this from the lower floor, and most go to the upper floor which needs goron bracelet. Adult can do this with hammer and KSword, But child cannot. 
    mTrickOptions[RT_FOREST_MQ_JS_HALLWAY_SWITCH] = TrickOption::LogicTrick(RCQUEST_MQ, RA_FOREST_TEMPLE, {Tricks::Tag::NOVICE}, false, "Forest Temple MQ Twisted Hallway Switch with Jump Slash", "The switch to twist the hallway can be hit with a jump slash through the glass block. To get in front of the switch, either use the Hover Boots or hit the shortcut switch at the top of the room and jump from the glass blocks that spawn. Sticks can be used as child, but the Kokiri Sword is too short to reach through the glass.");
    // mTrickOptions[RT_FOREST_MQ_HOOKSHOT_HALLWAY_SWITCH] = TrickOption::LogicTrick(RCQUEST_MQ, RA_FOREST_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Forest Temple MQ Twisted Hallway Switch with Hookshot", "There's a very small gap between the glass block and the wall. Through that gap you can hookshot the target on the ceiling.");
    mTrickOptions[RT_FOREST_MQ_RANG_HALLWAY_SWITCH] = TrickOption::LogicTrick(RCQUEST_MQ, RA_FOREST_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Forest Temple MQ Twisted Hallway Switch with Boomerang", "The Boomerang can return to Link through walls, allowing child to hit the hallway switch. This can be used to allow adult to pass through later, or in conjuction with \"Forest Temple Outside Backdoor with Jump Slash\".");
    mTrickOptions[RT_FIRE_BOSS_DOOR_JUMP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_FIRE_TEMPLE, {Tricks::Tag::NOVICE}, false, "Fire Temple Boss Door without Hover Boots or Pillar", "The Fire Temple Boss Door can be reached as adult with a precise jump. You must be touching the side wall of the room so that Link will grab the ledge from farther away than is normally possible.");
    //Is also used in MQ logic, but has no practical effect there as of now
    mTrickOptions[RT_FIRE_SOT] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_FIRE_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Fire Temple Song of Time Room GS without Song of Time", "A precise jump can be used to reach this room.");
    mTrickOptions[RT_FIRE_STRENGTH] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_FIRE_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Fire Temple Climb without Strength", "A precise jump can be used to skip pushing the block.");
    mTrickOptions[RT_FIRE_SCARECROW] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_FIRE_TEMPLE, {Tricks::Tag::EXPERT}, false, "Fire Temple East Tower without Scarecrow\'s Song", "Also known as \"Pixelshot\". The Longshot can reach the target on the elevator itself, allowing you to skip needing to spawn the scarecrow.");
    mTrickOptions[RT_FIRE_FLAME_MAZE] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_FIRE_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Fire Temple Flame Wall Maze Skip", "If you move quickly you can sneak past the edge of a flame wall before it can rise up to block you. To do it without taking damage is more precise. Allows you to progress without needing either a Small Key or Hover Boots.");
    mTrickOptions[RT_FIRE_MQ_NEAR_BOSS] = TrickOption::LogicTrick(RCQUEST_MQ, RA_FIRE_TEMPLE, {Tricks::Tag::NOVICE}, false, "Fire Temple MQ Chest Near Boss without Breaking Crate", "The hitbox for the torch extends a bit outside of the crate. Shoot a flaming arrow at the side of the crate to light the torch without needing to get over there and break the crate.");
    mTrickOptions[RT_FIRE_MQ_BLOCKED_CHEST] = TrickOption::LogicTrick(RCQUEST_MQ, RA_FIRE_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Fire Temple MQ Big Lava Room Blocked Door without Hookshot", "There is a gap between the hitboxes of the flame wall in the big lava room. If you know where this gap is located, you can jump through it and skip needing to use the Hookshot. To do this without taking damage is more precise.");
    mTrickOptions[RT_FIRE_MQ_BK_CHEST] = TrickOption::LogicTrick(RCQUEST_MQ, RA_FIRE_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Fire Temple MQ Boss Key Chest without Bow", "It is possible to light both of the timed torches to unbar the door to the boss key chest's room with just Din's Fire if you move very quickly between the two torches. It is also possible to unbar the door with just Din's by abusing an oversight in the way the game counts how many torches have been lit.");
    mTrickOptions[RT_FIRE_MQ_CLIMB] = TrickOption::LogicTrick(RCQUEST_MQ, RA_FIRE_TEMPLE, {Tricks::Tag::NOVICE}, false, "Fire Temple MQ Climb without Fire Source", "You can use the Hover Boots to hover around to the climbable wall, skipping the need to use a fire source and spawn a Hookshot target.");
    mTrickOptions[RT_FIRE_MQ_MAZE_SIDE_ROOM] = TrickOption::LogicTrick(RCQUEST_MQ, RA_FIRE_TEMPLE, {Tricks::Tag::NOVICE}, false, "Fire Temple MQ Lizalfos Maze Side Room without Box", "You can walk from the blue switch to the door and quickly open the door before the bars reclose. This skips needing to reach the upper sections of the maze to get a box to place on the switch.");
    mTrickOptions[RT_FIRE_MQ_MAZE_HOVERS] = TrickOption::LogicTrick(RCQUEST_MQ, RA_FIRE_TEMPLE, {Tricks::Tag::NOVICE}, false, "Fire Temple MQ Lower to Upper Lizalfos Maze with Hover Boots", "Use the Hover Boots off of a crate to climb to the upper maze without needing to spawn and use the Hookshot targets.");
    mTrickOptions[RT_FIRE_MQ_MAZE_JUMP] = TrickOption::LogicTrick(RCQUEST_MQ, RA_FIRE_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Fire Temple MQ Lower to Upper Lizalfos Maze with Precise Jump", "A precise jump off of a crate can be used to climb to the upper maze without needing to spawn and use the Hookshot targets. This trick supersedes both \"Fire Temple MQ Lower to Upper Lizalfos Maze with Hover Boots\" and \"Fire Temple MQ Lizalfos Maze Side Room without Box\".");
    mTrickOptions[RT_FIRE_MQ_ABOVE_MAZE_GS] = TrickOption::LogicTrick(RCQUEST_MQ, RA_FIRE_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Fire Temple MQ Above Flame Wall Maze GS from Below with Longshot", "The floor of the room that contains this Skulltula is only solid from above. From the maze below, the Longshot can be shot through the ceiling to obtain the token with two fewer small keys than normal.");
    mTrickOptions[RT_FIRE_MQ_FLAME_MAZE] = TrickOption::LogicTrick(RCQUEST_MQ, RA_FIRE_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Fire Temple MQ Flame Wall Maze Skip", "If you move quickly you can sneak past the edge of a flame wall before it can rise up to block you. To do it without taking damage is more precise. Allows you to reach the side room GS without needing Song of Time or Hover Boots. If either of \"Fire Temple MQ Lower to Upper Lizalfos Maze with Hover Boots\" or \"with Precise Jump\" are enabled, this also allows you to progress deeper into the dungeon without Hookshot.");
    mTrickOptions[RT_WATER_LONGSHOT_TORCH] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_WATER_TEMPLE, {Tricks::Tag::NOVICE}, false, "Water Temple Torch Longshot", "Stand on the eastern side of the central pillar and longshot the torches on the bottom level. Swim through the corridor and float up to the top level. This allows access to this area and lower water levels without Iron Boots. The majority of the tricks that allow you to skip Iron Boots in the Water Temple are not going to be relevant unless this trick is first enabled.");
    mTrickOptions[RT_WATER_CRACKED_WALL_HOVERS] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_WATER_TEMPLE, {Tricks::Tag::NOVICE}, false, "Water Temple Cracked Wall with Hover Boots", "With a midair side-hop while wearing the Hover Boots, you can reach the cracked wall without needing to raise the water up to the middle level.");
    mTrickOptions[RT_WATER_CRACKED_WALL] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_WATER_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Water Temple Cracked Wall with No Additional Items", "A precise jump slash (among other methods) will get you to the cracked wall without needing the Hover Boots or to raise the water to the middle level. This trick supersedes \"Water Temple Cracked Wall with Hover Boots\".");
    mTrickOptions[RT_WATER_BK_REGION] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_WATER_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Water Temple Boss Key Region with Hover Boots", "With precise Hover Boots movement it is possible to reach the boss key chest's region without needing the Longshot. It is not necessary to take damage from the spikes. The Gold Skulltula Token in the following room can also be obtained with just the Hover Boots.");
    mTrickOptions[RT_WATER_NORTH_BASEMENT_LEDGE_JUMP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_WATER_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Water Temple North Basement Ledge with Precise Jump", "In the northern basement there's a ledge from where, in vanilla Water Temple, boulders roll out into the room. Normally to jump directly to this ledge logically requires the Hover Boots, but with precise jump, it can be done without them. This trick applies to both Vanilla and Master Quest.");
    mTrickOptions[RT_WATER_BK_JUMP_DIVE] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_WATER_TEMPLE, {Tricks::Tag::NOVICE}, false, "Water Temple Boss Key Jump Dive", "Stand on the very edge of the raised corridor leading from the push block room to the rolling boulder corridor. Face the gold skulltula on the waterfall and jump over the boulder corridor floor into the pool of water, swimming right once underwater. This allows access to the boss key room without Iron boots.");
    mTrickOptions[RT_WATER_FW_CENTRAL_GS] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_WATER_TEMPLE, {Tricks::Tag::NOVICE}, false, "Water Temple Central Pillar GS with Farore\'s Wind", "If you set Farore's Wind inside the central pillar and then return to that warp point after raising the water to the highest level, you can obtain this Skulltula Token with Hookshot or Boomerang.");
    mTrickOptions[RT_WATER_IRONS_CENTRAL_GS] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_WATER_TEMPLE, {Tricks::Tag::NOVICE}, false, "Water Temple Central Pillar GS with Iron Boots", "After opening the middle water level door into the central pillar, the door will stay unbarred so long as you do not leave the room -- even if you were to raise the water up to the highest level. With the Iron Boots to go through the door after the water has been raised, you can obtain the Skulltula Token with the Hookshot.");
    mTrickOptions[RT_WATER_CENTRAL_BOW] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_WATER_TEMPLE, {Tricks::Tag::ADVANCED}, false, "Water Temple Central Bow Target without Longshot or Hover Boots", "A very precise Bow shot can hit the eye switch from the floor above. Then, you can jump down into the hallway and make through it before the gate closes. It can also be done as child, using the Slingshot instead of the Bow.");
    mTrickOptions[RT_WATER_HOOKSHOT_FALLING_PLATFORM_GS] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_WATER_TEMPLE, {Tricks::Tag::NOVICE}, false, "Water Temple Falling Platform Room GS with Hookshot", "If you stand on the very edge of the platform, this Gold Skulltula can be obtained with only the Hookshot.");
    mTrickOptions[RT_WATER_RANG_FALLING_PLATFORM_GS] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_WATER_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Water Temple Falling Platform Room GS with Boomerang", "If you stand on the very edge of the platform, this Gold Skulltula can be obtained with only the Boomerang.");
    mTrickOptions[RT_WATER_RIVER_GS] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_WATER_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Water Temple River GS without Iron Boots", "Standing on the exposed ground toward the end of the river, a precise Longshot use can obtain the token. The Longshot cannot normally reach far enough to kill the Skulltula, however. You'll first have to find some other way of killing it.");
    mTrickOptions[RT_WATER_DRAGON_JUMP_DIVE] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_WATER_TEMPLE, {Tricks::Tag::NOVICE}, false, "Water Temple Dragon Statue Jump Dive", "If you come into the dragon statue room from the serpent river, you can jump down from above and get into the tunnel without needing either Iron Boots or a Scale. This trick applies to both Vanilla and Master Quest. In Vanilla, you must shoot the switch from above with the Bow, and then quickly get through the tunnel before the gate closes.");
    mTrickOptions[RT_WATER_ADULT_DRAGON] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_WATER_TEMPLE, {Tricks::Tag::NOVICE}, false, "Water Temple Dragon Statue Switch from Above the Water as Adult", "Normally you need both Hookshot and Iron Boots to hit the switch and swim through the tunnel to get to the chest. But by hitting the switch from dry land, using one of Bombchus, Hookshot, or Bow, it is possible to skip one or both of those requirements. After the gate has been opened, besides just using the Iron Boots, a well-timed dive with at least the Silver Scale could be used to swim through the tunnel. If coming from the serpent river, a jump dive can also be used to get into the tunnel.");
    mTrickOptions[RT_WATER_CHILD_DRAGON] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_WATER_TEMPLE, {Tricks::Tag::ADVANCED}, false, "Water Temple Dragon Statue Switch from Above the Water as Child", "It is possible for child to hit the switch from dry land using one of Bombchus, Slingshot or Boomerang. Then, to get to the chest, child can dive through the tunnel using at least the Silver Scale. The timing and positioning of this dive needs to be perfect to actually make it under the gate, and it all needs to be done very quickly to be able to get through before the gate closes. Be sure to enable \"Water Temple Dragon Statue Switch from Above the Water as Adult\" for adult's variant of this trick.");
    mTrickOptions[RT_WATER_MQ_CENTRAL_PILLAR] = TrickOption::LogicTrick(RCQUEST_MQ, RA_WATER_TEMPLE, {Tricks::Tag::NOVICE}, false, "Water Temple MQ Central Pillar with Fire Arrows", "Slanted torches have misleading hitboxes. Whenever you see a slanted torch jutting out of the wall, you can expect most or all of its hitbox is actually on the other side that wall. This can make slanted torches very finicky to light when using arrows. The torches in the central pillar of MQ Water Temple are a particularly egregious example. Logic normally expects Din's Fire and Song of Time.");
    mTrickOptions[RT_WATER_MQ_LOCKED_GS] = TrickOption::LogicTrick(RCQUEST_MQ, RA_WATER_TEMPLE, {Tricks::Tag::NOVICE}, false, "Water Temple MQ North Basement GS without Small Key", "There is an invisible Hookshot target that can be used to get over the gate that blocks you from going to this Skulltula early, skipping a small key as well as needing Hovers or Scarecrow to reach the locked door.");
    mTrickOptions[RT_LENS_SHADOW] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_SHADOW_TEMPLE, {Tricks::Tag::NOVICE}, false, "Shadow Temple Stationary Objects without Lens of Truth", "Removes the requirements for the Lens of Truth in Shadow Temple for most areas in the dungeon except for crossing the moving platform in the huge pit room and for fighting Bongo Bongo.");
    mTrickOptions[RT_LENS_SHADOW_PLATFORM] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_SHADOW_TEMPLE, {Tricks::Tag::NOVICE}, false, "Shadow Temple Invisible Moving Platform without Lens of Truth", "Removes the requirements for the Lens of Truth in Shadow Temple to cross the invisible moving platform in the huge pit room in either direction.");
    mTrickOptions[RT_LENS_BONGO] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_SHADOW_TEMPLE, {Tricks::Tag::NOVICE}, false, "Shadow Temple Bongo Bongo without Lens of Truth", "Bongo Bongo can be defeated without the use of Lens of Truth, as the hands give a pretty good idea of where the eye is.");
    mTrickOptions[RT_SHADOW_UMBRELLA] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_SHADOW_TEMPLE, {Tricks::Tag::EXPERT}, false, "Shadow Temple Stone Umbrella Skip", "A very precise Hover Boots movement from off of the lower chest can get you on top of the crushing spikes without needing to pull the block. Applies to both Vanilla and Master Quest.");
    mTrickOptions[RT_SHADOW_UMBRELLA_GS] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_SHADOW_TEMPLE, {Tricks::Tag::EXPERT}, false, "Shadow Temple Falling Spikes GS with Hover Boots", "After killing the Skulltula, a very precise Hover Boots movement from off of the lower chest can get you on top of the crushing spikes without needing to pull the block. From there, another very precise Hover Boots movement can be used to obtain the token without needing the Hookshot. Applies to both Vanilla and Master Quest. For obtaining the chests in this room with just Hover Boots, be sure to enable \"Shadow Temple Stone Umbrella Skip\".");
    mTrickOptions[RT_SHADOW_FREESTANDING_KEY] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_SHADOW_TEMPLE, {Tricks::Tag::NOVICE}, false, "Shadow Temple Freestanding Key with Bombchu", "Release the Bombchu with good timing so that it explodes near the bottom of the pot.");
    mTrickOptions[RT_SHADOW_STATUE] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_SHADOW_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Shadow Temple River Statue with Bombchu", "By sending a Bombchu around the edge of the gorge, you can knock down the statue without needing a Bow. Applies in both vanilla and MQ Shadow.");
    mTrickOptions[RT_SHADOW_BONGO] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_SHADOW_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Shadow Temple Bongo Bongo without projectiles", "Using precise sword slashes, Bongo Bongo can be defeated without using projectiles. This is only relevant in conjunction with Shadow Temple dungeon shortcuts or shuffled boss entrances.");
    mTrickOptions[RT_LENS_SHADOW_MQ] = TrickOption::LogicTrick(RCQUEST_MQ, RA_SHADOW_TEMPLE, {Tricks::Tag::NOVICE}, false, "Shadow Temple MQ Stationary Objects without Lens of Truth", "Removes the requirements for the Lens of Truth in Shadow Temple MQ for most areas in the dungeon. See \"Shadow Temple MQ Invisible Moving Platform without Lens of Truth\", \"Shadow Temple MQ Invisible Blades Silver Rupees without Lens of Truth\", \"Shadow Temple MQ 2nd Dead Hand without Lens of Truth\", and \"Shadow Temple Bongo Bongo without Lens of Truth\" for exceptions.");
    mTrickOptions[RT_LENS_SHADOW_MQ_INVISIBLE_BLADES] = TrickOption::LogicTrick(RCQUEST_MQ, RA_SHADOW_TEMPLE, {Tricks::Tag::NOVICE}, false, "Shadow Temple MQ Invisible Blades Silver Rupees without Lens of Truth", "Removes the requirement for the Lens of Truth or Nayru's Love in Shadow Temple MQ for the Invisible Blades room silver rupee collection.");
    mTrickOptions[RT_LENS_SHADOW_MQ_PLATFORM] = TrickOption::LogicTrick(RCQUEST_MQ, RA_SHADOW_TEMPLE, {Tricks::Tag::NOVICE}, false, "Shadow Temple MQ Invisible Moving Platform without Lens of Truth", "Removes the requirements for the Lens of Truth in Shadow Temple MQ to cross the invisible moving platform in the huge pit room in either direction.");
    mTrickOptions[RT_LENS_SHADOW_MQ_DEADHAND] = TrickOption::LogicTrick(RCQUEST_MQ, RA_SHADOW_TEMPLE, {Tricks::Tag::NOVICE}, false, "Shadow Temple MQ 2nd Dead Hand without Lens of Truth", "Dead Hand spawns in a random spot within the room. Having Lens removes the hassle of having to comb the room looking for his spawn location.");
    mTrickOptions[RT_SHADOW_MQ_GAP] = TrickOption::LogicTrick(RCQUEST_MQ, RA_SHADOW_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Shadow Temple MQ Truth Spinner Gap with Longshot", "You can Longshot a torch and jump-slash recoil onto the tongue. It works best if you Longshot the right torch from the left side of the room.");
    mTrickOptions[RT_SHADOW_MQ_INVISIBLE_BLADES] = TrickOption::LogicTrick(RCQUEST_MQ, RA_SHADOW_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Shadow Temple MQ Invisible Blades without Song of Time", "The Like Like can be used to boost you into the silver rupee or recovery hearts that normally require Song of Time. This cannot be performed on OHKO since the Like Like does not boost you high enough if you die.");
    mTrickOptions[RT_SHADOW_MQ_HUGE_PIT] = TrickOption::LogicTrick(RCQUEST_MQ, RA_SHADOW_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Shadow Temple MQ Lower Huge Pit without Fire Source", "Normally a frozen eye switch spawns some platforms that you can use to climb down, but there's actually a small piece of ground that you can stand on that you can just jump down to.");
    // mTrickOptions[RT_SHADOW_MQ_WINDY_WALKWAY] = TrickOption::LogicTrick(RCQUEST_MQ, RA_SHADOW_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Shadow Temple MQ Windy Walkway Reverse without Hover Boots", "With shadow dungeon shortcuts enabled, it is possible to jump from the alcove in the windy hallway to the middle platform. There are two methods: wait out the fan opposite the door and hold forward, or jump to the right to be pushed by the fan there towards the platform ledge. Note that jumps of this distance are inconsistent, but still possible.");
    mTrickOptions[RT_LENS_SPIRIT] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, {Tricks::Tag::NOVICE}, false, "Spirit Temple without Lens of Truth", "Removes the requirements for the Lens of Truth in Spirit Temple.");
    mTrickOptions[RT_SPIRIT_CHILD_CHU] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, {Tricks::Tag::NOVICE}, false, "Spirit Temple Child Side Bridge with Bombchu", "A carefully-timed Bombchu can hit the switch.");
    mTrickOptions[RT_SPIRIT_LOBBY_GS] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, {Tricks::Tag::NOVICE}, false, "Spirit Temple Main Room GS with Boomerang", "Standing on the highest part of the arm of the statue, a precise Boomerang throw can kill and obtain this Gold Skulltula. You must throw the Boomerang slightly off to the side so that it curves into the Skulltula, as aiming directly at it will clank off of the wall in front.");
    mTrickOptions[RT_SPIRIT_LOWER_ADULT_SWITCH] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, {Tricks::Tag::ADVANCED}, false, "Spirit Temple Lower Adult Switch with Bombs", "A bomb can be used to hit the switch on the ceiling, but it must be thrown from a particular distance away and with precise timing.");
    mTrickOptions[RT_SPIRIT_LOBBY_JUMP] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_SPIRIT_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Spirit Temple Main Room Jump from Hands to Upper Ledges", "A precise jump to obtain the following as adult without needing one of Hover Boots, or Hookshot (in vanilla) or Song of Time (in MQ): - Spirit Temple Statue Room Northeast Chest - Spirit Temple GS Lobby - Spirit Temple MQ Central Chamber Top Left Pot (Left) - Spirit Temple MQ Central Chamber Top Left Pot (Right)");
    // mTrickOptions[RT_SPIRIT_PLATFORM_HOOKSHOT] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Spirit Temple Main Room Hookshot to Boss Platform", "Precise hookshot aiming at the platform chains can be used to reach the boss platform from the middle landings. Using a jump slash immediately after reaching a chain makes aiming more lenient. Relevant only when Spirit Temple boss shortcuts are on.");
    mTrickOptions[RT_SPIRIT_MAP_CHEST] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, {Tricks::Tag::NOVICE}, false, "Spirit Temple Map Chest with Bow", "To get a line of sight from the upper torch to the map chest torches, you must pull an Armos statue all the way up the stairs.");
    mTrickOptions[RT_SPIRIT_SUN_CHEST] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, {Tricks::Tag::ADVANCED}, false, "Spirit Temple Sun Block Room Chest with Bow", "Using the blocks in the room as platforms you can get lines of sight to all three torches. The timer on the torches is quite short so you must move quickly in order to light all three.");
    mTrickOptions[RT_SPIRIT_WALL] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Spirit Temple Shifting Wall with No Additional Items", "Logic normally guarantees a way of dealing with both the Beamos and the Walltula before climbing the wall.");
    mTrickOptions[RT_LENS_SPIRIT_MQ] = TrickOption::LogicTrick(RCQUEST_MQ, RA_SPIRIT_TEMPLE, {Tricks::Tag::NOVICE}, false, "Spirit Temple MQ without Lens of Truth", "Removes the requirements for the Lens of Truth in Spirit Temple MQ.");
    mTrickOptions[RT_SPIRIT_MQ_SUN_BLOCK_SOT] = TrickOption::LogicTrick(RCQUEST_MQ, RA_SPIRIT_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Spirit Temple MQ Sun Block Room as Child without Song of Time", "While adult can easily jump directly to the switch that unbars the door to the sun block room, child Link cannot make the jump without spawning a Song of Time block to jump from. You can skip this by throwing the crate down onto the switch from above, which does unbar the door, however the crate immediately breaks, so you must move quickly to get through the door before it closes back up.");
    mTrickOptions[RT_SPIRIT_MQ_SUN_BLOCK_GS] = TrickOption::LogicTrick(RCQUEST_MQ, RA_SPIRIT_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Spirit Temple MQ Sun Block Room GS with Boomerang", "Throw the Boomerang in such a way that it curves through the side of the glass block to hit the Gold Skulltula.");
    mTrickOptions[RT_SPIRIT_MQ_LOWER_ADULT] = TrickOption::LogicTrick(RCQUEST_MQ, RA_SPIRIT_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Spirit Temple MQ Lower Adult without Fire Arrows", "By standing in a precise position it is possible to light two of the torches with a single use of Din\'s Fire. This saves enough time to be able to light all three torches with only Din\'s.");
    mTrickOptions[RT_SPIRIT_MQ_FROZEN_EYE] = TrickOption::LogicTrick(RCQUEST_MQ, RA_SPIRIT_TEMPLE, {Tricks::Tag::NOVICE}, false, "Spirit Temple MQ Frozen Eye Switch without Fire", "You can melt the ice by shooting an arrow through a torch. The only way to find a line of sight for this shot is to first spawn a Song of Time block, and then stand on the very edge of it.");
    mTrickOptions[RT_ICE_BLOCK_GS] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Ice Cavern Block Room GS with Hover Boots", "The Hover Boots can be used to get in front of the Skulltula to kill it with a jump slash. Then, the Hover Boots can again be used to obtain the Token, all without Hookshot or Boomerang.");
    mTrickOptions[RT_ICE_MQ_RED_ICE_GS] = TrickOption::LogicTrick(RCQUEST_MQ, RA_SPIRIT_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Ice Cavern MQ Red Ice GS without Song of Time", "If you side-hop into the perfect position, you can briefly stand on the platform with the red ice just long enough to dump some blue fire.");
    mTrickOptions[RT_ICE_MQ_SCARECROW] = TrickOption::LogicTrick(RCQUEST_MQ, RA_SPIRIT_TEMPLE, {Tricks::Tag::INTERMEDIATE}, false, "Ice Cavern MQ Scarecrow GS with No Additional Items", "As adult a precise jump can be used to reach this alcove.");
    mTrickOptions[RT_LENS_GTG] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_GERUDO_TRAINING_GROUND, {Tricks::Tag::NOVICE}, false, "Gerudo Training Ground without Lens of Truth", "Removes the requirements for the Lens of Truth in Gerudo Training Ground.");
    mTrickOptions[RT_GTG_WITHOUT_HOOKSHOT] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_GERUDO_TRAINING_GROUND, {Tricks::Tag::INTERMEDIATE}, false, "Gerudo Training Ground Left Side Silver Rupees without Hookshot", "After collecting the rest of the silver rupees in the room, you can reach the final silver rupee on the ceiling by being pulled up into it after getting grabbed by the Wallmaster. Then, you must also reach the exit of the room without the use of the Hookshot. If you move quickly you can sneak past the edge of a flame wall before it can rise up to block you. To do so without taking damage is more precise.");
    mTrickOptions[RT_GTG_FAKE_WALL] = TrickOption::LogicTrick(RCQUEST_BOTH, RA_GERUDO_TRAINING_GROUND, {Tricks::Tag::NOVICE}, false, "Reach Gerudo Training Ground Fake Wall Ledge with Hover Boots", "A precise Hover Boots use from the top of the chest can allow you to grab the ledge without needing the usual requirements. In Master Quest, this always skips a Song of Time requirement. In Vanilla, this skips a Hookshot requirement, but is only relevant if \"Gerudo Training Ground Left Side Silver Rupees without Hookshot\" is enabled.");
    mTrickOptions[RT_LENS_GTG_MQ] = TrickOption::LogicTrick(RCQUEST_MQ, RA_GERUDO_TRAINING_GROUND, {Tricks::Tag::NOVICE}, false, "Gerudo Training Ground MQ without Lens of Truth", "Removes the requirements for the Lens of Truth in Gerudo Training Ground MQ.");
    mTrickOptions[RT_GTG_MQ_WITH_HOOKSHOT] = TrickOption::LogicTrick(RCQUEST_MQ, RA_GERUDO_TRAINING_GROUND, {Tricks::Tag::NOVICE}, false, "Gerudo Training Ground MQ Left Side Silver Rupees with Hookshot", "The highest silver rupee can be obtained by hookshotting the target and then immediately jump slashing toward the rupee.");
    mTrickOptions[RT_GTG_MQ_WIHTOUT_HOOKSHOT] = TrickOption::LogicTrick(RCQUEST_MQ, RA_GERUDO_TRAINING_GROUND, {Tricks::Tag::INTERMEDIATE}, false, "Gerudo Training Ground MQ Left Side Silver Rupees without Hookshot", "After collecting the rest of the silver rupees in the room, you can reach the final silver rupee on the ceiling by being pulled up into it after getting grabbed by the Wallmaster. The Wallmaster will not track you to directly underneath the rupee. You should take the last step to be under the rupee after the Wallmaster has begun its attempt to grab you. Also included with this trick is that fact that the switch that unbars the door to the final chest of GTG can be hit without a projectile, using a precise jump slash. This trick supersedes \"Gerudo Training Ground MQ Left Side Silver Rupees with Hookshot\".");
    mTrickOptions[RT_LENS_GANON] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_GANONS_CASTLE, {Tricks::Tag::NOVICE}, false, "Ganon\'s Castle without Lens of Truth", "Removes the requirements for the Lens of Truth in Ganon's Castle.");
    mTrickOptions[RT_GANON_SPIRIT_TRIAL_HOOKSHOT] = TrickOption::LogicTrick(RCQUEST_VANILLA, RA_GANONS_CASTLE, {Tricks::Tag::NOVICE}, false, "Spirit Trial without Hookshot", "The highest rupee can be obtained as either age by performing a precise jump and a well-timed jumpslash off of an Armos.");
    mTrickOptions[RT_LENS_GANON_MQ] = TrickOption::LogicTrick(RCQUEST_MQ, RA_GANONS_CASTLE, {Tricks::Tag::NOVICE}, false, "Ganon\'s Castle MQ without Lens of Truth", "Removes the requirements for the Lens of Truth in Ganon's Castle MQ.");
    mTrickOptions[RT_GANON_MQ_FIRE_TRIAL] = TrickOption::LogicTrick(RCQUEST_MQ, RA_GANONS_CASTLE, {Tricks::Tag::ADVANCED}, false, "Fire Trial MQ with Hookshot", "It's possible to hook the target at the end of fire trial with just Hookshot, but it requires precise aim and perfect positioning. The main difficulty comes from getting on the very corner of the obelisk without falling into the lava.");
    mTrickOptions[RT_GANON_MQ_SHADOW_TRIAL] = TrickOption::LogicTrick(RCQUEST_MQ, RA_GANONS_CASTLE, {Tricks::Tag::NOVICE}, false, "Shadow Trial MQ Torch with Bow", "You can light the torch in this room without a fire source by shooting an arrow through the lit torch at the beginning of the room. Because the room is so dark and the unlit torch is so far away, it can be difficult to aim the shot correctly.");
    mTrickOptions[RT_GANON_MQ_LIGHT_TRIAL] = TrickOption::LogicTrick(RCQUEST_MQ, RA_GANONS_CASTLE, {Tricks::Tag::INTERMEDIATE}, false, "Light Trial MQ without Hookshot", "If you move quickly you can sneak past the edge of a flame wall before it can rise up to block you. In this case to do it without taking damage is especially precise.");

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
        &mTrickOptions[RT_DEKU_B1_BACKFLIP_OVER_SPIKED_LOG],
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
    for (int i = 0; i < RT_MAX; i++) {
        auto& trick = mTrickOptions[i];
        if (!trick.GetName().empty()) {
            mTrickNameToEnum[std::string(trick.GetName())] = static_cast<RandomizerTrick>(i);
        }
        mTricksByArea[trick.GetArea()].push_back(static_cast<RandomizerTrick>(i));
    }
    // TODO: Glitches
    mOptionGroups[RSG_AREA_ACCESS_IMGUI] = OptionGroup::SubGroup("Area Access", {
        &mOptions[RSK_FOREST],
        &mOptions[RSK_KAK_GATE],
        &mOptions[RSK_DOOR_OF_TIME],
        &mOptions[RSK_ZORAS_FOUNTAIN],
    }, false, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_WORLD_IMGUI] = OptionGroup::SubGroup("World Settings", {
        &mOptions[RSK_STARTING_AGE],
        &mOptions[RSK_GERUDO_FORTRESS],
        &mOptions[RSK_RAINBOW_BRIDGE],
        &mOptions[RSK_BRIDGE_OPTIONS],
        &mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT],
        &mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT],
        &mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT],
        &mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT],
        &mOptions[RSK_RAINBOW_BRIDGE_TOKEN_COUNT],
        &mOptions[RSK_GANONS_TRIALS],
        &mOptions[RSK_TRIAL_COUNT],
        &mOptions[RSK_MQ_DUNGEON_RANDOM],
        &mOptions[RSK_MQ_DUNGEON_COUNT],
        &mOptions[RSK_MQ_DUNGEON_SET],
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
        &mOptions[RSK_MQ_GANONS_CASTLE],
        &mOptions[RSK_TRIFORCE_HUNT],
        &mOptions[RSK_TRIFORCE_HUNT_PIECES_TOTAL],
        &mOptions[RSK_TRIFORCE_HUNT_PIECES_REQUIRED]
    }, false, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_SHUFFLE_ENTRANCES_IMGUI] = OptionGroup::SubGroup("Shuffle Entrances", {
        &mOptions[RSK_SHUFFLE_DUNGEON_ENTRANCES],
        &mOptions[RSK_SHUFFLE_BOSS_ENTRANCES],
        &mOptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES],
        &mOptions[RSK_SHUFFLE_INTERIOR_ENTRANCES],
        &mOptions[RSK_SHUFFLE_GROTTO_ENTRANCES],
        &mOptions[RSK_SHUFFLE_OWL_DROPS],
        &mOptions[RSK_SHUFFLE_WARP_SONGS],
        &mOptions[RSK_SHUFFLE_OVERWORLD_SPAWNS],
        &mOptions[RSK_DECOUPLED_ENTRANCES],
        &mOptions[RSK_MIXED_ENTRANCE_POOLS],
        &mOptions[RSK_MIX_DUNGEON_ENTRANCES],
        &mOptions[RSK_MIX_BOSS_ENTRANCES],
        &mOptions[RSK_MIX_OVERWORLD_ENTRANCES],
        &mOptions[RSK_MIX_INTERIOR_ENTRANCES],
        &mOptions[RSK_MIX_GROTTO_ENTRANCES]
    }, false, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_WORLD_IMGUI_TABLE] = OptionGroup::SubGroup("World", {
        &mOptionGroups[RSG_AREA_ACCESS_IMGUI],
        &mOptionGroups[RSG_WORLD_IMGUI],
        &mOptionGroups[RSG_SHUFFLE_ENTRANCES_IMGUI],
    }, false, WidgetContainerType::TABLE);
    mOptionGroups[RSG_SHUFFLE_ITEMS_IMGUI] = OptionGroup::SubGroup("Shuffle Items", {
        &mOptions[RSK_SHUFFLE_SONGS],
        &mOptions[RSK_SHUFFLE_TOKENS],
        &mOptions[RSK_SKULLS_SUNS_SONG],
        &mOptions[RSK_SHUFFLE_KOKIRI_SWORD],
        &mOptions[RSK_SHUFFLE_MASTER_SWORD],
        &mOptions[RSK_SHUFFLE_CHILD_WALLET],
        &mOptions[RSK_INCLUDE_TYCOON_WALLET],
        &mOptions[RSK_SHUFFLE_OCARINA],
        &mOptions[RSK_SHUFFLE_OCARINA_BUTTONS],
        &mOptions[RSK_SHUFFLE_SWIM],
        &mOptions[RSK_SHUFFLE_WEIRD_EGG],
        &mOptions[RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD],
        &mOptions[RSK_SHUFFLE_FISHING_POLE],
        &mOptions[RSK_SHUFFLE_DEKU_STICK_BAG],
        &mOptions[RSK_SHUFFLE_DEKU_NUT_BAG],
    }, false, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_SHUFFLE_NPCS_IMGUI] = OptionGroup::SubGroup("Shuffle NPCs & Merchants", {
        &mOptions[RSK_SHOPSANITY],
        &mOptions[RSK_SHOPSANITY_COUNT],
        &mOptions[RSK_SHOPSANITY_PRICES],
        &mOptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE],
        &mOptions[RSK_SHOPSANITY_PRICES_RANGE_1],
        &mOptions[RSK_SHOPSANITY_PRICES_RANGE_2],
        &mOptions[RSK_SHOPSANITY_PRICES_NO_WALLET_WEIGHT],
        &mOptions[RSK_SHOPSANITY_PRICES_CHILD_WALLET_WEIGHT],
        &mOptions[RSK_SHOPSANITY_PRICES_ADULT_WALLET_WEIGHT],
        &mOptions[RSK_SHOPSANITY_PRICES_GIANT_WALLET_WEIGHT],
        &mOptions[RSK_SHOPSANITY_PRICES_TYCOON_WALLET_WEIGHT],
        &mOptions[RSK_SHOPSANITY_PRICES_AFFORDABLE],
        &mOptions[RSK_FISHSANITY],
        &mOptions[RSK_FISHSANITY_POND_COUNT],
        &mOptions[RSK_FISHSANITY_AGE_SPLIT],
        &mOptions[RSK_SHUFFLE_SCRUBS],
        &mOptions[RSK_SCRUBS_PRICES],
        &mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE],
        &mOptions[RSK_SCRUBS_PRICES_RANGE_1],
        &mOptions[RSK_SCRUBS_PRICES_RANGE_2],
        &mOptions[RSK_SCRUBS_PRICES_NO_WALLET_WEIGHT],
        &mOptions[RSK_SCRUBS_PRICES_CHILD_WALLET_WEIGHT],
        &mOptions[RSK_SCRUBS_PRICES_ADULT_WALLET_WEIGHT],
        &mOptions[RSK_SCRUBS_PRICES_GIANT_WALLET_WEIGHT],
        &mOptions[RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT],
        &mOptions[RSK_SCRUBS_PRICES_AFFORDABLE],
        &mOptions[RSK_SHUFFLE_BEEHIVES],
        &mOptions[RSK_SHUFFLE_COWS],
        &mOptions[RSK_SHUFFLE_MERCHANTS],
        &mOptions[RSK_MERCHANT_PRICES],
        &mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE],
        &mOptions[RSK_MERCHANT_PRICES_RANGE_1],
        &mOptions[RSK_MERCHANT_PRICES_RANGE_2],
        &mOptions[RSK_MERCHANT_PRICES_NO_WALLET_WEIGHT],
        &mOptions[RSK_MERCHANT_PRICES_CHILD_WALLET_WEIGHT],
        &mOptions[RSK_MERCHANT_PRICES_ADULT_WALLET_WEIGHT],
        &mOptions[RSK_MERCHANT_PRICES_GIANT_WALLET_WEIGHT],
        &mOptions[RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT],
        &mOptions[RSK_MERCHANT_PRICES_AFFORDABLE],
        &mOptions[RSK_SHUFFLE_FROG_SONG_RUPEES],
        &mOptions[RSK_SHUFFLE_ADULT_TRADE],
        &mOptions[RSK_SHUFFLE_100_GS_REWARD],
        &mOptions[RSK_SHUFFLE_BOSS_SOULS],
    }, false, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_SHUFFLE_DUNGEON_ITEMS_IMGUI] = OptionGroup::SubGroup("Shuffle Dungeon Items", {
        &mOptions[RSK_SHUFFLE_DUNGEON_REWARDS],
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
    }, false, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_ITEMS_IMGUI_TABLE] = OptionGroup::SubGroup("Items", {
        &mOptionGroups[RSG_SHUFFLE_ITEMS_IMGUI],
        &mOptionGroups[RSG_SHUFFLE_NPCS_IMGUI],
        &mOptionGroups[RSG_SHUFFLE_DUNGEON_ITEMS_IMGUI],
    }, false, WidgetContainerType::TABLE);
    mOptionGroups[RSG_TIMESAVERS_IMGUI] = OptionGroup::SubGroup("Timesavers", {
        &mOptions[RSK_CUCCO_COUNT],
        &mOptions[RSK_BIG_POE_COUNT],
        &mOptions[RSK_SKIP_CHILD_STEALTH],
        &mOptions[RSK_SKIP_CHILD_ZELDA],
        &mOptions[RSK_SKIP_EPONA_RACE],
        &mOptions[RSK_COMPLETE_MASK_QUEST],
        &mOptions[RSK_SKIP_SCARECROWS_SONG]
    }, false, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_ITEM_POOL_HINTS_IMGUI] = OptionGroup::SubGroup("", {
        &mOptions[RSK_ITEM_POOL],
        &mOptions[RSK_ICE_TRAPS],
        &mOptions[RSK_GOSSIP_STONE_HINTS],
        &mOptions[RSK_HINT_CLARITY],
        &mOptions[RSK_HINT_DISTRIBUTION],
    }, false, WidgetContainerType::SECTION);
    mOptionGroups[RSG_EXTRA_HINTS_IMGUI] = OptionGroup::SubGroup("Extra Hints", {
        &mOptions[RSK_TOT_ALTAR_HINT],
        &mOptions[RSK_GANONDORF_HINT],
        &mOptions[RSK_SHEIK_LA_HINT],
        &mOptions[RSK_DAMPES_DIARY_HINT],
        &mOptions[RSK_GREG_HINT],
        &mOptions[RSK_LOACH_HINT],
        &mOptions[RSK_SARIA_HINT],
        &mOptions[RSK_FROGS_HINT],
        &mOptions[RSK_OOT_HINT],
        &mOptions[RSK_BIGGORON_HINT],
        &mOptions[RSK_BIG_POES_HINT],
        &mOptions[RSK_CHICKENS_HINT],
        &mOptions[RSK_MALON_HINT],
        &mOptions[RSK_HBA_HINT],
        &mOptions[RSK_FISHING_POLE_HINT],
        &mOptions[RSK_WARP_SONG_HINTS],
        &mOptions[RSK_SCRUB_TEXT_HINT],
        &mOptions[RSK_MERCHANT_TEXT_HINT],
        &mOptions[RSK_KAK_10_SKULLS_HINT],
        &mOptions[RSK_KAK_20_SKULLS_HINT],
        &mOptions[RSK_KAK_30_SKULLS_HINT],
        &mOptions[RSK_KAK_40_SKULLS_HINT],
        &mOptions[RSK_KAK_50_SKULLS_HINT],
        &mOptions[RSK_KAK_100_SKULLS_HINT],
        &mOptions[RSK_MASK_SHOP_HINT]
    }, false, WidgetContainerType::SECTION, "This setting adds some hints at locations other than Gossip Stones.");
    mOptionGroups[RSG_ITEM_POOL_HINTS_IMGUI_COLUMN] = OptionGroup::SubGroup("Item Pool & Hints", std::initializer_list<OptionGroup*>{
        &mOptionGroups[RSG_ITEM_POOL_HINTS_IMGUI],
        &mOptionGroups[RSG_EXTRA_HINTS_IMGUI],
    }, false, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_ADDITIONAL_FEATURES_IMGUI] = OptionGroup::SubGroup("Additional Features", {
        &mOptions[RSK_FULL_WALLETS],
        &mOptions[RSK_BOMBCHUS_IN_LOGIC],
        &mOptions[RSK_ENABLE_BOMBCHU_DROPS],
        &mOptions[RSK_BLUE_FIRE_ARROWS],
        &mOptions[RSK_SUNLIGHT_ARROWS],
        &mOptions[RSK_INFINITE_UPGRADES],
        &mOptions[RSK_SKELETON_KEY],
    }, false, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_GAMEPLAY_IMGUI_TABLE] = OptionGroup::SubGroup("Gameplay", {
        &mOptionGroups[RSG_TIMESAVERS_IMGUI],
        &mOptionGroups[RSG_ITEM_POOL_HINTS_IMGUI_COLUMN],
        &mOptionGroups[RSG_ADDITIONAL_FEATURES_IMGUI]
    }, false, WidgetContainerType::TABLE);
    mOptionGroups[RSG_STARTING_EQUIPMENT_IMGUI] = OptionGroup::SubGroup("Starting Equipment", {
        &mOptions[RSK_LINKS_POCKET],
        &mOptions[RSK_STARTING_KOKIRI_SWORD],
        &mOptions[RSK_STARTING_MASTER_SWORD],
        &mOptions[RSK_STARTING_DEKU_SHIELD]
    }, false, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_STARTING_ITEMS_IMGUI] = OptionGroup::SubGroup("Starting Items", {
        &mOptions[RSK_STARTING_OCARINA],
        &mOptions[RSK_STARTING_CONSUMABLES],
        &mOptions[RSK_STARTING_SKULLTULA_TOKEN],
        &mOptions[RSK_STARTING_HEARTS],
    }, false, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_STARTING_NORMAL_SONGS_IMGUI] = OptionGroup::SubGroup("Normal Songs", {
        &mOptions[RSK_STARTING_ZELDAS_LULLABY],
        &mOptions[RSK_STARTING_EPONAS_SONG],
        &mOptions[RSK_STARTING_SARIAS_SONG],
        &mOptions[RSK_STARTING_SUNS_SONG],
        &mOptions[RSK_STARTING_SONG_OF_TIME],
        &mOptions[RSK_STARTING_SONG_OF_STORMS],
    }, false, WidgetContainerType::SECTION);
    mOptionGroups[RSG_STARTING_WARP_SONGS_IMGUI] = OptionGroup::SubGroup("Warp Songs", {
        &mOptions[RSK_STARTING_MINUET_OF_FOREST],
        &mOptions[RSK_STARTING_BOLERO_OF_FIRE],
        &mOptions[RSK_STARTING_SERENADE_OF_WATER],
        &mOptions[RSK_STARTING_REQUIEM_OF_SPIRIT],
        &mOptions[RSK_STARTING_NOCTURNE_OF_SHADOW],
        &mOptions[RSK_STARTING_PRELUDE_OF_LIGHT]
    }, false, WidgetContainerType::SECTION);
    mOptionGroups[RSG_STARTING_SONGS_IMGUI] = OptionGroup::SubGroup("Starting Songs", std::initializer_list<OptionGroup*>({
        &mOptionGroups[RSG_STARTING_NORMAL_SONGS_IMGUI],
        &mOptionGroups[RSG_STARTING_WARP_SONGS_IMGUI],
    }), false, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_STARTING_INVENTORY_IMGUI_TABLE] = OptionGroup::SubGroup("Starting Inventory", {
        &mOptionGroups[RSG_STARTING_EQUIPMENT_IMGUI],
        &mOptionGroups[RSG_STARTING_ITEMS_IMGUI],
        &mOptionGroups[RSG_STARTING_SONGS_IMGUI]
    }, false, WidgetContainerType::TABLE);
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
        &mOptions[RSK_MIX_BOSS_ENTRANCES],
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
        &mOptions[RSK_SHOPSANITY_COUNT],
        &mOptions[RSK_SHOPSANITY_PRICES],
        &mOptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE],
        &mOptions[RSK_SHOPSANITY_PRICES_RANGE_1],
        &mOptions[RSK_SHOPSANITY_PRICES_RANGE_2],
        &mOptions[RSK_SHOPSANITY_PRICES_NO_WALLET_WEIGHT],
        &mOptions[RSK_SHOPSANITY_PRICES_CHILD_WALLET_WEIGHT],
        &mOptions[RSK_SHOPSANITY_PRICES_ADULT_WALLET_WEIGHT],
        &mOptions[RSK_SHOPSANITY_PRICES_GIANT_WALLET_WEIGHT],
        &mOptions[RSK_SHOPSANITY_PRICES_TYCOON_WALLET_WEIGHT],
        &mOptions[RSK_SHOPSANITY_PRICES_AFFORDABLE],
        &mOptions[RSK_FISHSANITY],
        &mOptions[RSK_FISHSANITY_POND_COUNT],
        &mOptions[RSK_FISHSANITY_AGE_SPLIT],
        &mOptions[RSK_SHUFFLE_FISHING_POLE],
        &mOptions[RSK_SHUFFLE_TOKENS],
        &mOptions[RSK_SHUFFLE_SCRUBS],
        &mOptions[RSK_SCRUBS_PRICES],
        &mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE],
        &mOptions[RSK_SCRUBS_PRICES_RANGE_1],
        &mOptions[RSK_SCRUBS_PRICES_RANGE_2],
        &mOptions[RSK_SCRUBS_PRICES_NO_WALLET_WEIGHT],
        &mOptions[RSK_SCRUBS_PRICES_CHILD_WALLET_WEIGHT],
        &mOptions[RSK_SCRUBS_PRICES_ADULT_WALLET_WEIGHT],
        &mOptions[RSK_SCRUBS_PRICES_GIANT_WALLET_WEIGHT],
        &mOptions[RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT],
        &mOptions[RSK_SCRUBS_PRICES_AFFORDABLE],
        &mOptions[RSK_SHUFFLE_BEEHIVES],
        &mOptions[RSK_SHUFFLE_COWS],
        &mOptions[RSK_SHUFFLE_KOKIRI_SWORD],
        &mOptions[RSK_SHUFFLE_OCARINA],
        &mOptions[RSK_SHUFFLE_OCARINA_BUTTONS],
        &mOptions[RSK_SHUFFLE_SWIM],
        &mOptions[RSK_SHUFFLE_WEIRD_EGG],
        &mOptions[RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD],
        &mOptions[RSK_SHUFFLE_MERCHANTS],
        &mOptions[RSK_MERCHANT_PRICES],
        &mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE],
        &mOptions[RSK_MERCHANT_PRICES_RANGE_1],
        &mOptions[RSK_MERCHANT_PRICES_RANGE_2],
        &mOptions[RSK_MERCHANT_PRICES_NO_WALLET_WEIGHT],
        &mOptions[RSK_MERCHANT_PRICES_CHILD_WALLET_WEIGHT],
        &mOptions[RSK_MERCHANT_PRICES_ADULT_WALLET_WEIGHT],
        &mOptions[RSK_MERCHANT_PRICES_GIANT_WALLET_WEIGHT],
        &mOptions[RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT],
        &mOptions[RSK_MERCHANT_PRICES_AFFORDABLE],
        &mOptions[RSK_SHUFFLE_FROG_SONG_RUPEES],
        &mOptions[RSK_SHUFFLE_ADULT_TRADE],
        &mOptions[RSK_SHUFFLE_CHEST_MINIGAME],
        &mOptions[RSK_SHUFFLE_100_GS_REWARD],
        &mOptions[RSK_SHUFFLE_BOSS_SOULS],
        &mOptions[RSK_SHUFFLE_DEKU_STICK_BAG],
        &mOptions[RSK_SHUFFLE_DEKU_NUT_BAG],
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
        &mOptions[RSK_STARTING_HEARTS],
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
        &mOptions[RSK_GANONDORF_HINT],
        &mOptions[RSK_SHEIK_LA_HINT],
        &mOptions[RSK_DAMPES_DIARY_HINT],
        &mOptions[RSK_GREG_HINT],
        &mOptions[RSK_LOACH_HINT],
        &mOptions[RSK_SARIA_HINT],
        &mOptions[RSK_FROGS_HINT],
        &mOptions[RSK_OOT_HINT],
        &mOptions[RSK_WARP_SONG_HINTS],
        &mOptions[RSK_BIGGORON_HINT],
        &mOptions[RSK_BIG_POES_HINT],
        &mOptions[RSK_CHICKENS_HINT],
        &mOptions[RSK_MALON_HINT],
        &mOptions[RSK_HBA_HINT],
        &mOptions[RSK_KAK_10_SKULLS_HINT],
        &mOptions[RSK_KAK_20_SKULLS_HINT],
        &mOptions[RSK_KAK_30_SKULLS_HINT],
        &mOptions[RSK_KAK_40_SKULLS_HINT],
        &mOptions[RSK_KAK_50_SKULLS_HINT],
        &mOptions[RSK_KAK_100_SKULLS_HINT],
        &mOptions[RSK_MASK_SHOP_HINT],
        &mOptions[RSK_SCRUB_TEXT_HINT],
        &mOptions[RSK_MERCHANT_TEXT_HINT],
        &mOptions[RSK_FISHING_POLE_HINT],
        // TODO: Compasses show Reward/WOTH, Maps show Dungeon Mode, Starting Time
        &mOptions[RSK_DAMAGE_MULTIPLIER],
        &mOptions[RSK_BLUE_FIRE_ARROWS],
        &mOptions[RSK_SUNLIGHT_ARROWS],
        &mOptions[RSK_INFINITE_UPGRADES],
        &mOptions[RSK_SKELETON_KEY],
    });
    mOptionGroups[RSG_ITEM_POOL] = OptionGroup("Item Pool Settings", std::initializer_list<Option*>({
        &mOptions[RSK_ITEM_POOL],
        &mOptions[RSK_ICE_TRAPS]
    }));
    // TODO: Progressive Goron Sword, Remove Double Defense
    mOptionGroups[RSG_EXCLUDES_KOKIRI_FOREST] = OptionGroup::SubGroup("Kokiri Forest", mExcludeLocationsOptionsAreas[RCAREA_KOKIRI_FOREST], false);
    mOptionGroups[RSG_EXCLUDES_LOST_WOODS] = OptionGroup::SubGroup("Lost Woods", mExcludeLocationsOptionsAreas[RCAREA_LOST_WOODS], false);
    mOptionGroups[RSG_EXCLUDES_SACRED_FOREST_MEADOW] = OptionGroup::SubGroup("Sacred Forest Meadow", mExcludeLocationsOptionsAreas[RCAREA_SACRED_FOREST_MEADOW], false);
    mOptionGroups[RSG_EXCLUDES_DEKU_TREE] = OptionGroup::SubGroup("Deku Tree", mExcludeLocationsOptionsAreas[RCAREA_DEKU_TREE], false);
    mOptionGroups[RSG_EXCLUDES_FOREST_TEMPLE] = OptionGroup::SubGroup("Forest Temple", mExcludeLocationsOptionsAreas[RCAREA_FOREST_TEMPLE], false);
    mOptionGroups[RSG_EXCLUDES_KAKARIKO_VILLAGE] = OptionGroup::SubGroup("Kakariko Village", mExcludeLocationsOptionsAreas[RCAREA_KAKARIKO_VILLAGE], false);
    mOptionGroups[RSG_EXCLUDES_GRAVEYARD] = OptionGroup::SubGroup("Graveyard", mExcludeLocationsOptionsAreas[RCAREA_GRAVEYARD], false);
    mOptionGroups[RSG_EXCLUDES_BOTTOM_OF_THE_WELL] = OptionGroup::SubGroup("Bottom of the Well", mExcludeLocationsOptionsAreas[RCAREA_BOTTOM_OF_THE_WELL], false);
    mOptionGroups[RSG_EXCLUDES_SHADOW_TEMPLE] = OptionGroup::SubGroup("Shadow Temple", mExcludeLocationsOptionsAreas[RCAREA_SHADOW_TEMPLE], false);
    mOptionGroups[RSG_EXCLUDES_DEATH_MOUNTAIN_TRAIL] = OptionGroup::SubGroup("Death Mountain Trail", mExcludeLocationsOptionsAreas[RCAREA_DEATH_MOUNTAIN_TRAIL], false);
    mOptionGroups[RSG_EXCLUDES_DEATH_MOUNTAIN_CRATER] = OptionGroup::SubGroup("Death Mountain Crater", mExcludeLocationsOptionsAreas[RCAREA_DEATH_MOUNTAIN_CRATER], false);
    mOptionGroups[RSG_EXCLUDES_GORON_CITY] = OptionGroup::SubGroup("Goron City", mExcludeLocationsOptionsAreas[RCAREA_GORON_CITY], false);
    mOptionGroups[RSG_EXCLUDES_DODONGOS_CAVERN] = OptionGroup::SubGroup("Dodongo's Cavern", mExcludeLocationsOptionsAreas[RCAREA_DODONGOS_CAVERN], false);
    mOptionGroups[RSG_EXCLUDES_FIRE_TEMPLE] = OptionGroup::SubGroup("Fire Temple", mExcludeLocationsOptionsAreas[RCAREA_FIRE_TEMPLE], false);
    mOptionGroups[RSG_EXCLUDES_ZORAS_RIVER] = OptionGroup::SubGroup("Zora's River", mExcludeLocationsOptionsAreas[RCAREA_ZORAS_RIVER], false);
    mOptionGroups[RSG_EXCLUDES_ZORAS_DOMAIN] = OptionGroup::SubGroup("Zora's Domain", mExcludeLocationsOptionsAreas[RCAREA_ZORAS_DOMAIN], false);
    mOptionGroups[RSG_EXCLUDES_ZORAS_FOUNTAIN] = OptionGroup::SubGroup("Zora's Fountain", mExcludeLocationsOptionsAreas[RCAREA_ZORAS_FOUNTAIN], false);
    mOptionGroups[RSG_EXCLUDES_JABU_JABU] = OptionGroup::SubGroup("Jabu Jabu's Belly", mExcludeLocationsOptionsAreas[RCAREA_JABU_JABUS_BELLY], false);
    mOptionGroups[RSG_EXCLUDES_ICE_CAVERN] = OptionGroup::SubGroup("Ice Cavern", mExcludeLocationsOptionsAreas[RCAREA_ICE_CAVERN], false);
    mOptionGroups[RSG_EXCLUDES_HYRULE_FIELD] = OptionGroup::SubGroup("Hyrule Field", mExcludeLocationsOptionsAreas[RCAREA_HYRULE_FIELD], false);
    mOptionGroups[RSG_EXCLUDES_LON_LON_RANCH] = OptionGroup::SubGroup("Lon Lon Ranch", mExcludeLocationsOptionsAreas[RCAREA_LON_LON_RANCH], false);
    mOptionGroups[RSG_EXCLUDES_LAKE_HYLIA] = OptionGroup::SubGroup("Lake Hylia", mExcludeLocationsOptionsAreas[RCAREA_LAKE_HYLIA], false);
    mOptionGroups[RSG_EXCLUDES_WATER_TEMPLE] = OptionGroup::SubGroup("Water Temple", mExcludeLocationsOptionsAreas[RCAREA_WATER_TEMPLE], false);
    mOptionGroups[RSG_EXCLUDES_GERUDO_VALLEY] = OptionGroup::SubGroup("Gerudo Valley", mExcludeLocationsOptionsAreas[RCAREA_GERUDO_VALLEY], false);
    mOptionGroups[RSG_EXCLUDES_GERUDO_FORTRESS] = OptionGroup::SubGroup("Gerudo Fortress", mExcludeLocationsOptionsAreas[RCAREA_GERUDO_FORTRESS], false);
    mOptionGroups[RSG_EXCLUDES_HAUNTED_WASTELAND] = OptionGroup::SubGroup("Haunted Wasteland", mExcludeLocationsOptionsAreas[RCAREA_WASTELAND], false);
    mOptionGroups[RSG_EXCLUDES_DESERT_COLOSSUS] = OptionGroup::SubGroup("Desert Colossus", mExcludeLocationsOptionsAreas[RCAREA_DESERT_COLOSSUS], false);
    mOptionGroups[RSG_EXCLUDES_GERUDO_TRAINING_GROUNDS] = OptionGroup::SubGroup("Gerudo Training Grounds", mExcludeLocationsOptionsAreas[RCAREA_GERUDO_TRAINING_GROUND], false);
    mOptionGroups[RSG_EXCLUDES_SPIRIT_TEMPLE] = OptionGroup::SubGroup("Spirit Temple", mExcludeLocationsOptionsAreas[RCAREA_SPIRIT_TEMPLE], false);
    mOptionGroups[RSG_EXCLUDES_HYRULE_CASTLE] = OptionGroup::SubGroup("Hyrule Castle", mExcludeLocationsOptionsAreas[RCAREA_HYRULE_CASTLE], false);
    mOptionGroups[RSG_EXCLUDES_MARKET] = OptionGroup::SubGroup("Market", mExcludeLocationsOptionsAreas[RCAREA_MARKET], false);
    mOptionGroups[RSG_EXCLUDES_GANONS_CASTLE] = OptionGroup::SubGroup("Ganon's Castle", mExcludeLocationsOptionsAreas[RCAREA_GANONS_CASTLE], false);
    mOptionGroups[RSG_EXCLUDES] = OptionGroup::SubGroup("Exclude Locations", {
        &mOptionGroups[RSG_EXCLUDES_KOKIRI_FOREST],
        &mOptionGroups[RSG_EXCLUDES_LOST_WOODS],
        &mOptionGroups[RSG_EXCLUDES_SACRED_FOREST_MEADOW],
        &mOptionGroups[RSG_EXCLUDES_DEKU_TREE],
        &mOptionGroups[RSG_EXCLUDES_FOREST_TEMPLE],
        &mOptionGroups[RSG_EXCLUDES_KAKARIKO_VILLAGE],
        &mOptionGroups[RSG_EXCLUDES_GRAVEYARD],
        &mOptionGroups[RSG_EXCLUDES_BOTTOM_OF_THE_WELL],
        &mOptionGroups[RSG_EXCLUDES_SHADOW_TEMPLE],
        &mOptionGroups[RSG_EXCLUDES_DEATH_MOUNTAIN_TRAIL],
        &mOptionGroups[RSG_EXCLUDES_DEATH_MOUNTAIN_CRATER],
        &mOptionGroups[RSG_EXCLUDES_GORON_CITY],
        &mOptionGroups[RSG_EXCLUDES_DODONGOS_CAVERN],
        &mOptionGroups[RSG_EXCLUDES_FIRE_TEMPLE],
        &mOptionGroups[RSG_EXCLUDES_ZORAS_RIVER],
        &mOptionGroups[RSG_EXCLUDES_ZORAS_DOMAIN],
        &mOptionGroups[RSG_EXCLUDES_ZORAS_FOUNTAIN],
        &mOptionGroups[RSG_EXCLUDES_JABU_JABU],
        &mOptionGroups[RSG_EXCLUDES_ICE_CAVERN],
        &mOptionGroups[RSG_EXCLUDES_HYRULE_FIELD],
        &mOptionGroups[RSG_EXCLUDES_LON_LON_RANCH],
        &mOptionGroups[RSG_EXCLUDES_LAKE_HYLIA],
        &mOptionGroups[RSG_EXCLUDES_WATER_TEMPLE],
        &mOptionGroups[RSG_EXCLUDES_GERUDO_VALLEY],
        &mOptionGroups[RSG_EXCLUDES_GERUDO_FORTRESS],
        &mOptionGroups[RSG_EXCLUDES_HAUNTED_WASTELAND],
        &mOptionGroups[RSG_EXCLUDES_DESERT_COLOSSUS],
        &mOptionGroups[RSG_EXCLUDES_GERUDO_TRAINING_GROUNDS],
        &mOptionGroups[RSG_EXCLUDES_SPIRIT_TEMPLE],
        &mOptionGroups[RSG_EXCLUDES_HYRULE_CASTLE],
        &mOptionGroups[RSG_EXCLUDES_MARKET],
        &mOptionGroups[RSG_EXCLUDES_GANONS_CASTLE],
    }, false);
    mOptionGroups[RSG_DETAILED_LOGIC] = OptionGroup("Detailed Logic Settings", {
        &mOptionGroups[RSG_LOGIC],
        &mOptionGroups[RSG_TRICKS],
        &mOptionGroups[RSG_EXCLUDES]
    });

    VanillaLogicDefaults = {//RANDOTODO check what this does
        &mOptions[RSK_LINKS_POCKET],
        &mOptions[RSK_SHUFFLE_DUNGEON_REWARDS],
        &mOptions[RSK_SHUFFLE_SONGS],
        &mOptions[RSK_SHOPSANITY],
        &mOptions[RSK_SHOPSANITY_COUNT],
        &mOptions[RSK_SHOPSANITY_PRICES],
        &mOptions[RSK_SHOPSANITY_PRICES_AFFORDABLE],
        &mOptions[RSK_FISHSANITY],
        &mOptions[RSK_FISHSANITY_POND_COUNT],
        &mOptions[RSK_FISHSANITY_AGE_SPLIT],
        &mOptions[RSK_SHUFFLE_SCRUBS],
        &mOptions[RSK_SHUFFLE_BEEHIVES],
        &mOptions[RSK_SHUFFLE_COWS],
        &mOptions[RSK_SHUFFLE_MERCHANTS],
        &mOptions[RSK_SHUFFLE_FROG_SONG_RUPEES],
        &mOptions[RSK_SHUFFLE_ADULT_TRADE],
        &mOptions[RSK_SHUFFLE_100_GS_REWARD],
        &mOptions[RSK_GOSSIP_STONE_HINTS],
    };

//RANDOTODO refactor OptionGroups so we can actually make maintainable enum conversion.
    mSpoilerfileSettingNameToEnum = {
        { "Logic Options:Logic", RSK_LOGIC_RULES },
        { "Open Settings:Forest", RSK_FOREST },
        { "Open Settings:Kakariko Gate", RSK_KAK_GATE },
        { "Open Settings:Door of Time", RSK_DOOR_OF_TIME },
        { "Open Settings:Zora's Fountain", RSK_ZORAS_FOUNTAIN },
        { "World Settings:Starting Age", RSK_STARTING_AGE },
        { "Open Settings:Gerudo Fortress", RSK_GERUDO_FORTRESS },
        { "Open Settings:Rainbow Bridge", RSK_RAINBOW_BRIDGE },
        { "Open Settings:Stone Count", RSK_RAINBOW_BRIDGE_STONE_COUNT },
        { "Open Settings:Medallion Count", RSK_RAINBOW_BRIDGE_MEDALLION_COUNT },
        { "Open Settings:Reward Count", RSK_RAINBOW_BRIDGE_REWARD_COUNT },
        { "Open Settings:Dungeon Count", RSK_RAINBOW_BRIDGE_DUNGEON_COUNT },
        { "Open Settings:Token Count", RSK_RAINBOW_BRIDGE_TOKEN_COUNT },
        { "Open Settings:Bridge Reward Options", RSK_BRIDGE_OPTIONS },
        { "Open Settings:Ganon's Trials", RSK_GANONS_TRIALS },
        { "Open Settings:Ganon's Trials Count", RSK_TRIAL_COUNT },
        { "Start with Ocarina", RSK_STARTING_OCARINA },
        { "Shuffle Settings:Shuffle Ocarinas", RSK_SHUFFLE_OCARINA },
        { "Shuffle Settings:Shuffle Ocarina Buttons", RSK_SHUFFLE_OCARINA_BUTTONS },
        { "Shuffle Settings:Shuffle Swim", RSK_SHUFFLE_SWIM },
        { "Start with Deku Shield", RSK_STARTING_DEKU_SHIELD },
        { "Start with Kokiri Sword", RSK_STARTING_KOKIRI_SWORD },
        { "Start with Master Sword", RSK_STARTING_MASTER_SWORD },
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
        { "Shuffle Settings:Shuffle Kokiri Sword", RSK_SHUFFLE_KOKIRI_SWORD },
        { "Shuffle Settings:Shuffle Master Sword", RSK_SHUFFLE_MASTER_SWORD },
        { "Shuffle Settings:Shuffle Child's Wallet", RSK_SHUFFLE_CHILD_WALLET },
        { "Shuffle Settings:Include Tycoon Wallet", RSK_INCLUDE_TYCOON_WALLET },
        { "Shuffle Settings:Shuffle Dungeon Rewards", RSK_SHUFFLE_DUNGEON_REWARDS },
        { "Shuffle Settings:Shuffle Songs", RSK_SHUFFLE_SONGS },
        { "Shuffle Settings:Tokensanity", RSK_SHUFFLE_TOKENS },
        { "Shuffle Settings:Shopsanity", RSK_SHOPSANITY },
        { "Shuffle Settings:Shopsanity Specific Count", RSK_SHOPSANITY_COUNT },
        { "Shuffle Settings:Shopsanity Prices", RSK_SHOPSANITY_PRICES },
        { "Shuffle Settings:Shopsanity Fixed Amount", RSK_SHOPSANITY_PRICES_FIXED_PRICE },
        { "Shuffle Settings:Shopsanity Range 1", RSK_SHOPSANITY_PRICES_RANGE_1 },
        { "Shuffle Settings:Shopsanity Range 2", RSK_SHOPSANITY_PRICES_RANGE_2 },
        { "Shuffle Settings:Shopsanity No Wallet Weight", RSK_SHOPSANITY_PRICES_NO_WALLET_WEIGHT },
        { "Shuffle Settings:Shopsanity Child Wallet Weight", RSK_SHOPSANITY_PRICES_CHILD_WALLET_WEIGHT },
        { "Shuffle Settings:Shopsanity Adult Wallet Weight", RSK_SHOPSANITY_PRICES_ADULT_WALLET_WEIGHT },
        { "Shuffle Settings:Shopsanity Giants Wallet Weight", RSK_SHOPSANITY_PRICES_GIANT_WALLET_WEIGHT },
        { "Shuffle Settings:Shopsanity Tycoon Wallet Weight", RSK_SHOPSANITY_PRICES_TYCOON_WALLET_WEIGHT },
        { "Shuffle Settings:Shopsanity Affordable Prices", RSK_SHOPSANITY_PRICES_AFFORDABLE },
        { "Shuffle Settings:Scrub Shuffle", RSK_SHUFFLE_SCRUBS },
        { "Shuffle Settings:Scrubs Prices", RSK_SCRUBS_PRICES },
        { "Shuffle Settings:Scrubs Fixed Amount", RSK_SCRUBS_PRICES_FIXED_PRICE },
        { "Shuffle Settings:Scrubs Range 1", RSK_SCRUBS_PRICES_RANGE_1 },
        { "Shuffle Settings:Scrubs Range 2", RSK_SCRUBS_PRICES_RANGE_2 },
        { "Shuffle Settings:Scrubs No Wallet Weight", RSK_SCRUBS_PRICES_NO_WALLET_WEIGHT },
        { "Shuffle Settings:Scrubs Child Wallet Weight", RSK_SCRUBS_PRICES_CHILD_WALLET_WEIGHT },
        { "Shuffle Settings:Scrubs Adult Wallet Weight", RSK_SCRUBS_PRICES_ADULT_WALLET_WEIGHT },
        { "Shuffle Settings:Scrubs Giants Wallet Weight", RSK_SCRUBS_PRICES_GIANT_WALLET_WEIGHT },
        { "Shuffle Settings:Scrubs Tycoon Wallet Weight", RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT },
        { "Shuffle Settings:Scrubs Affordable Prices", RSK_SCRUBS_PRICES_AFFORDABLE },
        { "Shuffle Settings:Beehive Shuffle", RSK_SHUFFLE_BEEHIVES },
        { "Shuffle Settings:Shuffle Cows", RSK_SHUFFLE_COWS },
        { "Shuffle Settings:Shuffle Weird Egg", RSK_SHUFFLE_WEIRD_EGG },
        { "Shuffle Settings:Shuffle Gerudo Membership Card", RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD },
        { "Shuffle Settings:Shuffle Frog Song Rupees", RSK_SHUFFLE_FROG_SONG_RUPEES },
        { "Item Pool Settings:Item Pool", RSK_ITEM_POOL },
        { "Item Pool Settings:Ice Traps", RSK_ICE_TRAPS },
        { "Miscellaneous Settings:Gossip Stone Hints", RSK_GOSSIP_STONE_HINTS },
        { "Miscellaneous Settings:ToT Altar Hint", RSK_TOT_ALTAR_HINT },
        { "Miscellaneous Settings:Ganondorf Hint", RSK_GANONDORF_HINT },
        { "Miscellaneous Settings:Sheik Light Arrow Hint", RSK_SHEIK_LA_HINT },
        { "Miscellaneous Settings:Dampe's Diary Hint", RSK_DAMPES_DIARY_HINT },
        { "Miscellaneous Settings:Greg the Rupee Hint", RSK_GREG_HINT },
        { "Miscellaneous Settings:Hyrule Loach Hint", RSK_LOACH_HINT },
        { "Miscellaneous Settings:Saria's Hint", RSK_SARIA_HINT },
        { "Miscellaneous Settings:Frog Ocarina Game Hint", RSK_FROGS_HINT },
        { "Miscellaneous Settings:Ocarina of Time Hint", RSK_OOT_HINT },
        { "Miscellaneous Settings:10 GS Hint", RSK_KAK_10_SKULLS_HINT },
        { "Miscellaneous Settings:20 GS Hint", RSK_KAK_20_SKULLS_HINT },
        { "Miscellaneous Settings:30 GS Hint", RSK_KAK_30_SKULLS_HINT },
        { "Miscellaneous Settings:40 GS Hint", RSK_KAK_40_SKULLS_HINT },
        { "Miscellaneous Settings:50 GS Hint", RSK_KAK_50_SKULLS_HINT },
        { "Miscellaneous Settings:100 GS Hint", RSK_KAK_100_SKULLS_HINT },
        { "Miscellaneous Settings:Mask Shop Hint", RSK_MASK_SHOP_HINT },
        { "Miscellaneous Settings:Biggoron's Hint", RSK_BIGGORON_HINT },
        { "Miscellaneous Settings:Big Poes Hint", RSK_BIG_POES_HINT },
        { "Miscellaneous Settings:Chickens Hint", RSK_CHICKENS_HINT },
        { "Miscellaneous Settings:Malon Hint", RSK_MALON_HINT },
        { "Miscellaneous Settings:Horseback Archery Hint", RSK_HBA_HINT },
        { "Miscellaneous Settings:Warp Song Hints", RSK_WARP_SONG_HINTS },
        { "Miscellaneous Settings:Scrub Hint Text", RSK_SCRUB_TEXT_HINT },
        { "Miscellaneous Settings:Merchant Hint Text", RSK_MERCHANT_TEXT_HINT },
        { "Miscellaneous Settings:Fishing Pole Hint", RSK_FISHING_POLE_HINT },
        { "Miscellaneous Settings:Hint Clarity", RSK_HINT_CLARITY },
        { "Miscellaneous Settings:Hint Distribution", RSK_HINT_DISTRIBUTION },
        { "Shuffle Dungeon Items:Maps/Compasses", RSK_SHUFFLE_MAPANDCOMPASS },
        { "Shuffle Dungeon Items:Small Keys", RSK_KEYSANITY },
        { "Shuffle Dungeon Items:Gerudo Fortress Keys", RSK_GERUDO_KEYS },
        { "Shuffle Dungeon Items:Boss Keys", RSK_BOSS_KEYSANITY },
        { "Shuffle Dungeon Items:Ganon's Boss Key", RSK_GANONS_BOSS_KEY },
        { "Timesaver Settings:Skip Child Stealth", RSK_SKIP_CHILD_STEALTH },
        { "Timesaver Settings:Skip Child Zelda", RSK_SKIP_CHILD_ZELDA },
        { "Start with Consumables", RSK_STARTING_CONSUMABLES },
        { "Full Wallets", RSK_FULL_WALLETS },
        { "Timesaver Settings:Cuccos to return", RSK_CUCCO_COUNT },
        { "Timesaver Settings:Big Poe Target Count", RSK_BIG_POE_COUNT },
        { "Timesaver Settings:Skip Epona Race", RSK_SKIP_EPONA_RACE },
        { "Timesaver Settings:Complete Mask Quest", RSK_COMPLETE_MASK_QUEST },
        { "Timesaver Settings:Skip Scarecrow's Song", RSK_SKIP_SCARECROWS_SONG },
        { "Timesaver Settings:Enable Glitch-Useful Cutscenes", RSK_ENABLE_GLITCH_CUTSCENES },
        { "Logic Options:Night Skulltula's Expect Sun's Song", RSK_SKULLS_SUNS_SONG },
        { "Shuffle Settings:Shuffle Adult Trade", RSK_SHUFFLE_ADULT_TRADE },
        { "Shuffle Settings:Shuffle Merchants", RSK_SHUFFLE_MERCHANTS },
        { "Shuffle Settings:Merchant Prices", RSK_MERCHANT_PRICES },
        { "Shuffle Settings:Merchant Fixed Amount", RSK_MERCHANT_PRICES_FIXED_PRICE },
        { "Shuffle Settings:Merchant Range 1", RSK_MERCHANT_PRICES_RANGE_1 },
        { "Shuffle Settings:Merchant Range 2", RSK_MERCHANT_PRICES_RANGE_2 },
        { "Shuffle Settings:Merchant No Wallet Weight", RSK_MERCHANT_PRICES_NO_WALLET_WEIGHT },
        { "Shuffle Settings:Merchant Child Wallet Weight", RSK_MERCHANT_PRICES_CHILD_WALLET_WEIGHT },
        { "Shuffle Settings:Merchant Adult Wallet Weight", RSK_MERCHANT_PRICES_ADULT_WALLET_WEIGHT },
        { "Shuffle Settings:Merchant Giants Wallet Weight", RSK_MERCHANT_PRICES_GIANT_WALLET_WEIGHT },
        { "Shuffle Settings:Merchant Tycoon Wallet Weight", RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT },
        { "Shuffle Settings:Merchant Affordable Prices", RSK_MERCHANT_PRICES_AFFORDABLE },
        { "Miscellaneous Settings:Blue Fire Arrows", RSK_BLUE_FIRE_ARROWS },
        { "Miscellaneous Settings:Sunlight Arrows", RSK_SUNLIGHT_ARROWS },
        // TODO: Ammo Drop settings
        { "World Settings:Bombchu Drops", RSK_ENABLE_BOMBCHU_DROPS },
        { "World Settings:Bombchus in Logic", RSK_BOMBCHUS_IN_LOGIC },
        { "Shuffle Settings:Link's Pocket", RSK_LINKS_POCKET },
        { "World Settings:MQ Dungeon Setting", RSK_MQ_DUNGEON_RANDOM },
        { "World Settings:MQ Dungeon Count", RSK_MQ_DUNGEON_COUNT },
        { "World Settings:Set Dungeon Quests", RSK_MQ_DUNGEON_SET },
        { "Shuffle Dungeon Quest:Deku Tree", RSK_MQ_DEKU_TREE },
        { "Shuffle Dungeon Quest:Dodongo's Cavern", RSK_MQ_DODONGOS_CAVERN },
        { "Shuffle Dungeon Quest:Jabu-Jabu's Belly", RSK_MQ_JABU_JABU },
        { "Shuffle Dungeon Quest:Forest Temple", RSK_MQ_FOREST_TEMPLE },
        { "Shuffle Dungeon Quest:Fire Temple", RSK_MQ_FIRE_TEMPLE },
        { "Shuffle Dungeon Quest:Water Temple", RSK_MQ_WATER_TEMPLE },
        { "Shuffle Dungeon Quest:Spirit Temple", RSK_MQ_SPIRIT_TEMPLE },
        { "Shuffle Dungeon Quest:Shadow Temple", RSK_MQ_SHADOW_TEMPLE },
        { "Shuffle Dungeon Quest:Bottom of the Well", RSK_MQ_BOTTOM_OF_THE_WELL },
        { "Shuffle Dungeon Quest:Ice Cavern", RSK_MQ_ICE_CAVERN },
        { "Shuffle Dungeon Quest:GTG", RSK_MQ_GTG },
        { "Shuffle Dungeon Quest:Ganon's Castle", RSK_MQ_GANONS_CASTLE },
        { "Shuffle Dungeon Items:Stone Count", RSK_LACS_STONE_COUNT },
        { "Shuffle Dungeon Items:Medallion Count", RSK_LACS_MEDALLION_COUNT },
        { "Shuffle Dungeon Items:Reward Count", RSK_LACS_REWARD_COUNT },
        { "Shuffle Dungeon Items:Dungeon Count", RSK_LACS_DUNGEON_COUNT },
        { "Shuffle Dungeon Items:Token Count", RSK_LACS_TOKEN_COUNT },
        { "Shuffle Dungeon Items:LACS Reward Options", RSK_LACS_OPTIONS },
        { "Shuffle Dungeon Items:Key Rings", RSK_KEYRINGS },
        { "Shuffle Dungeon Items:Keyring Dungeon Count", RSK_KEYRINGS_RANDOM_COUNT },
        { "Shuffle Dungeon Items:Gerudo Fortress Keyring", RSK_KEYRINGS_GERUDO_FORTRESS },
        { "Shuffle Dungeon Items:Forest Temple Keyring", RSK_KEYRINGS_FOREST_TEMPLE },
        { "Shuffle Dungeon Items:Fire Temple Keyring", RSK_KEYRINGS_FIRE_TEMPLE },
        { "Shuffle Dungeon Items:Water Temple Keyring", RSK_KEYRINGS_WATER_TEMPLE },
        { "Shuffle Dungeon Items:Spirit Temple Keyring", RSK_KEYRINGS_SPIRIT_TEMPLE },
        { "Shuffle Dungeon Items:Shadow Temple Keyring", RSK_KEYRINGS_SHADOW_TEMPLE },
        { "Shuffle Dungeon Items:Bottom of the Well Keyring", RSK_KEYRINGS_BOTTOM_OF_THE_WELL },
        { "Shuffle Dungeon Items:GTG Keyring", RSK_KEYRINGS_GTG },
        { "Shuffle Dungeon Items:Ganon's Castle Keyring", RSK_KEYRINGS_GANONS_CASTLE },
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
        { "World Settings:Mix Bosses", RSK_MIX_BOSS_ENTRANCES },
        { "World Settings:Mix Overworld", RSK_MIX_OVERWORLD_ENTRANCES },
        { "World Settings:Mix Interiors", RSK_MIX_INTERIOR_ENTRANCES },
        { "World Settings:Mix Grottos", RSK_MIX_GROTTO_ENTRANCES },
        { "World Settings:Decouple Entrances", RSK_DECOUPLED_ENTRANCES },
        { "Gold Skulltula Tokens", RSK_STARTING_SKULLTULA_TOKEN },
        { "Hearts", RSK_STARTING_HEARTS },
        { "Logic Options:All Locations Reachable", RSK_ALL_LOCATIONS_REACHABLE },
        { "World Settings:Boss Entrances", RSK_SHUFFLE_BOSS_ENTRANCES },
        { "Shuffle Settings:Shuffle 100 GS Reward", RSK_SHUFFLE_100_GS_REWARD },
        { "World Settings:Triforce Hunt", RSK_TRIFORCE_HUNT },
        { "World Settings:Triforce Hunt Total Pieces", RSK_TRIFORCE_HUNT_PIECES_TOTAL },
        { "World Settings:Triforce Hunt Required Pieces", RSK_TRIFORCE_HUNT_PIECES_REQUIRED },
        { "Shuffle Settings:Shuffle Boss Souls", RSK_SHUFFLE_BOSS_SOULS },
        { "Shuffle Settings:Fishsanity", RSK_FISHSANITY },
        { "Shuffle Settings:Pond Fish Count", RSK_FISHSANITY_POND_COUNT },
        { "Shuffle Settings:Split Pond Fish", RSK_FISHSANITY_AGE_SPLIT },
        { "Shuffle Settings:Shuffle Fishing Pole", RSK_SHUFFLE_FISHING_POLE },
        { "Miscellaneous Settings:Infinite Upgrades", RSK_INFINITE_UPGRADES },
        { "Miscellaneous Settings:Skeleton Key", RSK_SKELETON_KEY },
        { "Shuffle Settings:Shuffle Deku Stick Bag", RSK_SHUFFLE_DEKU_STICK_BAG },
        { "Shuffle Settings:Shuffle Deku Nut Bag", RSK_SHUFFLE_DEKU_NUT_BAG },
    };
}

Option& Settings::GetOption(const RandomizerSettingKey key) {
    return mOptions[key];
}

TrickOption& Settings::GetTrickOption(const RandomizerTrick key) {
    return mTrickOptions[key];
}

void Settings::ResetTrickOptions() {
    for (int count = 0; count < RT_MAX; count++){
        mTrickOptions[count].SetSelectedIndex(0); //RANDOTODO this can probably be done better
    };
}

const std::array<Option, RSK_MAX>& Settings::GetAllOptions() const {
    return mOptions;
}

std::vector<Option *>& Settings::GetExcludeOptionsForArea(const RandomizerCheckArea area) {
    return mExcludeLocationsOptionsAreas[area];
}

const std::vector<std::vector<Option *>>& Settings::GetExcludeLocationsOptions() const {
    return mExcludeLocationsOptionsAreas;
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
    mHash = std::move(hash);
}

const std::string& Settings::GetSeedString() const {
    return mSeedString;
}

void Settings::SetSeedString(std::string seedString) {
    mSeedString = std::move(seedString);
}

uint32_t Settings::GetSeed() const {
    return mFinalSeed;
}

void Settings::SetSeed(const uint32_t seed) {
    mFinalSeed = seed;
}

const std::array<OptionGroup, RSG_MAX>& Settings::GetOptionGroups() {
    return mOptionGroups;
}

const OptionGroup& Settings::GetOptionGroup(const RandomizerSettingGroupKey key) {
    return mOptionGroups[key];
}

void Settings::SetAllFromCVar() {
    for (auto& option : mOptions) {
        option.SetFromCVar();
    }
}

void Settings::UpdateOptionProperties() {
    // Default to hiding bridge opts and the extra sliders.
    mOptions[RSK_RAINBOW_BRIDGE].AddFlag(IMFLAG_SEPARATOR_BOTTOM);
    mOptions[RSK_BRIDGE_OPTIONS].Hide();
    mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT].Hide();
    mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT].Hide();
    mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT].Hide();
    mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT].Hide();
    mOptions[RSK_RAINBOW_BRIDGE_TOKEN_COUNT].Hide();
    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("LogicRules"), RO_LOGIC_GLITCHLESS) == RO_LOGIC_VANILLA) {
        mOptionGroups[RSG_AREA_ACCESS_IMGUI].Disable();
        mOptions[RSK_STARTING_AGE].Disable("");
        mOptions[RSK_GERUDO_FORTRESS].Disable("");
        mOptions[RSK_RAINBOW_BRIDGE].Disable("");
        mOptions[RSK_BRIDGE_OPTIONS].Disable("");
        mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT].Disable("");
        mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT].Disable("");
        mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT].Disable("");
        mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT].Disable("");
        mOptions[RSK_RAINBOW_BRIDGE_TOKEN_COUNT].Disable("");
        mOptions[RSK_GANONS_TRIALS].Disable("");
        mOptions[RSK_TRIAL_COUNT].Disable("");
        mOptions[RSK_TRIFORCE_HUNT].Disable("");
        mOptions[RSK_TRIFORCE_HUNT_PIECES_TOTAL].Disable("");
        mOptions[RSK_TRIFORCE_HUNT_PIECES_REQUIRED].Disable("");
        mOptionGroups[RSG_ITEMS_IMGUI_TABLE].Disable();
        mOptionGroups[RSG_GAMEPLAY_IMGUI_TABLE].Disable();
        mOptions[RSK_LINKS_POCKET].Disable("");
        mOptions[RSK_STARTING_OCARINA].Disable("");
    } else {
        mOptionGroups[RSG_AREA_ACCESS_IMGUI].Enable();
        // Starting Age - Disabled when Forest is set to Closed or under very specific conditions
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("Forest"), RO_FOREST_CLOSED) == RO_FOREST_CLOSED ||
            (CVarGetInteger(CVAR_RANDOMIZER_SETTING("DoorOfTime"), RO_DOOROFTIME_CLOSED) == RO_DOOROFTIME_CLOSED &&
            CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleOcarinas"), RO_GENERIC_OFF) == RO_GENERIC_OFF)) /* closed door of time with ocarina shuffle off */ {
            mOptions[RSK_STARTING_AGE].Disable("This option is disabled due to other options making the game unbeatable");
        } else {
            mOptions[RSK_STARTING_AGE].Enable();
        }
        mOptions[RSK_GERUDO_FORTRESS].Enable();
        mOptions[RSK_RAINBOW_BRIDGE].Enable();
        mOptions[RSK_BRIDGE_OPTIONS].Enable();
        mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT].Enable();
        mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT].Enable();
        mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT].Enable();
        mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT].Enable();
        mOptions[RSK_RAINBOW_BRIDGE_TOKEN_COUNT].Enable();
        const uint8_t bridgeOpt = CVarGetInteger(CVAR_RANDOMIZER_SETTING("BridgeRewardOptions"), RO_BRIDGE_STANDARD_REWARD);
        switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("RainbowBridge"), RO_BRIDGE_VANILLA)) {
            case RO_BRIDGE_STONES:
                // Show Bridge Options and Stone Count slider
                mOptions[RSK_RAINBOW_BRIDGE].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
                mOptions[RSK_BRIDGE_OPTIONS].Unhide();
                mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT].Unhide();
                if (bridgeOpt == RO_BRIDGE_GREG_REWARD) {
                    if (mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT].GetOptionCount() == 4) {
                        mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT].ChangeOptions(NumOpts(0, 4));
                    }
                } else {
                    if (mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT].GetOptionCount() == 5) {
                        mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT].ChangeOptions(NumOpts(0, 3));
                    }
                }
                break;
            case RO_BRIDGE_MEDALLIONS:
                // Show Bridge Options and Medallion Count Slider
                mOptions[RSK_RAINBOW_BRIDGE].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
                mOptions[RSK_BRIDGE_OPTIONS].Unhide();
                mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT].Unhide();
                if (bridgeOpt == RO_BRIDGE_GREG_REWARD) {
                    if (mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT].GetOptionCount() == 7) {
                        mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT].ChangeOptions(NumOpts(0, 7));
                    }
                } else {
                    if (mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT].GetOptionCount() == 8) {
                        mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT].ChangeOptions(NumOpts(0, 6));
                    }
                }
                break;
            case RO_BRIDGE_DUNGEON_REWARDS:
                // Show Bridge Options and Dungeon Reward Count Slider
                mOptions[RSK_RAINBOW_BRIDGE].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
                mOptions[RSK_BRIDGE_OPTIONS].Unhide();
                mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT].Unhide();
                if (bridgeOpt == RO_BRIDGE_GREG_REWARD) {
                    if (mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT].GetOptionCount() == 10) {
                        mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT].ChangeOptions(NumOpts(0, 10));
                    }
                } else {
                    if (mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT].GetOptionCount() == 11) {
                        mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT].ChangeOptions(NumOpts(0, 9));
                    }
                }
                break;
            case RO_BRIDGE_DUNGEONS:
                // Show Bridge Options and Dungeon Count Slider
                mOptions[RSK_RAINBOW_BRIDGE].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
                mOptions[RSK_BRIDGE_OPTIONS].Unhide();
                mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT].Unhide();
                if (bridgeOpt == RO_BRIDGE_GREG_REWARD) {
                    if (mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT].GetOptionCount() == 9) {
                        mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT].ChangeOptions(NumOpts(0, 9));
                    }
                } else {
                    if (mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT].GetOptionCount() == 10) {
                        mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT].ChangeOptions(NumOpts(0, 8));
                    }
                }
                break;
            case RO_BRIDGE_TOKENS:
                // Show token count slider (not bridge options)
                mOptions[RSK_RAINBOW_BRIDGE].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
                mOptions[RSK_BRIDGE_OPTIONS].Hide();
                mOptions[RSK_RAINBOW_BRIDGE_TOKEN_COUNT].Unhide();
                break;
            default:
                break;
        }
        mOptions[RSK_GANONS_TRIALS].Enable();
        mOptions[RSK_TRIAL_COUNT].Enable();
        // Only show the trial count slider if Trials is set to Set Number
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("GanonTrial"), RO_GANONS_TRIALS_SKIP) == RO_GANONS_TRIALS_SET_NUMBER) {
            mOptions[RSK_GANONS_TRIALS].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
            mOptions[RSK_TRIAL_COUNT].Unhide();
        } else {
            mOptions[RSK_GANONS_TRIALS].AddFlag(IMFLAG_SEPARATOR_BOTTOM);
            mOptions[RSK_TRIAL_COUNT].Hide();
        }
        mOptions[RSK_TRIFORCE_HUNT].Enable();
        mOptions[RSK_TRIFORCE_HUNT_PIECES_TOTAL].Enable();
        mOptions[RSK_TRIFORCE_HUNT_PIECES_REQUIRED].Enable();
        // Remove the pieces required/total sliders and add a separator after Tirforce Hunt if Triforce Hunt is off
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("TriforceHunt"), RO_GENERIC_OFF) == RO_GENERIC_OFF) {
            mOptions[RSK_TRIFORCE_HUNT_PIECES_REQUIRED].Hide();
            mOptions[RSK_TRIFORCE_HUNT_PIECES_TOTAL].Hide();
            mOptions[RSK_TRIFORCE_HUNT].AddFlag(IMFLAG_SEPARATOR_BOTTOM);
        } else {
            mOptions[RSK_TRIFORCE_HUNT_PIECES_REQUIRED].Unhide();
            mOptions[RSK_TRIFORCE_HUNT_PIECES_TOTAL].Unhide();
            mOptions[RSK_TRIFORCE_HUNT].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
        }
        // Update triforce pieces required to be capped at the current value for pieces total.
        const uint8_t triforceTotal = CVarGetInteger(CVAR_RANDOMIZER_SETTING("TriforceHuntTotalPieces"), 30);
        if (mOptions[RSK_TRIFORCE_HUNT_PIECES_REQUIRED].GetOptionCount() != triforceTotal + 1) {
            mOptions[RSK_TRIFORCE_HUNT_PIECES_REQUIRED].ChangeOptions(NumOpts(1, triforceTotal + 1));
        }
        mOptionGroups[RSG_ITEMS_IMGUI_TABLE].Enable();
        mOptionGroups[RSG_GAMEPLAY_IMGUI_TABLE].Enable();
        // Link's Pocket - Disabled when Dungeon Rewards are shuffled to End of Dungeon
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleDungeonReward"), RO_DUNGEON_REWARDS_END_OF_DUNGEON) == RO_DUNGEON_REWARDS_END_OF_DUNGEON) {
            mOptions[RSK_LINKS_POCKET].Disable("This option is disabled because \"Dungeon Rewards\" are shuffled to \"End of Dungeons\".");
        } else {
            mOptions[RSK_LINKS_POCKET].Enable();
        }
        mOptions[RSK_STARTING_OCARINA].Enable();
    }

    // Don't show any MQ options if both quests aren't available
    if (!(OTRGlobals::Instance->HasMasterQuest() && OTRGlobals::Instance->HasOriginal())) {
        mOptions[RSK_MQ_DUNGEON_RANDOM].Hide();
        mOptions[RSK_MQ_DUNGEON_COUNT].Hide();
        mOptions[RSK_MQ_DUNGEON_SET].Hide();
        mOptions[RSK_MQ_DEKU_TREE].Hide();
        mOptions[RSK_MQ_DODONGOS_CAVERN].Hide();
        mOptions[RSK_MQ_JABU_JABU].Hide();
        mOptions[RSK_MQ_FOREST_TEMPLE].Hide();
        mOptions[RSK_MQ_FIRE_TEMPLE].Hide();
        mOptions[RSK_MQ_WATER_TEMPLE].Hide();
        mOptions[RSK_MQ_SPIRIT_TEMPLE].Hide();
        mOptions[RSK_MQ_SHADOW_TEMPLE].Hide();
        mOptions[RSK_MQ_BOTTOM_OF_THE_WELL].Hide();
        mOptions[RSK_MQ_ICE_CAVERN].Hide();
        mOptions[RSK_MQ_GTG].Hide();
        mOptions[RSK_MQ_GANONS_CASTLE].Hide();
    } else {
        // If any MQ Options are available, show the MQ Dungeon Randomization Combobox
        mOptions[RSK_MQ_DUNGEON_RANDOM].Unhide();
        switch(CVarGetInteger(CVAR_RANDOMIZER_SETTING("MQDungeons"), RO_MQ_DUNGEONS_NONE)) {
            // If No MQ Dungeons, add a separator after the combobx and hide
            // the count slider and the toggle for individual dungeon selections.
            case RO_MQ_DUNGEONS_NONE:
                mOptions[RSK_MQ_DUNGEON_RANDOM].AddFlag(IMFLAG_SEPARATOR_BOTTOM);
                mOptions[RSK_MQ_DUNGEON_COUNT].Hide();
                mOptions[RSK_MQ_DUNGEON_SET].Hide();
                break;
            // If Set Number, remove the separator and show both the count slider and the
            // individual dungeon selection toggle.
            case RO_MQ_DUNGEONS_SET_NUMBER:
                mOptions[RSK_MQ_DUNGEON_RANDOM].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
                mOptions[RSK_MQ_DUNGEON_COUNT].Unhide();
                mOptions[RSK_MQ_DUNGEON_SET].Unhide();
                break;
            // else if random number or selection only, remove the separator and only show
            // the individual dungeon selection toggle.
            case RO_MQ_DUNGEONS_RANDOM_NUMBER:
                mOptions[RSK_MQ_DUNGEON_RANDOM].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
                mOptions[RSK_MQ_DUNGEON_COUNT].Hide();
                mOptions[RSK_MQ_DUNGEON_SET].Unhide();
                break;
            case RO_MQ_DUNGEONS_SELECTION:
                mOptions[RSK_MQ_DUNGEON_RANDOM].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
                mOptions[RSK_MQ_DUNGEON_COUNT].Hide();
                mOptions[RSK_MQ_DUNGEON_SET].Hide();
                break;
            default:
                break;
        }
        // Controls whether or not to show the selectors for individual dungeons.
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("MQDungeons"), RO_MQ_DUNGEONS_NONE) != RO_MQ_DUNGEONS_NONE &&
            (CVarGetInteger(CVAR_RANDOMIZER_SETTING("MQDungeonsSelection"), RO_GENERIC_OFF) == RO_GENERIC_ON ||
             CVarGetInteger(CVAR_RANDOMIZER_SETTING("MQDungeons"), RO_MQ_DUNGEONS_NONE) == RO_MQ_DUNGEONS_SELECTION)) {
            // if showing the dungeon selectors, remove the separator after the Set Dungeons checkbox.
            mOptions[RSK_MQ_DUNGEON_SET].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
            mOptions[RSK_MQ_DEKU_TREE].Unhide();
            mOptions[RSK_MQ_DODONGOS_CAVERN].Unhide();
            mOptions[RSK_MQ_JABU_JABU].Unhide();
            mOptions[RSK_MQ_FOREST_TEMPLE].Unhide();
            mOptions[RSK_MQ_FIRE_TEMPLE].Unhide();
            mOptions[RSK_MQ_WATER_TEMPLE].Unhide();
            mOptions[RSK_MQ_SPIRIT_TEMPLE].Unhide();
            mOptions[RSK_MQ_SHADOW_TEMPLE].Unhide();
            mOptions[RSK_MQ_BOTTOM_OF_THE_WELL].Unhide();
            mOptions[RSK_MQ_ICE_CAVERN].Unhide();
            mOptions[RSK_MQ_GTG].Unhide();
            mOptions[RSK_MQ_GANONS_CASTLE].Unhide();
        } else {
            // If those are not shown, add a separator after the Set Dungeons checkbox.
            mOptions[RSK_MQ_DUNGEON_SET].AddFlag(IMFLAG_SEPARATOR_BOTTOM);
            mOptions[RSK_MQ_DEKU_TREE].Hide();
            mOptions[RSK_MQ_DODONGOS_CAVERN].Hide();
            mOptions[RSK_MQ_JABU_JABU].Hide();
            mOptions[RSK_MQ_FOREST_TEMPLE].Hide();
            mOptions[RSK_MQ_FIRE_TEMPLE].Hide();
            mOptions[RSK_MQ_WATER_TEMPLE].Hide();
            mOptions[RSK_MQ_SPIRIT_TEMPLE].Hide();
            mOptions[RSK_MQ_SHADOW_TEMPLE].Hide();
            mOptions[RSK_MQ_BOTTOM_OF_THE_WELL].Hide();
            mOptions[RSK_MQ_ICE_CAVERN].Hide();
            mOptions[RSK_MQ_GTG].Hide();
            mOptions[RSK_MQ_GANONS_CASTLE].Hide();
        }
    }

    // Show mixed entrance pool options if mixed entrance pools are enabled at all.
    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("MixedEntrances"), RO_GENERIC_OFF)) {
        mOptions[RSK_MIXED_ENTRANCE_POOLS].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
        mOptions[RSK_MIX_DUNGEON_ENTRANCES].Unhide();
        mOptions[RSK_MIX_BOSS_ENTRANCES].Unhide();
        mOptions[RSK_MIX_OVERWORLD_ENTRANCES].Unhide();
        mOptions[RSK_MIX_INTERIOR_ENTRANCES].Unhide();
        mOptions[RSK_MIX_GROTTO_ENTRANCES].Unhide();
    } else {
        mOptions[RSK_MIXED_ENTRANCE_POOLS].AddFlag(IMFLAG_SEPARATOR_BOTTOM);
        mOptions[RSK_MIX_DUNGEON_ENTRANCES].Hide();
        mOptions[RSK_MIX_BOSS_ENTRANCES].Hide();
        mOptions[RSK_MIX_OVERWORLD_ENTRANCES].Hide();
        mOptions[RSK_MIX_INTERIOR_ENTRANCES].Hide();
        mOptions[RSK_MIX_GROTTO_ENTRANCES].Hide();
    }
    // Shuffle Weird Egg - Disabled when Skip Child Zelda is active
    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("SkipChildZelda"), RO_GENERIC_DONT_SKIP)) {
        mOptions[RSK_SHUFFLE_WEIRD_EGG].Disable("This option is disabled because \"Skip Child Zelda\" is enabled.");
    } else {
        mOptions[RSK_SHUFFLE_WEIRD_EGG].Enable();
    }
    bool isTycoon = CVarGetInteger(CVAR_RANDOMIZER_SETTING("IncludeTycoonWallet"), RO_GENERIC_OFF);
    // Hide shopsanity prices if shopsanity is off or zero
    switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("Shopsanity"), RO_SHOPSANITY_OFF)) {
        case RO_SHOPSANITY_OFF:
            mOptions[RSK_SHOPSANITY].AddFlag(IMFLAG_SEPARATOR_BOTTOM);
            mOptions[RSK_SHOPSANITY_COUNT].Hide();
            mOptions[RSK_SHOPSANITY_COUNT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_AFFORDABLE].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_RANGE_1].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_RANGE_2].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_NO_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_CHILD_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_ADULT_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_GIANT_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_TYCOON_WALLET_WEIGHT].Hide();
            break;
        case RO_SHOPSANITY_SPECIFIC_COUNT:
            mOptions[RSK_SHOPSANITY_COUNT].Unhide();
            HandleShopsanityPriceUI();
            break;
        case RO_SHOPSANITY_RANDOM:
            mOptions[RSK_SHOPSANITY_COUNT].Hide();
            HandleShopsanityPriceUI();
            break;
    }
    switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleScrubs"), RO_SCRUBS_OFF)) {
        case RO_SCRUBS_OFF:
            mOptions[RSK_SHUFFLE_SCRUBS].AddFlag(IMFLAG_SEPARATOR_BOTTOM);
            mOptions[RSK_SCRUBS_PRICES].Hide();
            mOptions[RSK_SCRUBS_PRICES_AFFORDABLE].Hide();
            mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].Hide();
            mOptions[RSK_SCRUBS_PRICES_RANGE_1].Hide();
            mOptions[RSK_SCRUBS_PRICES_RANGE_2].Hide();
            mOptions[RSK_SCRUBS_PRICES_NO_WALLET_WEIGHT].Hide();
            mOptions[RSK_SCRUBS_PRICES_CHILD_WALLET_WEIGHT].Hide();
            mOptions[RSK_SCRUBS_PRICES_ADULT_WALLET_WEIGHT].Hide();
            mOptions[RSK_SCRUBS_PRICES_GIANT_WALLET_WEIGHT].Hide();
            mOptions[RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT].Hide();
            break;
        default:
            mOptions[RSK_SHUFFLE_SCRUBS].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
            mOptions[RSK_SCRUBS_PRICES].Unhide();
            switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ScrubsPrices"), RO_PRICE_VANILLA)){
                case RO_PRICE_FIXED:
                    mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].Unhide();
                    mOptions[RSK_SCRUBS_PRICES_RANGE_1].Hide();
                    mOptions[RSK_SCRUBS_PRICES_RANGE_2].Hide();
                    mOptions[RSK_SCRUBS_PRICES_NO_WALLET_WEIGHT].Hide();
                    mOptions[RSK_SCRUBS_PRICES_CHILD_WALLET_WEIGHT].Hide();
                    mOptions[RSK_SCRUBS_PRICES_ADULT_WALLET_WEIGHT].Hide();
                    mOptions[RSK_SCRUBS_PRICES_GIANT_WALLET_WEIGHT].Hide();
                    mOptions[RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT].Hide();
                    if (isTycoon ? mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].GetOptionCount() == 501 : mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].GetOptionCount() == 1000) {
                        mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].ChangeOptions(isTycoon ? NumOpts(0, 999) : NumOpts(0, 500));
                    }
                    mOptions[RSK_SCRUBS_PRICES_AFFORDABLE].Hide();
                    break;
                case RO_PRICE_RANGE:
                    mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].Hide();
                    mOptions[RSK_SCRUBS_PRICES_RANGE_1].Unhide();
                    mOptions[RSK_SCRUBS_PRICES_RANGE_2].Unhide();
                    mOptions[RSK_SCRUBS_PRICES_NO_WALLET_WEIGHT].Hide();
                    mOptions[RSK_SCRUBS_PRICES_CHILD_WALLET_WEIGHT].Hide();
                    mOptions[RSK_SCRUBS_PRICES_ADULT_WALLET_WEIGHT].Hide();
                    mOptions[RSK_SCRUBS_PRICES_GIANT_WALLET_WEIGHT].Hide();
                    mOptions[RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT].Hide();
                    if (isTycoon ? mOptions[RSK_SCRUBS_PRICES_RANGE_1].GetOptionCount() == 101 : mOptions[RSK_SCRUBS_PRICES_RANGE_1].GetOptionCount() == 200) {
                        mOptions[RSK_SCRUBS_PRICES_RANGE_1].ChangeOptions(isTycoon ? NumOpts(0, 995, 5) : NumOpts(0, 500, 5));
                        mOptions[RSK_SCRUBS_PRICES_RANGE_2].ChangeOptions(isTycoon ? NumOpts(0, 995, 5) : NumOpts(0, 500, 5));
                    }
                    mOptions[RSK_SCRUBS_PRICES_AFFORDABLE].Unhide();
                    break;
                case RO_PRICE_SET_BY_WALLET:
                    mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].Hide();
                    mOptions[RSK_SCRUBS_PRICES_RANGE_1].Hide();
                    mOptions[RSK_SCRUBS_PRICES_RANGE_2].Hide();
                    mOptions[RSK_SCRUBS_PRICES_NO_WALLET_WEIGHT].Unhide();
                    mOptions[RSK_SCRUBS_PRICES_CHILD_WALLET_WEIGHT].Unhide();
                    mOptions[RSK_SCRUBS_PRICES_ADULT_WALLET_WEIGHT].Unhide();
                    mOptions[RSK_SCRUBS_PRICES_GIANT_WALLET_WEIGHT].Unhide();
                    if (isTycoon){
                        mOptions[RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT].Unhide();
                    } else {
                        mOptions[RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT].Hide();
                    }
                    mOptions[RSK_SCRUBS_PRICES_AFFORDABLE].Unhide();
                    break;
                default:
                    mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].Hide();
                    mOptions[RSK_SCRUBS_PRICES_RANGE_1].Hide();
                    mOptions[RSK_SCRUBS_PRICES_RANGE_2].Hide();
                    mOptions[RSK_SCRUBS_PRICES_NO_WALLET_WEIGHT].Hide();
                    mOptions[RSK_SCRUBS_PRICES_CHILD_WALLET_WEIGHT].Hide();
                    mOptions[RSK_SCRUBS_PRICES_ADULT_WALLET_WEIGHT].Hide();
                    mOptions[RSK_SCRUBS_PRICES_GIANT_WALLET_WEIGHT].Hide();
                    mOptions[RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT].Hide();
                    mOptions[RSK_SCRUBS_PRICES_AFFORDABLE].Unhide();
                    break;
            }
            break;
    }
    switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleMerchants"), RO_SHUFFLE_MERCHANTS_OFF)) {
        case RO_SHUFFLE_MERCHANTS_OFF:
            mOptions[RSK_SHUFFLE_MERCHANTS].AddFlag(IMFLAG_SEPARATOR_BOTTOM);
            mOptions[RSK_MERCHANT_PRICES].Hide();
            mOptions[RSK_MERCHANT_PRICES_AFFORDABLE].Hide();
            mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].Hide();
            mOptions[RSK_MERCHANT_PRICES_RANGE_1].Hide();
            mOptions[RSK_MERCHANT_PRICES_RANGE_2].Hide();
            mOptions[RSK_MERCHANT_PRICES_NO_WALLET_WEIGHT].Hide();
            mOptions[RSK_MERCHANT_PRICES_CHILD_WALLET_WEIGHT].Hide();
            mOptions[RSK_MERCHANT_PRICES_ADULT_WALLET_WEIGHT].Hide();
            mOptions[RSK_MERCHANT_PRICES_GIANT_WALLET_WEIGHT].Hide();
            mOptions[RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT].Hide();
            break;
        default:
            mOptions[RSK_SHUFFLE_MERCHANTS].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
            mOptions[RSK_MERCHANT_PRICES].Unhide();
            switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("MerchantPrices"), RO_PRICE_VANILLA)){
                case RO_PRICE_FIXED:
                    mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].Unhide();
                    mOptions[RSK_MERCHANT_PRICES_RANGE_1].Hide();
                    mOptions[RSK_MERCHANT_PRICES_RANGE_2].Hide();
                    mOptions[RSK_MERCHANT_PRICES_NO_WALLET_WEIGHT].Hide();
                    mOptions[RSK_MERCHANT_PRICES_CHILD_WALLET_WEIGHT].Hide();
                    mOptions[RSK_MERCHANT_PRICES_ADULT_WALLET_WEIGHT].Hide();
                    mOptions[RSK_MERCHANT_PRICES_GIANT_WALLET_WEIGHT].Hide();
                    mOptions[RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT].Hide();
                    if (isTycoon ? mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].GetOptionCount() == 501 : mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].GetOptionCount() == 1000) {
                        mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].ChangeOptions(isTycoon ? NumOpts(0, 999) : NumOpts(0, 500));
                    }
                    mOptions[RSK_MERCHANT_PRICES_AFFORDABLE].Hide();
                    break;
                case RO_PRICE_RANGE:
                    mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].Hide();
                    mOptions[RSK_MERCHANT_PRICES_RANGE_1].Unhide();
                    mOptions[RSK_MERCHANT_PRICES_RANGE_2].Unhide();
                    mOptions[RSK_MERCHANT_PRICES_NO_WALLET_WEIGHT].Hide();
                    mOptions[RSK_MERCHANT_PRICES_CHILD_WALLET_WEIGHT].Hide();
                    mOptions[RSK_MERCHANT_PRICES_ADULT_WALLET_WEIGHT].Hide();
                    mOptions[RSK_MERCHANT_PRICES_GIANT_WALLET_WEIGHT].Hide();
                    mOptions[RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT].Hide();
                    if (isTycoon ? mOptions[RSK_MERCHANT_PRICES_RANGE_1].GetOptionCount() == 101 : mOptions[RSK_MERCHANT_PRICES_RANGE_1].GetOptionCount() == 200) {
                        mOptions[RSK_MERCHANT_PRICES_RANGE_1].ChangeOptions(isTycoon ? NumOpts(0, 995, 5) : NumOpts(0, 500, 5));
                        mOptions[RSK_MERCHANT_PRICES_RANGE_2].ChangeOptions(isTycoon ? NumOpts(0, 995, 5) : NumOpts(0, 500, 5));
                    }
                    mOptions[RSK_MERCHANT_PRICES_AFFORDABLE].Unhide();
                    break;
                case RO_PRICE_SET_BY_WALLET:
                    mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].Hide();
                    mOptions[RSK_MERCHANT_PRICES_RANGE_1].Hide();
                    mOptions[RSK_MERCHANT_PRICES_RANGE_2].Hide();
                    mOptions[RSK_MERCHANT_PRICES_NO_WALLET_WEIGHT].Unhide();
                    mOptions[RSK_MERCHANT_PRICES_CHILD_WALLET_WEIGHT].Unhide();
                    mOptions[RSK_MERCHANT_PRICES_ADULT_WALLET_WEIGHT].Unhide();
                    mOptions[RSK_MERCHANT_PRICES_GIANT_WALLET_WEIGHT].Unhide();
                    if (isTycoon){
                        mOptions[RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT].Unhide();
                    } else {
                        mOptions[RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT].Hide();
                    }
                    mOptions[RSK_MERCHANT_PRICES_AFFORDABLE].Unhide();
                    break;
                default:
                    mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].Hide();
                    mOptions[RSK_MERCHANT_PRICES_RANGE_1].Hide();
                    mOptions[RSK_MERCHANT_PRICES_RANGE_2].Hide();
                    mOptions[RSK_MERCHANT_PRICES_NO_WALLET_WEIGHT].Hide();
                    mOptions[RSK_MERCHANT_PRICES_CHILD_WALLET_WEIGHT].Hide();
                    mOptions[RSK_MERCHANT_PRICES_ADULT_WALLET_WEIGHT].Hide();
                    mOptions[RSK_MERCHANT_PRICES_GIANT_WALLET_WEIGHT].Hide();
                    mOptions[RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT].Hide();
                    mOptions[RSK_MERCHANT_PRICES_AFFORDABLE].Unhide();
                    break;
            }
            break;
    }
    // Hide fishing pond settings if we aren't shuffling the fishing pond
    switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("Fishsanity"), RO_FISHSANITY_OFF)) {
        case RO_FISHSANITY_POND:
        case RO_FISHSANITY_BOTH:
            mOptions[RSK_FISHSANITY].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
            mOptions[RSK_FISHSANITY_POND_COUNT].Unhide();
            mOptions[RSK_FISHSANITY_AGE_SPLIT].Unhide();
            break;
        default:
            mOptions[RSK_FISHSANITY].AddFlag(IMFLAG_SEPARATOR_BOTTOM);
            mOptions[RSK_FISHSANITY_POND_COUNT].Hide();
            mOptions[RSK_FISHSANITY_AGE_SPLIT].Hide();
    }
    // Disable fishing pole hint if the fishing pole is not shuffled
    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleFishingPole"), RO_GENERIC_OFF)) {
        mOptions[RSK_FISHING_POLE_HINT].Enable();
    } else {
        mOptions[RSK_FISHING_POLE_HINT].Disable("This option is disabled since the fishing pole is not shuffled.");
    }
    // Shuffle 100 GS Reward - Force-Enabled if Ganon's Boss Key is on the 100 GS Reward
    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleGanonBossKey"), RO_GANON_BOSS_KEY_VANILLA) == RO_GANON_BOSS_KEY_KAK_TOKENS) {
        mOptions[RSK_SHUFFLE_100_GS_REWARD].Disable("This option is force-enabled because \"Ganon's Boss Key\" is set to \"100 GS Reward.\"", UIWidgets::CheckboxGraphics::Checkmark);
    } else {
        mOptions[RSK_SHUFFLE_100_GS_REWARD].Enable();
    }
    // Default state for Keyrings GUI
    mOptions[RSK_KEYRINGS].AddFlag(IMFLAG_SEPARATOR_BOTTOM);
    mOptions[RSK_KEYRINGS_RANDOM_COUNT].Hide();
    mOptions[RSK_KEYRINGS_GERUDO_FORTRESS].Hide();
    mOptions[RSK_KEYRINGS_FOREST_TEMPLE].Hide();
    mOptions[RSK_KEYRINGS_FIRE_TEMPLE].Hide();
    mOptions[RSK_KEYRINGS_WATER_TEMPLE].Hide();
    mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE].Hide();
    mOptions[RSK_KEYRINGS_SHADOW_TEMPLE].Hide();
    mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL].Hide();
    mOptions[RSK_KEYRINGS_GTG].Hide();
    mOptions[RSK_KEYRINGS_GANONS_CASTLE].Hide();
    switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleKeyRings"), RO_KEYRINGS_OFF)) {
        case RO_KEYRINGS_COUNT:
            // Show count slider.
            mOptions[RSK_KEYRINGS].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
            mOptions[RSK_KEYRINGS_RANDOM_COUNT].Unhide();
            break;
        case RO_KEYRINGS_SELECTION:
            // Show checkboxes for each dungeon with keys.
            mOptions[RSK_KEYRINGS].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
            mOptions[RSK_KEYRINGS_GERUDO_FORTRESS].Unhide();
            mOptions[RSK_KEYRINGS_FOREST_TEMPLE].Unhide();
            mOptions[RSK_KEYRINGS_FIRE_TEMPLE].Unhide();
            mOptions[RSK_KEYRINGS_WATER_TEMPLE].Unhide();
            mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE].Unhide();
            mOptions[RSK_KEYRINGS_SHADOW_TEMPLE].Unhide();
            mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL].Unhide();
            mOptions[RSK_KEYRINGS_GTG].Unhide();
            mOptions[RSK_KEYRINGS_GANONS_CASTLE].Unhide();
        default:
            break;
    }
    const uint8_t maxKeyringCount = (CVarGetInteger(CVAR_RANDOMIZER_SETTING("GerudoFortress"), RO_GF_NORMAL) == RO_GF_NORMAL &&
                               CVarGetInteger(CVAR_RANDOMIZER_SETTING("GerudoKeys"), RO_GERUDO_KEYS_VANILLA) != RO_GERUDO_KEYS_VANILLA) ? 9 : 8;
    if (mOptions[RSK_KEYRINGS_RANDOM_COUNT].GetOptionCount() != maxKeyringCount + 1) {
        mOptions[RSK_KEYRINGS_RANDOM_COUNT].ChangeOptions(NumOpts(0, maxKeyringCount));
    }
    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("GerudoFortress"), RO_GF_NORMAL) != RO_GF_NORMAL ||
        CVarGetInteger(CVAR_RANDOMIZER_SETTING("GerudoKeys"), RO_GERUDO_KEYS_VANILLA) == RO_GERUDO_KEYS_VANILLA) {
        mOptions[RSK_KEYRINGS_GERUDO_FORTRESS].Disable("Disabled because the currently selected Gerudo Fortress Carpenters\n"
            "setting and/or Gerudo Fortress Keys setting is incompatible with\n"
            "having a Gerudo Fortress Keyring.");
    } else {
        mOptions[RSK_KEYRINGS_GERUDO_FORTRESS].Enable();
    }
    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("TriforceHunt"), RO_GENERIC_OFF)) {
        mOptions[RSK_GANONS_BOSS_KEY].Disable("This option is disabled because Triforce Hunt is enabled."
            "Ganon's Boss key\nwill instead be given to you after Triforce Hunt completion.");
    } else {
        mOptions[RSK_GANONS_BOSS_KEY].Enable();
    }
    mOptions[RSK_GANONS_BOSS_KEY].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
    mOptions[RSK_LACS_OPTIONS].Hide();
    mOptions[RSK_LACS_STONE_COUNT].Hide();
    mOptions[RSK_LACS_MEDALLION_COUNT].Hide();
    mOptions[RSK_LACS_REWARD_COUNT].Hide();
    mOptions[RSK_LACS_DUNGEON_COUNT].Hide();
    mOptions[RSK_LACS_TOKEN_COUNT].Hide();
    const uint8_t lacsOpts = CVarGetInteger(CVAR_RANDOMIZER_SETTING("LacsRewardOptions"), RO_LACS_STANDARD_REWARD);
    switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleGanonBossKey"), RO_GANON_BOSS_KEY_VANILLA)) {
        case RO_GANON_BOSS_KEY_LACS_STONES:
            mOptions[RSK_LACS_OPTIONS].Unhide();
            mOptions[RSK_LACS_STONE_COUNT].Unhide();
            if (lacsOpts == RO_LACS_GREG_REWARD) {
                if (mOptions[RSK_LACS_STONE_COUNT].GetOptionCount() == 4) {
                    mOptions[RSK_LACS_STONE_COUNT].ChangeOptions(NumOpts(0, 4));
                }
            } else {
                if (mOptions[RSK_LACS_STONE_COUNT].GetOptionCount() == 5) {
                    mOptions[RSK_LACS_STONE_COUNT].ChangeOptions(NumOpts(0, 3));
                }
            }
            break;
        case RO_GANON_BOSS_KEY_LACS_MEDALLIONS:
            mOptions[RSK_LACS_OPTIONS].Unhide();
            mOptions[RSK_LACS_MEDALLION_COUNT].Unhide();
            if (lacsOpts == RO_LACS_GREG_REWARD) {
                if (mOptions[RSK_LACS_MEDALLION_COUNT].GetOptionCount() == 7) {
                    mOptions[RSK_LACS_MEDALLION_COUNT].ChangeOptions(NumOpts(0, 7));
                }
            } else {
                if (mOptions[RSK_LACS_MEDALLION_COUNT].GetOptionCount() == 8) {
                    mOptions[RSK_LACS_MEDALLION_COUNT].ChangeOptions(NumOpts(0, 6));
                }
            }
            break;
        case RO_GANON_BOSS_KEY_LACS_REWARDS:
            mOptions[RSK_LACS_OPTIONS].Unhide();
            mOptions[RSK_LACS_REWARD_COUNT].Unhide();
            if (lacsOpts == RO_LACS_GREG_REWARD) {
                if (mOptions[RSK_LACS_REWARD_COUNT].GetOptionCount() == 10) {
                    mOptions[RSK_LACS_REWARD_COUNT].ChangeOptions(NumOpts(0, 10));
                }
            } else {
                if (mOptions[RSK_LACS_REWARD_COUNT].GetOptionCount() == 11) {
                    mOptions[RSK_LACS_REWARD_COUNT].ChangeOptions(NumOpts(0, 9));
                }
            }
            break;
        case RO_GANON_BOSS_KEY_LACS_DUNGEONS:
            mOptions[RSK_LACS_OPTIONS].Unhide();
            mOptions[RSK_LACS_DUNGEON_COUNT].Unhide();
            if (lacsOpts == RO_LACS_GREG_REWARD) {
                if (mOptions[RSK_LACS_DUNGEON_COUNT].GetOptionCount() == 9) {
                    mOptions[RSK_LACS_DUNGEON_COUNT].ChangeOptions(NumOpts(0, 9));
                }
            } else {
                if (mOptions[RSK_LACS_DUNGEON_COUNT].GetOptionCount() == 10) {
                    mOptions[RSK_LACS_DUNGEON_COUNT].ChangeOptions(NumOpts(0, 8));
                }
            }
            break;
        case RO_GANON_BOSS_KEY_LACS_TOKENS:
            mOptions[RSK_LACS_TOKEN_COUNT].Unhide();
            break;
        default:
            mOptions[RSK_GANONS_BOSS_KEY].AddFlag(IMFLAG_SEPARATOR_BOTTOM);
            break;
    }
    // Skip Child Stealth - Disabled when Skip Child Zelda is active
    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("SkipChildZelda"), RO_GENERIC_DONT_SKIP)) {
        mOptions[RSK_SKIP_CHILD_STEALTH].Disable("This option is disabled because \"Skip Child Zelda\" is enabled.");
    } else {
        mOptions[RSK_SKIP_CHILD_STEALTH].Enable();
    }
    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("GossipStoneHints"), RO_GOSSIP_STONES_NEED_NOTHING) == RO_GOSSIP_STONES_NONE) {
        mOptions[RSK_HINT_CLARITY].Hide();
        mOptions[RSK_HINT_DISTRIBUTION].Hide();
    } else {
        mOptions[RSK_HINT_CLARITY].Unhide();
        mOptions[RSK_HINT_DISTRIBUTION].Unhide();
    }

    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleWarpSongs"), RO_GENERIC_ON)) {
        mOptions[RSK_WARP_SONG_HINTS].Enable();
    } else {
        mOptions[RSK_WARP_SONG_HINTS].Disable("This option is disabled since warp song locations not shuffled.");
    }

    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleCows"), RO_GENERIC_OFF)) {
        mOptions[RSK_MALON_HINT].Enable();
    } else {
        mOptions[RSK_MALON_HINT].Disable("Malon's hint points to a cow, so requires cows to be shuffled.");
    }

    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("Shuffle100GSReward"), RO_GENERIC_OFF)) {
        mOptions[RSK_KAK_100_SKULLS_HINT].Enable();
    } else {
        mOptions[RSK_KAK_100_SKULLS_HINT].Disable("There is no point to hinting 100 skulls if it is not shuffled");
    }

    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("Fishsanity"), RO_FISHSANITY_OFF) == RO_FISHSANITY_HYRULE_LOACH) {
        mOptions[RSK_LOACH_HINT].Enable();
    } else {
        mOptions[RSK_LOACH_HINT].Disable("Loach hint is only avaliable with \"Fishsanity\" set to \"Shuffle only Hyrule Loach\"\nas that's the only setting where you present the loach to the fishing pond owner");
    }

    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("CuccosToReturn"), 7) == 0) {
        mOptions[RSK_CHICKENS_HINT].Disable("Anju will just give you the item instead with 0 chickens");
    } else {
        mOptions[RSK_CHICKENS_HINT].Enable();
    }
}

void Settings::FinalizeSettings(const std::set<RandomizerCheck>& excludedLocations, const std::set<RandomizerTrick>& enabledTricks) {
    const auto ctx = Rando::Context::GetInstance();
    if (!ctx->IsSpoilerLoaded()) {
        // If we've loaded a spoiler file, the settings have already been populated, so we
        // only need to do things like resolve the starting age or determine MQ dungeons.
        // Any logic dependent on cvarSettings should go in this if statement
        
        // if we skip child zelda, we start with zelda's letter, and malon starts
        // at the ranch, so we should *not* shuffle the weird egg
        if (mOptions[RSK_SKIP_CHILD_ZELDA]) {
            mOptions[RSK_SHUFFLE_WEIRD_EGG].SetSelectedIndex(RO_GENERIC_OFF);
        }

        // With certain access settings, the seed is only beatable if Starting Age is set to Child.
        if (mOptions[RSK_FOREST].Is(RO_FOREST_CLOSED) || (mOptions[RSK_DOOR_OF_TIME].Is(RO_DOOROFTIME_CLOSED) &&
            !mOptions[RSK_SHUFFLE_OCARINA])) {
            mOptions[RSK_STARTING_AGE].SetSelectedIndex(RO_AGE_CHILD);
        }

        if (mOptions[RSK_TRIFORCE_HUNT]) {
            mOptions[RSK_GANONS_BOSS_KEY].SetSelectedIndex(RO_GANON_BOSS_KEY_TRIFORCE_HUNT);
        }

        // Force 100 GS Shuffle if that's where Ganon's Boss Key is
        if (mOptions[RSK_GANONS_BOSS_KEY].Is(RO_GANON_BOSS_KEY_KAK_TOKENS)) {
            mOptions[RSK_SHUFFLE_100_GS_REWARD].SetSelectedIndex(1);
        }

        // If we only have MQ, set all dungeons to MQ
        if (OTRGlobals::Instance->HasMasterQuest() && !OTRGlobals::Instance->HasOriginal()) {
            mOptions[RSK_MQ_DUNGEON_RANDOM].SetSelectedIndex(RO_MQ_DUNGEONS_SET_NUMBER);
            mOptions[RSK_MQ_DUNGEON_COUNT].SetSelectedIndex(12);
            mOptions[RSK_MQ_DUNGEON_SET].SetSelectedIndex(RO_GENERIC_OFF);
        }

        // If we don't have MQ, set all dungeons to Vanilla
        if (OTRGlobals::Instance->HasOriginal() && !OTRGlobals::Instance->HasMasterQuest()) {
            mOptions[RSK_MQ_DUNGEON_RANDOM].SetSelectedIndex(RO_MQ_DUNGEONS_NONE);
        }

        if (mOptions[RSK_MQ_DUNGEON_RANDOM].Is(RO_MQ_DUNGEONS_NONE)) {
            mOptions[RSK_MQ_DUNGEON_COUNT].SetSelectedIndex(0);
            mOptions[RSK_MQ_DUNGEON_SET].SetSelectedIndex(RO_GENERIC_OFF);
        }

        // If any of the individual shuffle settings are on, turn on the main Shuffle Entrances option
        if (mOptions[RSK_SHUFFLE_DUNGEON_ENTRANCES].IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF)
            || mOptions[RSK_SHUFFLE_BOSS_ENTRANCES].IsNot(RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF)
            || mOptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES]
            || mOptions[RSK_SHUFFLE_INTERIOR_ENTRANCES].IsNot(RO_INTERIOR_ENTRANCE_SHUFFLE_OFF)
            || mOptions[RSK_SHUFFLE_GROTTO_ENTRANCES] || mOptions[RSK_SHUFFLE_OWL_DROPS]
            || mOptions[RSK_SHUFFLE_WARP_SONGS] || mOptions[RSK_SHUFFLE_OVERWORLD_SPAWNS]) {
            mOptions[RSK_SHUFFLE_ENTRANCES].SetSelectedIndex(RO_GENERIC_ON);
        } else {
            mOptions[RSK_SHUFFLE_ENTRANCES].SetSelectedIndex(RO_GENERIC_OFF);
        }

        if (mOptions[RSK_SHUFFLE_DUNGEON_REWARDS].Is(RO_DUNGEON_REWARDS_END_OF_DUNGEON)) {
            mOptions[RSK_LINKS_POCKET].SetSelectedIndex(RO_LINKS_POCKET_DUNGEON_REWARD);
        }

        if (!ctx->IsSpoilerLoaded()) {
            ctx->AddExcludedOptions();
            for (const auto locationKey : ctx->everyPossibleLocation) {
                if (const auto location = ctx->GetItemLocation(locationKey); excludedLocations.contains(location->GetRandomizerCheck())) {
                    location->GetExcludedOption()->SetSelectedIndex(1);
                } else {
                    location->GetExcludedOption()->SetSelectedIndex(0);
                }
            }
            // Tricks
            ResetTrickOptions();
            for (const auto randomizerTrick : enabledTricks) {
                mTrickOptions[randomizerTrick].SetSelectedIndex(1);
            }
        }
        if (!mOptions[RSK_SHUFFLE_KOKIRI_SWORD]) {
            if (mOptions[RSK_STARTING_KOKIRI_SWORD]) {
                ctx->GetItemLocation(RC_KF_KOKIRI_SWORD_CHEST)->GetExcludedOption()->SetSelectedIndex(1);
            }
        }
        if (!mOptions[RSK_SHUFFLE_MASTER_SWORD]) {
            if (mOptions[RSK_STARTING_MASTER_SWORD]) {
                ctx->GetItemLocation(RC_MASTER_SWORD_PEDESTAL)->GetExcludedOption()->SetSelectedIndex(1);
            }
        }
        if (!mOptions[RSK_SHUFFLE_OCARINA]) {
            if (mOptions[RSK_STARTING_OCARINA].IsNot(RO_STARTING_OCARINA_OFF)) {
                ctx->GetItemLocation(RC_LW_GIFT_FROM_SARIA)->GetExcludedOption()->SetSelectedIndex(1);
                if (mOptions[RSK_STARTING_OCARINA].Is(RO_STARTING_OCARINA_TIME)) {
                    ctx->GetItemLocation(RC_HF_OCARINA_OF_TIME_ITEM)->GetExcludedOption()->SetSelectedIndex(1);
                }
            }
        }
    }

    // RANDOTODO implement chest shuffle with keysanity
    // ShuffleChestMinigame.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_CHEST_MINIGAME]);
    mOptions[RSK_SHUFFLE_CHEST_MINIGAME].SetSelectedIndex(RO_CHEST_GAME_OFF);
    
    //TODO: RandomizeAllSettings(true) when implementing the ability to randomize the options themselves.
    std::array<DungeonInfo*, 12> dungeons = ctx->GetDungeons()->GetDungeonList();

    //reset the MQ vars
    for (auto dungeon: dungeons) {
        dungeon->ClearMQ();
        dungeon->SetDungeonKnown(true);
    }
    //if it's selection mode, process the selection directly
    if (mOptions[RSK_MQ_DUNGEON_RANDOM].Value<uint8_t>() == RO_MQ_DUNGEONS_SELECTION){
        mOptions[RSK_MQ_DUNGEON_SET].SetSelectedIndex(RO_GENERIC_ON);
        //How many dungeons are set to MQ in selection
        uint8_t mqSet = 0;
        for (auto dungeon: dungeons) {
            switch (mOptions[dungeon->GetMQSetting()].Value<uint8_t>()) {
                case RO_MQ_SET_MQ:
                    dungeon->SetMQ();
                    mqSet += 1;
                    break;
                case RO_MQ_SET_RANDOM:
                    //50% per dungeon, rolled seperatly so people can either have a linear distribtuion
                    //or a bell curve for the number of MQ dungeons per seed.
                    if (Random(0,2)){
                        dungeon->SetMQ();
                        mqSet += 1;
                    }
                    dungeon->SetDungeonKnown(false);
                    break;
                default:
                    break;
            }
        }
        //override the dungeons set with the ones set by selection, so it's accurate for anything that wants to know MQ dungeon count
        mOptions[RSK_MQ_DUNGEON_COUNT].SetSelectedIndex(mqSet);
    //handling set number and random number together
    } else if (mOptions[RSK_MQ_DUNGEON_RANDOM].Value<uint8_t>() != RO_MQ_DUNGEONS_NONE){
        // so we don't have to call this repeatedly
        uint8_t mqCount = mOptions[RSK_MQ_DUNGEON_COUNT].Value<uint8_t>();
        //How many dungeons are set to MQ in selection
        uint8_t mqSet = 0;
        //the number of random 
        uint8_t mqToSet = 0;
        //store the dungeons to randomly decide between. we use the id instead of a dungeon object to avoid a lot of casting.
        std::vector<uint8_t> randMQOption = {};
        //if dungeons have been preset, process them
        if (mOptions[RSK_MQ_DUNGEON_SET]){
            for (size_t i = 0; i < dungeons.size(); i++) {
                switch (mOptions[dungeons[i]->GetMQSetting()].Value<uint8_t>()) {
                case RO_MQ_SET_MQ:
                    dungeons[i]->SetMQ();
                    mqSet += 1;
                    break;
                case RO_MQ_SET_RANDOM:
                    randMQOption.push_back(i);
                    dungeons[i]->SetDungeonKnown(false);
                    break;
                default:
                    break;
                }
            }
        //otherwise, every dungeon is possible
        } else {
            //if the count is fixed to 12, we know everything is MQ, so can skip some setps and do not set Known
            if (mOptions[RSK_MQ_DUNGEON_RANDOM].Value<uint8_t>() == RO_MQ_DUNGEONS_SET_NUMBER &&
                mqCount == 12) {
                randMQOption = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
                for (auto dungeon: dungeons) {
                    mOptions[dungeon->GetMQSetting()].SetSelectedIndex(RO_MQ_SET_MQ);
                }
            //if it's fixed to zero, set it to None instead. the rest is processed after
            } else if (mOptions[RSK_MQ_DUNGEON_RANDOM].Value<uint8_t>() == RO_MQ_DUNGEONS_SET_NUMBER &&
                       mqCount == 0){
                mOptions[RSK_MQ_DUNGEON_RANDOM].SetSelectedIndex(RO_MQ_DUNGEONS_NONE);
            //otherwise, make everything a possibility and unknown
            } else {
                for (size_t i = 0; i < dungeons.size(); i++) {
                    randMQOption.push_back(i);
                    dungeons[i]->SetDungeonKnown(false);
                    mOptions[dungeons[i]->GetMQSetting()].SetSelectedIndex(RO_MQ_SET_RANDOM);
                }
            }
        }
        //if there's no random options, we can skip this
        if (randMQOption.size() > 0){
            //Figure out how many dungeons to select, rolling the random number if needed
            if (mOptions[RSK_MQ_DUNGEON_RANDOM].Is(RO_MQ_DUNGEONS_RANDOM_NUMBER)){
                mqToSet = Random(0, static_cast<int>(randMQOption.size()) + 1);
            } else if (mqCount > mqSet) {
                mqToSet = std::min(mqCount - mqSet, static_cast<int>(randMQOption.size()));
            }
            //we only need to shuffle if we're not using them all
            if (mqToSet <= static_cast<int8_t>(randMQOption.size()) && mqToSet > 0) {
                Shuffle(randMQOption);
            } 
            for (uint8_t i = 0; i < mqToSet; i++){
                dungeons[randMQOption[i]]->SetMQ();
            }
        } else {
            //if there's no random options, check if we can collapse the setting into None or Selection
            if (mqSet == 0){   
                mOptions[RSK_MQ_DUNGEON_RANDOM].SetSelectedIndex(RO_MQ_DUNGEONS_NONE);
            } else {
                mOptions[RSK_MQ_DUNGEON_RANDOM].SetSelectedIndex(RO_MQ_DUNGEONS_SELECTION);
            }
        }
        //reset the value set based on what was actually set
        mOptions[RSK_MQ_DUNGEON_COUNT].SetSelectedIndex(mqToSet + mqSet);
    } 
    //Not an if else as other settings can become None in processing
    if (mOptions[RSK_MQ_DUNGEON_RANDOM].Value<uint8_t>() == RO_MQ_DUNGEONS_NONE) {
        mOptions[RSK_MQ_DUNGEON_SET].SetSelectedIndex(RO_GENERIC_OFF);
        mOptions[RSK_MQ_DUNGEON_COUNT].SetSelectedIndex(0);
        for (auto dungeon: dungeons) {
            mOptions[dungeon->GetMQSetting()].SetSelectedIndex(RO_MQ_SET_VANILLA);
        }
    }

    // Set key ring for each dungeon
    for (const auto dungeon : dungeons) {
        dungeon->ClearKeyRing();
    }

    const std::vector<Option*> keyRingOptions = {
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
        auto keyrings = keyRingOptions;
        if (mOptions[RSK_GERUDO_FORTRESS].Is(RO_GF_NORMAL) && mOptions[RSK_GERUDO_KEYS].IsNot(RO_GERUDO_KEYS_VANILLA)) {
            keyrings.push_back(&mOptions[RSK_KEYRINGS_GERUDO_FORTRESS]);
        } else {
            mOptions[RSK_KEYRINGS_GERUDO_FORTRESS].SetSelectedIndex(RO_KEYRING_FOR_DUNGEON_OFF);
        }
        if (mOptions[RSK_KEYRINGS].Is(RO_KEYRINGS_RANDOM) || mOptions[RSK_KEYRINGS].Is(RO_KEYRINGS_COUNT)) {
            const uint32_t keyRingCount = mOptions[RSK_KEYRINGS].Is(RO_KEYRINGS_COUNT) ? mOptions[RSK_KEYRINGS_RANDOM_COUNT].Value<uint8_t>() : Random(0, static_cast<int>(keyrings.size()));
            Shuffle(keyrings);
            for (size_t i = 0; i < keyRingCount; i++) {
                keyrings[i]->SetSelectedIndex(RO_KEYRING_FOR_DUNGEON_ON);
            }
            for (size_t i = keyRingCount; i < keyrings.size(); i++) {
                keyrings[i]->SetSelectedIndex(RO_KEYRING_FOR_DUNGEON_OFF);
            }
        }
        if (mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL].Is(RO_KEYRING_FOR_DUNGEON_ON) || (mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 1)) {
            ctx->GetDungeon(BOTTOM_OF_THE_WELL)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_FOREST_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_ON) || (mOptions[RSK_KEYRINGS_FOREST_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 1)) {
            ctx->GetDungeon(FOREST_TEMPLE)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_FIRE_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_ON) || (mOptions[RSK_KEYRINGS_FIRE_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 1)) {
            ctx->GetDungeon(FIRE_TEMPLE)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_WATER_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_ON) || (mOptions[RSK_KEYRINGS_WATER_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 1)) {
            ctx->GetDungeon(WATER_TEMPLE)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_ON) || (mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 1)) {
            ctx->GetDungeon(SPIRIT_TEMPLE)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_SHADOW_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_ON) || (mOptions[RSK_KEYRINGS_SHADOW_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 1)) {
            ctx->GetDungeon(SHADOW_TEMPLE)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_GTG].Is(RO_KEYRING_FOR_DUNGEON_ON) || (mOptions[RSK_KEYRINGS_GTG].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 1)) {
            ctx->GetDungeon(GERUDO_TRAINING_GROUNDS)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_GANONS_CASTLE].Is(RO_KEYRING_FOR_DUNGEON_ON) || (mOptions[RSK_KEYRINGS_GANONS_CASTLE].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 1)) {
            ctx->GetDungeon(GANONS_CASTLE)->SetKeyRing();
        }
    }

    auto trials = ctx->GetTrials()->GetTrialList();
    Shuffle(trials);
    for (const auto trial : trials) {
        trial->SetAsSkipped();
    }
    if(mOptions[RSK_GANONS_TRIALS].Is(RO_GANONS_TRIALS_SKIP)){
        mOptions[RSK_TRIAL_COUNT].SetSelectedIndex(0);
    } else if(mOptions[RSK_GANONS_TRIALS].Is(RO_GANONS_TRIALS_RANDOM_NUMBER)) {
        mOptions[RSK_TRIAL_COUNT].SetSelectedIndex(Random(0, static_cast<int>(mOptions[RSK_TRIAL_COUNT].GetOptionCount())));
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
        if (const uint32_t choice = Random(0, 2); choice == 0) {
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

    if (!mOptions[RSK_SHUFFLE_WARP_SONGS]) {
        mOptions[RSK_WARP_SONG_HINTS].SetSelectedIndex(RO_GENERIC_OFF);
    }

    if (!mOptions[RSK_SHUFFLE_COWS]) {
        mOptions[RSK_MALON_HINT].SetSelectedIndex(RO_GENERIC_OFF);
    }

    if (!mOptions[RSK_SHUFFLE_100_GS_REWARD]) {
        mOptions[RSK_KAK_100_SKULLS_HINT].SetSelectedIndex(RO_GENERIC_OFF);
    }

    if (!mOptions[RSK_SHUFFLE_FISHING_POLE]) {
        mOptions[RSK_FISHING_POLE_HINT].SetSelectedIndex(RO_GENERIC_OFF);
    }

    if (mOptions[RSK_FISHSANITY].IsNot(RO_FISHSANITY_HYRULE_LOACH)) {
        mOptions[RSK_LOACH_HINT].SetSelectedIndex(RO_GENERIC_OFF);
    }

    if (mOptions[RSK_CUCCO_COUNT].Is(0)) {
        mOptions[RSK_CHICKENS_HINT].SetSelectedIndex(RO_GENERIC_OFF);
    }
}
void Settings::ParseJson(nlohmann::json spoilerFileJson) {
    mSeedString = spoilerFileJson["seed"].get<std::string>();
    mFinalSeed = spoilerFileJson["finalSeed"].get<uint32_t>();
    nlohmann::json settingsJson = spoilerFileJson["settings"];
    for (auto it = settingsJson.begin(); it != settingsJson.end(); ++it) {
        // todo load into cvars for UI

        if (mSpoilerfileSettingNameToEnum.contains(it.key())) {
            std::string numericValueString;
            // this is annoying but the same strings are used in different orders
            // and i don't want the spoilerfile to just have numbers instead of
            // human readable settings values so it'll have to do for now
            switch (const RandomizerSettingKey index = mSpoilerfileSettingNameToEnum[it.key()]) {
                case RSK_LOGIC_RULES:
                    if (it.value() == "Glitchless") {
                        mOptions[index].SetSelectedIndex(RO_LOGIC_GLITCHLESS);
                    } else if (it.value() == "No Logic") {
                        mOptions[index].SetSelectedIndex(RO_LOGIC_NO_LOGIC);
                    } else if (it.value() == "Vanilla") {
                        mOptions[index].SetSelectedIndex(RO_LOGIC_VANILLA);
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
                case RSK_CUCCO_COUNT:
                case RSK_FISHSANITY_POND_COUNT:
                case RSK_STARTING_SKULLTULA_TOKEN:
                case RSK_SHOPSANITY_COUNT:
                    numericValueString = it.value();
                    mOptions[index].SetSelectedIndex(std::stoi(numericValueString));
                    break;
                // Same as the above section, but the indexes are off by one from the text
                // (i.e. 10 Big Poes is index 9).
                case RSK_BIG_POE_COUNT:
                case RSK_TRIFORCE_HUNT_PIECES_TOTAL:
                case RSK_TRIFORCE_HUNT_PIECES_REQUIRED:
                case RSK_STARTING_HEARTS:
                    numericValueString = it.value();
                    mOptions[index].SetSelectedIndex(std::stoi(numericValueString) - 1);
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
                    } else if (it.value() == "Specific Count") {
                        mOptions[index].SetSelectedIndex(RO_SHOPSANITY_SPECIFIC_COUNT);
                    } else if (it.value() == "Random") {
                        mOptions[index].SetSelectedIndex(RO_SHOPSANITY_RANDOM);
                    }
                    break;
                case RSK_SHOPSANITY_PRICES:
                case RSK_SCRUBS_PRICES:
                case RSK_MERCHANT_PRICES:
                    if (it.value() == "Vanilla") {
                        mOptions[index].SetSelectedIndex(RO_PRICE_VANILLA);
                    } else if (it.value() == "Cheap Balanced") {
                        mOptions[index].SetSelectedIndex(RO_PRICE_CHEAP_BALANCED);
                    } else if (it.value() == "Balanced") {
                        mOptions[index].SetSelectedIndex(RO_PRICE_BALANCED);
                    } else if (it.value() == "Fixed") {
                        mOptions[index].SetSelectedIndex(RO_PRICE_FIXED);
                    } else if (it.value() == "Range") {
                        mOptions[index].SetSelectedIndex(RO_PRICE_RANGE);
                    } else if (it.value() == "Set By Wallet") {
                        mOptions[index].SetSelectedIndex(RO_PRICE_SET_BY_WALLET);
                    }
                    break;
                case RSK_SHUFFLE_SCRUBS:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_SCRUBS_OFF);
                    } else if (it.value() == "Major Items Only") {
                        mOptions[index].SetSelectedIndex(RO_SCRUBS_MAJOR_ONLY);
                    } else if (it.value() == "All") {
                        mOptions[index].SetSelectedIndex(RO_SCRUBS_ALL);
                    }
                    break;
                case RSK_SHUFFLE_FISHING_POLE:
                case RSK_FISHSANITY_AGE_SPLIT:
                case RSK_FISHING_POLE_HINT:
                case RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD:
                case RSK_SHUFFLE_BEEHIVES:
                case RSK_SHUFFLE_COWS:
                case RSK_SHUFFLE_ADULT_TRADE:
                case RSK_SHUFFLE_KOKIRI_SWORD:
                case RSK_SHUFFLE_WEIRD_EGG:
                case RSK_SHUFFLE_FROG_SONG_RUPEES:
                case RSK_SHUFFLE_100_GS_REWARD:
                case RSK_SHUFFLE_OCARINA:
                case RSK_SHUFFLE_OCARINA_BUTTONS:
                case RSK_SHUFFLE_SWIM:
                case RSK_SHUFFLE_CHILD_WALLET:
                case RSK_INCLUDE_TYCOON_WALLET:
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
                case RSK_INFINITE_UPGRADES:
                case RSK_SKELETON_KEY:
                case RSK_BOMBCHUS_IN_LOGIC:
                case RSK_TOT_ALTAR_HINT:
                case RSK_GANONDORF_HINT:
                case RSK_SHEIK_LA_HINT:
                case RSK_DAMPES_DIARY_HINT:
                case RSK_GREG_HINT:
                case RSK_LOACH_HINT:
                case RSK_SARIA_HINT:
                case RSK_FROGS_HINT:
                case RSK_OOT_HINT:
                case RSK_KAK_10_SKULLS_HINT:
                case RSK_KAK_20_SKULLS_HINT:
                case RSK_KAK_30_SKULLS_HINT:
                case RSK_KAK_40_SKULLS_HINT:
                case RSK_KAK_50_SKULLS_HINT:
                case RSK_KAK_100_SKULLS_HINT:
                case RSK_MASK_SHOP_HINT:
                case RSK_BIGGORON_HINT:
                case RSK_BIG_POES_HINT:
                case RSK_CHICKENS_HINT:
                case RSK_MALON_HINT:
                case RSK_HBA_HINT:
                case RSK_WARP_SONG_HINTS:
                case RSK_SCRUB_TEXT_HINT:
                case RSK_MERCHANT_TEXT_HINT:
                case RSK_SHUFFLE_ENTRANCES:
                case RSK_SHUFFLE_OVERWORLD_ENTRANCES:
                case RSK_SHUFFLE_GROTTO_ENTRANCES:
                case RSK_SHUFFLE_OWL_DROPS:
                case RSK_SHUFFLE_WARP_SONGS:
                case RSK_SHUFFLE_OVERWORLD_SPAWNS:
                case RSK_MIXED_ENTRANCE_POOLS:
                case RSK_MIX_DUNGEON_ENTRANCES:
                case RSK_MIX_BOSS_ENTRANCES:
                case RSK_MIX_OVERWORLD_ENTRANCES:
                case RSK_MIX_INTERIOR_ENTRANCES:
                case RSK_MIX_GROTTO_ENTRANCES:
                case RSK_DECOUPLED_ENTRANCES:
                case RSK_SHOPSANITY_PRICES_AFFORDABLE:
                case RSK_SCRUBS_PRICES_AFFORDABLE:
                case RSK_MERCHANT_PRICES_AFFORDABLE:
                case RSK_ALL_LOCATIONS_REACHABLE:
                case RSK_TRIFORCE_HUNT:
                case RSK_MQ_DUNGEON_SET:
                case RSK_SHUFFLE_DEKU_NUT_BAG:
                case RSK_SHUFFLE_DEKU_STICK_BAG:
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
                case RSK_KEYRINGS_GERUDO_FORTRESS:
                case RSK_KEYRINGS_FOREST_TEMPLE:
                case RSK_KEYRINGS_FIRE_TEMPLE:
                case RSK_KEYRINGS_WATER_TEMPLE:
                case RSK_KEYRINGS_SHADOW_TEMPLE:
                case RSK_KEYRINGS_SPIRIT_TEMPLE:
                case RSK_KEYRINGS_BOTTOM_OF_THE_WELL:
                case RSK_KEYRINGS_GTG:
                case RSK_KEYRINGS_GANONS_CASTLE:
                    if (it.value() == "No") {
                        mOptions[index].SetSelectedIndex(RO_KEYRING_FOR_DUNGEON_OFF);
                    } else if (it.value() == "Random") {
                        mOptions[index].SetSelectedIndex(RO_KEYRING_FOR_DUNGEON_RANDOM);
                    } else if (it.value() == "Yes") {
                        mOptions[index].SetSelectedIndex(RO_KEYRING_FOR_DUNGEON_ON);
                    }
                    break;
                case RSK_SHUFFLE_MERCHANTS:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_SHUFFLE_MERCHANTS_OFF);
                    } else if (it.value() == "Beans Only") {
                        mOptions[index].SetSelectedIndex(RO_SHUFFLE_MERCHANTS_BEANS_ONLY);
                    } else if (it.value() == "All but Beans") {
                        mOptions[index].SetSelectedIndex(RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS);
                    } else if (it.value() == "All") {
                        mOptions[index].SetSelectedIndex(RO_SHUFFLE_MERCHANTS_ALL);
                    }
                    break;
                // Uses Ammo Drops option for now. "Off" not yet implemented
                // TODO: Change to Ammo Drops
                case RSK_ENABLE_BOMBCHU_DROPS:
                    if (it.value() == "Yes") {
                        mOptions[index].SetSelectedIndex(RO_AMMO_DROPS_ON);
                    // } else if (it.value() == "On + Bombchu") {
                    //     mOptions[index].SetSelectedIndex(RO_AMMO_DROPS_ON_PLUS_BOMBCHU);
                    } else if (it.value() == "No") {
                        mOptions[index].SetSelectedIndex(RO_AMMO_DROPS_OFF);
                    }
                    break;
                case RSK_FISHSANITY:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_FISHSANITY_OFF);
                    } else if (it.value() == "Shuffle Fishing Pond") {
                        mOptions[index].SetSelectedIndex(RO_FISHSANITY_POND);
                    } else if (it.value() == "Shuffle Overworld Fish") {
                        mOptions[index].SetSelectedIndex(RO_FISHSANITY_OVERWORLD);
                    } else if (it.value() == "Shuffle Both") {
                        mOptions[index].SetSelectedIndex(RO_FISHSANITY_BOTH);
                    }
                    break;
                case RSK_SHUFFLE_BOSS_SOULS:
                    if (it.value() == "Off") {
                        mOptions[index].SetSelectedIndex(RO_BOSS_SOULS_OFF);
                    } else if (it.value() == "On") {
                        mOptions[index].SetSelectedIndex(RO_BOSS_SOULS_ON);
                    } else if (it.value() == "On + Ganon") {
                        mOptions[index].SetSelectedIndex(RO_BOSS_SOULS_ON_PLUS_GANON);
                    }
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
    const auto ctx = Context::GetInstance();

    ctx->AddExcludedOptions();
    for (auto it = jsonExcludedLocations.begin(); it != jsonExcludedLocations.end(); ++it) {
        const RandomizerCheck rc = Rando::StaticData::locationNameToEnum[it.value()];
        ctx->GetItemLocation(rc)->GetExcludedOption()->SetSelectedIndex(RO_GENERIC_ON);
    }

    nlohmann::json enabledTricksJson = spoilerFileJson["enabledTricks"];
    for (auto it = enabledTricksJson.begin(); it != enabledTricksJson.end(); ++it) {
        const RandomizerTrick rt = mTrickNameToEnum[it.value()];
        GetTrickOption(rt).SetSelectedIndex(RO_GENERIC_ON);
    }
}

void Settings::ReloadOptions() {
    for (int i = 0; i < RSK_MAX; i++) {
        mOptions[i].SetFromCVar();
    }
}
} // namespace Rando
