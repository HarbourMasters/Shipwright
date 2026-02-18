
#include "ExternalModManager.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <limits>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <zip.h>

#include <ship/Context.h>

#include "ExternalModItemRuntime.h"
#include "ExternalModWasmRuntime.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Notification/Notification.h"
#include "soh/OTRGlobals.h"

extern "C" {
#include <z64.h>
#include "macros.h"
#include "variables.h"

extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

namespace SOH {

namespace {
constexpr int32_t kExternalModApiVersionMin = 1;
constexpr int32_t kExternalModApiVersionMax = 2;
constexpr int32_t kExternalModApiVersionV2 = 2;
constexpr uint64_t kMaxManifestBytes = 256 * 1024;
constexpr uint64_t kMaxScriptBytes = 1024 * 1024;
constexpr uint64_t kMaxAssetBytes = 512ull * 1024ull * 1024ull;
constexpr uint64_t kMaxWasmBytes = 4ull * 1024ull * 1024ull;
constexpr uint64_t kMaxItemDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxInputDefinitionBytes = 256 * 1024;
constexpr int32_t kDefaultTriggerCooldownFrames = 90;
constexpr int32_t kDefaultRuntimeMemoryKb = 1024;
constexpr int32_t kDefaultRuntimeCallMs = 2;

const std::unordered_map<std::string, int16_t> kSceneAliases = {
    { "SCENE_KOKIRI_FOREST", SCENE_KOKIRI_FOREST },
    { "SCENE_LINKS_HOUSE", SCENE_LINKS_HOUSE },
    { "SCENE_KAKARIKO_VILLAGE", SCENE_KAKARIKO_VILLAGE },
};

const std::unordered_map<std::string, int16_t> kEntranceAliases = {
    { "ENTR_KOKIRI_FOREST_0", ENTR_KOKIRI_FOREST_0 },
    { "ENTR_KOKIRI_FOREST_OUTSIDE_LINKS_HOUSE", ENTR_KOKIRI_FOREST_OUTSIDE_LINKS_HOUSE },
    { "ENTR_LINKS_HOUSE_CHILD_SPAWN", ENTR_LINKS_HOUSE_CHILD_SPAWN },
    { "ENTR_LINKS_HOUSE_0_1", ENTR_LINKS_HOUSE_0_1 },
    { "ENTR_KAKARIKO_VILLAGE_FRONT_GATE", ENTR_KAKARIKO_VILLAGE_FRONT_GATE },
    { "ENTR_KAKARIKO_VILLAGE_OUTSIDE_POTION_SHOP_BACK", ENTR_KAKARIKO_VILLAGE_OUTSIDE_POTION_SHOP_BACK },
};

const std::unordered_map<std::string, int32_t> kButtonAliases = {
    { "BTN_A", BTN_A },
    { "BTN_B", BTN_B },
    { "BTN_Z", BTN_Z },
    { "BTN_START", BTN_START },
    { "BTN_DUP", BTN_DUP },
    { "BTN_DDOWN", BTN_DDOWN },
    { "BTN_DLEFT", BTN_DLEFT },
    { "BTN_DRIGHT", BTN_DRIGHT },
    { "BTN_L", BTN_L },
    { "BTN_R", BTN_R },
    { "BTN_CUP", BTN_CUP },
    { "BTN_CDOWN", BTN_CDOWN },
    { "BTN_CLEFT", BTN_CLEFT },
    { "BTN_CRIGHT", BTN_CRIGHT },
    { "BTN_CUSTOM_MODIFIER1", BTN_CUSTOM_MODIFIER1 },
    { "BTN_CUSTOM_MODIFIER2", BTN_CUSTOM_MODIFIER2 },
    { "A", BTN_A },
    { "B", BTN_B },
    { "Z", BTN_Z },
    { "START", BTN_START },
    { "DUP", BTN_DUP },
    { "DDOWN", BTN_DDOWN },
    { "DLEFT", BTN_DLEFT },
    { "DRIGHT", BTN_DRIGHT },
    { "L", BTN_L },
    { "R", BTN_R },
    { "CUP", BTN_CUP },
    { "CDOWN", BTN_CDOWN },
    { "CLEFT", BTN_CLEFT },
    { "CRIGHT", BTN_CRIGHT },
};

std::string ToLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

std::string ToUpper(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return value;
}

std::string SanitizeModIdForPath(const std::string& modId) {
    if (modId.empty()) {
        return "external_mod";
    }
    std::string sanitized = modId;
    std::transform(sanitized.begin(), sanitized.end(), sanitized.begin(), [](unsigned char c) {
        if (std::isalnum(c) || c == '.' || c == '_' || c == '-') {
            return static_cast<char>(c);
        }
        return '_';
    });
    return sanitized;
}

std::string SanitizeCVarSegment(const std::string& value) {
    if (value.empty()) {
        return "default";
    }
    std::string sanitized = value;
    std::transform(sanitized.begin(), sanitized.end(), sanitized.begin(), [](unsigned char c) {
        if (std::isalnum(c) || c == '_') {
            return static_cast<char>(c);
        }
        return '_';
    });
    return sanitized;
}

bool HasSupportedArchiveExtension(const std::filesystem::path& path) {
    const auto extension = ToLower(path.extension().string());
    return extension == ".otr" || extension == ".o2r";
}

bool ValidateRequiredString(const nlohmann::json& json, const char* key, std::string& value, std::string& outError) {
    if (!json.contains(key) || !json[key].is_string()) {
        outError = std::string("Missing or invalid field: ") + key;
        return false;
    }
    value = json[key].get<std::string>();
    if (value.empty()) {
        outError = std::string("Field must not be empty: ") + key;
        return false;
    }
    return true;
}

bool ParseInt16FromString(const std::string& value, int16_t& outValue) {
    try {
        size_t parsedLength = 0;
        const auto parsed = std::stoll(value, &parsedLength, 0);
        if (parsedLength != value.size() || parsed < std::numeric_limits<int16_t>::min() ||
            parsed > std::numeric_limits<int16_t>::max()) {
            return false;
        }
        outValue = static_cast<int16_t>(parsed);
        return true;
    } catch (...) {
        return false;
    }
}

bool ParseInt32FromString(const std::string& value, int32_t& outValue) {
    try {
        size_t parsedLength = 0;
        const auto parsed = std::stoll(value, &parsedLength, 0);
        if (parsedLength != value.size() || parsed < std::numeric_limits<int32_t>::min() ||
            parsed > std::numeric_limits<int32_t>::max()) {
            return false;
        }
        outValue = static_cast<int32_t>(parsed);
        return true;
    } catch (...) {
        return false;
    }
}

bool ParseAliasedInt16(const nlohmann::json& json, const std::unordered_map<std::string, int16_t>& aliases,
                       const char* fieldName, int16_t& outValue, std::string& outError) {
    if (json.is_number_integer()) {
        const auto raw = json.get<long long>();
        if (raw < std::numeric_limits<int16_t>::min() || raw > std::numeric_limits<int16_t>::max()) {
            outError = std::string("Invalid ") + fieldName + ": out of range";
            return false;
        }
        outValue = static_cast<int16_t>(raw);
        return true;
    }

    if (!json.is_string()) {
        outError = std::string("Invalid ") + fieldName + ": expected integer or alias";
        return false;
    }

    const auto value = json.get<std::string>();
    const auto it = aliases.find(value);
    if (it != aliases.end()) {
        outValue = it->second;
        return true;
    }

    if (ParseInt16FromString(value, outValue)) {
        return true;
    }

    outError = std::string("Invalid ") + fieldName + ": unknown alias " + value;
    return false;
}

bool ParseButtonMask(const nlohmann::json& json, int32_t& outMask, std::string& outError) {
    if (json.is_number_integer()) {
        const auto raw = json.get<long long>();
        if (raw < 0 || raw > std::numeric_limits<int32_t>::max()) {
            outError = "button mask out of range";
            return false;
        }
        outMask = static_cast<int32_t>(raw);
        return true;
    }

    if (!json.is_string()) {
        outError = "button mask must be integer or alias string";
        return false;
    }

    const auto value = ToUpper(json.get<std::string>());
    const auto aliasIt = kButtonAliases.find(value);
    if (aliasIt != kButtonAliases.end()) {
        outMask = aliasIt->second;
        return true;
    }

    if (ParseInt32FromString(value, outMask)) {
        return true;
    }

    outError = "unknown button alias: " + value;
    return false;
}
bool ParseInputTriggerType(const nlohmann::json& json, ExternalModInputTriggerType& outType, std::string& outError) {
    if (!json.is_string()) {
        outError = "trigger must be a string";
        return false;
    }

    const auto trigger = ToLower(json.get<std::string>());
    if (trigger == "pressed") {
        outType = ExternalModInputTriggerType::Pressed;
        return true;
    }
    if (trigger == "held") {
        outType = ExternalModInputTriggerType::Held;
        return true;
    }
    if (trigger == "released") {
        outType = ExternalModInputTriggerType::Released;
        return true;
    }

    outError = "trigger must be one of: pressed|held|released";
    return false;
}

bool ParseAction(const nlohmann::json& json, int32_t apiVersion, ExternalModAction& outAction, std::string& outError) {
    if (!json.is_object()) {
        outError = "Action must be an object";
        return false;
    }

    std::string actionType;
    if (!ValidateRequiredString(json, "action", actionType, outError)) {
        return false;
    }

    if (actionType == "showNotification") {
        outAction.type = ExternalModActionType::ShowNotification;
        return ValidateRequiredString(json, "text", outAction.text, outError);
    }

    if (actionType == "teleportToEntrance") {
        if (!json.contains("entrance")) {
            outError = "teleportToEntrance requires entrance";
            return false;
        }
        outAction.type = ExternalModActionType::TeleportToEntrance;
        return ParseAliasedInt16(json["entrance"], kEntranceAliases, "entrance", outAction.entranceIndex, outError);
    }

    if (apiVersion < kExternalModApiVersionV2) {
        outError = "Unsupported action for apiVersion 1: " + actionType;
        return false;
    }

    if (actionType == "pressButton") {
        const char* key = json.contains("button") ? "button" : (json.contains("mask") ? "mask" : nullptr);
        if (key == nullptr) {
            outError = "pressButton requires button";
            return false;
        }
        outAction.type = ExternalModActionType::PressButton;
        return ParseButtonMask(json[key], outAction.buttonMask, outError);
    }

    if (actionType == "grantModItem") {
        outAction.type = ExternalModActionType::GrantModItem;
        if (json.contains("itemId")) {
            return ValidateRequiredString(json, "itemId", outAction.itemId, outError);
        }
        if (json.contains("id")) {
            return ValidateRequiredString(json, "id", outAction.itemId, outError);
        }
        outError = "grantModItem requires itemId";
        return false;
    }

    if (actionType == "revokeModItem") {
        outAction.type = ExternalModActionType::RevokeModItem;
        if (json.contains("itemId")) {
            return ValidateRequiredString(json, "itemId", outAction.itemId, outError);
        }
        if (json.contains("id")) {
            return ValidateRequiredString(json, "id", outAction.itemId, outError);
        }
        outError = "revokeModItem requires itemId";
        return false;
    }

    if (actionType == "invokeWasm") {
        outAction.type = ExternalModActionType::InvokeWasm;
        if (!ValidateRequiredString(json, "export", outAction.exportName, outError)) {
            return false;
        }
        if (json.contains("args")) {
            if (!json["args"].is_array()) {
                outError = "invokeWasm args must be an array";
                return false;
            }
            for (const auto& arg : json["args"]) {
                if (!arg.is_number_integer()) {
                    outError = "invokeWasm args must be integers";
                    return false;
                }
                outAction.args.push_back(arg.get<int32_t>());
            }
        }
        return true;
    }

    outError = "Unsupported action: " + actionType;
    return false;
}

bool ParseActionArray(const nlohmann::json& json, int32_t apiVersion, const char* fieldName,
                      std::vector<ExternalModAction>& outActions, std::string& outError) {
    if (!json.is_array()) {
        outError = std::string(fieldName) + " must be an array";
        return false;
    }

    outActions.clear();
    for (size_t i = 0; i < json.size(); ++i) {
        ExternalModAction action;
        if (!ParseAction(json[i], apiVersion, action, outError)) {
            outError = std::string(fieldName) + "[" + std::to_string(i) + "]: " + outError;
            return false;
        }
        outActions.push_back(std::move(action));
    }
    return true;
}

bool ParseVec3(const nlohmann::json& json, float& x, float& y, float& z, const char* fieldName, std::string& outError) {
    if (!json.is_array() || json.size() != 3 || !json[0].is_number() || !json[1].is_number() || !json[2].is_number()) {
        outError = std::string(fieldName) + " must be [x,y,z] numbers";
        return false;
    }
    x = json[0].get<float>();
    y = json[1].get<float>();
    z = json[2].get<float>();
    return true;
}

bool IsWithinAabb(const ExternalModTriggerVolume& trigger, const Vec3f& point) {
    return point.x >= trigger.minX && point.x <= trigger.maxX && point.y >= trigger.minY && point.y <= trigger.maxY &&
           point.z >= trigger.minZ && point.z <= trigger.maxZ;
}

bool IsPathWithinBase(const std::filesystem::path& basePath, const std::filesystem::path& candidatePath) {
    try {
        const auto canonicalBase = std::filesystem::weakly_canonical(basePath);
        const auto canonicalCandidate = std::filesystem::weakly_canonical(candidatePath);
        const auto mismatch = std::mismatch(canonicalBase.begin(), canonicalBase.end(), canonicalCandidate.begin(),
                                            canonicalCandidate.end());
        return mismatch.first == canonicalBase.end();
    } catch (...) {
        return false;
    }
}

bool MatchButtonMask(int32_t buttons, int32_t mask) {
    return (mask != 0) && ((buttons & mask) == mask);
}

float GetParamOrDefault(const std::unordered_map<std::string, float>& params, const char* key, float defaultValue) {
    const auto it = params.find(key);
    if (it == params.end()) {
        return defaultValue;
    }
    return it->second;
}

} // namespace

ExternalModManager& ExternalModManager::Instance() {
    static ExternalModManager instance;
    return instance;
}

const std::vector<ExternalModPackage>& ExternalModManager::GetPackages() const {
    return mPackages;
}

std::string ExternalModManager::BuildBindingCVarName(const std::string& modId, const std::string& bindingId) {
    return "gExternalMods.Input." + SanitizeCVarSegment(modId) + "." + SanitizeCVarSegment(bindingId);
}

void ExternalModManager::Shutdown() {
    UnregisterHooks();
    for (auto& package : mPackages) {
        package.runtime.enabled = false;
        package.runtime.onGameLoadedActions.clear();
        package.runtime.onSceneInitActions.clear();
        package.runtime.frameTriggers.clear();
        package.runtime.inputBindings.clear();
        package.runtime.inputTriggers.clear();
        package.runtime.itemDefinitions.clear();
        package.runtime.wasmRuntime.reset();
    }
}

void ExternalModManager::DiscoverPackages() {
    mPackages.clear();
    const auto modsPath = std::filesystem::path(Ship::Context::LocateFileAcrossAppDirs("mods", "soh"));
    if (!std::filesystem::exists(modsPath)) {
        SPDLOG_INFO("[ExternalMods] Mods path does not exist: {}", modsPath.string());
        return;
    }

    std::unordered_set<std::string> seenIds;
    for (const auto& entry : std::filesystem::directory_iterator(modsPath)) {
        if (!entry.is_directory() && !entry.is_regular_file()) {
            continue;
        }

        ExternalModPackage package;
        package.sourcePath = entry.path();
        package.isZip = entry.is_regular_file() && ToLower(entry.path().extension().string()) == ".zip";

        if (entry.is_regular_file() && !package.isZip) {
            continue;
        }

        std::string manifestContent;
        std::string manifestError;
        bool manifestRead = entry.is_directory() ? ReadManifestFromDirectory(entry.path(), manifestContent, manifestError)
                                                 : (package.isZip
                                                        ? ReadManifestFromZip(entry.path(), manifestContent, manifestError)
                                                        : false);
        if (!manifestRead) {
            package.valid = false;
            package.error = manifestError.empty() ? "Manifest not found" : manifestError;
            SPDLOG_WARN("[ExternalMods] Skipping package {}: {}", package.sourcePath.string(), package.error);
            mPackages.push_back(std::move(package));
            continue;
        }

        std::string parseError;
        if (!TryParseManifest(manifestContent, package.manifest, parseError)) {
            package.valid = false;
            package.error = parseError;
            SPDLOG_WARN("[ExternalMods] Invalid manifest in {}: {}", package.sourcePath.string(), package.error);
            mPackages.push_back(std::move(package));
            continue;
        }

        if (seenIds.contains(package.manifest.id)) {
            package.valid = false;
            package.error = "Duplicate mod id detected: " + package.manifest.id;
            SPDLOG_WARN("[ExternalMods] Skipping package {}: {}", package.sourcePath.string(), package.error);
            mPackages.push_back(std::move(package));
            continue;
        }

        seenIds.insert(package.manifest.id);
        package.valid = true;
        SPDLOG_INFO("[ExternalMods] Loaded manifest {} ({}) from {}", package.manifest.name, package.manifest.id,
                    package.sourcePath.string());
        mPackages.push_back(std::move(package));
    }

    SPDLOG_INFO("[ExternalMods] Discovery complete: {} package(s)", mPackages.size());
}

void ExternalModManager::Initialize() {
    UnregisterHooks();
    DiscoverPackages();

    std::vector<size_t> validIndices;
    validIndices.reserve(mPackages.size());
    for (size_t i = 0; i < mPackages.size(); ++i) {
        if (mPackages[i].valid) {
            validIndices.push_back(i);
        }
    }

    std::stable_sort(validIndices.begin(), validIndices.end(), [this](size_t a, size_t b) {
        const auto& lhs = mPackages[a].manifest;
        const auto& rhs = mPackages[b].manifest;
        if (lhs.loadOrder != rhs.loadOrder) {
            return lhs.loadOrder < rhs.loadOrder;
        }
        return lhs.id < rhs.id;
    });
    size_t runtimeCount = 0;
    for (const auto index : validIndices) {
        auto& package = mPackages[index];
        package.runtime = ExternalModRuntime{};
        package.mountedAssets.clear();

        std::string error;
        if (!LoadRuntimeForPackage(package, error)) {
            package.valid = false;
            package.error = error;
            SPDLOG_WARN("[ExternalMods] Runtime disabled for {}: {}", package.sourcePath.string(), package.error);
            continue;
        }

        if (!MountAssetsForPackage(package, error)) {
            package.valid = false;
            package.error = error;
            SPDLOG_WARN("[ExternalMods] Asset mount failed for {}: {}", package.sourcePath.string(), package.error);
            continue;
        }

        for (const auto& binding : package.runtime.inputBindings) {
            const auto cvarName = BuildBindingCVarName(package.manifest.id, binding.id);
            const auto existing = CVarGetInteger(cvarName.c_str(), binding.defaultMask);
            if (existing == 0 && binding.defaultMask != 0) {
                CVarSetInteger(cvarName.c_str(), binding.defaultMask);
            }
        }

        package.runtime.enabled = true;
        runtimeCount++;
        SPDLOG_INFO("[ExternalMods] Runtime enabled: {} ({})", package.manifest.name, package.manifest.id);
    }

    if (runtimeCount > 0) {
        RegisterHooks();
    }

    SPDLOG_INFO("[ExternalMods] Initialization complete: {} runtime(s)", runtimeCount);
}

bool ExternalModManager::TryParseManifest(const std::string& content, ExternalModManifest& outManifest,
                                          std::string& outError) {
    outManifest = ExternalModManifest{};

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("JSON parse error: ") + ex.what();
        return false;
    }

    if (!ValidateRequiredString(json, "id", outManifest.id, outError) ||
        !ValidateRequiredString(json, "name", outManifest.name, outError) ||
        !ValidateRequiredString(json, "version", outManifest.version, outError) ||
        !ValidateRequiredString(json, "entryScript", outManifest.entryScript, outError)) {
        return false;
    }

    std::filesystem::path normalizedEntryScript;
    if (!IsSafePackageRelativePath(outManifest.entryScript, normalizedEntryScript, outError)) {
        outError = "Invalid entryScript: " + outError;
        return false;
    }
    outManifest.entryScript = normalizedEntryScript.generic_string();

    if (!json.contains("apiVersion") || !json["apiVersion"].is_number_integer()) {
        outError = "Missing or invalid field: apiVersion";
        return false;
    }
    outManifest.apiVersion = json["apiVersion"].get<int32_t>();
    if (outManifest.apiVersion < kExternalModApiVersionMin || outManifest.apiVersion > kExternalModApiVersionMax) {
        outError = "Unsupported apiVersion: " + std::to_string(outManifest.apiVersion);
        return false;
    }

    if (json.contains("gameVersionMin")) {
        if (!json["gameVersionMin"].is_string()) {
            outError = "Invalid field: gameVersionMin must be string";
            return false;
        }
        outManifest.gameVersionMin = json["gameVersionMin"].get<std::string>();
    }

    if (json.contains("loadOrder")) {
        if (!json["loadOrder"].is_number_integer()) {
            outError = "Invalid field: loadOrder must be integer";
            return false;
        }
        outManifest.loadOrder = json["loadOrder"].get<int32_t>();
    }

    if (json.contains("assets")) {
        if (!json["assets"].is_array()) {
            outError = "Invalid field: assets must be an array";
            return false;
        }
        for (size_t i = 0; i < json["assets"].size(); ++i) {
            const auto& asset = json["assets"][i];
            if (!asset.is_string()) {
                outError = "Invalid field: assets entries must be strings";
                return false;
            }

            std::filesystem::path normalizedAsset;
            if (!IsSafePackageRelativePath(asset.get<std::string>(), normalizedAsset, outError)) {
                outError = "Invalid assets[" + std::to_string(i) + "]: " + outError;
                return false;
            }
            if (!HasSupportedArchiveExtension(normalizedAsset)) {
                outError = "Invalid assets[" + std::to_string(i) + "]: only .otr/.o2r are supported";
                return false;
            }
            outManifest.assets.push_back(normalizedAsset.generic_string());
        }
    }

    if (json.contains("dependencies")) {
        if (!json["dependencies"].is_array()) {
            outError = "Invalid field: dependencies must be an array";
            return false;
        }
        for (const auto& dep : json["dependencies"]) {
            if (!dep.is_string()) {
                outError = "Invalid field: dependencies entries must be strings";
                return false;
            }
            outManifest.dependencies.push_back(dep.get<std::string>());
        }
    }

    if (outManifest.apiVersion >= kExternalModApiVersionV2) {
        if (!json.contains("runtime") || !json["runtime"].is_object()) {
            outError = "Missing or invalid field: runtime";
            return false;
        }

        const auto& runtime = json["runtime"];
        if (!ValidateRequiredString(runtime, "type", outManifest.runtimeType, outError)) {
            outError = "Invalid runtime.type: " + outError;
            return false;
        }
        if (!ValidateRequiredString(runtime, "module", outManifest.runtimeModule, outError)) {
            outError = "Invalid runtime.module: " + outError;
            return false;
        }

        std::filesystem::path normalizedRuntimeModule;
        if (!IsSafePackageRelativePath(outManifest.runtimeModule, normalizedRuntimeModule, outError)) {
            outError = "Invalid runtime.module: " + outError;
            return false;
        }
        outManifest.runtimeModule = normalizedRuntimeModule.generic_string();

        outManifest.runtimeMaxMemoryKb = kDefaultRuntimeMemoryKb;
        outManifest.runtimeMaxCallMs = kDefaultRuntimeCallMs;
        if (runtime.contains("maxMemoryKb")) {
            if (!runtime["maxMemoryKb"].is_number_integer()) {
                outError = "runtime.maxMemoryKb must be integer";
                return false;
            }
            outManifest.runtimeMaxMemoryKb = runtime["maxMemoryKb"].get<int32_t>();
        }
        if (runtime.contains("maxCallMs")) {
            if (!runtime["maxCallMs"].is_number_integer()) {
                outError = "runtime.maxCallMs must be integer";
                return false;
            }
            outManifest.runtimeMaxCallMs = runtime["maxCallMs"].get<int32_t>();
        }

        if (outManifest.runtimeMaxMemoryKb < 64 || outManifest.runtimeMaxMemoryKb > 4096) {
            outError = "runtime.maxMemoryKb must be in [64, 4096]";
            return false;
        }
        if (outManifest.runtimeMaxCallMs < 1 || outManifest.runtimeMaxCallMs > 4) {
            outError = "runtime.maxCallMs must be in [1, 4]";
            return false;
        }

        if (!ValidateRequiredString(json, "itemDefinitions", outManifest.itemDefinitions, outError)) {
            outError = "Missing or invalid field: itemDefinitions";
            return false;
        }
        if (!ValidateRequiredString(json, "inputDefinitions", outManifest.inputDefinitions, outError)) {
            outError = "Missing or invalid field: inputDefinitions";
            return false;
        }

        std::filesystem::path normalizedItemsPath;
        if (!IsSafePackageRelativePath(outManifest.itemDefinitions, normalizedItemsPath, outError)) {
            outError = "Invalid itemDefinitions: " + outError;
            return false;
        }
        outManifest.itemDefinitions = normalizedItemsPath.generic_string();

        std::filesystem::path normalizedInputPath;
        if (!IsSafePackageRelativePath(outManifest.inputDefinitions, normalizedInputPath, outError)) {
            outError = "Invalid inputDefinitions: " + outError;
            return false;
        }
        outManifest.inputDefinitions = normalizedInputPath.generic_string();
    }

    return true;
}
bool ExternalModManager::TryParseEntryScript(const std::string& content, int32_t apiVersion, ExternalModRuntime& outRuntime,
                                             std::string& outError) {
    outRuntime = ExternalModRuntime{};
    outRuntime.apiVersion = apiVersion;

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("Entry script parse error: ") + ex.what();
        return false;
    }

    if (!json.is_object()) {
        outError = "Entry script must be an object";
        return false;
    }

    if (json.contains("apiVersion")) {
        if (!json["apiVersion"].is_number_integer()) {
            outError = "Entry script apiVersion must be integer";
            return false;
        }
        const auto scriptVersion = json["apiVersion"].get<int32_t>();
        if (scriptVersion != apiVersion) {
            outError = "Entry script apiVersion mismatch";
            return false;
        }
    }

    const nlohmann::json* root = &json;
    if (json.contains("hooks")) {
        if (!json["hooks"].is_object()) {
            outError = "hooks must be an object";
            return false;
        }
        root = &json["hooks"];
    }

    if (root->contains("onGameLoaded") &&
        !ParseActionArray((*root)["onGameLoaded"], apiVersion, "onGameLoaded", outRuntime.onGameLoadedActions,
                          outError)) {
        return false;
    }

    if (root->contains("onSceneInit")) {
        const auto& onSceneInit = (*root)["onSceneInit"];
        if (!onSceneInit.is_array()) {
            outError = "onSceneInit must be an array";
            return false;
        }
        for (size_t i = 0; i < onSceneInit.size(); ++i) {
            const auto& item = onSceneInit[i];
            if (!item.is_object()) {
                outError = "onSceneInit[" + std::to_string(i) + "] must be an object";
                return false;
            }
            if (!item.contains("scene") || !item.contains("actions")) {
                outError = "onSceneInit[" + std::to_string(i) + "] requires scene and actions";
                return false;
            }
            ExternalModSceneAction sceneAction;
            if (!ParseAliasedInt16(item["scene"], kSceneAliases, "scene", sceneAction.sceneId, outError)) {
                outError = "onSceneInit[" + std::to_string(i) + "]: " + outError;
                return false;
            }
            if (!ParseActionArray(item["actions"], apiVersion, "actions", sceneAction.actions, outError)) {
                outError = "onSceneInit[" + std::to_string(i) + "]: " + outError;
                return false;
            }
            outRuntime.onSceneInitActions.push_back(std::move(sceneAction));
        }
    }

    const nlohmann::json* triggers = nullptr;
    if (root->contains("onFrameTriggers")) {
        triggers = &(*root)["onFrameTriggers"];
    } else if (root->contains("triggers")) {
        triggers = &(*root)["triggers"];
    }

    if (triggers != nullptr) {
        if (!triggers->is_array()) {
            outError = "onFrameTriggers must be an array";
            return false;
        }
        for (size_t i = 0; i < triggers->size(); ++i) {
            const auto& item = (*triggers)[i];
            if (!item.is_object()) {
                outError = "onFrameTriggers[" + std::to_string(i) + "] must be an object";
                return false;
            }
            if (!item.contains("scene") || !item.contains("bounds") || !item.contains("actions")) {
                outError = "onFrameTriggers[" + std::to_string(i) + "] requires scene, bounds and actions";
                return false;
            }

            ExternalModTriggerVolume trigger;
            trigger.id = "trigger_" + std::to_string(i);
            if (item.contains("id")) {
                if (!item["id"].is_string() || item["id"].get<std::string>().empty()) {
                    outError = "onFrameTriggers[" + std::to_string(i) + "].id must be non-empty string";
                    return false;
                }
                trigger.id = item["id"].get<std::string>();
            }
            if (!ParseAliasedInt16(item["scene"], kSceneAliases, "scene", trigger.sceneId, outError)) {
                outError = "onFrameTriggers[" + std::to_string(i) + "]: " + outError;
                return false;
            }
            const auto& bounds = item["bounds"];
            if (!bounds.is_object() || !bounds.contains("min") || !bounds.contains("max")) {
                outError = "onFrameTriggers[" + std::to_string(i) + "].bounds requires min and max";
                return false;
            }
            if (!ParseVec3(bounds["min"], trigger.minX, trigger.minY, trigger.minZ, "bounds.min", outError) ||
                !ParseVec3(bounds["max"], trigger.maxX, trigger.maxY, trigger.maxZ, "bounds.max", outError)) {
                outError = "onFrameTriggers[" + std::to_string(i) + "]: " + outError;
                return false;
            }
            if (trigger.minX > trigger.maxX || trigger.minY > trigger.maxY || trigger.minZ > trigger.maxZ) {
                outError = "onFrameTriggers[" + std::to_string(i) + "] has invalid bounds";
                return false;
            }
            if (item.contains("cooldownFrames")) {
                if (!item["cooldownFrames"].is_number_integer()) {
                    outError = "onFrameTriggers[" + std::to_string(i) + "].cooldownFrames must be an integer";
                    return false;
                }
                trigger.cooldownFrames = std::max(0, item["cooldownFrames"].get<int32_t>());
            } else {
                trigger.cooldownFrames = kDefaultTriggerCooldownFrames;
            }
            if (!ParseActionArray(item["actions"], apiVersion, "actions", trigger.actions, outError)) {
                outError = "onFrameTriggers[" + std::to_string(i) + "]: " + outError;
                return false;
            }
            outRuntime.frameTriggers.push_back(std::move(trigger));
        }
    }

    if (root->contains("onInput")) {
        if (apiVersion < kExternalModApiVersionV2) {
            outError = "onInput requires apiVersion 2";
            return false;
        }

        const auto& onInput = (*root)["onInput"];
        if (!onInput.is_array()) {
            outError = "onInput must be an array";
            return false;
        }

        for (size_t i = 0; i < onInput.size(); ++i) {
            const auto& item = onInput[i];
            if (!item.is_object()) {
                outError = "onInput[" + std::to_string(i) + "] must be an object";
                return false;
            }

            ExternalModInputActionTrigger inputTrigger;
            inputTrigger.id = "input_trigger_" + std::to_string(i);
            if (item.contains("id")) {
                if (!item["id"].is_string() || item["id"].get<std::string>().empty()) {
                    outError = "onInput[" + std::to_string(i) + "].id must be non-empty string";
                    return false;
                }
                inputTrigger.id = item["id"].get<std::string>();
            }

            if (item.contains("binding")) {
                if (!ValidateRequiredString(item, "binding", inputTrigger.bindingId, outError)) {
                    outError = "onInput[" + std::to_string(i) + "]: " + outError;
                    return false;
                }
            } else if (item.contains("bindingId")) {
                if (!ValidateRequiredString(item, "bindingId", inputTrigger.bindingId, outError)) {
                    outError = "onInput[" + std::to_string(i) + "]: " + outError;
                    return false;
                }
            } else {
                outError = "onInput[" + std::to_string(i) + "] requires binding";
                return false;
            }

            if (item.contains("trigger")) {
                if (!ParseInputTriggerType(item["trigger"], inputTrigger.trigger, outError)) {
                    outError = "onInput[" + std::to_string(i) + "]: " + outError;
                    return false;
                }
            }

            if (item.contains("cooldownFrames")) {
                if (!item["cooldownFrames"].is_number_integer()) {
                    outError = "onInput[" + std::to_string(i) + "].cooldownFrames must be an integer";
                    return false;
                }
                inputTrigger.cooldownFrames = std::max(0, item["cooldownFrames"].get<int32_t>());
            }

            if (!item.contains("actions") ||
                !ParseActionArray(item["actions"], apiVersion, "actions", inputTrigger.actions, outError)) {
                outError = "onInput[" + std::to_string(i) + "]: " + outError;
                return false;
            }

            outRuntime.inputTriggers.push_back(std::move(inputTrigger));
        }
    }

    return true;
}

