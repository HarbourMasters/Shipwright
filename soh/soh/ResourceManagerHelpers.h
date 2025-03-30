#pragma once

#include "libultraship/libultra/types.h"

#define GAME_REGION_NTSC 0
#define GAME_REGION_PAL 1

#define GAME_PLATFORM_N64 0
#define GAME_PLATFORM_GC 1

#ifdef __cplusplus
#include <memory>
#include <Resource.h>

std::shared_ptr<Ship::IResource> ResourceMgr_GetResourceByNameHandlingMQ(const char* path);

extern "C" {
#endif // __cplusplus
    #include "z64animation.h"
    #include "z64audio.h"
    #include "z64bgcheck.h"
    uint32_t ResourceMgr_IsGameMasterQuest();
    uint32_t ResourceMgr_IsSceneMasterQuest(s16 sceneNum);
    uint32_t ResourceMgr_GameHasMasterQuest();
    uint32_t ResourceMgr_GameHasOriginal();
    uint32_t ResourceMgr_GetNumGameVersions();
    uint32_t ResourceMgr_GetGameVersion(int index);
    uint32_t ResourceMgr_GetGamePlatform(int index);
    uint32_t ResourceMgr_GetGameRegion(int index);
    bool ResourceMgr_IsPalLoaded();
    void ResourceMgr_LoadDirectory(const char* resName);
    void ResourceMgr_UnloadResource(const char* resName);
    char** ResourceMgr_ListFiles(const char* searchMask, int* resultSize);
    uint8_t ResourceMgr_FileExists(const char* resName);
    uint8_t ResourceMgr_FileAltExists(const char* resName);
    void ResourceMgr_UnloadOriginalWhenAltExists(const char* resName);
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
    bool ResourceMgr_IsAltAssetsEnabled();
    SkeletonHeader* ResourceMgr_LoadSkeletonByName(const char* path, SkelAnime* skelAnime);
    void ResourceMgr_UnregisterSkeleton(SkelAnime* skelAnime);
    void ResourceMgr_ClearSkeletons();
    s32* ResourceMgr_LoadCSByName(const char* path);
    int ResourceMgr_OTRSigCheck(char* imgData);
    char* ResourceMgr_GetResourceDataByNameHandlingMQ(const char* path);
#ifdef __cplusplus
}
#endif // __cplusplus