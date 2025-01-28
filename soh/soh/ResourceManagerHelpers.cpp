#include "ResourceManagerHelpers.h"
#include "OTRGlobals.h"
#include "variables.h"
#include "z64.h"
#include "cvar_prefixes.h"
#include "Enhancements/enhancementTypes.h"
#include "Enhancements/randomizer/dungeon.h"
#include <libultraship/libultraship.h>
#include <soh/GameVersions.h>
#include "resource/type/SohResourceType.h"
#include "resource/type/Array.h"
#include "resource/type/Skeleton.h"
#include "resource/type/PlayerAnimation.h"
#include "soh/ActorDB.h"
#include "soh/Enhancements/TimeDisplay/TimeDisplay.h"
#include "soh/resource/type/Scene.h"
#include "soh/resource/type/scenecommand/SetRoomList.h"
#include "soh/resource/type/scenecommand/SetActorList.h"
#include "soh/resource/type/scenecommand/SetObjectList.h"
#include <Fast3D/gfx_pc.h>
#include <DisplayList.h>
#include "resource/ResourceManager.h"

s16 unloadScene = -1;
std::shared_ptr<BS::thread_pool> helperThreads;

extern "C" PlayState* gPlayState;

extern "C" uint32_t ResourceMgr_GetNumGameVersions() {
    return Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->GetGameVersions().size();
}

extern "C" uint32_t ResourceMgr_GetGameVersion(int index) {
    return Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->GetGameVersions()[index];
}

extern "C" uint32_t ResourceMgr_GetGamePlatform(int index) {
    uint32_t version = Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->GetGameVersions()[index];

    switch (version) {
        case OOT_NTSC_US_10:
        case OOT_NTSC_US_11:
        case OOT_NTSC_US_12:
        case OOT_PAL_10:
        case OOT_PAL_11:
            return GAME_PLATFORM_N64;
        case OOT_NTSC_JP_GC:
        case OOT_NTSC_US_GC:
        case OOT_PAL_GC:
        case OOT_NTSC_JP_MQ:
        case OOT_NTSC_US_MQ:
        case OOT_PAL_MQ:
        case OOT_PAL_GC_DBG1:
        case OOT_PAL_GC_DBG2:
        case OOT_PAL_GC_MQ_DBG:
            return GAME_PLATFORM_GC;
    }
}

extern "C" uint32_t ResourceMgr_GetGameRegion(int index) {
    uint32_t version = Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->GetGameVersions()[index];

    switch (version) {
        case OOT_NTSC_US_10:
        case OOT_NTSC_US_11:
        case OOT_NTSC_US_12:
        case OOT_NTSC_JP_GC:
        case OOT_NTSC_US_GC:
        case OOT_NTSC_JP_MQ:
        case OOT_NTSC_US_MQ:
            return GAME_REGION_NTSC;
        case OOT_PAL_10:
        case OOT_PAL_11:
        case OOT_PAL_GC:
        case OOT_PAL_MQ:
        case OOT_PAL_GC_DBG1:
        case OOT_PAL_GC_DBG2:
        case OOT_PAL_GC_MQ_DBG:
            return GAME_REGION_PAL;
    }
}

u32 IsSceneMasterQuest(s16 sceneNum) {
    u8 mqMode = CVarGetInteger(CVAR_GENERAL("BetterDebugWarpScreenMQMode"), WARP_MODE_OVERRIDE_OFF);
    if (mqMode == WARP_MODE_OVERRIDE_MQ_AS_VANILLA) {
        return true;
    }

    if (mqMode == WARP_MODE_OVERRIDE_VANILLA_AS_MQ) {
        return false;
    }

    if (OTRGlobals::Instance->HasMasterQuest()) {
        if (!OTRGlobals::Instance->HasOriginal()) {
            return true;
        }

        if (IS_MASTER_QUEST) {
            return true;
        }

        if (IS_RANDO) {
            auto dungeon = OTRGlobals::Instance->gRandoContext->GetDungeons()->GetDungeonFromScene(sceneNum);
            if (dungeon != nullptr && dungeon->IsMQ()) {
                return true;
            }
        }
    }

    return false;
}

extern "C" uint32_t ResourceMgr_GameHasMasterQuest() {
    return OTRGlobals::Instance->HasMasterQuest();
}

extern "C" uint32_t ResourceMgr_GameHasOriginal() {
    return OTRGlobals::Instance->HasOriginal();
}

extern "C" uint32_t ResourceMgr_IsSceneMasterQuest(s16 sceneNum) {
    return IsSceneMasterQuest(sceneNum);
}

