#ifndef OTR_GLOBALS_H
#define OTR_GLOBALS_H

#pragma once

#define BTN_CUSTOM_MODIFIER1 0x0040
#define BTN_CUSTOM_MODIFIER2 0x0080

// Ocarina custom controls (using bits beyond standard 16-bit N64 buttons)
#define BTN_CUSTOM_OCARINA_NOTE_D4 ((CONTROLLERBUTTONS_T)0x00010000)
#define BTN_CUSTOM_OCARINA_NOTE_F4 ((CONTROLLERBUTTONS_T)0x00020000)
#define BTN_CUSTOM_OCARINA_NOTE_A4 ((CONTROLLERBUTTONS_T)0x00040000)
#define BTN_CUSTOM_OCARINA_NOTE_B4 ((CONTROLLERBUTTONS_T)0x00080000)
#define BTN_CUSTOM_OCARINA_NOTE_D5 ((CONTROLLERBUTTONS_T)0x00100000)
#define BTN_CUSTOM_OCARINA_DISABLE_SONGS ((CONTROLLERBUTTONS_T)0x00200000)
#define BTN_CUSTOM_OCARINA_PITCH_UP ((CONTROLLERBUTTONS_T)0x00400000)
#define BTN_CUSTOM_OCARINA_PITCH_DOWN ((CONTROLLERBUTTONS_T)0x00800000)

#define GAME_REGION_NTSC 0
#define GAME_REGION_PAL 1

#define GAME_PLATFORM_N64 0
#define GAME_PLATFORM_GC 1

#define MM_NTSC_US_10 0x5354631C
#define MM_NTSC_US_GC 0xB443EB08

#ifdef __cplusplus
#include <ship/Context.h>

#include <vector>

struct ImFont;

const std::string customMessageTableID = "BaseGameOverrides";
const std::string appShortName = "2ship";

#ifdef __WIIU__
const uint32_t defaultImGuiScale = 3;
#else
const uint32_t defaultImGuiScale = 1;
#endif

const float imguiScaleOptionToValue[4] = { 0.75f, 1.0f, 1.5f, 2.0f };

class OTRGlobals {
  public:
    static OTRGlobals* Instance;

    ImFont* fontStandard = nullptr;
    ImFont* fontStandardLarger = nullptr;
    ImFont* fontStandardLargest = nullptr;
    ImFont* fontMono = nullptr;
    ImFont* fontMonoLarger = nullptr;
    ImFont* fontMonoLargest = nullptr;

    Ship::Context* context = nullptr;

    OTRGlobals();
    ~OTRGlobals();

    uint32_t GetInterpolationFPS();
    std::shared_ptr<std::vector<std::string>> ListFiles(std::string path);
    void RunExtract(int argc, char* argv[]);
    void Initialize();
    void ScaleImGui();

  private:
    ImFont* CreateFontWithSize(float size, std::string fontPath = "");
    void CheckSaveFile(size_t sramSize) const;
    ImFont* CreateDefaultFontWithSize(float size);
};

uint32_t IsGameMasterQuest();
#endif

