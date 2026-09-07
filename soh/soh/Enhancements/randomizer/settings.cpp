#include <spdlog/spdlog.h>
#include <libultraship/bridge/consolevariablebridge.h>
#include <ship/Context.h>
#include <ship/window/Window.h>

#include "settings.h"
#include "trial.h"
#include "dungeon.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/rng.h"
#include "soh/OTRGlobals.h"

namespace Rando {
std::shared_ptr<Settings> Settings::mInstance;

std::vector<std::string> NumOpts(const int min, const int max, const int step = 1, const std::string& textBefore = {},
                                 const std::string& textAfter = {}) {
    std::vector<std::string> options;
    options.reserve((max - min) / step + 1);
    for (int i = min; i <= max; i += step) {
        options.push_back(textBefore + std::to_string(i) += textAfter);
    }
    return options;
}

std::vector<std::string> MultiVecOpts(const std::vector<std::vector<std::string>>& optionsVector) {
    size_t totalSize = 0;
    for (const auto& vector : optionsVector) {
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

void Settings::HandleShopsanityPriceUI() {
    bool isTycoon = CVarGetInteger(CVAR_RANDOMIZER_SETTING("IncludeTycoonWallet"), RO_GENERIC_OFF);
    mOptions[RSK_SHOPSANITY].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
    mOptions[RSK_SHOPSANITY_PRICES].Unhide();
    switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShopsanityPrices"), RO_PRICE_VANILLA)) {
        case RO_PRICE_FIXED:
            mOptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE].Unhide();
            mOptions[RSK_SHOPSANITY_PRICES_RANGE_1].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_RANGE_2].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_NO_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_CHILD_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_ADULT_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_GIANT_WALLET_WEIGHT].Hide();
            mOptions[RSK_SHOPSANITY_PRICES_TYCOON_WALLET_WEIGHT].Hide();
            if (isTycoon ? mOptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE].GetOptionCount() == 501
                         : mOptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE].GetOptionCount() == 1000) {
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
            if (isTycoon ? mOptions[RSK_SHOPSANITY_PRICES_RANGE_1].GetOptionCount() == 101
                         : mOptions[RSK_SHOPSANITY_PRICES_RANGE_1].GetOptionCount() == 200) {
                mOptions[RSK_SHOPSANITY_PRICES_RANGE_1].ChangeOptions(isTycoon ? NumOpts(0, 995, 5)
                                                                               : NumOpts(0, 500, 5));
                mOptions[RSK_SHOPSANITY_PRICES_RANGE_2].ChangeOptions(isTycoon ? NumOpts(0, 995, 5)
                                                                               : NumOpts(0, 500, 5));
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
            if (isTycoon) {
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

#define OPT_U8(rsk, ...) mOptions[rsk] = Option::U8(rsk, __VA_ARGS__)
#define OPT_BOOL(rsk, ...) mOptions[rsk] = Option::Bool(rsk, __VA_ARGS__)
#define OPT_TRICK(rsk, ...) mTrickSettings[rsk] = TrickSetting::LogicTrick(rsk, __VA_ARGS__)
// All callbacks will be called once when the widget is Added (on boot, essentially) and
// once when the widget is interacted with such that the value was changed.
#define OPT_CALLBACK(rsk, body) mOptions[rsk].SetCallback([this](WidgetInfo & info) body)
#define OPT_CALLBACK_FN(rsk, fn) mOptions[rsk].SetCallback(fn)

void Settings::HandleMixedEntrancePoolsUI() {
    bool dungeonShuffle =
        CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleDungeonsEntrances"), RO_DUNGEON_ENTRANCE_SHUFFLE_OFF);
    bool bossShuffle =
        CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleBossEntrances"), RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF);
    bool overworldShuffle = CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleOverworldEntrances"), RO_GENERIC_OFF);
    bool interiorShuffle = CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleInteriorsEntrances"), RO_GENERIC_OFF);
    bool grottoShuffle = CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleGrottosEntrances"), RO_GENERIC_OFF);
    bool thievesHideoutShuffle =
        CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleThievesHideoutEntrances"), RO_GENERIC_OFF);

    // Hide Mixed Entrances option if 1 or no applicable entrance shuffles are visible
    if (dungeonShuffle + bossShuffle + overworldShuffle + interiorShuffle + grottoShuffle + thievesHideoutShuffle <=
        1) {
        mOptions[RSK_MIXED_ENTRANCE_POOLS].Hide();
    } else {
        mOptions[RSK_MIXED_ENTRANCE_POOLS].Unhide();
    }
}

void Settings::HandleKeyringUI() {
    const bool fortressKeyring =
        CVarGetInteger(CVAR_RANDOMIZER_SETTING("FortressCarpenters"), RO_GF_CARPENTERS_NORMAL) ==
            RO_GF_CARPENTERS_NORMAL &&
        CVarGetInteger(CVAR_RANDOMIZER_SETTING("GerudoKeys"), RO_GERUDO_KEYS_VANILLA) != RO_GERUDO_KEYS_VANILLA;
    const bool chestGameKeyring =
        CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleChestMinigame"), RO_GENERIC_OFF) != RO_GENERIC_OFF;
    const uint8_t maxKeyringCount = 8 + fortressKeyring + chestGameKeyring;
    if (mOptions[RSK_KEYRINGS_RANDOM_COUNT].GetOptionCount() != static_cast<size_t>(maxKeyringCount) + 1) {
        mOptions[RSK_KEYRINGS_RANDOM_COUNT].ChangeOptions(NumOpts(0, maxKeyringCount));
    }
    if (fortressKeyring) {
        mOptions[RSK_KEYRINGS_GERUDO_FORTRESS].Enable();
    } else {
        mOptions[RSK_KEYRINGS_GERUDO_FORTRESS].Disable(
            "Disabled because the currently selected Gerudo Fortress Carpenters\n"
            "setting and/or Gerudo Fortress Keys setting is incompatible with\n"
            "having a Gerudo Fortress Keyring.");
    }
    if (chestGameKeyring) {
        mOptions[RSK_KEYRINGS_CHEST_GAME].Enable();
    } else {
        mOptions[RSK_KEYRINGS_CHEST_GAME].Disable("Disabled because Shuffle Chest Minigame is off.");
    }
}

void Settings::HandleStartingAgeUI() {
    // Starting Age - Disabled under very specific conditions unless it's No Logic
    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("LogicRules"), RO_LOGIC_GLITCHLESS) != RO_LOGIC_NO_LOGIC &&
        // If Closed DoT requires OoT then we can only start as child
        ((CVarGetInteger(CVAR_RANDOMIZER_SETTING("DoorOfTime"), RO_DOOROFTIME_CLOSED) == RO_DOOROFTIME_CLOSED &&
          CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleOcarinas"), RO_GENERIC_OFF) == RO_GENERIC_OFF) ||
         // If Forest is Closed, we cannot start as Adult unless there's a sphere 0 entrance shuffle in Kokiri forest,
         // or there's random spawns, as the player may saveload as child and get stuck.
         // Grottos only lead somewhere if decoupled
         (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ClosedForest"), RO_CLOSED_FOREST_ON) == RO_CLOSED_FOREST_ON &&
          CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleOverworldSpawns"), RO_GENERIC_OFF) == RO_GENERIC_OFF &&
          (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleGrottosEntrances"), RO_GENERIC_OFF) == RO_GENERIC_OFF ||
           CVarGetInteger(CVAR_RANDOMIZER_SETTING("DecoupleEntrances"), RO_GENERIC_OFF) == RO_GENERIC_OFF) &&
          CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleInteriorsEntrances"), RO_GENERIC_OFF) == RO_GENERIC_OFF &&
          CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleOverworldEntrances"), RO_GENERIC_OFF) == RO_GENERIC_OFF))) {
        mOptions[RSK_STARTING_AGE].Disable("This option is disabled due to other options making the game unbeatable.");
    } else {
        mOptions[RSK_STARTING_AGE].Enable();
    }
}

void Settings::CreateOptions() {
    // clang-format off
    OPT_U8(RSK_FOREST, {"On", "Deku Only", "Off"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ClosedForest"), WIDGET_CVAR_COMBOBOX, RO_CLOSED_FOREST_ON);
    OPT_CALLBACK(RSK_FOREST, {
        HandleStartingAgeUI();
    });
    OPT_U8(RSK_DOOR_OF_TIME, {"Closed", "Song only", "Open"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("DoorOfTime"), WIDGET_CVAR_COMBOBOX);
    OPT_CALLBACK(RSK_DOOR_OF_TIME, {
        HandleStartingAgeUI();
    });
    OPT_U8(RSK_ZORAS_FOUNTAIN, {"Closed", "Closed as child", "Open"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ZorasFountain"));
    OPT_U8(RSK_SLEEPING_WATERFALL, {"Closed", "Open"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("SleepingWaterfall"));
    OPT_U8(RSK_JABU_OPEN, {"Closed", "Open"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("JabuJabu"));
    OPT_BOOL(RSK_LOCK_OVERWORLD_DOORS, CVAR_RANDOMIZER_SETTING("LockOverworldDoors"));
    OPT_U8(RSK_GERUDO_FORTRESS, {"Normal", "Fast", "Free"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("FortressCarpenters"));
    OPT_CALLBACK(RSK_GERUDO_FORTRESS, {
        HandleKeyringUI();
    });
    OPT_U8(RSK_RAINBOW_BRIDGE, {"Vanilla", "Always open", "Stones", "Medallions", "Dungeon rewards", "Dungeons", "Tokens", "Triforce Pieces", "Greg"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("RainbowBridge"), WIDGET_CVAR_COMBOBOX, RO_BRIDGE_VANILLA, false, nullptr, IMFLAG_NONE);
    OPT_CALLBACK(RSK_RAINBOW_BRIDGE, {
        mOptions[RSK_BRIDGE_OPTIONS].Hide();
        mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT].Hide();
        mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT].Hide();
        mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT].Hide();
        mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT].Hide();
        mOptions[RSK_RAINBOW_BRIDGE_TOKEN_COUNT].Hide();
        mOptions[RSK_RAINBOW_BRIDGE_TRIFORCE_COUNT].Hide();
        switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("RainbowBridge"), RO_BRIDGE_VANILLA)) {
            case RO_BRIDGE_STONES:
                mOptions[RSK_RAINBOW_BRIDGE].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
                mOptions[RSK_BRIDGE_OPTIONS].Unhide();
                mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT].Unhide();
                break;
            case RO_BRIDGE_MEDALLIONS:
                mOptions[RSK_RAINBOW_BRIDGE].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
                mOptions[RSK_BRIDGE_OPTIONS].Unhide();
                mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT].Unhide();
                break;
            case RO_BRIDGE_DUNGEON_REWARDS:
                mOptions[RSK_RAINBOW_BRIDGE].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
                mOptions[RSK_BRIDGE_OPTIONS].Unhide();
                mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT].Unhide();
                break;
            case RO_BRIDGE_DUNGEONS:
                mOptions[RSK_RAINBOW_BRIDGE].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
                mOptions[RSK_BRIDGE_OPTIONS].Unhide();
                mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT].Unhide();
                break;
            case RO_BRIDGE_TOKENS:
                mOptions[RSK_RAINBOW_BRIDGE].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
                mOptions[RSK_BRIDGE_OPTIONS].Hide();
                mOptions[RSK_RAINBOW_BRIDGE_TOKEN_COUNT].Unhide();
                break;
            case RO_BRIDGE_TRIFORCE_PIECES:
                mOptions[RSK_RAINBOW_BRIDGE].RemoveFlag(IMFLAG_SEPARATOR_BOTTOM);
                mOptions[RSK_BRIDGE_OPTIONS].Hide();
                mOptions[RSK_RAINBOW_BRIDGE_TRIFORCE_COUNT].Unhide();
                break;
            default:
                break;
        }
    });
    OPT_U8(RSK_RAINBOW_BRIDGE_STONE_COUNT, {NumOpts(0, 4)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StoneCount"), WIDGET_CVAR_SLIDER_INT, 3, true);
    OPT_U8(RSK_RAINBOW_BRIDGE_MEDALLION_COUNT, {NumOpts(0, 7)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MedallionCount"), WIDGET_CVAR_SLIDER_INT, 6, true);
    OPT_U8(RSK_RAINBOW_BRIDGE_REWARD_COUNT, {NumOpts(0, 10)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("RewardCount"), WIDGET_CVAR_SLIDER_INT, 9, true);
    OPT_U8(RSK_RAINBOW_BRIDGE_DUNGEON_COUNT, {NumOpts(0, 9)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("DungeonCount"), WIDGET_CVAR_SLIDER_INT, 8, true);
    OPT_U8(RSK_RAINBOW_BRIDGE_TOKEN_COUNT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("TokenCount"), WIDGET_CVAR_SLIDER_INT, 100, true);
    OPT_U8(RSK_RAINBOW_BRIDGE_TRIFORCE_COUNT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("TriforcePieceCount"), WIDGET_CVAR_SLIDER_INT, 100, true);
    OPT_U8(RSK_BRIDGE_OPTIONS, {"Standard Rewards", "Greg as Reward", "Greg as Wildcard"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("BridgeRewardOptions"), WIDGET_CVAR_COMBOBOX, RO_BRIDGE_STANDARD_REWARD, false, nullptr, IMFLAG_NONE);
    OPT_CALLBACK(RSK_BRIDGE_OPTIONS, {
        const uint8_t bridgeOpt = CVarGetInteger(CVAR_RANDOMIZER_SETTING("BridgeRewardOptions"), RO_BRIDGE_STANDARD_REWARD);
        if (bridgeOpt == RO_BRIDGE_GREG_REWARD) {
            mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT].ChangeOptions(NumOpts(0, 4));
            mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT].ChangeOptions(NumOpts(0, 7));
            mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT].ChangeOptions(NumOpts(0, 10));
            mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT].ChangeOptions(NumOpts(0, 9));
        } else {
            mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT].ChangeOptions(NumOpts(0, 3));
            mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT].ChangeOptions(NumOpts(0, 6));
            mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT].ChangeOptions(NumOpts(0, 9));
            mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT].ChangeOptions(NumOpts(0, 8));
        }
    });
    OPT_U8(RSK_GANONS_TRIALS, {"Skip", "Set Number", "Random Number"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GanonTrial"), WIDGET_CVAR_COMBOBOX, RO_GANONS_TRIALS_SET_NUMBER);
    OPT_CALLBACK(RSK_GANONS_TRIALS, {
        // Only show the trial count slider if Trials is set to Set Number
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("GanonTrial"), RO_GANONS_TRIALS_SET_NUMBER) ==
            RO_GANONS_TRIALS_SET_NUMBER) {
            mOptions[RSK_TRIAL_COUNT].Unhide();
        } else {
            mOptions[RSK_TRIAL_COUNT].Hide();
        }
    });
    OPT_U8(RSK_TRIAL_COUNT, {NumOpts(0, 6)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GanonTrialCount"), WIDGET_CVAR_SLIDER_INT, 6, true);
    OPT_BOOL(RSK_MEDALLION_LOCKED_TRIALS, CVAR_RANDOMIZER_SETTING("MedallionLockedTrials"));
    OPT_U8(RSK_STARTING_AGE, {"Child", "Adult", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingAge"), WIDGET_CVAR_COMBOBOX, RO_AGE_CHILD);
    OPT_U8(RSK_SELECTED_STARTING_AGE, {"Child", "Adult"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("SelectedStartingAge"), WIDGET_CVAR_COMBOBOX, RO_AGE_CHILD);
    OPT_BOOL(RSK_SHUFFLE_ENTRANCES, "");
    OPT_U8(RSK_SHUFFLE_DUNGEON_ENTRANCES, {"Off", "On", "On + Ganon"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleDungeonsEntrances"), WIDGET_CVAR_COMBOBOX, RO_DUNGEON_ENTRANCE_SHUFFLE_OFF);
    OPT_CALLBACK(RSK_SHUFFLE_DUNGEON_ENTRANCES, {
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleDungeonsEntrances"), RO_DUNGEON_ENTRANCE_SHUFFLE_OFF) ==
            RO_DUNGEON_ENTRANCE_SHUFFLE_OFF ||
            CVarGetInteger(CVAR_RANDOMIZER_SETTING("MixedEntrances"), RO_GENERIC_OFF) == RO_GENERIC_OFF) {
            mOptions[RSK_MIX_DUNGEON_ENTRANCES].Hide();
        } else {
            mOptions[RSK_MIX_DUNGEON_ENTRANCES].Unhide();
        }
    });
    OPT_U8(RSK_SHUFFLE_BOSS_ENTRANCES, {"Off", "Age Restricted", "Full"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleBossEntrances"), WIDGET_CVAR_COMBOBOX, RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF);
    OPT_CALLBACK(RSK_SHUFFLE_BOSS_ENTRANCES, {
        HandleMixedEntrancePoolsUI();

        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleBossEntrances"), RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF) == RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF) {
            mOptions[RSK_SHUFFLE_GANONS_TOWER_ENTRANCE].Hide();
        } else {
            mOptions[RSK_SHUFFLE_GANONS_TOWER_ENTRANCE].Unhide();
        }

        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleBossEntrances"), RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF) ==
            RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF ||
            CVarGetInteger(CVAR_RANDOMIZER_SETTING("MixedEntrances"), RO_GENERIC_OFF) == RO_GENERIC_OFF) {
            mOptions[RSK_MIX_BOSS_ENTRANCES].Hide();
        } else {
            mOptions[RSK_MIX_BOSS_ENTRANCES].Unhide();
        }
    });
    OPT_BOOL(RSK_SHUFFLE_GANONS_TOWER_ENTRANCE, CVAR_RANDOMIZER_SETTING("ShuffleGanonTowerEntrance"));
    OPT_BOOL(RSK_SHUFFLE_OVERWORLD_ENTRANCES, CVAR_RANDOMIZER_SETTING("ShuffleOverworldEntrances"));
    OPT_CALLBACK(RSK_SHUFFLE_OVERWORLD_ENTRANCES, {
        HandleMixedEntrancePoolsUI();

        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleOverworldEntrances"), RO_GENERIC_OFF) == RO_GENERIC_OFF ||
            CVarGetInteger(CVAR_RANDOMIZER_SETTING("MixedEntrances"), RO_GENERIC_OFF) == RO_GENERIC_OFF) {
            mOptions[RSK_MIX_OVERWORLD_ENTRANCES].Hide();
        } else {
            mOptions[RSK_MIX_OVERWORLD_ENTRANCES].Unhide();
        }

        HandleStartingAgeUI();
    });
    OPT_U8(RSK_SHUFFLE_INTERIOR_ENTRANCES, {"Off", "Simple", "All"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleInteriorsEntrances"), WIDGET_CVAR_COMBOBOX, RO_INTERIOR_ENTRANCE_SHUFFLE_OFF);
    OPT_CALLBACK(RSK_SHUFFLE_INTERIOR_ENTRANCES, {
        HandleMixedEntrancePoolsUI();

        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleInteriorsEntrances"), RO_GENERIC_OFF) == RO_GENERIC_OFF ||
            CVarGetInteger(CVAR_RANDOMIZER_SETTING("MixedEntrances"), RO_GENERIC_OFF) == RO_GENERIC_OFF) {
            mOptions[RSK_MIX_INTERIOR_ENTRANCES].Hide();
        } else {
            mOptions[RSK_MIX_INTERIOR_ENTRANCES].Unhide();
        }

        HandleStartingAgeUI();
    });
    OPT_BOOL(RSK_SHUFFLE_THIEVES_HIDEOUT_ENTRANCES, CVAR_RANDOMIZER_SETTING("ShuffleThievesHideoutEntrances"));
    OPT_CALLBACK(RSK_SHUFFLE_THIEVES_HIDEOUT_ENTRANCES, {
        HandleMixedEntrancePoolsUI();

        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleThievesHideoutEntrances"), RO_GENERIC_OFF) ==
            RO_GENERIC_OFF ||
            CVarGetInteger(CVAR_RANDOMIZER_SETTING("MixedEntrances"), RO_GENERIC_OFF) == RO_GENERIC_OFF) {
            mOptions[RSK_MIX_THIEVES_HIDEOUT_ENTRANCES].Hide();
        } else {
            mOptions[RSK_MIX_THIEVES_HIDEOUT_ENTRANCES].Unhide();
        }
    });
    OPT_BOOL(RSK_SHUFFLE_GROTTO_ENTRANCES, CVAR_RANDOMIZER_SETTING("ShuffleGrottosEntrances"));
    OPT_CALLBACK(RSK_SHUFFLE_GROTTO_ENTRANCES, {
        HandleMixedEntrancePoolsUI();

        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleGrottosEntrances"), RO_GENERIC_OFF) == RO_GENERIC_OFF ||
            CVarGetInteger(CVAR_RANDOMIZER_SETTING("MixedEntrances"), RO_GENERIC_OFF) == RO_GENERIC_OFF) {
            mOptions[RSK_MIX_GROTTO_ENTRANCES].Hide();
        } else {
            mOptions[RSK_MIX_GROTTO_ENTRANCES].Unhide();
        }

        HandleStartingAgeUI();
    });
    OPT_BOOL(RSK_SHUFFLE_OWL_DROPS, CVAR_RANDOMIZER_SETTING("ShuffleOwlDrops"));
    OPT_BOOL(RSK_SHUFFLE_WARP_SONGS, CVAR_RANDOMIZER_SETTING("ShuffleWarpSongs"));
    OPT_CALLBACK(RSK_SHUFFLE_WARP_SONGS, {
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleWarpSongs"), RO_GENERIC_ON)) {
            mOptions[RSK_WARP_SONG_HINTS].Enable();
        } else {
            mOptions[RSK_WARP_SONG_HINTS].Disable("This option is disabled since warp song locations are not shuffled.");
        }
    });
    OPT_BOOL(RSK_SHUFFLE_OVERWORLD_SPAWNS, CVAR_RANDOMIZER_SETTING("ShuffleOverworldSpawns"));
    OPT_CALLBACK(RSK_SHUFFLE_OVERWORLD_SPAWNS, {
        HandleStartingAgeUI();
    });
    OPT_BOOL(RSK_MIXED_ENTRANCE_POOLS, CVAR_RANDOMIZER_SETTING("MixedEntrances"));
    OPT_CALLBACK(RSK_MIXED_ENTRANCE_POOLS, {
        // Show mixed entrance pool options if mixed entrance pools are enabled, but only the ones that aren't off
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("MixedEntrances"), RO_GENERIC_OFF) == RO_GENERIC_OFF ||
            mOptions[RSK_MIXED_ENTRANCE_POOLS].IsHidden()) {
            mOptions[RSK_MIXED_ENTRANCE_POOLS].AddFlag(IMFLAG_SEPARATOR_BOTTOM);
            mOptions[RSK_MIX_DUNGEON_ENTRANCES].Hide();
            mOptions[RSK_MIX_BOSS_ENTRANCES].Hide();
            mOptions[RSK_MIX_OVERWORLD_ENTRANCES].Hide();
            mOptions[RSK_MIX_INTERIOR_ENTRANCES].Hide();
            mOptions[RSK_MIX_THIEVES_HIDEOUT_ENTRANCES].Hide();
            mOptions[RSK_MIX_GROTTO_ENTRANCES].Hide();
        } else {
            if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleDungeonsEntrances"), RO_DUNGEON_ENTRANCE_SHUFFLE_OFF) !=
                RO_DUNGEON_ENTRANCE_SHUFFLE_OFF) {
                mOptions[RSK_MIX_DUNGEON_ENTRANCES].Unhide();
            }
            if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleBossEntrances"), RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF) !=
                RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF) {
                mOptions[RSK_MIX_BOSS_ENTRANCES].Unhide();
            }
            if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleOverworldEntrances"), RO_GENERIC_OFF) != RO_GENERIC_OFF) {
                mOptions[RSK_MIX_OVERWORLD_ENTRANCES].Unhide();
            }
            if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleInteriorsEntrances"), RO_GENERIC_OFF) != RO_GENERIC_OFF) {
                mOptions[RSK_MIX_INTERIOR_ENTRANCES].Unhide();
            }
            if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleThievesHideoutEntrances"), RO_GENERIC_OFF) != RO_GENERIC_OFF) {
                mOptions[RSK_MIX_THIEVES_HIDEOUT_ENTRANCES].Unhide();
            }
            if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleGrottosEntrances"), RO_GENERIC_OFF) != RO_GENERIC_OFF) {
                mOptions[RSK_MIX_GROTTO_ENTRANCES].Unhide();
            }
        }
    });
    OPT_BOOL(RSK_MIX_DUNGEON_ENTRANCES, CVAR_RANDOMIZER_SETTING("MixDungeons"), IMFLAG_NONE);
    OPT_BOOL(RSK_MIX_BOSS_ENTRANCES, CVAR_RANDOMIZER_SETTING("MixBosses"), IMFLAG_NONE);
    OPT_BOOL(RSK_MIX_OVERWORLD_ENTRANCES, CVAR_RANDOMIZER_SETTING("MixOverworld"), IMFLAG_NONE);
    OPT_BOOL(RSK_MIX_INTERIOR_ENTRANCES, CVAR_RANDOMIZER_SETTING("MixInteriors"), IMFLAG_NONE);
    OPT_BOOL(RSK_MIX_THIEVES_HIDEOUT_ENTRANCES, CVAR_RANDOMIZER_SETTING("MixThievesHideout"));
    OPT_BOOL(RSK_MIX_GROTTO_ENTRANCES, CVAR_RANDOMIZER_SETTING("MixGrottos"));
    OPT_BOOL(RSK_DECOUPLED_ENTRANCES, CVAR_RANDOMIZER_SETTING("DecoupleEntrances"));
    OPT_CALLBACK(RSK_DECOUPLED_ENTRANCES, {
        HandleStartingAgeUI();
    });
    OPT_U8(RSK_BOMBCHU_BAG, {"None", "Single Bag", "Progressive Bags"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("BombchuBag"), WIDGET_CVAR_COMBOBOX, RO_BOMBCHU_BAG_NONE);
    OPT_U8(RSK_ENABLE_BOMBCHU_DROPS, {"No", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("EnableBombchuDrops"), WIDGET_CVAR_COMBOBOX, RO_AMMO_DROPS_ON);
    OPT_BOOL(RSK_PROGRESSIVE_GORON_SWORD, CVAR_RANDOMIZER_SETTING("ProgressiveGoronSword"));
    // TODO: AmmoDrops and/or HeartDropRefill, combine with/separate Ammo Drops from Bombchu Drops?
    // Triforce Hunt: the total piece count is the on/off control. Zero disables the hunt entirely; any
    // positive value adds that many Triforce Pieces to the pool and unlocks the pieces-location option.
    OPT_U8(RSK_TRIFORCE_HUNT_PIECES_TOTAL, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("TriforceHuntTotalPieces"), WIDGET_CVAR_SLIDER_INT, 0, false, nullptr, IMFLAG_NONE);
    OPT_CALLBACK(RSK_TRIFORCE_HUNT_PIECES_TOTAL, {
        const uint8_t triforceTotal = CVarGetInteger(CVAR_RANDOMIZER_SETTING("TriforceHuntTotalPieces"), 0);
        if (triforceTotal == 0) {
            mOptions[RSK_TRIFORCE_HUNT_PIECES_LOCATION].Hide();
        } else {
            mOptions[RSK_TRIFORCE_HUNT_PIECES_LOCATION].Unhide();
        }
        if (mOptions[RSK_RAINBOW_BRIDGE_TRIFORCE_COUNT].GetOptionCount() != static_cast<size_t>(triforceTotal) + 1) {
            mOptions[RSK_RAINBOW_BRIDGE_TRIFORCE_COUNT].ChangeOptions(NumOpts(0, triforceTotal));
        }
        if (mOptions[RSK_GBK_TRIFORCE_COUNT].GetOptionCount() != static_cast<size_t>(triforceTotal) + 1) {
            mOptions[RSK_GBK_TRIFORCE_COUNT].ChangeOptions(NumOpts(0, triforceTotal));
        }
        if (mOptions[RSK_GANONS_SOUL_TRIFORCE_COUNT].GetOptionCount() != static_cast<size_t>(triforceTotal) + 1) {
            mOptions[RSK_GANONS_SOUL_TRIFORCE_COUNT].ChangeOptions(NumOpts(0, triforceTotal));
        }
        if (mOptions[RSK_WINCON_TRIFORCE_COUNT].GetOptionCount() != static_cast<size_t>(triforceTotal) + 1) {
            mOptions[RSK_WINCON_TRIFORCE_COUNT].ChangeOptions(NumOpts(0, triforceTotal));
        }
    });
    OPT_U8(RSK_TRIFORCE_HUNT_PIECES_LOCATION, {"Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("TriforceHuntPiecesLocation"), WIDGET_CVAR_COMBOBOX, RO_TRIFORCE_HUNT_LOCATION_ANYWHERE);
    OPT_U8(RSK_MQ_DUNGEON_RANDOM, {"None", "Set Number", "Random", "Selection Only"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeons"), WIDGET_CVAR_COMBOBOX, RO_MQ_DUNGEONS_NONE, false, nullptr, IMFLAG_NONE);
    OPT_CALLBACK(RSK_MQ_DUNGEON_RANDOM, {
        switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("MQDungeons"), RO_MQ_DUNGEONS_NONE)) {
            // If No MQ Dungeons, add a separator after the combobx and hide
            // the count slider and the toggle for individual dungeon selections.
            case RO_MQ_DUNGEONS_NONE:
                mOptions[RSK_MQ_DUNGEON_COUNT].Hide();
                mOptions[RSK_MQ_DUNGEON_SET].Hide();
                break;
            // If Set Number, remove the separator and show both the count slider and the
            // individual dungeon selection toggle.
            case RO_MQ_DUNGEONS_SET_NUMBER:
                mOptions[RSK_MQ_DUNGEON_COUNT].Unhide();
                mOptions[RSK_MQ_DUNGEON_SET].Unhide();
                break;
            // else if random number or selection only, remove the separator and only show
            // the individual dungeon selection toggle.
            case RO_MQ_DUNGEONS_RANDOM_NUMBER:
                mOptions[RSK_MQ_DUNGEON_COUNT].Hide();
                mOptions[RSK_MQ_DUNGEON_SET].Unhide();
                break;
            case RO_MQ_DUNGEONS_SELECTION:
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
    });
    OPT_U8(RSK_MQ_DUNGEON_COUNT, {NumOpts(0, MAX_MQ_DUNGEON_COUNT)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonCount"), WIDGET_CVAR_SLIDER_INT, MAX_MQ_DUNGEON_COUNT, true, nullptr, IMFLAG_NONE);
    OPT_BOOL(RSK_MQ_DUNGEON_SET, {"Off", "On"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsSelection"), WIDGET_CVAR_CHECKBOX, false, false, nullptr, IMFLAG_NONE);
    OPT_CALLBACK(RSK_MQ_DUNGEON_SET, {
        // Controls whether or not to show the selectors for individual dungeons.
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("MQDungeons"), RO_MQ_DUNGEONS_NONE) != RO_MQ_DUNGEONS_NONE &&
            (CVarGetInteger(CVAR_RANDOMIZER_SETTING("MQDungeonsSelection"), RO_GENERIC_OFF) == RO_GENERIC_ON ||
             CVarGetInteger(CVAR_RANDOMIZER_SETTING("MQDungeons"), RO_MQ_DUNGEONS_NONE) == RO_MQ_DUNGEONS_SELECTION)) {
            // if showing the dungeon selectors, remove the separator after the Set Dungeons checkbox.
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
    });
    OPT_U8(RSK_MQ_DEKU_TREE, {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsDekuTree"), WIDGET_CVAR_COMBOBOX, RO_MQ_SET_VANILLA, false, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MQ_DODONGOS_CAVERN, {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsDodongosCavern"), WIDGET_CVAR_COMBOBOX, RO_MQ_SET_VANILLA, false, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MQ_JABU_JABU, {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsJabuJabu"), WIDGET_CVAR_COMBOBOX, RO_MQ_SET_VANILLA, false, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MQ_FOREST_TEMPLE, {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsForestTemple"), WIDGET_CVAR_COMBOBOX, RO_MQ_SET_VANILLA, false, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MQ_FIRE_TEMPLE, {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsFireTemple"), WIDGET_CVAR_COMBOBOX, RO_MQ_SET_VANILLA, false, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MQ_WATER_TEMPLE, {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsWaterTemple"), WIDGET_CVAR_COMBOBOX, RO_MQ_SET_VANILLA, false, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MQ_SPIRIT_TEMPLE, {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsSpiritTemple"), WIDGET_CVAR_COMBOBOX, RO_MQ_SET_VANILLA, false, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MQ_SHADOW_TEMPLE, {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsShadowTemple"), WIDGET_CVAR_COMBOBOX, RO_MQ_SET_VANILLA, false, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MQ_BOTTOM_OF_THE_WELL, {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsBottomOfTheWell"), WIDGET_CVAR_COMBOBOX, RO_MQ_SET_VANILLA, false, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MQ_ICE_CAVERN, {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsIceCavern"), WIDGET_CVAR_COMBOBOX, RO_MQ_SET_VANILLA, false, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MQ_GTG, {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsGTG"), WIDGET_CVAR_COMBOBOX, RO_MQ_SET_VANILLA, false, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MQ_GANONS_CASTLE, {"Vanilla", "Master Quest", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MQDungeonsGanonsCastle"), WIDGET_CVAR_COMBOBOX, RO_MQ_SET_VANILLA);
    OPT_U8(RSK_SHUFFLE_DUNGEON_REWARDS, {"Vanilla", "End of Dungeons", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleDungeonReward"), WIDGET_CVAR_COMBOBOX, RO_DUNGEON_REWARDS_END_OF_DUNGEON);
    OPT_CALLBACK(RSK_SHUFFLE_DUNGEON_REWARDS, {
        // Link's Pocket - Disabled when Dungeon Rewards are shuffled to End of Dungeon
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleDungeonReward"), RO_DUNGEON_REWARDS_END_OF_DUNGEON) ==
            RO_DUNGEON_REWARDS_END_OF_DUNGEON) {
            mOptions[RSK_LINKS_POCKET].Disable(
                "This option is disabled because \"Dungeon Rewards\" are shuffled to \"End of Dungeons\".");
            mOptions[RSK_LINKS_POCKET_REWARD].Enable();
            mOptions[RSK_LINKS_POCKET_REWARD].Unhide();
        } else {
            if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleDungeonReward"), RO_DUNGEON_REWARDS_END_OF_DUNGEON) ==
                RO_DUNGEON_REWARDS_OWN_DUNGEON) {
                mOptions[RSK_LINKS_POCKET].Enable();
                mOptions[RSK_LINKS_POCKET_REWARD].Disable(
                    "As \"Link's Pocket\" is set to \"Dungeon Reward\" while \"Dungeon Rewards\" is set to \"Own Dungeon\", Link's Pocket will always have the Light Medallion");
            }else if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleDungeonReward"), RO_DUNGEON_REWARDS_END_OF_DUNGEON) ==
                RO_DUNGEON_REWARDS_VANILLA) {
                mOptions[RSK_LINKS_POCKET].Enable();
                mOptions[RSK_LINKS_POCKET_REWARD].Disable(
                    "As \"Link's Pocket\" is set to \"Dungeon Reward\" while \"Dungeon Rewards\" is set to \"Vanilla\", Link's Pocket will always have the Light Medallion");
            } else {
                mOptions[RSK_LINKS_POCKET].Enable();
                mOptions[RSK_LINKS_POCKET_REWARD].Enable();
            }
            if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("LinksPocket"), RO_LINKS_POCKET_DUNGEON_REWARD) == RO_LINKS_POCKET_DUNGEON_REWARD) {
                mOptions[RSK_LINKS_POCKET_REWARD].Unhide();
            } else {
                mOptions[RSK_LINKS_POCKET_REWARD].Hide();
            }
        }
    });
    OPT_U8(RSK_LINKS_POCKET, {"Dungeon Reward", "Advancement", "Anything", "Nothing"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("LinksPocket"), WIDGET_CVAR_COMBOBOX, RO_LINKS_POCKET_DUNGEON_REWARD);
    OPT_CALLBACK(RSK_LINKS_POCKET, {
        // Only show the dungeon reward type if Link's Pocket is set to Dungeon Reward and Dungeon Rewards are not Vanilla, OR Dungeon Rewards are end of dungeon
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("LinksPocket"), RO_LINKS_POCKET_DUNGEON_REWARD) == RO_LINKS_POCKET_DUNGEON_REWARD ||
            CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleDungeonReward"), RO_DUNGEON_REWARDS_END_OF_DUNGEON) == RO_DUNGEON_REWARDS_END_OF_DUNGEON) {
            mOptions[RSK_LINKS_POCKET_REWARD].Unhide();
        } else {
            mOptions[RSK_LINKS_POCKET_REWARD].Hide();
        }
    });
    OPT_U8(RSK_LINKS_POCKET_REWARD, {"Any Reward", "Any Stone", "Any Medallion", "Light Medallion"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("LinksPocketReward"), WIDGET_CVAR_COMBOBOX, RO_LINKS_POCKET_ANY_REWARD);
    OPT_U8(RSK_SHUFFLE_SONGS, {"Off", "Song Locations", "Dungeon Rewards", "Anywhere"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleSongs"), WIDGET_CVAR_COMBOBOX, RO_SONG_SHUFFLE_SONG_LOCATIONS);
    OPT_U8(RSK_SHOPSANITY, {"Off", "Specific Count", "Random"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("Shopsanity"), WIDGET_CVAR_COMBOBOX, RO_SHOPSANITY_OFF);
    OPT_CALLBACK(RSK_SHOPSANITY, {
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
    });
    OPT_U8(RSK_SHOPSANITY_COUNT, {NumOpts(0, 8)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityCount"), WIDGET_CVAR_SLIDER_INT, 0, false, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_SHOPSANITY_PRICES, {"Vanilla", "Cheap Balanced", "Balanced", "Fixed", "Range", "Set By Wallet"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityPrices"), WIDGET_CVAR_COMBOBOX, RO_PRICE_VANILLA, false, nullptr, IMFLAG_NONE);
    OPT_CALLBACK(RSK_SHOPSANITY_PRICES, {
        HandleShopsanityPriceUI();
    });
    OPT_U8(RSK_SHOPSANITY_PRICES_FIXED_PRICE, {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityFixedPrice"), WIDGET_CVAR_SLIDER_INT, 10, true);
    OPT_U8(RSK_SHOPSANITY_PRICES_RANGE_1, {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityPriceRange1"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_SHOPSANITY_PRICES_RANGE_2, {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityPriceRange2"), WIDGET_CVAR_SLIDER_INT, 100, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_SHOPSANITY_PRICES_NO_WALLET_WEIGHT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityNoWalletWeight"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_SHOPSANITY_PRICES_CHILD_WALLET_WEIGHT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityChildWalletWeight"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_SHOPSANITY_PRICES_ADULT_WALLET_WEIGHT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityAdultWalletWeight"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_SHOPSANITY_PRICES_GIANT_WALLET_WEIGHT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityGiantWalletWeight"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_SHOPSANITY_PRICES_TYCOON_WALLET_WEIGHT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShopsanityTycoonWalletWeight"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_BOOL(RSK_SHOPSANITY_PRICES_AFFORDABLE, CVAR_RANDOMIZER_SETTING("ShopsanityPricesAffordable"));
    OPT_BOOL(RSK_SHOP_SHIELDS_AND_TUNICS_ONLY_REFILL, CVAR_RANDOMIZER_SETTING("ShopShieldsTunicsGate"));
    OPT_U8(RSK_SHUFFLE_TOKENS, {"Off", "Dungeons", "Overworld", "All Tokens"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleTokens"), WIDGET_CVAR_COMBOBOX, RO_TOKENSANITY_OFF);
    OPT_U8(RSK_SHUFFLE_SCRUBS, {"Off", "One-Time Only", "All"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleScrubs"), WIDGET_CVAR_COMBOBOX, RO_SCRUBS_OFF);
    OPT_CALLBACK(RSK_SHUFFLE_SCRUBS, {
        bool isTycoon = CVarGetInteger(CVAR_RANDOMIZER_SETTING("IncludeTycoonWallet"), RO_GENERIC_OFF);
        switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleScrubs"), RO_SCRUBS_OFF)) {
            case RO_SCRUBS_OFF:
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
                mOptions[RSK_SCRUBS_PRICES].Unhide();
                switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ScrubsPrices"), RO_PRICE_VANILLA)) {
                    case RO_PRICE_FIXED:
                        mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].Unhide();
                        mOptions[RSK_SCRUBS_PRICES_RANGE_1].Hide();
                        mOptions[RSK_SCRUBS_PRICES_RANGE_2].Hide();
                        mOptions[RSK_SCRUBS_PRICES_NO_WALLET_WEIGHT].Hide();
                        mOptions[RSK_SCRUBS_PRICES_CHILD_WALLET_WEIGHT].Hide();
                        mOptions[RSK_SCRUBS_PRICES_ADULT_WALLET_WEIGHT].Hide();
                        mOptions[RSK_SCRUBS_PRICES_GIANT_WALLET_WEIGHT].Hide();
                        mOptions[RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT].Hide();
                        if (isTycoon ? mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].GetOptionCount() == 501
                                    : mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].GetOptionCount() == 1000) {
                            mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].ChangeOptions(isTycoon ? NumOpts(0, 999)
                                                                                        : NumOpts(0, 500));
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
                        if (isTycoon ? mOptions[RSK_SCRUBS_PRICES_RANGE_1].GetOptionCount() == 101
                                    : mOptions[RSK_SCRUBS_PRICES_RANGE_1].GetOptionCount() == 200) {
                            mOptions[RSK_SCRUBS_PRICES_RANGE_1].ChangeOptions(isTycoon ? NumOpts(0, 995, 5)
                                                                                    : NumOpts(0, 500, 5));
                            mOptions[RSK_SCRUBS_PRICES_RANGE_2].ChangeOptions(isTycoon ? NumOpts(0, 995, 5)
                                                                                    : NumOpts(0, 500, 5));
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
                        if (isTycoon) {
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
    });
    OPT_U8(RSK_SCRUBS_PRICES, {"Vanilla", "Cheap Balanced", "Balanced", "Fixed", "Range", "Set By Wallet"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsPrices"), WIDGET_CVAR_COMBOBOX, RO_PRICE_VANILLA, false, nullptr, IMFLAG_NONE);
    OPT_CALLBACK(RSK_SCRUBS_PRICES, {
        bool isTycoon = CVarGetInteger(CVAR_RANDOMIZER_SETTING("IncludeTycoonWallet"), RO_GENERIC_OFF);
        switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ScrubsPrices"), RO_PRICE_VANILLA)) {
            case RO_PRICE_FIXED:
                mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].Unhide();
                mOptions[RSK_SCRUBS_PRICES_RANGE_1].Hide();
                mOptions[RSK_SCRUBS_PRICES_RANGE_2].Hide();
                mOptions[RSK_SCRUBS_PRICES_NO_WALLET_WEIGHT].Hide();
                mOptions[RSK_SCRUBS_PRICES_CHILD_WALLET_WEIGHT].Hide();
                mOptions[RSK_SCRUBS_PRICES_ADULT_WALLET_WEIGHT].Hide();
                mOptions[RSK_SCRUBS_PRICES_GIANT_WALLET_WEIGHT].Hide();
                mOptions[RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT].Hide();
                if (isTycoon ? mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].GetOptionCount() == 501
                            : mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].GetOptionCount() == 1000) {
                    mOptions[RSK_SCRUBS_PRICES_FIXED_PRICE].ChangeOptions(isTycoon ? NumOpts(0, 999)
                                                                                : NumOpts(0, 500));
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
                if (isTycoon ? mOptions[RSK_SCRUBS_PRICES_RANGE_1].GetOptionCount() == 101
                            : mOptions[RSK_SCRUBS_PRICES_RANGE_1].GetOptionCount() == 200) {
                    mOptions[RSK_SCRUBS_PRICES_RANGE_1].ChangeOptions(isTycoon ? NumOpts(0, 995, 5)
                                                                            : NumOpts(0, 500, 5));
                    mOptions[RSK_SCRUBS_PRICES_RANGE_2].ChangeOptions(isTycoon ? NumOpts(0, 995, 5)
                                                                            : NumOpts(0, 500, 5));
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
                if (isTycoon) {
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
    });
    OPT_U8(RSK_SCRUBS_PRICES_FIXED_PRICE, {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsFixedPrice"), WIDGET_CVAR_SLIDER_INT, 10, true);
    OPT_U8(RSK_SCRUBS_PRICES_RANGE_1, {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsPriceRange1"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_SCRUBS_PRICES_RANGE_2, {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsPriceRange2"), WIDGET_CVAR_SLIDER_INT, 100, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_SCRUBS_PRICES_NO_WALLET_WEIGHT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsNoWalletWeight"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_SCRUBS_PRICES_CHILD_WALLET_WEIGHT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsChildWalletWeight"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_SCRUBS_PRICES_ADULT_WALLET_WEIGHT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsAdultWalletWeight"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_SCRUBS_PRICES_GIANT_WALLET_WEIGHT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsGiantWalletWeight"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ScrubsTycoonWalletWeight"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_BOOL(RSK_SCRUBS_PRICES_AFFORDABLE, CVAR_RANDOMIZER_SETTING("ScrubsPricesAffordable"));
    OPT_BOOL(RSK_SHUFFLE_BEEHIVES, CVAR_RANDOMIZER_SETTING("ShuffleBeehives"));
    OPT_CALLBACK(RSK_SHUFFLE_BEEHIVES, {
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleBeehives"), RO_GENERIC_OFF)) {
            mOptions[RSK_SLINGBOW_BREAK_BEEHIVES].Enable();
        } else {
            mOptions[RSK_SLINGBOW_BREAK_BEEHIVES].Disable(
                "This option is disabled because Shuffle Beehives is not enabled.");
        }
    });
    OPT_BOOL(RSK_SHUFFLE_COWS, CVAR_RANDOMIZER_SETTING("ShuffleCows"));
    OPT_CALLBACK(RSK_SHUFFLE_COWS, {
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleCows"), RO_GENERIC_OFF)) {
            mOptions[RSK_MALON_HINT].Enable();
        } else {
            mOptions[RSK_MALON_HINT].Disable("Malon's hint points to a cow, so requires cows to be shuffled.");
        }
    });
    OPT_BOOL(RSK_SHUFFLE_KOKIRI_SWORD, CVAR_RANDOMIZER_SETTING("ShuffleKokiriSword"));
    OPT_BOOL(RSK_SHUFFLE_MASTER_SWORD, CVAR_RANDOMIZER_SETTING("ShuffleMasterSword"));
    OPT_BOOL(RSK_SWORDLESS_EPONA_ITEMS, CVAR_RANDOMIZER_SETTING("SwordlessEponaItems"));
    OPT_BOOL(RSK_SHUFFLE_CHILD_WALLET, CVAR_RANDOMIZER_SETTING("ShuffleChildWallet"), IMFLAG_NONE);
    OPT_CALLBACK(RSK_SHUFFLE_CHILD_WALLET, {
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleChildWallet"), 0)) {
            CVarSetInteger(CVAR_RANDOMIZER_SETTING("StartingWallet"), 0);
            mOptions[RSK_STARTING_WALLET].Disable("Disabled because Shuffle Child's Wallet is on.");
        } else {
            mOptions[RSK_STARTING_WALLET].Enable();
        }
    });
    OPT_BOOL(RSK_INCLUDE_TYCOON_WALLET, CVAR_RANDOMIZER_SETTING("IncludeTycoonWallet"));
    OPT_BOOL(RSK_SHUFFLE_OCARINA, CVAR_RANDOMIZER_SETTING("ShuffleOcarinas"));
    OPT_CALLBACK(RSK_SHUFFLE_OCARINA, {
        HandleStartingAgeUI();
    });
    OPT_BOOL(RSK_SHUFFLE_OCARINA_BUTTONS, CVAR_RANDOMIZER_SETTING("ShuffleOcarinaButtons"));
    OPT_BOOL(RSK_SHUFFLE_SWIM, CVAR_RANDOMIZER_SETTING("ShuffleSwim"));
    OPT_CALLBACK(RSK_SHUFFLE_SWIM, {
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleSwim"), 0)) {
            CVarSetInteger(CVAR_RANDOMIZER_SETTING("StartingScale"), 0);
            mOptions[RSK_STARTING_SCALE].Disable("Disabled because Shuffle Swim is on.");
        } else {
            mOptions[RSK_STARTING_SCALE].Enable();
        }
    });
    OPT_BOOL(RSK_SHUFFLE_CLIMB, CVAR_RANDOMIZER_SETTING("ShuffleClimb"));
    OPT_BOOL(RSK_SHUFFLE_CRAWL, CVAR_RANDOMIZER_SETTING("ShuffleCrawl"));
    OPT_BOOL(RSK_SHUFFLE_GRAB, CVAR_RANDOMIZER_SETTING("ShuffleGrab"));
    OPT_CALLBACK(RSK_SHUFFLE_GRAB, {
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleGrab"), 0)) {
            CVarSetInteger(CVAR_RANDOMIZER_SETTING("StartingStrength"), 0);
            mOptions[RSK_STARTING_STRENGTH].Disable("Disabled because Shuffle Grab is on.");
        } else {
            mOptions[RSK_STARTING_STRENGTH].Enable();
        }
    });
    OPT_BOOL(RSK_SHUFFLE_SPEAK, CVAR_RANDOMIZER_SETTING("ShuffleSpeak"));
    OPT_U8(RSK_SHUFFLE_OPEN_CHEST, {"Off", "On", "Progressive"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleOpenChest"), WIDGET_CVAR_COMBOBOX, RO_OPEN_CHEST_OFF);
    OPT_U8(RSK_SHUFFLE_WEIRD_EGG, {"Vanilla", "Shuffled", "Skip Waking Talon"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleWeirdEgg"), WIDGET_CVAR_COMBOBOX, RO_WEIRD_EGG_VANILLA);
    OPT_BOOL(RSK_SHUFFLE_ZELDAS_LETTER, CVAR_RANDOMIZER_SETTING("ShuffleZeldasLetter"));
    OPT_BOOL(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD, CVAR_RANDOMIZER_SETTING("ShuffleGerudoToken"));
    OPT_U8(RSK_SHUFFLE_POTS, {"Off", "Dungeons", "Overworld", "All Pots"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShufflePots"), WIDGET_CVAR_COMBOBOX, RO_SHUFFLE_POTS_OFF);
    OPT_U8(RSK_SHUFFLE_GRASS, {"Off", "Dungeons", "Overworld", "All Grass"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleGrass"), WIDGET_CVAR_COMBOBOX, RO_SHUFFLE_GRASS_OFF);
    OPT_U8(RSK_SHUFFLE_CRATES, {"Off", "Dungeons", "Overworld", "All Crates"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleCrates"), WIDGET_CVAR_COMBOBOX, RO_SHUFFLE_CRATES_OFF);
    OPT_BOOL(RSK_SHUFFLE_ROCKS, CVAR_RANDOMIZER_SETTING("ShuffleRocks"));
    OPT_U8(RSK_SHUFFLE_BOULDERS, {"Off", "Dungeons", "Overworld", "All Boulders"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleBoulders"), WIDGET_CVAR_COMBOBOX, RO_SHUFFLE_BOULDERS_OFF);
    OPT_BOOL(RSK_SHUFFLE_TREES, CVAR_RANDOMIZER_SETTING("ShuffleTrees"));
    OPT_BOOL(RSK_SHUFFLE_BUSHES, CVAR_RANDOMIZER_SETTING("ShuffleBushes"));
    OPT_BOOL(RSK_SHUFFLE_ICICLES, CVAR_RANDOMIZER_SETTING("ShuffleIcicles"));
    OPT_BOOL(RSK_SHUFFLE_RED_ICE, CVAR_RANDOMIZER_SETTING("ShuffleRedIce"));
    OPT_U8(RSK_SHUFFLE_SIGNS, {"Off", "Dungeons", "Overworld", "All Signs"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleSigns"), WIDGET_CVAR_COMBOBOX, RO_SHUFFLE_SIGNS_OFF);
    OPT_BOOL(RSK_SHUFFLE_FISHING_POLE, CVAR_RANDOMIZER_SETTING("ShuffleFishingPole"));
    OPT_CALLBACK(RSK_SHUFFLE_FISHING_POLE, {
        // Disable fishing pole hint if the fishing pole is not shuffled
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleFishingPole"), RO_GENERIC_OFF)) {
            mOptions[RSK_FISHING_POLE_HINT].Enable();
        } else {
            mOptions[RSK_FISHING_POLE_HINT].Disable("This option is disabled since the fishing pole is not shuffled.");
        }
    });
    OPT_U8(RSK_SHUFFLE_MERCHANTS, {"Off", "Bean Merchant Only", "All But Beans", "All"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleMerchants"), WIDGET_CVAR_COMBOBOX, RO_SHUFFLE_MERCHANTS_OFF, IMFLAG_NONE);
    OPT_CALLBACK(RSK_SHUFFLE_MERCHANTS, {
        bool isTycoon = CVarGetInteger(CVAR_RANDOMIZER_SETTING("IncludeTycoonWallet"), RO_GENERIC_OFF);
        switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleMerchants"), RO_SHUFFLE_MERCHANTS_OFF)) {
            case RO_SHUFFLE_MERCHANTS_OFF:
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
                mOptions[RSK_MERCHANT_PRICES].Unhide();
                switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("MerchantPrices"), RO_PRICE_VANILLA)) {
                    case RO_PRICE_FIXED:
                        mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].Unhide();
                        mOptions[RSK_MERCHANT_PRICES_RANGE_1].Hide();
                        mOptions[RSK_MERCHANT_PRICES_RANGE_2].Hide();
                        mOptions[RSK_MERCHANT_PRICES_NO_WALLET_WEIGHT].Hide();
                        mOptions[RSK_MERCHANT_PRICES_CHILD_WALLET_WEIGHT].Hide();
                        mOptions[RSK_MERCHANT_PRICES_ADULT_WALLET_WEIGHT].Hide();
                        mOptions[RSK_MERCHANT_PRICES_GIANT_WALLET_WEIGHT].Hide();
                        mOptions[RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT].Hide();
                        if (isTycoon ? mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].GetOptionCount() == 501
                                    : mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].GetOptionCount() == 1000) {
                            mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].ChangeOptions(isTycoon ? NumOpts(0, 999)
                                                                                            : NumOpts(0, 500));
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
                        if (isTycoon ? mOptions[RSK_MERCHANT_PRICES_RANGE_1].GetOptionCount() == 101
                                    : mOptions[RSK_MERCHANT_PRICES_RANGE_1].GetOptionCount() == 200) {
                            mOptions[RSK_MERCHANT_PRICES_RANGE_1].ChangeOptions(isTycoon ? NumOpts(0, 995, 5)
                                                                                        : NumOpts(0, 500, 5));
                            mOptions[RSK_MERCHANT_PRICES_RANGE_2].ChangeOptions(isTycoon ? NumOpts(0, 995, 5)
                                                                                        : NumOpts(0, 500, 5));
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
                        if (isTycoon) {
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
    });
    OPT_U8(RSK_MERCHANT_PRICES, {"Vanilla", "Cheap Balanced", "Balanced", "Fixed", "Range", "Set By Wallet"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantPrices"), WIDGET_CVAR_COMBOBOX, RO_PRICE_VANILLA, false, nullptr, IMFLAG_NONE);
    OPT_CALLBACK(RSK_MERCHANT_PRICES, {
        bool isTycoon = CVarGetInteger(CVAR_RANDOMIZER_SETTING("IncludeTycoonWallet"), RO_GENERIC_OFF);
        switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("MerchantPrices"), RO_PRICE_VANILLA)) {
            case RO_PRICE_FIXED:
                mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].Unhide();
                mOptions[RSK_MERCHANT_PRICES_RANGE_1].Hide();
                mOptions[RSK_MERCHANT_PRICES_RANGE_2].Hide();
                mOptions[RSK_MERCHANT_PRICES_NO_WALLET_WEIGHT].Hide();
                mOptions[RSK_MERCHANT_PRICES_CHILD_WALLET_WEIGHT].Hide();
                mOptions[RSK_MERCHANT_PRICES_ADULT_WALLET_WEIGHT].Hide();
                mOptions[RSK_MERCHANT_PRICES_GIANT_WALLET_WEIGHT].Hide();
                mOptions[RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT].Hide();
                if (isTycoon ? mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].GetOptionCount() == 501
                            : mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].GetOptionCount() == 1000) {
                    mOptions[RSK_MERCHANT_PRICES_FIXED_PRICE].ChangeOptions(isTycoon ? NumOpts(0, 999)
                                                                                    : NumOpts(0, 500));
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
                if (isTycoon ? mOptions[RSK_MERCHANT_PRICES_RANGE_1].GetOptionCount() == 101
                            : mOptions[RSK_MERCHANT_PRICES_RANGE_1].GetOptionCount() == 200) {
                    mOptions[RSK_MERCHANT_PRICES_RANGE_1].ChangeOptions(isTycoon ? NumOpts(0, 995, 5)
                                                                                : NumOpts(0, 500, 5));
                    mOptions[RSK_MERCHANT_PRICES_RANGE_2].ChangeOptions(isTycoon ? NumOpts(0, 995, 5)
                                                                                : NumOpts(0, 500, 5));
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
                if (isTycoon) {
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
    });
    OPT_U8(RSK_MERCHANT_PRICES_FIXED_PRICE, {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantFixedPrice"), WIDGET_CVAR_SLIDER_INT, 10, true);
    OPT_U8(RSK_MERCHANT_PRICES_RANGE_1, {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantPriceRange1"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MERCHANT_PRICES_RANGE_2, {NumOpts(0, 995, 5)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantPriceRange2"), WIDGET_CVAR_SLIDER_INT, 100, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MERCHANT_PRICES_NO_WALLET_WEIGHT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantNoWalletWeight"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MERCHANT_PRICES_CHILD_WALLET_WEIGHT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantChildWalletWeight"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MERCHANT_PRICES_ADULT_WALLET_WEIGHT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantAdultWalletWeight"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MERCHANT_PRICES_GIANT_WALLET_WEIGHT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantGiantWalletWeight"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_U8(RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("MerchantTycoonWalletWeight"), WIDGET_CVAR_SLIDER_INT, 10, true, nullptr, IMFLAG_NONE);
    OPT_BOOL(RSK_MERCHANT_PRICES_AFFORDABLE, CVAR_RANDOMIZER_SETTING("MerchantPricesAffordable"));
    OPT_BOOL(RSK_SHUFFLE_BEGGAR, CVAR_RANDOMIZER_SETTING("ShuffleBeggar"));
    OPT_BOOL(RSK_SHUFFLE_FROG_SONG_RUPEES, CVAR_RANDOMIZER_SETTING("ShuffleFrogSongRupees"));
    OPT_BOOL(RSK_SHUFFLE_ADULT_TRADE, CVAR_RANDOMIZER_SETTING("ShuffleAdultTrade"));
    OPT_CALLBACK(RSK_SHUFFLE_ADULT_TRADE, {
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleAdultTrade"), RO_GENERIC_OFF)) {
            mOptions[RSK_EARLY_GRANNYS_SHOP].Disable("This has no effect when Shuffle Adult Trade is on.");
        } else {
            mOptions[RSK_EARLY_GRANNYS_SHOP].Enable();
        }
    });
    OPT_BOOL(RSK_SHUFFLE_CHEST_MINIGAME, CVAR_RANDOMIZER_SETTING("ShuffleChestMinigame"));
    OPT_CALLBACK(RSK_SHUFFLE_CHEST_MINIGAME, {
        HandleKeyringUI();
    });
    OPT_BOOL(RSK_SHUFFLE_100_GS_REWARD, CVAR_RANDOMIZER_SETTING("Shuffle100GSReward"), IMFLAG_SEPARATOR_BOTTOM, WIDGET_CVAR_CHECKBOX, RO_GENERIC_OFF);
    OPT_CALLBACK(RSK_SHUFFLE_100_GS_REWARD, {
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("Shuffle100GSReward"), RO_GENERIC_OFF)) {
            mOptions[RSK_KAK_100_SKULLS_HINT].Enable();
        } else {
            mOptions[RSK_KAK_100_SKULLS_HINT].Disable("There is no point to hinting 100 skulls if it is not shuffled.");
        }
    });
    OPT_BOOL(RSK_SHUFFLE_BEAN_SOULS, CVAR_RANDOMIZER_SETTING("ShuffleBeanSouls"), IMFLAG_SEPARATOR_BOTTOM, WIDGET_CVAR_CHECKBOX, RO_GENERIC_OFF);
    OPT_U8(RSK_SHUFFLE_BOSS_SOULS, {"Off", "On"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleBossSouls"), WIDGET_CVAR_COMBOBOX);
    OPT_BOOL(RSK_SHUFFLE_DEKU_STICK_BAG, CVAR_RANDOMIZER_SETTING("ShuffleDekuStickBag"), IMFLAG_SEPARATOR_BOTTOM, WIDGET_CVAR_CHECKBOX, RO_GENERIC_OFF);
    OPT_CALLBACK(RSK_SHUFFLE_DEKU_STICK_BAG, {
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleDekuStickBag"), 0)) {
            mOptions[RSK_STARTING_STICKS].Disable("Disabled because Shuffle Deku Stick Bag is on.");
        } else {
            mOptions[RSK_STARTING_STICKS].Enable();
        }
    });
    OPT_BOOL(RSK_SHUFFLE_DEKU_NUT_BAG, CVAR_RANDOMIZER_SETTING("ShuffleDekuNutBag"), IMFLAG_SEPARATOR_BOTTOM, WIDGET_CVAR_CHECKBOX, RO_GENERIC_OFF);
    OPT_CALLBACK(RSK_SHUFFLE_DEKU_NUT_BAG, {
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleDekuNutBag"), 0)) {
            mOptions[RSK_STARTING_NUTS].Disable("Disabled because Shuffle Deku Nut Bag is on.");
        } else {
            mOptions[RSK_STARTING_NUTS].Enable();
        }
    });
    OPT_U8(RSK_SHUFFLE_FREESTANDING, {"Off", "Dungeons", "Overworld", "All Items"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleFreestanding"), WIDGET_CVAR_COMBOBOX, RO_SHUFFLE_FREESTANDING_OFF);
    OPT_U8(RSK_SHUFFLE_WONDER_ITEMS, {"Off", "Dungeons", "Overworld", "All Items"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleWonderItems"), WIDGET_CVAR_COMBOBOX, RO_SHUFFLE_WONDER_ITEMS_OFF);
    OPT_U8(RSK_SHUFFLE_SILVER, {"Off", "On", "Wallet", "Start With"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleSilver"), WIDGET_CVAR_COMBOBOX, RO_SHUFFLE_SILVER_OFF);
    OPT_U8(RSK_FISHSANITY, {"Off", "Shuffle only Hyrule Loach", "Shuffle Fishing Pond", "Shuffle Overworld Fish", "Shuffle Both"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("Fishsanity"), WIDGET_CVAR_COMBOBOX, RO_FISHSANITY_OFF);
    OPT_CALLBACK(RSK_FISHSANITY, {
        // Hide fishing pond settings if we aren't shuffling the fishing pond
        switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("Fishsanity"), RO_FISHSANITY_OFF)) {
            case RO_FISHSANITY_POND:
            case RO_FISHSANITY_BOTH:
                mOptions[RSK_FISHSANITY_POND_COUNT].Unhide();
                mOptions[RSK_FISHSANITY_AGE_SPLIT].Unhide();
                break;
            default:
                mOptions[RSK_FISHSANITY_POND_COUNT].Hide();
                mOptions[RSK_FISHSANITY_AGE_SPLIT].Hide();
        }
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("Fishsanity"), RO_FISHSANITY_OFF) == RO_FISHSANITY_HYRULE_LOACH) {
            mOptions[RSK_LOACH_HINT].Enable();
        } else {
            mOptions[RSK_LOACH_HINT].Disable(
                "Loach hint is only avaliable with \"Fishsanity\" set to \"Shuffle only Hyrule Loach\"\nas that's the only "
                "setting where you present the loach to the fishing pond owner.");
        }
    });
    OPT_U8(RSK_FISHSANITY_POND_COUNT, {NumOpts(0,17,1)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("FishsanityPondCount"), WIDGET_CVAR_SLIDER_INT, 0, true, nullptr, IMFLAG_NONE);
    OPT_BOOL(RSK_FISHSANITY_AGE_SPLIT, CVAR_RANDOMIZER_SETTING("FishsanityAgeSplit"));
    OPT_BOOL(RSK_SHUFFLE_FOUNTAIN_FAIRIES, CVAR_RANDOMIZER_SETTING("ShuffleFountainFairies"));
    OPT_BOOL(RSK_SHUFFLE_STONE_FAIRIES, CVAR_RANDOMIZER_SETTING("ShuffleStoneFairies"));
    OPT_BOOL(RSK_SHUFFLE_BEAN_FAIRIES, CVAR_RANDOMIZER_SETTING("ShuffleBeanFairies"));
    OPT_BOOL(RSK_SHUFFLE_SONG_FAIRIES, CVAR_RANDOMIZER_SETTING("ShuffleFairySpots"));
    OPT_BOOL(RSK_SHUFFLE_BUTTERFLY_FAIRIES, CVAR_RANDOMIZER_SETTING("ShuffleButterflyFairies"));
    OPT_U8(RSK_SHUFFLE_MAPANDCOMPASS, {"Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingMapsCompasses"), WIDGET_CVAR_COMBOBOX, RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
    OPT_U8(RSK_KEYSANITY, {"Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("Keysanity"), WIDGET_CVAR_COMBOBOX, RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
    OPT_U8(RSK_GERUDO_KEYS, {"Vanilla", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GerudoKeys"), WIDGET_CVAR_COMBOBOX, RO_GERUDO_KEYS_VANILLA);
    OPT_CALLBACK(RSK_GERUDO_KEYS, {
        HandleKeyringUI();
    });
    OPT_U8(RSK_BOSS_KEYSANITY, {"Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("BossKeysanity"), WIDGET_CVAR_COMBOBOX, RO_DUNGEON_ITEM_LOC_OWN_DUNGEON);
    OPT_U8(RSK_GANONS_BOSS_KEY, {"Vanilla", "Own Dungeon", "Start With", "Any Dungeon", "Overworld", "Anywhere", "Trigger-Stones", "Trigger-Medallions", "Trigger-Rewards", "Trigger-Dungeons", "Trigger-Tokens", "Trigger-Triforce Pieces"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleGanonBossKey"), WIDGET_CVAR_COMBOBOX, RO_GANON_BOSS_KEY_VANILLA);
    OPT_CALLBACK(RSK_GANONS_BOSS_KEY, {
        mOptions[RSK_GBK_OPTIONS].Hide();
        mOptions[RSK_GBK_STONE_COUNT].Hide();
        mOptions[RSK_GBK_MEDALLION_COUNT].Hide();
        mOptions[RSK_GBK_REWARD_COUNT].Hide();
        mOptions[RSK_GBK_DUNGEON_COUNT].Hide();
        mOptions[RSK_GBK_TOKEN_COUNT].Hide();
        mOptions[RSK_GBK_TRIFORCE_COUNT].Hide();
        switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleGanonBossKey"), RO_GANON_BOSS_KEY_VANILLA)) {
            case RO_GANON_BOSS_KEY_STONES:
                mOptions[RSK_GBK_OPTIONS].Unhide();
                mOptions[RSK_GBK_STONE_COUNT].Unhide();
                break;
            case RO_GANON_BOSS_KEY_MEDALLIONS:
                mOptions[RSK_GBK_OPTIONS].Unhide();
                mOptions[RSK_GBK_MEDALLION_COUNT].Unhide();
                break;
            case RO_GANON_BOSS_KEY_REWARDS:
                mOptions[RSK_GBK_OPTIONS].Unhide();
                mOptions[RSK_GBK_REWARD_COUNT].Unhide();
                break;
            case RO_GANON_BOSS_KEY_DUNGEONS:
                mOptions[RSK_GBK_OPTIONS].Unhide();
                mOptions[RSK_GBK_DUNGEON_COUNT].Unhide();
                break;
            case RO_GANON_BOSS_KEY_TOKENS:
                mOptions[RSK_GBK_TOKEN_COUNT].Unhide();
                break;
            case RO_GANON_BOSS_KEY_TRIFORCE_PIECES:
                mOptions[RSK_GBK_TRIFORCE_COUNT].Unhide();
                break;
        }
    });
    OPT_U8(RSK_GBK_STONE_COUNT, {NumOpts(0, 4)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GbkStoneCount"), WIDGET_CVAR_SLIDER_INT, 3, true);
    OPT_U8(RSK_GBK_MEDALLION_COUNT, {NumOpts(0, 7)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GbkMedallionCount"), WIDGET_CVAR_SLIDER_INT, 6, true);
    OPT_U8(RSK_GBK_REWARD_COUNT, {NumOpts(0, 10)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GbkRewardCount"), WIDGET_CVAR_SLIDER_INT, 9, true);
    OPT_U8(RSK_GBK_DUNGEON_COUNT, {NumOpts(0, 9)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GbkDungeonCount"), WIDGET_CVAR_SLIDER_INT, 8, true);
    OPT_U8(RSK_GBK_TOKEN_COUNT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GbkTokenCount"), WIDGET_CVAR_SLIDER_INT, 100, true);
    OPT_U8(RSK_GBK_TRIFORCE_COUNT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GbkTriforceCount"), WIDGET_CVAR_SLIDER_INT, 100, true);
    OPT_U8(RSK_GBK_OPTIONS, {"Standard Reward", "Greg as Reward", "Greg as Wildcard"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GbkRewardOptions"), WIDGET_CVAR_COMBOBOX, RO_CHECK_TRIGGER_STANDARD_REWARD);
    OPT_CALLBACK(RSK_GBK_OPTIONS, {
        const uint8_t gbkOpts = CVarGetInteger(CVAR_RANDOMIZER_SETTING("GbkRewardOptions"), RO_CHECK_TRIGGER_STANDARD_REWARD);
        if (gbkOpts == RO_CHECK_TRIGGER_GREG_REWARD) {
            mOptions[RSK_GBK_STONE_COUNT].ChangeOptions(NumOpts(0, 4));
            mOptions[RSK_GBK_MEDALLION_COUNT].ChangeOptions(NumOpts(0, 7));
            mOptions[RSK_GBK_REWARD_COUNT].ChangeOptions(NumOpts(0, 10));
            mOptions[RSK_GBK_DUNGEON_COUNT].ChangeOptions(NumOpts(0, 9));
        } else {
            mOptions[RSK_GBK_STONE_COUNT].ChangeOptions(NumOpts(0, 3));
            mOptions[RSK_GBK_MEDALLION_COUNT].ChangeOptions(NumOpts(0, 6));
            mOptions[RSK_GBK_REWARD_COUNT].ChangeOptions(NumOpts(0, 9));
            mOptions[RSK_GBK_DUNGEON_COUNT].ChangeOptions(NumOpts(0, 8));
        }
    });
    OPT_U8(RSK_GANONS_SOUL, {"Start With", "Any Dungeon", "Overworld", "Anywhere", "Trigger-Stones", "Trigger-Medallions", "Trigger-Rewards", "Trigger-Dungeons", "Trigger-Tokens", "Trigger-Triforce Pieces"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleGanonsSoul"), WIDGET_CVAR_COMBOBOX, RO_GANONS_SOUL_STARTWITH);
    OPT_CALLBACK(RSK_GANONS_SOUL, {
        mOptions[RSK_GANONS_SOUL_OPTIONS].Hide();
        mOptions[RSK_GANONS_SOUL_STONE_COUNT].Hide();
        mOptions[RSK_GANONS_SOUL_MEDALLION_COUNT].Hide();
        mOptions[RSK_GANONS_SOUL_REWARD_COUNT].Hide();
        mOptions[RSK_GANONS_SOUL_DUNGEON_COUNT].Hide();
        mOptions[RSK_GANONS_SOUL_TOKEN_COUNT].Hide();
        mOptions[RSK_GANONS_SOUL_TRIFORCE_COUNT].Hide();
        switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleGanonsSoul"), RO_GANONS_SOUL_STARTWITH)) {
            case RO_GANONS_SOUL_STONES:
                mOptions[RSK_GANONS_SOUL_OPTIONS].Unhide();
                mOptions[RSK_GANONS_SOUL_STONE_COUNT].Unhide();
                break;
            case RO_GANONS_SOUL_MEDALLIONS:
                mOptions[RSK_GANONS_SOUL_OPTIONS].Unhide();
                mOptions[RSK_GANONS_SOUL_MEDALLION_COUNT].Unhide();
                break;
            case RO_GANONS_SOUL_REWARDS:
                mOptions[RSK_GANONS_SOUL_OPTIONS].Unhide();
                mOptions[RSK_GANONS_SOUL_REWARD_COUNT].Unhide();
                break;
            case RO_GANONS_SOUL_DUNGEONS:
                mOptions[RSK_GANONS_SOUL_OPTIONS].Unhide();
                mOptions[RSK_GANONS_SOUL_DUNGEON_COUNT].Unhide();
                break;
            case RO_GANONS_SOUL_TOKENS:
                mOptions[RSK_GANONS_SOUL_TOKEN_COUNT].Unhide();
                break;
            case RO_GANONS_SOUL_TRIFORCE_PIECES:
                mOptions[RSK_GANONS_SOUL_TRIFORCE_COUNT].Unhide();
                break;
        }
    });
    OPT_U8(RSK_GANONS_SOUL_STONE_COUNT, {NumOpts(0, 4)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GanonsSoulStoneCount"), WIDGET_CVAR_SLIDER_INT, 3, true);
    OPT_U8(RSK_GANONS_SOUL_MEDALLION_COUNT, {NumOpts(0, 7)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GanonsSoulMedallionCount"), WIDGET_CVAR_SLIDER_INT, 6, true);
    OPT_U8(RSK_GANONS_SOUL_REWARD_COUNT, {NumOpts(0, 10)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GanonsSoulRewardCount"), WIDGET_CVAR_SLIDER_INT, 9, true);
    OPT_U8(RSK_GANONS_SOUL_DUNGEON_COUNT, {NumOpts(0, 9)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GanonsSoulDungeonCount"), WIDGET_CVAR_SLIDER_INT, 8, true);
    OPT_U8(RSK_GANONS_SOUL_TOKEN_COUNT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GanonsSoulTokenCount"), WIDGET_CVAR_SLIDER_INT, 100, true);
    OPT_U8(RSK_GANONS_SOUL_TRIFORCE_COUNT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GanonsSoulTriforceCount"), WIDGET_CVAR_SLIDER_INT, 100, true);
    OPT_U8(RSK_GANONS_SOUL_OPTIONS, {"Standard Reward", "Greg as Reward", "Greg as Wildcard"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GanonsSoulRewardOptions"), WIDGET_CVAR_COMBOBOX, RO_CHECK_TRIGGER_STANDARD_REWARD);
    OPT_CALLBACK(RSK_GANONS_SOUL_OPTIONS, {
        const uint8_t soulOpts = CVarGetInteger(CVAR_RANDOMIZER_SETTING("GanonsSoulRewardOptions"), RO_CHECK_TRIGGER_STANDARD_REWARD);
        if (soulOpts == RO_CHECK_TRIGGER_GREG_REWARD) {
            mOptions[RSK_GANONS_SOUL_STONE_COUNT].ChangeOptions(NumOpts(0, 4));
            mOptions[RSK_GANONS_SOUL_MEDALLION_COUNT].ChangeOptions(NumOpts(0, 7));
            mOptions[RSK_GANONS_SOUL_REWARD_COUNT].ChangeOptions(NumOpts(0, 10));
            mOptions[RSK_GANONS_SOUL_DUNGEON_COUNT].ChangeOptions(NumOpts(0, 9));
        } else {
            mOptions[RSK_GANONS_SOUL_STONE_COUNT].ChangeOptions(NumOpts(0, 3));
            mOptions[RSK_GANONS_SOUL_MEDALLION_COUNT].ChangeOptions(NumOpts(0, 6));
            mOptions[RSK_GANONS_SOUL_REWARD_COUNT].ChangeOptions(NumOpts(0, 9));
            mOptions[RSK_GANONS_SOUL_DUNGEON_COUNT].ChangeOptions(NumOpts(0, 8));
        }
    });
    OPT_U8(RSK_WINCON, {"Defeat Ganon", "Anywhere", "Trigger-Stones", "Trigger-Medallions", "Trigger-Rewards", "Trigger-Dungeons", "Trigger-Tokens", "Trigger-Triforce Pieces"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleWincon"), WIDGET_CVAR_COMBOBOX, RO_WINCON_DEFEAT_GANON);
    OPT_CALLBACK(RSK_WINCON, {
        mOptions[RSK_WINCON_OPTIONS].Hide();
        mOptions[RSK_WINCON_STONE_COUNT].Hide();
        mOptions[RSK_WINCON_MEDALLION_COUNT].Hide();
        mOptions[RSK_WINCON_REWARD_COUNT].Hide();
        mOptions[RSK_WINCON_DUNGEON_COUNT].Hide();
        mOptions[RSK_WINCON_TOKEN_COUNT].Hide();
        mOptions[RSK_WINCON_TRIFORCE_COUNT].Hide();
        switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleWincon"), RO_WINCON_DEFEAT_GANON)) {
            case RO_WINCON_STONES:
                mOptions[RSK_WINCON_OPTIONS].Unhide();
                mOptions[RSK_WINCON_STONE_COUNT].Unhide();
                break;
            case RO_WINCON_MEDALLIONS:
                mOptions[RSK_WINCON_OPTIONS].Unhide();
                mOptions[RSK_WINCON_MEDALLION_COUNT].Unhide();
                break;
            case RO_WINCON_REWARDS:
                mOptions[RSK_WINCON_OPTIONS].Unhide();
                mOptions[RSK_WINCON_REWARD_COUNT].Unhide();
                break;
            case RO_WINCON_DUNGEONS:
                mOptions[RSK_WINCON_OPTIONS].Unhide();
                mOptions[RSK_WINCON_DUNGEON_COUNT].Unhide();
                break;
            case RO_WINCON_TOKENS:
                mOptions[RSK_WINCON_TOKEN_COUNT].Unhide();
                break;
            case RO_WINCON_TRIFORCE_PIECES:
                mOptions[RSK_WINCON_TRIFORCE_COUNT].Unhide();
                break;
        }
    });
    OPT_U8(RSK_WINCON_STONE_COUNT, {NumOpts(0, 4)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("WinconStoneCount"), WIDGET_CVAR_SLIDER_INT, 3, true);
    OPT_U8(RSK_WINCON_MEDALLION_COUNT, {NumOpts(0, 7)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("WinconMedallionCount"), WIDGET_CVAR_SLIDER_INT, 6, true);
    OPT_U8(RSK_WINCON_REWARD_COUNT, {NumOpts(0, 10)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("WinconRewardCount"), WIDGET_CVAR_SLIDER_INT, 9, true);
    OPT_U8(RSK_WINCON_DUNGEON_COUNT, {NumOpts(0, 9)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("WinconDungeonCount"), WIDGET_CVAR_SLIDER_INT, 8, true);
    OPT_U8(RSK_WINCON_TOKEN_COUNT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("WinconTokenCount"), WIDGET_CVAR_SLIDER_INT, 100, true);
    OPT_U8(RSK_WINCON_TRIFORCE_COUNT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("WinconTriforceCount"), WIDGET_CVAR_SLIDER_INT, 100, true);
    OPT_U8(RSK_WINCON_OPTIONS, {"Standard Reward", "Greg as Reward", "Greg as Wildcard"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("WinconRewardOptions"), WIDGET_CVAR_COMBOBOX, RO_CHECK_TRIGGER_STANDARD_REWARD);
    OPT_CALLBACK(RSK_WINCON_OPTIONS, {
        const uint8_t winconOpts = CVarGetInteger(CVAR_RANDOMIZER_SETTING("WinconRewardOptions"), RO_CHECK_TRIGGER_STANDARD_REWARD);
        if (winconOpts == RO_CHECK_TRIGGER_GREG_REWARD) {
            mOptions[RSK_WINCON_STONE_COUNT].ChangeOptions(NumOpts(0, 4));
            mOptions[RSK_WINCON_MEDALLION_COUNT].ChangeOptions(NumOpts(0, 7));
            mOptions[RSK_WINCON_REWARD_COUNT].ChangeOptions(NumOpts(0, 10));
            mOptions[RSK_WINCON_DUNGEON_COUNT].ChangeOptions(NumOpts(0, 9));
        } else {
            mOptions[RSK_WINCON_STONE_COUNT].ChangeOptions(NumOpts(0, 3));
            mOptions[RSK_WINCON_MEDALLION_COUNT].ChangeOptions(NumOpts(0, 6));
            mOptions[RSK_WINCON_REWARD_COUNT].ChangeOptions(NumOpts(0, 9));
            mOptions[RSK_WINCON_DUNGEON_COUNT].ChangeOptions(NumOpts(0, 8));
        }
    });
    OPT_U8(RSK_KEYRINGS, {"Off", "Random", "Count", "Selection"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRings"), WIDGET_CVAR_COMBOBOX, RO_KEYRINGS_OFF);
    OPT_CALLBACK(RSK_KEYRINGS, {
        switch (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShuffleKeyRings"), RO_KEYRINGS_OFF)) {
            case RO_KEYRINGS_COUNT:
                // Show count slider.
                mOptions[RSK_KEYRINGS_RANDOM_COUNT].Unhide();
                mOptions[RSK_KEYRINGS_GERUDO_FORTRESS].Hide();
                mOptions[RSK_KEYRINGS_FOREST_TEMPLE].Hide();
                mOptions[RSK_KEYRINGS_FIRE_TEMPLE].Hide();
                mOptions[RSK_KEYRINGS_WATER_TEMPLE].Hide();
                mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE].Hide();
                mOptions[RSK_KEYRINGS_SHADOW_TEMPLE].Hide();
                mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL].Hide();
                mOptions[RSK_KEYRINGS_GTG].Hide();
                mOptions[RSK_KEYRINGS_GANONS_CASTLE].Hide();
                mOptions[RSK_KEYRINGS_CHEST_GAME].Hide();
                break;
            case RO_KEYRINGS_SELECTION:
                // Show checkboxes for each dungeon with keys.
                mOptions[RSK_KEYRINGS_RANDOM_COUNT].Hide();
                mOptions[RSK_KEYRINGS_GERUDO_FORTRESS].Unhide();
                mOptions[RSK_KEYRINGS_FOREST_TEMPLE].Unhide();
                mOptions[RSK_KEYRINGS_FIRE_TEMPLE].Unhide();
                mOptions[RSK_KEYRINGS_WATER_TEMPLE].Unhide();
                mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE].Unhide();
                mOptions[RSK_KEYRINGS_SHADOW_TEMPLE].Unhide();
                mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL].Unhide();
                mOptions[RSK_KEYRINGS_GTG].Unhide();
                mOptions[RSK_KEYRINGS_GANONS_CASTLE].Unhide();
                mOptions[RSK_KEYRINGS_CHEST_GAME].Unhide();
                break;
            default:
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
                mOptions[RSK_KEYRINGS_CHEST_GAME].Hide();
                break;
        }
    });
    OPT_U8(RSK_KEYRINGS_RANDOM_COUNT, {NumOpts(0, 10)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsRandomCount"), WIDGET_CVAR_SLIDER_INT, 8);
    OPT_U8(RSK_KEYRINGS_GERUDO_FORTRESS, {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsGerudoFortress"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_KEYRINGS_FOREST_TEMPLE, {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsForestTemple"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_KEYRINGS_FIRE_TEMPLE, {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsFireTemple"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_KEYRINGS_WATER_TEMPLE, {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsWaterTemple"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_KEYRINGS_SPIRIT_TEMPLE, {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsSpiritTemple"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_KEYRINGS_SHADOW_TEMPLE, {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsShadowTemple"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_KEYRINGS_BOTTOM_OF_THE_WELL, {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsBottomOfTheWell"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_KEYRINGS_GTG, {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsGTG"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_KEYRINGS_GANONS_CASTLE, {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsGanonsCastle"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_KEYRINGS_CHEST_GAME, {"No", "Random", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsChestGame"), WIDGET_CVAR_COMBOBOX, 0);
    //Dummied out due to redundancy with TimeSavers.SkipChildStealth until such a time that logic needs to consider child stealth e.g. because it's freestanding checks are added to freestanding shuffle.
    //To undo this dummying, readd this setting to an OptionGroup so it appears in the UI, then edit the timesaver check hooks to look at this, and the timesaver setting to lock itself as needed.
    OPT_BOOL(RSK_SKIP_CHILD_STEALTH, {"Don't Skip", "Skip"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("SkipChildStealth"), WIDGET_CVAR_CHECKBOX, RO_GENERIC_DONT_SKIP);
    OPT_BOOL(RSK_EARLY_GRANNYS_SHOP, CVAR_RANDOMIZER_SETTING("EarlyGrannysShop"));
    OPT_BOOL(RSK_SKIP_EPONA_RACE, {"Don't Skip", "Skip"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("SkipEponaRace"), WIDGET_CVAR_CHECKBOX, RO_GENERIC_DONT_SKIP);
    OPT_BOOL(RSK_SKIP_SCARECROWS_SONG, CVAR_RANDOMIZER_SETTING("SkipScarecrowsSong"));
    OPT_BOOL(RSK_SKIP_PLANTING_BEANS, CVAR_RANDOMIZER_SETTING("SkipPlantingBeans"));
    OPT_U8(RSK_BIG_POE_COUNT, {NumOpts(0, 10)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("BigPoeTargetCount"), WIDGET_CVAR_SLIDER_INT, 10);
    OPT_CALLBACK(RSK_BIG_POE_COUNT, {
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("BigPoeTargetCount"), 10) == 0) {
            mOptions[RSK_BIG_POES_HINT].Disable("Poe Collector will just give you the item instead with 0 big poes.");
        } else {
            mOptions[RSK_BIG_POES_HINT].Enable();
        }
    });
    OPT_BOOL(RSK_SHUFFLE_MASKS, CVAR_RANDOMIZER_SETTING("ShuffleMasks"));
    OPT_U8(RSK_GOSSIP_STONE_HINTS, {"No Hints", "Need Nothing", "Mask of Truth", "Stone of Agony"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GossipStoneHints"), WIDGET_CVAR_COMBOBOX, RO_GOSSIP_STONES_NEED_NOTHING, false, nullptr, IMFLAG_NONE);
    OPT_CALLBACK(RSK_GOSSIP_STONE_HINTS, {
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("GossipStoneHints"), RO_GOSSIP_STONES_NEED_NOTHING) ==
            RO_GOSSIP_STONES_NONE) {
            mOptions[RSK_HINT_CLARITY].Hide();
            mOptions[RSK_HINT_DISTRIBUTION].Hide();
        } else {
            mOptions[RSK_HINT_CLARITY].Unhide();
            mOptions[RSK_HINT_DISTRIBUTION].Unhide();
        }
    });
    OPT_U8(RSK_HINT_CLARITY, {"Obscure", "Ambiguous", "Clear"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("HintClarity"), WIDGET_CVAR_COMBOBOX, RO_HINT_CLARITY_CLEAR, true, nullptr, IMFLAG_INDENT);
    OPT_U8(RSK_HINT_DISTRIBUTION, {"Useless", "Balanced", "Strong", "Very Strong"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("HintDistribution"), WIDGET_CVAR_COMBOBOX, RO_HINT_DIST_BALANCED, true, nullptr, IMFLAG_UNINDENT);
    OPT_BOOL(RSK_TOT_ALTAR_HINT, {"Off", "On"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("AltarHint"), WIDGET_CVAR_CHECKBOX, RO_GENERIC_ON, false, nullptr, IMFLAG_INDENT);
    // RANDOTODO make this hint text about no dupe hints a global hint for static hints. Add to navi?
    OPT_BOOL(RSK_GANONDORF_HINT, {"Off", "On"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("GanondorfHint"), WIDGET_CVAR_CHECKBOX, RO_GENERIC_ON, false, nullptr, IMFLAG_NONE);
    OPT_BOOL(RSK_SHEIK_LA_HINT, {"Off", "On"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("SheikLAHint"), WIDGET_CVAR_CHECKBOX, RO_GENERIC_ON, false, nullptr, IMFLAG_NONE);
    OPT_BOOL(RSK_BOSS_KEY_HINT, CVAR_RANDOMIZER_SETTING("BossKeyHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_DAMPES_DIARY_HINT, CVAR_RANDOMIZER_SETTING("DampeHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_GREG_HINT, CVAR_RANDOMIZER_SETTING("GregHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_LOACH_HINT, CVAR_RANDOMIZER_SETTING("LoachHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_SARIA_HINT, CVAR_RANDOMIZER_SETTING("SariaHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_MIDO_HINT, CVAR_RANDOMIZER_SETTING("MidoHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_FISHING_POLE_HINT, CVAR_RANDOMIZER_SETTING("FishingPoleHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_FROGS_HINT, CVAR_RANDOMIZER_SETTING("FrogsHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_OOT_HINT, CVAR_RANDOMIZER_SETTING("OoTHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_BIGGORON_HINT, CVAR_RANDOMIZER_SETTING("BiggoronHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_BIG_POES_HINT, CVAR_RANDOMIZER_SETTING("BigPoesHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_CHICKENS_HINT, CVAR_RANDOMIZER_SETTING("ChickensHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_MALON_HINT, CVAR_RANDOMIZER_SETTING("MalonHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_HBA_HINT, CVAR_RANDOMIZER_SETTING("HBAHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_WARP_SONG_HINTS, CVAR_RANDOMIZER_SETTING("WarpSongText"), IMFLAG_NONE, WIDGET_CVAR_CHECKBOX, RO_GENERIC_ON);
    OPT_BOOL(RSK_SCRUB_TEXT_HINT, CVAR_RANDOMIZER_SETTING("ScrubText"), IMFLAG_NONE);
    OPT_BOOL(RSK_MERCHANT_TEXT_HINT, CVAR_RANDOMIZER_SETTING("MerchantText"), IMFLAG_NONE);
    OPT_BOOL(RSK_KAK_10_SKULLS_HINT, CVAR_RANDOMIZER_SETTING("10GSHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_KAK_20_SKULLS_HINT, CVAR_RANDOMIZER_SETTING("20GSHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_KAK_30_SKULLS_HINT, CVAR_RANDOMIZER_SETTING("30GSHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_KAK_40_SKULLS_HINT, CVAR_RANDOMIZER_SETTING("40GSHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_KAK_50_SKULLS_HINT, CVAR_RANDOMIZER_SETTING("50GSHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_KAK_100_SKULLS_HINT, CVAR_RANDOMIZER_SETTING("100GSHint"), IMFLAG_NONE);
    OPT_BOOL(RSK_MASK_SHOP_HINT, CVAR_RANDOMIZER_SETTING("MaskShopHint"));
    // TODO: Compasses show rewards/woth, maps show dungeon mode
    OPT_BOOL(RSK_BLUE_FIRE_ARROWS, CVAR_RANDOMIZER_SETTING("BlueFireArrows"));
    OPT_BOOL(RSK_SUNLIGHT_ARROWS, CVAR_RANDOMIZER_SETTING("SunlightArrows"));
    OPT_BOOL(RSK_BUNNY_HOOD, CVAR_RANDOMIZER_SETTING("BunnyHood"));
    OPT_BOOL(RSK_MASKS_AS_ADULT, CVAR_RANDOMIZER_SETTING("MasksAsAdult"));
    OPT_BOOL(RSK_ROCS_FEATHER, CVAR_RANDOMIZER_SETTING("RocsFeather"));
    OPT_U8(RSK_INFINITE_UPGRADES, {"Off", "Progressive", "Condensed Progressive"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("InfiniteUpgrades"));
    OPT_BOOL(RSK_SKELETON_KEY, CVAR_RANDOMIZER_SETTING("SkeletonKey"));
    OPT_BOOL(RSK_SLINGBOW_BREAK_BEEHIVES, CVAR_RANDOMIZER_SETTING("SlingBowBeehives"));
    OPT_U8(RSK_ITEM_POOL, {"Plentiful", "Balanced", "Scarce", "Minimal"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("ItemPool"), WIDGET_CVAR_COMBOBOX, RO_ITEM_POOL_BALANCED);
    OPT_BOOL(RSK_BASE_ICE_TRAPS, CVAR_RANDOMIZER_SETTING("BaseIceTraps"), IMFLAG_NONE, WIDGET_CVAR_COMBOBOX, RO_GENERIC_ON);
    OPT_U8(RSK_ADDITIONAL_ICE_TRAPS, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("AdditionalIceTraps"), WIDGET_CVAR_SLIDER_INT, 0);
    OPT_U8(RSK_ICE_TRAP_PERCENT, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("IceTrapPercent"), WIDGET_CVAR_SLIDER_INT, 0);
    OPT_U8(RSK_ICE_TRAP_NAMES, {"Identical", "Similar", "Misspelled (Vowel)", "Misspelled (Duplicate)", "Revealed"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("IceTrapNames"), WIDGET_CVAR_COMBOBOX, RO_ICE_TRAP_NAMES_SIMILAR);
    // TODO: Remove Double Defense
    OPT_U8(RSK_STARTING_OCARINA, {"Off", "Fairy Ocarina", "Ocarina of Time"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingOcarina"), WIDGET_CVAR_COMBOBOX, RO_STARTING_OCARINA_OFF);
    OPT_BOOL(RSK_STARTING_DEKU_SHIELD, CVAR_RANDOMIZER_SETTING("StartingDekuShield"));
    OPT_BOOL(RSK_STARTING_KOKIRI_SWORD, CVAR_RANDOMIZER_SETTING("StartingKokiriSword"));
    OPT_BOOL(RSK_STARTING_MASTER_SWORD, CVAR_RANDOMIZER_SETTING("StartingMasterSword"));
    OPT_BOOL(RSK_STARTING_STICKS, {"No", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingSticks"), WIDGET_CVAR_CHECKBOX, RO_GENERIC_OFF);
    OPT_BOOL(RSK_STARTING_NUTS, {"No", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingNuts"), WIDGET_CVAR_CHECKBOX, RO_GENERIC_OFF);
    OPT_BOOL(RSK_STARTING_BEANS, {"No", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingBeans"), WIDGET_CVAR_CHECKBOX, RO_GENERIC_OFF);
    OPT_BOOL(RSK_STARTING_MEGATON_HAMMER, CVAR_RANDOMIZER_SETTING("StartingMegatonHammer"));
    OPT_BOOL(RSK_STARTING_BOOMERANG, CVAR_RANDOMIZER_SETTING("StartingBoomerang"));
    OPT_BOOL(RSK_STARTING_LENS_OF_TRUTH, CVAR_RANDOMIZER_SETTING("StartingLensOfTruth"));
    OPT_BOOL(RSK_STARTING_DINS_FIRE, CVAR_RANDOMIZER_SETTING("StartingDinsFire"));
    OPT_BOOL(RSK_STARTING_FARORES_WIND, CVAR_RANDOMIZER_SETTING("StartingFaroresWind"));
    OPT_BOOL(RSK_STARTING_NAYRUS_LOVE, CVAR_RANDOMIZER_SETTING("StartingNayrusLove"));
    OPT_BOOL(RSK_STARTING_FIRE_ARROWS, CVAR_RANDOMIZER_SETTING("StartingFireArrows"));
    OPT_BOOL(RSK_STARTING_ICE_ARROWS, CVAR_RANDOMIZER_SETTING("StartingIceArrows"));
    OPT_BOOL(RSK_STARTING_LIGHT_ARROWS, CVAR_RANDOMIZER_SETTING("StartingLightArrows"));
    OPT_BOOL(RSK_STARTING_IRON_BOOTS, CVAR_RANDOMIZER_SETTING("StartingIronBoots"));
    OPT_BOOL(RSK_STARTING_HOVER_BOOTS, CVAR_RANDOMIZER_SETTING("StartingHoverBoots"));
    OPT_BOOL(RSK_STARTING_HYLIAN_SHIELD, CVAR_RANDOMIZER_SETTING("StartingHylianShield"));
    OPT_BOOL(RSK_STARTING_MIRROR_SHIELD, CVAR_RANDOMIZER_SETTING("StartingMirrorShield"));
    OPT_BOOL(RSK_STARTING_GORON_TUNIC, CVAR_RANDOMIZER_SETTING("StartingGoronTunic"));
    OPT_BOOL(RSK_STARTING_ZORA_TUNIC, CVAR_RANDOMIZER_SETTING("StartingZoraTunic"));
    OPT_BOOL(RSK_STARTING_STONE_OF_AGONY, CVAR_RANDOMIZER_SETTING("StartingStoneOfAgony"));
    OPT_U8(RSK_STARTING_HOOKSHOT, {"Off", "Hookshot", "Longshot"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingHookshot"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_STARTING_BOW, {"Off", "Bow (Quiver 30)", "Bow (Quiver 40)", "Bow (Quiver 50)"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingBow"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_STARTING_SLINGSHOT, {"Off", "Slingshot (30)", "Slingshot (40)", "Slingshot (50)"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingSlingshot"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_STARTING_BOMB_BAG, {"Off", "Bomb Bag (20)", "Bomb Bag (30)", "Bomb Bag (40)"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingBombBag"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_STARTING_STRENGTH, {"Off", "Goron's Bracelet", "Silver Gauntlets", "Golden Gauntlets"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingStrength"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_STARTING_SCALE, {"Off", "Silver Scale", "Golden Scale"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingScale"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_STARTING_WALLET, {"Off", "Adult's Wallet", "Giant's Wallet"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingWallet"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_STARTING_MAGIC_METER, {"Off", "Single Magic", "Double Magic"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingMagicMeter"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_STARTING_BOMBCHU_BAG, {"Off", "Bombchu Bag (20)", "Bombchu Bag (30)", "Bombchu Bag (50)"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingBombchuBag"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_STARTING_BOTTLE_1, {"Off", "Empty Bottle", "Bottle with Big Poe", "Ruto's Letter"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingBottle1"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_STARTING_BOTTLE_2, {"Off", "Empty Bottle", "Bottle with Big Poe"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingBottle2"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_STARTING_BOTTLE_3, {"Off", "Empty Bottle", "Bottle with Big Poe"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingBottle3"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_U8(RSK_STARTING_BOTTLE_4, {"Off", "Empty Bottle", "Bottle with Big Poe"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingBottle4"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_BOOL(RSK_STARTING_WEIRD_EGG, CVAR_RANDOMIZER_SETTING("StartingWeirdEgg"));
    OPT_BOOL(RSK_STARTING_ZELDAS_LETTER, CVAR_RANDOMIZER_SETTING("StartingZeldasLetter"));
    OPT_BOOL(RSK_STARTING_CLAIM_CHECK, CVAR_RANDOMIZER_SETTING("StartingClaimCheck"));
    OPT_BOOL(RSK_STARTING_GERUDO_CARD, CVAR_RANDOMIZER_SETTING("StartingGerudoCard"));
    OPT_BOOL(RSK_STARTING_KEATON_MASK, CVAR_RANDOMIZER_SETTING("StartingKeatonMask"));
    OPT_BOOL(RSK_STARTING_SKULL_MASK, CVAR_RANDOMIZER_SETTING("StartingSkullMask"));
    OPT_BOOL(RSK_STARTING_SPOOKY_MASK, CVAR_RANDOMIZER_SETTING("StartingSpookyMask"));
    OPT_BOOL(RSK_STARTING_BUNNY_HOOD, CVAR_RANDOMIZER_SETTING("StartingBunnyHood"));
    OPT_BOOL(RSK_STARTING_GORON_MASK, CVAR_RANDOMIZER_SETTING("StartingGoronMask"));
    OPT_BOOL(RSK_STARTING_ZORA_MASK, CVAR_RANDOMIZER_SETTING("StartingZoraMask"));
    OPT_BOOL(RSK_STARTING_GERUDO_MASK, CVAR_RANDOMIZER_SETTING("StartingGerudoMask"));
    OPT_BOOL(RSK_STARTING_MASK_OF_TRUTH, CVAR_RANDOMIZER_SETTING("StartingMaskOfTruth"));
    OPT_U8(RSK_STARTING_BIGGORON_SWORD, {"Off", "Giant's Knife", "Biggoron's Sword"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingBiggoronSword"), WIDGET_CVAR_COMBOBOX, 0);
    OPT_BOOL(RSK_FULL_WALLETS, {"No", "Yes"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("FullWallets"), WIDGET_CVAR_CHECKBOX, RO_GENERIC_OFF);
    OPT_BOOL(RSK_STARTING_ZELDAS_LULLABY, CVAR_RANDOMIZER_SETTING("StartingZeldasLullaby"), IMFLAG_NONE);
    OPT_BOOL(RSK_STARTING_EPONAS_SONG, CVAR_RANDOMIZER_SETTING("StartingEponasSong"), IMFLAG_NONE);
    OPT_BOOL(RSK_STARTING_SARIAS_SONG, CVAR_RANDOMIZER_SETTING("StartingSariasSong"), IMFLAG_NONE);
    OPT_BOOL(RSK_STARTING_SUNS_SONG, CVAR_RANDOMIZER_SETTING("StartingSunsSong"), IMFLAG_NONE);
    OPT_BOOL(RSK_STARTING_SONG_OF_TIME, CVAR_RANDOMIZER_SETTING("StartingSongOfTime"), IMFLAG_NONE);
    OPT_BOOL(RSK_STARTING_SONG_OF_STORMS, CVAR_RANDOMIZER_SETTING("StartingSongOfStorms"), IMFLAG_NONE);
    OPT_BOOL(RSK_STARTING_MINUET_OF_FOREST, CVAR_RANDOMIZER_SETTING("StartingMinuetOfForest"), IMFLAG_NONE);
    OPT_BOOL(RSK_STARTING_BOLERO_OF_FIRE, CVAR_RANDOMIZER_SETTING("StartingBoleroOfFire"), IMFLAG_NONE);
    OPT_BOOL(RSK_STARTING_SERENADE_OF_WATER, CVAR_RANDOMIZER_SETTING("StartingSerenadeOfWater"), IMFLAG_NONE);
    OPT_BOOL(RSK_STARTING_REQUIEM_OF_SPIRIT, CVAR_RANDOMIZER_SETTING("StartingRequiemOfSpirit"), IMFLAG_NONE);
    OPT_BOOL(RSK_STARTING_NOCTURNE_OF_SHADOW, CVAR_RANDOMIZER_SETTING("StartingNocturneOfShadow"), IMFLAG_NONE);
    OPT_BOOL(RSK_STARTING_PRELUDE_OF_LIGHT, CVAR_RANDOMIZER_SETTING("StartingPreludeOfLight"));
    OPT_U8(RSK_STARTING_SKULLTULA_TOKEN, {NumOpts(0, 100)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingSkulltulaToken"), WIDGET_CVAR_SLIDER_INT);
    OPT_U8(RSK_STARTING_HEARTS, {NumOpts(1, 20)}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("StartingHearts"), WIDGET_CVAR_SLIDER_INT, 2);
    // TODO: Remainder of Starting Items
    OPT_U8(RSK_LOGIC_RULES, {"Glitchless", "No Logic"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("LogicRules"), WIDGET_CVAR_COMBOBOX, RO_LOGIC_GLITCHLESS, false, nullptr, IMFLAG_LABEL_INLINE);
    OPT_CALLBACK(RSK_LOGIC_RULES, {
        HandleStartingAgeUI();
        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("LogicRules"), RO_LOGIC_GLITCHLESS) != RO_LOGIC_NO_LOGIC &&
            CVarGetInteger(CVAR_RANDOMIZER_SETTING("ShopsanityCount"), 0) > 7) {
            CVarSetInteger(CVAR_RANDOMIZER_SETTING("ShopsanityCount"), 7);
        }
    });
    OPT_BOOL(RSK_ALL_LOCATIONS_REACHABLE, {"Off", "On"}, OptionCategory::Setting, CVAR_RANDOMIZER_SETTING("AllLocationsReachable"), WIDGET_CVAR_CHECKBOX, RO_GENERIC_ON, false, nullptr, IMFLAG_SAME_LINE);
    OPT_BOOL(RSK_SKULLS_SUNS_SONG, CVAR_RANDOMIZER_SETTING("GsExpectSunsSong"));
    OPT_U8(RSK_DAMAGE_MULTIPLIER, {"x1/2", "x1", "x2", "x4", "x8", "x16", "OHKO"}, OptionCategory::Setting, "", WIDGET_CVAR_SLIDER_INT, RO_DAMAGE_MULTIPLIER_DEFAULT);
    // Don't show any MQ options if both quests aren't available
    if (!(OTRGlobals::Instance->HasMasterQuest() && OTRGlobals::Instance->HasOriginal())) {
        mOptions[RSK_MQ_DUNGEON_RANDOM].Disable("This option has been disabled because only one type of O2R has been loaded");
        mOptions[RSK_MQ_DUNGEON_COUNT].Disable("This option has been disabled because only one type of O2R has been loaded");
        mOptions[RSK_MQ_DUNGEON_SET].Disable("This option has been disabled because only one type of O2R has been loaded");
        mOptions[RSK_MQ_DEKU_TREE].Disable("This option has been disabled because only one type of O2R has been loaded");
        mOptions[RSK_MQ_DODONGOS_CAVERN].Disable("This option has been disabled because only one type of O2R has been loaded");
        mOptions[RSK_MQ_JABU_JABU].Disable("This option has been disabled because only one type of O2R has been loaded");
        mOptions[RSK_MQ_FOREST_TEMPLE].Disable("This option has been disabled because only one type of O2R has been loaded");
        mOptions[RSK_MQ_FIRE_TEMPLE].Disable("This option has been disabled because only one type of O2R has been loaded");
        mOptions[RSK_MQ_WATER_TEMPLE].Disable("This option has been disabled because only one type of O2R has been loaded");
        mOptions[RSK_MQ_SPIRIT_TEMPLE].Disable("This option has been disabled because only one type of O2R has been loaded");
        mOptions[RSK_MQ_SHADOW_TEMPLE].Disable("This option has been disabled because only one type of O2R has been loaded");
        mOptions[RSK_MQ_BOTTOM_OF_THE_WELL].Disable("This option has been disabled because only one type of O2R has been loaded");
        mOptions[RSK_MQ_ICE_CAVERN].Disable("This option has been disabled because only one type of O2R has been loaded");
        mOptions[RSK_MQ_GTG].Disable("This option has been disabled because only one type of O2R has been loaded");
        mOptions[RSK_MQ_GANONS_CASTLE].Disable("This option has been disabled because only one type of O2R has been loaded");
    } else {
        // If any MQ Options are available, show the MQ Dungeon Randomization Combobox
        mOptions[RSK_MQ_DUNGEON_RANDOM].Enable();
        mOptions[RSK_MQ_DUNGEON_COUNT].Enable();
        mOptions[RSK_MQ_DUNGEON_SET].Enable();
        mOptions[RSK_MQ_DEKU_TREE].Enable();
        mOptions[RSK_MQ_DODONGOS_CAVERN].Enable();
        mOptions[RSK_MQ_JABU_JABU].Enable();
        mOptions[RSK_MQ_FOREST_TEMPLE].Enable();
        mOptions[RSK_MQ_FIRE_TEMPLE].Enable();
        mOptions[RSK_MQ_WATER_TEMPLE].Enable();
        mOptions[RSK_MQ_SPIRIT_TEMPLE].Enable();
        mOptions[RSK_MQ_SHADOW_TEMPLE].Enable();
        mOptions[RSK_MQ_BOTTOM_OF_THE_WELL].Enable();
        mOptions[RSK_MQ_ICE_CAVERN].Enable();
        mOptions[RSK_MQ_GTG].Enable();
        mOptions[RSK_MQ_GANONS_CASTLE].Enable();
    }
    // clang-format on

    StaticData::optionNameToEnum = PopulateOptionNameToEnum();

    // RANDOTODO sweep trick descriptions and make sure they match a post-refactor, post shuffles reality
    /* Common abbreviations in name tags
    - A: Adult
    - Blk: Block
    - Blu: Blue (Switch)
    - Bmb: Bombs
    - Bou: Boulder
    - C: Child
    - Clp: Clip
    - Col: Collision
    - Cuc: Cucoo
    - Crt: Crate
    - Diff: Difficult (Weapons)
    - Ent: Entrance
    - HB: Hover Boots
    - Jmp: Jump
    - Ldg: Ledge
    - LoT: Lens of Truth
    - Prj: Projectile
    - Rng: Boomerang
    - Sli: Slingshot
    - Skp: Skip
    - Swt: Switch
    - Tor: Torch
    Try to keep Name Tags less than 8 chars.
    */

    // the following are glitches and are currently disabled

    // OPT_TRICK(RT_ACUTE_ANGLE_CLIP, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::ADVANCED, Tricks::Tag::EXPERIMENTAL,
    // Tricks::Tag::GLITCH });

    // OPT_TRICK(RT_ADVANCED_CLIPS, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::ADVANCED, Tricks::Tag::EXPERIMENTAL,
    // Tricks::Tag::GLITCH });

    // OPT_TRICK(RT_BLANK_A, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::ADVANCED, Tricks::Tag::EXPERIMENTAL,
    // Tricks::Tag::GLITCH });

    // OPT_TRICK(RT_DOOM_JUMP, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::ADVANCED, Tricks::Tag::EXPERIMENTAL,
    // Tricks::Tag::GLITCH });

    // OPT_TRICK(RT_EPG, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::ADVANCED, Tricks::Tag::EXPERIMENTAL,
    // Tricks::Tag::GLITCH});

    // OPT_TRICK(RT_EQUIP_SWAP, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::ADVANCED, Tricks::Tag::EXPERIMENTAL,
    // Tricks::Tag::GLITCH });

    // OPT_TRICK(RT_EQUIP_SWAP_EXPECTS_DINS, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::ADVANCED, Tricks::Tag::EXPERIMENTAL,
    // Tricks::Tag::GLITCH });

    // OPT_TRICK(RT_FLAME_STORAGE, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::ADVANCED, Tricks::Tag::EXPERIMENTAL,
    // Tricks::Tag::GLITCH });

    // OPT_TRICK(RT_GROUND_CLIP, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::ADVANCED, Tricks::Tag::EXPERIMENTAL,
    // Tricks::Tag::GLITCH });

    // OPT_TRICK(RT_HESS, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::ADVANCED, Tricks::Tag::EXPERIMENTAL,
    // Tricks::Tag::GLITCH});

    // OPT_TRICK(RT_HOOKSHOT_CLIP, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::ADVANCED, Tricks::Tag::EXPERIMENTAL,
    // Tricks::Tag::GLITCH });

    // OPT_TRICK(RT_HOOKSHOT_JUMP, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::ADVANCED, Tricks::Tag::EXPERIMENTAL,
    // Tricks::Tag::GLITCH });

    // OPT_TRICK(RT_ISG, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::ADVANCED, Tricks::Tag::EXPERIMENTAL,
    // Tricks::Tag::GLITCH});

    OPT_TRICK(RT_VISIBLE_COLLISION, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::NOVICE }, "VisCol");
    OPT_TRICK(RT_GROTTOS_WITHOUT_AGONY, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::NOVICE }, "NoSoA");
    OPT_TRICK(RT_FEWER_TUNIC_REQUIREMENTS, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::INTERMEDIATE }, "FTR");
    OPT_TRICK(RT_UNINTUITIVE_JUMPS, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::NOVICE }, "UnJmp");
    OPT_TRICK(RT_FIRE_RINGS, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::INTERMEDIATE }, "FlaChst");

    OPT_TRICK(RT_DAMAGE_BOOST_SIMPLE, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::ADVANCED, Tricks::Tag::EXPERIMENTAL },
              "SDmgBoo");
    OPT_TRICK(RT_HOVER_BOOST_SIMPLE, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::ADVANCED, Tricks::Tag::GLITCH }, "SHovBoo");
    OPT_TRICK(RT_BOMBCHU_BEEHIVES, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::NOVICE }, "ChuBee");
    OPT_TRICK(RT_HOOKSHOT_LADDERS, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::NOVICE }, "HSLad");
    OPT_TRICK(RT_BLUE_FIRE_MUD_WALLS, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::NOVICE }, "BluFire");
    OPT_TRICK(RT_OPEN_UNDERWATER_CHEST, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::NOVICE, Tricks::Tag::GLITCH }, "OpenUC");
    OPT_TRICK(RT_BOULDER_COLLISION, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::NOVICE, Tricks::Tag::GLITCH }, "BolCol");
    OPT_TRICK(RT_ITEM_EXTENSION, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::INTERMEDIATE }, "HSExt");
    OPT_TRICK(RT_BIG_SKULLTULA_PAUSE_LIFT, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::NOVICE, Tricks::Tag::GLITCH },
              "SkulPaus");
    OPT_TRICK(RT_GROUND_JUMP, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::NOVICE, Tricks::Tag::GLITCH }, "GrdJmp");
    OPT_TRICK(RT_GROUND_JUMP_HARD, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::INTERMEDIATE, Tricks::Tag::GLITCH },
              "HGrdJmp");
    OPT_TRICK(RT_SLIDE_JUMP, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::NOVICE }, "SldJmp");
    OPT_TRICK(RT_VOIDOUT_COLLECTION, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::NOVICE }, "VdCl");
    OPT_TRICK(RT_BOMB_DETONATION, RCQUEST_BOTH, RA_NONE, { Tricks::Tag::NOVICE }, "BmbDet");
    OPT_TRICK(RT_KF_ADULT_GS, RCQUEST_BOTH, RA_KOKIRI_FOREST, { Tricks::Tag::NOVICE }, "KFGSHB");
    OPT_TRICK(RT_LW_BRIDGE, RCQUEST_BOTH, RA_THE_LOST_WOODS, { Tricks::Tag::EXPERT }, "LWBrgJmp");
    OPT_TRICK(RT_LW_MIDO_BACKFLIP, RCQUEST_BOTH, RA_THE_LOST_WOODS, { Tricks::Tag::NOVICE }, "MidoSkip");
    OPT_TRICK(RT_LOST_WOOD_NAVI_DIVE, RCQUEST_BOTH, RA_THE_LOST_WOODS, { Tricks::Tag::NOVICE, Tricks::Tag::GLITCH },
              "LWNaviD");
    OPT_TRICK(RT_LW_GS_BEAN, RCQUEST_BOTH, RA_THE_LOST_WOODS, { Tricks::Tag::INTERMEDIATE }, "LWGSHS");
    OPT_TRICK(RT_HC_STORMS_GS, RCQUEST_BOTH, RA_HYRULE_CASTLE, { Tricks::Tag::INTERMEDIATE }, "HCGrGSRng");
    OPT_TRICK(RT_HF_BIG_POE_WITHOUT_EPONA, RCQUEST_BOTH, RA_HYRULE_FIELD, { Tricks::Tag::NOVICE }, "PoeDiff");
    OPT_TRICK(RT_KAK_TOWER_GS, RCQUEST_BOTH, RA_KAKARIKO_VILLAGE, { Tricks::Tag::INTERMEDIATE }, "KakGSJS");
    OPT_TRICK(RT_KAK_CHILD_WINDMILL_POH, RCQUEST_BOTH, RA_KAKARIKO_VILLAGE, { Tricks::Tag::EXTREME }, "WndCJS");
    OPT_TRICK(RT_KAK_ROOFTOP_GS, RCQUEST_BOTH, RA_KAKARIKO_VILLAGE, { Tricks::Tag::ADVANCED }, "KakGSHB");
    OPT_TRICK(RT_GY_POH, RCQUEST_BOTH, RA_THE_GRAVEYARD, { Tricks::Tag::INTERMEDIATE }, "GYPoHRng");
    OPT_TRICK(RT_GY_CHILD_DAMPE_RACE_POH, RCQUEST_BOTH, RA_THE_GRAVEYARD, { Tricks::Tag::NOVICE }, "CDmpRace");
    OPT_TRICK(RT_GY_SHADOW_FIRE_ARROWS, RCQUEST_BOTH, RA_THE_GRAVEYARD, { Tricks::Tag::EXPERT }, "FAEntry");
    OPT_TRICK(RT_DMT_SHIELDLESS_CLIMB, RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, { Tricks::Tag::NOVICE }, "DMTCWoS");
    OPT_TRICK(RT_DMT_SOIL_GS, RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, { Tricks::Tag::INTERMEDIATE }, "DMTSoil");
    OPT_TRICK(RT_DMT_BOMBABLE, RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, { Tricks::Tag::INTERMEDIATE }, "DMTSTR");
    OPT_TRICK(RT_DMT_HOVERS_LOWER_GS, RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, { Tricks::Tag::ADVANCED }, "DMTGSHB");
    OPT_TRICK(RT_DMT_BEAN_LOWER_GS, RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, { Tricks::Tag::EXPERT }, "DMTGSMB");
    OPT_TRICK(RT_DMT_JS_LOWER_GS, RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, { Tricks::Tag::INTERMEDIATE }, "DMTGSJS");
    OPT_TRICK(RT_DMT_CLIMB_HOVERS, RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, { Tricks::Tag::ADVANCED }, "DMTBouHB");
    OPT_TRICK(RT_DMT_UPPER_GS, RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, { Tricks::Tag::NOVICE }, "DMTGSBF");

    // disabled for now, only applies when trade quest is not shuffled so there's a timer (currently not considered in
    // logic)

    // OPT_TRICK(RT_DMT_BOLERO_BIGGORON, RCQUEST_BOTH, RA_DEATH_MOUNTAIN_TRAIL, { Tricks::Tag::INTERMEDIATE });

    OPT_TRICK(RT_GC_POT, RCQUEST_BOTH, RA_GORON_CITY, { Tricks::Tag::ADVANCED }, "GorPotChu");
    OPT_TRICK(RT_GC_POT_STRENGTH, RCQUEST_BOTH, RA_GORON_CITY, { Tricks::Tag::INTERMEDIATE }, "GorPotStr");
    OPT_TRICK(RT_GC_ROLLING_STRENGTH, RCQUEST_BOTH, RA_GORON_CITY, { Tricks::Tag::INTERMEDIATE }, "GorStrC");
    OPT_TRICK(RT_GC_LEFTMOST, RCQUEST_BOTH, RA_GORON_CITY, { Tricks::Tag::ADVANCED }, "GCMazHB");
    OPT_TRICK(RT_GC_GROTTO, RCQUEST_BOTH, RA_GORON_CITY, { Tricks::Tag::ADVANCED }, "GorGroHS");
    OPT_TRICK(RT_GC_LINK_GORON_DINS, RCQUEST_BOTH, RA_GORON_CITY, { Tricks::Tag::NOVICE }, "GorDinA");
    OPT_TRICK(RT_DMC_HOVER_BEAN_POH, RCQUEST_BOTH, RA_DEATH_MOUNTAIN_CRATER, { Tricks::Tag::NOVICE }, "DMCHB");
    OPT_TRICK(RT_DMC_BOLERO_JUMP, RCQUEST_BOTH, RA_DEATH_MOUNTAIN_CRATER, { Tricks::Tag::EXTREME }, "DMCBolJump");
    OPT_TRICK(RT_DMC_BOULDER_JS, RCQUEST_BOTH, RA_DEATH_MOUNTAIN_CRATER, { Tricks::Tag::NOVICE }, "DMCHam");
    OPT_TRICK(RT_DMC_BOULDER_SKIP, RCQUEST_BOTH, RA_DEATH_MOUNTAIN_CRATER, { Tricks::Tag::INTERMEDIATE }, "DMCULJmp");
    OPT_TRICK(RT_ZR_LOWER, RCQUEST_BOTH, RA_ZORAS_RIVER, { Tricks::Tag::INTERMEDIATE }, "ZRLJmp");
    OPT_TRICK(RT_ZR_UPPER, RCQUEST_BOTH, RA_ZORAS_RIVER, { Tricks::Tag::INTERMEDIATE }, "ZRUJmp");
    OPT_TRICK(RT_ZR_HOVERS, RCQUEST_BOTH, RA_ZORAS_RIVER, { Tricks::Tag::NOVICE }, "ZRZDHB");
    OPT_TRICK(RT_ZR_CUCCO, RCQUEST_BOTH, RA_ZORAS_RIVER, { Tricks::Tag::NOVICE }, "ZRZDCuc");
    OPT_TRICK(RT_ZD_KING_ZORA_SKIP, RCQUEST_BOTH, RA_ZORAS_DOMAIN, { Tricks::Tag::INTERMEDIATE }, "Mweep");
    OPT_TRICK(RT_ZD_GS, RCQUEST_BOTH, RA_ZORAS_DOMAIN, { Tricks::Tag::INTERMEDIATE }, "ZDGS");
    OPT_TRICK(RT_ZF_GREAT_FAIRY_WITHOUT_EXPLOSIVES, RCQUEST_BOTH, RA_ZORAS_FOUNTAIN, { Tricks::Tag::NOVICE },
              "ZFGFStr2");
    OPT_TRICK(RT_LH_LAB_WALL_GS, RCQUEST_BOTH, RA_LAKE_HYLIA, { Tricks::Tag::NOVICE }, "LHGSJS");
    OPT_TRICK(RT_LH_LAB_DIVING, RCQUEST_BOTH, RA_LAKE_HYLIA, { Tricks::Tag::NOVICE }, "LabHS");
    OPT_TRICK(RT_LH_WATER_HOOKSHOT, RCQUEST_BOTH, RA_LAKE_HYLIA, { Tricks::Tag::INTERMEDIATE }, "WTEntHS");
    OPT_TRICK(RT_GV_CRATE_HOVERS, RCQUEST_BOTH, RA_GERUDO_VALLEY, { Tricks::Tag::INTERMEDIATE }, "GVPoHHB");
    OPT_TRICK(RT_GV_CHILD_TENT, RCQUEST_BOTH, RA_GERUDO_VALLEY, { Tricks::Tag::NOVICE }, "GVTent");
    OPT_TRICK(RT_GV_CHILD_CUCCO_JUMP, RCQUEST_BOTH, RA_GERUDO_VALLEY, { Tricks::Tag::INTERMEDIATE }, "GVCUC");
    OPT_TRICK(RT_GV_HOOKSHOT_BRIDGE, RCQUEST_BOTH, RA_GERUDO_VALLEY, { Tricks::Tag::ADVANCED }, "GVHSBrg");
    OPT_TRICK(RT_PASS_GUARDS_WITH_NOTHING, RCQUEST_BOTH, RA_GERUDO_FORTRESS, { Tricks::Tag::NOVICE }, "Guards");
    OPT_TRICK(RT_GF_WASTELAND_GATE_SIDEHOP_SKIP, RCQUEST_BOTH, RA_GERUDO_FORTRESS, { Tricks::Tag::NOVICE }, "GFHWC");
    OPT_TRICK(RT_GF_ADULT_SKIP_WASTELAND_GATE, RCQUEST_BOTH, RA_GERUDO_FORTRESS, { Tricks::Tag::INTERMEDIATE },
              "GFHWA");
    OPT_TRICK(RT_GF_WARRIOR_WITH_DIFFICULT_WEAPON, RCQUEST_BOTH, RA_GERUDO_FORTRESS, { Tricks::Tag::NOVICE }, "GWDiff");
    OPT_TRICK(RT_GF_LEDGE_CLIP_INTO_GTG, RCQUEST_BOTH, RA_GERUDO_FORTRESS, { Tricks::Tag::NOVICE, Tricks::Tag::GLITCH },
              "GTGLdgClp");

    // only does anything with the Bunny Hood Effect setting on
    OPT_TRICK(RT_HW_BUNNY_CROSSING, RCQUEST_BOTH, RA_HAUNTED_WASTELAND, { Tricks::Tag::NOVICE }, "HWBun");

    OPT_TRICK(RT_HW_CROSSING, RCQUEST_BOTH, RA_HAUNTED_WASTELAND, { Tricks::Tag::INTERMEDIATE }, "RvrSand");
    OPT_TRICK(RT_LENS_HW, RCQUEST_BOTH, RA_HAUNTED_WASTELAND, { Tricks::Tag::INTERMEDIATE }, "HWNoLoT");
    OPT_TRICK(RT_HW_REVERSE, RCQUEST_BOTH, RA_HAUNTED_WASTELAND, { Tricks::Tag::INTERMEDIATE }, "RevHW");
    OPT_TRICK(RT_COLOSSUS_GS, RCQUEST_BOTH, RA_DESERT_COLOSSUS, { Tricks::Tag::NOVICE }, "ColGSHS");
    OPT_TRICK(RT_DEKU_BASEMENT_GS, RCQUEST_VANILLA, RA_DEKU_TREE, { Tricks::Tag::NOVICE }, "DTGSJS");
    OPT_TRICK(RT_DEKU_B1_SKIP, RCQUEST_BOTH, RA_DEKU_TREE, { Tricks::Tag::INTERMEDIATE }, "B1Skip");
    OPT_TRICK(RT_DEKU_B1_BOW_WEBS, RCQUEST_VANILLA, RA_DEKU_TREE, { Tricks::Tag::NOVICE }, "DTWebBow");
    OPT_TRICK(RT_DEKU_B1_BACKFLIP_OVER_SPIKED_LOG, RCQUEST_VANILLA, RA_DEKU_TREE, { Tricks::Tag::NOVICE }, "DTLogBF");
    OPT_TRICK(RT_DEKU_MQ_COMPASS_GS, RCQUEST_MQ, RA_DEKU_TREE, { Tricks::Tag::NOVICE }, "DTGSHam");
    OPT_TRICK(RT_DEKU_MQ_LOG, RCQUEST_MQ, RA_DEKU_TREE, { Tricks::Tag::NOVICE }, "DTLogRol");
    OPT_TRICK(RT_DC_SCARECROW_GS, RCQUEST_VANILLA, RA_DODONGOS_CAVERN, { Tricks::Tag::NOVICE }, "DCArmos");
    OPT_TRICK(RT_DC_VINES_GS, RCQUEST_VANILLA, RA_DODONGOS_CAVERN, { Tricks::Tag::NOVICE }, "DCGSLS");
    OPT_TRICK(RT_DC_ALCOVE_GS, RCQUEST_VANILLA, RA_DODONGOS_CAVERN, { Tricks::Tag::INTERMEDIATE }, "DCAGSLS");
    OPT_TRICK(RT_DC_STAIRS_WITH_BOW, RCQUEST_VANILLA, RA_DODONGOS_CAVERN, { Tricks::Tag::NOVICE }, "DCStaBow");
    OPT_TRICK(RT_DC_SLINGSHOT_SKIP, RCQUEST_VANILLA, RA_DODONGOS_CAVERN, { Tricks::Tag::EXPERT }, "DCSliSkp");
    OPT_TRICK(RT_DC_SCRUB_ROOM, RCQUEST_VANILLA, RA_DODONGOS_CAVERN, { Tricks::Tag::NOVICE }, "DCSrbStr");
    OPT_TRICK(RT_DC_HAMMER_FLOOR, RCQUEST_BOTH, RA_DODONGOS_CAVERN, { Tricks::Tag::NOVICE }, "KDHamFl");
    OPT_TRICK(RT_DC_DODONGO_CHU, RCQUEST_BOTH, RA_DODONGOS_CAVERN, { Tricks::Tag::ADVANCED }, "KDChu");
    OPT_TRICK(RT_DC_MQ_STAIRS_WITH_ONLY_STRENGTH, RCQUEST_MQ, RA_DODONGOS_CAVERN, { Tricks::Tag::NOVICE }, "DCStaStr");
    OPT_TRICK(RT_DC_CHILD_LOBBY, RCQUEST_BOTH, RA_DODONGOS_CAVERN, { Tricks::Tag::ADVANCED }, "DCLobyJS");
    OPT_TRICK(RT_DC_MQ_CHILD_EYES, RCQUEST_MQ, RA_DODONGOS_CAVERN, { Tricks::Tag::EXPERT }, "DCEyeStrC");
    OPT_TRICK(RT_DC_MQ_ADULT_EYES, RCQUEST_MQ, RA_DODONGOS_CAVERN, { Tricks::Tag::ADVANCED }, "DCEyeStrA");
    OPT_TRICK(RT_DC_EYES_CHU, RCQUEST_BOTH, RA_DODONGOS_CAVERN, { Tricks::Tag::ADVANCED }, "DCEyeChu");
    OPT_TRICK(RT_DC_EYES_BUNNYHOVERS, RCQUEST_BOTH, RA_DODONGOS_CAVERN, { Tricks::Tag::ADVANCED }, "DCEyeBun");
    OPT_TRICK(RT_JABU_BOSS_HOVER, RCQUEST_VANILLA, RA_JABU_JABUS_BELLY, { Tricks::Tag::INTERMEDIATE }, "JbuBoxHB");
    OPT_TRICK(RT_JABU_NEAR_BOSS_RANGED, RCQUEST_BOTH, RA_JABU_JABUS_BELLY, { Tricks::Tag::NOVICE }, "JbuBosPrj");
    OPT_TRICK(RT_JABU_NEAR_BOSS_EXPLOSIVES, RCQUEST_VANILLA, RA_JABU_JABUS_BELLY, { Tricks::Tag::INTERMEDIATE },
              "JbuBosExp");
    OPT_TRICK(RT_JABU_B1_CUBE_HOVER, RCQUEST_VANILLA, RA_JABU_JABUS_BELLY, { Tricks::Tag::NOVICE }, "JbuJigHB");
    OPT_TRICK(RT_LENS_JABU_MQ, RCQUEST_MQ, RA_JABU_JABUS_BELLY, { Tricks::Tag::NOVICE }, "JbuLoT");
    OPT_TRICK(RT_JABU_MQ_RANG_JUMP, RCQUEST_MQ, RA_JABU_JABUS_BELLY, { Tricks::Tag::ADVANCED }, "JbuSwtRng");
    OPT_TRICK(RT_JABU_MQ_SOT_GS, RCQUEST_MQ, RA_JABU_JABUS_BELLY, { Tricks::Tag::INTERMEDIATE }, "JbuSoTRng");
    OPT_TRICK(RT_JABU_BARINADE_POTS, RCQUEST_BOTH, RA_JABU_JABUS_BELLY, { Tricks::Tag::ADVANCED }, "BariPot");
    OPT_TRICK(RT_LENS_BOTW, RCQUEST_VANILLA, RA_BOTTOM_OF_THE_WELL, { Tricks::Tag::NOVICE }, "BWLoT");
    OPT_TRICK(RT_BOTTOM_OF_THE_WELL_NAVI_DIVE, RCQUEST_BOTH, RA_BOTTOM_OF_THE_WELL,
              { Tricks::Tag::NOVICE, Tricks::Tag::GLITCH }, "KakNviD");
    OPT_TRICK(RT_BOTW_CHILD_DEADHAND, RCQUEST_BOTH, RA_BOTTOM_OF_THE_WELL, { Tricks::Tag::NOVICE }, "DHDiff");
    OPT_TRICK(RT_BOTW_BASEMENT, RCQUEST_VANILLA, RA_BOTTOM_OF_THE_WELL, { Tricks::Tag::NOVICE }, "BWBmbFl");
    // RANDOTODO with doorsanity, this can be relevant in Vanilla
    OPT_TRICK(RT_BOTW_PITS, RCQUEST_MQ, RA_BOTTOM_OF_THE_WELL, { Tricks::Tag::NOVICE }, "BWPitJmp");
    OPT_TRICK(RT_BOTW_MQ_DEADHAND_KEY, RCQUEST_MQ, RA_BOTTOM_OF_THE_WELL, { Tricks::Tag::NOVICE }, "BWKeyRng");
    OPT_TRICK(RT_FOREST_FIRST_GS, RCQUEST_VANILLA, RA_FOREST_TEMPLE, { Tricks::Tag::NOVICE }, "FT1stGS");
    OPT_TRICK(RT_FOREST_COURTYARD_EAST_GS, RCQUEST_VANILLA, RA_FOREST_TEMPLE, { Tricks::Tag::NOVICE }, "FTGSRng");
    OPT_TRICK(RT_FOREST_VINES, RCQUEST_BOTH, RA_FOREST_TEMPLE, { Tricks::Tag::NOVICE }, "FTVineHS");
    OPT_TRICK(RT_FOREST_COURTYARD_LEDGE, RCQUEST_BOTH, RA_FOREST_TEMPLE, { Tricks::Tag::NOVICE }, "FTLdgHB");
    OPT_TRICK(RT_FOREST_DOORFRAME, RCQUEST_BOTH, RA_FOREST_TEMPLE, { Tricks::Tag::ADVANCED }, "FTDoorHB");
    OPT_TRICK(RT_FOREST_OUTSIDE_BACKDOOR, RCQUEST_BOTH, RA_FOREST_TEMPLE, { Tricks::Tag::ADVANCED }, "FTBlkJS");
    OPT_TRICK(RT_FOREST_COURTYARD_HEARTS_BOOMERANG, RCQUEST_BOTH, RA_FOREST_TEMPLE, { Tricks::Tag::NOVICE },
              "FTHrtRng");
    OPT_TRICK(RT_FOREST_WELL_SWIM, RCQUEST_MQ, RA_FOREST_TEMPLE, { Tricks::Tag::NOVICE }, "FTSwim");
    OPT_TRICK(RT_FOREST_MQ_BLOCK_PUZZLE, RCQUEST_MQ, RA_FOREST_TEMPLE, { Tricks::Tag::NOVICE }, "FTBlkChu");
    // Child with hovers cannot do this from the lower floor, and must go to the upper floor which needs goron bracelet.
    // Adult can do this with hammer and KSword, But child cannot.
    OPT_TRICK(RT_FOREST_MQ_JS_HALLWAY_SWITCH, RCQUEST_MQ, RA_FOREST_TEMPLE, { Tricks::Tag::NOVICE }, "FTTwstJS");
    OPT_TRICK(RT_FOREST_MQ_HOOKSHOT_HALLWAY_SWITCH, RCQUEST_MQ, RA_FOREST_TEMPLE, { Tricks::Tag::INTERMEDIATE },
              "FTTwstHS");
    OPT_TRICK(RT_FOREST_MQ_RANG_HALLWAY_SWITCH, RCQUEST_MQ, RA_FOREST_TEMPLE, { Tricks::Tag::INTERMEDIATE },
              "FTTwstRng");
    OPT_TRICK(RT_FOREST_MQ_CHILD_DOORFRAME, RCQUEST_MQ, RA_FOREST_TEMPLE, { Tricks::Tag::NOVICE }, "FTDoorC");
    // Is also used in MQ logic, but has no practical effect there as of now
    OPT_TRICK(RT_FIRE_SOT, RCQUEST_VANILLA, RA_FIRE_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "FISoTSkp");
    OPT_TRICK(RT_FIRE_STRENGTH, RCQUEST_VANILLA, RA_FIRE_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "FIStrSkp");
    OPT_TRICK(RT_FIRE_SCARECROW, RCQUEST_VANILLA, RA_FIRE_TEMPLE, { Tricks::Tag::EXPERT }, "PixelShot");
    OPT_TRICK(RT_FIRE_SKIP_FLAME_WALLS, RCQUEST_VANILLA, RA_FIRE_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "FIRWAL");
    OPT_TRICK(RT_FIRE_MQ_NEAR_BOSS, RCQUEST_MQ, RA_FIRE_TEMPLE, { Tricks::Tag::NOVICE }, "FICrtTor");
    OPT_TRICK(RT_FIRE_MQ_BLOCKED_CHEST, RCQUEST_MQ, RA_FIRE_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "FIHSSkp");
    OPT_TRICK(RT_FIRE_MQ_BK_CHEST, RCQUEST_MQ, RA_FIRE_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "FIBowSkp");
    OPT_TRICK(RT_FIRE_MQ_CLIMB, RCQUEST_MQ, RA_FIRE_TEMPLE, { Tricks::Tag::NOVICE }, "FIFirSkp");
    OPT_TRICK(RT_FIRE_MQ_MAZE_SIDE_ROOM, RCQUEST_MQ, RA_FIRE_TEMPLE, { Tricks::Tag::NOVICE }, "FIBoxSkp");
    OPT_TRICK(RT_FIRE_MQ_MAZE_HOVERS, RCQUEST_MQ, RA_FIRE_TEMPLE, { Tricks::Tag::NOVICE }, "FIMazHB");
    OPT_TRICK(RT_FIRE_MQ_MAZE_JUMP, RCQUEST_MQ, RA_FIRE_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "FIMazJmp");
    OPT_TRICK(RT_FIRE_MQ_ABOVE_MAZE_GS, RCQUEST_MQ, RA_FIRE_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "FIGSLS");
    OPT_TRICK(RT_WATER_HIGH_EMBLEM_JUMP, RCQUEST_BOTH, RA_WATER_TEMPLE, { Tricks::Tag::NOVICE }, "WTHiJmp");
    OPT_TRICK(RT_WATER_LONGSHOT_TORCH, RCQUEST_VANILLA, RA_WATER_TEMPLE, { Tricks::Tag::NOVICE }, "WTTorLS");
    OPT_TRICK(RT_WATER_CRACKED_WALL_HOVERS, RCQUEST_VANILLA, RA_WATER_TEMPLE, { Tricks::Tag::NOVICE }, "WTCrkHB");
    OPT_TRICK(RT_WATER_CRACKED_WALL, RCQUEST_VANILLA, RA_WATER_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "WTCrkJmp");
    OPT_TRICK(RT_WATER_BK_REGION, RCQUEST_VANILLA, RA_WATER_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "WTBKHB");
    OPT_TRICK(RT_WATER_NORTH_BASEMENT_LEDGE_JUMP, RCQUEST_BOTH, RA_WATER_TEMPLE, { Tricks::Tag::INTERMEDIATE },
              "WTBolLdg");
    // Also used in MQ logic, but won't be relevant unless a way to enter tower without irons exists (likely a clip +
    // swim)
    OPT_TRICK(RT_WATER_FW_CENTRAL_GS, RCQUEST_VANILLA, RA_WATER_TEMPLE, { Tricks::Tag::NOVICE }, "WTGSFW");
    OPT_TRICK(RT_WATER_IRONS_CENTRAL_GS, RCQUEST_VANILLA, RA_WATER_TEMPLE, { Tricks::Tag::NOVICE }, "WTGSIB");
    OPT_TRICK(RT_WATER_CENTRAL_BOW, RCQUEST_VANILLA, RA_WATER_TEMPLE, { Tricks::Tag::ADVANCED }, "WTBowJmp");
    OPT_TRICK(RT_WATER_HOOKSHOT_FALLING_PLATFORM_GS, RCQUEST_VANILLA, RA_WATER_TEMPLE, { Tricks::Tag::NOVICE },
              "WTWfalHS");
    OPT_TRICK(RT_WATER_RANG_FALLING_PLATFORM_GS, RCQUEST_VANILLA, RA_WATER_TEMPLE, { Tricks::Tag::INTERMEDIATE },
              "WTWfalRng");
    OPT_TRICK(RT_WATER_RIVER_GS, RCQUEST_VANILLA, RA_WATER_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "WTRvrLS");
    OPT_TRICK(RT_WATER_DRAGON_JUMP_DIVE, RCQUEST_BOTH, RA_WATER_TEMPLE, { Tricks::Tag::NOVICE }, "WTDrgJmp");
    OPT_TRICK(RT_WATER_ADULT_DRAGON, RCQUEST_VANILLA, RA_WATER_TEMPLE, { Tricks::Tag::NOVICE }, "WTDrgA");
    OPT_TRICK(RT_WATER_CHILD_DRAGON, RCQUEST_VANILLA, RA_WATER_TEMPLE, { Tricks::Tag::ADVANCED }, "WTDrgC");
    OPT_TRICK(RT_WATER_MQ_CENTRAL_PILLAR, RCQUEST_MQ, RA_WATER_TEMPLE, { Tricks::Tag::NOVICE }, "WTCntFA");
    OPT_TRICK(RT_WATER_IRON_BOOTS_LEDGE_GRAB, RCQUEST_BOTH, RA_WATER_TEMPLE,
              { Tricks::Tag::NOVICE, Tricks::Tag::GLITCH }, "IBSrfLG");
    OPT_TRICK(RT_WATER_INVISIBLE_HOOKSHOT_TARGET, RCQUEST_BOTH, RA_WATER_TEMPLE, { Tricks::Tag::NOVICE }, "WTTarg");
    OPT_TRICK(RT_WATER_MORPHA_WITHOUT_HOOKSHOT, RCQUEST_BOTH, RA_WATER_TEMPLE, { Tricks::Tag::EXTREME }, "MorphDiff");
    OPT_TRICK(RT_LENS_SHADOW, RCQUEST_VANILLA, RA_SHADOW_TEMPLE, { Tricks::Tag::NOVICE }, "STStLoT");
    OPT_TRICK(RT_LENS_SHADOW_PLATFORM, RCQUEST_VANILLA, RA_SHADOW_TEMPLE, { Tricks::Tag::NOVICE }, "STMvLot");
    OPT_TRICK(RT_LENS_BONGO, RCQUEST_BOTH, RA_SHADOW_TEMPLE, { Tricks::Tag::NOVICE }, "BNGLoT");
    OPT_TRICK(RT_SHADOW_UMBRELLA_HOVER, RCQUEST_BOTH, RA_SHADOW_TEMPLE, { Tricks::Tag::EXPERT }, "STUmbSkp");
    OPT_TRICK(RT_SHADOW_UMBRELLA_CLIP, RCQUEST_BOTH, RA_SHADOW_TEMPLE, { Tricks::Tag::NOVICE, Tricks::Tag::GLITCH },
              "STUmbClp");
    OPT_TRICK(RT_SHADOW_UMBRELLA_GS, RCQUEST_BOTH, RA_SHADOW_TEMPLE, { Tricks::Tag::EXPERT }, "STUmbHB");
    OPT_TRICK(RT_SHADOW_FREESTANDING_KEY, RCQUEST_VANILLA, RA_SHADOW_TEMPLE, { Tricks::Tag::NOVICE }, "STPotChu");
    OPT_TRICK(RT_SHADOW_STATUE, RCQUEST_BOTH, RA_SHADOW_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "STStaChu");
    OPT_TRICK(RT_SHADOW_BONGO, RCQUEST_BOTH, RA_SHADOW_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "BngNoPrg");
    OPT_TRICK(RT_LENS_SHADOW_MQ, RCQUEST_MQ, RA_SHADOW_TEMPLE, { Tricks::Tag::NOVICE }, "STMQStLoT");
    OPT_TRICK(RT_LENS_SHADOW_MQ_INVISIBLE_BLADES, RCQUEST_MQ, RA_SHADOW_TEMPLE, { Tricks::Tag::NOVICE }, "STBldLoT");
    OPT_TRICK(RT_LENS_SHADOW_MQ_PLATFORM, RCQUEST_MQ, RA_SHADOW_TEMPLE, { Tricks::Tag::NOVICE }, "STMQMvLot");
    OPT_TRICK(RT_LENS_SHADOW_MQ_DEADHAND, RCQUEST_MQ, RA_SHADOW_TEMPLE, { Tricks::Tag::NOVICE }, "STDHLoT");
    OPT_TRICK(RT_SHADOW_MQ_GAP, RCQUEST_MQ, RA_SHADOW_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "STTSLS");
    OPT_TRICK(RT_SHADOW_MQ_INVISIBLE_BLADES, RCQUEST_MQ, RA_SHADOW_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "STSoTSkp");
    OPT_TRICK(RT_SHADOW_MQ_HUGE_PIT, RCQUEST_MQ, RA_SHADOW_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "STPitJmp");
    OPT_TRICK(RT_SHADOW_MQ_WINDY_WALKWAY, RCQUEST_MQ, RA_SHADOW_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "STWindSkp");
    OPT_TRICK(RT_LENS_SPIRIT, RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, { Tricks::Tag::NOVICE }, "SPLoT");
    OPT_TRICK(RT_SPIRIT_CHILD_CHU, RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, { Tricks::Tag::NOVICE }, "SPBrgChu");
    OPT_TRICK(RT_SPIRIT_WEST_LEDGE, RCQUEST_BOTH, RA_SPIRIT_TEMPLE, { Tricks::Tag::NOVICE }, "SPWeRng");
    OPT_TRICK(RT_SPIRIT_LOWER_ADULT_SWITCH, RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, { Tricks::Tag::ADVANCED }, "SPSwtBmb");
    OPT_TRICK(RT_SPIRIT_STATUE_JUMP, RCQUEST_BOTH, RA_SPIRIT_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "SPHndJmp");

    // disabled since "Spirit Temple boss shortcuts" (pre-lowers the platform where you break the statues face) isn't a
    // setting in ship

    // OPT_TRICK(RT_SPIRIT_PLATFORM_HOOKSHOT, RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, { Tricks::Tag::INTERMEDIATE });

    OPT_TRICK(RT_SPIRIT_MAP_CHEST, RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, { Tricks::Tag::NOVICE }, "SPMapBow");
    OPT_TRICK(RT_SPIRIT_SUN_CHEST, RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, { Tricks::Tag::ADVANCED }, "SPSUNBOW");
    OPT_TRICK(RT_SPIRIT_WALL, RCQUEST_VANILLA, RA_SPIRIT_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "SPWall");
    OPT_TRICK(RT_LENS_SPIRIT_MQ, RCQUEST_MQ, RA_SPIRIT_TEMPLE, { Tricks::Tag::NOVICE }, "SPMQLoT");
    OPT_TRICK(RT_SPIRIT_MQ_SUN_BLOCK_SOT, RCQUEST_MQ, RA_SPIRIT_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "SPBluSkp");
    OPT_TRICK(RT_SPIRIT_MQ_SUN_BLOCK_GS, RCQUEST_MQ, RA_SPIRIT_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "SPBlkGS");
    OPT_TRICK(RT_SPIRIT_MQ_LOWER_ADULT, RCQUEST_MQ, RA_SPIRIT_TEMPLE, { Tricks::Tag::INTERMEDIATE }, "SPTorDin");
    OPT_TRICK(RT_SPIRIT_MQ_FROZEN_EYE, RCQUEST_MQ, RA_SPIRIT_TEMPLE, { Tricks::Tag::NOVICE }, "SPFEBow");
    OPT_TRICK(RT_ICE_STALAGMITE_CLIP, RCQUEST_BOTH, RA_ICE_CAVERN, { Tricks::Tag::NOVICE }, "StalClp");
    OPT_TRICK(RT_ICE_STALAGMITE_HOOKSHOT, RCQUEST_BOTH, RA_ICE_CAVERN, { Tricks::Tag::NOVICE }, "StalHS");
    OPT_TRICK(RT_ICE_BLOCK_GS, RCQUEST_VANILLA, RA_ICE_CAVERN, { Tricks::Tag::INTERMEDIATE }, "ICBlkHB");
    OPT_TRICK(RT_ICE_MQ_RED_ICE_GS, RCQUEST_MQ, RA_ICE_CAVERN, { Tricks::Tag::INTERMEDIATE }, "ICNoSoT");
    OPT_TRICK(RT_LENS_GTG, RCQUEST_VANILLA, RA_GERUDO_TRAINING_GROUND, { Tricks::Tag::NOVICE }, "GTGLoT");
    OPT_TRICK(RT_GTG_WITHOUT_HOOKSHOT, RCQUEST_VANILLA, RA_GERUDO_TRAINING_GROUND, { Tricks::Tag::INTERMEDIATE },
              "GTGNoHS");
    OPT_TRICK(RT_GTG_FAKE_WALL, RCQUEST_BOTH, RA_GERUDO_TRAINING_GROUND, { Tricks::Tag::NOVICE }, "GTGWallHB");
    OPT_TRICK(RT_GTG_LAVA_JUMP, RCQUEST_BOTH, RA_GERUDO_TRAINING_GROUND, { Tricks::Tag::INTERMEDIATE }, "GTGLavaJmp");
    OPT_TRICK(RT_GTG_STATUE_JUMP, RCQUEST_BOTH, RA_GERUDO_TRAINING_GROUND, { Tricks::Tag::INTERMEDIATE }, "GTGStJmp");
    OPT_TRICK(RT_LENS_GTG_MQ, RCQUEST_MQ, RA_GERUDO_TRAINING_GROUND, { Tricks::Tag::NOVICE }, "GTGMQLoT");
    OPT_TRICK(RT_GTG_MQ_WITH_HOOKSHOT, RCQUEST_MQ, RA_GERUDO_TRAINING_GROUND, { Tricks::Tag::NOVICE }, "GTGMQHS");
    OPT_TRICK(RT_GTG_MQ_WITHOUT_HOOKSHOT, RCQUEST_MQ, RA_GERUDO_TRAINING_GROUND, { Tricks::Tag::INTERMEDIATE },
              "GTGMQNoHS");
    OPT_TRICK(RT_GTG_MQ_LAVA_ROOM, RCQUEST_MQ, RA_GERUDO_TRAINING_GROUND, { Tricks::Tag::INTERMEDIATE }, "GTGMQLava");
    OPT_TRICK(RT_LENS_GANON, RCQUEST_VANILLA, RA_GANONS_CASTLE, { Tricks::Tag::NOVICE }, "GCLoT");
    OPT_TRICK(RT_GANON_SPIRIT_TRIAL_HOOKSHOT, RCQUEST_VANILLA, RA_GANONS_CASTLE, { Tricks::Tag::NOVICE }, "GCNoHS");
    OPT_TRICK(RT_LENS_GANON_MQ, RCQUEST_MQ, RA_GANONS_CASTLE, { Tricks::Tag::NOVICE }, "GCMQLoT");
    OPT_TRICK(RT_GANON_MQ_FIRE_TRIAL, RCQUEST_MQ, RA_GANONS_CASTLE, { Tricks::Tag::ADVANCED }, "GCFTHS");
    OPT_TRICK(RT_GANON_MQ_SHADOW_TRIAL, RCQUEST_MQ, RA_GANONS_CASTLE, { Tricks::Tag::NOVICE }, "GCSTBow");
    OPT_TRICK(RT_GANON_MQ_LIGHT_TRIAL, RCQUEST_MQ, RA_GANONS_CASTLE, { Tricks::Tag::INTERMEDIATE }, "GCFirWal");

    for (auto trick : mTrickSettings) {
        if (trick.GetNameTag() != "") {
            if (StaticData::trickToEnum.contains(trick.GetNameTag())) {
                SPDLOG_ERROR("REPEATED TRICK NAME TAG " + trick.GetName());
                assert(false);
            } else {
                StaticData::trickToEnum[trick.GetNameTag()] = trick.GetKey();
            }
        }
    }

    mOptionGroups[RSG_LOGIC] = OptionGroup::SubGroup("Logic Options", {
                                                                          &mOptions[RSK_LOGIC_RULES],
                                                                          &mOptions[RSK_ALL_LOCATIONS_REACHABLE],
                                                                          &mOptions[RSK_SKULLS_SUNS_SONG],
                                                                          &mOptions[RSK_BIG_POE_COUNT],
                                                                      });
    // TODO: Exclude Locations Menus
    mTricksByArea.clear();
    std::vector<Option*> tricksOption;
    tricksOption.reserve(mTrickSettings.size());
    for (int i = 0; i < RT_MAX; i++) {
        auto trick = &mTrickSettings[i];
        if (!trick->GetName().empty()) {
            tricksOption.push_back(trick);
            mTrickNameToEnum[std::string(trick->GetName())] = static_cast<RandomizerTrick>(i);
            mTricksByArea[trick->GetArea()].push_back(static_cast<RandomizerTrick>(i));
        }
    }
    mOptionGroups[RSG_TRICKS] = OptionGroup::SubGroup("Logical Tricks", tricksOption);
    mOptionGroups[RSG_MENU_SECTION_LOGIC] = OptionGroup::SubGroup("Logic",
                                                                  {
                                                                      &mOptions[RSK_LOGIC_RULES],
                                                                      &mOptions[RSK_ALL_LOCATIONS_REACHABLE],
                                                                      &mOptions[RSK_STARTING_AGE],
                                                                      &mOptions[RSK_SKULLS_SUNS_SONG],
                                                                      &mOptions[RSK_BIG_POE_COUNT],
                                                                      &mOptions[RSK_BLUE_FIRE_ARROWS],
                                                                      &mOptions[RSK_SUNLIGHT_ARROWS],
                                                                      &mOptions[RSK_BUNNY_HOOD],
                                                                      &mOptions[RSK_MASKS_AS_ADULT],
                                                                      &mOptions[RSK_FULL_WALLETS],
                                                                      &mOptions[RSK_SLINGBOW_BREAK_BEEHIVES],
                                                                      &mOptions[RSK_SWORDLESS_EPONA_ITEMS],
                                                                      &mOptions[RSK_SKIP_CHILD_STEALTH],
                                                                      &mOptions[RSK_EARLY_GRANNYS_SHOP],
                                                                      &mOptions[RSK_SKIP_PLANTING_BEANS],
                                                                      &mOptions[RSK_SKIP_EPONA_RACE],
                                                                      &mOptions[RSK_SKIP_SCARECROWS_SONG],
                                                                  },
                                                                  WidgetContainerType::SECTION);
    mOptionGroups[RSG_MENU_SECTION_WINCON] = OptionGroup::SubGroup("Win Condition",
                                                                   { &mOptions[RSK_TRIFORCE_HUNT_PIECES_TOTAL],
                                                                     &mOptions[RSK_TRIFORCE_HUNT_PIECES_LOCATION],
                                                                     &mOptions[RSK_GANONS_BOSS_KEY],
                                                                     &mOptions[RSK_GBK_OPTIONS],
                                                                     &mOptions[RSK_GBK_MEDALLION_COUNT],
                                                                     &mOptions[RSK_GBK_STONE_COUNT],
                                                                     &mOptions[RSK_GBK_DUNGEON_COUNT],
                                                                     &mOptions[RSK_GBK_REWARD_COUNT],
                                                                     &mOptions[RSK_GBK_TOKEN_COUNT],
                                                                     &mOptions[RSK_GBK_TRIFORCE_COUNT],
                                                                     &mOptions[RSK_GANONS_SOUL],
                                                                     &mOptions[RSK_GANONS_SOUL_OPTIONS],
                                                                     &mOptions[RSK_GANONS_SOUL_MEDALLION_COUNT],
                                                                     &mOptions[RSK_GANONS_SOUL_STONE_COUNT],
                                                                     &mOptions[RSK_GANONS_SOUL_DUNGEON_COUNT],
                                                                     &mOptions[RSK_GANONS_SOUL_REWARD_COUNT],
                                                                     &mOptions[RSK_GANONS_SOUL_TOKEN_COUNT],
                                                                     &mOptions[RSK_GANONS_SOUL_TRIFORCE_COUNT],
                                                                     &mOptions[RSK_WINCON],
                                                                     &mOptions[RSK_WINCON_OPTIONS],
                                                                     &mOptions[RSK_WINCON_MEDALLION_COUNT],
                                                                     &mOptions[RSK_WINCON_STONE_COUNT],
                                                                     &mOptions[RSK_WINCON_DUNGEON_COUNT],
                                                                     &mOptions[RSK_WINCON_REWARD_COUNT],
                                                                     &mOptions[RSK_WINCON_TOKEN_COUNT],
                                                                     &mOptions[RSK_WINCON_TRIFORCE_COUNT] },
                                                                   WidgetContainerType::SECTION);
    mOptionGroups[RSG_MENU_COLUMN_LOGIC_WINCON] = OptionGroup::SubGroup("",
                                                                        std::initializer_list<OptionGroup*>{
                                                                            &mOptionGroups[RSG_ITEM_POOL],
                                                                            &mOptionGroups[RSG_MENU_SECTION_LOGIC],
                                                                            &mOptionGroups[RSG_MENU_SECTION_WINCON],
                                                                        },
                                                                        WidgetContainerType::COLUMN);
    mOptionGroups[RSG_MENU_SECTION_AREA_ACCESS] =
        OptionGroup::SubGroup("Area Access",
                              {
                                  &mOptions[RSK_FOREST],
                                  &mOptions[RSK_DOOR_OF_TIME],
                                  &mOptions[RSK_ZORAS_FOUNTAIN],
                                  &mOptions[RSK_SLEEPING_WATERFALL],
                                  &mOptions[RSK_JABU_OPEN],
                                  &mOptions[RSK_LOCK_OVERWORLD_DOORS],
                                  &mOptions[RSK_GERUDO_FORTRESS],
                                  &mOptions[RSK_RAINBOW_BRIDGE],
                                  &mOptions[RSK_BRIDGE_OPTIONS],
                                  &mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT],
                                  &mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT],
                                  &mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT],
                                  &mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT],
                                  &mOptions[RSK_RAINBOW_BRIDGE_TOKEN_COUNT],
                                  &mOptions[RSK_RAINBOW_BRIDGE_TRIFORCE_COUNT],
                                  &mOptions[RSK_GANONS_TRIALS],
                                  &mOptions[RSK_TRIAL_COUNT],
                                  &mOptions[RSK_MEDALLION_LOCKED_TRIALS],
                              },
                              WidgetContainerType::SECTION);
    mOptionGroups[RSG_MENU_COLUMN_AREA_ACCESS] =
        OptionGroup::SubGroup("", { &mOptionGroups[RSG_MENU_SECTION_AREA_ACCESS] }, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_MENU_SECTION_ENTRANCES] = OptionGroup::SubGroup(
        "Entrances",
        { &mOptions[RSK_SHUFFLE_DUNGEON_ENTRANCES], &mOptions[RSK_SHUFFLE_BOSS_ENTRANCES],
          &mOptions[RSK_SHUFFLE_GANONS_TOWER_ENTRANCE], &mOptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES],
          &mOptions[RSK_SHUFFLE_INTERIOR_ENTRANCES], &mOptions[RSK_SHUFFLE_THIEVES_HIDEOUT_ENTRANCES],
          &mOptions[RSK_SHUFFLE_GROTTO_ENTRANCES], &mOptions[RSK_SHUFFLE_OWL_DROPS], &mOptions[RSK_SHUFFLE_WARP_SONGS],
          &mOptions[RSK_SHUFFLE_OVERWORLD_SPAWNS], &mOptions[RSK_DECOUPLED_ENTRANCES],
          &mOptions[RSK_MIXED_ENTRANCE_POOLS], &mOptions[RSK_MIX_DUNGEON_ENTRANCES], &mOptions[RSK_MIX_BOSS_ENTRANCES],
          &mOptions[RSK_MIX_OVERWORLD_ENTRANCES], &mOptions[RSK_MIX_INTERIOR_ENTRANCES],
          &mOptions[RSK_MIX_THIEVES_HIDEOUT_ENTRANCES], &mOptions[RSK_MIX_GROTTO_ENTRANCES] },
        WidgetContainerType::SECTION);
    mOptionGroups[RSG_MENU_COLUMN_ENTRANCES] =
        OptionGroup::SubGroup("", { &mOptionGroups[RSG_MENU_SECTION_ENTRANCES] }, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_MENU_SIDEBAR_LOGIC_ACCESS] =
        OptionGroup::SubGroup("Logic/Access",
                              std::initializer_list<OptionGroup*>{ &mOptionGroups[RSG_MENU_COLUMN_LOGIC_WINCON],
                                                                   &mOptionGroups[RSG_MENU_COLUMN_AREA_ACCESS],
                                                                   &mOptionGroups[RSG_MENU_COLUMN_ENTRANCES] },
                              WidgetContainerType::TABLE);
    mOptionGroups[RSG_MENU_SECTION_DUNGEON_ITEMS] = OptionGroup::SubGroup("Dungeon Items",
                                                                          {
                                                                              &mOptions[RSK_SHUFFLE_MAPANDCOMPASS],
                                                                              &mOptions[RSK_KEYSANITY],
                                                                              &mOptions[RSK_BOSS_KEYSANITY],
                                                                              &mOptions[RSK_SHUFFLE_DUNGEON_REWARDS],
                                                                              &mOptions[RSK_GERUDO_KEYS],
                                                                              &mOptions[RSK_SHUFFLE_BOSS_SOULS],
                                                                          },
                                                                          WidgetContainerType::SECTION);
    mOptionGroups[RSG_MENU_COLUMN_DUNGEON_ITEMS] =
        OptionGroup::SubGroup("", { &mOptionGroups[RSG_MENU_SECTION_DUNGEON_ITEMS] }, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_MENU_SECTION_MQ] = OptionGroup::SubGroup("Master Quest",
                                                               {
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
                                                               },
                                                               WidgetContainerType::SECTION);
    mOptionGroups[RSG_MENU_COLUMN_MQ] =
        OptionGroup::SubGroup("", { &mOptionGroups[RSG_MENU_SECTION_MQ] }, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_MENU_SECTION_KEYRINGS] =
        OptionGroup::SubGroup("Keyrings",
                              { &mOptions[RSK_KEYRINGS], &mOptions[RSK_KEYRINGS_RANDOM_COUNT],
                                &mOptions[RSK_KEYRINGS_FOREST_TEMPLE], &mOptions[RSK_KEYRINGS_FIRE_TEMPLE],
                                &mOptions[RSK_KEYRINGS_WATER_TEMPLE], &mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE],
                                &mOptions[RSK_KEYRINGS_SHADOW_TEMPLE], &mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL],
                                &mOptions[RSK_KEYRINGS_GTG], &mOptions[RSK_KEYRINGS_GANONS_CASTLE],
                                &mOptions[RSK_KEYRINGS_GERUDO_FORTRESS], &mOptions[RSK_KEYRINGS_CHEST_GAME] },
                              WidgetContainerType::SECTION);
    mOptionGroups[RSG_MENU_COLUMN_KEYRINGS] =
        OptionGroup::SubGroup("", { &mOptionGroups[RSG_MENU_SECTION_KEYRINGS] }, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_MENU_SIDEBAR_DUNGEONS] = OptionGroup::SubGroup("Dungeons",
                                                                     std::initializer_list<OptionGroup*>{
                                                                         &mOptionGroups[RSG_MENU_COLUMN_DUNGEON_ITEMS],
                                                                         &mOptionGroups[RSG_MENU_COLUMN_KEYRINGS],
                                                                         &mOptionGroups[RSG_MENU_COLUMN_MQ],
                                                                     },
                                                                     WidgetContainerType::TABLE);
    mOptionGroups[RSG_MENU_SECTION_BASIC_SHUFFLES] =
        OptionGroup::SubGroup("Shuffle Items",
                              {
                                  &mOptions[RSK_SHUFFLE_SONGS],         &mOptions[RSK_SHUFFLE_TOKENS],
                                  &mOptions[RSK_SHUFFLE_KOKIRI_SWORD],  &mOptions[RSK_SHUFFLE_MASTER_SWORD],
                                  &mOptions[RSK_SHUFFLE_OCARINA],       &mOptions[RSK_SHUFFLE_WEIRD_EGG],
                                  &mOptions[RSK_SHUFFLE_ZELDAS_LETTER], &mOptions[RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD],
                                  &mOptions[RSK_SHUFFLE_MASKS],         &mOptions[RSK_FISHSANITY],
                                  &mOptions[RSK_FISHSANITY_POND_COUNT], &mOptions[RSK_FISHSANITY_AGE_SPLIT],
                                  &mOptions[RSK_SHUFFLE_FREESTANDING],  &mOptions[RSK_SHUFFLE_WONDER_ITEMS],
                                  &mOptions[RSK_SHUFFLE_SILVER],        &mOptions[RSK_SHUFFLE_BEEHIVES],
                                  &mOptions[RSK_SHUFFLE_COWS],          &mOptions[RSK_SHUFFLE_POTS],
                                  &mOptions[RSK_SHUFFLE_GRASS],         &mOptions[RSK_SHUFFLE_CRATES],
                                  &mOptions[RSK_SHUFFLE_BOULDERS],      &mOptions[RSK_SHUFFLE_ROCKS],
                                  &mOptions[RSK_SHUFFLE_TREES],         &mOptions[RSK_SHUFFLE_BUSHES],
                                  &mOptions[RSK_SHUFFLE_ICICLES],       &mOptions[RSK_SHUFFLE_RED_ICE],
                                  &mOptions[RSK_SHUFFLE_SIGNS],         &mOptions[RSK_SHUFFLE_FROG_SONG_RUPEES],
                                  &mOptions[RSK_SHUFFLE_ADULT_TRADE],   &mOptions[RSK_SHUFFLE_CHEST_MINIGAME],
                                  &mOptions[RSK_SHUFFLE_100_GS_REWARD], &mOptions[RSK_SHUFFLE_FOUNTAIN_FAIRIES],
                                  &mOptions[RSK_SHUFFLE_STONE_FAIRIES], &mOptions[RSK_SHUFFLE_BEAN_FAIRIES],
                                  &mOptions[RSK_SHUFFLE_SONG_FAIRIES],  &mOptions[RSK_SHUFFLE_BUTTERFLY_FAIRIES],
                              },
                              WidgetContainerType::SECTION);
    mOptionGroups[RSG_MENU_COLUMN_BASIC_SHUFFLES] =
        OptionGroup::SubGroup("", { &mOptionGroups[RSG_MENU_SECTION_BASIC_SHUFFLES] }, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_MENU_SECTION_SHOP_SHUFFLES] =
        OptionGroup::SubGroup("Shuffle Shops & Merchants",
                              {
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
                                  &mOptions[RSK_SHOP_SHIELDS_AND_TUNICS_ONLY_REFILL],
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
                                  &mOptions[RSK_SHUFFLE_BEGGAR],
                              },
                              WidgetContainerType::SECTION);
    mOptionGroups[RSG_MENU_COLUMN_SHOP_SHUFFLES] =
        OptionGroup::SubGroup("", { &mOptionGroups[RSG_MENU_SECTION_SHOP_SHUFFLES] }, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_MENU_SECTION_ADDITIONAL_ITEMS] = OptionGroup::SubGroup("Additional Items",
                                                                             {
                                                                                 &mOptions[RSK_SHUFFLE_CHILD_WALLET],
                                                                                 &mOptions[RSK_INCLUDE_TYCOON_WALLET],
                                                                                 &mOptions[RSK_SHUFFLE_FISHING_POLE],
                                                                                 &mOptions[RSK_SHUFFLE_DEKU_STICK_BAG],
                                                                                 &mOptions[RSK_SHUFFLE_DEKU_NUT_BAG],
                                                                                 &mOptions[RSK_SHUFFLE_OCARINA_BUTTONS],
                                                                                 &mOptions[RSK_SHUFFLE_SWIM],
                                                                                 &mOptions[RSK_SHUFFLE_GRAB],
                                                                                 &mOptions[RSK_SHUFFLE_CLIMB],
                                                                                 &mOptions[RSK_SHUFFLE_CRAWL],
                                                                                 &mOptions[RSK_SHUFFLE_SPEAK],
                                                                                 &mOptions[RSK_SHUFFLE_OPEN_CHEST],
                                                                                 &mOptions[RSK_SHUFFLE_BEAN_SOULS],
                                                                                 &mOptions[RSK_ROCS_FEATHER],
                                                                                 &mOptions[RSK_BOMBCHU_BAG],
                                                                                 &mOptions[RSK_ENABLE_BOMBCHU_DROPS],
                                                                                 &mOptions[RSK_PROGRESSIVE_GORON_SWORD],
                                                                                 &mOptions[RSK_INFINITE_UPGRADES],
                                                                                 &mOptions[RSK_SKELETON_KEY],
                                                                             },
                                                                             WidgetContainerType::SECTION);
    mOptionGroups[RSG_MENU_COLUMN_ADDITIONAL_ITEMS] =
        OptionGroup::SubGroup("", { &mOptionGroups[RSG_MENU_SECTION_ADDITIONAL_ITEMS] }, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_MENU_SIDEBAR_SHUFFLES] =
        OptionGroup::SubGroup("Shuffles",
                              {
                                  &mOptionGroups[RSG_MENU_COLUMN_BASIC_SHUFFLES],
                                  &mOptionGroups[RSG_MENU_COLUMN_SHOP_SHUFFLES],
                                  &mOptionGroups[RSG_MENU_COLUMN_ADDITIONAL_ITEMS],
                              },
                              WidgetContainerType::TABLE);
    mOptionGroups[RSG_MENU_SECTION_HINTS] = OptionGroup::SubGroup("Hints",
                                                                  {
                                                                      &mOptions[RSK_GOSSIP_STONE_HINTS],
                                                                      &mOptions[RSK_HINT_CLARITY],
                                                                      &mOptions[RSK_HINT_DISTRIBUTION],
                                                                  },
                                                                  WidgetContainerType::SECTION);
    mOptionGroups[RSG_MENU_SECTION_TRAPS] = OptionGroup::SubGroup("Traps",
                                                                  {
                                                                      &mOptions[RSK_BASE_ICE_TRAPS],
                                                                      &mOptions[RSK_ADDITIONAL_ICE_TRAPS],
                                                                      &mOptions[RSK_ICE_TRAP_PERCENT],
                                                                      &mOptions[RSK_ICE_TRAP_NAMES],
                                                                  },
                                                                  WidgetContainerType::SECTION);
    mOptionGroups[RSG_MENU_COLUMN_HINTS_TRAPS] =
        OptionGroup::SubGroup("",
                              std::initializer_list<OptionGroup*>{ &mOptionGroups[RSG_MENU_SECTION_HINTS],
                                                                   &mOptionGroups[RSG_MENU_SECTION_TRAPS] },
                              WidgetContainerType::COLUMN);
    mOptionGroups[RSG_MENU_SECTION_STATIC_HINTS] = OptionGroup::SubGroup(
        "Static Hints", { &mOptions[RSK_TOT_ALTAR_HINT],     &mOptions[RSK_GANONDORF_HINT],
                          &mOptions[RSK_SHEIK_LA_HINT],      &mOptions[RSK_BOSS_KEY_HINT],
                          &mOptions[RSK_DAMPES_DIARY_HINT],  &mOptions[RSK_GREG_HINT],
                          &mOptions[RSK_LOACH_HINT],         &mOptions[RSK_SARIA_HINT],
                          &mOptions[RSK_MIDO_HINT],          &mOptions[RSK_FROGS_HINT],
                          &mOptions[RSK_OOT_HINT],           &mOptions[RSK_BIGGORON_HINT],
                          &mOptions[RSK_BIG_POES_HINT],      &mOptions[RSK_CHICKENS_HINT],
                          &mOptions[RSK_MALON_HINT],         &mOptions[RSK_HBA_HINT],
                          &mOptions[RSK_FISHING_POLE_HINT],  &mOptions[RSK_WARP_SONG_HINTS],
                          &mOptions[RSK_SCRUB_TEXT_HINT],    &mOptions[RSK_MERCHANT_TEXT_HINT],
                          &mOptions[RSK_KAK_10_SKULLS_HINT], &mOptions[RSK_KAK_20_SKULLS_HINT],
                          &mOptions[RSK_KAK_30_SKULLS_HINT], &mOptions[RSK_KAK_40_SKULLS_HINT],
                          &mOptions[RSK_KAK_50_SKULLS_HINT], &mOptions[RSK_KAK_100_SKULLS_HINT],
                          &mOptions[RSK_MASK_SHOP_HINT] },
        WidgetContainerType::SECTION, "This setting adds some hints at locations other than Gossip Stones.");
    mOptionGroups[RSG_MENU_COLUMN_STATIC_HINTS] =
        OptionGroup::SubGroup("", { &mOptionGroups[RSG_MENU_SECTION_STATIC_HINTS] }, WidgetContainerType::COLUMN);
    mOptionGroups[RSG_MENU_SIDEBAR_HINTS_TRAPS] =
        OptionGroup::SubGroup("Hints/Traps",
                              std::initializer_list<OptionGroup*>{
                                  &mOptionGroups[RSG_MENU_COLUMN_HINTS_TRAPS],
                                  &mOptionGroups[RSG_MENU_COLUMN_STATIC_HINTS],
                              },
                              WidgetContainerType::TABLE);
    mOptionGroups[RSG_OPEN] = OptionGroup("Open Settings", {
                                                               &mOptions[RSK_FOREST],
                                                               &mOptions[RSK_DOOR_OF_TIME],
                                                               &mOptions[RSK_ZORAS_FOUNTAIN],
                                                               &mOptions[RSK_SLEEPING_WATERFALL],
                                                               &mOptions[RSK_JABU_OPEN],
                                                               &mOptions[RSK_LOCK_OVERWORLD_DOORS],
                                                               &mOptions[RSK_GERUDO_FORTRESS],
                                                               &mOptions[RSK_RAINBOW_BRIDGE],
                                                               &mOptions[RSK_RAINBOW_BRIDGE_STONE_COUNT],
                                                               &mOptions[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT],
                                                               &mOptions[RSK_RAINBOW_BRIDGE_REWARD_COUNT],
                                                               &mOptions[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT],
                                                               &mOptions[RSK_RAINBOW_BRIDGE_TOKEN_COUNT],
                                                               &mOptions[RSK_RAINBOW_BRIDGE_TRIFORCE_COUNT],
                                                               &mOptions[RSK_BRIDGE_OPTIONS],
                                                               &mOptions[RSK_GANONS_TRIALS],
                                                               &mOptions[RSK_TRIAL_COUNT],
                                                               &mOptions[RSK_MEDALLION_LOCKED_TRIALS],
                                                           });
    mOptionGroups[RSG_WORLD] = OptionGroup("World Settings", {
                                                                 &mOptions[RSK_STARTING_AGE],
                                                                 &mOptions[RSK_SHUFFLE_ENTRANCES],
                                                                 &mOptions[RSK_SHUFFLE_DUNGEON_ENTRANCES],
                                                                 &mOptions[RSK_SHUFFLE_BOSS_ENTRANCES],
                                                                 &mOptions[RSK_SHUFFLE_GANONS_TOWER_ENTRANCE],
                                                                 &mOptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES],
                                                                 &mOptions[RSK_SHUFFLE_INTERIOR_ENTRANCES],
                                                                 &mOptions[RSK_SHUFFLE_THIEVES_HIDEOUT_ENTRANCES],
                                                                 &mOptions[RSK_SHUFFLE_GROTTO_ENTRANCES],
                                                                 &mOptions[RSK_SHUFFLE_OWL_DROPS],
                                                                 &mOptions[RSK_SHUFFLE_WARP_SONGS],
                                                                 &mOptions[RSK_SHUFFLE_OVERWORLD_SPAWNS],
                                                                 &mOptions[RSK_MIXED_ENTRANCE_POOLS],
                                                                 &mOptions[RSK_MIX_DUNGEON_ENTRANCES],
                                                                 &mOptions[RSK_MIX_BOSS_ENTRANCES],
                                                                 &mOptions[RSK_MIX_OVERWORLD_ENTRANCES],
                                                                 &mOptions[RSK_MIX_INTERIOR_ENTRANCES],
                                                                 &mOptions[RSK_MIX_THIEVES_HIDEOUT_ENTRANCES],
                                                                 &mOptions[RSK_MIX_GROTTO_ENTRANCES],
                                                                 &mOptions[RSK_DECOUPLED_ENTRANCES],
                                                                 &mOptions[RSK_BOMBCHU_BAG],
                                                                 &mOptions[RSK_ENABLE_BOMBCHU_DROPS],
                                                                 &mOptions[RSK_TRIFORCE_HUNT_PIECES_TOTAL],
                                                                 &mOptions[RSK_TRIFORCE_HUNT_PIECES_LOCATION],
                                                                 &mOptions[RSK_MQ_DUNGEON_RANDOM],
                                                                 &mOptions[RSK_MQ_DUNGEON_COUNT],
                                                                 &mOptions[RSK_MQ_DUNGEON_SET],
                                                             });
    mOptionGroups[RSG_SHUFFLE_DUNGEON_QUESTS] = OptionGroup::SubGroup(
        "Shuffle Dungeon Quest",
        { &mOptions[RSK_MQ_DEKU_TREE], &mOptions[RSK_MQ_DODONGOS_CAVERN], &mOptions[RSK_MQ_JABU_JABU],
          &mOptions[RSK_MQ_FOREST_TEMPLE], &mOptions[RSK_MQ_FIRE_TEMPLE], &mOptions[RSK_MQ_WATER_TEMPLE],
          &mOptions[RSK_MQ_SPIRIT_TEMPLE], &mOptions[RSK_MQ_SHADOW_TEMPLE], &mOptions[RSK_MQ_BOTTOM_OF_THE_WELL],
          &mOptions[RSK_MQ_ICE_CAVERN], &mOptions[RSK_MQ_GTG], &mOptions[RSK_MQ_GANONS_CASTLE] });
    mOptionGroups[RSG_SHUFFLE] =
        OptionGroup("Shuffle Settings", {
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
                                            &mOptions[RSK_SHOP_SHIELDS_AND_TUNICS_ONLY_REFILL],
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
                                            &mOptions[RSK_SHUFFLE_POTS],
                                            &mOptions[RSK_SHUFFLE_GRASS],
                                            &mOptions[RSK_SHUFFLE_CRATES],
                                            &mOptions[RSK_SHUFFLE_BOULDERS],
                                            &mOptions[RSK_SHUFFLE_ROCKS],
                                            &mOptions[RSK_SHUFFLE_TREES],
                                            &mOptions[RSK_SHUFFLE_BUSHES],
                                            &mOptions[RSK_SHUFFLE_ICICLES],
                                            &mOptions[RSK_SHUFFLE_RED_ICE],
                                            &mOptions[RSK_SHUFFLE_SIGNS],
                                            &mOptions[RSK_SHUFFLE_KOKIRI_SWORD],
                                            &mOptions[RSK_SHUFFLE_OCARINA],
                                            &mOptions[RSK_SHUFFLE_OCARINA_BUTTONS],
                                            &mOptions[RSK_SHUFFLE_SWIM],
                                            &mOptions[RSK_SHUFFLE_GRAB],
                                            &mOptions[RSK_SHUFFLE_CLIMB],
                                            &mOptions[RSK_SHUFFLE_CRAWL],
                                            &mOptions[RSK_SHUFFLE_SPEAK],
                                            &mOptions[RSK_SHUFFLE_OPEN_CHEST],
                                            &mOptions[RSK_SHUFFLE_WEIRD_EGG],
                                            &mOptions[RSK_SHUFFLE_ZELDAS_LETTER],
                                            &mOptions[RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD],
                                            &mOptions[RSK_SHUFFLE_MASKS],
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
                                            &mOptions[RSK_SHUFFLE_BEGGAR],
                                            &mOptions[RSK_SHUFFLE_FROG_SONG_RUPEES],
                                            &mOptions[RSK_SHUFFLE_ADULT_TRADE],
                                            &mOptions[RSK_SHUFFLE_CHEST_MINIGAME],
                                            &mOptions[RSK_SHUFFLE_100_GS_REWARD],
                                            &mOptions[RSK_SHUFFLE_BEAN_SOULS],
                                            &mOptions[RSK_ROCS_FEATHER],
                                            &mOptions[RSK_PROGRESSIVE_GORON_SWORD],
                                            &mOptions[RSK_SHUFFLE_BOSS_SOULS],
                                            &mOptions[RSK_SHUFFLE_DEKU_STICK_BAG],
                                            &mOptions[RSK_SHUFFLE_DEKU_NUT_BAG],
                                            &mOptions[RSK_SHUFFLE_FREESTANDING],
                                            &mOptions[RSK_SHUFFLE_WONDER_ITEMS],
                                            &mOptions[RSK_SHUFFLE_SILVER],
                                            &mOptions[RSK_SHUFFLE_FOUNTAIN_FAIRIES],
                                            &mOptions[RSK_SHUFFLE_STONE_FAIRIES],
                                            &mOptions[RSK_SHUFFLE_BEAN_FAIRIES],
                                            &mOptions[RSK_SHUFFLE_SONG_FAIRIES],
                                            &mOptions[RSK_SHUFFLE_BUTTERFLY_FAIRIES],
                                        });
    mOptionGroups[RSG_SHUFFLE_DUNGEON_ITEMS] =
        OptionGroup("Shuffle Dungeon Items", {
                                                 &mOptions[RSK_SHUFFLE_MAPANDCOMPASS],
                                                 &mOptions[RSK_KEYSANITY],
                                                 &mOptions[RSK_GERUDO_KEYS],
                                                 &mOptions[RSK_BOSS_KEYSANITY],
                                                 &mOptions[RSK_GANONS_BOSS_KEY],
                                                 &mOptions[RSK_GBK_STONE_COUNT],
                                                 &mOptions[RSK_GBK_MEDALLION_COUNT],
                                                 &mOptions[RSK_GBK_DUNGEON_COUNT],
                                                 &mOptions[RSK_GBK_REWARD_COUNT],
                                                 &mOptions[RSK_GBK_TOKEN_COUNT],
                                                 &mOptions[RSK_GBK_TRIFORCE_COUNT],
                                                 &mOptions[RSK_GBK_OPTIONS],
                                                 &mOptions[RSK_GANONS_SOUL],
                                                 &mOptions[RSK_GANONS_SOUL_STONE_COUNT],
                                                 &mOptions[RSK_GANONS_SOUL_MEDALLION_COUNT],
                                                 &mOptions[RSK_GANONS_SOUL_DUNGEON_COUNT],
                                                 &mOptions[RSK_GANONS_SOUL_REWARD_COUNT],
                                                 &mOptions[RSK_GANONS_SOUL_TOKEN_COUNT],
                                                 &mOptions[RSK_GANONS_SOUL_TRIFORCE_COUNT],
                                                 &mOptions[RSK_GANONS_SOUL_OPTIONS],
                                                 &mOptions[RSK_WINCON],
                                                 &mOptions[RSK_WINCON_STONE_COUNT],
                                                 &mOptions[RSK_WINCON_MEDALLION_COUNT],
                                                 &mOptions[RSK_WINCON_DUNGEON_COUNT],
                                                 &mOptions[RSK_WINCON_REWARD_COUNT],
                                                 &mOptions[RSK_WINCON_TOKEN_COUNT],
                                                 &mOptions[RSK_WINCON_TRIFORCE_COUNT],
                                                 &mOptions[RSK_WINCON_OPTIONS],
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
                                                 &mOptions[RSK_KEYRINGS_CHEST_GAME],
                                             });
    mOptionGroups[RSG_STARTING_ITEMS] = OptionGroup::SubGroup(
        "Items", { &mOptions[RSK_STARTING_OCARINA],        &mOptions[RSK_STARTING_KOKIRI_SWORD],
                   &mOptions[RSK_STARTING_MASTER_SWORD],   &mOptions[RSK_STARTING_DEKU_SHIELD],
                   &mOptions[RSK_STARTING_HYLIAN_SHIELD],  &mOptions[RSK_STARTING_MIRROR_SHIELD],
                   &mOptions[RSK_STARTING_GORON_TUNIC],    &mOptions[RSK_STARTING_ZORA_TUNIC],
                   &mOptions[RSK_STARTING_IRON_BOOTS],     &mOptions[RSK_STARTING_HOVER_BOOTS],
                   &mOptions[RSK_STARTING_MEGATON_HAMMER], &mOptions[RSK_STARTING_BOOMERANG],
                   &mOptions[RSK_STARTING_LENS_OF_TRUTH],  &mOptions[RSK_STARTING_DINS_FIRE],
                   &mOptions[RSK_STARTING_FARORES_WIND],   &mOptions[RSK_STARTING_NAYRUS_LOVE],
                   &mOptions[RSK_STARTING_FIRE_ARROWS],    &mOptions[RSK_STARTING_ICE_ARROWS],
                   &mOptions[RSK_STARTING_LIGHT_ARROWS],   &mOptions[RSK_STARTING_STONE_OF_AGONY],
                   &mOptions[RSK_STARTING_HOOKSHOT],       &mOptions[RSK_STARTING_BOW],
                   &mOptions[RSK_STARTING_SLINGSHOT],      &mOptions[RSK_STARTING_BOMB_BAG],
                   &mOptions[RSK_STARTING_STRENGTH],       &mOptions[RSK_STARTING_SCALE],
                   &mOptions[RSK_STARTING_WALLET],         &mOptions[RSK_STARTING_MAGIC_METER],
                   &mOptions[RSK_STARTING_BOMBCHU_BAG],    &mOptions[RSK_STARTING_BOTTLE_1],
                   &mOptions[RSK_STARTING_BOTTLE_2],       &mOptions[RSK_STARTING_BOTTLE_3],
                   &mOptions[RSK_STARTING_BOTTLE_4],       &mOptions[RSK_STARTING_WEIRD_EGG],
                   &mOptions[RSK_STARTING_ZELDAS_LETTER],  &mOptions[RSK_STARTING_CLAIM_CHECK],
                   &mOptions[RSK_STARTING_GERUDO_CARD],    &mOptions[RSK_STARTING_BIGGORON_SWORD],
                   &mOptions[RSK_STARTING_BUNNY_HOOD],     &mOptions[RSK_STARTING_KEATON_MASK],
                   &mOptions[RSK_STARTING_SKULL_MASK],     &mOptions[RSK_STARTING_SPOOKY_MASK],
                   &mOptions[RSK_STARTING_GORON_MASK],     &mOptions[RSK_STARTING_ZORA_MASK],
                   &mOptions[RSK_STARTING_GERUDO_MASK],    &mOptions[RSK_STARTING_MASK_OF_TRUTH] });
    mOptionGroups[RSG_STARTING_SONGS] =
        OptionGroup::SubGroup("Ocarina Songs", {
                                                   &mOptions[RSK_STARTING_ZELDAS_LULLABY],
                                                   &mOptions[RSK_STARTING_EPONAS_SONG],
                                                   &mOptions[RSK_STARTING_SARIAS_SONG],
                                                   &mOptions[RSK_STARTING_SUNS_SONG],
                                                   &mOptions[RSK_STARTING_SONG_OF_TIME],
                                                   &mOptions[RSK_STARTING_SONG_OF_STORMS],
                                                   &mOptions[RSK_STARTING_MINUET_OF_FOREST],
                                                   &mOptions[RSK_STARTING_BOLERO_OF_FIRE],
                                                   &mOptions[RSK_STARTING_SERENADE_OF_WATER],
                                                   &mOptions[RSK_STARTING_REQUIEM_OF_SPIRIT],
                                                   &mOptions[RSK_STARTING_NOCTURNE_OF_SHADOW],
                                                   &mOptions[RSK_STARTING_PRELUDE_OF_LIGHT],
                                               });
    mOptionGroups[RSG_STARTING_OTHER] = OptionGroup::SubGroup("Other", {
                                                                           &mOptions[RSK_STARTING_STICKS],
                                                                           &mOptions[RSK_STARTING_NUTS],
                                                                           &mOptions[RSK_STARTING_BEANS],
                                                                           &mOptions[RSK_FULL_WALLETS],
                                                                           &mOptions[RSK_STARTING_SKULLTULA_TOKEN],
                                                                           &mOptions[RSK_STARTING_HEARTS],
                                                                       });
    mOptionGroups[RSG_STARTING_INVENTORY] = OptionGroup("Starting Inventory",
                                                        {
                                                            &mOptionGroups[RSG_STARTING_ITEMS],
                                                            &mOptionGroups[RSG_STARTING_SONGS],
                                                            &mOptionGroups[RSG_STARTING_OTHER],
                                                        },
                                                        OptionGroupType::DEFAULT);
    mOptionGroups[RSG_TIMESAVERS] = OptionGroup("Timesaver Settings", {
                                                                          &mOptions[RSK_SKIP_EPONA_RACE],
                                                                          &mOptions[RSK_SKIP_SCARECROWS_SONG],
                                                                          &mOptions[RSK_SKIP_PLANTING_BEANS],
                                                                          &mOptions[RSK_BIG_POE_COUNT],
                                                                      });
    mOptionGroups[RSG_MISC] = OptionGroup("Miscellaneous Settings",
                                          {
                                              &mOptions[RSK_GOSSIP_STONE_HINTS],
                                              &mOptions[RSK_HINT_CLARITY],
                                              &mOptions[RSK_HINT_DISTRIBUTION],
                                              &mOptions[RSK_TOT_ALTAR_HINT],
                                              &mOptions[RSK_GANONDORF_HINT],
                                              &mOptions[RSK_SHEIK_LA_HINT],
                                              &mOptions[RSK_BOSS_KEY_HINT],
                                              &mOptions[RSK_DAMPES_DIARY_HINT],
                                              &mOptions[RSK_GREG_HINT],
                                              &mOptions[RSK_LOACH_HINT],
                                              &mOptions[RSK_SARIA_HINT],
                                              &mOptions[RSK_MIDO_HINT],
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
                                              &mOptions[RSK_BUNNY_HOOD],
                                              &mOptions[RSK_MASKS_AS_ADULT],
                                              &mOptions[RSK_INFINITE_UPGRADES],
                                              &mOptions[RSK_SKELETON_KEY],
                                              &mOptions[RSK_SLINGBOW_BREAK_BEEHIVES],
                                          });
    mOptionGroups[RSG_ITEM_POOL] =
        OptionGroup("Item Pool Settings", std::initializer_list<Option*>({ &mOptions[RSK_ITEM_POOL] }));
    // TODO: Progressive Goron Sword, Remove Double Defense
    mOptionGroups[RSG_EXCLUDES_KOKIRI_FOREST] =
        OptionGroup::SubGroup("Kokiri Forest", mExcludeLocationsOptionsAreas[RCAREA_KOKIRI_FOREST]);
    mOptionGroups[RSG_EXCLUDES_LOST_WOODS] =
        OptionGroup::SubGroup("Lost Woods", mExcludeLocationsOptionsAreas[RCAREA_LOST_WOODS]);
    mOptionGroups[RSG_EXCLUDES_SACRED_FOREST_MEADOW] =
        OptionGroup::SubGroup("Sacred Forest Meadow", mExcludeLocationsOptionsAreas[RCAREA_SACRED_FOREST_MEADOW]);
    mOptionGroups[RSG_EXCLUDES_DEKU_TREE] =
        OptionGroup::SubGroup("Deku Tree", mExcludeLocationsOptionsAreas[RCAREA_DEKU_TREE]);
    mOptionGroups[RSG_EXCLUDES_FOREST_TEMPLE] =
        OptionGroup::SubGroup("Forest Temple", mExcludeLocationsOptionsAreas[RCAREA_FOREST_TEMPLE]);
    mOptionGroups[RSG_EXCLUDES_KAKARIKO_VILLAGE] =
        OptionGroup::SubGroup("Kakariko Village", mExcludeLocationsOptionsAreas[RCAREA_KAKARIKO_VILLAGE]);
    mOptionGroups[RSG_EXCLUDES_GRAVEYARD] =
        OptionGroup::SubGroup("Graveyard", mExcludeLocationsOptionsAreas[RCAREA_GRAVEYARD]);
    mOptionGroups[RSG_EXCLUDES_BOTTOM_OF_THE_WELL] =
        OptionGroup::SubGroup("Bottom of the Well", mExcludeLocationsOptionsAreas[RCAREA_BOTTOM_OF_THE_WELL]);
    mOptionGroups[RSG_EXCLUDES_SHADOW_TEMPLE] =
        OptionGroup::SubGroup("Shadow Temple", mExcludeLocationsOptionsAreas[RCAREA_SHADOW_TEMPLE]);
    mOptionGroups[RSG_EXCLUDES_DEATH_MOUNTAIN_TRAIL] =
        OptionGroup::SubGroup("Death Mountain Trail", mExcludeLocationsOptionsAreas[RCAREA_DEATH_MOUNTAIN_TRAIL]);
    mOptionGroups[RSG_EXCLUDES_DEATH_MOUNTAIN_CRATER] =
        OptionGroup::SubGroup("Death Mountain Crater", mExcludeLocationsOptionsAreas[RCAREA_DEATH_MOUNTAIN_CRATER]);
    mOptionGroups[RSG_EXCLUDES_GORON_CITY] =
        OptionGroup::SubGroup("Goron City", mExcludeLocationsOptionsAreas[RCAREA_GORON_CITY]);
    mOptionGroups[RSG_EXCLUDES_DODONGOS_CAVERN] =
        OptionGroup::SubGroup("Dodongo's Cavern", mExcludeLocationsOptionsAreas[RCAREA_DODONGOS_CAVERN]);
    mOptionGroups[RSG_EXCLUDES_FIRE_TEMPLE] =
        OptionGroup::SubGroup("Fire Temple", mExcludeLocationsOptionsAreas[RCAREA_FIRE_TEMPLE]);
    mOptionGroups[RSG_EXCLUDES_ZORAS_RIVER] =
        OptionGroup::SubGroup("Zora's River", mExcludeLocationsOptionsAreas[RCAREA_ZORAS_RIVER]);
    mOptionGroups[RSG_EXCLUDES_ZORAS_DOMAIN] =
        OptionGroup::SubGroup("Zora's Domain", mExcludeLocationsOptionsAreas[RCAREA_ZORAS_DOMAIN]);
    mOptionGroups[RSG_EXCLUDES_ZORAS_FOUNTAIN] =
        OptionGroup::SubGroup("Zora's Fountain", mExcludeLocationsOptionsAreas[RCAREA_ZORAS_FOUNTAIN]);
    mOptionGroups[RSG_EXCLUDES_JABU_JABU] =
        OptionGroup::SubGroup("Jabu Jabu's Belly", mExcludeLocationsOptionsAreas[RCAREA_JABU_JABUS_BELLY]);
    mOptionGroups[RSG_EXCLUDES_ICE_CAVERN] =
        OptionGroup::SubGroup("Ice Cavern", mExcludeLocationsOptionsAreas[RCAREA_ICE_CAVERN]);
    mOptionGroups[RSG_EXCLUDES_HYRULE_FIELD] =
        OptionGroup::SubGroup("Hyrule Field", mExcludeLocationsOptionsAreas[RCAREA_HYRULE_FIELD]);
    mOptionGroups[RSG_EXCLUDES_LON_LON_RANCH] =
        OptionGroup::SubGroup("Lon Lon Ranch", mExcludeLocationsOptionsAreas[RCAREA_LON_LON_RANCH]);
    mOptionGroups[RSG_EXCLUDES_LAKE_HYLIA] =
        OptionGroup::SubGroup("Lake Hylia", mExcludeLocationsOptionsAreas[RCAREA_LAKE_HYLIA]);
    mOptionGroups[RSG_EXCLUDES_WATER_TEMPLE] =
        OptionGroup::SubGroup("Water Temple", mExcludeLocationsOptionsAreas[RCAREA_WATER_TEMPLE]);
    mOptionGroups[RSG_EXCLUDES_GERUDO_VALLEY] =
        OptionGroup::SubGroup("Gerudo Valley", mExcludeLocationsOptionsAreas[RCAREA_GERUDO_VALLEY]);
    mOptionGroups[RSG_EXCLUDES_GERUDO_FORTRESS] =
        OptionGroup::SubGroup("Gerudo Fortress", mExcludeLocationsOptionsAreas[RCAREA_GERUDO_FORTRESS]);
    mOptionGroups[RSG_EXCLUDES_HAUNTED_WASTELAND] =
        OptionGroup::SubGroup("Haunted Wasteland", mExcludeLocationsOptionsAreas[RCAREA_WASTELAND]);
    mOptionGroups[RSG_EXCLUDES_DESERT_COLOSSUS] =
        OptionGroup::SubGroup("Desert Colossus", mExcludeLocationsOptionsAreas[RCAREA_DESERT_COLOSSUS]);
    mOptionGroups[RSG_EXCLUDES_GERUDO_TRAINING_GROUND] =
        OptionGroup::SubGroup("Gerudo Training Ground", mExcludeLocationsOptionsAreas[RCAREA_GERUDO_TRAINING_GROUND]);
    mOptionGroups[RSG_EXCLUDES_SPIRIT_TEMPLE] =
        OptionGroup::SubGroup("Spirit Temple", mExcludeLocationsOptionsAreas[RCAREA_SPIRIT_TEMPLE]);
    mOptionGroups[RSG_EXCLUDES_HYRULE_CASTLE] =
        OptionGroup::SubGroup("Hyrule Castle", mExcludeLocationsOptionsAreas[RCAREA_HYRULE_CASTLE]);
    mOptionGroups[RSG_EXCLUDES_MARKET] = OptionGroup::SubGroup("Market", mExcludeLocationsOptionsAreas[RCAREA_MARKET]);
    mOptionGroups[RSG_EXCLUDES_GANONS_CASTLE] =
        OptionGroup::SubGroup("Ganon's Castle", mExcludeLocationsOptionsAreas[RCAREA_GANONS_CASTLE]);
    mOptionGroups[RSG_EXCLUDES] =
        OptionGroup::SubGroup("Exclude Locations", {
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
                                                       &mOptionGroups[RSG_EXCLUDES_GERUDO_TRAINING_GROUND],
                                                       &mOptionGroups[RSG_EXCLUDES_SPIRIT_TEMPLE],
                                                       &mOptionGroups[RSG_EXCLUDES_HYRULE_CASTLE],
                                                       &mOptionGroups[RSG_EXCLUDES_MARKET],
                                                       &mOptionGroups[RSG_EXCLUDES_GANONS_CASTLE],
                                                   });
    mOptionGroups[RSG_DETAILED_LOGIC] =
        OptionGroup("Detailed Logic Settings",
                    { &mOptionGroups[RSG_LOGIC], &mOptionGroups[RSG_TRICKS], &mOptionGroups[RSG_EXCLUDES] });
}

std::unordered_map<std::string, RandomizerSettingKey> Settings::PopulateOptionNameToEnum() {
    std::unordered_map<std::string, RandomizerSettingKey> output = {};
    for (size_t count = 0; count < RSK_MAX; count++) {
        // skip unassigned settings (RSK_NONE)
        if (mOptions[count].GetOptionCount() > 0) {
            output[mOptions[count].GetName()] = static_cast<RandomizerSettingKey>(count);
        }
    }
    return output;
}

Option& Settings::GetOption(const RandomizerSettingKey key) {
    return mOptions[key];
}

TrickSetting& Settings::GetTrickSetting(const RandomizerTrick key) {
    return mTrickSettings[key];
}

int Settings::GetRandomizerTrickByName(const std::string& name) {
    const auto& it = mTrickNameToEnum.find(name);
    if (it == mTrickNameToEnum.end()) {
        return -1;
    }
    return it->second;
}

void Context::ResetTrickOptions() {
    for (int count = 0; count < RT_MAX; count++) {
        mTrickOptions[count].Set(0); // RANDOTODO this can probably be done better
    };
}

const std::array<Option, RSK_MAX>& Settings::GetAllOptions() const {
    return mOptions;
}

std::vector<Option*>& Settings::GetExcludeOptionsForArea(const RandomizerCheckArea area) {
    return mExcludeLocationsOptionsAreas[area];
}

const std::array<std::vector<Option*>, RCAREA_INVALID>& Settings::GetExcludeLocationsOptions() const {
    return mExcludeLocationsOptionsAreas;
}

const std::array<OptionGroup, RSG_MAX>& Settings::GetOptionGroups() {
    return mOptionGroups;
}

const OptionGroup& Settings::GetOptionGroup(const RandomizerSettingGroupKey key) {
    return mOptionGroups[key];
}

void Settings::UpdateAllOptions() {
    for (auto& option : mOptions) {
        option.RunCallback();
    }
}

void Context::FinalizeSettings(const std::set<RandomizerCheck>& excludedLocations,
                               const std::set<RandomizerTrick>& enabledTricks) {
    // With certain access settings, the seed is only beatable if Starting Age is set to Child.
    if (mOptions[RSK_LOGIC_RULES].IsNot(RO_LOGIC_NO_LOGIC) &&
        ((mOptions[RSK_DOOR_OF_TIME].Is(RO_DOOROFTIME_CLOSED) && !mOptions[RSK_SHUFFLE_OCARINA]) ||
         (mOptions[RSK_FOREST].Is(RO_CLOSED_FOREST_ON) && mOptions[RSK_SHUFFLE_OVERWORLD_SPAWNS].Is(RO_GENERIC_OFF) &&
          mOptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES].Is(RO_GENERIC_OFF) &&
          mOptions[RSK_SHUFFLE_INTERIOR_ENTRANCES].Is(RO_GENERIC_OFF) &&
          (mOptions[RSK_SHUFFLE_GROTTO_ENTRANCES].Is(RO_GENERIC_OFF) &&
           mOptions[RSK_DECOUPLED_ENTRANCES].Is(RO_GENERIC_OFF))))) {
        mOptions[RSK_STARTING_AGE].Set(RO_AGE_CHILD);
    }

    // If we only have MQ, set all dungeons to MQ
    if (OTRGlobals::Instance->HasMasterQuest() && !OTRGlobals::Instance->HasOriginal()) {
        mOptions[RSK_MQ_DUNGEON_RANDOM].Set(RO_MQ_DUNGEONS_SET_NUMBER);
        mOptions[RSK_MQ_DUNGEON_COUNT].Set(MAX_MQ_DUNGEON_COUNT);
        mOptions[RSK_MQ_DUNGEON_SET].Set(RO_GENERIC_OFF);
    }

    // If we don't have MQ, set all dungeons to Vanilla
    if (OTRGlobals::Instance->HasOriginal() && !OTRGlobals::Instance->HasMasterQuest()) {
        mOptions[RSK_MQ_DUNGEON_RANDOM].Set(RO_MQ_DUNGEONS_NONE);
    }

    if (mOptions[RSK_MQ_DUNGEON_RANDOM].Is(RO_MQ_DUNGEONS_NONE)) {
        mOptions[RSK_MQ_DUNGEON_COUNT].Set(0);
        mOptions[RSK_MQ_DUNGEON_SET].Set(RO_GENERIC_OFF);
    }

    // If any of the individual shuffle settings are on, turn on the main Shuffle Entrances option
    if (mOptions[RSK_SHUFFLE_DUNGEON_ENTRANCES].IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF) ||
        mOptions[RSK_SHUFFLE_BOSS_ENTRANCES].IsNot(RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF) ||
        mOptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES] ||
        mOptions[RSK_SHUFFLE_INTERIOR_ENTRANCES].IsNot(RO_INTERIOR_ENTRANCE_SHUFFLE_OFF) ||
        mOptions[RSK_SHUFFLE_THIEVES_HIDEOUT_ENTRANCES] || mOptions[RSK_SHUFFLE_GROTTO_ENTRANCES] ||
        mOptions[RSK_SHUFFLE_OWL_DROPS] || mOptions[RSK_SHUFFLE_WARP_SONGS] || mOptions[RSK_SHUFFLE_OVERWORLD_SPAWNS]) {
        mOptions[RSK_SHUFFLE_ENTRANCES].Set(RO_GENERIC_ON);
    } else {
        mOptions[RSK_SHUFFLE_ENTRANCES].Set(RO_GENERIC_OFF);
    }

    if (mOptions[RSK_SHUFFLE_BOSS_ENTRANCES].Is(RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF)) {
        mOptions[RSK_SHUFFLE_GANONS_TOWER_ENTRANCE].Set(RO_GENERIC_OFF);
    }

    if (mOptions[RSK_SHUFFLE_DUNGEON_REWARDS].Is(RO_DUNGEON_REWARDS_END_OF_DUNGEON)) {
        mOptions[RSK_LINKS_POCKET].Set(RO_LINKS_POCKET_DUNGEON_REWARD);
    } else if (mOptions[RSK_SHUFFLE_DUNGEON_REWARDS].Is(RO_DUNGEON_REWARDS_OWN_DUNGEON) ||
               mOptions[RSK_SHUFFLE_DUNGEON_REWARDS].Is(RO_DUNGEON_REWARDS_VANILLA)) {
        mOptions[RSK_LINKS_POCKET_REWARD].Set(RO_LINKS_POCKET_LIGHT_MEDALLION);
    }

    if (mOptions[RSK_LINKS_POCKET].IsNot(RO_LINKS_POCKET_DUNGEON_REWARD)) {
        mOptions[RSK_LINKS_POCKET_REWARD].Set(RO_LINKS_POCKET_ANY_REWARD);
    }

    for (const auto locationKey : this->everyPossibleLocation) {
        if (const auto location = this->GetItemLocation(locationKey);
            excludedLocations.contains(location->GetRandomizerCheck())) {
            location->SetExcludedOption(1);
        } else {
            location->SetExcludedOption(0);
        }
    }
    // Tricks
    ResetTrickOptions();
    for (const auto randomizerTrick : enabledTricks) {
        mTrickOptions[randomizerTrick].Set(1);
    }
    if (!mOptions[RSK_SHUFFLE_KOKIRI_SWORD]) {
        if (mOptions[RSK_STARTING_KOKIRI_SWORD]) {
            this->GetItemLocation(RC_KF_KOKIRI_SWORD_CHEST)->SetExcludedOption(1);
        }
    }
    if (!mOptions[RSK_SHUFFLE_MASTER_SWORD]) {
        if (mOptions[RSK_STARTING_MASTER_SWORD]) {
            this->GetItemLocation(RC_TOT_MASTER_SWORD)->SetExcludedOption(1);
        }
    }
    if (!mOptions[RSK_SHUFFLE_OCARINA]) {
        if (mOptions[RSK_STARTING_OCARINA].IsNot(RO_STARTING_OCARINA_OFF)) {
            this->GetItemLocation(RC_LW_GIFT_FROM_SARIA)->SetExcludedOption(1);
            if (mOptions[RSK_STARTING_OCARINA].Is(RO_STARTING_OCARINA_TIME)) {
                this->GetItemLocation(RC_HF_OCARINA_OF_TIME_ITEM)->SetExcludedOption(1);
            }
        }
    }

    if (mOptions[RSK_SHUFFLE_DEKU_STICK_BAG]) {
        mOptions[RSK_STARTING_STICKS].Set(false);
    }
    if (mOptions[RSK_SHUFFLE_DEKU_NUT_BAG]) {
        mOptions[RSK_STARTING_NUTS].Set(false);
    }
    if (mOptions[RSK_SHUFFLE_SWIM]) {
        mOptions[RSK_STARTING_SCALE].Set(0);
    }
    if (mOptions[RSK_SHUFFLE_GRAB]) {
        mOptions[RSK_STARTING_STRENGTH].Set(0);
    }
    if (mOptions[RSK_SHUFFLE_CHILD_WALLET]) {
        mOptions[RSK_STARTING_WALLET].Set(0);
    }

    if (mOptions[RSK_ZORAS_FOUNTAIN].IsNot(RO_ZF_OPEN) &&
        mOptions[RSK_STARTING_BOTTLE_1].IsNot(RO_STARTING_BOTTLE_RUTOS_LETTER)) {
        mOptions[RSK_STARTING_BOTTLE_4].Set(RO_STARTING_BOTTLE_OFF);
    }

    // TODO: RandomizeAllSettings(true) when implementing the ability to randomize the options themselves.
    std::array<DungeonInfo*, 12> dungeons = this->GetDungeons()->GetDungeonList();

    // reset the MQ vars
    for (auto dungeon : dungeons) {
        dungeon->ClearMQ();
        dungeon->SetDungeonKnown(true);
    }
    // if it's selection mode, process the selection directly
    if (mOptions[RSK_MQ_DUNGEON_RANDOM].Is(RO_MQ_DUNGEONS_SELECTION)) {
        mOptions[RSK_MQ_DUNGEON_SET].Set(RO_GENERIC_ON);
        // How many dungeons are set to MQ in selection
        uint8_t mqSet = 0;
        for (auto dungeon : dungeons) {
            switch (mOptions[dungeon->GetMQSetting()].Get()) {
                case RO_MQ_SET_MQ:
                    dungeon->SetMQ();
                    mqSet += 1;
                    break;
                case RO_MQ_SET_RANDOM:
                    // 50% per dungeon, rolled separatly so people can either have a linear distribtuion
                    // or a bell curve for the number of MQ dungeons per seed.
                    if (Random(0, 2)) {
                        dungeon->SetMQ();
                        mqSet += 1;
                    }
                    dungeon->SetDungeonKnown(false);
                    break;
                default:
                    break;
            }
        }
        // override the dungeons set with the ones set by selection, so it's accurate for anything that wants to know MQ
        // dungeon count
        mOptions[RSK_MQ_DUNGEON_COUNT].Set(mqSet);
        // handling set number and random number together
    } else if (mOptions[RSK_MQ_DUNGEON_RANDOM].IsNot(RO_MQ_DUNGEONS_NONE)) {
        // so we don't have to call this repeatedly
        uint8_t mqCount = mOptions[RSK_MQ_DUNGEON_COUNT].Get();
        // How many dungeons are set to MQ in selection
        uint8_t mqSet = 0;
        // the number of random
        uint8_t mqToSet = 0;
        // store the dungeons to randomly decide between. we use the id instead of a dungeon object to avoid a lot of
        // casting.
        std::vector<uint8_t> randMQOption = {};
        // if dungeons have been preset, process them
        if (mOptions[RSK_MQ_DUNGEON_SET]) {
            for (size_t i = 0; i < dungeons.size(); i++) {
                switch (mOptions[dungeons[i]->GetMQSetting()].Get()) {
                    case RO_MQ_SET_MQ:
                        dungeons[i]->SetMQ();
                        mqSet += 1;
                        break;
                    case RO_MQ_SET_RANDOM:
                        randMQOption.push_back(static_cast<uint8_t>(i));
                        dungeons[i]->SetDungeonKnown(false);
                        break;
                    default:
                        break;
                }
            }
            // otherwise, every dungeon is possible
        } else {
            // if count is MAX_MQ_DUNGEON_COUNT, we know everything is MQ, so can skip some setps and not set Known
            if (mOptions[RSK_MQ_DUNGEON_RANDOM].Is(RO_MQ_DUNGEONS_SET_NUMBER) && mqCount == MAX_MQ_DUNGEON_COUNT) {
                randMQOption.resize(MAX_MQ_DUNGEON_COUNT);
                for (int i = 0; i < MAX_MQ_DUNGEON_COUNT; i++) {
                    randMQOption[i] = i;
                }
                for (auto dungeon : dungeons) {
                    mOptions[dungeon->GetMQSetting()].Set(RO_MQ_SET_MQ);
                }
                // if it's fixed to zero, set it to None instead. the rest is processed after
            } else if (mOptions[RSK_MQ_DUNGEON_RANDOM].Is(RO_MQ_DUNGEONS_SET_NUMBER) && mqCount == 0) {
                mOptions[RSK_MQ_DUNGEON_RANDOM].Set(RO_MQ_DUNGEONS_NONE);
                // otherwise, make everything a possibility and unknown
            } else {
                for (size_t i = 0; i < dungeons.size(); i++) {
                    randMQOption.push_back(static_cast<uint8_t>(i));
                    dungeons[i]->SetDungeonKnown(false);
                    mOptions[dungeons[i]->GetMQSetting()].Set(RO_MQ_SET_RANDOM);
                }
            }
        }
        // if there's no random options, we can skip this
        if (randMQOption.size() > 0) {
            // Figure out how many dungeons to select, rolling the random number if needed
            if (mOptions[RSK_MQ_DUNGEON_RANDOM].Is(RO_MQ_DUNGEONS_RANDOM_NUMBER)) {
                mqToSet = Random(0, static_cast<int>(randMQOption.size()) + 1);
            } else if (mqCount > mqSet) {
                mqToSet = std::min(mqCount - mqSet, static_cast<int>(randMQOption.size()));
            }
            // we only need to shuffle if we're not using them all
            if (mqToSet <= static_cast<int8_t>(randMQOption.size()) && mqToSet > 0) {
                Shuffle(randMQOption);
            }
            for (uint8_t i = 0; i < mqToSet; i++) {
                dungeons[randMQOption[i]]->SetMQ();
            }
        } else {
            // if there's no random options, check if we can collapse the setting into None or Selection
            if (mqSet == 0) {
                mOptions[RSK_MQ_DUNGEON_RANDOM].Set(RO_MQ_DUNGEONS_NONE);
            } else {
                mOptions[RSK_MQ_DUNGEON_RANDOM].Set(RO_MQ_DUNGEONS_SELECTION);
            }
        }
        // reset the value set based on what was actually set
        mOptions[RSK_MQ_DUNGEON_COUNT].Set(mqToSet + mqSet);
    }
    // Not an if else as other settings can become None in processing
    if (mOptions[RSK_MQ_DUNGEON_RANDOM].Is(RO_MQ_DUNGEONS_NONE)) {
        mOptions[RSK_MQ_DUNGEON_SET].Set(RO_GENERIC_OFF);
        mOptions[RSK_MQ_DUNGEON_COUNT].Set(0);
        for (auto dungeon : dungeons) {
            mOptions[dungeon->GetMQSetting()].Set(RO_MQ_SET_VANILLA);
        }
    }

    // Set keyring for each dungeon
    for (const auto dungeon : dungeons) {
        dungeon->ClearKeyRing();
    }

    const std::vector<OptionValue*> keyRingOptions = {
        &mOptions[RSK_KEYRINGS_FOREST_TEMPLE], &mOptions[RSK_KEYRINGS_FIRE_TEMPLE],
        &mOptions[RSK_KEYRINGS_WATER_TEMPLE],  &mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE],
        &mOptions[RSK_KEYRINGS_SHADOW_TEMPLE], &mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL],
        &mOptions[RSK_KEYRINGS_GTG],           &mOptions[RSK_KEYRINGS_GANONS_CASTLE],
    };

    if (mOptions[RSK_KEYRINGS]) {
        // Random Keyrings
        auto keyrings = keyRingOptions;
        if (mOptions[RSK_GERUDO_FORTRESS].Is(RO_GF_CARPENTERS_NORMAL) &&
            mOptions[RSK_GERUDO_KEYS].IsNot(RO_GERUDO_KEYS_VANILLA)) {
            keyrings.push_back(&mOptions[RSK_KEYRINGS_GERUDO_FORTRESS]);
        } else {
            mOptions[RSK_KEYRINGS_GERUDO_FORTRESS].Set(RO_KEYRING_FOR_DUNGEON_OFF);
        }
        if (mOptions[RSK_SHUFFLE_CHEST_MINIGAME]) {
            keyrings.push_back(&mOptions[RSK_KEYRINGS_CHEST_GAME]);
        } else {
            mOptions[RSK_KEYRINGS_CHEST_GAME].Set(RO_KEYRING_FOR_DUNGEON_OFF);
        }
        if (mOptions[RSK_KEYRINGS].Is(RO_KEYRINGS_RANDOM) || mOptions[RSK_KEYRINGS].Is(RO_KEYRINGS_COUNT)) {
            const uint32_t keyRingCount = mOptions[RSK_KEYRINGS].Is(RO_KEYRINGS_COUNT)
                                              ? mOptions[RSK_KEYRINGS_RANDOM_COUNT].Get()
                                              : Random(0, static_cast<int>(keyrings.size()));
            Shuffle(keyrings);
            for (size_t i = 0; i < keyRingCount; i++) {
                keyrings[i]->Set(RO_KEYRING_FOR_DUNGEON_ON);
            }
            for (size_t i = keyRingCount; i < keyrings.size(); i++) {
                keyrings[i]->Set(RO_KEYRING_FOR_DUNGEON_OFF);
            }
        }
        if (mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL].Is(RO_KEYRING_FOR_DUNGEON_ON) ||
            (mOptions[RSK_KEYRINGS_BOTTOM_OF_THE_WELL].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 0)) {
            this->GetDungeon(BOTTOM_OF_THE_WELL)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_FOREST_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_ON) ||
            (mOptions[RSK_KEYRINGS_FOREST_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 0)) {
            this->GetDungeon(FOREST_TEMPLE)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_FIRE_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_ON) ||
            (mOptions[RSK_KEYRINGS_FIRE_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 0)) {
            this->GetDungeon(FIRE_TEMPLE)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_WATER_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_ON) ||
            (mOptions[RSK_KEYRINGS_WATER_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 0)) {
            this->GetDungeon(WATER_TEMPLE)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_ON) ||
            (mOptions[RSK_KEYRINGS_SPIRIT_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 0)) {
            this->GetDungeon(SPIRIT_TEMPLE)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_SHADOW_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_ON) ||
            (mOptions[RSK_KEYRINGS_SHADOW_TEMPLE].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 0)) {
            this->GetDungeon(SHADOW_TEMPLE)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_GTG].Is(RO_KEYRING_FOR_DUNGEON_ON) ||
            (mOptions[RSK_KEYRINGS_GTG].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 0)) {
            this->GetDungeon(GERUDO_TRAINING_GROUND)->SetKeyRing();
        }
        if (mOptions[RSK_KEYRINGS_GANONS_CASTLE].Is(RO_KEYRING_FOR_DUNGEON_ON) ||
            (mOptions[RSK_KEYRINGS_GANONS_CASTLE].Is(RO_KEYRING_FOR_DUNGEON_RANDOM) && Random(0, 2) == 0)) {
            this->GetDungeon(GANONS_CASTLE)->SetKeyRing();
        }
        // Gerudo Fortress & the chest minigame have no DungeonInfo, so resolve their random rolls into the option
        for (const auto keyring : { &mOptions[RSK_KEYRINGS_GERUDO_FORTRESS], &mOptions[RSK_KEYRINGS_CHEST_GAME] }) {
            if (keyring->Is(RO_KEYRING_FOR_DUNGEON_RANDOM)) {
                keyring->Set(Random(0, 2) == 0 ? RO_KEYRING_FOR_DUNGEON_ON : RO_KEYRING_FOR_DUNGEON_OFF);
            }
        }
    }

    auto trials = this->GetTrials()->GetTrialList();
    Shuffle(trials);
    for (const auto trial : trials) {
        trial->SetAsSkipped();
    }
    if (mOptions[RSK_GANONS_TRIALS].Is(RO_GANONS_TRIALS_SKIP)) {
        mOptions[RSK_TRIAL_COUNT].Set(0);
    } else if (mOptions[RSK_GANONS_TRIALS].Is(RO_GANONS_TRIALS_RANDOM_NUMBER)) {
        mOptions[RSK_TRIAL_COUNT].Set(
            Random(0, static_cast<int>(Rando::Settings::GetInstance()->GetOption(RSK_TRIAL_COUNT).GetOptionCount())));
    }
    for (uint8_t i = 0; i < mOptions[RSK_TRIAL_COUNT].Get(); i++) {
        trials[i]->SetAsRequired();
    }

    bool dungeonShuffle = !mOptions[RSK_SHUFFLE_DUNGEON_ENTRANCES].Is(RO_GENERIC_OFF);
    bool bossShuffle = !mOptions[RSK_SHUFFLE_BOSS_ENTRANCES].Is(RO_GENERIC_OFF);
    bool overworldShuffle = !mOptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES].Is(RO_GENERIC_OFF);
    bool interiorShuffle = !mOptions[RSK_SHUFFLE_INTERIOR_ENTRANCES].Is(RO_INTERIOR_ENTRANCE_SHUFFLE_OFF);
    bool gerudoFortressShuffle = !mOptions[RSK_SHUFFLE_THIEVES_HIDEOUT_ENTRANCES].Is(RO_GENERIC_OFF);
    bool grottoShuffle = !mOptions[RSK_SHUFFLE_GROTTO_ENTRANCES].Is(RO_GENERIC_OFF);

    if (dungeonShuffle + bossShuffle + overworldShuffle + interiorShuffle + grottoShuffle <= 1) {
        mOptions[RSK_MIXED_ENTRANCE_POOLS].Set(RO_GENERIC_OFF);
    }

    if (!mOptions[RSK_MIXED_ENTRANCE_POOLS] || !dungeonShuffle) {
        mOptions[RSK_MIX_DUNGEON_ENTRANCES].Set(RO_GENERIC_OFF);
    }

    if (!mOptions[RSK_MIXED_ENTRANCE_POOLS] || !bossShuffle) {
        mOptions[RSK_MIX_BOSS_ENTRANCES].Set(RO_GENERIC_OFF);
    }

    if (!mOptions[RSK_MIXED_ENTRANCE_POOLS] || !overworldShuffle) {
        mOptions[RSK_MIX_OVERWORLD_ENTRANCES].Set(RO_GENERIC_OFF);
    }

    if (!mOptions[RSK_MIXED_ENTRANCE_POOLS] || !interiorShuffle) {
        mOptions[RSK_MIX_INTERIOR_ENTRANCES].Set(RO_GENERIC_OFF);
    }

    if (!mOptions[RSK_MIXED_ENTRANCE_POOLS] || !gerudoFortressShuffle) {
        mOptions[RSK_MIX_THIEVES_HIDEOUT_ENTRANCES].Set(RO_GENERIC_OFF);
    }

    if (!mOptions[RSK_MIXED_ENTRANCE_POOLS] || !grottoShuffle) {
        mOptions[RSK_MIX_GROTTO_ENTRANCES].Set(RO_GENERIC_OFF);
    }

    if (mOptions[RSK_STARTING_AGE].Is(RO_AGE_RANDOM)) {
        if (const uint32_t choice = Random(0, 2); choice == 0) {
            mOptions[RSK_SELECTED_STARTING_AGE].Set(RO_AGE_CHILD);
        } else {
            mOptions[RSK_SELECTED_STARTING_AGE].Set(RO_AGE_ADULT);
        }
    } else {
        mOptions[RSK_SELECTED_STARTING_AGE].Set(mOptions[RSK_STARTING_AGE].Get());
    }

    // TODO: Random Starting Time

    if (mOptions[RSK_GANONS_BOSS_KEY].Is(RO_GANON_BOSS_KEY_STONES)) {
        mGBKCondition = RO_CHECK_TRIGGER_STONES;
    } else if (mOptions[RSK_GANONS_BOSS_KEY].Is(RO_GANON_BOSS_KEY_MEDALLIONS)) {
        mGBKCondition = RO_CHECK_TRIGGER_MEDALLIONS;
    } else if (mOptions[RSK_GANONS_BOSS_KEY].Is(RO_GANON_BOSS_KEY_REWARDS)) {
        mGBKCondition = RO_CHECK_TRIGGER_REWARDS;
    } else if (mOptions[RSK_GANONS_BOSS_KEY].Is(RO_GANON_BOSS_KEY_DUNGEONS)) {
        mGBKCondition = RO_CHECK_TRIGGER_DUNGEONS;
    } else if (mOptions[RSK_GANONS_BOSS_KEY].Is(RO_GANON_BOSS_KEY_TOKENS)) {
        mGBKCondition = RO_CHECK_TRIGGER_TOKENS;
    } else if (mOptions[RSK_GANONS_BOSS_KEY].Is(RO_GANON_BOSS_KEY_TRIFORCE_PIECES)) {
        mGBKCondition = RO_CHECK_TRIGGER_TRIFORCE_PIECES;
    } else {
        mGBKCondition = RO_CHECK_TRIGGER_NONE;
    }

    if (mOptions[RSK_GANONS_SOUL].Is(RO_GANONS_SOUL_STONES)) {
        mGanonsSoulCondition = RO_CHECK_TRIGGER_STONES;
    } else if (mOptions[RSK_GANONS_SOUL].Is(RO_GANONS_SOUL_MEDALLIONS)) {
        mGanonsSoulCondition = RO_CHECK_TRIGGER_MEDALLIONS;
    } else if (mOptions[RSK_GANONS_SOUL].Is(RO_GANONS_SOUL_REWARDS)) {
        mGanonsSoulCondition = RO_CHECK_TRIGGER_REWARDS;
    } else if (mOptions[RSK_GANONS_SOUL].Is(RO_GANONS_SOUL_DUNGEONS)) {
        mGanonsSoulCondition = RO_CHECK_TRIGGER_DUNGEONS;
    } else if (mOptions[RSK_GANONS_SOUL].Is(RO_GANONS_SOUL_TOKENS)) {
        mGanonsSoulCondition = RO_CHECK_TRIGGER_TOKENS;
    } else if (mOptions[RSK_GANONS_SOUL].Is(RO_GANONS_SOUL_TRIFORCE_PIECES)) {
        mGanonsSoulCondition = RO_CHECK_TRIGGER_TRIFORCE_PIECES;
    } else {
        mGanonsSoulCondition = RO_CHECK_TRIGGER_NONE;
    }

    if (mOptions[RSK_WINCON].Is(RO_WINCON_STONES)) {
        mWinCondition = RO_WINCON_STONES;
    } else if (mOptions[RSK_WINCON].Is(RO_WINCON_MEDALLIONS)) {
        mWinCondition = RO_WINCON_MEDALLIONS;
    } else if (mOptions[RSK_WINCON].Is(RO_WINCON_REWARDS)) {
        mWinCondition = RO_WINCON_REWARDS;
    } else if (mOptions[RSK_WINCON].Is(RO_WINCON_DUNGEONS)) {
        mWinCondition = RO_WINCON_DUNGEONS;
    } else if (mOptions[RSK_WINCON].Is(RO_WINCON_TOKENS)) {
        mWinCondition = RO_WINCON_TOKENS;
    } else if (mOptions[RSK_WINCON].Is(RO_WINCON_TRIFORCE_PIECES)) {
        mWinCondition = RO_WINCON_TRIFORCE_PIECES;
    } else if (mOptions[RSK_WINCON].Is(RO_WINCON_ANYWHERE)) {
        mWinCondition = RO_WINCON_ANYWHERE;
    } else {
        mWinCondition = RO_WINCON_DEFEAT_GANON;
    }

    if (!mOptions[RSK_SHUFFLE_WARP_SONGS]) {
        mOptions[RSK_WARP_SONG_HINTS].Set(RO_GENERIC_OFF);
    }

    if (!mOptions[RSK_SHUFFLE_COWS]) {
        mOptions[RSK_MALON_HINT].Set(RO_GENERIC_OFF);
    }

    if (!mOptions[RSK_SHUFFLE_100_GS_REWARD]) {
        mOptions[RSK_KAK_100_SKULLS_HINT].Set(RO_GENERIC_OFF);
    }

    if (!mOptions[RSK_SHUFFLE_FISHING_POLE]) {
        mOptions[RSK_FISHING_POLE_HINT].Set(RO_GENERIC_OFF);
    }

    if (mOptions[RSK_FISHSANITY].IsNot(RO_FISHSANITY_HYRULE_LOACH)) {
        mOptions[RSK_LOACH_HINT].Set(RO_GENERIC_OFF);
    }
}

void Settings::ParseJson(const nlohmann::json& spoilerFileJson) {
    mContext->SetSeedString(spoilerFileJson.at("seed").get<std::string>());
    mContext->SetSeed(spoilerFileJson.at("finalSeed").get<uint32_t>());
    nlohmann::json settingsJson = spoilerFileJson.value("settings", nlohmann::json());
    for (auto it = settingsJson.begin(); it != settingsJson.end(); ++it) {
        // todo load into cvars for UI
        // RANDOTODO handle numeric value to options conversion better than brute force
        if (StaticData::optionNameToEnum.contains(it.key())) {
            const RandomizerSettingKey index = StaticData::optionNameToEnum[it.key()];
            mContext->GetOption(index).Set(mOptions[index].GetValueFromText(it.value()));
        }
    }

    nlohmann::json jsonExcludedLocations = spoilerFileJson.value("excludedLocations", nlohmann::json());
    const auto ctx = Context::GetInstance();

    for (auto it = jsonExcludedLocations.begin(); it != jsonExcludedLocations.end(); ++it) {
        const RandomizerCheck rc = Rando::StaticData::locationNameToEnum[it.value()];
        ctx->GetItemLocation(rc)->SetExcludedOption(RO_GENERIC_ON);
    }

    nlohmann::json enabledTricksJson = spoilerFileJson.value("enabledTricks", nlohmann::json());
    for (auto it = enabledTricksJson.begin(); it != enabledTricksJson.end(); ++it) {
        const RandomizerTrick rt = mTrickNameToEnum[it.value()];
        GetTrickSetting(rt).SetContextIndex(RO_GENERIC_ON);
    }
}

void Settings::AssignContext(std::shared_ptr<Context> ctx) {
    mContext = ctx;
}

void Settings::ClearContext() {
    mContext = nullptr;
}

void Settings::SetAllToContext() {
    for (int i = 0; i < RSK_MAX; i++) {
        mContext->GetOption(static_cast<RandomizerSettingKey>(i)).Set(mOptions[i].GetOptionIndex());
    }
    for (int i = 0; i < RT_MAX; i++) {
        mContext->GetTrickOption(static_cast<RandomizerTrick>(i)).Set(mTrickSettings[i].GetOptionIndex());
    }
    for (int i = 0; i < RC_MAX; i++) {
        mContext->GetItemLocation(i)->SetExcludedOption(
            StaticData::GetLocation(static_cast<RandomizerCheck>(i))->GetExcludedOption()->GetOptionIndex());
    }
}

void Settings::RandomizeAllSettings() {
    // Randomize all settings except tricks
    for (int i = 0; i < RSK_MAX; i++) {
        switch (static_cast<RandomizerSettingKey>(i)) {
            case RSK_STARTING_SKULLTULA_TOKEN:
            case RSK_STARTING_HEARTS:
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
            case RSK_STARTING_MEGATON_HAMMER:
            case RSK_STARTING_BOOMERANG:
            case RSK_STARTING_LENS_OF_TRUTH:
            case RSK_STARTING_DINS_FIRE:
            case RSK_STARTING_FARORES_WIND:
            case RSK_STARTING_NAYRUS_LOVE:
            case RSK_STARTING_FIRE_ARROWS:
            case RSK_STARTING_ICE_ARROWS:
            case RSK_STARTING_LIGHT_ARROWS:
            case RSK_STARTING_IRON_BOOTS:
            case RSK_STARTING_HOVER_BOOTS:
            case RSK_STARTING_HYLIAN_SHIELD:
            case RSK_STARTING_MIRROR_SHIELD:
            case RSK_STARTING_GORON_TUNIC:
            case RSK_STARTING_ZORA_TUNIC:
            case RSK_STARTING_STONE_OF_AGONY:
            case RSK_STARTING_HOOKSHOT:
            case RSK_STARTING_BOW:
            case RSK_STARTING_SLINGSHOT:
            case RSK_STARTING_BOMB_BAG:
            case RSK_STARTING_STRENGTH:
            case RSK_STARTING_SCALE:
            case RSK_STARTING_WALLET:
            case RSK_STARTING_MAGIC_METER:
            case RSK_STARTING_BOMBCHU_BAG:
            case RSK_STARTING_BOTTLE_1:
            case RSK_STARTING_BOTTLE_2:
            case RSK_STARTING_BOTTLE_3:
            case RSK_STARTING_BOTTLE_4:
            case RSK_STARTING_WEIRD_EGG:
            case RSK_STARTING_ZELDAS_LETTER:
            case RSK_STARTING_CLAIM_CHECK:
            case RSK_STARTING_GERUDO_CARD:
            case RSK_STARTING_BIGGORON_SWORD:
            case RSK_STARTING_BUNNY_HOOD:
            case RSK_STARTING_KEATON_MASK:
            case RSK_STARTING_SKULL_MASK:
            case RSK_STARTING_SPOOKY_MASK:
            case RSK_STARTING_GORON_MASK:
            case RSK_STARTING_ZORA_MASK:
            case RSK_STARTING_GERUDO_MASK:
            case RSK_STARTING_MASK_OF_TRUTH:
                continue;
            default:
                break;
        }

        auto key = static_cast<RandomizerSettingKey>(i);
        Option& option = mOptions[key];

        if (option.GetOptionCount() == 0) {
            continue;
        }

        uint8_t randomIndex = Random(0, static_cast<uint32_t>(option.GetOptionCount()));

        option.SetContextIndex(randomIndex);
        if (!option.GetCVarName().empty()) {
            CVarSetInteger(option.GetCVarName().c_str(), randomIndex);
        }
        option.RunCallback();
    }

    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

std::shared_ptr<Settings> Settings::GetInstance() {
    if (mInstance == nullptr) {
        mInstance = std::make_shared<Settings>();
    }
    return mInstance;
}
} // namespace Rando