bool ExternalModManager::TryParseItemDefinitions(const std::string& content,
                                                 std::vector<ExternalModItemDefinition>& outItems,
                                                 std::string& outError) {
    outItems.clear();

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("items.json parse error: ") + ex.what();
        return false;
    }

    const nlohmann::json* items = nullptr;
    if (json.is_array()) {
        items = &json;
    } else if (json.is_object() && json.contains("items") && json["items"].is_array()) {
        items = &json["items"];
    }

    if (items == nullptr) {
        outError = "items.json must be an array or object with items[]";
        return false;
    }
    const std::unordered_set<std::string> kAllowedParams = { "range", "speed", "cooldown", "pullForce" };

    for (size_t i = 0; i < items->size(); ++i) {
        const auto& item = (*items)[i];
        if (!item.is_object()) {
            outError = "items[" + std::to_string(i) + "] must be an object";
            return false;
        }

        ExternalModItemDefinition definition;
        if (!ValidateRequiredString(item, "id", definition.id, outError) ||
            !ValidateRequiredString(item, "displayName", definition.displayName, outError)) {
            outError = "items[" + std::to_string(i) + "]: " + outError;
            return false;
        }

        std::string slot;
        if (!ValidateRequiredString(item, "slot", slot, outError)) {
            outError = "items[" + std::to_string(i) + "]: " + outError;
            return false;
        }
        if (slot != "SLOT_HOOKSHOT") {
            outError = "items[" + std::to_string(i) + "]: only SLOT_HOOKSHOT is supported in MVP";
            return false;
        }
        definition.slot = ExternalModItemSlot::Hookshot;

        if (item.contains("iconAsset")) {
            if (!item["iconAsset"].is_string()) {
                outError = "items[" + std::to_string(i) + "].iconAsset must be string";
                return false;
            }
            definition.iconAsset = item["iconAsset"].get<std::string>();
        }

        if (!item.contains("behavior") || !item["behavior"].is_object()) {
            outError = "items[" + std::to_string(i) + "] requires behavior object";
            return false;
        }

        const auto& behavior = item["behavior"];
        if (!ValidateRequiredString(behavior, "exportOnUse", definition.onUseExport, outError)) {
            outError = "items[" + std::to_string(i) + "].behavior.exportOnUse is required";
            return false;
        }
        if (behavior.contains("exportOnUpdate")) {
            if (!behavior["exportOnUpdate"].is_string()) {
                outError = "items[" + std::to_string(i) + "].behavior.exportOnUpdate must be string";
                return false;
            }
            definition.onUpdateExport = behavior["exportOnUpdate"].get<std::string>();
        }

        if (item.contains("params")) {
            if (!item["params"].is_object()) {
                outError = "items[" + std::to_string(i) + "].params must be object";
                return false;
            }
            for (const auto& [key, value] : item["params"].items()) {
                if (!kAllowedParams.contains(key)) {
                    outError = "items[" + std::to_string(i) + "].params has unsupported key: " + key;
                    return false;
                }
                if (!value.is_number()) {
                    outError = "items[" + std::to_string(i) + "].params." + key + " must be numeric";
                    return false;
                }
                definition.params[key] = value.get<float>();
            }
        }

        definition.cooldownFrames =
            static_cast<int32_t>(std::max(0.0f, GetParamOrDefault(definition.params, "cooldown", 0.0f)));
        outItems.push_back(std::move(definition));
    }

    if (outItems.empty()) {
        outError = "items.json must define at least one item";
        return false;
    }

    return true;
}