#ifndef __cplusplus
#include <z64audio.h>
#include <z64bgcheck.h>
#include <z64camera.h>
#include <z64game.h>
#include <z64keyframe.h>
#include <z64scene.h>
#include <z64skin.h>
void InitOTR(int argc, char* argv[]);
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
uint32_t ResourceMgr_GetNumGameVersions();
uint32_t ResourceMgr_GetGameVersion(int index);
uint32_t ResourceMgr_GetGamePlatform(int index);
uint32_t ResourceMgr_GetGameRegion(int index);
void ResourceMgr_LoadDirectory(const char* resName);
char** ResourceMgr_ListFiles(const char* searchMask, int* resultSize);
uint8_t ResourceMgr_FileExists(const char* resName);
void ResourceMgr_LoadFile(const char* resName);
char* ResourceMgr_LoadFileFromDisk(const char* filePath);
uint8_t ResourceMgr_ResourceIsBackground(char* texPath);
char* ResourceMgr_LoadJPEG(char* data, size_t dataSize);
uint16_t ResourceMgr_LoadTexWidthByName(char* texPath);
uint16_t ResourceMgr_LoadTexHeightByName(char* texPath);
CollisionHeader* ResourceMgr_LoadColByName(const char* path);
AnimatedMaterial* ResourceMgr_LoadAnimatedMatByName(const char* path);
char* ResourceMgr_LoadTexOrDListByName(const char* filePath);
char* ResourceMgr_LoadIfDListByName(const char* filePath);
char* ResourceMgr_LoadPlayerAnimByName(const char* animPath);
AnimationHeaderCommon* ResourceMgr_LoadAnimByName(const char* path);
char* ResourceMgr_GetNameByCRC(uint64_t crc, char* alloc);
Gfx* ResourceMgr_LoadGfxByCRC(uint64_t crc);
Gfx* ResourceMgr_LoadGfxByName(const char* path);
void ResourceMgr_PatchGfxByName(const char* path, const char* patchName, int index, Gfx instruction);
void ResourceMgr_UnpatchGfxByName(const char* path, const char* patchName);
size_t ResourceMgr_GetPatchCountForDL(const char* path);
void ResourceMgr_ResetAllPatchesForDL(const char* path);
u8* ResourceMgr_LoadArrayByNameAsU8(const char* path, u8* buffer);
char* ResourceMgr_LoadArrayByNameAsVec3s(const char* path);
char* ResourceMgr_LoadArrayByName(const char* path);
size_t ResourceMgr_GetArraySizeByName(const char* path);
Vtx* ResourceMgr_LoadVtxByCRC(uint64_t crc);
char* ResourceMgr_LoadVtxArrayByName(const char* path);
size_t ResourceMgr_GetVtxArraySizeByName(const char* path);
Vtx* ResourceMgr_LoadVtxByName(char* path);
SequenceData* ResourceMgr_LoadSeqPtrByName(const char* path);
Mtx* ResourceMgr_LoadMtxByName(char* path);
KeyFrameSkeleton* ResourceMgr_LoadKeyFrameSkelByName(const char* path);
KeyFrameAnimation* ResourceMgr_LoadKeyFrameAnimByName(const char* path);

void Ctx_ReadSaveFile(uintptr_t addr, void* dramAddr, size_t size);
void Ctx_WriteSaveFile(uintptr_t addr, void* dramAddr, size_t size);

uint64_t GetPerfCounter();
bool ResourceMgr_IsAltAssetsEnabled();
struct SkeletonHeader* ResourceMgr_LoadSkeletonByName(const char* path, SkelAnime* skelAnime);
void ResourceMgr_UnregisterSkeleton(SkelAnime* skelAnime);
void ResourceMgr_ClearSkeletons();
s32* ResourceMgr_LoadCSByName(const char* path);
int ResourceMgr_OTRSigCheck(char* imgData);
uint64_t osGetTime(void);
uint32_t osGetCount(void);
uint64_t GetFrequency();
uint32_t OTRGetCurrentWidth(void);
uint32_t OTRGetCurrentHeight(void);
float OTRGetAspectRatio(void);
int32_t OTRConvertHUDXToScreenX(int32_t v);
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
void Overlay_DisplayText(float duration, const char* text);
void Overlay_DisplayText_Seconds(int seconds, const char* text);
uint32_t Ship_GetInterpolationFPS();

void Gfx_RegisterBlendedTexture(const char* name, u8* mask, u8* replacement);
void Gfx_UnregisterBlendedTexture(const char* name);
void Gfx_TextureCacheDelete(const uint8_t* texAddr);
void CheckTracker_OnMessageClose();

void Messagebox_ShowErrorBox(char* title, char* body);
bool Ship_HandleConsoleCrashAsReset();

int32_t GetGIID(uint32_t itemID);
#endif

#ifdef __cplusplus
extern "C" {
#endif
uint64_t GetUnixTimestamp();
void CrashHandler_PrintExt(char* buffer, size_t* pos);
#ifdef __cplusplus
};
#endif

#endif