extern "C" uint32_t ResourceMgr_IsGameMasterQuest() {
    return gPlayState != NULL ? IsSceneMasterQuest(gPlayState->sceneNum) : 0;
}

extern "C" void ResourceMgr_LoadDirectory(const char* filter) {
    Ship::Context::GetInstance()->GetResourceManager()->LoadResources(filter);
}

extern "C" void ResourceMgr_DirtyDirectory(const char* resName) {
    Ship::Context::GetInstance()->GetResourceManager()->DirtyResources(resName);
}

extern "C" void ResourceMgr_UnloadResource(const char* resName) {
    std::string path = resName;
    if (path.substr(0, 7) == "__OTR__") {
        path = path.substr(7);
    }
    auto res = Ship::Context::GetInstance()->GetResourceManager()->UnloadResource(path);
}

// OTRTODO: There is probably a more elegant way to go about this...
// Kenix: This is definitely leaking memory when it's called.
extern "C" char** ResourceMgr_ListFiles(const char* searchMask, int* resultSize) {
    auto lst = Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->ListFiles({searchMask}, {});
    char** result = (char**)malloc(lst->size() * sizeof(char*));

    for (size_t i = 0; i < lst->size(); i++) {
        char* str = (char*)malloc(lst.get()[0][i].size() + 1);
        memcpy(str, lst.get()[0][i].data(), lst.get()[0][i].size());
        str[lst.get()[0][i].size()] = '\0';
        result[i] = str;
    }
    *resultSize = lst->size();

    return result;
}

extern "C" uint8_t ResourceMgr_FileExists(const char* filePath) {
    std::string path = filePath;
    if (path.substr(0, 7) == "__OTR__") {
        path = path.substr(7);
    }

    return ExtensionCache.contains(path);
}

extern "C" uint8_t ResourceMgr_FileAltExists(const char* filePath) {
    std::string path = filePath;
    if (path.substr(0, 7) == "__OTR__") {
        path = path.substr(7);
    }

    if (path.substr(0, 4) != "alt/") {
        path = "alt/" + path;
    }

    return ExtensionCache.contains(path);
}

extern "C" bool ResourceMgr_IsAltAssetsEnabled() {
    return Ship::Context::GetInstance()->GetResourceManager()->IsAltAssetsEnabled();
}

// Unloads a resource if an alternate version exists when alt assets are enabled
// The resource is only removed from the internal cache to prevent it from used in the next resource lookup
extern "C" void ResourceMgr_UnloadOriginalWhenAltExists(const char* resName) {
    if (ResourceMgr_IsAltAssetsEnabled() && ResourceMgr_FileAltExists((char*)resName)) {
        ResourceMgr_UnloadResource((char*)resName);
    }
}

std::shared_ptr<Ship::IResource> ResourceMgr_GetResourceByNameHandlingMQ(const char* path) {
    std::string Path = path;
    if (ResourceMgr_IsGameMasterQuest()) {
        size_t pos = 0;
        if ((pos = Path.find("/nonmq/", 0)) != std::string::npos) {
            Path.replace(pos, 7, "/mq/");
        }
    }
    return Ship::Context::GetInstance()->GetResourceManager()->LoadResource(Path.c_str());
}

extern "C" char* ResourceMgr_GetResourceDataByNameHandlingMQ(const char* path) {
    auto res = ResourceMgr_GetResourceByNameHandlingMQ(path);

    if (res == nullptr) {
        return nullptr;
    }

    return (char*)res->GetRawPointer();
}

extern "C" uint8_t ResourceMgr_TexIsRaw(const char* texPath) {
    auto res = std::static_pointer_cast<Fast::Texture>(ResourceMgr_GetResourceByNameHandlingMQ(texPath));
    return res->Flags & TEX_FLAG_LOAD_AS_RAW;
}

extern "C" uint8_t ResourceMgr_ResourceIsBackground(char* texPath) {
    auto res = ResourceMgr_GetResourceByNameHandlingMQ(texPath);
    return res->GetInitData()->Type == static_cast<uint32_t>(SOH::ResourceType::SOH_Background);
}