bool ExternalModManager::TryParseInputDefinitions(const std::string& content,
                                                  std::vector<ExternalModInputBinding>& outBindings,
                                                  std::string& outError) {
    outBindings.clear();

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("input.json parse error: ") + ex.what();
        return false;
    }

    const nlohmann::json* bindings = nullptr;
    if (json.is_array()) {
        bindings = &json;
    } else if (json.is_object() && json.contains("bindings") && json["bindings"].is_array()) {
        bindings = &json["bindings"];
    }

    if (bindings == nullptr) {
        outError = "input.json must be an array or object with bindings[]";
        return false;
    }

    for (size_t i = 0; i < bindings->size(); ++i) {
        const auto& item = (*bindings)[i];
        if (!item.is_object()) {
            outError = "bindings[" + std::to_string(i) + "] must be object";
            return false;
        }

        ExternalModInputBinding binding;
        if (!ValidateRequiredString(item, "id", binding.id, outError)) {
            outError = "bindings[" + std::to_string(i) + "]: " + outError;
            return false;
        }

        if (!item.contains("defaultMask")) {
            outError = "bindings[" + std::to_string(i) + "] requires defaultMask";
            return false;
        }
        if (!ParseButtonMask(item["defaultMask"], binding.defaultMask, outError)) {
            outError = "bindings[" + std::to_string(i) + "].defaultMask: " + outError;
            return false;
        }

        if (item.contains("trigger")) {
            if (!ParseInputTriggerType(item["trigger"], binding.defaultTrigger, outError)) {
                outError = "bindings[" + std::to_string(i) + "].trigger: " + outError;
                return false;
            }
        }

        if (item.contains("allowUserRemap")) {
            if (!item["allowUserRemap"].is_boolean()) {
                outError = "bindings[" + std::to_string(i) + "].allowUserRemap must be boolean";
                return false;
            }
            binding.allowUserRemap = item["allowUserRemap"].get<bool>();
        }

        outBindings.push_back(std::move(binding));
    }

    if (outBindings.empty()) {
        outError = "input.json must define at least one binding";
        return false;
    }

    return true;
}

