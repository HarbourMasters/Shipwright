#pragma once

#include <map>
#include <vector>
#include <variant>
#include <cstdint>
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/enhancementTypes.h"

enum PresetEntryType {
    PRESET_ENTRY_TYPE_S32,
    PRESET_ENTRY_TYPE_FLOAT,
    PRESET_ENTRY_TYPE_STRING,
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
};

typedef struct PresetEntry {
    const char* cvar;
    PresetEntryType type;
    std::variant<int32_t, float, const char*> value;
} PresetEntry;

#define PRESET_ENTRY_S32(cvar, value) \
    { cvar,    PRESET_ENTRY_TYPE_S32, value }
#define PRESET_ENTRY_FLOAT(cvar, value) \
    { cvar,  PRESET_ENTRY_TYPE_FLOAT, value }
#define PRESET_ENTRY_STRING(cvar, value) \
    { cvar, PRESET_ENTRY_TYPE_STRING, value }

void DrawPresetSelector(PresetType presetType);
void clearCvars(std::vector<const char*> cvarsToClear);
void applyPreset(std::vector<PresetEntry> entries);

// TODO: Ideally everything below this point will come from one/many JSON files

const std::vector<const char*> enhancementsCvars = {
    "gDpadPause",
    "gDpadText",
    "gDpadOcarina",
    "gRStickOcarina",
    "gDpadEquips",
    "gPauseAnyCursor",
    "gDpadNoDropOcarinaInput",
    "gNaviOnL",
    "gInvertXAxis",
    "gInvertYAxis",
    "gRightStickAiming",
    "gDisableAutoCenterViewFirstPerson",
    "gTextSpeed",
    "gMweepSpeed",
    "gForgeTime",
    "gClimbSpeed",
    "gFasterBlockPush",
    "gFasterHeavyBlockLift",
    "gNoForcedNavi",
    "gSkulltulaFreeze",
    "gMMBunnyHood",
    "gAdultBunnyHood",
    "gFastChests",
    "gChestSizeAndTextureMatchesContents",
    "gFastDrops",
    "gBetterOwl",
    "gFastOcarinaPlayback",
    "gInstantPutaway",
    "gFastBoomerang",
    "gAskToEquip",
    "gMaskSelect",
    "gRememberSaveLocation",
    "gDamageMul",
    "gFallDamageMul",
    "gVoidDamageMul",
    "gBonkDamageMul",
    "gNoRandomDrops",
    "gNoHeartDrops",
    "gBombchuDrops",
    "gGoronPot",
    "gFullHealthSpawn",
    "gDampeWin",
    "gRedPotionEffect",
    "gRedPotionHealth",
    "gRedPercentRestore",
    "gGreenPotionEffect",
    "gGreenPotionMana",
    "gGreenPercentRestore",
    "gBluePotionEffects",
    "gBluePotionHealth",
    "gBlueHealthPercentRestore",
    "gBluePotionMana",
    "gBlueManaPercentRestore",
    "gMilkEffect",
    "gMilkHealth",
    "gMilkPercentRestore",
    "gSeparateHalfMilkEffect",
    "gHalfMilkHealth",
    "gHalfMilkPercentRestore",
    "gFairyEffect",
    "gFairyHealth",
    "gFairyPercentRestore",
    "gFairyReviveEffect",
    "gFairyReviveHealth",
    "gFairyRevivePercentRestore",
    "gCustomizeFishing",
    "gInstantFishing",
    "gGuaranteeFishingBite",
    "gFishNeverEscape",
    "gChildMinimumWeightFish",
    "gAdultMinimumWeightFish",
    "gLowHpAlarm",
    "gMinimalUI",
    "gDisableNaviCallAudio",
    "gVisualAgony",
    "gAssignableTunicsAndBoots",
    "gEquipmentCanBeRemoved",
    "gCowOfTime",
    "gGuardVision",
    "gTimeFlowFileSelect",
    "gInjectItemCounts",
    "gDayGravePull",
    "gDampeAllNight",
    "gSkipSwimDeepEndAnim",
    "gSkipScarecrow",
    "gBlueFireArrows",
    "gSunlightArrows",
    "gPauseLiveLinkRotation",
    "gPauseLiveLink",
    "gMinFrameCount",
    "gN64Mode",
    "gNewDrops",
    "gDisableBlackBars",
    "gDynamicWalletIcon",
    "gAlwaysShowDungeonMinimapIcon",
    "gUniformLR",
    "gNGCKaleidoSwitcher",
    "gFixDungeonMinimapIcon",
    "gTwoHandedIdle",
    "gGravediggingTourFix",
    "gDekuNutUpgradeFix",
    "gNaviTextFix",
    "gAnubisFix",
    "gCrouchStabHammerFix",
    "gCrouchStabFix",
    "gGerudoWarriorClothingFix",
    "gFixCameraDrift",
    "gFixCameraSwing",
    "gFixHangingLedgeSwingRate",
    "gRedGanonBlood",
    "gHoverFishing",
    "gN64WeirdFrames",
    "gBombchusOOB",
    "gQuickPutaway",
    "gGsCutscene",
    "gRestoreRBAValues",
    "gSkipSaveConfirmation",
    "gAutosave",
    "gDisableCritWiggle",
    "gChestSizeDependsStoneOfAgony",
    "gSkipArrowAnimation",
    "gSeparateArrows",
    "gCustomizeShootingGallery",
    "gInstantShootingGalleryWin",
    "gConstantAdultGallery",
    "gChildShootingGalleryAmmunition",
    "gAdultShootingGalleryAmmunition",
    "gCustomizeBombchuBowling",
    "gBombchuBowlingNoSmallCucco",
    "gBombchuBowlingNoBigCucco",
    "gBombchuBowlingAmmunition",
    "gCreditsFix",
    "gSilverRupeeJingleExtend",
    "gStaticExplosionRadius",
    "gNoInputForCredits",
    "gFastFarores",
    "gNightGSAlwaysSpawn",
    "gSkipText",
    "gLinkDefaultName",
    "gMarketSneak",
    "gTimeTravel",
    "gNutsExplodeBombs",
    "gBowSlingShotAmmoFix",
    "gBetterFW",
    "gDisableFirstPersonChus",
    "gHyperBosses",
    "gRupeeDash",
    "gDashInterval",
    "gDogFollowsEverywhere",
    "gDisableTunicWarningText",
    "gDisableLOD",
    "gDisableDrawDistance",
    "gDisableKokiriDrawDistance",
    "gLowResMode",
    "gDrawLineupTick",
    "gQuickBongoKill",
    "gFPSGauntlets",
    "gSceneSpecificDirtPathFix",
    "gZFightingMode",
    "gAuthenticLogo",
    "gPauseLiveLinkRotationSpeed",
    "gBowReticle",
    "gFixTexturesOOB",
    "gIvanCoopModeEnabled",
    "gEnemySpawnsOverWaterboxes",
};

