#include "soh/Network/Anchor/AnchorModRegistry.h"
#include <filesystem>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <ship/Context.h>
#include <ship/resource/ResourceManager.h>
#include <ship/resource/Resource.h>
#include <ship/resource/archive/O2rArchive.h>
#include <ship/utils/StringHelper.h>
#include "soh/OTRGlobals.h"
#include "soh/resource/type/Skeleton.h"
#include "soh/cvar_prefixes.h"
#include <objects/object_link_child/object_link_child.h>
#include <objects/object_link_boy/object_link_boy.h>
#include <fast/resource/ResourceType.h>
#include <fast/resource/type/DisplayList.h>

#ifdef INCLUDE_MPQ_SUPPORT
#include <ship/resource/archive/OtrArchive.h>
#endif

extern "C" {
#include "macros.h"
#include "functions.h"
#if defined(MODDING) || defined(_MSC_VER) || defined(__GNUC__)
extern void* sEyeTextures[2][8];
extern void* sMouthTextures[2][4];
#endif
extern u8 sEyeMouthIndexes[][2];
}

namespace {
constexpr const char* kAnchorModMetadataPath = "mod.json";
constexpr const char* kEnabledModsCvarName = CVAR_SETTING("EnabledMods");
constexpr const char* kEnabledModsSeparator = "|";

struct AnchorModelEntry {
    std::shared_ptr<Ship::Archive> archive;
    std::shared_ptr<SOH::Skeleton> adultSkeleton;
    std::shared_ptr<SOH::Skeleton> childSkeleton;
    std::string resolvedAdultPath;
    std::string resolvedChildPath;
};

std::unordered_map<std::string, AnchorModelEntry> sAnchorModelsById;
std::string sLocalModelId;
bool sInitialized = false;

struct AnchorModelOverrideState {
    bool active = false;
    std::string modelId;
    int32_t linkAge = LINK_AGE_CHILD;
};

AnchorModelOverrideState sAnchorModelOverride;

constexpr const char* kAdultEyeTextureNames[] = { "gLinkAdultEyesOpenTex", "gLinkAdultEyesHalfTex",
                                                  "gLinkAdultEyesClosedfTex", "gLinkAdultEyesRollLeftTex",
                                                  "gLinkAdultEyesRollRightTex", "gLinkAdultEyesShockTex",
                                                  "gLinkAdultEyesUnk1Tex", "gLinkAdultEyesUnk2Tex" };
constexpr const char* kChildEyeTextureNames[] = { "gLinkChildEyesOpenTex", "gLinkChildEyesHalfTex",
                                                  "gLinkChildEyesClosedfTex", "gLinkChildEyesRollLeftTex",
                                                  "gLinkChildEyesRollRightTex", "gLinkChildEyesShockTex",
                                                  "gLinkChildEyesUnk1Tex", "gLinkChildEyesUnk2Tex" };
constexpr const char* kAdultMouthTextureNames[] = { "gLinkAdultMouth1Tex", "gLinkAdultMouth2Tex", "gLinkAdultMouth3Tex",
                                                    "gLinkAdultMouth4Tex" };
constexpr const char* kChildMouthTextureNames[] = { "gLinkChildMouth1Tex", "gLinkChildMouth2Tex", "gLinkChildMouth3Tex",
                                                    "gLinkChildMouth4Tex" };

std::unordered_map<std::string, std::shared_ptr<Ship::IResource>> sCustomTextureCache;

std::string StripOtrPrefix(const std::string& path) {
    if (path.starts_with("__OTR__")) {
        return path.substr(7);
    }
    return path;
}

std::string GetBaseName(const std::string& path) {
    size_t slashPos = path.find_last_of('/');
    if (slashPos == std::string::npos) {
        return path;
    }
    return path.substr(slashPos + 1);
}

bool IsAnchorFlipbookTextureName(const std::string& baseName) {
    return baseName.starts_with("gLinkAdultEyes") || baseName.starts_with("gLinkChildEyes") ||
           baseName.starts_with("gLinkAdultMouth") || baseName.starts_with("gLinkChildMouth");
}

int ClampIndex(int value, int maxValue) {
    if (value < 0) {
        return 0;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

void* LoadCustomTexture(const std::string& modelId, int32_t linkAge, const char* baseName) {
    if (modelId.empty() || baseName == nullptr) {
        return nullptr;
    }

    auto archive = AnchorModRegistry::FindArchiveById(modelId);
    if (archive == nullptr) {
        return nullptr;
    }

    std::string folder = linkAge == LINK_AGE_ADULT ? (modelId + "_adult") : (modelId + "_child");
    std::string path = "objects/object_anchor_models/" + folder + "/" + baseName;
    std::string cacheKey = archive->GetPath() + "|" + path;

    auto cached = sCustomTextureCache.find(cacheKey);
    if (cached != sCustomTextureCache.end()) {
        return cached->second ? cached->second->GetRawPointer() : nullptr;
    }

    auto context = Ship::Context::GetInstance();
    if (context == nullptr || context->GetResourceManager() == nullptr) {
        return nullptr;
    }

    auto resource = context->GetResourceManager()->LoadResource(Ship::ResourceIdentifier(path.c_str(), 0, archive), true);
    if (resource == nullptr) {
        sCustomTextureCache.emplace(cacheKey, nullptr);
        return nullptr;
    }

    sCustomTextureCache.emplace(cacheKey, resource);
    return resource->GetRawPointer();
}

bool HasValidModExtension(const std::filesystem::path& path) {
    std::string extension = path.extension().generic_string();
    if (StringHelper::IEquals(extension, ".o2r")) {
        return true;
    }
#ifdef INCLUDE_MPQ_SUPPORT
    if (StringHelper::IEquals(extension, ".otr")) {
        return true;
    }
#endif
    return false;
}

std::string ResolveModsFolder() {
    std::string path = Ship::Context::LocateFileAcrossAppDirs("mods", appShortName);
    if (path.empty()) {
        path = Ship::Context::GetPathRelativeToAppDirectory("mods", appShortName);
    }

    return path;
}

std::shared_ptr<Ship::Archive> CreateArchiveForPath(const std::filesystem::path& path) {
    if (!HasValidModExtension(path)) {
        return nullptr;
    }

    std::shared_ptr<Ship::Archive> archive;
    std::string extension = path.extension().generic_string();
    if (StringHelper::IEquals(extension, ".o2r")) {
        archive = std::make_shared<Ship::O2rArchive>(path.generic_string());
    }
#ifdef INCLUDE_MPQ_SUPPORT
    else if (StringHelper::IEquals(extension, ".otr")) {
        archive = std::make_shared<Ship::OtrArchive>(path.generic_string());
    }
#endif

    if (archive == nullptr) {
        return nullptr;
    }

    if (!archive->Open()) {
        return nullptr;
    }

    return archive;
}

std::string ReadAnchorModId(const std::shared_ptr<Ship::Archive>& archive) {
    if (archive == nullptr) {
        return "";
    }

    auto file = archive->LoadFile(kAnchorModMetadataPath);
    if (file == nullptr || !file->IsLoaded || file->Buffer == nullptr) {
        return "";
    }

    std::string jsonText(file->Buffer->begin(), file->Buffer->end());
    auto parsed = nlohmann::json::parse(jsonText, nullptr, false);
    if (parsed.is_discarded() || !parsed.contains("id") || !parsed["id"].is_string()) {
        return "";
    }

    return parsed["id"].get<std::string>();
}

void ScanAnchorMods() {
    sAnchorModelsById.clear();

    std::string modsPath = ResolveModsFolder();
    if (modsPath.empty() || !std::filesystem::exists(modsPath)) {
        return;
    }

    for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(
             modsPath, std::filesystem::directory_options::follow_directory_symlink |
                           std::filesystem::directory_options::skip_permission_denied)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        const std::filesystem::path& path = entry.path();
        if (!HasValidModExtension(path)) {
            continue;
        }

        auto archive = CreateArchiveForPath(path);
        if (archive == nullptr) {
            continue;
        }

        std::string id = ReadAnchorModId(archive);
        if (id.empty() || sAnchorModelsById.contains(id)) {
            continue;
        }

        AnchorModelEntry entryData;
        entryData.archive = archive;
        sAnchorModelsById.emplace(id, std::move(entryData));
    }
}

std::unordered_map<std::string, std::filesystem::path> BuildModPathMap(const std::string& modsPath) {
    std::unordered_map<std::string, std::filesystem::path> modPaths;
    if (modsPath.empty() || !std::filesystem::exists(modsPath)) {
        return modPaths;
    }

    for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(
             modsPath, std::filesystem::directory_options::follow_directory_symlink |
                           std::filesystem::directory_options::skip_permission_denied)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        const std::filesystem::path& path = entry.path();
        if (!HasValidModExtension(path)) {
            continue;
        }

        std::string fileName = path.filename().generic_string();
        auto dotPos = fileName.rfind(".");
        if (dotPos == std::string::npos) {
            continue;
        }

        std::string modName = fileName.substr(0, dotPos);
        if (!modName.empty() && !modPaths.contains(modName)) {
            modPaths.emplace(modName, path);
        }
    }

    return modPaths;
}

std::string FindLocalModelId() {
    std::string enabledMods = CVarGetString(kEnabledModsCvarName, "");
    if (enabledMods.empty()) {
        return "";
    }

    std::vector<std::string> enabledList = StringHelper::Split(enabledMods, kEnabledModsSeparator);
    if (enabledList.empty()) {
        return "";
    }

    auto modPaths = BuildModPathMap(ResolveModsFolder());
    if (modPaths.empty()) {
        return "";
    }

    for (const std::string& modName : enabledList) {
        auto it = modPaths.find(modName);
        if (it == modPaths.end()) {
            continue;
        }

        auto archive = CreateArchiveForPath(it->second);
        if (archive == nullptr) {
            continue;
        }

        std::string id = ReadAnchorModId(archive);
        if (!id.empty()) {
            return id;
        }
    }

    return "";
}

std::string BuildAnchorSkeletonPath(const std::string& id, int32_t linkAge) {
    std::string folder = linkAge == LINK_AGE_ADULT ? (id + "_adult") : (id + "_child");
    std::string path = "objects/object_anchor_models/" + folder + "/";
    path += linkAge == LINK_AGE_ADULT ? "gLinkAdultSkel" : "gLinkChildSkel";
    return path;
}

std::string FindSkeletonPathInArchive(const std::shared_ptr<Ship::Archive>& archive, int32_t linkAge) {
    if (archive == nullptr) {
        return "";
    }

    const char* filter = linkAge == LINK_AGE_ADULT
                             ? "objects/object_anchor_models/*_adult/gLinkAdultSkel"
                             : "objects/object_anchor_models/*_child/gLinkChildSkel";
    auto matches = archive->ListFiles(filter);
    if (matches == nullptr || matches->empty()) {
        return "";
    }

    return matches->begin()->second;
}

std::shared_ptr<SOH::Skeleton> LoadSkeletonFromArchive(AnchorModelEntry& entry, const std::string& basePath) {
    auto context = Ship::Context::GetInstance();
    if (context == nullptr || context->GetResourceManager() == nullptr) {
        return nullptr;
    }

    std::vector<std::string> candidates;
    candidates.push_back(basePath);
    candidates.push_back("__OTR__" + basePath);

    for (const std::string& candidate : candidates) {
        auto resource = context->GetResourceManager()->LoadResource(
            Ship::ResourceIdentifier(candidate.c_str(), 0, entry.archive), true);
        auto skeleton = std::dynamic_pointer_cast<SOH::Skeleton>(resource);
        if (skeleton != nullptr) {
            return skeleton;
        }
    }

    return nullptr;
}

std::shared_ptr<SOH::Skeleton> GetModelSkeleton(const std::string& id, int32_t linkAge) {
    if (id.empty()) {
        return nullptr;
    }

    auto it = sAnchorModelsById.find(id);
    if (it == sAnchorModelsById.end()) {
        return nullptr;
    }

    AnchorModelEntry& entry = it->second;
    if (linkAge == LINK_AGE_ADULT) {
        if (entry.adultSkeleton == nullptr) {
            if (!entry.resolvedAdultPath.empty()) {
                entry.adultSkeleton = LoadSkeletonFromArchive(entry, entry.resolvedAdultPath);
            }
            if (entry.adultSkeleton == nullptr) {
                entry.adultSkeleton = LoadSkeletonFromArchive(entry, BuildAnchorSkeletonPath(id, linkAge));
            }
            if (entry.adultSkeleton == nullptr) {
                entry.resolvedAdultPath = FindSkeletonPathInArchive(entry.archive, linkAge);
                if (!entry.resolvedAdultPath.empty()) {
                    entry.adultSkeleton = LoadSkeletonFromArchive(entry, entry.resolvedAdultPath);
                }
            }
        }
        return entry.adultSkeleton;
    }

    if (entry.childSkeleton == nullptr) {
        if (!entry.resolvedChildPath.empty()) {
            entry.childSkeleton = LoadSkeletonFromArchive(entry, entry.resolvedChildPath);
        }
        if (entry.childSkeleton == nullptr) {
            entry.childSkeleton = LoadSkeletonFromArchive(entry, BuildAnchorSkeletonPath(id, linkAge));
        }
        if (entry.childSkeleton == nullptr) {
            entry.resolvedChildPath = FindSkeletonPathInArchive(entry.archive, linkAge);
            if (!entry.resolvedChildPath.empty()) {
                entry.childSkeleton = LoadSkeletonFromArchive(entry, entry.resolvedChildPath);
            }
        }
    }
    return entry.childSkeleton;
}

const char* GetLinkSkeletonPath(int32_t linkAge) {
    if (linkAge == LINK_AGE_ADULT) {
        return gLinkAdultSkel;
    }

    return gLinkChildSkel;
}

bool ApplySkeletonToSkelAnime(SkelAnime* skelAnime, SOH::Skeleton* newSkel) {
    if (skelAnime == nullptr || newSkel == nullptr) {
        return false;
    }

    int32_t newLimbCount = newSkel->limbCount + 1;
    if (skelAnime->limbCount != 0 && skelAnime->limbCount != newLimbCount) {
        return false;
    }

    skelAnime->limbCount = newLimbCount;
    skelAnime->skeleton = newSkel->skeletonData.skeletonHeader.segment;
    uintptr_t skelPtr = (uintptr_t)newSkel->GetPointer();
    memcpy(&skelAnime->skeletonHeader, &skelPtr, sizeof(uintptr_t));
    return true;
}
} // namespace