bool ExternalModManager::ReadManifestFromDirectory(const std::filesystem::path& dirPath, std::string& outContent,
                                                   std::string& outError) {
    return ReadFileFromDirectory(dirPath / "mod.json", kMaxManifestBytes, outContent, outError);
}

bool ExternalModManager::ReadManifestFromZip(const std::filesystem::path& zipPath, std::string& outContent,
                                             std::string& outError) {
    std::vector<char> bytes;
    if (!ReadFileFromZip(zipPath, "mod.json", kMaxManifestBytes, bytes, outError)) {
        return false;
    }
    outContent.assign(bytes.begin(), bytes.end());
    return true;
}

bool ExternalModManager::ReadFileFromDirectory(const std::filesystem::path& filePath, uint64_t maxBytes,
                                               std::string& outContent, std::string& outError) {
    if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath)) {
        outError = "File not found: " + filePath.generic_string();
        return false;
    }
    if (std::filesystem::file_size(filePath) > maxBytes) {
        outError = "File too large: " + filePath.generic_string();
        return false;
    }

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        outError = "Unable to open file: " + filePath.generic_string();
        return false;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    outContent = ss.str();
    return true;
}
bool ExternalModManager::ReadFileFromZip(const std::filesystem::path& zipPath,
                                         const std::filesystem::path& packageRelativePath, uint64_t maxBytes,
                                         std::vector<char>& outBytes, std::string& outError) {
    int err = 0;
    zip_t* archive = zip_open(zipPath.string().c_str(), ZIP_RDONLY, &err);
    if (archive == nullptr) {
        outError = "Unable to open zip archive: " + zipPath.generic_string();
        return false;
    }

    std::string zipEntry = packageRelativePath.generic_string();
    std::string zipEntryBackslash = zipEntry;
    std::replace(zipEntryBackslash.begin(), zipEntryBackslash.end(), '/', '\\');

    zip_stat_t stat;
    zip_stat_init(&stat);
    if (zip_stat(archive, zipEntry.c_str(), ZIP_FL_ENC_GUESS, &stat) != 0) {
        if (zipEntryBackslash == zipEntry || zip_stat(archive, zipEntryBackslash.c_str(), ZIP_FL_ENC_GUESS, &stat) != 0) {
            outError = "Zip entry not found: " + zipEntry;
            zip_close(archive);
            return false;
        }
        zipEntry = zipEntryBackslash;
    }
    if (stat.size > maxBytes) {
        outError = "Zip entry too large: " + zipEntry;
        zip_close(archive);
        return false;
    }

    outBytes.clear();
    outBytes.resize(static_cast<size_t>(stat.size));

    if (stat.size > 0) {
        zip_file_t* file = zip_fopen(archive, zipEntry.c_str(), ZIP_FL_ENC_GUESS);
        if (file == nullptr) {
            outError = "Unable to open zip entry: " + zipEntry;
            zip_close(archive);
            return false;
        }

        uint64_t totalRead = 0;
        while (totalRead < stat.size) {
            const auto toRead = static_cast<zip_uint64_t>(stat.size - totalRead);
            const auto readCount = zip_fread(file, outBytes.data() + totalRead, toRead);
            if (readCount < 0) {
                outError = "Failed reading zip entry: " + zipEntry;
                zip_fclose(file);
                zip_close(archive);
                return false;
            }
            if (readCount == 0) {
                break;
            }
            totalRead += static_cast<uint64_t>(readCount);
        }
        zip_fclose(file);
        outBytes.resize(static_cast<size_t>(totalRead));
    }

    zip_close(archive);
    return true;
}