const std::vector<const char*> cheatCvars = {
    "gConsoleEnabled",
    "gActorViewerEnabled",
    "gCollisionViewerEnabled",
    "gDLViewerEnabled",
    "gSaveEditorEnabled",
    "gEnableWalkModify",
    "gWalkSpeedToggle",
    "gWalkModifierOne",
    "gWalkModifierTwo",
    "gGoronPot",
    "gDampeWin",
    "gCustomizeShootingGallery",
    "gCustomizeBombchuBowling",
    "gCustomizeFishing",
    "gInfiniteAmmo",
    "gInfiniteEpona",
    "gInfiniteHealth",
    "gInfiniteMagic",
    "gInfiniteMoney",
    "gInfiniteNayru",
    "gNoClip",
    "gClimbEverything",
    "gHookshotEverything",
    "gCheatHookshotReachMultiplier",
    "gMoonJumpOnL",
    "gSuperTunic",
    "gEzISG",
    "gEzQPA",
    "gTimelessEquipment",
    "gCheatEasyPauseBufferEnabled",
    "gCheatEasyInputBufferingEnabled",
    "gNoRestrictItems",
    "gFreezeTime",
    "gPrevTime",
    "gDropsDontDie",
    "gFireproofDekuShield",
    "gShieldTwoHanded",
    "gTimeSync",
    "gDebugEnabled",
    "gSkulltulaDebugEnabled",
    "gSkipLogoTitle",
    "gSaveFileID",
    "gEnableBetaQuest",
    "gBetterDebugWarpScreen",
    "gSwitchAge",
    "gSwitchTimeline",
    "gNoRedeadFreeze",
    "gBombTimerMultiplier",
    "gNoFishDespawn",
    "gNoBugsDespawn"
};

const std::vector<const char*> randomizerCvars = {
    "gRandomize10GSHint",
    "gRandomize20GSHint",
    "gRandomize30GSHint",
    "gRandomize40GSHint",
    "gRandomize50GSHint",
    "gRandomizeAllLocationsReachable",
    "gRandomizeAltarHint",
    "gRandomizeBigPoeTargetCount",
    "gRandomizeBlueFireArrows",
    "gRandomizeBombchusInLogic",
    "gRandomizeBossKeysanity",
    "gRandomizeBridgeRewardOptions",
    "gRandomizeCompleteMaskQuest",
    "gRandomizeCuccosToReturn",
    "gRandomizeDampeHint",
    "gRandomizeDecoupleEntrances",
    "gRandomizeDoorOfTime",
    "gRandomizeDungeonCount",
    "gRandomizeEnableBombchuDrops",
    "gRandomizeEnableGlitchCutscenes",
    "gRandomizeEnabledGlitches",
    "gRandomizeEnabledTricks",
    "gRandomizeExcludedLocations",
    "gRandomizeForest",
    "gRandomizeFullWallets",
    "gRandomizeGanonTrial",
    "gRandomizeGanonTrialCount",
    "gRandomizeGerudoFortress",
    "gRandomizeGerudoKeys",
    "gRandomizeGossipStoneHints",
    "gRandomizeGsExpectSunsSong",
    "gRandomizeHintClarity",
    "gRandomizeHintDistribution",
    "gRandomizeIceTraps",
    "gRandomizeItemPool",
    "gRandomizeKakarikoGate",
    "gRandomizeKeysanity",
    "gRandomizeLacsDungeonCount",
    "gRandomizeLacsMedallionCount",
    "gRandomizeLacsRewardCount",
    "gRandomizeLacsRewardOptions",
    "gRandomizeLacsStoneCount",
    "gRandomizeLacsTokenCount",
    "gRandomizeLAHint",
    "gRandomizeLinksPocket",
    "gRandomizeLogicRules",
    "gRandomizeMedallionCount",
    "gRandomizeMixDungeons",
    "gRandomizeMixedEntrances",
    "gRandomizeMixGrottos",
    "gRandomizeMixInteriors",
    "gRandomizeMixOverworld",
    "gRandomizeMqDungeonCount",
    "gRandomizeMqDungeons",
    "gRandomizeRainbowBridge",
    "gRandomizeRewardCount",
    "gRandomizeScrubText",
    "gRandomizeShopsanity",
    "gRandomizeShopsanityPrices",
    "gRandomizeShopsanityPricesAffordable",
    "gRandomizeShuffleAdultTrade",
    "gRandomizeShuffleBeans",
    "gRandomizeShuffleBossEntrances",
    "gRandomizeShuffleCows",
    "gRandomizeShuffleDungeonReward",
    "gRandomizeShuffleDungeonsEntrances",
    "gRandomizeShuffleFrogSongRupees",
    "gRandomizeShuffleGanonBossKey",
    "gRandomizeShuffleGerudoToken",
    "gRandomizeShuffleGrottosEntrances",
    "gRandomizeShuffleInteriorsEntrances",
    "gRandomizeShuffleKeyRings",
    "gRandomizeShuffleKeyRingsBottomOfTheWell",
    "gRandomizeShuffleKeyRingsFireTemple",
    "gRandomizeShuffleKeyRingsForestTemple",
    "gRandomizeShuffleKeyRingsGanonsCastle",
    "gRandomizeShuffleKeyRingsGerudoFortress",
    "gRandomizeShuffleKeyRingsGTG",
    "gRandomizeShuffleKeyRingsRandomCount",
    "gRandomizeShuffleKeyRingsShadowTemple",
    "gRandomizeShuffleKeyRingsSpiritTemple",
    "gRandomizeShuffleKeyRingsWaterTemple",
    "gRandomizeShuffleKokiriSword",
    "gRandomizeShuffleMerchants",
    "gRandomizeShuffleOcarinas",
    "gRandomizeShuffleOverworldEntrances",
    "gRandomizeShuffleOverworldSpawns",
    "gRandomizeShuffleOwlDrops",
    "gRandomizeShuffleScrubs",
    "gRandomizeShuffleSongs",
    "gRandomizeShuffleTokens",
    "gRandomizeShuffleWarpSongs",
    "gRandomizeShuffleWeirdEgg",
    "gRandomizeSkipChildStealth",
    "gRandomizeSkipChildZelda",
    "gRandomizeSkipEponaRace",
    "gRandomizeSkipScarecrowsSong",
    "gRandomizeSkipTowerEscape",
    "gRandomizeStartingAge",
    "gRandomizeStartingConsumables",
    "gRandomizeStartingBoleroOfFire",
    "gRandomizeStartingDekuShield",
    "gRandomizeStartingEponasSong",
    "gRandomizeStartingKokiriSword",
    "gRandomizeStartingMapsCompasses",
    "gRandomizeStartingMinuetOfForest",
    "gRandomizeStartingNocturneOfShadow",
    "gRandomizeStartingOcarina",
    "gRandomizeStartingPreludeOfLight",
    "gRandomizeStartingRequiemOfSpirit",
    "gRandomizeStartingSariasSong",
    "gRandomizeStartingSerenadeOfWater",
    "gRandomizeStartingSkulltulaToken",
    "gRandomizeStartingSongOfStorms",
    "gRandomizeStartingSongOfTime",
    "gRandomizeStartingSunsSong",
    "gRandomizeStartingZeldasLullaby",
    "gRandomizeStoneCount",
    "gRandomizeSunlightArrows",
    "gRandomizeTokenCount",
    "gRandomizeWarpSongText",
    "gRandomizeZorasFountain",
    "gRandomizeShuffle100GSReward",
    "gRandomizeGregHint",
    "gRandoManualSeedEntry",
    "gRandomizerSettingsEnabled",
};