void AnchorModRegistry::Init() {
    if (sInitialized) {
        return;
    }

    auto context = Ship::Context::GetInstance();
    if (context == nullptr || context->GetConsoleVariables() == nullptr) {
        return;
    }

    sLocalModelId = FindLocalModelId();
    ScanAnchorMods();
    sInitialized = true;
}

const std::string& AnchorModRegistry::GetLocalModelId() {
    Init();
    return sLocalModelId;
}

std::shared_ptr<Ship::Archive> AnchorModRegistry::FindArchiveById(const std::string& id) {
    Init();
    if (id.empty()) {
        return nullptr;
    }

    auto it = sAnchorModelsById.find(id);
    if (it == sAnchorModelsById.end()) {
        return nullptr;
    }

    return it->second.archive;
}

void AnchorModRegistry::SetAnchorModelOverride(const std::string& modelId, int32_t linkAge) {
    sAnchorModelOverride.active = !modelId.empty();
    sAnchorModelOverride.modelId = modelId;
    sAnchorModelOverride.linkAge = linkAge;
}

void AnchorModRegistry::ClearAnchorModelOverride() {
    sAnchorModelOverride.active = false;
    sAnchorModelOverride.modelId.clear();
}

void* AnchorModRegistry::TryLoadAnchorOverride(const char* path) {
    if (path == nullptr || !sAnchorModelOverride.active || sAnchorModelOverride.modelId.empty()) {
        return nullptr;
    }

    std::string pathStr = StripOtrPrefix(path);
    if (pathStr.find("objects/object_link_") == std::string::npos) {
        return nullptr;
    }

    std::string baseName = GetBaseName(pathStr);
    if (!baseName.starts_with("gLink")) {
        return nullptr;
    }

    auto archive = FindArchiveById(sAnchorModelOverride.modelId);
    if (archive == nullptr) {
        return nullptr;
    }

    std::string folder = sAnchorModelOverride.linkAge == LINK_AGE_ADULT ? (sAnchorModelOverride.modelId + "_adult")
                                                                        : (sAnchorModelOverride.modelId + "_child");
    std::string customPath = "objects/object_anchor_models/" + folder + "/" + baseName;

    auto context = Ship::Context::GetInstance();
    if (context == nullptr || context->GetResourceManager() == nullptr) {
        return nullptr;
    }

    auto resource = context->GetResourceManager()->LoadResource(Ship::ResourceIdentifier(customPath.c_str(), 0, archive),
                                                                true);
    if (resource == nullptr) {
        return nullptr;
    }

    if (resource->GetInitData()->Type != static_cast<uint32_t>(Fast::ResourceType::DisplayList)) {
        return nullptr;
    }

    auto displayList = std::static_pointer_cast<Fast::DisplayList>(resource);
    return (void*)&displayList->Instructions[0];
}