bool ExternalModManager::ReadFileFromPackage(const ExternalModPackage& package,
                                             const std::filesystem::path& packageRelativePath, uint64_t maxBytes,
                                             std::string& outContent, std::string& outError) {
    if (package.isZip) {
        std::vector<char> bytes;
        if (!ReadFileFromZip(package.sourcePath, packageRelativePath, maxBytes, bytes, outError)) {
            return false;
        }
        outContent.assign(bytes.begin(), bytes.end());
        return true;
    }
    return ReadFileFromDirectory(package.sourcePath / packageRelativePath, maxBytes, outContent, outError);
}

bool ExternalModManager::ReadBinaryFromPackage(const ExternalModPackage& package,
                                               const std::filesystem::path& packageRelativePath, uint64_t maxBytes,
                                               std::vector<uint8_t>& outBytes, std::string& outError) {
    outBytes.clear();

    if (package.isZip) {
        std::vector<char> bytes;
        if (!ReadFileFromZip(package.sourcePath, packageRelativePath, maxBytes, bytes, outError)) {
            return false;
        }
        outBytes.assign(bytes.begin(), bytes.end());
        return true;
    }

    const auto fullPath = package.sourcePath / packageRelativePath;
    if (!std::filesystem::exists(fullPath) || !std::filesystem::is_regular_file(fullPath)) {
        outError = "File not found: " + fullPath.generic_string();
        return false;
    }
    if (std::filesystem::file_size(fullPath) > maxBytes) {
        outError = "File too large: " + fullPath.generic_string();
        return false;
    }

    std::ifstream file(fullPath, std::ios::binary);
    if (!file.is_open()) {
        outError = "Unable to open file: " + fullPath.generic_string();
        return false;
    }

    file.seekg(0, std::ios::end);
    const auto size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    outBytes.resize(size);
    if (size > 0) {
        file.read(reinterpret_cast<char*>(outBytes.data()), static_cast<std::streamsize>(size));
    }
    if (!file.good() && !file.eof()) {
        outError = "Failed to read binary file: " + fullPath.generic_string();
        return false;
    }

    return true;
}