const std::vector<PresetEntry> vanillaPlusPresetEntries = {
    // D-pad Support in text and file select
    PRESET_ENTRY_S32("gDpadText", 1),
    // Play Ocarina with D-pad
    PRESET_ENTRY_S32("gDpadOcarina", 1),
    // Play Ocarina with Right Stick
    PRESET_ENTRY_S32("gRStickOcarina", 1),
    // D-pad as Equip Items
    PRESET_ENTRY_S32("gDpadEquips", 1),
    // Prevent Dropped Ocarina Inputs
    PRESET_ENTRY_S32("gDpadNoDropOcarinaInput", 1),
    // Right Stick Aiming
    PRESET_ENTRY_S32("gRightStickAiming", 1),

    // Text Speed (1 to 5)
    PRESET_ENTRY_S32("gTextSpeed", 5),
    // Skip Text
    PRESET_ENTRY_S32("gSkipText", 1),
    // King Zora Speed (1 to 5)
    PRESET_ENTRY_S32("gMweepSpeed", 2),
    // Faster Block Push (+0 to +5)
    PRESET_ENTRY_S32("gFasterBlockPush", 5),
    // Better Owl
    PRESET_ENTRY_S32("gBetterOwl", 1),

    // Assignable Tunics and Boots
    PRESET_ENTRY_S32("gAssignableTunicsAndBoots", 1),
    // Enable passage of time on file select
    PRESET_ENTRY_S32("gTimeFlowFileSelect", 1),
    // Inject Item Counts in messages
    PRESET_ENTRY_S32("gInjectItemCounts", 1),

    // Pause link animation (0 to 16)
    PRESET_ENTRY_S32("gPauseLiveLink", 1),

    // Dynamic Wallet Icon
    PRESET_ENTRY_S32("gDynamicWalletIcon", 1),
    // Always show dungeon entrances
    PRESET_ENTRY_S32("gAlwaysShowDungeonMinimapIcon", 1),

    // Fix L&R Pause menu
    PRESET_ENTRY_S32("gUniformLR", 1),
    // Fix Dungeon entrances
    PRESET_ENTRY_S32("gFixDungeonMinimapIcon", 1),
    // Fix Two Handed idle animations
    PRESET_ENTRY_S32("gTwoHandedIdle", 1),
    // Fix the Gravedigging Tour Glitch
    PRESET_ENTRY_S32("gGravediggingTourFix", 1),
    // Fix Deku Nut upgrade
    PRESET_ENTRY_S32("gDekuNutUpgradeFix", 1),
    // Fix Navi text HUD position
    PRESET_ENTRY_S32("gNaviTextFix", 1),
    // Extend Silver Rupee Jingle
    PRESET_ENTRY_S32("gSilverRupeeJingleExtend", 1),

    // Red Ganon blood
    PRESET_ENTRY_S32("gRedGanonBlood", 1),
    // Fish while hovering
    PRESET_ENTRY_S32("gHoverFishing", 1),
    // N64 Weird Frames
    PRESET_ENTRY_S32("gN64WeirdFrames", 1),
    // Bombchus out of bounds
    PRESET_ENTRY_S32("gBombchusOOB", 1),
    // Quick Putaway
    PRESET_ENTRY_S32("gQuickPutaway", 1),
    // Skip save confirmation
    PRESET_ENTRY_S32("gSkipSaveConfirmation", 1),
};

const std::vector<PresetEntry> enhancedPresetEntries = {
    // D-pad Support in text and file select
    PRESET_ENTRY_S32("gDpadText", 1),
    // Play Ocarina with D-pad
    PRESET_ENTRY_S32("gDpadOcarina", 1),
    // Play Ocarina with Right Stick
    PRESET_ENTRY_S32("gRStickOcarina", 1),
    // D-pad as Equip Items
    PRESET_ENTRY_S32("gDpadEquips", 1),
    // Prevent Dropped Ocarina Inputs
    PRESET_ENTRY_S32("gDpadNoDropOcarinaInput", 1),
    // Right Stick Aiming
    PRESET_ENTRY_S32("gRightStickAiming", 1),

    // Text Speed (1 to 5)
    PRESET_ENTRY_S32("gTextSpeed", 5),
    // Skip Text
    PRESET_ENTRY_S32("gSkipText", 1),
    // King Zora Speed (1 to 5)
    PRESET_ENTRY_S32("gMweepSpeed", 5),
    // Faster Block Push (+0 to +5)
    PRESET_ENTRY_S32("gFasterBlockPush", 5),
    // Better Owl
    PRESET_ENTRY_S32("gBetterOwl", 1),

    // Assignable Tunics and Boots
    PRESET_ENTRY_S32("gAssignableTunicsAndBoots", 1),
    // Enable passage of time on file select
    PRESET_ENTRY_S32("gTimeFlowFileSelect", 1),
    // Inject Item Counts in messages
    PRESET_ENTRY_S32("gInjectItemCounts", 1),

    // Pause link animation (0 to 16)
    PRESET_ENTRY_S32("gPauseLiveLink", 1),

    // Dynamic Wallet Icon
    PRESET_ENTRY_S32("gDynamicWalletIcon", 1),
    // Always show dungeon entrances
    PRESET_ENTRY_S32("gAlwaysShowDungeonMinimapIcon", 1),

    // Fix L&R Pause menu
    PRESET_ENTRY_S32("gUniformLR", 1),
    // Fix Dungeon entrances
    PRESET_ENTRY_S32("gFixDungeonMinimapIcon", 1),
    // Fix Two Handed idle animations
    PRESET_ENTRY_S32("gTwoHandedIdle", 1),
    // Fix the Gravedigging Tour Glitch
    PRESET_ENTRY_S32("gGravediggingTourFix", 1),
    // Fix Deku Nut upgrade
    PRESET_ENTRY_S32("gDekuNutUpgradeFix", 1),
    // Fix Navi text HUD position
    PRESET_ENTRY_S32("gNaviTextFix", 1),
    // Extend Silver Rupee Jingle
    PRESET_ENTRY_S32("gSilverRupeeJingleExtend", 1),
    // Fix enemies not spawning on ground over water
    PRESET_ENTRY_S32("gEnemySpawnsOverWaterboxes", 1),

    // Red Ganon blood
    PRESET_ENTRY_S32("gRedGanonBlood", 1),
    // Fish while hovering
    PRESET_ENTRY_S32("gHoverFishing", 1),
    // N64 Weird Frames
    PRESET_ENTRY_S32("gN64WeirdFrames", 1),
    // Bombchus out of bounds
    PRESET_ENTRY_S32("gBombchusOOB", 1),
    // Quick Putaway
    PRESET_ENTRY_S32("gQuickPutaway", 1),
    // Skip save confirmation
    PRESET_ENTRY_S32("gSkipSaveConfirmation", 1),
    // Biggoron Forge Time (0 to 3)
    PRESET_ENTRY_S32("gForgeTime", 0),
    // Vine/Ladder Climb speed (+0 to +12)
    PRESET_ENTRY_S32("gClimbSpeed", 3),
    // Faster Heavy Block Lift
    PRESET_ENTRY_S32("gFasterHeavyBlockLift", 1),
    // No Forced Navi
    PRESET_ENTRY_S32("gNoForcedNavi", 1),
    // No Skulltula Freeze
    PRESET_ENTRY_S32("gSkulltulaFreeze", 1),
    // MM Bunny Hood
    PRESET_ENTRY_S32("gMMBunnyHood", BUNNY_HOOD_FAST_AND_JUMP),
    // Adult Bunny Hood
    PRESET_ENTRY_S32("gAdultBunnyHood", 1),
    // Fast Chests
    PRESET_ENTRY_S32("gFastChests", 1),
    // Fast Drops
    PRESET_ENTRY_S32("gFastDrops", 1),
    // Fast Ocarina Playback
    PRESET_ENTRY_S32("gFastOcarinaPlayback", 1),
    // Instant Putaway
    PRESET_ENTRY_S32("gInstantPutaway", 1),
    // Instant Boomerang Recall
    PRESET_ENTRY_S32("gFastBoomerang", 1),
    // Nuts Explode Bombs
    PRESET_ENTRY_S32("gNutsExplodeBombs", 1),
    // Ask to Equip New Items
    PRESET_ENTRY_S32("gAskToEquip", 1),
    // Mask Select in Inventory
    PRESET_ENTRY_S32("gMaskSelect", 1),
    // Always Win Goron Pot
    PRESET_ENTRY_S32("gGoronPot", 1),
    // Always Win Dampe Digging
    PRESET_ENTRY_S32("gDampeWin", 1),
    // Skip Magic Arrow Equip Animation
    PRESET_ENTRY_S32("gSkipArrowAnimation", 1),

    // Equip arrows on multiple slots
    PRESET_ENTRY_S32("gSeparateArrows", 1),

    // Disable Navi Call Audio
    PRESET_ENTRY_S32("gDisableNaviCallAudio", 1),

    // Equipment Toggle
    PRESET_ENTRY_S32("gEquipmentCanBeRemoved", 1),
    // Link's Cow in Both Time Periods
    PRESET_ENTRY_S32("gCowOfTime", 1),

    // Enable 3D Dropped items/projectiles
    PRESET_ENTRY_S32("gNewDrops", 1),

    // Fix Anubis fireballs
    PRESET_ENTRY_S32("gAnubisFix", 1),

    // Autosave
    PRESET_ENTRY_S32("gAutosave", AUTOSAVE_LOCATION_AND_MAJOR_ITEMS),
};

