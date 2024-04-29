#ifndef OTR_GLOBALS_H
#define OTR_GLOBALS_H

#pragma once

#include "SaveManager.h"
#include <soh/Enhancements/item-tables/ItemTableTypes.h>

#define GAME_REGION_NTSC 0
#define GAME_REGION_PAL 1

#define GAME_PLATFORM_N64 0
#define GAME_PLATFORM_GC 1

#define BTN_MODIFIER1 0x00040
#define BTN_MODIFIER2 0x00080

#ifdef __cplusplus
#include <Context.h>
#include "Enhancements/savestates.h"
#include "Enhancements/randomizer/randomizer.h"
#include <vector>

const std::string customMessageTableID = "BaseGameOverrides";
const std::string appShortName = "soh";

#ifdef __WIIU__
const uint32_t defaultImGuiScale = 3;
#else
const uint32_t defaultImGuiScale = 1;
#endif

const float imguiScaleOptionToValue[4] = { 0.75f, 1.0f, 1.5f, 2.0f };

class OTRGlobals
{
public:
    static OTRGlobals* Instance;

    std::shared_ptr<Ship::Context> context;
    std::shared_ptr<SaveStateMgr> gSaveStateMgr;
    std::shared_ptr<Randomizer> gRandomizer;

    ImFont* defaultFontSmaller;
    ImFont* defaultFontLarger;
    ImFont* defaultFontLargest;

    OTRGlobals();
    ~OTRGlobals();
    
    void ScaleImGui();

    bool HasMasterQuest();
    bool HasOriginal();
    uint32_t GetInterpolationFPS();
    std::shared_ptr<std::vector<std::string>> ListFiles(std::string path);

private:
	void CheckSaveFile(size_t sramSize) const;
    bool hasMasterQuest;
    bool hasOriginal;
    ImFont* CreateDefaultFontWithSize(float size);
};

uint32_t IsGameMasterQuest();
#endif

