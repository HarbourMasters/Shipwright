#pragma once

#include <map>
#include <string>
#include <vector>
#include <variant>
#include <cstdint>
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/OTRGlobals.h"
#include "soh/cvar_prefixes.h"

enum PresetEntryType {
    PRESET_ENTRY_TYPE_S32,
    PRESET_ENTRY_TYPE_FLOAT,
    PRESET_ENTRY_TYPE_STRING,
    PRESET_ENTRY_TYPE_CPP_STRING,
};

enum PresetType {
    PRESET_TYPE_ENHANCEMENTS,
    PRESET_TYPE_RANDOMIZER,
};

enum EnhancementPreset {
    ENHANCEMENT_PRESET_DEFAULT,
    ENHANCEMENT_PRESET_VANILLA_PLUS,
    ENHANCEMENT_PRESET_ENHANCED,
    ENHANCEMENT_PRESET_RANDOMIZER,
};

enum RandomizerPreset {
    RANDOMIZER_PRESET_DEFAULT,
    RANDOMIZER_PRESET_SPOCK_RACE,
    RANDOMIZER_PRESET_SPOCK_RACE_NO_LOGIC,
    RANDOMIZER_PRESET_S6,
    RANDOMIZER_PRESET_HELL_MODE,
    RANDOMIZER_PRESET_BENCHMARK,
};

typedef struct PresetEntry {
    const char* cvar;
    PresetEntryType type;
    std::variant<int32_t, float, const char*, std::string> value;
} PresetEntry;

std::string FormatLocations(std::vector<RandomizerCheck> locs);

#define PRESET_ENTRY_S32(cvar, value) \
    { cvar,    PRESET_ENTRY_TYPE_S32, value }
#define PRESET_ENTRY_FLOAT(cvar, value) \
    { cvar,  PRESET_ENTRY_TYPE_FLOAT, value }
#define PRESET_ENTRY_STRING(cvar, value) \
    { cvar, PRESET_ENTRY_TYPE_STRING, value }
#define PRESET_ENTRY_CPP_STRING(cvar, value) \
    { cvar, PRESET_ENTRY_TYPE_CPP_STRING, value }

void DrawPresetSelector(PresetType presetType);
void clearCvars(std::vector<const char*> cvarsToClear);
void applyPreset(std::vector<PresetEntry> entries);

// TODO: Ideally everything below this point will come from one/many JSON files

const std::vector<PresetEntry> vanillaPlusPresetEntries = {
    // D-pad Support in text and file select
    PRESET_ENTRY_S32(CVAR_SETTING("DpadInText"), 1),
    // Play Ocarina with D-pad
    PRESET_ENTRY_S32(CVAR_SETTING("OcarinaControl.Dpad"), 1),
    // Play Ocarina with Right Stick
    PRESET_ENTRY_S32(CVAR_SETTING("OcarinaControl.RStick"), 1),
    // D-pad as Equip Items
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DpadEquips"), 1),
    // Prevent Dropped Ocarina Inputs
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DpadNoDropOcarinaInput"), 1),

    // Text Speed (1 to 5)
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("TextSpeed"), 5),
    // Slow Text Speed (1 to 5)
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SlowTextSpeed"), 5),
    // Skip Text
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SkipText"), 1),
    // King Zora Speed (1 to 5)
    PRESET_ENTRY_FLOAT(CVAR_ENHANCEMENT("MweepSpeed"), 2.0f),
    // Faster Block Push (+0 to +5)
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FasterBlockPush"), 5),
    // Better Owl
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("BetterOwl"), 1),

    // Assignable Tunics and Boots
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AssignableTunicsAndBoots"), 1),
    // Enable passage of time on file select
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("TimeFlowFileSelect"), 1),
    // Inject Item Counts in messages
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InjectItemCounts.GoldSkulltula"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InjectItemCounts.HeartPiece"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InjectItemCounts.HeartContainer"), 1),

    // Dynamic Wallet Icon
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DynamicWalletIcon"), 1),
    // Always show dungeon entrances
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AlwaysShowDungeonMinimapIcon"), 1),

    // Fix L&R Pause menu
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FixMenuLR"), 1),
    // Fix Dungeon entrances
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FixDungeonMinimapIcon"), 1),
    // Fix Two Handed idle animations
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("TwoHandedIdle"), 1),
    // Fix the Gravedigging Tour Glitch
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("GravediggingTourFix"), 1),
    // Fix Deku Nut upgrade
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DekuNutUpgradeFix"), 1),
    // Fix Navi text HUD position
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("NaviTextFix"), 1),
    // Extend Silver Rupee Jingle
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SilverRupeeJingleExtend"), 1),
    // Fix some Floor Switches
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FixFloorSwitches"), 1),

    // Red Ganon blood
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("RedGanonBlood"), 1),
    // Fish while hovering
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("HoverFishing"), 1),
    // N64 Weird Frames
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("N64WeirdFrames"), 1),
    // Bombchus out of bounds
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("BombchusOOB"), 1),
    // Quick Putaway
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("QuickPutaway"), 1),
    // Skip save confirmation
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SkipSaveConfirmation"), 1),
};

