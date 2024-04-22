#pragma once

#include <map>
#include <string>
#include <vector>
#include <variant>
#include <cstdint>
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/OTRGlobals.h"

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
    "gCrawlSpeed",
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
    "gQuitFishingAtDoor",
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
    "gCustomizeOcarinaGame",
    "gInstantOcarinaGameWin",
    "gOcarinaGameNoteSpeed",
    "gOcarinaUnlimitedFailTime",
    "gOcarinaGameStartingNotes",
    "gOcarinaGameRoundOneNotes",
    "gOcarinaGameRoundTwoNotes",
    "gOcarinaGameRoundThreeNotes",
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
    "gTreeStickDrops",
    "gShadowTag",
    "gRandomizedEnemySizes",
    "gRandomizedEnemies",
    "gMirroredWorldMode",
    "gMirroredWorld",
    "gHyperEnemies",
    "gHookshotableReticle",
    "gHideBunnyHood",
    "gFixVineFall",
    "gFileSelectMoreInfo",
    "gEnemyHealthBar",
    "gBushDropFix",
    "gAllDogsRichard",
    "gAddTraps.enabled",
    "gAddTraps.Ammo",
    "gAddTraps.Bomb",
    "gAddTraps.Burn",
    "gAddTraps.Ice",
    "gAddTraps.Kill",
    "gAddTraps.Knock",
    "gAddTraps.Shock",
    "gAddTraps.Speed",
    "gAddTraps.Tele",
    "gAddTraps.Void",
    "gToTMedallionsColors", 
    "gCuccoStayDurationMultiplier",
    "gDeleteFileOnDeath",
    "gEnemySizeScalesHealth",
    "gEnhancements.BetterAmmoRendering",
    "gEnhancements.EquimentAlwaysVisible",
    "gEnhancements.FixDaruniaDanceSpeed",
    "gEnhancements.OpenAllHours",
    "gEnhancements.ResetNaviTimer",
    "gEnhancements.ScaleAdultEquimentAsChild",
    "gEnhancements.LeeverSpawnRate",
    "gEnhancements.SwordToggle",
    "gEnhancements.FixFloorSwitches", 
    "gFixZoraHintDialogue",
    "gHurtContainer",
    "gPauseWarp",
    "gPermanentHeartLoss",
    "gRemoveExplosiveLimit",
    "gToggleStrength",
};

const std::vector<const char*> cheatCvars = {
    "gConsoleEnabled",
    CVAR_WINDOW("ActorViewer"),
    CVAR_WINDOW("CollisionViewer"),
    CVAR_WINDOW("DLViewer"),
    CVAR_WINDOW("SaveEditor"),
    "gEnableWalkModify",
    "gWalkSpeedToggle",
    "gWalkModifierOne",
    "gWalkModifierTwo",
    "gSwimModifierOne",
    "gSwimModifierTwo",
    "gGoronPot",
    "gDampeWin",
    "gCustomizeShootingGallery",
    "gCustomizeBombchuBowling",
    "gCustomizeFishing",
    CVAR_CHEAT("InfiniteAmmo"),
    CVAR_CHEAT("InfiniteEponaBoost"),
    CVAR_CHEAT("InfiniteHealth"),
    CVAR_CHEAT("InfiniteMagic"),
    CVAR_CHEAT("InfiniteMoney"),
    CVAR_CHEAT("InfiniteNayru"),
    CVAR_CHEAT("NoClip"),
    CVAR_CHEAT("ClimbEverything"),
    CVAR_CHEAT("HookshotEverything"),
    CVAR_CHEAT("HookshotReachMultiplier"),
    CVAR_CHEAT("MoonJumpOnL"),
    CVAR_CHEAT("SuperTunic"),
    CVAR_CHEAT("EasyISG"),
    CVAR_CHEAT("EasyQPA"),
    CVAR_CHEAT("TimelessEquipment"),
    CVAR_CHEAT("EasyPauseBuffer"),
    CVAR_CHEAT("EasyInputBuffer"),
    CVAR_CHEAT("NoRestrictItems"),
    CVAR_CHEAT("FreezeTime"),
    "gPrevTime",
    CVAR_CHEAT("DropsDontDie"),
    CVAR_CHEAT("FireproofDekuShield"),
    CVAR_CHEAT("ShieldTwoHanded"),
    CVAR_CHEAT("TimeSync"),
    CVAR_DEVELOPER_TOOLS("DebugEnabled"),
    CVAR_DEVELOPER_TOOLS("SkulltulaDebugEnabled"),
    CVAR_DEVELOPER_TOOLS("SkipLogoTitle"),
    CVAR_DEVELOPER_TOOLS("SaveFileID"),
    CVAR_CHEAT("EnableBetaQuest"),
    CVAR_DEVELOPER_TOOLS("BetterDebugWarpScreen"),
    CVAR_GENERAL("SwitchAge"),
    "gSwitchTimeline",
    CVAR_CHEAT("NoRedeadFreeze"),
    CVAR_CHEAT("NoKeeseGuayTarget"),
    CVAR_CHEAT("BombTimerMultiplier"),
    CVAR_CHEAT("NoFishDespawn"),
    CVAR_CHEAT("NoBugsDespawn"),
    "gWalkModifierDoesntChangeJump",
    "gStatsEnabled",
    CVAR_CHEAT("SaveStatesEnabled"),
    CVAR_CHEAT("SaveStatePromise"),
    CVAR_DEVELOPER_TOOLS("RegEditEnabled"),
    "gPreset0",
    "gPreset1",
    CVAR_CHEAT("DekuStick"),
    CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"),
    CVAR_DEVELOPER_TOOLS("DebugSaveFileMode"),
    CVAR_COSMETIC("Link.BodyScale.Changed"),
    CVAR_COSMETIC("Link.BodyScale.Value"),
    CVAR_COSMETIC("Link.HeadScale.Changed"),
    CVAR_COSMETIC("Link.HeadScale.Value"),
    CVAR_COSMETIC("Link.SwordScale.Changed"),
    CVAR_COSMETIC("Link.SwordScale.Value"),
    "gEnhancements.RememberMapToggleState",
};