extern "C" char* ResourceMgr_LoadJPEG(char* data, size_t dataSize) {
    static char* finalBuffer = 0;

    if (finalBuffer == 0) {
        finalBuffer = (char*)malloc(dataSize);
    }

    int w;
    int h;
    int comp;

    unsigned char* pixels = stbi_load_from_memory((const unsigned char*)data, 320 * 240 * 2, &w, &h, &comp, STBI_rgb_alpha);
    //unsigned char* pixels = stbi_load_from_memory((const unsigned char*)data, 480 * 240 * 2, &w, &h, &comp, STBI_rgb_alpha);
    int idx = 0;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            uint16_t* bufferTest = (uint16_t*)finalBuffer;
            int pixelIdx = ((y * w) + x) * 4;

            uint8_t r = pixels[pixelIdx + 0] / 8;
            uint8_t g = pixels[pixelIdx + 1] / 8;
            uint8_t b = pixels[pixelIdx + 2] / 8;

            uint8_t alphaBit = pixels[pixelIdx + 3] != 0;

            uint16_t data = (r << 11) + (g << 6) + (b << 1) + alphaBit;

            finalBuffer[idx++] = (data & 0xFF00) >> 8;
            finalBuffer[idx++] = (data & 0x00FF);
        }
    }

    return (char*)finalBuffer;
}

extern "C" char* ResourceMgr_LoadTexOrDListByName(const char* filePath) {
    auto res = ResourceMgr_GetResourceByNameHandlingMQ(filePath);

    if (res->GetInitData()->Type == static_cast<uint32_t>(Fast::ResourceType::DisplayList)) {
        return (char*)&((std::static_pointer_cast<Fast::DisplayList>(res))->Instructions[0]);
    }

    if (res->GetInitData()->Type == static_cast<uint32_t>(SOH::ResourceType::SOH_Array)) {
        return (char*)(std::static_pointer_cast<SOH::Array>(res))->Vertices.data();
    }

    return (char*)ResourceMgr_GetResourceDataByNameHandlingMQ(filePath);
}

extern "C" char* ResourceMgr_LoadIfDListByName(const char* filePath) {
    auto res = ResourceMgr_GetResourceByNameHandlingMQ(filePath);

    if (res->GetInitData()->Type == static_cast<uint32_t>(Fast::ResourceType::DisplayList)) {
        return (char*)&((std::static_pointer_cast<Fast::DisplayList>(res))->Instructions[0]);
    }

    return nullptr;
}

extern "C" char* ResourceMgr_LoadPlayerAnimByName(const char* animPath) {
    auto anim = std::static_pointer_cast<SOH::PlayerAnimation>(ResourceMgr_GetResourceByNameHandlingMQ(animPath));

    return (char*)&anim->limbRotData[0];
}

extern "C" void ResourceMgr_PushCurrentDirectory(char* path) {
    gfx_push_current_dir(path);
}

extern "C" Gfx* ResourceMgr_LoadGfxByName(const char* path) {
    // When an alt resource exists for the DL, we need to unload the original asset
    // to clear the cache so the alt asset will be loaded instead
    // OTRTODO: If Alt loading over original cache is fixed, this line can most likely be removed
    ResourceMgr_UnloadOriginalWhenAltExists(path);

    auto res = std::static_pointer_cast<Fast::DisplayList>(ResourceMgr_GetResourceByNameHandlingMQ(path));
    return (Gfx*)&res->Instructions[0];
}

extern "C" uint8_t ResourceMgr_FileIsCustomByName(const char* path) {
    auto res = std::static_pointer_cast<Fast::DisplayList>(ResourceMgr_GetResourceByNameHandlingMQ(path));
    return res->GetInitData()->IsCustom;
}

typedef struct {
    int index;
    Gfx instruction;
} GfxPatch;

std::unordered_map<std::string, std::unordered_map<std::string, GfxPatch>> originalGfx;

// Attention! This is primarily for cosmetics & bug fixes. For things like mods and model replacement you should be using OTRs
// instead (When that is available). Index can be found using the commented out section below.
extern "C" void ResourceMgr_PatchGfxByName(const char* path, const char* patchName, int index, Gfx instruction) {
    auto res = std::static_pointer_cast<Fast::DisplayList>(
        Ship::Context::GetInstance()->GetResourceManager()->LoadResource(path));

    // Leaving this here for people attempting to find the correct Dlist index to patch
    /*if (strcmp("__OTR__objects/object_gi_longsword/gGiBiggoronSwordDL", path) == 0) {
        for (int i = 0; i < res->instructions.size(); i++) {
            Gfx* gfx = (Gfx*)&res->instructions[i];
            // Log all commands
            // SPDLOG_INFO("index:{} command:{}", i, gfx->words.w0 >> 24);
            // Log only SetPrimColors
            if (gfx->words.w0 >> 24 == 250) {
                SPDLOG_INFO("index:{} r:{} g:{} b:{} a:{}", i, _SHIFTR(gfx->words.w1, 24, 8), _SHIFTR(gfx->words.w1, 16, 8), _SHIFTR(gfx->words.w1, 8, 8), _SHIFTR(gfx->words.w1, 0, 8));
            }
        }
    }*/

    // Index refers to individual gfx words, which are half the size on 32-bit
    // if (sizeof(uintptr_t) < 8) {
    // index /= 2;
    // }

    // Do not patch custom assets as they most likely do not have the same instructions as authentic assets
    if (res->GetInitData()->IsCustom) {
        return;
    }

    Gfx* gfx = (Gfx*)&res->Instructions[index];

    if (!originalGfx.contains(path) || !originalGfx[path].contains(patchName)) {
        originalGfx[path][patchName] = {
            index,
            *gfx
        };
    }

    *gfx = instruction;
}