const std::vector<PresetEntry> enhancedPresetEntries = {
    // D-pad Support in text and file select
    PRESET_ENTRY_S32(CVAR_SETTING("DpadInText"), 1),
    // Play Ocarina with D-pad
    PRESET_ENTRY_S32(CVAR_SETTING("OcarinaControl.Dpad"), 1),
    // Play Ocarina with Right Stick
    PRESET_ENTRY_S32(CVAR_SETTING("OcarinaControl.RStick"), 1),
    // D-pad as Equip Items
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DpadEquips"), 1),
    // Prevent Dropped Ocarina Inputs
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DpadNoDropOcarinaInput"), 1),

    // Text Speed (1 to 5)
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("TextSpeed"), 5),
    // Slow Text Speed (1 to 5)
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SlowTextSpeed"), 5),
    // Skip Text
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SkipText"), 1),
    // King Zora Speed (1 to 5)
    PRESET_ENTRY_FLOAT(CVAR_ENHANCEMENT("MweepSpeed"), 5.0f),
    // Faster Block Push (+0 to +5)
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FasterBlockPush"), 5),
    // Better Owl
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("BetterOwl"), 1),

    // Assignable Tunics and Boots
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AssignableTunicsAndBoots"), 1),
    // Enable passage of time on file select
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("TimeFlowFileSelect"), 1),
    // Inject Item Counts in messages
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InjectItemCounts.GoldSkulltula"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InjectItemCounts.HeartPiece"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InjectItemCounts.HeartContainer"), 1),

    // Dynamic Wallet Icon
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DynamicWalletIcon"), 1),
    // Always show dungeon entrances
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AlwaysShowDungeonMinimapIcon"), 1),

    // Fix L&R Pause menu
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FixMenuLR"), 1),
    // Fix Dungeon entrances
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FixDungeonMinimapIcon"), 1),
    // Fix Two Handed idle animations
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("TwoHandedIdle"), 1),
    // Fix the Gravedigging Tour Glitch
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("GravediggingTourFix"), 1),
    // Fix Deku Nut upgrade
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DekuNutUpgradeFix"), 1),
    // Fix Navi text HUD position
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("NaviTextFix"), 1),
    // Extend Silver Rupee Jingle
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SilverRupeeJingleExtend"), 1),
    // Fix enemies not spawning on ground over water
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("EnemySpawnsOverWaterboxes"), 1),
    // Fix some Floor Switches
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FixFloorSwitches"), 1),

    // Red Ganon blood
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("RedGanonBlood"), 1),
    // Fish while hovering
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("HoverFishing"), 1),
    // N64 Weird Frames
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("N64WeirdFrames"), 1),
    // Bombchus out of bounds
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("BombchusOOB"), 1),
    // Quick Putaway
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("QuickPutaway"), 1),
    // Skip save confirmation
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SkipSaveConfirmation"), 1),
    // Biggoron Forge Time (0 to 3)
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("ForgeTime"), 0),
    // Vine/Ladder Climb speed (+0 to +12)
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("ClimbSpeed"), 3),
    // Faster Heavy Block Lift
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FasterHeavyBlockLift"), 1),
    // No Forced Navi
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("NoForcedNavi"), 1),
    // No Skulltula Freeze
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SkulltulaFreeze"), 1),
    // MM Bunny Hood
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MMBunnyHood"), BUNNY_HOOD_FAST_AND_JUMP),
    // Adult Bunny Hood
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AdultMasks"), 1),
    // Fast Chests
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastChests"), 1),
    // Fast Drops
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastDrops"), 1),
    // Fast Ocarina Playback
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastOcarinaPlayback"), 1),
    // Instant Putaway
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InstantPutaway"), 1),
    // Instant Boomerang Recall
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastBoomerang"), 1),
    // Nuts Explode Bombs
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("NutsExplodeBombs"), 1),
    // Ask to Equip New Items
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AskToEquip"), 1),
    // Mask Select in Inventory
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MaskSelect"), 1),
    // Always Win Goron Pot
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("GoronPot"), 1),
    // Always Win Dampe Digging
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DampeWin"), 1),
    // Skip Magic Arrow Equip Animation
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SkipArrowAnimation"), 1),

    // Equip arrows on multiple slots
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SeparateArrows"), 1),

    // Disable Navi Call Audio
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DisableNaviCallAudio"), 1),

    // Equipment Toggle
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("EquipmentCanBeRemoved"), 1),
    // Link's Cow in Both Time Periods
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("CowOfTime"), 1),

    // Enable 3D Dropped items/projectiles
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("NewDrops"), 1),

    // Fix Anubis fireballs
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AnubisFix"), 1),

    // Autosave
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("Autosave"), 1),

    // Bombchu shop doesn't sell out, and 10 bombchus cost 99 instead of 100
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("BetterBombchuShopping"), 1),
};