#define CVAR_RANDOMIZER_ENHANCEMENT(var) "gRandoEnhancements." var
#define CVAR_RANDOMIZER_SETTING(var) "gRandoSettings." var
#define CVAR_RANDOMIZER_SETTING_10GS_HINT CVAR_RANDOMIZER_SETTING("10GSHint")
#define CVAR_RANDOMIZER_SETTING_20GS_HINT CVAR_RANDOMIZER_SETTING("20GSHint")
#define CVAR_RANDOMIZER_SETTING_30GS_HINT CVAR_RANDOMIZER_SETTING("30GSHint")
#define CVAR_RANDOMIZER_SETTING_40GS_HINT CVAR_RANDOMIZER_SETTING("40GSHint")
#define CVAR_RANDOMIZER_SETTING_50GS_HINT CVAR_RANDOMIZER_SETTING("50GSHint")
#define CVAR_RANDOMIZER_SETTING_ALL_LOCATIONS_REACHABLE CVAR_RANDOMIZER_SETTING("AllLocationsReachable")
#define CVAR_RANDOMIZER_SETTING_ALTAR_HINT CVAR_RANDOMIZER_SETTING("AltarHint")
#define CVAR_RANDOMIZER_SETTING_BIG_POE_TARGET_COUNT CVAR_RANDOMIZER_SETTING("BigPoeTargetCount")
#define CVAR_RANDOMIZER_SETTING_BLUE_FIRE_ARROWS CVAR_RANDOMIZER_SETTING("BlueFireArrows")
#define CVAR_RANDOMIZER_SETTING_BOMBCHUS_IN_LOGIC CVAR_RANDOMIZER_SETTING("BombchusInLogic")
#define CVAR_RANDOMIZER_SETTING_BOSS_KEYSANITY CVAR_RANDOMIZER_SETTING("BossKeysanity")
#define CVAR_RANDOMIZER_SETTING_BRIDGE_REWARD_OPTIONS CVAR_RANDOMIZER_SETTING("BridgeRewardOptions")
#define CVAR_RANDOMIZER_SETTING_COMPLETE_MASK_QUEST CVAR_RANDOMIZER_SETTING("CompleteMaskQuest")
#define CVAR_RANDOMIZER_SETTING_CUCCOS_TO_RETURN CVAR_RANDOMIZER_SETTING("CuccosToReturn")
#define CVAR_RANDOMIZER_SETTING_DAMPE_HINT CVAR_RANDOMIZER_SETTING("DampeHint")
#define CVAR_RANDOMIZER_SETTING_DECOUPLE_ENTRANCES CVAR_RANDOMIZER_SETTING("DecoupleEntrances")
#define CVAR_RANDOMIZER_SETTING_DONT_GENERATE_SPOILER CVAR_RANDOMIZER_SETTING("DontGenerateSpoiler")
#define CVAR_RANDOMIZER_SETTING_DOOR_OF_TIME CVAR_RANDOMIZER_SETTING("DoorOfTime")
#define CVAR_RANDOMIZER_SETTING_DUNGEON_COUNT CVAR_RANDOMIZER_SETTING("DungeonCount")
#define CVAR_RANDOMIZER_SETTING_ENABLE_BOMBCHU_DROPS CVAR_RANDOMIZER_SETTING("EnableBombchuDrops")
#define CVAR_RANDOMIZER_SETTING_ENABLE_GLITCH_CUTSCENES CVAR_RANDOMIZER_SETTING("EnableGlitchCutscenes")
#define CVAR_RANDOMIZER_SETTING_ENABLED_GLITCHES CVAR_RANDOMIZER_SETTING("EnabledGlitches")
#define CVAR_RANDOMIZER_SETTING_ENABLED_TRICKS CVAR_RANDOMIZER_SETTING("EnabledTricks")
#define CVAR_RANDOMIZER_SETTING_EXCLUDED_LOCATIONS CVAR_RANDOMIZER_SETTING("ExcludedLocations")
#define CVAR_RANDOMIZER_SETTING_FOREST CVAR_RANDOMIZER_SETTING("Forest")
#define CVAR_RANDOMIZER_SETTING_FROGS_HINT CVAR_RANDOMIZER_SETTING("FrogsHint")
#define CVAR_RANDOMIZER_SETTING_FULL_WALLETS CVAR_RANDOMIZER_SETTING("FullWallets")
#define CVAR_RANDOMIZER_SETTING_GANON_TRIAL CVAR_RANDOMIZER_SETTING("GanonTrial")
#define CVAR_RANDOMIZER_SETTING_GANON_TRIAL_COUNT CVAR_RANDOMIZER_SETTING("GanonTrialCount")
#define CVAR_RANDOMIZER_SETTING_GERUDO_FORTRESS CVAR_RANDOMIZER_SETTING("GerudoFortress")
#define CVAR_RANDOMIZER_SETTING_GERUDO_KEYS CVAR_RANDOMIZER_SETTING("GerudoKeys")
#define CVAR_RANDOMIZER_SETTING_GOSSIP_STONE_HINTS CVAR_RANDOMIZER_SETTING("GossipStoneHints")
#define CVAR_RANDOMIZER_SETTING_GREG_HINT CVAR_RANDOMIZER_SETTING("GregHint")
#define CVAR_RANDOMIZER_SETTING_GS_EXPECT_SUNS_SONG CVAR_RANDOMIZER_SETTING("GsExpectSunsSong")
#define CVAR_RANDOMIZER_SETTING_HINT_CLARITY CVAR_RANDOMIZER_SETTING("HintClarity")
#define CVAR_RANDOMIZER_SETTING_HINT_DISTRIBUTION CVAR_RANDOMIZER_SETTING("HintDistribution")
#define CVAR_RANDOMIZER_SETTING_ICE_TRAPS CVAR_RANDOMIZER_SETTING("IceTraps")
#define CVAR_RANDOMIZER_SETTING_ITEM_POOL CVAR_RANDOMIZER_SETTING("ItemPool")
#define CVAR_RANDOMIZER_SETTING_KAKARIKO_GATE CVAR_RANDOMIZER_SETTING("KakarikoGate")
#define CVAR_RANDOMIZER_SETTING_KEYSANITY CVAR_RANDOMIZER_SETTING("Keysanity")
#define CVAR_RANDOMIZER_SETTING_LACS_DUNGEON_COUNT CVAR_RANDOMIZER_SETTING("LacsDungeonCount")
#define CVAR_RANDOMIZER_SETTING_LACS_MEDALLION_COUNT CVAR_RANDOMIZER_SETTING("LacsMedallionCount")
#define CVAR_RANDOMIZER_SETTING_LACS_REWARD_COUNT CVAR_RANDOMIZER_SETTING("LacsRewardCount")
#define CVAR_RANDOMIZER_SETTING_LACS_REWARD_OPTIONS CVAR_RANDOMIZER_SETTING("LacsRewardOptions")
#define CVAR_RANDOMIZER_SETTING_LACS_STONE_COUNT CVAR_RANDOMIZER_SETTING("LacsStoneCount")
#define CVAR_RANDOMIZER_SETTING_LACS_TOKEN_COUNT CVAR_RANDOMIZER_SETTING("LacsTokenCount")
#define CVAR_RANDOMIZER_SETTING_LA_HINT CVAR_RANDOMIZER_SETTING("LAHint")
#define CVAR_RANDOMIZER_SETTING_LINKS_POCKET CVAR_RANDOMIZER_SETTING("LinksPocket")
#define CVAR_RANDOMIZER_SETTING_LOGIC_RULES CVAR_RANDOMIZER_SETTING("LogicRules")
#define CVAR_RANDOMIZER_SETTING_MANUAL_SEED_ENTRY CVAR_RANDOMIZER_SETTING("ManualSeedEntry")
#define CVAR_RANDOMIZER_SETTING_MEDALLION_COUNT CVAR_RANDOMIZER_SETTING("MedallionCount")
#define CVAR_RANDOMIZER_SETTING_MIX_BOSSES CVAR_RANDOMIZER_SETTING("MixBosses")
#define CVAR_RANDOMIZER_SETTING_MIX_DUNGEONS CVAR_RANDOMIZER_SETTING("MixDungeons")
#define CVAR_RANDOMIZER_SETTING_MIXED_ENTRANCES CVAR_RANDOMIZER_SETTING("MixedEntrances")
#define CVAR_RANDOMIZER_SETTING_MIX_GROTTOS CVAR_RANDOMIZER_SETTING("MixGrottos")
#define CVAR_RANDOMIZER_SETTING_MIX_INTERIORS CVAR_RANDOMIZER_SETTING("MixInteriors")
#define CVAR_RANDOMIZER_SETTING_MIX_OVERWORLD CVAR_RANDOMIZER_SETTING("MixOverworld")
#define CVAR_RANDOMIZER_SETTING_MQ_DUNGEON_COUNT CVAR_RANDOMIZER_SETTING("MQDungeonCount")
#define CVAR_RANDOMIZER_SETTING_MQ_DUNGEONS CVAR_RANDOMIZER_SETTING("MQDungeons")
#define CVAR_RANDOMIZER_SETTING_MQ_DUNGEONS_BOTTOM_OF_WELL CVAR_RANDOMIZER_SETTING("MQDungeonsBottomOfTheWell")
#define CVAR_RANDOMIZER_SETTING_MQ_DUNGEONS_DEKU_TREE CVAR_RANDOMIZER_SETTING("MQDungeonsDekuTree")
#define CVAR_RANDOMIZER_SETTING_MQ_DUNGEONS_DODONGOS_CAVERN CVAR_RANDOMIZER_SETTING("MQDungeonsDodongosCavern")
#define CVAR_RANDOMIZER_SETTING_MQ_DUNGEONS_FIRE_TEMPLE CVAR_RANDOMIZER_SETTING("MQDungeonsFireTemple")
#define CVAR_RANDOMIZER_SETTING_MQ_DUNGEONS_FOREST_TEMPLE CVAR_RANDOMIZER_SETTING("MQDungeonsForestTemple")
#define CVAR_RANDOMIZER_SETTING_MQ_DUNGEONS_GANONS_CASTLE CVAR_RANDOMIZER_SETTING("MQDungeonsGanonsCastle")
#define CVAR_RANDOMIZER_SETTING_MQ_DUNGEONS_GTG CVAR_RANDOMIZER_SETTING("MQDungeonsGTG")
#define CVAR_RANDOMIZER_SETTING_MQ_DUNGEONS_ICE_CAVERN CVAR_RANDOMIZER_SETTING("MQDungeonsIceCavern")
#define CVAR_RANDOMIZER_SETTING_MQ_DUNGEONS_JABU_JABU CVAR_RANDOMIZER_SETTING("MQDungeonsJabuJabu")
#define CVAR_RANDOMIZER_SETTING_MQ_DUNGEONS_SHADOW_TEMPLE CVAR_RANDOMIZER_SETTING("MQDungeonsShadowTemple")
#define CVAR_RANDOMIZER_SETTING_MQ_DUNGEONS_SPIRIT_TEMPLE CVAR_RANDOMIZER_SETTING("MQDungeonsSpiritTemple")
#define CVAR_RANDOMIZER_SETTING_MQ_DUNGEONS_WATER_TEMPLE CVAR_RANDOMIZER_SETTING("MQDungeonsWaterTemple")
#define CVAR_RANDOMIZER_SETTING_RAINBOW_BRIDGE CVAR_RANDOMIZER_SETTING("RainbowBridge")
#define CVAR_RANDOMIZER_SETTING_REWARD_COUNT CVAR_RANDOMIZER_SETTING("RewardCount")
#define CVAR_RANDOMIZER_SETTING_SARIA_HINT CVAR_RANDOMIZER_SETTING("SariaHint")
#define CVAR_RANDOMIZER_SETTING_SCRUB_TEXT CVAR_RANDOMIZER_SETTING("ScrubText")
#define CVAR_RANDOMIZER_SETTING_SHOPSANITY CVAR_RANDOMIZER_SETTING("Shopsanity")
#define CVAR_RANDOMIZER_SETTING_SHOPSANITY_PRICES CVAR_RANDOMIZER_SETTING("ShopsanityPrices")
#define CVAR_RANDOMIZER_SETTING_SHOPSANITY_PRICES_AFFORDABLE CVAR_RANDOMIZER_SETTING("ShopsanityPricesAffordable")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_100GS_REWARD CVAR_RANDOMIZER_SETTING("Shuffle100GSReward")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_ADULT_TRADE CVAR_RANDOMIZER_SETTING("ShuffleAdultTrade")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_BEANS CVAR_RANDOMIZER_SETTING("ShuffleBeans")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_BOSS_ENTRANCES CVAR_RANDOMIZER_SETTING("ShuffleBossEntrances")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_COWS CVAR_RANDOMIZER_SETTING("ShuffleCows")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_DUNGEON_REWARD CVAR_RANDOMIZER_SETTING("ShuffleDungeonReward")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_DUNGEONS_ENTRANCES CVAR_RANDOMIZER_SETTING("ShuffleDungeonsEntrances")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_FROG_SONG_RUPEES CVAR_RANDOMIZER_SETTING("ShuffleFrogSongRupees")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_GANON_BOSS_KEY CVAR_RANDOMIZER_SETTING("ShuffleGanonBossKey")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_GERUDO_TOKEN CVAR_RANDOMIZER_SETTING("ShuffleGerudoToken")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_GROTTOS_ENTRANCES CVAR_RANDOMIZER_SETTING("ShuffleGrottosEntrances")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_INTERIORS_ENTRANCES CVAR_RANDOMIZER_SETTING("ShuffleInteriorsEntrances")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_KEY_RINGS CVAR_RANDOMIZER_SETTING("ShuffleKeyRings")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_KEY_RINGS_BOTTOM_OF_WELL CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsBottomOfTheWell")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_KEY_RINGS_FIRE_TEMPLE CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsFireTemple")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_KEY_RINGS_FOREST_TEMPLE CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsForestTemple")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_KEY_RINGS_GANONS_CASTLE CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsGanonsCastle")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_KEY_RINGS_GERUDO_FORTRESS CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsGerudoFortress")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_KEY_RINGS_GTG CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsGTG")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_KEY_RINGS_RANDOM_COUNT CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsRandomCount")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_KEY_RINGS_SHADOW_TEMPLE CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsShadowTemple")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_KEY_RINGS_SPIRIT_TEMPLE CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsSpiritTemple")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_KEY_RINGS_WATER_TEMPLE CVAR_RANDOMIZER_SETTING("ShuffleKeyRingsWaterTemple")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_KOKIRI_SWORD CVAR_RANDOMIZER_SETTING("ShuffleKokiriSword")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_MASTER_SWORD CVAR_RANDOMIZER_SETTING("ShuffleMasterSword")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_MERCHANTS CVAR_RANDOMIZER_SETTING("ShuffleMerchants")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_OCARINAS CVAR_RANDOMIZER_SETTING("ShuffleOcarinas")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_OVERWORLD_ENTRANCES CVAR_RANDOMIZER_SETTING("ShuffleOverworldEntrances")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_OVERWORLD_SPAWNS CVAR_RANDOMIZER_SETTING("ShuffleOverworldSpawns")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_OWL_DROPS CVAR_RANDOMIZER_SETTING("ShuffleOwlDrops")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_SCRUBS CVAR_RANDOMIZER_SETTING("ShuffleScrubs")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_SONGS CVAR_RANDOMIZER_SETTING("ShuffleSongs")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_TOKENS CVAR_RANDOMIZER_SETTING("ShuffleTokens")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_WARP_SONGS CVAR_RANDOMIZER_SETTING("ShuffleWarpSongs")
#define CVAR_RANDOMIZER_SETTING_SHUFFLE_WEIRD_EGG CVAR_RANDOMIZER_SETTING("ShuffleWeirdEgg")
#define CVAR_RANDOMIZER_SETTING_SKIP_CHILD_STEALTH CVAR_RANDOMIZER_SETTING("SkipChildStealth")
#define CVAR_RANDOMIZER_SETTING_SKIP_CHILD_ZELDA CVAR_RANDOMIZER_SETTING("SkipChildZelda")
#define CVAR_RANDOMIZER_SETTING_SKIP_EPONA_RACE CVAR_RANDOMIZER_SETTING("SkipEponaRace")
#define CVAR_RANDOMIZER_SETTING_SKIP_SCARECROWS_SONG CVAR_RANDOMIZER_SETTING("SkipScarecrowsSong")
#define CVAR_RANDOMIZER_SETTING_SKIP_TOWER_ESCAPE CVAR_RANDOMIZER_SETTING("SkipTowerEscape")
#define CVAR_RANDOMIZER_SETTING_STARTING_AGE CVAR_RANDOMIZER_SETTING("StartingAge")
#define CVAR_RANDOMIZER_SETTING_STARTING_CONSUMABLES CVAR_RANDOMIZER_SETTING("StartingConsumables")
#define CVAR_RANDOMIZER_SETTING_STARTING_BOLERO_OF_FIRE CVAR_RANDOMIZER_SETTING("StartingBoleroOfFire")
#define CVAR_RANDOMIZER_SETTING_STARTING_DEKU_SHIELD CVAR_RANDOMIZER_SETTING("StartingDekuShield")
#define CVAR_RANDOMIZER_SETTING_STARTING_EPONAS_SONG CVAR_RANDOMIZER_SETTING("StartingEponasSong")
#define CVAR_RANDOMIZER_SETTING_STARTING_KOKIRI_SWORD CVAR_RANDOMIZER_SETTING("StartingKokiriSword")
#define CVAR_RANDOMIZER_SETTING_STARTING_MAPS_COMPASSES CVAR_RANDOMIZER_SETTING("StartingMapsCompasses")
#define CVAR_RANDOMIZER_SETTING_STARTING_MINUET_OF_FOREST CVAR_RANDOMIZER_SETTING("StartingMinuetOfForest")
#define CVAR_RANDOMIZER_SETTING_STARTING_NOCTURNE_OF_SHADOW CVAR_RANDOMIZER_SETTING("StartingNocturneOfShadow")
#define CVAR_RANDOMIZER_SETTING_STARTING_OCARINA CVAR_RANDOMIZER_SETTING("StartingOcarina")
#define CVAR_RANDOMIZER_SETTING_STARTING_PRELUDE_OF_LIGHT CVAR_RANDOMIZER_SETTING("StartingPreludeOfLight")
#define CVAR_RANDOMIZER_SETTING_STARTING_REQUIEM_OF_SPIRIT CVAR_RANDOMIZER_SETTING("StartingRequiemOfSpirit")
#define CVAR_RANDOMIZER_SETTING_STARTING_SARIAS_SONG CVAR_RANDOMIZER_SETTING("StartingSariasSong")
#define CVAR_RANDOMIZER_SETTING_STARTING_SERENADE_OF_WATER CVAR_RANDOMIZER_SETTING("StartingSerenadeOfWater")
#define CVAR_RANDOMIZER_SETTING_STARTING_SKULLTULA_TOKEN CVAR_RANDOMIZER_SETTING("StartingSkulltulaToken")
#define CVAR_RANDOMIZER_SETTING_STARTING_SONG_OF_STORMS CVAR_RANDOMIZER_SETTING("StartingSongOfStorms")
#define CVAR_RANDOMIZER_SETTING_STARTING_SONG_OF_TIME CVAR_RANDOMIZER_SETTING("StartingSongOfTime")
#define CVAR_RANDOMIZER_SETTING_STARTING_SUNS_SONG CVAR_RANDOMIZER_SETTING("StartingSunsSong")
#define CVAR_RANDOMIZER_SETTING_STARTING_ZELDAS_LULLABY CVAR_RANDOMIZER_SETTING("StartingZeldasLullaby")
#define CVAR_RANDOMIZER_SETTING_STONE_COUNT CVAR_RANDOMIZER_SETTING("StoneCount")
#define CVAR_RANDOMIZER_SETTING_SUNLIGHT_ARROWS CVAR_RANDOMIZER_SETTING("SunlightArrows")
#define CVAR_RANDOMIZER_SETTING_TOKEN_COUNT CVAR_RANDOMIZER_SETTING("TokenCount")
#define CVAR_RANDOMIZER_SETTING_TRIFORCE_HUNT_TOTAL_PIECES CVAR_RANDOMIZER_SETTING("TriforceHuntTotalPieces")
#define CVAR_RANDOMIZER_SETTING_TRIFORCE_HUNT_REQUIRED_PIECES CVAR_RANDOMIZER_SETTING("TriforceHuntRequiredPieces")
#define CVAR_RANDOMIZER_SETTING_TRIFORCE_HUNT CVAR_RANDOMIZER_SETTING("TriforceHunt")
#define CVAR_RANDOMIZER_SETTING_WARP_SONG_TEXT CVAR_RANDOMIZER_SETTING("WarpSongText")
#define CVAR_RANDOMIZER_SETTING_ZORAS_FOUNTAIN CVAR_RANDOMIZER_SETTING("ZorasFountain")

