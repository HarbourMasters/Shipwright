#ifndef OTR_GLOBALS_H
#define OTR_GLOBALS_H

#pragma once

#include "SaveManager.h"
#include <soh/Enhancements/item-tables/ItemTableTypes.h>

#define GAME_REGION_NTSC 0
#define GAME_REGION_PAL 1

#define GAME_PLATFORM_N64 0
#define GAME_PLATFORM_GC 1

#define BTN_CUSTOM_MODIFIER1 0x0040
#define BTN_CUSTOM_MODIFIER2 0x0080

#define BTN_CUSTOM_OCARINA_NOTE_D4 ((CONTROLLERBUTTONS_T)0x00010000)
#define BTN_CUSTOM_OCARINA_NOTE_F4 ((CONTROLLERBUTTONS_T)0x00020000)
#define BTN_CUSTOM_OCARINA_NOTE_A4 ((CONTROLLERBUTTONS_T)0x00040000)
#define BTN_CUSTOM_OCARINA_NOTE_B4 ((CONTROLLERBUTTONS_T)0x00080000)
#define BTN_CUSTOM_OCARINA_NOTE_D5 ((CONTROLLERBUTTONS_T)0x00100000)
#define BTN_CUSTOM_OCARINA_DISABLE_SONGS ((CONTROLLERBUTTONS_T)0x00200000)
#define BTN_CUSTOM_OCARINA_PITCH_UP ((CONTROLLERBUTTONS_T)0x00400000)
#define BTN_CUSTOM_OCARINA_PITCH_DOWN ((CONTROLLERBUTTONS_T)0x00800000)

#ifdef __cplusplus
#include <Context.h>
#include "Enhancements/savestates.h"
#include "Enhancements/randomizer/randomizer.h"
#include <vector>
#include <string>

struct ExtensionEntry {
    std::string path;
    std::string ext;
};

extern std::unordered_map<std::string, ExtensionEntry> ExtensionCache;
#include "Enhancements/randomizer/context.h"

const std::string customMessageTableID = "BaseGameOverrides";
const std::string appShortName = "soh";

#ifdef __WIIU__
const uint32_t defaultImGuiScale = 3;
#else
const uint32_t defaultImGuiScale = 1;
#endif

const float imguiScaleOptionToValue[4] = { 0.75f, 1.0f, 1.5f, 2.0f };

class OTRGlobals {
    public:
        static OTRGlobals* Instance;

    std::shared_ptr<Ship::Context> context;
    std::shared_ptr<SaveStateMgr> gSaveStateMgr;
    std::shared_ptr<Randomizer> gRandomizer;
    std::shared_ptr<Rando::Context> gRandoContext;

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
#endif

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
char* GetResourceDataByNameHandlingMQ(const char* path);

void Ctx_ReadSaveFile(uintptr_t addr, void* dramAddr, size_t size);
void Ctx_WriteSaveFile(uintptr_t addr, void* dramAddr, size_t size);

uint64_t GetPerfCounter();
uint64_t osGetTime(void);
uint32_t osGetCount(void);
uint32_t OTRGetCurrentWidth(void);
uint32_t OTRGetCurrentHeight(void);
float OTRGetAspectRatio(void);
float OTRGetDimensionFromLeftEdge(float v);
float OTRGetDimensionFromRightEdge(float v);
int16_t OTRGetRectDimensionFromLeftEdge(float v);
int16_t OTRGetRectDimensionFromRightEdge(float v);
uint32_t OTRGetGameRenderWidth();
uint32_t OTRGetGameRenderHeight();
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
uint8_t GetSeedIconIndex(uint8_t index);
u8 Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);
RandomizerCheck Randomizer_GetCheckFromActor(s16 actorId, s16 sceneNum, s16 actorParams);
ScrubIdentity Randomizer_IdentifyScrub(s32 sceneNum, s32 actorParams, s32 respawnData);
BeehiveIdentity Randomizer_IdentifyBeehive(s32 sceneNum, s16 xPosition, s32 respawnData);
ShopItemIdentity Randomizer_IdentifyShopItem(s32 sceneNum, u8 slotIndex);
CowIdentity Randomizer_IdentifyCow(s32 sceneNum, s32 posX, s32 posZ);
FishIdentity Randomizer_IdentifyFish(s32 sceneNum, s32 actorParams);
void Randomizer_ParseSpoiler(const char* fileLoc);
void Randomizer_LoadHintMessages();
void Randomizer_LoadMerchantMessages();
bool Randomizer_IsTrialRequired(s32 trialFlag);
GetItemEntry Randomizer_GetItemFromActor(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogId);
GetItemEntry Randomizer_GetItemFromActorWithoutObtainabilityCheck(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogId);
GetItemEntry Randomizer_GetItemFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogId);
GetItemEntry Randomizer_GetItemFromKnownCheckWithoutObtainabilityCheck(RandomizerCheck randomizerCheck, GetItemID ogId);
RandomizerInf Randomizer_GetRandomizerInfFromCheck(RandomizerCheck randomizerCheck);
bool Randomizer_IsCheckShuffled(RandomizerCheck check);
GetItemEntry GetItemMystery();
ItemObtainability Randomizer_GetItemObtainabilityFromRandomizerCheck(RandomizerCheck randomizerCheck);
void Randomizer_GenerateSeed();
uint8_t Randomizer_IsSeedGenerated();
void Randomizer_SetSeedGenerated(bool seedGenerated);
uint8_t Randomizer_IsSpoilerLoaded();
void Randomizer_SetSpoilerLoaded(bool spoilerLoaded);
uint8_t Randomizer_IsPlandoLoaded();
void Randomizer_SetPlandoLoaded(bool plandoLoaded);
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