const std::vector<PresetEntry> randomizerPresetEntries = {
    // D-pad Support in text and file select
    PRESET_ENTRY_S32(CVAR_SETTING("DpadInText"), 1),
    // Play Ocarina with D-pad
    PRESET_ENTRY_S32(CVAR_SETTING("OcarinaControl.Dpad"), 1),
    // Play Ocarina with Right Stick
    PRESET_ENTRY_S32(CVAR_SETTING("OcarinaControl.RStick"), 1),
    // D-pad as Equip Items
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DpadEquips"), 1),
    // Prevent Dropped Ocarina Inputs
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DpadNoDropOcarinaInput"), 1),

    // Text Speed (1 to 5)
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("TextSpeed"), 5),
    // Slow Text Speed (1 to 5)
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SlowTextSpeed"), 5),
    // Skip Text
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SkipText"), 1),
    // King Zora Speed (1 to 5)
    PRESET_ENTRY_FLOAT(CVAR_ENHANCEMENT("MweepSpeed"), 5.0f),
    // Faster Block Push (+0 to +5)
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FasterBlockPush"), 5),
    // Better Owl
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("BetterOwl"), 1),

    // Assignable Tunics and Boots
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AssignableTunicsAndBoots"), 1),
    // Enable passage of time on file select
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("TimeFlowFileSelect"), 1),
    // Inject Item Counts in messages
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InjectItemCounts.GoldSkulltula"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InjectItemCounts.HeartPiece"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InjectItemCounts.HeartContainer"), 1),

    // Dynamic Wallet Icon
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DynamicWalletIcon"), 1),
    // Always show dungeon entrances
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AlwaysShowDungeonMinimapIcon"), 1),

    // Fix L&R Pause menu
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FixMenuLR"), 1),
    // Fix Dungeon entrances
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FixDungeonMinimapIcon"), 1),
    // Fix Two Handed idle animations
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("TwoHandedIdle"), 1),
    // Fix the Gravedigging Tour Glitch
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("GravediggingTourFix"), 1),
    // Fix Deku Nut upgrade
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DekuNutUpgradeFix"), 1),
    // Fix Navi text HUD position
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("NaviTextFix"), 1),
    // Extend Silver Rupee Jingle
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SilverRupeeJingleExtend"), 1),
    // Fix some Floor Switches
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FixFloorSwitches"), 1),

    // Red Ganon blood
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("RedGanonBlood"), 1),
    // Fish while hovering
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("HoverFishing"), 1),
    // N64 Weird Frames
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("N64WeirdFrames"), 1),
    // Bombchus out of bounds
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("BombchusOOB"), 1),
    // Quick Putaway
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("QuickPutaway"), 1),
    // Skip save confirmation
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SkipSaveConfirmation"), 1),
    // Biggoron Forge Time (0 to 3)
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("ForgeTime"), 0),
    // Vine/Ladder Climb speed (+0 to +12)
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("ClimbSpeed"), 3),
    // Faster Heavy Block Lift
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FasterHeavyBlockLift"), 1),
    // No Forced Navi
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("NoForcedNavi"), 1),
    // No Skulltula Freeze
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SkulltulaFreeze"), 1),
    // MM Bunny Hood
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MMBunnyHood"), BUNNY_HOOD_FAST_AND_JUMP),
    // Adult Bunny Hood
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AdultMasks"), 1),
    // Fast Chests
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastChests"), 1),
    // Fast Drops
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastDrops"), 1),
    // Fast Ocarina Playback
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastOcarinaPlayback"), 1),
    // Instant Putaway
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InstantPutaway"), 1),
    // Instant Boomerang Recall
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastBoomerang"), 1),
    // Nuts Explode Bombs
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("NutsExplodeBombs"), 1),
    // Ask to Equip New Items
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AskToEquip"), 1),
    // Mask Select in Inventory
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MaskSelect"), 1),
    // Always Win Goron Pot
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("GoronPot"), 1),
    // Always Win Dampe Digging
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DampeWin"), 1),
    // Skip Magic Arrow Equip Animation
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SkipArrowAnimation"), 1),
    // Exit Market at Night
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MarketSneak"), 1),

    // Equip arrows on multiple slots
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SeparateArrows"), 1),

    // Disable Navi Call Audio
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DisableNaviCallAudio"), 1),

    // Equipment Toggle
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("EquipmentCanBeRemoved"), 1),
    // Link's Cow in Both Time Periods
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("CowOfTime"), 1),

    // Enable 3D Dropped items/projectiles
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("NewDrops"), 1),

    // Fix Anubis fireballs
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AnubisFix"), 1),

    // Autosave
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("Autosave"), 1),

    // Customize Fishing Behaviour
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("CustomizeFishing"), 1),
    // Guarantee Bite
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("GuaranteeFishingBite"), 1),
    // Fish Never Escape
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FishNeverEscape"), 1),
    // Child Minimum Weight (6 to 10)
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MinimumFishWeightChild"), 3),
    // Adult Minimum Weight (8 to 13)
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MinimumFishWeightAdult"), 6),

    // Customize Lost Woods Ocarina Game Behavior
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("CustomizeOcarinaGame"), 1),
    // Start With Five Notes
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("OcarinaGame.StartingNotes"), 5),
    // Round One Notes
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("OcarinaGame.RoundOneNotes"), 5),

    // Visual Stone of Agony
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("VisualAgony"), 1),
    // Pull grave during the day
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DayGravePull"), 1),
    // Pull out Ocarina to Summon Scarecrow
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InstantScarecrow"), 1),
    // Chest size & texture matches contents
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"), CSMC_BOTH),

    // Color Temple of Time's Medallions
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("ToTMedallionsColors"), 1),

    // Frames to wait
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MinFrameCount"), 200),

    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("NoInputForCredits"), 1),

    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastFarores"), 1),
};