const std::vector<const char*> randomizerCvars = {
    CVAR_RANDOMIZER_SETTING("10GSHint"),
    CVAR_RANDOMIZER_SETTING("20GSHint"),
    CVAR_RANDOMIZER_SETTING("30GSHint"),
    CVAR_RANDOMIZER_SETTING("40GSHint"),
    CVAR_RANDOMIZER_SETTING("50GSHint"),
    CVAR_RANDOMIZER_SETTING("AllLocationsReachable"),
    CVAR_RANDOMIZER_SETTING("AltarHint"),
    CVAR_RANDOMIZER_SETTING("BigPoeTargetCount"),
    CVAR_RANDOMIZER_SETTING("BlueFireArrows"),
    CVAR_RANDOMIZER_SETTING("BombchusInLogic"),
    CVAR_RANDOMIZER_SETTING("BossKeysanity"),
    CVAR_RANDOMIZER_SETTING("BridgeRewardOptions"),
    CVAR_RANDOMIZER_SETTING("CompleteMaskQuest"),
    CVAR_RANDOMIZER_SETTING("CuccosToReturn"),
    CVAR_RANDOMIZER_SETTING("DampeHint"),
    CVAR_RANDOMIZER_SETTING("DecoupleEntrances"),
    CVAR_RANDOMIZER_SETTING("DoorOfTime"),
    CVAR_RANDOMIZER_SETTING("DungeonCount"),
    CVAR_RANDOMIZER_SETTING("EnableBombchuDrops"),
    CVAR_RANDOMIZER_SETTING("EnableGlitchCutscenes"),
    CVAR_RANDOMIZER_SETTING("EnabledGlitches"),
    CVAR_RANDOMIZER_SETTING("EnabledTricks"),
    CVAR_RANDOMIZER_SETTING("ExcludedLocations"),
    CVAR_RANDOMIZER_SETTING("Forest"),
    CVAR_RANDOMIZER_SETTING("FullWallets"),
    CVAR_RANDOMIZER_SETTING("GanonTrial"),
    CVAR_RANDOMIZER_SETTING("GanonTrialCount"),
    CVAR_RANDOMIZER_SETTING("GerudoFortress"),
    CVAR_RANDOMIZER_SETTING("GerudoKeys"),
    CVAR_RANDOMIZER_SETTING("GossipStoneHints"),
    CVAR_RANDOMIZER_SETTING("GsExpectSunsSong"),
    CVAR_RANDOMIZER_SETTING("HintClarity"),
    CVAR_RANDOMIZER_SETTING("HintDistribution"),
    CVAR_RANDOMIZER_SETTING("IceTraps"),
    CVAR_RANDOMIZER_SETTING("ItemPool"),
    CVAR_RANDOMIZER_SETTING("KakarikoGate"),
    CVAR_RANDOMIZER_SETTING("Keysanity"),
    CVAR_RANDOMIZER_SETTING("LacsDungeonCount"),
    CVAR_RANDOMIZER_SETTING("LacsMedallionCount"),
    CVAR_RANDOMIZER_SETTING("LacsRewardCount"),
    CVAR_RANDOMIZER_SETTING("LacsRewardOptions"),
    CVAR_RANDOMIZER_SETTING("LacsStoneCount"),
    CVAR_RANDOMIZER_SETTING("LacsTokenCount"),
    CVAR_RANDOMIZER_SETTING("LAHint"),
    CVAR_RANDOMIZER_SETTING("LinksPocket"),
    CVAR_RANDOMIZER_SETTING("LogicRules"),
    CVAR_RANDOMIZER_SETTING("MedallionCount"),
    CVAR_RANDOMIZER_SETTING("MixDungeons"),
    CVAR_RANDOMIZER_SETTING("MixedEntrances"),
    CVAR_RANDOMIZER_SETTING("MixGrottos"),
    CVAR_RANDOMIZER_SETTING("MixInteriors"),
    CVAR_RANDOMIZER_SETTING("MixOverworld"),
    CVAR_RANDOMIZER_SETTING("MQDungeonCount"),
    CVAR_RANDOMIZER_SETTING("MQDungeons"),
    CVAR_RANDOMIZER_SETTING("RainbowBridge"),
    CVAR_RANDOMIZER_SETTING("RewardCount"),
    CVAR_RANDOMIZER_SETTING("ScrubText"),
    CVAR_RANDOMIZER_SETTING("Shopsanity"),
    CVAR_RANDOMIZER_SETTING("ShopsanityPrices"),
    CVAR_RANDOMIZER_SETTING("ShopsanityPricesAffordable"),
    CVAR_RANDOMIZER_SETTING("ShuffleAdultTrade"),
    CVAR_RANDOMIZER_SETTING("ShuffleBeans"),
    CVAR_RANDOMIZER_SETTING("ShuffleBossEntrances"),
    CVAR_RANDOMIZER_SETTING("ShuffleCows"),
    CVAR_RANDOMIZER_SETTING("ShuffleDungeonReward"),
    CVAR_RANDOMIZER_SETTING("ShuffleDungeonsEntrances"),
    CVAR_RANDOMIZER_SETTING("ShuffleFrogSongRupees"),
    CVAR_RANDOMIZER_SETTING("ShuffleGanonBossKey"),
    CVAR_RANDOMIZER_SETTING("ShuffleGerudoToken"),
    CVAR_RANDOMIZER_SETTING("ShuffleGrottosEntrances"),
    CVAR_RANDOMIZER_SETTING("ShuffleInteriorsEntrances"),
    CVAR_RANDOMIZER_SETTING("ShuffleKeyRings"),
    CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsBottomOfTheWell"),
    CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsFireTemple"),
    CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsForestTemple"),
    CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsGanonsCastle"),
    CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsGerudoFortress"),
    CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsGTG"),
    CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsRandomCount"),
    CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsShadowTemple"),
    CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsSpiritTemple"),
    CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsWaterTemple"),
    CVAR_RANDOMIZER_SETTING("ShuffleKokiriSword"),
    CVAR_RANDOMIZER_SETTING("ShuffleMerchants"),
    CVAR_RANDOMIZER_SETTING("ShuffleOcarinas"),
    CVAR_RANDOMIZER_SETTING("ShuffleOverworldEntrances"),
    CVAR_RANDOMIZER_SETTING("ShuffleOverworldSpawns"),
    CVAR_RANDOMIZER_SETTING("ShuffleOwlDrops"),
    CVAR_RANDOMIZER_SETTING("ShuffleScrubs"),
    CVAR_RANDOMIZER_SETTING("ShuffleSongs"),
    CVAR_RANDOMIZER_SETTING("ShuffleTokens"),
    CVAR_RANDOMIZER_SETTING("ShuffleWarpSongs"),
    CVAR_RANDOMIZER_SETTING("ShuffleWeirdEgg"),
    CVAR_RANDOMIZER_SETTING("SkipChildStealth"),
    CVAR_RANDOMIZER_SETTING("SkipChildZelda"),
    CVAR_RANDOMIZER_SETTING("SkipEponaRace"),
    CVAR_RANDOMIZER_SETTING("SkipScarecrowsSong"),
    CVAR_RANDOMIZER_SETTING("SkipTowerEscape"),
    CVAR_RANDOMIZER_SETTING("StartingAge"),
    CVAR_RANDOMIZER_SETTING("StartingConsumables"),
    CVAR_RANDOMIZER_SETTING("StartingBoleroOfFire"),
    CVAR_RANDOMIZER_SETTING("StartingDekuShield"),
    CVAR_RANDOMIZER_SETTING("StartingEponasSong"),
    CVAR_RANDOMIZER_SETTING("StartingKokiriSword"),
    CVAR_RANDOMIZER_SETTING("StartingMapsCompasses"),
    CVAR_RANDOMIZER_SETTING("StartingMinuetOfForest"),
    CVAR_RANDOMIZER_SETTING("StartingNocturneOfShadow"),
    CVAR_RANDOMIZER_SETTING("StartingOcarina"),
    CVAR_RANDOMIZER_SETTING("StartingPreludeOfLight"),
    CVAR_RANDOMIZER_SETTING("StartingRequiemOfSpirit"),
    CVAR_RANDOMIZER_SETTING("StartingSariasSong"),
    CVAR_RANDOMIZER_SETTING("StartingSerenadeOfWater"),
    CVAR_RANDOMIZER_SETTING("StartingSkulltulaToken"),
    CVAR_RANDOMIZER_SETTING("StartingSongOfStorms"),
    CVAR_RANDOMIZER_SETTING("StartingSongOfTime"),
    CVAR_RANDOMIZER_SETTING("StartingSunsSong"),
    CVAR_RANDOMIZER_SETTING("StartingZeldasLullaby"),
    CVAR_RANDOMIZER_SETTING("StoneCount"),
    CVAR_RANDOMIZER_SETTING("SunlightArrows"),
    CVAR_RANDOMIZER_SETTING("TokenCount"),
    CVAR_RANDOMIZER_SETTING("WarpSongText"),
    CVAR_RANDOMIZER_SETTING("ZorasFountain"),
    CVAR_RANDOMIZER_SETTING("Shuffle100GSReward"),
    CVAR_RANDOMIZER_SETTING("GregHint"),
    CVAR_RANDOMIZER_SETTING("ManualSeedEntry"),
    CVAR_RANDOMIZER_SETTING("TriforceHuntTotalPieces"),
    CVAR_RANDOMIZER_SETTING("TriforceHuntRequiredPieces"),
    CVAR_RANDOMIZER_SETTING("TriforceHunt"),
    CVAR_RANDOMIZER_SETTING("ShuffleMasterSword"),
    CVAR_RANDOMIZER_SETTING("SariaHint"),
    CVAR_RANDOMIZER_ENHANCEMENT("RandomizeRupeeNames"),
    CVAR_RANDOMIZER_SETTING("FrogsHint"),
    CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"),
    CVAR_RANDOMIZER_ENHANCEMENT("QuestItemFanfares"),
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
    // Fix some Floor Switches
    PRESET_ENTRY_S32("gEnhancements.FixFloorSwitches", 1),

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
    // Fix some Floor Switches
    PRESET_ENTRY_S32("gEnhancements.FixFloorSwitches", 1),

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
    // Fix some Floor Switches
    PRESET_ENTRY_S32("gEnhancements.FixFloorSwitches", 1),

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

    // Customize Lost Woods Ocarina Game Behavior
    PRESET_ENTRY_S32("gCustomizeOcarinaGame", 1),
    // Start With Five Notes
    PRESET_ENTRY_S32("gOcarinaGameStartingNotes", 5),
    // Round One Notes
    PRESET_ENTRY_S32("gOcarinaGameRoundOneNotes", 5),

    // Visual Stone of Agony
    PRESET_ENTRY_S32("gVisualAgony", 1),
    // Pull grave during the day
    PRESET_ENTRY_S32("gDayGravePull", 1),
    // Pull out Ocarina to Summon Scarecrow
    PRESET_ENTRY_S32("gSkipScarecrow", 1),
    // Chest size & texture matches contents
    PRESET_ENTRY_S32("gChestSizeAndTextureMatchesContents", CSMC_BOTH),

    // Color Temple of Time's Medallions
    PRESET_ENTRY_S32("gToTMedallionsColors", 1),

    // Pause link animation (0 to 16)
    PRESET_ENTRY_S32("gPauseLiveLink", 16),
    // Frames to wait
    PRESET_ENTRY_S32("gMinFrameCount", 200),

    PRESET_ENTRY_S32("gNoInputForCredits", 1),

    PRESET_ENTRY_S32("gFastFarores", 1),
};