extern "C" void ResourceMgr_PatchGfxCopyCommandByName(const char* path, const char* patchName, int destinationIndex, int sourceIndex) {
    auto res = std::static_pointer_cast<Fast::DisplayList>(
        Ship::Context::GetInstance()->GetResourceManager()->LoadResource(path));

    // Do not patch custom assets as they most likely do not have the same instructions as authentic assets
    if (res->GetInitData()->IsCustom) {
        return;
    }

    Gfx* destinationGfx = (Gfx*)&res->Instructions[destinationIndex];
    Gfx sourceGfx = *(Gfx*)&res->Instructions[sourceIndex];

    if (!originalGfx.contains(path) || !originalGfx[path].contains(patchName)) {
        originalGfx[path][patchName] = {
            destinationIndex,
            *destinationGfx
        };
    }

    *destinationGfx = sourceGfx;
}

extern "C" void ResourceMgr_UnpatchGfxByName(const char* path, const char* patchName) {
    if (originalGfx.contains(path) && originalGfx[path].contains(patchName)) {
        auto res = std::static_pointer_cast<Fast::DisplayList>(
            Ship::Context::GetInstance()->GetResourceManager()->LoadResource(path));

        Gfx* gfx = (Gfx*)&res->Instructions[originalGfx[path][patchName].index];
        *gfx = originalGfx[path][patchName].instruction;

        originalGfx[path].erase(patchName);
    }
}

extern "C" char* ResourceMgr_LoadArrayByName(const char* path) {
    auto res = std::static_pointer_cast<SOH::Array>(ResourceMgr_GetResourceByNameHandlingMQ(path));

    return (char*)res->Scalars.data();
}

// Return of LoadArrayByNameAsVec3s must be freed by the caller
extern "C" char* ResourceMgr_LoadArrayByNameAsVec3s(const char* path) {
    auto res = std::static_pointer_cast<SOH::Array>(ResourceMgr_GetResourceByNameHandlingMQ(path));

    Vec3s* data = (Vec3s*)malloc(sizeof(Vec3s) * res->Scalars.size());

    for (size_t i = 0; i < res->Scalars.size(); i += 3) {
        data[(i / 3)].x = res->Scalars[i + 0].s16;
        data[(i / 3)].y = res->Scalars[i + 1].s16;
        data[(i / 3)].z = res->Scalars[i + 2].s16;
    }

    return (char*)data;
}

extern "C" CollisionHeader* ResourceMgr_LoadColByName(const char* path) {
    return (CollisionHeader*)ResourceGetDataByName(path);
}

extern "C" Vtx* ResourceMgr_LoadVtxByName(char* path) {
    return (Vtx*)ResourceGetDataByName(path);
}

extern "C" SequenceData ResourceMgr_LoadSeqByName(const char* path) {
    SequenceData* sequence = (SequenceData*)ResourceGetDataByName(path);
    return *sequence;
}

extern "C" SoundFontSample* ResourceMgr_LoadAudioSample(const char* path) {
    return (SoundFontSample*)ResourceGetDataByName(path);
}

extern "C" SoundFont* ResourceMgr_LoadAudioSoundFont(const char* path) {
    return (SoundFont*)ResourceGetDataByName(path);
}

extern "C" int ResourceMgr_OTRSigCheck(char* imgData) {
    uintptr_t i = (uintptr_t)(imgData);

    // if (i == 0xD9000000 || i == 0xE7000000 || (i & 1) == 1)
    if ((i & 1) == 1)
        return 0;

    // if ((i & 0xFF000000) != 0xAB000000 && (i & 0xFF000000) != 0xCD000000 && i != 0) {
    if (i != 0) {
        if (
            imgData[0] == '_' &&
            imgData[1] == '_' &&
            imgData[2] == 'O' &&
            imgData[3] == 'T' &&
            imgData[4] == 'R' &&
            imgData[5] == '_' &&
            imgData[6] == '_'
            ) {
            return 1;
        }
    }

    return 0;
}