const std::vector<PresetEntry> randomizerPresetEntries = {
    // D-pad Support in text and file select
    PRESET_ENTRY_S32("gDpadText", 1),
    // Play Ocarina with D-pad
    PRESET_ENTRY_S32("gDpadOcarina", 1),
    // Play Ocarina with Right Stick
    PRESET_ENTRY_S32("gRStickOcarina", 1),
    // D-pad as Equip Items
    PRESET_ENTRY_S32("gDpadEquips", 1),
    // Prevent Dropped Ocarina Inputs
    PRESET_ENTRY_S32("gDpadNoDropOcarinaInput", 1),
    // Right Stick Aiming
    PRESET_ENTRY_S32("gRightStickAiming", 1),

    // Text Speed (1 to 5)
    PRESET_ENTRY_S32("gTextSpeed", 5),
    // Skip Text
    PRESET_ENTRY_S32("gSkipText", 1),
    // King Zora Speed (1 to 5)
    PRESET_ENTRY_S32("gMweepSpeed", 5),
    // Faster Block Push (+0 to +5)
    PRESET_ENTRY_S32("gFasterBlockPush", 5),
    // Better Owl
    PRESET_ENTRY_S32("gBetterOwl", 1),

    // Assignable Tunics and Boots
    PRESET_ENTRY_S32("gAssignableTunicsAndBoots", 1),
    // Enable passage of time on file select
    PRESET_ENTRY_S32("gTimeFlowFileSelect", 1),
    // Inject Item Counts in messages
    PRESET_ENTRY_S32("gInjectItemCounts", 1),

    // Dynamic Wallet Icon
    PRESET_ENTRY_S32("gDynamicWalletIcon", 1),
    // Always show dungeon entrances
    PRESET_ENTRY_S32("gAlwaysShowDungeonMinimapIcon", 1),

    // Fix L&R Pause menu
    PRESET_ENTRY_S32("gUniformLR", 1),
    // Fix Dungeon entrances
    PRESET_ENTRY_S32("gFixDungeonMinimapIcon", 1),
    // Fix Two Handed idle animations
    PRESET_ENTRY_S32("gTwoHandedIdle", 1),
    // Fix the Gravedigging Tour Glitch
    PRESET_ENTRY_S32("gGravediggingTourFix", 1),
    // Fix Deku Nut upgrade
    PRESET_ENTRY_S32("gDekuNutUpgradeFix", 1),
    // Fix Navi text HUD position
    PRESET_ENTRY_S32("gNaviTextFix", 1),
    // Extend Silver Rupee Jingle
    PRESET_ENTRY_S32("gSilverRupeeJingleExtend", 1),

    // Red Ganon blood
    PRESET_ENTRY_S32("gRedGanonBlood", 1),
    // Fish while hovering
    PRESET_ENTRY_S32("gHoverFishing", 1),
    // N64 Weird Frames
    PRESET_ENTRY_S32("gN64WeirdFrames", 1),
    // Bombchus out of bounds
    PRESET_ENTRY_S32("gBombchusOOB", 1),
    // Quick Putaway
    PRESET_ENTRY_S32("gQuickPutaway", 1),
    // Skip save confirmation
    PRESET_ENTRY_S32("gSkipSaveConfirmation", 1),
    // Biggoron Forge Time (0 to 3)
    PRESET_ENTRY_S32("gForgeTime", 0),
    // Vine/Ladder Climb speed (+0 to +12)
    PRESET_ENTRY_S32("gClimbSpeed", 3),
    // Faster Heavy Block Lift
    PRESET_ENTRY_S32("gFasterHeavyBlockLift", 1),
    // No Forced Navi
    PRESET_ENTRY_S32("gNoForcedNavi", 1),
    // No Skulltula Freeze
    PRESET_ENTRY_S32("gSkulltulaFreeze", 1),
    // MM Bunny Hood
    PRESET_ENTRY_S32("gMMBunnyHood", BUNNY_HOOD_FAST_AND_JUMP),
    // Adult Bunny Hood
    PRESET_ENTRY_S32("gAdultBunnyHood", 1),
    // Fast Chests
    PRESET_ENTRY_S32("gFastChests", 1),
    // Fast Drops
    PRESET_ENTRY_S32("gFastDrops", 1),
    // Fast Ocarina Playback
    PRESET_ENTRY_S32("gFastOcarinaPlayback", 1),
    // Instant Putaway
    PRESET_ENTRY_S32("gInstantPutaway", 1),
    // Instant Boomerang Recall
    PRESET_ENTRY_S32("gFastBoomerang", 1),
    // Nuts Explode Bombs
    PRESET_ENTRY_S32("gNutsExplodeBombs", 1),
    // Ask to Equip New Items
    PRESET_ENTRY_S32("gAskToEquip", 1),
    // Mask Select in Inventory
    PRESET_ENTRY_S32("gMaskSelect", 1),
    // Always Win Goron Pot
    PRESET_ENTRY_S32("gGoronPot", 1),
    // Always Win Dampe Digging
    PRESET_ENTRY_S32("gDampeWin", 1),
    // Skip Magic Arrow Equip Animation
    PRESET_ENTRY_S32("gSkipArrowAnimation", 1),
    // Exit Market at Night
    PRESET_ENTRY_S32("gMarketSneak", 1),

    // Equip arrows on multiple slots
    PRESET_ENTRY_S32("gSeparateArrows", 1),

    // Disable Navi Call Audio
    PRESET_ENTRY_S32("gDisableNaviCallAudio", 1),

    // Equipment Toggle
    PRESET_ENTRY_S32("gEquipmentCanBeRemoved", 1),
    // Link's Cow in Both Time Periods
    PRESET_ENTRY_S32("gCowOfTime", 1),

    // Enable 3D Dropped items/projectiles
    PRESET_ENTRY_S32("gNewDrops", 1),

    // Fix Anubis fireballs
    PRESET_ENTRY_S32("gAnubisFix", 1),

    // Autosave
    PRESET_ENTRY_S32("gAutosave", AUTOSAVE_LOCATION_AND_MAJOR_ITEMS),

    // Customize Fishing Behaviour
    PRESET_ENTRY_S32("gCustomizeFishing", 1),
    // Guarantee Bite
    PRESET_ENTRY_S32("gGuaranteeFishingBite", 1),
    // Fish Never Escape
    PRESET_ENTRY_S32("gFishNeverEscape", 1),
    // Child Minimum Weight (6 to 10)
    PRESET_ENTRY_S32("gChildMinimumWeightFish", 3),
    // Adult Minimum Weight (8 to 13)
    PRESET_ENTRY_S32("gAdultMinimumWeightFish", 6),

    // Visual Stone of Agony
    PRESET_ENTRY_S32("gVisualAgony", 1),
    // Pull grave during the day
    PRESET_ENTRY_S32("gDayGravePull", 1),
    // Pull out Ocarina to Summon Scarecrow
    PRESET_ENTRY_S32("gSkipScarecrow", 1),
    // Chest size & texture matches contents
    PRESET_ENTRY_S32("gChestSizeAndTextureMatchesContents", CSMC_BOTH),

    // Pause link animation (0 to 16)
    PRESET_ENTRY_S32("gPauseLiveLink", 16),
    // Frames to wait
    PRESET_ENTRY_S32("gMinFrameCount", 200),

    PRESET_ENTRY_S32("gNoInputForCredits", 1),

    PRESET_ENTRY_S32("gFastFarores", 1),
};