void* AnchorModRegistry::TryLoadAnchorTextureOverride(const char* path) {
    if (path == nullptr || !sAnchorModelOverride.active || sAnchorModelOverride.modelId.empty()) {
        return nullptr;
    }

    std::string pathStr = StripOtrPrefix(path);
    if (pathStr.find("objects/object_link_") == std::string::npos) {
        return nullptr;
    }

    std::string baseName = GetBaseName(pathStr);
    if (!IsAnchorFlipbookTextureName(baseName)) {
        return nullptr;
    }

    return LoadCustomTexture(sAnchorModelOverride.modelId, sAnchorModelOverride.linkAge, baseName.c_str());
}

AnchorTextureOverrides AnchorModRegistry::ApplyAnchorFlipbookTextures(Player* player,
                                                                      const std::string& modelId,
                                                                      int32_t linkAge) {
    AnchorTextureOverrides overrides = {};
    overrides.eyeIndex = -1;
    overrides.mouthIndex = -1;

#if defined(MODDING) || defined(_MSC_VER) || defined(__GNUC__)
    if (player == nullptr) {
        return overrides;
    }

    int eyeIndex = (player->skelAnime.jointTable[22].x & 0xF) - 1;
    int mouthIndex = (player->skelAnime.jointTable[22].x >> 4) - 1;
    if (eyeIndex < 0) {
        eyeIndex = sEyeMouthIndexes[player->actor.shape.face][0];
    }
    if (mouthIndex < 0) {
        mouthIndex = sEyeMouthIndexes[player->actor.shape.face][1];
    }

    eyeIndex = ClampIndex(eyeIndex, 7);
    mouthIndex = ClampIndex(mouthIndex, 3);

    const char* eyeName = linkAge == LINK_AGE_ADULT ? kAdultEyeTextureNames[eyeIndex]
                                                    : kChildEyeTextureNames[eyeIndex];
    const char* mouthName = linkAge == LINK_AGE_ADULT ? kAdultMouthTextureNames[mouthIndex]
                                                      : kChildMouthTextureNames[mouthIndex];
    void* eyeTexture = LoadCustomTexture(modelId, linkAge, eyeName);
    void* mouthTexture = LoadCustomTexture(modelId, linkAge, mouthName);

    if (eyeTexture != nullptr) {
        overrides.eyeIndex = eyeIndex;
        overrides.originalEye = sEyeTextures[linkAge][eyeIndex];
        sEyeTextures[linkAge][eyeIndex] = eyeTexture;
        overrides.hasEye = true;
    }

    if (mouthTexture != nullptr) {
        overrides.mouthIndex = mouthIndex;
        overrides.originalMouth = sMouthTextures[linkAge][mouthIndex];
        sMouthTextures[linkAge][mouthIndex] = mouthTexture;
        overrides.hasMouth = true;
    }
#endif

    return overrides;
}