const std::vector<PresetEntry> spockRacePresetEntries = {
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("LogicRules"), 0),
    PRESET_ENTRY_S32("gTextSpeed", 5),
    PRESET_ENTRY_S32("gMweepSpeed", 5),
    PRESET_ENTRY_S32("gClimbSpeed", 4),
    PRESET_ENTRY_S32("gFasterBlockPush", 5),
    PRESET_ENTRY_S32("gFasterHeavyBlockLift", 1),
    PRESET_ENTRY_S32("gNoForcedNavi", 1),
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
    PRESET_ENTRY_S32("gAdultBunnyHood", 1),
    PRESET_ENTRY_S32("gMMBunnyHood", BUNNY_HOOD_FAST),
    PRESET_ENTRY_S32("gSeparateArrows", 1),
    PRESET_ENTRY_S32("gAssignableTunicsAndBoots", 1),
    PRESET_ENTRY_S32("gEquipmentCanBeRemoved", 1),
    PRESET_ENTRY_S32("gDayGravePull", 1),
    PRESET_ENTRY_S32("gNewDrops", 1),
    PRESET_ENTRY_S32("gCreditsFix", 1),
    PRESET_ENTRY_S32("gSkipText", 1),
    PRESET_ENTRY_S32("gSkulltulaFreeze", 1),
    PRESET_ENTRY_S32("gPauseAnyCursor", 1),
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
    PRESET_ENTRY_S32(CVAR_CHEAT("EasyPauseBuffer"), 1),
    PRESET_ENTRY_S32("gDampeAllNight", 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("10GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("20GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("30GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("40GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("50GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("AllLocationsReachable"), 0),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BlueFireArrows"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BombchusInLogic"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CompleteMaskQuest"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CuccosToReturn"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DampeHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DoorOfTime"), RO_DOOROFTIME_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("EnableBombchuDrops"), 1),
    PRESET_ENTRY_CPP_STRING(CVAR_RANDOMIZER_SETTING("ExcludedLocations"), FormatLocations( 
        { RC_MARKET_10_BIG_POES, RC_KAK_40_GOLD_SKULLTULA_REWARD, RC_KAK_50_GOLD_SKULLTULA_REWARD, RC_ZR_FROGS_OCARINA_GAME })),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Forest"), RO_FOREST_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("FullWallets"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GanonTrial"), RO_GANONS_TRIALS_SKIP),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GerudoFortress"), RO_GF_FAST),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GregHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GsExpectSunsSong"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("KakarikoGate"), RO_KAK_GATE_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("LacsRewardCount"), 5),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("RainbowBridge"), RO_BRIDGE_GREG),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ScrubText"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Shopsanity"), RO_SHOPSANITY_RANDOM),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleGanonBossKey"), RO_GANON_BOSS_KEY_LACS_REWARDS),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleKeyRings"), RO_KEYRINGS_COUNT),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleKokiriSword"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleOcarinas"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleScrubs"), RO_SCRUBS_AFFORDABLE),
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
    PRESET_ENTRY_S32("gAdultBunnyHood", 1),
    PRESET_ENTRY_S32("gAdultMinimumWeightFish", 6),
    PRESET_ENTRY_S32("gAssignableTunicsAndBoots", 1),
    PRESET_ENTRY_S32(CVAR_CHEAT("EasyPauseBuffer"), 1),
    PRESET_ENTRY_S32("gChildMinimumWeightFish", 3),
    PRESET_ENTRY_S32("gClimbSpeed", 4),
    PRESET_ENTRY_S32(CVAR_COSMETIC("Goron.NeckLength"), 1000),
    PRESET_ENTRY_S32("gCreditsFix", 1),
    PRESET_ENTRY_S32("gCustomizeFishing", 1),
    PRESET_ENTRY_S32("gDampeAllNight", 1),
    PRESET_ENTRY_S32("gDayGravePull", 1),
    PRESET_ENTRY_S32("gDisableNaviCallAudio", 1),
    PRESET_ENTRY_S32("gDpadNoDropOcarinaInput", 1),
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
    PRESET_ENTRY_S32("gNoForcedNavi", 1),
    PRESET_ENTRY_S32("gNoInputForCredits", 1),
    PRESET_ENTRY_S32("gPauseAnyCursor", 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("10GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("20GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("30GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("40GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("50GSHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("AllLocationsReachable"), 0),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BlueFireArrows"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BombchusInLogic"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BossKeysanity"), RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CompleteMaskQuest"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CuccosToReturn"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DampeHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DoorOfTime"), RO_DOOROFTIME_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("EnableBombchuDrops"), 1),
    PRESET_ENTRY_CPP_STRING(CVAR_RANDOMIZER_SETTING("ExcludedLocations"), FormatLocations(
        { RC_MARKET_10_BIG_POES, RC_KAK_40_GOLD_SKULLTULA_REWARD, RC_KAK_50_GOLD_SKULLTULA_REWARD, RC_ZR_FROGS_OCARINA_GAME })),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Forest"), RO_FOREST_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("FullWallets"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GanonTrial"), RO_GANONS_TRIALS_SKIP),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GerudoFortress"), RO_GF_FAST),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GerudoKeys"), RO_GERUDO_KEYS_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GregHint"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GsExpectSunsSong"), 0),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("KakarikoGate"), RO_KAK_GATE_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Keysanity"), RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("LacsRewardCount"), 5),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("LogicRules"), RO_LOGIC_NO_LOGIC),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("RainbowBridge"), RO_BRIDGE_GREG),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ScrubText"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Shopsanity"), RO_SHOPSANITY_RANDOM),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleAdultTrade"), 0),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleBeans"), 1),
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
    PRESET_ENTRY_S32("gSeparateArrows", 1),
    PRESET_ENTRY_S32("gSkipScarecrow", 1),
    PRESET_ENTRY_S32("gSkulltulaFreeze", 1),
};