bool ExternalModManager::LoadRuntimeForPackage(ExternalModPackage& package, std::string& outError) {
    std::filesystem::path scriptPath;
    if (!IsSafePackageRelativePath(package.manifest.entryScript, scriptPath, outError)) {
        return false;
    }
    std::string scriptContent;
    if (!ReadFileFromPackage(package, scriptPath, kMaxScriptBytes, scriptContent, outError)) {
        return false;
    }

    ExternalModRuntime runtime;
    if (!TryParseEntryScript(scriptContent, package.manifest.apiVersion, runtime, outError)) {
        return false;
    }
    runtime.apiVersion = package.manifest.apiVersion;

    if (package.manifest.apiVersion >= kExternalModApiVersionV2) {
        std::filesystem::path itemsPath;
        if (!IsSafePackageRelativePath(package.manifest.itemDefinitions, itemsPath, outError)) {
            return false;
        }
        std::string itemContent;
        if (!ReadFileFromPackage(package, itemsPath, kMaxItemDefinitionBytes, itemContent, outError)) {
            return false;
        }
        if (!TryParseItemDefinitions(itemContent, runtime.itemDefinitions, outError)) {
            return false;
        }

        std::filesystem::path inputPath;
        if (!IsSafePackageRelativePath(package.manifest.inputDefinitions, inputPath, outError)) {
            return false;
        }
        std::string inputContent;
        if (!ReadFileFromPackage(package, inputPath, kMaxInputDefinitionBytes, inputContent, outError)) {
            return false;
        }
        if (!TryParseInputDefinitions(inputContent, runtime.inputBindings, outError)) {
            return false;
        }

        for (const auto& inputTrigger : runtime.inputTriggers) {
            const auto bindingIt = std::find_if(runtime.inputBindings.begin(), runtime.inputBindings.end(),
                                                [&inputTrigger](const ExternalModInputBinding& binding) {
                                                    return binding.id == inputTrigger.bindingId;
                                                });
            if (bindingIt == runtime.inputBindings.end()) {
                outError = "onInput references unknown binding: " + inputTrigger.bindingId;
                return false;
            }
        }

        if (package.manifest.runtimeType != "wasm3-v1") {
            outError = "Unsupported runtime.type for apiVersion 2: " + package.manifest.runtimeType;
            return false;
        }
        std::filesystem::path wasmPath;
        if (!IsSafePackageRelativePath(package.manifest.runtimeModule, wasmPath, outError)) {
            outError = "Invalid runtime.module: " + outError;
            return false;
        }

        std::vector<uint8_t> wasmBytes;
        if (!ReadBinaryFromPackage(package, wasmPath, kMaxWasmBytes, wasmBytes, outError)) {
            outError = "Failed to read runtime.module: " + outError;
            return false;
        }

        runtime.wasmRuntime = std::make_unique<ExternalModWasmRuntime>();
        ExternalModWasmConfig config;
        config.modId = package.manifest.id;
        config.maxMemoryKb = package.manifest.runtimeMaxMemoryKb;
        config.maxCallMs = package.manifest.runtimeMaxCallMs;

        if (!runtime.wasmRuntime->Initialize(wasmBytes, config, outError)) {
            return false;
        }
    }

    package.runtime = std::move(runtime);
    return true;
}