const std::vector<PresetEntry> spockRacePresetEntries = {
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("LogicRules"), 0),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("TextSpeed"), 5),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SlowTextSpeed"), 5),
    PRESET_ENTRY_FLOAT(CVAR_ENHANCEMENT("MweepSpeed"), 5.0f),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("ClimbSpeed"), 4),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FasterBlockPush"), 5),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FasterHeavyBlockLift"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("NoForcedNavi"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DisableNaviCallAudio"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastChests"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastDrops"), 1),
    PRESET_ENTRY_S32(CVAR_SETTING("DpadInText"), 1),
    PRESET_ENTRY_S32(CVAR_SETTING("OcarinaControl.Dpad"), 1),
    PRESET_ENTRY_S32(CVAR_SETTING("OcarinaControl.RStick"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DpadEquips"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastOcarinaPlayback"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InstantScarecrow"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MarketSneak"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InstantPutaway"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastBoomerang"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AdultMasks"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MMBunnyHood"), BUNNY_HOOD_FAST),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SeparateArrows"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AssignableTunicsAndBoots"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("EquipmentCanBeRemoved"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DayGravePull"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("NewDrops"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("CreditsFix"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SkipText"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SkulltulaFreeze"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("PauseAnyCursor"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastFarores"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("NoInputForCredits"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MaskSelect"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InjectItemCounts.GoldSkulltula"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InjectItemCounts.HeartPiece"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InjectItemCounts.HeartContainer"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("CustomizeFishing"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FishNeverEscape"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InstantFishing"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("GuaranteeFishingBite"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MinimumFishWeightAdult"), 6),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MinimumFishWeightChild"), 3),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("GoronPot"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("ForgeTime"), 0),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DampeAllNight"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("10GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("20GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("30GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("40GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("50GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("AllLocationsReachable"), 0),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BlueFireArrows"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BombchuBag"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CompleteMaskQuest"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CuccosToReturn"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DampeHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DoorOfTime"), RO_DOOROFTIME_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("EnableBombchuDrops"), 1),
    PRESET_ENTRY_CPP_STRING(CVAR_RANDOMIZER_SETTING("ExcludedLocations"), FormatLocations( 
        { RC_MARKET_10_BIG_POES, RC_KAK_40_GOLD_SKULLTULA_REWARD, RC_KAK_50_GOLD_SKULLTULA_REWARD, RC_ZR_FROGS_OCARINA_GAME })),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ClosedForest"), RO_CLOSED_FOREST_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("FullWallets"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GanonTrial"), RO_GANONS_TRIALS_SKIP),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("FortressCarpenters"), RO_GF_CARPENTERS_FAST),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GregHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GsExpectSunsSong"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("KakarikoGate"), RO_KAK_GATE_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("LacsRewardCount"), 5),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("RainbowBridge"), RO_BRIDGE_GREG),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ScrubText"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("IncludeTycoonWallet"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Shopsanity"), RO_SHOPSANITY_RANDOM),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShopsanityPrices"), RO_PRICE_BALANCED),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleGanonBossKey"), RO_GANON_BOSS_KEY_LACS_REWARDS),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleKeyRings"), RO_KEYRINGS_COUNT),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleKokiriSword"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleOcarinas"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleScrubs"), RO_SCRUBS_ALL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ScrubsPrice"), RO_PRICE_FIXED),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ScrubsFixedPrice"), 10),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipChildStealth"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipChildZelda"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipEponaRace"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipScarecrowsSong"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipTowerEscape"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingConsumables"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingMapsCompasses"), RO_DUNGEON_ITEM_LOC_STARTWITH),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingOcarina"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SunlightArrows"), 1),
};