extern "C" AnimationHeaderCommon* ResourceMgr_LoadAnimByName(const char* path) {
    return (AnimationHeaderCommon*)ResourceGetDataByName(path);
}

extern "C" SkeletonHeader* ResourceMgr_LoadSkeletonByName(const char* path, SkelAnime* skelAnime) {
    std::string pathStr = std::string(path);
    static const std::string sOtr = "__OTR__";

    if (pathStr.starts_with(sOtr)) {
        pathStr = pathStr.substr(sOtr.length());
    }

    bool isAlt = ResourceMgr_IsAltAssetsEnabled();

    if (isAlt) {
        pathStr = Ship::IResource::gAltAssetPrefix + pathStr;
    }

    SkeletonHeader* skelHeader = (SkeletonHeader*)ResourceGetDataByName(pathStr.c_str());

    // If there isn't an alternate model, load the regular one
    if (isAlt && skelHeader == NULL) {
        skelHeader = (SkeletonHeader*)ResourceGetDataByName(path);
    }

    // This function is only called when a skeleton is initialized.
    // Therefore we can take this oppurtunity to take note of the Skeleton that is created...
    if (skelAnime != nullptr) {
        auto stringPath = std::string(path);
        SOH::SkeletonPatcher::RegisterSkeleton(stringPath, skelAnime);
    }

    return skelHeader;
}

extern "C" void ResourceMgr_UnregisterSkeleton(SkelAnime* skelAnime) {
    if (skelAnime != nullptr) {
        SOH::SkeletonPatcher::UnregisterSkeleton(skelAnime);
    }
}

extern "C" void ResourceMgr_ClearSkeletons() {
    SOH::SkeletonPatcher::ClearSkeletons();
}

extern "C" s32* ResourceMgr_LoadCSByName(const char* path) {
    return (s32*)ResourceMgr_GetResourceDataByNameHandlingMQ(path);
}

bool IsSharedScene(int16_t sceneNum) {
    return sceneNum != SCENE_DEKU_TREE &&
           sceneNum != SCENE_DODONGOS_CAVERN &&
           sceneNum != SCENE_JABU_JABU &&
           sceneNum != SCENE_FOREST_TEMPLE &&
           sceneNum != SCENE_FIRE_TEMPLE &&
           sceneNum != SCENE_WATER_TEMPLE &&
           sceneNum != SCENE_SPIRIT_TEMPLE &&
           sceneNum != SCENE_SHADOW_TEMPLE &&
           sceneNum != SCENE_BOTTOM_OF_THE_WELL &&
           sceneNum != SCENE_ICE_CAVERN &&
           sceneNum != SCENE_GERUDO_TRAINING_GROUND &&
           sceneNum != SCENE_INSIDE_GANONS_CASTLE;
}

std::string GetSceneRootPath(int16_t sceneNum, bool alt = true) {
    std::string sceneName = gSceneTable[sceneNum].sceneFile.fileName;
    std::string path = fmt::format("{}scenes/shared/{}/", (alt ? "alt/" : ""), sceneName);
    if (!IsSharedScene(sceneNum)) {
        size_t pos = path.find("/shared/", 0);
        if (IS_MASTER_QUEST || (IS_RANDO && OTRGlobals::Instance->gRandoContext->GetDungeons()->GetDungeonFromScene(sceneNum)->IsMQ())) {
            path.replace(pos, 8, "/mq/");
        } else {
            path.replace(pos, 8, "/nonmq/");
        }
    }
    return path;
}

// Return full path mask for scene assets
std::string GetScenePathMask(int16_t sceneNum, bool alt = true) {
    return GetSceneRootPath(sceneNum, alt) + "*";
}

std::string GetSceneFilePath(int16_t sceneNum, bool alt = true) {
    return GetSceneRootPath(sceneNum, alt) + gSceneTable[sceneNum].sceneFile.fileName;
}

std::array<std::unordered_set<std::string>, SCENE_TESTROOM + 1> sceneObjects;

void LoadSceneResourcesProcess(int16_t sceneNum) {
    auto play = gPlayState;
    for (auto objectName : sceneObjects[sceneNum]) {
        if (!sceneObjects[play->sceneNum].contains(objectName)) {
            OTRGlobals::Instance->context->GetResourceManager()->LoadResources("alt/objects/" + objectName + "/*");
        }
    }
    OTRGlobals::Instance->context->GetResourceManager()->LoadResources(GetScenePathMask(sceneNum));
}