#define CVAR_COSMETIC(var) "gCosmetics." var
#define CVAR_AUDIO(var) "gAudioEditor." var
#define CVAR_CHEAT(var) "gCheats." var
#define CVAR_ENHANCEMENT(var) "gEnhancements." var
#define CVAR_SETTING(var) "gSettings." var
#define CVAR_WINDOW(var) "gOpenWindows." var
#define CVAR_TRACKER(var) "gTrackers." var
#define CVAR_TRACKER_ITEM(var) CVAR_TRACKER("ItemTracker." var)
#define CVAR_TRACKER_CHECK(var) CVAR_TRACKER("CheckTracker." var)
#define CVAR_TRACKER_ENTRANCE(var) CVAR_TRACKER("EntranceTracker." var)
#define CVAR_DEVELOPER_TOOLS(var) "gDeveloperTools." var
#define CVAR_GENERAL(var) "gGeneral." var
#define CVAR_REMOTE(var) "gRemote." var

#ifndef __cplusplus
    void InitOTR(void);
void DeinitOTR(void);
void VanillaItemTable_Init();
void OTRAudio_Init();
void OTRMessage_Init();
void InitAudio();
void Graph_StartFrame();
void Graph_ProcessGfxCommands(Gfx* commands);
void Graph_ProcessFrame(void (*run_one_game_iter)(void));
void OTRLogString(const char* src);
void OTRGfxPrint(const char* str, void* printer, void (*printImpl)(void*, char));
void OTRGetPixelDepthPrepare(float x, float y);
uint16_t OTRGetPixelDepth(float x, float y);
int32_t OTRGetLastScancode();
uint32_t ResourceMgr_IsGameMasterQuest();
uint32_t ResourceMgr_IsSceneMasterQuest(s16 sceneNum);
uint32_t ResourceMgr_GameHasMasterQuest();
uint32_t ResourceMgr_GameHasOriginal();
uint32_t ResourceMgr_GetNumGameVersions();
uint32_t ResourceMgr_GetGameVersion(int index);
uint32_t ResourceMgr_GetGamePlatform(int index);
uint32_t ResourceMgr_GetGameRegion(int index);
void ResourceMgr_LoadDirectory(const char* resName);
void ResourceMgr_UnloadResource(const char* resName);
char** ResourceMgr_ListFiles(const char* searchMask, int* resultSize);
uint8_t ResourceMgr_FileExists(const char* resName);
uint8_t ResourceMgr_FileAltExists(const char* resName);
void ResourceMgr_UnloadOriginalWhenAltExists(const char* resName);
char* GetResourceDataByNameHandlingMQ(const char* path);
uint8_t ResourceMgr_TexIsRaw(const char* texPath);
uint8_t ResourceMgr_ResourceIsBackground(char* texPath);
char* ResourceMgr_LoadJPEG(char* data, size_t dataSize);
uint16_t ResourceMgr_LoadTexWidthByName(char* texPath);
uint16_t ResourceMgr_LoadTexHeightByName(char* texPath);
char* ResourceMgr_LoadTexOrDListByName(const char* filePath);
char* ResourceMgr_LoadPlayerAnimByName(const char* animPath);
AnimationHeaderCommon* ResourceMgr_LoadAnimByName(const char* path);
char* ResourceMgr_GetNameByCRC(uint64_t crc, char* alloc);
Gfx* ResourceMgr_LoadGfxByCRC(uint64_t crc);
Gfx* ResourceMgr_LoadGfxByName(const char* path);
uint8_t ResourceMgr_FileIsCustomByName(const char* path);
void ResourceMgr_PatchGfxByName(const char* path, const char* patchName, int index, Gfx instruction);
void ResourceMgr_UnpatchGfxByName(const char* path, const char* patchName);
char* ResourceMgr_LoadArrayByNameAsVec3s(const char* path);
Vtx* ResourceMgr_LoadVtxByCRC(uint64_t crc);