const std::vector<PresetEntry> spockRaceNoLogicPresetEntries = {
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AdultMasks"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MinimumFishWeightAdult"), 6),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AssignableTunicsAndBoots"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MinimumFishWeightChild"), 3),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("ClimbSpeed"), 4),
    PRESET_ENTRY_S32(CVAR_COSMETIC("Goron.NeckLength"), 1000),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("CreditsFix"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("CustomizeFishing"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DampeAllNight"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DayGravePull"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DisableNaviCallAudio"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("DpadNoDropOcarinaInput"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("EquipmentCanBeRemoved"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastBoomerang"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastChests"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastDrops"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastFarores"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastOcarinaPlayback"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FasterBlockPush"), 5),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FasterHeavyBlockLift"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FishNeverEscape"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("ForgeTime"), 0),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("GoronPot"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("GuaranteeFishingBite"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InjectItemCounts.GoldSkulltula"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InjectItemCounts.HeartPiece"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InjectItemCounts.HeartContainer"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InstantFishing"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InstantPutaway"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MMBunnyHood"), BUNNY_HOOD_FAST),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MarketSneak"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MaskSelect"), 1),
    PRESET_ENTRY_FLOAT(CVAR_ENHANCEMENT("MweepSpeed"), 5.0f),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("NoForcedNavi"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("NoInputForCredits"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("PauseAnyCursor"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("10GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("20GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("30GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("40GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("50GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("AllLocationsReachable"), 0),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BlueFireArrows"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BombchuBag"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BossKeysanity"), RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CompleteMaskQuest"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CuccosToReturn"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DampeHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DoorOfTime"), RO_DOOROFTIME_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("EnableBombchuDrops"), 1),
    PRESET_ENTRY_CPP_STRING(CVAR_RANDOMIZER_SETTING("ExcludedLocations"), FormatLocations(
        { RC_MARKET_10_BIG_POES, RC_KAK_40_GOLD_SKULLTULA_REWARD, RC_KAK_50_GOLD_SKULLTULA_REWARD, RC_ZR_FROGS_OCARINA_GAME })),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ClosedForest"), RO_CLOSED_FOREST_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("FullWallets"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GanonTrial"), RO_GANONS_TRIALS_SKIP),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("FortressCarpenters"), RO_GF_CARPENTERS_FAST),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GerudoKeys"), RO_GERUDO_KEYS_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GregHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GsExpectSunsSong"), 0),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("KakarikoGate"), RO_KAK_GATE_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Keysanity"), RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("LacsRewardCount"), 5),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("LogicRules"), RO_LOGIC_NO_LOGIC),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("RainbowBridge"), RO_BRIDGE_GREG),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ScrubText"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("IncludeTycoonWallet"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Shopsanity"), RO_SHOPSANITY_RANDOM),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShopsanityPrices"), RO_PRICE_BALANCED),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleAdultTrade"), 0),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleMerchants"), RO_SHUFFLE_MERCHANTS_BEANS_ONLY),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleMerchantPrices"), RO_PRICE_VANILLA),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleGanonBossKey"), RO_GANON_BOSS_KEY_LACS_REWARDS),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleGerudoToken"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleKeyRings"), RO_KEYRINGS_COUNT),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleKokiriSword"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleOcarinas"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleScrubs"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleSongs"), RO_SONG_SHUFFLE_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleTokens"), RO_TOKENSANITY_ALL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipChildStealth"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipChildZelda"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipEponaRace"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipScarecrowsSong"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipTowerEscape"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingConsumables"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingMapsCompasses"), RO_DUNGEON_ITEM_LOC_STARTWITH),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingOcarina"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SunlightArrows"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SeparateArrows"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("InstantScarecrow"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("SkulltulaFreeze"), 1),
};

