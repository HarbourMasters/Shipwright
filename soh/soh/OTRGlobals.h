#ifndef OTR_GLOBALS_H
#define OTR_GLOBALS_H

#pragma once

#include "SaveManager.h"

#ifdef __cplusplus
#include "Window.h"
#include "Enhancements/savestates.h"
#include "Enhancements/randomizer/randomizer.h"

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

private:
	void CheckSaveFile(size_t sramSize) const;
};
#endif

#ifndef __cplusplus
void OTRAudio_Init();
void InitAudio();
void Graph_StartFrame();
void Graph_ProcessGfxCommands(Gfx* commands);
void OTRLogString(const char* src);
void OTRGfxPrint(const char* str, void* printer, void (*printImpl)(void*, char));
void OTRGetPixelDepthPrepare(float x, float y);
uint16_t OTRGetPixelDepth(float x, float y);
int32_t OTRGetLastScancode();
uint32_t ResourceMgr_GetGameVersion();
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
Gfx* ResourceMgr_PatchGfxByName(const char* path, int size);
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
void Hooks_ExecuteAudioInit();
void* getN64WeirdFrame(s32 i);
Sprite* GetSeedTexture(uint8_t index);
void Randomizer_LoadSettings(const char* spoilerFileName);
u8 Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);
RandomizerCheck Randomizer_GetCheckFromActor(s16 actorId, s16 actorParams, s16 sceneNum);
void Randomizer_LoadHintLocations(const char* spoilerFileName);
void Randomizer_LoadItemLocations(const char* spoilerFileName, bool silent);
s16 Randomizer_GetItemModelFromId(s16 itemId);
s32 Randomizer_GetItemIDFromGetItemID(s32 getItemId);
s32 Randomizer_GetRandomizedItemId(GetItemID ogId, s16 actorId, s16 actorParams, s16 sceneNum);
s32 Randomizer_GetItemIdFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogId);
bool Randomizer_ObtainedFreestandingIceTrap(RandomizerCheck randomizerCheck, GetItemID ogId, Actor* actor);
bool Randomizer_ItemIsIceTrap(RandomizerCheck randomizerCheck, GetItemID ogId);
int CustomMessage_RetrieveIfExists(GlobalContext* globalCtx);
#endif

#endif