const std::vector<PresetEntry> spockRacePresetEntries = {
    PRESET_ENTRY_S32("gRandomizeLogicRules", 0),
    PRESET_ENTRY_S32("gGameplayStats.RTATiming", 1),
    PRESET_ENTRY_S32("gTextSpeed", 5),
    PRESET_ENTRY_S32("gMweepSpeed", 5),
    PRESET_ENTRY_S32("gClimbSpeed", 4),
    PRESET_ENTRY_S32("gFasterBlockPush", 5),
    PRESET_ENTRY_S32("gFasterHeavyBlockLift", 1),
    PRESET_ENTRY_S32("gNoForcedNavi", 1),
    PRESET_ENTRY_S32("gDisableLOD", 1),
    PRESET_ENTRY_S32("gDisableNaviCallAudio", 1),
    PRESET_ENTRY_S32("gFastChests", 1),
    PRESET_ENTRY_S32("gFastDrops", 1),
    PRESET_ENTRY_S32("gDpadText", 1),
    PRESET_ENTRY_S32("gDpadOcarina", 1),
    PRESET_ENTRY_S32("gRStickOcarina", 1),
    PRESET_ENTRY_S32("gDpadEquips", 1),
    PRESET_ENTRY_S32("gFastOcarinaPlayback", 1),
    PRESET_ENTRY_S32("gSkipScarecrow", 1),
    PRESET_ENTRY_S32("gMarketSneak", 1),
    PRESET_ENTRY_S32("gInstantPutaway", 1),
    PRESET_ENTRY_S32("gFastBoomerang", 1),
    PRESET_ENTRY_S32("gDpadNoDropOcarinaInput", 1),
    PRESET_ENTRY_S32("gAdultBunnyHood", 1),
    PRESET_ENTRY_S32("gMMBunnyHood", BUNNY_HOOD_FAST),
    PRESET_ENTRY_S32("gSeparateArrows", 1),
    PRESET_ENTRY_S32("gAssignableTunicsAndBoots", 1),
    PRESET_ENTRY_S32("gEquipmentCanBeRemoved", 1),
    PRESET_ENTRY_S32("gDayGravePull", 1),
    PRESET_ENTRY_S32("gDrawLineupTick", 1),
    PRESET_ENTRY_S32("gNewDrops", 1),
    PRESET_ENTRY_S32("gCreditsFix", 1),
    PRESET_ENTRY_S32("gSkipText", 1),
    PRESET_ENTRY_S32("gSkulltulaFreeze", 1),
    PRESET_ENTRY_S32("gPauseAnyCursor", 1),
    PRESET_ENTRY_S32("gHideBuildInfo", 1),
    PRESET_ENTRY_S32("gSkipLogoTitle", 1),
    PRESET_ENTRY_S32("gSaveFileID", 4),
    PRESET_ENTRY_S32("gFastFarores", 1),
    PRESET_ENTRY_S32("gNoInputForCredits", 1),
    PRESET_ENTRY_S32("gMaskSelect", 1),
    PRESET_ENTRY_S32("gInjectItemCounts", 1),
    PRESET_ENTRY_S32("gCustomizeFishing", 1),
    PRESET_ENTRY_S32("gFishNeverEscape", 1),
    PRESET_ENTRY_S32("gInstantFishing", 1),
    PRESET_ENTRY_S32("gGuaranteeFishingBite", 1),
    PRESET_ENTRY_S32("gAdultMinimumWeightFish", 6),
    PRESET_ENTRY_S32("gChildMinimumWeightFish", 3),
    PRESET_ENTRY_S32("gGoronPot", 1),
    PRESET_ENTRY_S32("gForgeTime", 0),
    PRESET_ENTRY_S32("gDisableFirstPersonChus", 1),
    PRESET_ENTRY_S32("gCheatEasyPauseBufferEnabled", 1),
    PRESET_ENTRY_S32("gDampeAllNight", 1),
    PRESET_ENTRY_S32("gRandomize10GSHint", 1),
    PRESET_ENTRY_S32("gRandomize20GSHint", 1),
    PRESET_ENTRY_S32("gRandomize30GSHint", 1),
    PRESET_ENTRY_S32("gRandomize40GSHint", 1),
    PRESET_ENTRY_S32("gRandomize50GSHint", 1),
    PRESET_ENTRY_S32("gRandomizeAllLocationsReachable", 0),
    PRESET_ENTRY_S32("gRandomizeBlueFireArrows", 1),
    PRESET_ENTRY_S32("gRandomizeBombchusInLogic", 1),
    PRESET_ENTRY_S32("gRandomizeCompleteMaskQuest", 1),
    PRESET_ENTRY_S32("gRandomizeCuccosToReturn", 1),
    PRESET_ENTRY_S32("gRandomizeDampeHint", 1),
    PRESET_ENTRY_S32("gRandomizeDoorOfTime", RO_DOOROFTIME_OPEN),
    PRESET_ENTRY_S32("gRandomizeEnableBombchuDrops", 1),
    PRESET_ENTRY_STRING("gRandomizeExcludedLocations", "78,143,144,229,"),
    PRESET_ENTRY_S32("gRandomizeForest", RO_FOREST_OPEN),
    PRESET_ENTRY_S32("gRandomizeFullWallets", 1),
    PRESET_ENTRY_S32("gRandomizeGanonTrial", RO_GANONS_TRIALS_SKIP),
    PRESET_ENTRY_S32("gRandomizeGerudoFortress", RO_GF_FAST),
    PRESET_ENTRY_S32("gRandomizeGregHint", 1),
    PRESET_ENTRY_S32("gRandomizeGsExpectSunsSong", 1),
    PRESET_ENTRY_S32("gRandomizeKakarikoGate", RO_KAK_GATE_OPEN),
    PRESET_ENTRY_S32("gRandomizeLacsRewardCount", 5),
    PRESET_ENTRY_S32("gRandomizeRainbowBridge", RO_BRIDGE_GREG),
    PRESET_ENTRY_S32("gRandomizeScrubText", 1),
    PRESET_ENTRY_S32("gRandomizeShopsanity", RO_SHOPSANITY_RANDOM),
    PRESET_ENTRY_S32("gRandomizeShuffleGanonBossKey", RO_GANON_BOSS_KEY_LACS_REWARDS),
    PRESET_ENTRY_S32("gRandomizeShuffleKeyRings", RO_KEYRINGS_COUNT),
    PRESET_ENTRY_S32("gRandomizeShuffleKokiriSword", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleOcarinas", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleScrubs", RO_SCRUBS_AFFORDABLE),
    PRESET_ENTRY_S32("gRandomizeSkipChildStealth", 1),
    PRESET_ENTRY_S32("gRandomizeSkipChildZelda", 1),
    PRESET_ENTRY_S32("gRandomizeSkipEponaRace", 1),
    PRESET_ENTRY_S32("gRandomizeSkipScarecrowsSong", 1),
    PRESET_ENTRY_S32("gRandomizeSkipTowerEscape", 1),
    PRESET_ENTRY_S32("gRandomizeStartingConsumables", 1),
    PRESET_ENTRY_S32("gRandomizeStartingMapsCompasses", RO_DUNGEON_ITEM_LOC_STARTWITH),
    PRESET_ENTRY_S32("gRandomizeStartingOcarina", 1),
    PRESET_ENTRY_S32("gRandomizeSunlightArrows", 1),
    PRESET_ENTRY_S32("gRandomizerSettingsEnabled", 1),
};