Vtx* ResourceMgr_LoadVtxByName(char* path);
SoundFont* ResourceMgr_LoadAudioSoundFont(const char* path);
SequenceData ResourceMgr_LoadSeqByName(const char* path);
SoundFontSample* ResourceMgr_LoadAudioSample(const char* path);
CollisionHeader* ResourceMgr_LoadColByName(const char* path);
void Ctx_ReadSaveFile(uintptr_t addr, void* dramAddr, size_t size);
void Ctx_WriteSaveFile(uintptr_t addr, void* dramAddr, size_t size);

uint64_t GetPerfCounter();
struct SkeletonHeader* ResourceMgr_LoadSkeletonByName(const char* path, SkelAnime* skelAnime);
void ResourceMgr_UnregisterSkeleton(SkelAnime* skelAnime);
void ResourceMgr_ClearSkeletons();
s32* ResourceMgr_LoadCSByName(const char* path);
int ResourceMgr_OTRSigCheck(char* imgData);
uint64_t osGetTime(void);
uint32_t osGetCount(void);
uint32_t OTRGetCurrentWidth(void);
uint32_t OTRGetCurrentHeight(void);
float OTRGetAspectRatio(void);
float OTRGetDimensionFromLeftEdge(float v);
float OTRGetDimensionFromRightEdge(float v);
int16_t OTRGetRectDimensionFromLeftEdge(float v);
int16_t OTRGetRectDimensionFromRightEdge(float v);
int AudioPlayer_Buffered(void);
int AudioPlayer_GetDesiredBuffered(void);
void AudioPlayer_Play(const uint8_t* buf, uint32_t len);
void AudioMgr_CreateNextAudioBuffer(s16* samples, u32 num_samples);
int Controller_ShouldRumble(size_t slot);
void Controller_BlockGameInput();
void Controller_UnblockGameInput();
void* getN64WeirdFrame(s32 i);
int GetEquipNowMessage(char* buffer, char* src, const int maxBufferSize);
u32 SpoilerFileExists(const char* spoilerFileName);
Sprite* GetSeedTexture(uint8_t index);
void Randomizer_LoadSettings(const char* spoilerFileName);
u8 Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);
RandomizerCheck Randomizer_GetCheckFromActor(s16 actorId, s16 sceneNum, s16 actorParams);
ScrubIdentity Randomizer_IdentifyScrub(s32 sceneNum, s32 actorParams, s32 respawnData);
ShopItemIdentity Randomizer_IdentifyShopItem(s32 sceneNum, u8 slotIndex);
CowIdentity Randomizer_IdentifyCow(s32 sceneNum, s32 posX, s32 posZ);
void Randomizer_LoadHintLocations(const char* spoilerFileName);
void Randomizer_LoadMerchantMessages(const char* spoilerFileName);
void Randomizer_LoadRequiredTrials(const char* spoilerFileName);
void Randomizer_LoadMasterQuestDungeons(const char* spoilerFileName);
void Randomizer_LoadItemLocations(const char* spoilerFileName, bool silent);
void Randomizer_LoadEntranceOverrides(const char* spoilerFileName, bool silent);
bool Randomizer_IsTrialRequired(RandomizerInf trial);
GetItemEntry Randomizer_GetItemFromActor(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogId);
GetItemEntry Randomizer_GetItemFromActorWithoutObtainabilityCheck(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogId);
GetItemEntry Randomizer_GetItemFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogId);
GetItemEntry Randomizer_GetItemFromKnownCheckWithoutObtainabilityCheck(RandomizerCheck randomizerCheck, GetItemID ogId);
ItemObtainability Randomizer_GetItemObtainabilityFromRandomizerCheck(RandomizerCheck randomizerCheck);
int CustomMessage_RetrieveIfExists(PlayState* play);
void Overlay_DisplayText(float duration, const char* text);
void Overlay_DisplayText_Seconds(int seconds, const char* text);
GetItemEntry ItemTable_Retrieve(int16_t getItemID);
GetItemEntry ItemTable_RetrieveEntry(s16 modIndex, s16 getItemID);
void Entrance_ClearEntranceTrackingData(void);
void Entrance_InitEntranceTrackingData(void);
void EntranceTracker_SetCurrentGrottoID(s16 entranceIndex);
void EntranceTracker_SetLastEntranceOverride(s16 entranceIndex);
void Gfx_RegisterBlendedTexture(const char* name, u8* mask, u8* replacement);
void Gfx_UnregisterBlendedTexture(const char* name);
void Gfx_TextureCacheDelete(const uint8_t* addr);
void SaveManager_ThreadPoolWait();
void CheckTracker_OnMessageClose();

GetItemID RetrieveGetItemIDFromItemID(ItemID itemID);
RandomizerGet RetrieveRandomizerGetFromItemID(ItemID itemID);
#endif

#ifdef __cplusplus
extern "C" {
#endif
uint64_t GetUnixTimestamp();
#ifdef __cplusplus
};
#endif

#endif