bool ExternalModManager::IsSafePackageRelativePath(const std::string& pathValue, std::filesystem::path& outNormalizedPath,
                                                   std::string& outError) {
    if (pathValue.empty()) {
        outError = "path must not be empty";
        return false;
    }
    if (pathValue.find('\0') != std::string::npos || pathValue.find(':') != std::string::npos) {
        outError = "path contains forbidden characters";
        return false;
    }

    const std::filesystem::path path(pathValue);
    if (path.has_root_name() || path.has_root_directory() || path.is_absolute()) {
        outError = "path must be relative";
        return false;
    }

    const auto normalized = path.lexically_normal();
    for (const auto& part : normalized) {
        if (part.generic_string() == "..") {
            outError = "path traversal is not allowed";
            return false;
        }
    }
    outNormalizedPath = normalized;
    return true;
}

bool ExternalModManager::MountAssetsForPackage(ExternalModPackage& package, std::string& outError) {
    if (package.manifest.assets.empty()) {
        return true;
    }

    std::vector<std::filesystem::path> preparedAssets;
    preparedAssets.reserve(package.manifest.assets.size());

    if (!package.isZip) {
        for (size_t i = 0; i < package.manifest.assets.size(); ++i) {
            std::filesystem::path assetRelativePath;
            if (!IsSafePackageRelativePath(package.manifest.assets[i], assetRelativePath, outError)) {
                outError = "Invalid assets[" + std::to_string(i) + "]: " + outError;
                return false;
            }
            const auto fullAssetPath = package.sourcePath / assetRelativePath;
            if (!std::filesystem::exists(fullAssetPath) || !std::filesystem::is_regular_file(fullAssetPath)) {
                outError = "Asset not found: " + assetRelativePath.generic_string();
                return false;
            }
            if (!IsPathWithinBase(package.sourcePath, fullAssetPath)) {
                outError = "Asset escapes package directory: " + assetRelativePath.generic_string();
                return false;
            }
            preparedAssets.push_back(fullAssetPath.lexically_normal());
        }
    } else {
        const auto cacheRoot = std::filesystem::path(
            Ship::Context::GetPathRelativeToAppDirectory("mods/.external-mod-cache", "soh"));
        const auto packageCacheRoot = cacheRoot / SanitizeModIdForPath(package.manifest.id);
        std::error_code ec;
        std::filesystem::remove_all(packageCacheRoot, ec);
        std::filesystem::create_directories(packageCacheRoot, ec);
        if (ec) {
            outError = "Failed to create package cache directory";
            return false;
        }

        for (size_t i = 0; i < package.manifest.assets.size(); ++i) {
            std::filesystem::path assetRelativePath;
            if (!IsSafePackageRelativePath(package.manifest.assets[i], assetRelativePath, outError)) {
                outError = "Invalid assets[" + std::to_string(i) + "]: " + outError;
                return false;
            }

            std::vector<char> assetBytes;
            if (!ReadFileFromZip(package.sourcePath, assetRelativePath, kMaxAssetBytes, assetBytes, outError)) {
                outError = "Failed to extract assets[" + std::to_string(i) + "]: " + outError;
                return false;
            }

            const auto extractedPath = packageCacheRoot / assetRelativePath;
            std::filesystem::create_directories(extractedPath.parent_path(), ec);
            if (ec) {
                outError = "Failed to create extracted asset directory";
                return false;
            }

            std::ofstream output(extractedPath, std::ios::binary | std::ios::trunc);
            if (!output.is_open()) {
                outError = "Unable to write extracted asset: " + extractedPath.generic_string();
                return false;
            }
            if (!assetBytes.empty()) {
                output.write(assetBytes.data(), static_cast<std::streamsize>(assetBytes.size()));
            }
            if (!output.good()) {
                outError = "Failed writing extracted asset: " + extractedPath.generic_string();
                return false;
            }
            preparedAssets.push_back(extractedPath.lexically_normal());
        }
    }

    auto context = Ship::Context::GetInstance();
    if (context == nullptr || context->GetResourceManager() == nullptr ||
        context->GetResourceManager()->GetArchiveManager() == nullptr) {
        outError = "Archive manager unavailable";
        return false;
    }

    for (const auto& assetPath : preparedAssets) {
        context->GetResourceManager()->GetArchiveManager()->AddArchive(assetPath.generic_string());
        package.mountedAssets.push_back(assetPath);
    }

    return true;
}
void ExternalModManager::ExecuteActions(ExternalModPackage& package, const std::vector<ExternalModAction>& actions,
                                        const char* triggerName) {
    for (const auto& action : actions) {
        if (!package.runtime.enabled) {
            return;
        }

        switch (action.type) {
            case ExternalModActionType::ShowNotification:
                Notification::Emit({
                    .message = "[" + package.manifest.name + "] " + action.text,
                    .remainingTime = 8.0f,
                });
                break;
            case ExternalModActionType::TeleportToEntrance:
                if (gPlayState != nullptr) {
                    gPlayState->nextEntranceIndex = action.entranceIndex;
                    gPlayState->transitionTrigger = TRANS_TRIGGER_START;
                    gPlayState->transitionType = TRANS_TYPE_FADE_BLACK;
                    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK_FAST;
                    SPDLOG_INFO("[ExternalMods] {} triggered teleport via {}", package.manifest.id, triggerName);
                }
                break;
            case ExternalModActionType::PressButton:
                if (gPlayState != nullptr && action.buttonMask != 0) {
                    auto* input = &gPlayState->state.input[0];
                    const auto mask = static_cast<uint16_t>(action.buttonMask & 0xFFFF);
                    input->press.button |= mask;
                    input->cur.button |= mask;
                }
                break;
            case ExternalModActionType::GrantModItem: {
                auto itemIt = std::find_if(package.runtime.itemDefinitions.begin(), package.runtime.itemDefinitions.end(),
                                           [&action](const ExternalModItemDefinition& item) {
                                               return item.id == action.itemId;
                                           });
                if (itemIt == package.runtime.itemDefinitions.end()) {
                    DisableRuntime(package, "grantModItem references unknown item id: " + action.itemId);
                    return;
                }
                itemIt->granted = true;
                itemIt->cooldownRemaining = 0;
                break;
            }
            case ExternalModActionType::RevokeModItem: {
                auto itemIt = std::find_if(package.runtime.itemDefinitions.begin(), package.runtime.itemDefinitions.end(),
                                           [&action](const ExternalModItemDefinition& item) {
                                               return item.id == action.itemId;
                                           });
                if (itemIt == package.runtime.itemDefinitions.end()) {
                    DisableRuntime(package, "revokeModItem references unknown item id: " + action.itemId);
                    return;
                }
                itemIt->granted = false;
                itemIt->cooldownRemaining = 0;
                break;
            }
            case ExternalModActionType::InvokeWasm: {
                if (!package.runtime.wasmRuntime) {
                    DisableRuntime(package, "invokeWasm requested but runtime is unavailable");
                    return;
                }
                std::string wasmError;
                if (!package.runtime.wasmRuntime->InvokeExport(action.exportName, action.args, wasmError)) {
                    DisableRuntime(package, "invokeWasm failed for export '" + action.exportName + "': " + wasmError);
                    return;
                }
                break;
            }
            default:
                break;
        }
    }
}

void ExternalModManager::DisableRuntime(ExternalModPackage& package, const std::string& reason) {
    package.runtime.enabled = false;
    package.valid = false;
    package.error = reason;
    SPDLOG_ERROR("[ExternalMods] Disabled runtime for {}: {}", package.manifest.id, reason);
}