const std::vector<PresetEntry> spockRaceNoLogicPresetEntries = {
    PRESET_ENTRY_S32("gAdultBunnyHood", 1),
    PRESET_ENTRY_S32("gAdultMinimumWeightFish", 6),
    PRESET_ENTRY_S32("gAssignableTunicsAndBoots", 1),
    PRESET_ENTRY_S32("gCheatEasyPauseBufferEnabled", 1),
    PRESET_ENTRY_S32("gChildMinimumWeightFish", 3),
    PRESET_ENTRY_S32("gClimbSpeed", 4),
    PRESET_ENTRY_S32("gCosmetics.Goron_NeckLength", 1000),
    PRESET_ENTRY_S32("gCreditsFix", 1),
    PRESET_ENTRY_S32("gCustomizeFishing", 1),
    PRESET_ENTRY_S32("gDampeAllNight", 1),
    PRESET_ENTRY_S32("gDayGravePull", 1),
    PRESET_ENTRY_S32("gDisableFirstPersonChus", 1),
    PRESET_ENTRY_S32("gDisableLOD", 1),
    PRESET_ENTRY_S32("gDisableNaviCallAudio", 1),
    PRESET_ENTRY_S32("gDpadEquips", 1),
    PRESET_ENTRY_S32("gDpadNoDropOcarinaInput", 1),
    PRESET_ENTRY_S32("gDpadOcarina", 1),
    PRESET_ENTRY_S32("gDpadText", 1),
    PRESET_ENTRY_S32("gDrawLineupTick", 1),
    PRESET_ENTRY_S32("gEquipmentCanBeRemoved", 1),
    PRESET_ENTRY_S32("gFastBoomerang", 1),
    PRESET_ENTRY_S32("gFastChests", 1),
    PRESET_ENTRY_S32("gFastDrops", 1),
    PRESET_ENTRY_S32("gFastFarores", 1),
    PRESET_ENTRY_S32("gFastOcarinaPlayback", 1),
    PRESET_ENTRY_S32("gFasterBlockPush", 5),
    PRESET_ENTRY_S32("gFasterHeavyBlockLift", 1),
    PRESET_ENTRY_S32("gFishNeverEscape", 1),
    PRESET_ENTRY_S32("gForgeTime", 0),
    PRESET_ENTRY_S32("gGameplayStats.RTATiming", 1),
    PRESET_ENTRY_S32("gGameplayStats.ShowIngameTimer", 1),
    PRESET_ENTRY_S32("gGoronPot", 1),
    PRESET_ENTRY_S32("gGuaranteeFishingBite", 1),
    PRESET_ENTRY_S32("gHideBuildInfo", 1),
    PRESET_ENTRY_S32("gInjectItemCounts", 1),
    PRESET_ENTRY_S32("gInstantFishing", 1),
    PRESET_ENTRY_S32("gInstantPutaway", 1),
    PRESET_ENTRY_S32("gMMBunnyHood", BUNNY_HOOD_FAST),
    PRESET_ENTRY_S32("gMarketSneak", 1),
    PRESET_ENTRY_S32("gMaskSelect", 1),
    PRESET_ENTRY_S32("gMweepSpeed", 5),
    PRESET_ENTRY_S32("gNewDrops", 1),
    PRESET_ENTRY_S32("gNoForcedNavi", 1),
    PRESET_ENTRY_S32("gNoInputForCredits", 1),
    PRESET_ENTRY_S32("gOnFileSelectNameEntry", 0),
    PRESET_ENTRY_S32("gOpenMenuBar", 1),
    PRESET_ENTRY_S32("gPauseAnyCursor", 1),
    PRESET_ENTRY_S32("gRStickOcarina", 1),
    PRESET_ENTRY_S32("gRandomize10GSHint", 1),
    PRESET_ENTRY_S32("gRandomize20GSHint", 1),
    PRESET_ENTRY_S32("gRandomize30GSHint", 1),
    PRESET_ENTRY_S32("gRandomize40GSHint", 1),
    PRESET_ENTRY_S32("gRandomize50GSHint", 1),
    PRESET_ENTRY_S32("gRandomizeAllLocationsReachable", 0),
    PRESET_ENTRY_S32("gRandomizeBlueFireArrows", 1),
    PRESET_ENTRY_S32("gRandomizeBombchusInLogic", 1),
    PRESET_ENTRY_S32("gRandomizeBossKeysanity", RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32("gRandomizeCompleteMaskQuest", 1),
    PRESET_ENTRY_S32("gRandomizeCuccosToReturn", 1),
    PRESET_ENTRY_S32("gRandomizeDampeHint", 1),
    PRESET_ENTRY_S32("gRandomizeDoorOfTime", RO_DOOROFTIME_OPEN),
    PRESET_ENTRY_S32("gRandomizeEnableBombchuDrops", 1),
    PRESET_ENTRY_STRING("gRandomizeExcludedLocations", "78,143,144,229,"),
    PRESET_ENTRY_S32("gRandomizeForest", RO_FOREST_OPEN),
    PRESET_ENTRY_S32("gRandomizeFullWallets", 1),
    PRESET_ENTRY_S32("gRandomizeGanonTrial", RO_GANONS_TRIALS_SKIP),
    PRESET_ENTRY_S32("gRandomizeGerudoFortress", RO_GF_FAST),
    PRESET_ENTRY_S32("gRandomizeGerudoKeys", RO_GERUDO_KEYS_ANYWHERE),
    PRESET_ENTRY_S32("gRandomizeGregHint", 1),
    PRESET_ENTRY_S32("gRandomizeGsExpectSunsSong", 0),
    PRESET_ENTRY_S32("gRandomizeKakarikoGate", RO_KAK_GATE_OPEN),
    PRESET_ENTRY_S32("gRandomizeKeysanity", RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32("gRandomizeLacsRewardCount", 5),
    PRESET_ENTRY_S32("gRandomizeLogicRules", RO_LOGIC_NO_LOGIC),
    PRESET_ENTRY_S32("gRandomizeRainbowBridge", RO_BRIDGE_GREG),
    PRESET_ENTRY_S32("gRandomizeScrubText", 1),
    PRESET_ENTRY_S32("gRandomizeShopsanity", RO_SHOPSANITY_RANDOM),
    PRESET_ENTRY_S32("gRandomizeShuffleAdultTrade", 0),
    PRESET_ENTRY_S32("gRandomizeShuffleBeans", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleGanonBossKey", RO_GANON_BOSS_KEY_LACS_REWARDS),
    PRESET_ENTRY_S32("gRandomizeShuffleGerudoToken", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleKeyRings", RO_KEYRINGS_COUNT),
    PRESET_ENTRY_S32("gRandomizeShuffleKokiriSword", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleOcarinas", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleScrubs", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleSongs", RO_SONG_SHUFFLE_ANYWHERE),
    PRESET_ENTRY_S32("gRandomizeShuffleTokens", RO_TOKENSANITY_ALL),
    PRESET_ENTRY_S32("gRandomizeSkipChildStealth", 1),
    PRESET_ENTRY_S32("gRandomizeSkipChildZelda", 1),
    PRESET_ENTRY_S32("gRandomizeSkipEponaRace", 1),
    PRESET_ENTRY_S32("gRandomizeSkipScarecrowsSong", 1),
    PRESET_ENTRY_S32("gRandomizeSkipTowerEscape", 1),
    PRESET_ENTRY_S32("gRandomizeStartingConsumables", 1),
    PRESET_ENTRY_S32("gRandomizeStartingMapsCompasses", RO_DUNGEON_ITEM_LOC_STARTWITH),
    PRESET_ENTRY_S32("gRandomizeStartingOcarina", 1),
    PRESET_ENTRY_S32("gRandomizeSunlightArrows", 1),
    PRESET_ENTRY_S32("gRandomizerSettingsEnabled", 1),
    PRESET_ENTRY_S32("gSaveFileID", 4),
    PRESET_ENTRY_S32("gSeparateArrows", 1),
    PRESET_ENTRY_S32("gSkipLogoTitle", 1),
    PRESET_ENTRY_S32("gSkipScarecrow", 1),
    PRESET_ENTRY_S32("gSkipText", 1),
    PRESET_ENTRY_S32("gSkulltulaFreeze", 1),
    PRESET_ENTRY_S32("gTextSpeed", 5),
    PRESET_ENTRY_S32("gZFightingMode", 0),
};

const std::vector<PresetEntry> s6PresetEntries = {
    PRESET_ENTRY_S32("gChestSizeAndTextureMatchesContents", CSMC_BOTH),
    PRESET_ENTRY_S32("gFastChests", 1),
    PRESET_ENTRY_S32("gMMBunnyHood", BUNNY_HOOD_FAST),
    PRESET_ENTRY_S32("gAdultBunnyHood", 1),
    PRESET_ENTRY_S32("gRandomizeBigPoeTargetCount", 1),
    PRESET_ENTRY_S32("gRandomizeCuccosToReturn", 4),
    PRESET_ENTRY_S32("gRandomizeDoorOfTime", RO_DOOROFTIME_OPEN),
    PRESET_ENTRY_STRING("gRandomizeExcludedLocations", "48,"),
    PRESET_ENTRY_S32("gRandomizeForest", RO_FOREST_CLOSED_DEKU),
    PRESET_ENTRY_S32("gRandomizeGanonTrial", RO_GANONS_TRIALS_SKIP),
    PRESET_ENTRY_S32("gRandomizeGerudoFortress", RO_GF_FAST),
    PRESET_ENTRY_S32("gRandomizeIceTraps", RO_ICE_TRAPS_OFF),
    PRESET_ENTRY_S32("gRandomizeKakarikoGate", RO_KAK_GATE_OPEN),
    PRESET_ENTRY_S32("gRandomizeMedallionCount", 6),
    PRESET_ENTRY_S32("gRandomizeMqDungeons", RO_MQ_DUNGEONS_NONE),
    PRESET_ENTRY_S32("gRandomizeRainbowBridge", RO_BRIDGE_MEDALLIONS),
    PRESET_ENTRY_S32("gRandomizeShuffleAdultTrade", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleDungeonReward", RO_DUNGEON_REWARDS_END_OF_DUNGEON),
    PRESET_ENTRY_S32("gRandomizeShuffleGanonBossKey", RO_GANON_BOSS_KEY_STARTWITH),
    PRESET_ENTRY_S32("gRandomizeShuffleKokiriSword", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleOverworldSpawns", RO_GENERIC_ON),
    PRESET_ENTRY_S32("gRandomizeSkipChildStealth", 1),
    PRESET_ENTRY_S32("gRandomizeSkipChildZelda", 1),
    PRESET_ENTRY_S32("gRandomizeSkipEponaRace", 1),
    PRESET_ENTRY_S32("gRandomizeSkipTowerEscape", 1),
    PRESET_ENTRY_S32("gRandomizeStartingAge", RO_AGE_RANDOM),
    PRESET_ENTRY_S32("gRandomizeStartingConsumables", 1),
    PRESET_ENTRY_S32("gRandomizeStartingDekuShield", 1),
    PRESET_ENTRY_S32("gRandomizeStartingMapsCompasses", RO_DUNGEON_ITEM_LOC_STARTWITH),
    PRESET_ENTRY_S32("gRandomizeStartingOcarina", 1),
    PRESET_ENTRY_S32("gRandomizeZorasFountain", 0),
};

const std::vector<PresetEntry> hellModePresetEntries = {
    PRESET_ENTRY_S32("gChestSizeAndTextureMatchesContents", CSMC_BOTH),
    PRESET_ENTRY_S32("gFastChests", 1),
    PRESET_ENTRY_S32("gMMBunnyHood", BUNNY_HOOD_FAST),
    PRESET_ENTRY_S32("gAdultBunnyHood", 1),
    PRESET_ENTRY_S32("gRandomizeBigPoeTargetCount", 1),
    PRESET_ENTRY_S32("gRandomizeBlueFireArrows", 1),
    PRESET_ENTRY_S32("gRandomizeBossKeysanity", RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32("gRandomizeCompleteMaskQuest", 1),
    PRESET_ENTRY_S32("gRandomizeCuccosToReturn", 1),
    PRESET_ENTRY_S32("gRandomizeDoorOfTime", RO_DOOROFTIME_OPEN),
    PRESET_ENTRY_S32("gRandomizeEnableBombchuDrops", 1),
    PRESET_ENTRY_S32("gRandomizeEnableGlitchCutscenes", 1),
    PRESET_ENTRY_S32("gRandomizeForest", RO_FOREST_OPEN),
    PRESET_ENTRY_S32("gRandomizeGanonTrial", RO_GANONS_TRIALS_SET_NUMBER),
    PRESET_ENTRY_S32("gRandomizeGanonTrialCount", 6),
    PRESET_ENTRY_S32("gRandomizeGerudoKeys", RO_GERUDO_KEYS_ANYWHERE),
    PRESET_ENTRY_S32("gRandomizeGsExpectSunsSong", 1),
    PRESET_ENTRY_S32("gRandomizeIceTraps", RO_ICE_TRAPS_ONSLAUGHT),
    PRESET_ENTRY_S32("gRandomizeItemPool", RO_ITEM_POOL_MINIMAL),
    PRESET_ENTRY_S32("gRandomizeKakarikoGate", RO_KAK_GATE_OPEN),
    PRESET_ENTRY_S32("gRandomizeKeysanity", RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32("gRandomizeLinksPocket", RO_LINKS_POCKET_NOTHING),
    PRESET_ENTRY_S32("gRandomizeMqDungeons", RO_MQ_DUNGEONS_RANDOM_NUMBER),
    PRESET_ENTRY_S32("gRandomizeRainbowBridge", RO_BRIDGE_DUNGEON_REWARDS),
    PRESET_ENTRY_S32("gRandomizeShopsanity", RO_SHOPSANITY_FOUR_ITEMS),
    PRESET_ENTRY_S32("gRandomizeShopsanityPrices", RO_SHOPSANITY_PRICE_TYCOON),
    PRESET_ENTRY_S32("gRandomizeShuffleAdultTrade", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleBeans", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleCows", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleDungeonReward", RO_DUNGEON_REWARDS_ANYWHERE),
    PRESET_ENTRY_S32("gRandomizeShuffleFrogSongRupees", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleGanonBossKey", RO_GANON_BOSS_KEY_LACS_DUNGEONS),
    PRESET_ENTRY_S32("gRandomizeShuffleGerudoToken", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleKeyRings", RO_KEYRINGS_RANDOM),
    PRESET_ENTRY_S32("gRandomizeShuffleKokiriSword", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleMasterSword", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleOcarinas", 1),
    PRESET_ENTRY_S32("gRandomizeShuffleScrubs", RO_SCRUBS_RANDOM),
    PRESET_ENTRY_S32("gRandomizeShuffleSongs", RO_SONG_SHUFFLE_ANYWHERE),
    PRESET_ENTRY_S32("gRandomizeShuffleTokens", RO_TOKENSANITY_ALL),
    PRESET_ENTRY_S32("gRandomizeShuffleWeirdEgg", 1),
    PRESET_ENTRY_S32("gRandomizeSkipChildStealth", 1),
    PRESET_ENTRY_S32("gRandomizeSkipEponaRace", 1),
    PRESET_ENTRY_S32("gRandomizeSkipScarecrowsSong", 1),
    PRESET_ENTRY_S32("gRandomizeSkipTowerEscape", 1),
    PRESET_ENTRY_S32("gRandomizeStartingAge", RO_AGE_RANDOM),
    PRESET_ENTRY_S32("gRandomizeStartingMapsCompasses", RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32("gRandomizeSunlightArrows", 1),
    PRESET_ENTRY_S32("gRandomizeZorasFountain", 2),
};

typedef struct PresetDefinition {
    const char* label;
    const char* description;
    std::vector<PresetEntry> entries;
} PresetDefinition;

typedef struct PresetTypeDefinition {
    std::vector<const char*> cvarsToClear;
    std::map<uint16_t, PresetDefinition> presets;
} PresetTypeDefinition;

const std::map<PresetType, PresetTypeDefinition> presetTypes = {
    { PRESET_TYPE_ENHANCEMENTS, { enhancementsCvars, {
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
    { PRESET_TYPE_RANDOMIZER, { randomizerCvars, {
        { RANDOMIZER_PRESET_DEFAULT, {
            "Default",
            "Reset all options to their default values.",
            {},
        } },
        { RANDOMIZER_PRESET_SPOCK_RACE, {
            "Spock Race",
            "Race preset used for the official Ship of Harkinian race on June 3rd 2023. The following settings are notable:\n" \
            "- Rainbow Bridge is set to Greg\n" \
            "- Ganons Boss Key is 5 dungeon rewards\n" \
            "- Shopsanity and Scrubsanity enabled\n" \
            "- All locations reachable is off\n", \
            spockRacePresetEntries,
        } },
        { RANDOMIZER_PRESET_SPOCK_RACE_NO_LOGIC, {
                "Spock Race - No Logic",
                "No Logic Race preset used for official Ship of Harkinian No Logic races. The following settings are "
                "notable:\n"
                "- Rainbow Bridge is set to Greg\n"
                "- Ganons Boss Key is 5 dungeon rewards\n"
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
    } } }
};