// Iterate over scene object/actor commands if not already done so, and load the scene and object assets
extern "C" void ResourceMgr_LoadAllSceneResources(int16_t sceneNum, bool now) {
    helperThreads->submit_task(std::bind(LoadSceneResourcesProcess, sceneNum));
}

extern "C" void ResourceMgr_RegisterUnloadSceneAssets(s16 prevScene) {
    unloadScene = prevScene;
}

// Unload previously determined scene assets
void UnloadSceneAssetsProcess() {
    for (auto objectName : sceneObjects[unloadScene]) {
        if (!sceneObjects[gPlayState->sceneNum].contains(objectName)) {
            std::string objectPath = fmt::format("alt/objects/{}/*", objectName);
            OTRGlobals::Instance->context->GetResourceManager()->UnloadResources(objectPath);
        }
    }
    OTRGlobals::Instance->context->GetResourceManager()->UnloadResources(GetScenePathMask(unloadScene));
    OTRGlobals::Instance->context->GetResourceManager()->UnloadResources("alt/textures/vr_holy*");
    unloadScene = -1;
}

// Start unload loops on a thread for performance purposes
extern "C" void ResourceMgr_UnloadSceneAssets() {
    if (unloadScene != -1) {
        helperThreads->submit_task(UnloadSceneAssetsProcess);
    }
}

// Persisted assets never unload, generally because they're used in multiple places. These include things like
// audio assets, icons, items, font, gameplay*keep objects, title cards, and interior assets (for now)
void ResourceMgr_LoadDelayedPersistentAltAssets() {
    // Load sound effects first for title screen "Press Start" and pause sounds. These are loaded
    // before the alt assets to prevent load lock for the audio itself
    static std::list<std::string> textureIncludes = { "textures/*", "overlays/*", "code/*", "misc/*", "text/*", "objects/*", "scenes/*",
        "alt/textures/parameter*", "alt/textures/icon*", "alt/textures/item*", "alt/textures/font*",
        "alt/objects/gameplay_*", "alt/overlays/*", "alt/code/*", "alt/textures/*", 
        };
    static std::list<std::string> textureExcludes = { "alt/textures/vr_holy*", "alt/textures/vr_cloud*", "alt/textures/vr_fine*", "textures/buttons/*" };
    Ship::Context::GetInstance()->GetResourceManager()->LoadResourcesAsync({textureIncludes, textureExcludes, 0, nullptr});
}

static int lastSkyboxLoad = -1;

void LoadSkyBoxProcess(TimeOfDay timeIndex, bool fileSelect) {
    std::string mask = fmt::format("alt/textures/vr_fine{}*", static_cast<uint8_t>(timeIndex));
    Ship::Context::GetInstance()->GetResourceManager()->LoadResources(mask);
    if (!fileSelect) {
        std::string mask = fmt::format("alt/textures/vr_cloud{}*", static_cast<uint8_t>(timeIndex));
        Ship::Context::GetInstance()->GetResourceManager()->LoadResources(mask);
    }
    lastSkyboxLoad = timeIndex;
}
// Load regular and cloudy skyboxes for specified TimeOfDay
extern "C" void ResourceMgr_LoadSkyBox(TimeOfDay timeIndex, bool fileSelect) {
    helperThreads->submit_task(std::bind(LoadSkyBoxProcess, timeIndex, fileSelect));
}

static int lastSkyboxUnload = -1;
// Unload skyboxes for specified TimeOfDay
extern "C" void ResourceMgr_UnloadSkyBox(TimeOfDay timeIndex) {
    helperThreads->submit_task([timeIndex]() -> void {
        std::string mask = fmt::format("alt/textures/vr_cloud{}*", static_cast<uint8_t>(timeIndex));
        ResourceUnloadDirectory(mask.c_str());
        mask = fmt::format("alt/textures/vr_fine{}*", static_cast<uint8_t>(timeIndex));
        ResourceUnloadDirectory(mask.c_str());
    });
}