void AnchorModRegistry::RestoreAnchorFlipbookTextures(const AnchorTextureOverrides& overrides, int32_t linkAge) {
#if defined(MODDING) || defined(_MSC_VER) || defined(__GNUC__)
    if (overrides.hasEye && overrides.eyeIndex >= 0) {
        sEyeTextures[linkAge][overrides.eyeIndex] = overrides.originalEye;
    }
    if (overrides.hasMouth && overrides.mouthIndex >= 0) {
        sMouthTextures[linkAge][overrides.mouthIndex] = overrides.originalMouth;
    }
#endif
}

bool AnchorModRegistry::ApplyModelToPlayer(const std::string& id, int32_t linkAge, Player* player) {
    if (player == nullptr) {
        return false;
    }

    Init();

    auto context = Ship::Context::GetInstance();
    if (context == nullptr || context->GetResourceManager() == nullptr) {
        return false;
    }

    auto customSkel = GetModelSkeleton(id, linkAge);
    if (customSkel != nullptr) {
        bool applied = ApplySkeletonToSkelAnime(&player->skelAnime, customSkel.get());
        applied = ApplySkeletonToSkelAnime(&player->upperSkelAnime, customSkel.get()) && applied;
        if (applied) {
            return true;
        }
    }

    const char* skeletonPath = GetLinkSkeletonPath(linkAge);
    if (skeletonPath == nullptr) {
        return false;
    }

    auto resource = context->GetResourceManager()->LoadResource(skeletonPath);
    auto vanillaSkel = std::dynamic_pointer_cast<SOH::Skeleton>(resource);
    if (vanillaSkel == nullptr) {
        return false;
    }

    bool applied = ApplySkeletonToSkelAnime(&player->skelAnime, vanillaSkel.get());
    applied = ApplySkeletonToSkelAnime(&player->upperSkelAnime, vanillaSkel.get()) && applied;
    return applied;
}

bool AnchorModRegistry::HasCustomModel(const std::string& id, int32_t linkAge, int32_t expectedLimbCount) {
    Init();

    auto customSkel = GetModelSkeleton(id, linkAge);
    if (customSkel == nullptr) {
        return false;
    }

    int32_t newLimbCount = customSkel->limbCount + 1;
    if (expectedLimbCount != 0 && expectedLimbCount != newLimbCount) {
        return false;
    }

    return true;
}