const std::vector<PresetEntry> s6PresetEntries = {
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"), CSMC_BOTH),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastChests"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MMBunnyHood"), BUNNY_HOOD_FAST),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AdultMasks"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BigPoeTargetCount"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CuccosToReturn"), 4),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DoorOfTime"), RO_DOOROFTIME_OPEN),
    PRESET_ENTRY_CPP_STRING(CVAR_RANDOMIZER_SETTING("ExcludedLocations"), FormatLocations({ RC_DEKU_THEATER_MASK_OF_TRUTH })),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ClosedForest"), RO_CLOSED_FOREST_DEKU_ONLY),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GanonTrial"), RO_GANONS_TRIALS_SKIP),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("FortressCarpenters"), RO_GF_CARPENTERS_FAST),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("IceTraps"), RO_ICE_TRAPS_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("KakarikoGate"), RO_KAK_GATE_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("MedallionCount"), 6),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("MQDungeons"), RO_MQ_DUNGEONS_NONE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("RainbowBridge"), RO_BRIDGE_MEDALLIONS),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleAdultTrade"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleDungeonReward"), RO_DUNGEON_REWARDS_END_OF_DUNGEON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleGanonBossKey"), RO_GANON_BOSS_KEY_STARTWITH),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleKokiriSword"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleOverworldSpawns"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipChildStealth"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipChildZelda"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipEponaRace"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipTowerEscape"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SleepingWaterfall"), RO_WATERFALL_CLOSED),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingAge"), RO_AGE_RANDOM),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingConsumables"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingDekuShield"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingMapsCompasses"), RO_DUNGEON_ITEM_LOC_STARTWITH),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingOcarina"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ZorasFountain"), RO_ZF_CLOSED),
};

const std::vector<PresetEntry> hellModePresetEntries = {
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"), CSMC_BOTH),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("FastChests"), 1),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("MMBunnyHood"), BUNNY_HOOD_FAST),
    PRESET_ENTRY_S32(CVAR_ENHANCEMENT("AdultMasks"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BigPoeTargetCount"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BlueFireArrows"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BossKeysanity"), RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CompleteMaskQuest"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CuccosToReturn"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DoorOfTime"), RO_DOOROFTIME_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("EnableBombchuDrops"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ClosedForest"), RO_CLOSED_FOREST_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GanonTrial"), RO_GANONS_TRIALS_SET_NUMBER),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GanonTrialCount"), 6),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GerudoKeys"), RO_GERUDO_KEYS_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GsExpectSunsSong"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("IceTraps"), RO_ICE_TRAPS_ONSLAUGHT),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ItemPool"), RO_ITEM_POOL_MINIMAL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("KakarikoGate"), RO_KAK_GATE_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Keysanity"), RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("LinksPocket"), RO_LINKS_POCKET_NOTHING),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("MQDungeons"), RO_MQ_DUNGEONS_RANDOM_NUMBER),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("RainbowBridge"), RO_BRIDGE_DUNGEON_REWARDS),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("IncludeTycoonWallet"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Shopsanity"), RO_SHOPSANITY_SPECIFIC_COUNT),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShopsanityCount"), RO_SHOPSANITY_COUNT_FOUR_ITEMS),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShopsanityPrices"), RO_PRICE_RANGE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShopsanityPriceRange1"), 0),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShopsanityPriceRange2"), 999),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleAdultTrade"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleMerchants"), RO_SHUFFLE_MERCHANTS_BEANS_ONLY),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleMerchantPrices"), RO_PRICE_VANILLA),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleCows"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleDungeonReward"), RO_DUNGEON_REWARDS_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleFrogSongRupees"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleGanonBossKey"), RO_GANON_BOSS_KEY_LACS_DUNGEONS),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleGerudoToken"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleKeyRings"), RO_KEYRINGS_RANDOM),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleKokiriSword"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleMasterSword"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleOcarinas"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleScrubs"), RO_SCRUBS_ALL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ScrubsPrice"), RO_PRICE_CHEAP_BALANCED),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleSongs"), RO_SONG_SHUFFLE_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleTokens"), RO_TOKENSANITY_ALL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleWeirdEgg"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipChildStealth"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipEponaRace"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipScarecrowsSong"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipTowerEscape"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SleepingWaterfall"), RO_WATERFALL_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingAge"), RO_AGE_RANDOM),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingMapsCompasses"), RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SunlightArrows"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ZorasFountain"), RO_ZF_OPEN),
};