// Setup initial preload based on Fast File Select and Save Index options
extern "C" void ResourceMgr_LoadPersistentAltAssets() {
    bool skipTitle = CVarGetInteger(CVAR_DEVELOPER_TOOLS("SkipLogoTitle"), 0);
    int fastFile = CVarGetInteger(CVAR_DEVELOPER_TOOLS("SaveFileID"), 0);

    if (!skipTitle) {
        ResourceLoadDirectoryAsync("alt/textures/nintendo_rogo_static/*");
        // Title screen/hyrule field
        ResourceLoadDirectoryAsync("alt/scenes/*/spot00*");
        // Title logos
        ResourceLoadDirectoryAsync("alt/objects/object_mag/*");
    } else if (skipTitle && fastFile == 4) {
        Ship::Context::GetInstance()->GetResourceManager()->LoadResourcesAsync({
            {"alt/overlays/ovl_file_choose/*", "alt/textures/title_static/*", "alt/objects/gameplay_keep/*", "alt/textures/vr_fine3*", "alt/textures/vr_fine0*"},
            {}, 0, nullptr});
    } else if (skipTitle && fastFile < 3) {
        ResourceLoadDirectoryAsync("alt/textures/icon*");
        ResourceLoadDirectoryAsync("alt/textures/do_action_static/*");
        ResourceLoadDirectoryAsync("alt/textures/map*");
        ResourceLoadDirectoryAsync("alt/textures/parameter_static/*");
    }
    ResourceMgr_LoadDelayedPersistentAltAssets();
}

// Just to make sure the cloudy skyboxes are loaded across transitions for things like Song of Storms
extern "C" void ResourceMgr_SceneInitSkybox() {
    if (gSaveContext.dayTime > SUNRISE_BEGINS && gSaveContext.dayTime < DAY_BEGINS) {
        ResourceMgr_LoadSkyBox(TOD_Sunrise, false);
        ResourceMgr_LoadSkyBox(TOD_Night, false);
        ResourceMgr_LoadSkyBox(TOD_Day, false);
    } else if (gSaveContext.dayTime > DAY_BEGINS && gSaveContext.dayTime < SUNSET_BEGINS) {
        ResourceMgr_LoadSkyBox(TOD_Day, false);
    } else if (gSaveContext.dayTime > SUNSET_BEGINS && gSaveContext.dayTime < NIGHT_BEGINS) {
        ResourceMgr_LoadSkyBox(TOD_Day, false);
        ResourceMgr_LoadSkyBox(TOD_Sunset, false);
        ResourceMgr_LoadSkyBox(TOD_Night, false);
    } else if (gSaveContext.dayTime > NIGHT_BEGINS || gSaveContext.dayTime < SUNRISE_BEGINS) {
        ResourceMgr_LoadSkyBox(TOD_Night, false);
    }
}

// These are absolutely arbitrary numbers, and are my best guess for lead time needed to load skyboxes for the given time of day
uint16_t skyboxLoadTimes[2][4] = { { SUNRISE_BEGINS - 3000, DAY_BEGINS - 3000, SUNSET_BEGINS - 3000, NIGHT_BEGINS - 3000 }, { 63000, 16000, 32000, 49000 } };
// These are what were needed to be able to unload the skyboxes without hiccups, since they're held onto for a bit after transitions are done
uint16_t skyboxUnloadTimes[2][4] = { { DAY_BEGINS + 4000, SUNSET_BEGINS + 4000, NIGHT_BEGINS + 4000, SUNRISE_BEGINS + 4000 }, { 32000, 49000, 63000, 16000 } };