const std::vector<PresetEntry> s6PresetEntries = {
    PRESET_ENTRY_S32("gChestSizeAndTextureMatchesContents", CSMC_BOTH),
    PRESET_ENTRY_S32("gFastChests", 1),
    PRESET_ENTRY_S32("gMMBunnyHood", BUNNY_HOOD_FAST),
    PRESET_ENTRY_S32("gAdultBunnyHood", 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BigPoeTargetCount"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CuccosToReturn"), 4),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DoorOfTime"), RO_DOOROFTIME_OPEN),
    PRESET_ENTRY_CPP_STRING(CVAR_RANDOMIZER_SETTING("ExcludedLocations"), FormatLocations({ RC_DEKU_THEATER_MASK_OF_TRUTH })),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Forest"), RO_FOREST_CLOSED_DEKU),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GanonTrial"), RO_GANONS_TRIALS_SKIP),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("GerudoFortress"), RO_GF_FAST),
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
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingAge"), RO_AGE_RANDOM),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingConsumables"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingDekuShield"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingMapsCompasses"), RO_DUNGEON_ITEM_LOC_STARTWITH),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingOcarina"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ZorasFountain"), 0),
};

const std::vector<PresetEntry> hellModePresetEntries = {
    PRESET_ENTRY_S32("gChestSizeAndTextureMatchesContents", CSMC_BOTH),
    PRESET_ENTRY_S32("gFastChests", 1),
    PRESET_ENTRY_S32("gMMBunnyHood", BUNNY_HOOD_FAST),
    PRESET_ENTRY_S32("gAdultBunnyHood", 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BigPoeTargetCount"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BlueFireArrows"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("BossKeysanity"), RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CompleteMaskQuest"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("CuccosToReturn"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("DoorOfTime"), RO_DOOROFTIME_OPEN),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("EnableBombchuDrops"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("EnableGlitchCutscenes"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Forest"), RO_FOREST_OPEN),
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
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("Shopsanity"), RO_SHOPSANITY_FOUR_ITEMS),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShopsanityPrices"), RO_SHOPSANITY_PRICE_TYCOON),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleAdultTrade"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleBeans"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleCows"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleDungeonReward"), RO_DUNGEON_REWARDS_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleFrogSongRupees"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleGanonBossKey"), RO_GANON_BOSS_KEY_LACS_DUNGEONS),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleGerudoToken"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleKeyRings"), RO_KEYRINGS_RANDOM),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleKokiriSword"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleMasterSword"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleOcarinas"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleScrubs"), RO_SCRUBS_RANDOM),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleSongs"), RO_SONG_SHUFFLE_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleTokens"), RO_TOKENSANITY_ALL),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ShuffleWeirdEgg"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipChildStealth"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipEponaRace"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipScarecrowsSong"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SkipTowerEscape"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingAge"), RO_AGE_RANDOM),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("StartingMapsCompasses"), RO_DUNGEON_ITEM_LOC_ANYWHERE),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("SunlightArrows"), 1),
    PRESET_ENTRY_S32(CVAR_RANDOMIZER_SETTING("ZorasFountain"), 2),
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