const std::vector<PresetEntry> BenchmarkPresetEntries = {
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ClosedForest"), RO_CLOSED_FOREST_DEKU_ONLY),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("KakarikoGate"), RO_KAK_GATE_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DoorOfTime"), RO_DOOROFTIME_SONGONLY),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SleepingWaterfall"), RO_WATERFALL_CLOSED),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ZorasFountain"), RO_ZF_CLOSED),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("FortressCarpenters"), RO_GF_CARPENTERS_NORMAL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("RainbowBridge"), RO_BRIDGE_DUNGEON_REWARDS),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("RewardCount"), 5),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BridgeRewardOptions"), RO_BRIDGE_GREG_REWARD),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GanonTrial"), RO_GANONS_TRIALS_SET_NUMBER),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GanonTrialCount"), 6),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingAge"), RO_AGE_RANDOM),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleDungeonsEntrances"), RO_DUNGEON_ENTRANCE_SHUFFLE_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleBossEntrances"), RO_BOSS_ROOM_ENTRANCE_SHUFFLE_FULL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleOverworldEntrances"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleInteriorsEntrances"), RO_INTERIOR_ENTRANCE_SHUFFLE_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleGrottosEntrances"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleWarpSongs"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleOverworldSpawns"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("MixedEntrances"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DecoupleEntrances"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BombchusInLogic"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("EnableBombchuDrops"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("TriforceHunt"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("MQDungeons"), RO_MQ_DUNGEONS_RANDOM_NUMBER),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("MQDungeonsSelection"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleDungeonReward"), RO_DUNGEON_REWARDS_END_OF_DUNGEON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("LinksPocket"), RO_LINKS_POCKET_DUNGEON_REWARD),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleSongs"), RO_SONG_SHUFFLE_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Shopsanity"), RO_SHOPSANITY_SPECIFIC_COUNT),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShopsanityCount"), RO_SHOPSANITY_COUNT_FOUR_ITEMS),
    //RANDOTODO add refactored price/scrub/merchant settings
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleTokens"), RO_TOKENSANITY_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleBeehives"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleCows"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleKokiriSword"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleMasterSword"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleChildWallet"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleOcarinas"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleOcarinaButtons"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleSwim"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleGerudoToken"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleFrogSongRupees"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleAdultTrade"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Shuffle100GSReward"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleBossSouls"), RO_BOSS_SOULS_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleDekuStickBag"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleDekuNutBag"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Fishsanity"), RO_FISHSANITY_BOTH),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("FishsanityAgeSplit"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingMapsCompasses"), RO_DUNGEON_ITEM_LOC_OWN_DUNGEON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GerudoKeys"), RO_GERUDO_KEYS_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BossKeysanity"), RO_DUNGEON_ITEM_LOC_OWN_DUNGEON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleGanonBossKey"), RO_GANON_BOSS_KEY_LACS_MEDALLIONS),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("LacsMedallionCount"), 6),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleKeyRings"), RO_KEYRINGS_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipChildZelda"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipEponaRace"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipScarecrowsSong"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BigPoeTargetCount"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CuccosToReturn"), 4),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CompleteMaskQuest"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GossipStoneHints"), RO_GOSSIP_STONES_NEED_NOTHING),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("HintClarity"), RO_HINT_CLARITY_CLEAR),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("HintDistribution"), RO_HINT_DIST_STRONG),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("AltarHint"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GanondorfHint"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SheikLAHint"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DampeHint"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GregHint"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SariaHint"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("FrogsHint"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("OoTHint"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BiggoronHint"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BigPoesHint"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ChickensHint"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("MalonHint"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("HBAHint"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("WarpSongText"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ScrubText"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("10GSHint"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("20GSHint"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("30GSHint"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("40GSHint"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("50GSHint"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("MaskShopHint"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BlueFireArrows"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SunlightArrows"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("InfiniteUpgrades"), RO_INF_UPGRADES_PROGRESSIVE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkeletonKey"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ItemPool"), RO_ITEM_POOL_BALANCED),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("IceTraps"), RO_ICE_TRAPS_NORMAL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingOcarina"), RO_STARTING_OCARINA_FAIRY),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingDekuShield"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingKokiriSword"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingMasterSword"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingConsumables"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("FullWallets"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingZeldasLullaby"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingEponasSong"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingSariasSong"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingSunsSong"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingSongOfTime"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingSongOfStorms"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingMinuetOfForest"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingBoleroOfFire"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingSerenadeOfWater"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingRequiemOfSpirit"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingNocturneOfShadow"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingPreludeOfLight"), RO_GENERIC_OFF),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingSkulltulaToken"), 0),
    PRESET_ENTRY_S32("gRandomizeStartingHearts", 2),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("LogicRules"), RO_LOGIC_GLITCHLESS),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("AllLocationsReachable"), RO_GENERIC_ON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GsExpectSunsSong"), RO_GENERIC_ON),
};

typedef struct PresetDefinition {
    const char* label;
    const char* description;
    std::vector<PresetEntry> entries;
} PresetDefinition;

typedef struct PresetTypeDefinition {
    std::vector<const char*> blocksToClear;
    std::map<uint16_t, PresetDefinition> presets;
} PresetTypeDefinition;

const std::map<PresetType, PresetTypeDefinition> presetTypes = {
    { PRESET_TYPE_ENHANCEMENTS, { { CVAR_PREFIX_ENHANCEMENT, CVAR_PREFIX_CHEAT }, {
        { ENHANCEMENT_PRESET_DEFAULT, {
            "Default",
            "Reset all options to their default values.",
            {},
        } },
        { ENHANCEMENT_PRESET_VANILLA_PLUS, {
            "Vanilla Plus",
            "Adds Quality of Life features that enhance your experience, but don't alter gameplay. Recommended for a first playthrough of OoT.",
            vanillaPlusPresetEntries,
        } },
        { ENHANCEMENT_PRESET_ENHANCED, {
            "Enhanced",
            "The \"Vanilla Plus\" preset, but with more quality of life enhancements that might alter gameplay slightly. Recommended for returning players.",
            enhancedPresetEntries
        } },
        { ENHANCEMENT_PRESET_RANDOMIZER, {
            "Randomizer",
            "The \"Enhanced\" preset, plus any other enhancements that are recommended for playing Randomizer.",
            randomizerPresetEntries
        } },
    } } },
    { PRESET_TYPE_RANDOMIZER, { { CVAR_PREFIX_RANDOMIZER_SETTING, CVAR_PREFIX_RANDOMIZER_ENHANCEMENT }, {
        { RANDOMIZER_PRESET_DEFAULT, {
            "Default",
            "Reset all options to their default values.",
            {},
        } },
        { RANDOMIZER_PRESET_SPOCK_RACE, {
            "Spock Race",
            "Race preset used for the official Ship of Harkinian race on June 3rd 2023. The following settings are notable:\n" \
            "- Rainbow Bridge is set to Greg\n" \
            "- Ganon's Boss Key is 5 dungeon rewards\n" \
            "- Shopsanity and Scrubsanity enabled\n" \
            "- All locations reachable is off\n", \
            spockRacePresetEntries,
        } },
        { RANDOMIZER_PRESET_SPOCK_RACE_NO_LOGIC, {
                "Spock Race - No Logic",
                "No Logic Race preset used for official Ship of Harkinian No Logic races. The following settings are "
                "notable:\n"
                "- Rainbow Bridge is set to Greg\n"
                "- Ganon's Boss Key is 5 dungeon rewards\n"
                "- Shopsanity and Scrubsanity enabled\n"
                "- All locations reachable is off\n",
                spockRaceNoLogicPresetEntries,
            } },
        { RANDOMIZER_PRESET_S6, {
            "S6 Tournament (Adapted)",
            "Matches OOTR S6 tournament settings as close as we can get with the options available in SoH. The following differences are notable:\n" \
            "- Both child and adult overworld spawns are randomized\n" \
            "- Dungeon rewards are shuffled at the end of dungeons, rather than at the end of their own dungeon\n" \
            "- Full adult trade sequence is shuffled instead of the selected 4\n" \
            "- Hint distribution no \"tournament\" mode, falling back to balanced",
            s6PresetEntries,
        } },
        { RANDOMIZER_PRESET_HELL_MODE, {
            "Hell Mode",
            "All settings maxed but still using glitchless logic. Expect pain.",
            hellModePresetEntries
        } },
        { RANDOMIZER_PRESET_BENCHMARK, {
            "Benchmark",
            "Used for benchmarking the logic.",
            BenchmarkPresetEntries
        } },
    } } }
};
