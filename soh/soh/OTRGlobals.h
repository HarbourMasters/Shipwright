#ifndef OTR_GLOBALS_H
#define OTR_GLOBALS_H

#pragma once

#include "SaveManager.h"
#include <soh/Enhancements/item-tables/ItemTableTypes.h>

#ifdef __cplusplus
#include <Window.h>
#include "Enhancements/savestates.h"
#include "Enhancements/randomizer/randomizer.h"
#include <vector>

const std::string customMessageTableID = "BaseGameOverrides";

class OTRGlobals
{
public:
    static OTRGlobals* Instance;

    std::shared_ptr<Ship::Window> context;
    std::shared_ptr<SaveStateMgr> gSaveStateMgr;
    std::shared_ptr<Randomizer> gRandomizer;

    OTRGlobals();
    ~OTRGlobals();

    bool HasMasterQuest();
    bool HasOriginal();
    std::shared_ptr<std::vector<std::string>> ListFiles(std::string path);

private:
	void CheckSaveFile(size_t sramSize) const;
    bool hasMasterQuest;
    bool hasOriginal;
};

uint32_t IsGameMasterQuest();
#endif

#ifndef __cplusplus
    void InitOTR(void);
void DeinitOTR(void);
void VanillaItemTable_Init();
void OTRAudio_Init();
void InitAudio();
void Graph_StartFrame();
void Graph_ProcessGfxCommands(Gfx* commands);
void OTRLogString(const char* src);
void OTRGfxPrint(const char* str, void* printer, void (*printImpl)(void*, char));
void OTRGetPixelDepthPrepare(float x, float y);
uint16_t OTRGetPixelDepth(float x, float y);
int32_t OTRGetLastScancode();
uint32_t ResourceMgr_IsGameMasterQuest();
uint32_t ResourceMgr_GameHasMasterQuest();
uint32_t ResourceMgr_GameHasOriginal();
uint32_t ResourceMgr_GetNumGameVersions();
uint32_t ResourceMgr_GetGameVersion(int index);
void ResourceMgr_CacheDirectory(const char* resName);
char** ResourceMgr_ListFiles(const char* searchMask, int* resultSize);
void ResourceMgr_LoadFile(const char* resName);
char* ResourceMgr_LoadFileFromDisk(const char* filePath);
char* ResourceMgr_LoadJPEG(char* data, int dataSize);
char* ResourceMgr_LoadTexByName(const char* texPath);
uint16_t ResourceMgr_LoadTexWidthByName(char* texPath);
uint16_t ResourceMgr_LoadTexHeightByName(char* texPath);
uint32_t ResourceMgr_LoadTexSizeByName(char* texPath);
char* ResourceMgr_LoadTexOrDListByName(const char* filePath);
char* ResourceMgr_LoadPlayerAnimByName(const char* animPath);
AnimationHeaderCommon* ResourceMgr_LoadAnimByName(const char* path);
char* ResourceMgr_GetNameByCRC(uint64_t crc, char* alloc);
Gfx* ResourceMgr_LoadGfxByCRC(uint64_t crc);
Gfx* ResourceMgr_LoadGfxByName(const char* path);
void ResourceMgr_PatchGfxByName(const char* path, const char* patchName, int index, Gfx instruction);
void ResourceMgr_UnpatchGfxByName(const char* path, const char* patchName);
char* ResourceMgr_LoadArrayByNameAsVec3s(const char* path);
Vtx* ResourceMgr_LoadVtxByCRC(uint64_t crc);

Vtx* ResourceMgr_LoadVtxByName(const char* path);
SoundFont* ResourceMgr_LoadAudioSoundFont(const char* path);
SequenceData ResourceMgr_LoadSeqByName(const char* path);
SoundFontSample* ResourceMgr_LoadAudioSample(const char* path);
CollisionHeader* ResourceMgr_LoadColByName(const char* path);
void Ctx_ReadSaveFile(uintptr_t addr, void* dramAddr, size_t size);
void Ctx_WriteSaveFile(uintptr_t addr, void* dramAddr, size_t size);

uint64_t GetPerfCounter();
struct SkeletonHeader* ResourceMgr_LoadSkeletonByName(const char* path);
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
char* ResourceMgr_LoadFileRaw(const char* resName);
bool AudioPlayer_Init(void);
int AudioPlayer_Buffered(void);
int AudioPlayer_GetDesiredBuffered(void);
void AudioPlayer_Play(const uint8_t* buf, uint32_t len);
void AudioMgr_CreateNextAudioBuffer(s16* samples, u32 num_samples);
int Controller_ShouldRumble(size_t i);
void Controller_BlockGameInput();
void Controller_UnblockGameInput();
void Hooks_ExecuteAudioInit();
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
GetItemEntry ItemTable_Retrieve(int16_t getItemID);
GetItemEntry ItemTable_RetrieveEntry(s16 modIndex, s16 getItemID);
void Entrance_ClearEntranceTrackingData(void);
void Entrance_InitEntranceTrackingData(void);
void EntranceTracker_SetCurrentGrottoID(s16 entranceIndex);
void EntranceTracker_SetLastEntranceOverride(s16 entranceIndex);
#endif

#endif