// Used to preload and unload skyboxes for time-based skybox changes
extern "C" void ResourceMgr_CheckLoadSkybox(bool fileSelect) {
    if (gSaveContext.skyboxTime > skyboxLoadTimes[fileSelect][TOD_Sunrise] && gSaveContext.skyboxTime < (skyboxLoadTimes[fileSelect][TOD_Sunrise] + 800)) {
        if (lastSkyboxLoad != 0) {
            lastSkyboxLoad = 0;
            ResourceMgr_LoadSkyBox(TOD_Sunrise, fileSelect);
        }
    } else if (gSaveContext.skyboxTime > skyboxLoadTimes[fileSelect][TOD_Day] && gSaveContext.skyboxTime < (skyboxLoadTimes[fileSelect][TOD_Day] + 800)) {
        if (lastSkyboxLoad != 1) {
            lastSkyboxLoad = 1;
            ResourceMgr_LoadSkyBox(TOD_Day, fileSelect);
        }
    } else if (gSaveContext.skyboxTime > skyboxLoadTimes[fileSelect][TOD_Sunset] && gSaveContext.skyboxTime < (skyboxLoadTimes[fileSelect][TOD_Sunset] + 800)) {
        if (lastSkyboxLoad != 2) {
            lastSkyboxLoad = 2;
            ResourceMgr_LoadSkyBox(TOD_Sunset, fileSelect);
        }
    } else if (gSaveContext.skyboxTime > skyboxLoadTimes[fileSelect][TOD_Night] && gSaveContext.skyboxTime < (skyboxLoadTimes[fileSelect][TOD_Night] + 800)) {
        if (lastSkyboxLoad != 3) {
            lastSkyboxLoad = 3;
            ResourceMgr_LoadSkyBox(TOD_Night, fileSelect);
        }
    }

    if (gSaveContext.skyboxTime > skyboxUnloadTimes[fileSelect][TOD_Sunrise] && gSaveContext.skyboxTime < (skyboxUnloadTimes[fileSelect][TOD_Sunrise] + 800)) {
        if (lastSkyboxUnload != 0) {
            lastSkyboxUnload = 0;
            ResourceMgr_UnloadSkyBox(TOD_Sunrise);
        }
    } else if (gSaveContext.skyboxTime > skyboxUnloadTimes[fileSelect][TOD_Day] && gSaveContext.skyboxTime < (skyboxUnloadTimes[fileSelect][TOD_Day] + 800)) {
        if (lastSkyboxUnload != 1) {
            lastSkyboxUnload = 1;
            ResourceMgr_UnloadSkyBox(TOD_Day);
        }
    } else if (gSaveContext.skyboxTime > skyboxUnloadTimes[fileSelect][TOD_Sunset] && gSaveContext.skyboxTime < (skyboxUnloadTimes[fileSelect][TOD_Sunset] + 800)) {
        if (lastSkyboxUnload != 2) {
            lastSkyboxUnload = 2;
            ResourceMgr_UnloadSkyBox(TOD_Sunset);
        }
    } else if (gSaveContext.skyboxTime > skyboxUnloadTimes[fileSelect][TOD_Night] && gSaveContext.skyboxTime < (skyboxUnloadTimes[fileSelect][TOD_Night] + 800)) {
        if (lastSkyboxUnload != 3) {
            lastSkyboxUnload = 3;
            ResourceMgr_UnloadSkyBox(TOD_Night);
        }
    }
}

extern "C" void ResourceMgr_Init() {
    helperThreads = std::make_shared<BS::thread_pool>(1);
    for (int16_t sceneNum = 0; sceneNum <= SCENE_OUTSIDE_GANONS_CASTLE; sceneNum++) {
        if (sceneObjects[sceneNum].empty()) {
            SOH::SceneCommandID cmdCode;
            std::string scenePath = GetSceneFilePath(sceneNum, false);
            auto scene = (SOH::Scene*)Ship::Context::GetInstance()->GetResourceManager()->LoadResource(scenePath.c_str()).get();
            for (auto sceneCmd : scene->commands) {
                if (sceneCmd->cmdId == SOH::SceneCommandID::SetRoomList) {
                    auto setRoomListCmd = std::dynamic_pointer_cast<SOH::SetRoomList>(sceneCmd);
                    for (auto room : setRoomListCmd->rooms) {
                        auto roomScene = (SOH::Scene*)Ship::Context::GetInstance()->GetResourceManager()->LoadResource(room.fileName).get();
                        for (auto roomSceneCmd : roomScene->commands) {
                            if (roomSceneCmd->cmdId == SOH::SceneCommandID::SetObjectList) {
                                auto setObjectCmd = std::dynamic_pointer_cast<SOH::SetObjectList>(roomSceneCmd);
                                for (auto objectId : setObjectCmd->objects) {
                                    std::string objectName = gObjectTable[objectId].fileName;
                                    sceneObjects[sceneNum].insert(objectName);
                                }
                            } else if (roomSceneCmd->cmdId == SOH::SceneCommandID::SetActorList) {
                                auto setActorCmd = std::dynamic_pointer_cast<SOH::SetActorList>(roomSceneCmd);
                                if (setActorCmd->numActors > 0) {
                                    for (uint16_t i = 0; i < setActorCmd->numActors; i++) {
                                        auto actorEntry = (ActorEntry*)setActorCmd->GetRawPointer();
                                        std::string objectName = gObjectTable[ActorDB::Instance->RetrieveEntry(actorEntry->id).entry.objectId].fileName;
                                        sceneObjects[sceneNum].insert(objectName);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

extern "C" void ResourceMgr_RegisterHooks() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        ResourceMgr_CheckLoadSkybox(false);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](uint32_t sceneNum) {
        ResourceMgr_SceneInitSkybox();
        ResourceMgr_UnloadSceneAssets();
    });
}
