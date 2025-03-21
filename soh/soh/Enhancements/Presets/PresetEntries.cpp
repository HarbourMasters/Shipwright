#include "Presets.h"

// TODO: Ideally everything in this file will come from one/many JSON files

// Enhancement presets
const std::vector<PresetEntry> vanillaPlusPresetEntries = {
    // Quality of Life

    // Skips & Speed-ups

    // Graphics

    // Items

    // Fixes

    // Difficulty

    // Minigames

    // Extra Modes

    // Cheats

};

const std::vector<PresetEntry> enhancedPresetEntries = {
    // Quality of Life

    // Skips & Speed-ups

    // Graphics

    // Items

    // Fixes

    // Difficulty

    // Minigames

    // Extra Modes

    // Cheats

};

const std::vector<PresetEntry> randomizerPresetEntries = {
    // Quality of Life

    // Skips & Speed-ups

    // Graphics

    // Items

    // Fixes

    // Difficulty

    // Minigames

    // Extra Modes

    // Cheats

};

// Randomizer presets
const std::vector<PresetEntry> randomizerBeginnerPresetEntries = {
    // World tab

    // Items tab

    // Gamplay tab

    // Locations tab

    // Tricks/Glitches tab

    // Starting inventory tab

};

const std::vector<PresetEntry> randomizerStandardPresetEntries = {
    // World tab

    // Items tab

    // Gamplay tab

    // Locations tab

    // Tricks/Glitches tab

    // Starting inventory tab

};

const std::vector<PresetEntry> randomizerAdvancedPresetEntries = {
    // World tab

    // Items tab

    // Gamplay tab

    // Locations tab

    // Tricks/Glitches tab

    // Starting inventory tab

};

const std::vector<PresetEntry> hellModePresetEntries = {
    // World tab
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ClosedForest"), RO_CLOSED_FOREST_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("KakarikoGate"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DoorOfTime"), 2),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("LockOverworldDoors"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("FortressCarpenters"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("RainbowBridge"), RO_BRIDGE_GREG),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingAge"), RO_AGE_RANDOM),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleDungeonsEntrances"), RO_DUNGEON_ENTRANCE_SHUFFLE_ON_PLUS_GANON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleBossEntrances"), RO_BOSS_ROOM_ENTRANCE_SHUFFLE_FULL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleOverworldEntrances"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleInteriorsEntrances"), RO_INTERIOR_ENTRANCE_SHUFFLE_ALL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleGrottosEntrances"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleOwlDrops"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleWarpSongs"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleOverworldSpawns"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("MixedEntrances"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("MixDungeons"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("MixBosses"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("MixOverworld"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("MixInteriors"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("MixGrottos"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DecoupleEntrances"), 1),

    // Items tab
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleSongs"), RO_SONG_SHUFFLE_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleTokens"), RO_TOKENSANITY_ALL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleKokiriSword"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleMasterSword"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleChildWallet"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("IncludeTycoonWallet"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleOcarinas"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleOcarinaButtons"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleSwim"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleWeirdEgg"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleGerudoToken"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleFishingPole"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleDekuNutBag"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleDekuStickBag"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleFreestanding"), RO_SHUFFLE_FREESTANDING_ALL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Shopsanity"), RO_SHOPSANITY_SPECIFIC_COUNT),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShopsanityCount"), 7),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShopsanityPrices"), RO_PRICE_BALANCED),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Fishsanity"), RO_FISHSANITY_BOTH),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("FishsanityPondCount"), 17),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("FishsanityAgeSplit"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleScrubs"), RO_SCRUBS_ALL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ScrubsPrices"), RO_PRICE_BALANCED),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleBeehives"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleCows"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShufflePots"), RO_SHUFFLE_POTS_ALL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleMerchants"), RO_SHUFFLE_MERCHANTS_ALL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleFrogSongRupees"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleAdultTrade"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Shuffle100GSReward"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleBossSouls"), 2),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleFairies"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingMapsCompasses"), RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Keysanity"), RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GerudoKeys"), RO_GERUDO_KEYS_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BossKeysanity"), RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleGanonBossKey"), RO_GANON_BOSS_KEY_LACS_REWARDS),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("LacsRewardCount"), 9),

    // Gamplay tab
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CuccosToReturn"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BigPoeTargetCount"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BombchuBag"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("EnableBombchuDrops"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BlueFireArrows"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SunlightArrows"), 1),

    // Locations tab
    // NONE

    // Tricks/Glitches tab
    // NONE

    // Starting inventory tab
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingHearts"), 1),
};

const std::map<PresetType, PresetTypeDefinition> presetTypes = {
    { PRESET_TYPE_ENHANCEMENTS,
      { { CVAR_PREFIX_ENHANCEMENT, CVAR_PREFIX_CHEAT },
        {
            { ENHANCEMENT_PRESET_DEFAULT,
              {
                  "Default",
                  "Reset all options to their default values.",
                  {},
              } },
            { ENHANCEMENT_PRESET_VANILLA_PLUS,
              {
                  "Vanilla Plus",
                  "Adds quality of life features that enhance your experience, but don't alter gameplay. Recommended "
                  "for a first playthrough of OoT.",
                  vanillaPlusPresetEntries,
              } },
            { ENHANCEMENT_PRESET_ENHANCED,
              { "Enhanced",
                "The \"Vanilla Plus\" preset, but with more quality of life enhancements that might alter gameplay "
                "slightly. Recommended for returning players going through the vanilla game again.",
                enhancedPresetEntries } },
            { ENHANCEMENT_PRESET_RANDOMIZER,
              { "Randomizer",
                "A baseline set of enhancements for playing randomizer. Includes many quality of life options and "
                "options to speed up gameplay.",
                randomizerPresetEntries } },
        } } },
    { PRESET_TYPE_RANDOMIZER,
      { { CVAR_PREFIX_RANDOMIZER_SETTING, CVAR_PREFIX_RANDOMIZER_ENHANCEMENT },
        {
            { RANDOMIZER_PRESET_DEFAULT,
              {
                  "Default",
                  "Reset all options to their default values.",
                  {},
              } },
            { RANDOMIZER_PRESET_BEGINNER,
              {
                  "Beginner",
                  "A simpler set of options and shuffled items meant for players new to the randomizer. ",
                  randomizerBeginnerPresetEntries,
              } },
            { RANDOMIZER_PRESET_STANDARD,
              {
                  "Standard",
                  "A set of options meant as a baseline for both newer and experienced randomizer players.",
                  randomizerStandardPresetEntries,
              } },
            { RANDOMIZER_PRESET_ADVANCED,
              {
                  "Advanced",
                  "Includes many more shuffled items and introduces some entrance shuffle options. Meant for advanced "
                  "randomizer players.",
                  randomizerAdvancedPresetEntries,
              } },
            { RANDOMIZER_PRESET_HELL_MODE,
              { "Hell Mode",
                "Every location randomized, all entrance settings enabled, but still using glitchless logic. Expect "
                "pain.",
                hellModePresetEntries } },
        } } }
};