void ExternalModManager::RegisterHooks() {
    if (GameInteractor::Instance == nullptr) {
        SPDLOG_WARN("[ExternalMods] GameInteractor is unavailable; hooks were not registered");
        return;
    }

    UnregisterHooks();

    mOnLoadGameHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>(
        [](int32_t fileNum) { ExternalModManager::Instance().OnLoadGame(fileNum); });
    mOnSceneInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>(
        [](int16_t sceneNum) { ExternalModManager::Instance().OnSceneInit(sceneNum); });
    mOnGameFrameHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>(
        []() { ExternalModManager::Instance().OnGameFrameUpdate(); });
    mOnPlayerUseItemHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUseItem>(
        [](void* player, int32_t itemId, bool* allowVanilla) {
            ExternalModManager::Instance().OnPlayerUseItem(player, itemId, allowVanilla);
        });
}

void ExternalModManager::UnregisterHooks() {
    if (GameInteractor::Instance != nullptr) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnLoadGame>(mOnLoadGameHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(mOnSceneInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnGameFrameUpdate>(mOnGameFrameHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUseItem>(mOnPlayerUseItemHook);
    }
    mOnLoadGameHook = 0;
    mOnSceneInitHook = 0;
    mOnGameFrameHook = 0;
    mOnPlayerUseItemHook = 0;
}

void ExternalModManager::OnLoadGame(int32_t fileNum) {
    (void)fileNum;
    for (auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }
        try {
            for (auto& trigger : package.runtime.frameTriggers) {
                trigger.wasInside = false;
                trigger.cooldownRemaining = 0;
            }
            for (auto& inputTrigger : package.runtime.inputTriggers) {
                inputTrigger.cooldownRemaining = 0;
            }
            for (auto& item : package.runtime.itemDefinitions) {
                item.granted = false;
                item.cooldownRemaining = 0;
            }

            ExecuteActions(package, package.runtime.onGameLoadedActions, "onGameLoaded");
        } catch (const std::exception& ex) {
            DisableRuntime(package, std::string("Unhandled exception on onGameLoaded: ") + ex.what());
        } catch (...) {
            DisableRuntime(package, "Unhandled exception on onGameLoaded");
        }
    }
}

void ExternalModManager::OnSceneInit(int16_t sceneNum) {
    for (auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }
        try {
            for (auto& trigger : package.runtime.frameTriggers) {
                trigger.wasInside = false;
                trigger.cooldownRemaining = 0;
            }
            for (const auto& sceneAction : package.runtime.onSceneInitActions) {
                if (sceneAction.sceneId == sceneNum) {
                    ExecuteActions(package, sceneAction.actions, "onSceneInit");
                }
            }
        } catch (const std::exception& ex) {
            DisableRuntime(package, std::string("Unhandled exception on onSceneInit: ") + ex.what());
        } catch (...) {
            DisableRuntime(package, "Unhandled exception on onSceneInit");
        }
    }
}
void ExternalModManager::OnGameFrameUpdate() {
    if (gPlayState == nullptr) {
        return;
    }
    auto* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return;
    }

    const auto sceneNum = static_cast<int16_t>(gPlayState->sceneNum);
    const auto playerPos = player->actor.world.pos;
    auto* input = &gPlayState->state.input[0];

    for (auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }
        try {
            for (auto& trigger : package.runtime.frameTriggers) {
                if (trigger.sceneId != sceneNum) {
                    trigger.wasInside = false;
                    continue;
                }
                if (trigger.cooldownRemaining > 0) {
                    trigger.cooldownRemaining--;
                }

                const bool inside = IsWithinAabb(trigger, playerPos);
                if (inside && !trigger.wasInside && trigger.cooldownRemaining == 0) {
                    ExecuteActions(package, trigger.actions, trigger.id.c_str());
                    trigger.cooldownRemaining = trigger.cooldownFrames;
                }
                trigger.wasInside = inside;
            }

            for (auto& inputTrigger : package.runtime.inputTriggers) {
                if (inputTrigger.cooldownRemaining > 0) {
                    inputTrigger.cooldownRemaining--;
                }

                const auto bindingIt = std::find_if(package.runtime.inputBindings.begin(),
                                                    package.runtime.inputBindings.end(),
                                                    [&inputTrigger](const ExternalModInputBinding& binding) {
                                                        return binding.id == inputTrigger.bindingId;
                                                    });
                if (bindingIt == package.runtime.inputBindings.end()) {
                    DisableRuntime(package, "onInput references unknown binding at runtime: " + inputTrigger.bindingId);
                    break;
                }

                const auto cvarName = BuildBindingCVarName(package.manifest.id, bindingIt->id);
                int32_t effectiveMask = bindingIt->allowUserRemap ? CVarGetInteger(cvarName.c_str(), bindingIt->defaultMask)
                                                                  : bindingIt->defaultMask;
                if (effectiveMask == 0) {
                    effectiveMask = bindingIt->defaultMask;
                }

                bool active = false;
                switch (inputTrigger.trigger) {
                    case ExternalModInputTriggerType::Pressed:
                        active = MatchButtonMask(input->press.button, effectiveMask) &&
                                 MatchButtonMask(input->cur.button, effectiveMask);
                        break;
                    case ExternalModInputTriggerType::Held:
                        active = MatchButtonMask(input->cur.button, effectiveMask);
                        break;
                    case ExternalModInputTriggerType::Released:
                        active = MatchButtonMask(input->rel.button, effectiveMask);
                        break;
                    default:
                        break;
                }

                if (active && inputTrigger.cooldownRemaining == 0) {
                    ExecuteActions(package, inputTrigger.actions, inputTrigger.id.c_str());
                    inputTrigger.cooldownRemaining = inputTrigger.cooldownFrames;
                }
            }

            for (auto& item : package.runtime.itemDefinitions) {
                if (item.cooldownRemaining > 0) {
                    item.cooldownRemaining--;
                }
                if (!item.granted || item.onUpdateExport.empty()) {
                    continue;
                }
                if (!package.runtime.wasmRuntime) {
                    continue;
                }

                std::string wasmError;
                if (!package.runtime.wasmRuntime->InvokeExport(item.onUpdateExport, wasmError)) {
                    DisableRuntime(package, "WASM onUpdate failed for item '" + item.id + "': " + wasmError);
                    break;
                }
            }
        } catch (const std::exception& ex) {
            DisableRuntime(package, std::string("Unhandled exception on onFrame: ") + ex.what());
        } catch (...) {
            DisableRuntime(package, "Unhandled exception on onFrame");
        }
    }
}

void ExternalModManager::OnPlayerUseItem(void* player, int32_t itemId, bool* allowVanilla) {
    if (allowVanilla == nullptr || !*allowVanilla) {
        return;
    }
    if (!ExternalModItemRuntime::IsHookshotItemId(itemId)) {
        return;
    }

    for (auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }

        for (auto& item : package.runtime.itemDefinitions) {
            if (!item.granted || item.slot != ExternalModItemSlot::Hookshot) {
                continue;
            }

            if (item.cooldownRemaining > 0) {
                *allowVanilla = false;
                return;
            }

            if (package.runtime.wasmRuntime && !item.onUseExport.empty()) {
                std::vector<int32_t> args = { itemId };
                std::string wasmError;
                if (!package.runtime.wasmRuntime->InvokeExport(item.onUseExport, args, wasmError)) {
                    DisableRuntime(package, "WASM onUse failed for item '" + item.id + "': " + wasmError);
                    *allowVanilla = true;
                    return;
                }
            }

            const float pullForce = GetParamOrDefault(item.params, "pullForce", 8.0f);
            const float speed = GetParamOrDefault(item.params, "speed", 12.0f);
            ExternalModItemRuntime::ApplySkyhookImpulse(player, pullForce, speed);

            const auto cooldown = static_cast<int32_t>(
                std::max(0.0f, GetParamOrDefault(item.params, "cooldown", static_cast<float>(item.cooldownFrames))));
            item.cooldownFrames = cooldown;
            item.cooldownRemaining = cooldown;

            *allowVanilla = false;
            return;
        }
    }
}

} // namespace SOH
