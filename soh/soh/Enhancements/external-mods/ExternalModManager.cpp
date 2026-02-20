#include "ExternalModManager.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cctype>
#include <cstring>
#include <functional>
#include <fstream>
#include <limits>
#include <map>
#include <queue>
#include <regex>
#include <sstream>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <zip.h>
#include <stb_image.h>

#include <ship/Context.h>
#include <ship/resource/File.h>
#include <ship/resource/archive/Archive.h>
#include <fast/resource/type/DisplayList.h>
#include <fast/resource/ResourceType.h>

#include "ExternalModItemRuntime.h"
#include "ExternalModWatCompiler.h"
#include "ExternalModWasmRuntime.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Notification/Notification.h"
#include "soh/OTRGlobals.h"
#include "soh/ResourceManagerHelpers.h"
#include "objects/object_link_boy/object_link_boy.h"

extern "C" {
#include <z64.h>
#include "macros.h"
#include "variables.h"
#include "functions.h"

GetItemEntry ItemTable_Retrieve(int16_t getItemID);
GetItemID RetrieveGetItemIDFromItemID(ItemID itemID);

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
constexpr uint64_t kMaxWatSourceBytes = 4ull * 1024ull * 1024ull;
constexpr uint64_t kMaxItemDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxInputDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxActorDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxHookDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxItemIconBytes = 4ull * 1024ull * 1024ull;
constexpr uint64_t kMaxItemModelBytes = 8ull * 1024ull * 1024ull;
constexpr uint64_t kMaxItemModelTextureBytes = 16ull * 1024ull * 1024ull;
constexpr uint64_t kMaxObjMaterialBytes = 512 * 1024;
constexpr uint64_t kMaxHookshotTextureBytes = 4ull * 1024ull * 1024ull;
constexpr int32_t kItemIconSize = 32;
constexpr int32_t kItemModelTextureSize = 32;
constexpr int32_t kMaxDecodedIconDimension = 2048;
constexpr int32_t kHookshotMetalTextureWidth = 8;
constexpr int32_t kHookshotMetalTextureHeight = 8;
constexpr int32_t kHookshotHandleTextureWidth = 16;
constexpr int32_t kHookshotHandleTextureHeight = 8;
constexpr int32_t kHookshotDesignTextureWidth = 16;
constexpr int32_t kHookshotDesignTextureHeight = 32;
constexpr int32_t kHookshotChainTextureWidth = 16;
constexpr int32_t kHookshotChainTextureHeight = 32;
constexpr int32_t kHookshotReticleTextureWidth = 64;
constexpr int32_t kHookshotReticleTextureHeight = 64;
constexpr size_t kMaxItemModelTriangles = 4096;
constexpr float kMinDisplayListModelScale = 0.05f;
constexpr float kMaxDisplayListModelScale = 2000.0f;
constexpr int32_t kDefaultTriggerCooldownFrames = 90;
constexpr int32_t kDefaultRuntimeMemoryKb = 1024;
constexpr int32_t kDefaultRuntimeCallMs = 2;
constexpr int32_t kDefaultRuntimeFrameBudgetMs = 2;
constexpr int32_t kDefaultRuntimeHookCallsPerFrame = 256;
constexpr int32_t kDefaultRuntimeActorInstances = 64;
constexpr u8 kAgeReqAdult = LINK_AGE_ADULT;
constexpr u8 kAgeReqChild = LINK_AGE_CHILD;
constexpr u8 kAgeReqNone = 9;
constexpr size_t kItemIconTableSize = sizeof(gItemIcons) / sizeof(gItemIcons[0]);
constexpr size_t kExternalModExtraInventoryCellCount = 40;
std::array<void*, kItemIconTableSize> gVanillaItemIcons{};
bool gVanillaItemIconsCaptured = false;

struct ExternalModHookshotTexturePatchRecord {
    std::string displayListPath;
    std::string patchName;
};

std::vector<ExternalModHookshotTexturePatchRecord> gExternalModHookshotTexturePatches{};
std::string gExternalModHookshotTextureOverrideKey;
std::unordered_set<std::string> gExternalModMissingDisplayListWarnings{};
std::unordered_set<std::string> gExternalModDisplayListDrawDebugLogs{};

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
    { "BTN_CUSTOM_MOD_ACTION1", BTN_CUSTOM_MOD_ACTION1 },
    { "BTN_CUSTOM_MOD_ACTION2", BTN_CUSTOM_MOD_ACTION2 },
    { "BTN_CUSTOM_MOD_ACTION3", BTN_CUSTOM_MOD_ACTION3 },
    { "BTN_CUSTOM_MOD_ACTION4", BTN_CUSTOM_MOD_ACTION4 },
    { "BTN_CUSTOM_MOD_ACTION5", BTN_CUSTOM_MOD_ACTION5 },
    { "BTN_CUSTOM_MOD_ACTION6", BTN_CUSTOM_MOD_ACTION6 },
    { "BTN_CUSTOM_MOD_ACTION7", BTN_CUSTOM_MOD_ACTION7 },
    { "MOD_ACTION1", BTN_CUSTOM_MOD_ACTION1 },
    { "MOD_ACTION2", BTN_CUSTOM_MOD_ACTION2 },
    { "MOD_ACTION3", BTN_CUSTOM_MOD_ACTION3 },
    { "MOD_ACTION4", BTN_CUSTOM_MOD_ACTION4 },
    { "MOD_ACTION5", BTN_CUSTOM_MOD_ACTION5 },
    { "MOD_ACTION6", BTN_CUSTOM_MOD_ACTION6 },
    { "MOD_ACTION7", BTN_CUSTOM_MOD_ACTION7 },
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

struct EquippedActionButtonMapping {
    int32_t mask;
    int32_t slotIndex;
    const char* alias;
};

constexpr std::array<EquippedActionButtonMapping, 7> kEquippedActionButtonMappings = {{
    { BTN_CLEFT, 1, "BTN_CLEFT" },
    { BTN_CDOWN, 2, "BTN_CDOWN" },
    { BTN_CRIGHT, 3, "BTN_CRIGHT" },
    { BTN_DUP, 4, "BTN_DUP" },
    { BTN_DDOWN, 5, "BTN_DDOWN" },
    { BTN_DLEFT, 6, "BTN_DLEFT" },
    { BTN_DRIGHT, 7, "BTN_DRIGHT" },
}};

bool TryResolveEquippedActionButtonMask(int32_t inputMask, int32_t& outResolvedMask, int32_t& outSlotIndex) {
    outResolvedMask = 0;
    outSlotIndex = 0;

    int32_t matches = 0;
    const auto normalizedMask = static_cast<uint32_t>(inputMask);
    for (const auto& mapping : kEquippedActionButtonMappings) {
        if ((normalizedMask & static_cast<uint32_t>(mapping.mask)) != 0) {
            outResolvedMask = mapping.mask;
            outSlotIndex = mapping.slotIndex;
            ++matches;
        }
    }

    return matches == 1;
}

std::string BuildSupportedEquippedActionButtonList() {
    std::stringstream ss;
    for (size_t i = 0; i < kEquippedActionButtonMappings.size(); ++i) {
        if (i > 0) {
            ss << '|';
        }
        ss << kEquippedActionButtonMappings[i].alias;
    }
    return ss.str();
}

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

ExternalModRuntimeModuleFormat ResolveRuntimeModuleFormat(const std::filesystem::path& modulePath) {
    const auto extension = ToLower(modulePath.extension().string());
    if (extension == ".wasm") {
        return ExternalModRuntimeModuleFormat::WasmBinary;
    }
    if (extension == ".wat") {
        return ExternalModRuntimeModuleFormat::WatText;
    }
    return ExternalModRuntimeModuleFormat::WasmBinary;
}

bool IsSupportedRuntimeModuleExtension(const std::filesystem::path& modulePath) {
    const auto extension = ToLower(modulePath.extension().string());
    return extension == ".wasm" || extension == ".wat";
}

const std::unordered_set<std::string> kSupportedCapabilities = {
    "hooks.extended.v1",
    "items.data.v2",
    "actors.vm.v1",
    "behaviors.graph.v1",
    "actors.generic.v1",
    "items.catalog.v1",
    "scenes.bundle.v1",
    "render.filter_override.v1",
};

const std::unordered_map<std::string, ExternalModHookType> kHookAliases = {
    { "onloadgame", ExternalModHookType::OnLoadGame },
    { "onexitgame", ExternalModHookType::OnExitGame },
    { "onsceneinit", ExternalModHookType::OnSceneInit },
    { "afterscenecommands", ExternalModHookType::AfterSceneCommands },
    { "ontransitionend", ExternalModHookType::OnTransitionEnd },
    { "onflagset", ExternalModHookType::OnFlagSet },
    { "onflagunset", ExternalModHookType::OnFlagUnset },
    { "onsceneflagset", ExternalModHookType::OnSceneFlagSet },
    { "onsceneflagunset", ExternalModHookType::OnSceneFlagUnset },
    { "onplayerupdate", ExternalModHookType::OnPlayerUpdate },
    { "onplayeruseitem", ExternalModHookType::OnPlayerUseItem },
    { "onplayerhealthchange", ExternalModHookType::OnPlayerHealthChange },
    { "onitemreceive", ExternalModHookType::OnItemReceive },
    { "onactorinit", ExternalModHookType::OnActorInit },
    { "onactorspawn", ExternalModHookType::OnActorSpawn },
    { "onactorupdate", ExternalModHookType::OnActorUpdate },
    { "onactorkill", ExternalModHookType::OnActorKill },
    { "onactordestroy", ExternalModHookType::OnActorDestroy },
    { "onenemydefeat", ExternalModHookType::OnEnemyDefeat },
    { "onbossdefeat", ExternalModHookType::OnBossDefeat },
    { "onplaydestroy", ExternalModHookType::OnPlayDestroy },
    { "ongameframeupdate", ExternalModHookType::OnGameFrameUpdate },
};
const std::unordered_map<std::string, ExternalModItemAgePolicy> kItemAgePolicyAliases = {
    { "respectvanilla", ExternalModItemAgePolicy::RespectVanilla },
    { "allowchild", ExternalModItemAgePolicy::AllowChild },
    { "allowadult", ExternalModItemAgePolicy::AllowAdult },
};

const std::unordered_map<std::string, ExternalModItemUseMode> kItemUseModeAliases = {
    { "vanilla", ExternalModItemUseMode::Vanilla },
    { "override", ExternalModItemUseMode::Override },
    { "augment", ExternalModItemUseMode::Augment },
};

const std::unordered_map<std::string, ExternalModModelUvOrigin> kModelUvOriginAliases = {
    { "auto", ExternalModModelUvOrigin::Auto },
    { "bottomleft", ExternalModModelUvOrigin::BottomLeft },
    { "bottom_left", ExternalModModelUvOrigin::BottomLeft },
    { "topleft", ExternalModModelUvOrigin::TopLeft },
    { "top_left", ExternalModModelUvOrigin::TopLeft },
};

const std::unordered_map<std::string, ExternalModModelTextureFilter> kModelTextureFilterAliases = {
    { "auto", ExternalModModelTextureFilter::Auto },
    { "point", ExternalModModelTextureFilter::Point },
    { "bilerp", ExternalModModelTextureFilter::Bilerp },
    { "bilinear", ExternalModModelTextureFilter::Bilerp },
};

const std::unordered_map<std::string, int32_t> kItemIdAliases = {
    { "ITEM_NONE", ITEM_NONE },
    { "ITEM_HOOKSHOT", ITEM_HOOKSHOT },
    { "ITEM_LONGSHOT", ITEM_LONGSHOT },
    { "ITEM_BOW", ITEM_BOW },
    { "ITEM_HAMMER", ITEM_HAMMER },
    { "ITEM_ARROW_FIRE", ITEM_ARROW_FIRE },
    { "ITEM_ARROW_ICE", ITEM_ARROW_ICE },
    { "ITEM_ARROW_LIGHT", ITEM_ARROW_LIGHT },
    { "ITEM_BOW_ARROW_FIRE", ITEM_BOW_ARROW_FIRE },
    { "ITEM_BOW_ARROW_ICE", ITEM_BOW_ARROW_ICE },
    { "ITEM_BOW_ARROW_LIGHT", ITEM_BOW_ARROW_LIGHT },
};

bool ManifestHasCapability(const ExternalModManifest& manifest, const char* capability) {
    return std::find(manifest.capabilities.begin(), manifest.capabilities.end(), capability) != manifest.capabilities.end();
}

bool ParseHookTypeAlias(const std::string& value, ExternalModHookType& outHookType) {
    const auto normalized = ToLower(value);
    const auto it = kHookAliases.find(normalized);
    if (it == kHookAliases.end()) {
        return false;
    }
    outHookType = it->second;
    return true;
}

bool ParseItemAgePolicy(const nlohmann::json& value, ExternalModItemAgePolicy& outPolicy, std::string& outError) {
    if (!value.is_string()) {
        outError = "must be string";
        return false;
    }

    const auto normalized = ToLower(value.get<std::string>());
    const auto it = kItemAgePolicyAliases.find(normalized);
    if (it == kItemAgePolicyAliases.end()) {
        outError = "unsupported value: " + value.get<std::string>() + " (expected respectVanilla|allowChild|allowAdult)";
        return false;
    }

    outPolicy = it->second;
    return true;
}

bool ParseItemUseMode(const nlohmann::json& value, ExternalModItemUseMode& outMode, std::string& outError) {
    if (!value.is_string()) {
        outError = "must be string";
        return false;
    }

    const auto normalized = ToLower(value.get<std::string>());
    const auto it = kItemUseModeAliases.find(normalized);
    if (it == kItemUseModeAliases.end()) {
        outError = "unsupported value: " + value.get<std::string>() + " (expected vanilla|override|augment)";
        return false;
    }

    outMode = it->second;
    return true;
}

bool ParseModelUvOrigin(const nlohmann::json& value, ExternalModModelUvOrigin& outOrigin, std::string& outError) {
    if (!value.is_string()) {
        outError = "must be string";
        return false;
    }

    const auto normalized = ToLower(value.get<std::string>());
    const auto it = kModelUvOriginAliases.find(normalized);
    if (it == kModelUvOriginAliases.end()) {
        outError = "unsupported value: " + value.get<std::string>() +
                   " (expected auto|bottom_left|top_left)";
        return false;
    }

    outOrigin = it->second;
    return true;
}

bool ParseModelTextureFilter(const nlohmann::json& value, ExternalModModelTextureFilter& outFilter,
                             std::string& outError) {
    if (!value.is_string()) {
        outError = "must be string";
        return false;
    }

    const auto normalized = ToLower(value.get<std::string>());
    const auto it = kModelTextureFilterAliases.find(normalized);
    if (it == kModelTextureFilterAliases.end()) {
        outError = "unsupported value: " + value.get<std::string>() + " (expected auto|point|bilerp|bilinear)";
        return false;
    }

    outFilter = it->second;
    return true;
}

bool ParseItemIdValue(const nlohmann::json& value, int32_t& outItemId, std::string& outError) {
    if (value.is_number_integer()) {
        outItemId = value.get<int32_t>();
        return true;
    }

    if (!value.is_string()) {
        outError = "must be integer or string alias";
        return false;
    }

    const auto stringValue = value.get<std::string>();
    const auto aliasIt = kItemIdAliases.find(ToUpper(stringValue));
    if (aliasIt != kItemIdAliases.end()) {
        outItemId = aliasIt->second;
        return true;
    }

    try {
        size_t parsedLength = 0;
        const auto parsed = std::stoll(stringValue, &parsedLength, 0);
        if (parsedLength == stringValue.size() && parsed >= std::numeric_limits<int32_t>::min() &&
            parsed <= std::numeric_limits<int32_t>::max()) {
            outItemId = static_cast<int32_t>(parsed);
            return true;
        }
    } catch (...) {
    }

    outError = "unsupported item id alias: " + stringValue;
    return false;
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

std::string TrimStringCopy(const std::string& value) {
    size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }
    size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }
    return value.substr(start, end - start);
}

bool EndsWithString(const std::string& value, const std::string& suffix) {
    if (suffix.size() > value.size()) {
        return false;
    }
    return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
}

std::string NormalizeZipEntryPath(std::string path) {
    std::replace(path.begin(), path.end(), '\\', '/');
    while (!path.empty() && path.front() == '/') {
        path.erase(path.begin());
    }
    return path;
}

std::vector<std::string> SplitTopLevelCommas(const std::string& text) {
    std::vector<std::string> args;
    std::string chunk;
    int32_t depth = 0;

    for (const char ch : text) {
        if (ch == '(') {
            depth++;
        } else if (ch == ')' && depth > 0) {
            depth--;
        }

        if (ch == ',' && depth == 0) {
            args.push_back(TrimStringCopy(chunk));
            chunk.clear();
            continue;
        }

        chunk.push_back(ch);
    }

    if (!chunk.empty()) {
        args.push_back(TrimStringCopy(chunk));
    }

    return args;
}

std::vector<std::string> ParseOrTokens(const std::string& expression) {
    std::string raw = expression;
    raw.erase(std::remove(raw.begin(), raw.end(), '('), raw.end());
    raw.erase(std::remove(raw.begin(), raw.end(), ')'), raw.end());

    std::vector<std::string> tokens;
    std::stringstream ss(raw);
    std::string part;
    while (std::getline(ss, part, '|')) {
        part = TrimStringCopy(part);
        if (!part.empty()) {
            tokens.push_back(part);
        }
    }
    return tokens;
}

bool TryParseIntToken(const std::string& token, int32_t& outValue) {
    static const std::unordered_map<std::string, int32_t> kAliases = {
        { "G_TX_RENDERTILE", 0 },
        { "G_TX_LOADTILE", 7 },
        { "G_ON", 1 },
        { "G_OFF", 0 },
        { "G_TX_NOMASK", 0 },
        { "G_TX_NOLOD", 0 },
        { "G_IM_FMT_RGBA", G_IM_FMT_RGBA },
        { "G_IM_FMT_YUV", G_IM_FMT_YUV },
        { "G_IM_FMT_CI", G_IM_FMT_CI },
        { "G_IM_FMT_IA", G_IM_FMT_IA },
        { "G_IM_FMT_I", G_IM_FMT_I },
        { "G_IM_SIZ_4b", G_IM_SIZ_4b },
        { "G_IM_SIZ_8b", G_IM_SIZ_8b },
        { "G_IM_SIZ_16b", G_IM_SIZ_16b },
        { "G_IM_SIZ_32b", G_IM_SIZ_32b },
        { "G_TL_TILE", G_TL_TILE },
        { "G_TL_LOD", G_TL_LOD },
        { "G_CYC_1CYCLE", G_CYC_1CYCLE },
        { "G_CYC_2CYCLE", G_CYC_2CYCLE },
        { "G_CYC_COPY", G_CYC_COPY },
        { "G_CYC_FILL", G_CYC_FILL },
        { "G_PM_1PRIMITIVE", G_PM_1PRIMITIVE },
        { "G_PM_NPRIMITIVE", G_PM_NPRIMITIVE },
        { "G_TD_CLAMP", G_TD_CLAMP },
        { "G_TD_SHARPEN", G_TD_SHARPEN },
        { "G_TD_DETAIL", G_TD_DETAIL },
        { "G_TP_NONE", G_TP_NONE },
        { "G_TP_PERSP", G_TP_PERSP },
        { "G_CK_NONE", G_CK_NONE },
        { "G_CK_KEY", G_CK_KEY },
        { "G_CD_MAGICSQ", G_CD_MAGICSQ },
        { "G_CD_BAYER", G_CD_BAYER },
        { "G_CD_NOISE", G_CD_NOISE },
        { "G_CD_DISABLE", G_CD_DISABLE },
        { "G_AC_NONE", G_AC_NONE },
        { "G_AC_THRESHOLD", G_AC_THRESHOLD },
        { "G_AC_DITHER", G_AC_DITHER },
        { "G_ZS_PIXEL", G_ZS_PIXEL },
        { "G_ZS_PRIM", G_ZS_PRIM },
    };

    const auto trimmed = TrimStringCopy(token);
    if (trimmed.empty()) {
        return false;
    }

    const auto aliasIt = kAliases.find(trimmed);
    if (aliasIt != kAliases.end()) {
        outValue = aliasIt->second;
        return true;
    }

    try {
        size_t parsedLength = 0;
        const auto parsed = std::stoll(trimmed, &parsedLength, 0);
        if (parsedLength != trimmed.size() || parsed < std::numeric_limits<int32_t>::min() ||
            parsed > std::numeric_limits<int32_t>::max()) {
            return false;
        }
        outValue = static_cast<int32_t>(parsed);
        return true;
    } catch (...) {
        return false;
    }
}

bool TryParseAlphaDitherModeToken(const std::string& token, int32_t& outValue) {
    static const std::unordered_map<std::string, int32_t> kAlphaDitherAliases = {
        { "G_AD_PATTERN", G_AD_PATTERN },
        { "G_AD_NOTPATTERN", G_AD_NOTPATTERN },
        { "G_AD_NOISE", G_AD_NOISE },
        { "G_AD_DISABLE", G_AD_DISABLE },
    };

    const auto normalized = TrimStringCopy(token);
    const auto it = kAlphaDitherAliases.find(normalized);
    if (it != kAlphaDitherAliases.end()) {
        outValue = it->second;
        return true;
    }

    return TryParseIntToken(normalized, outValue);
}

bool ParseMacroInvocation(const std::string& line, std::string& outMacro, std::vector<std::string>& outArgs) {
    const size_t openParen = line.find('(');
    const size_t closeParen = line.rfind(')');
    if (openParen == std::string::npos || closeParen == std::string::npos || closeParen <= openParen) {
        return false;
    }

    outMacro = TrimStringCopy(line.substr(0, openParen));
    if (outMacro.empty()) {
        return false;
    }

    const std::string argsRaw = line.substr(openParen + 1, closeParen - openParen - 1);
    outArgs = SplitTopLevelCommas(argsRaw);
    return true;
}

std::string XmlNode(const std::string& tag, const std::vector<std::pair<std::string, std::string>>& attrs = {}) {
    if (attrs.empty()) {
        return "\t<" + tag + "/>";
    }
    std::stringstream ss;
    ss << "\t<" << tag;
    for (const auto& [key, value] : attrs) {
        ss << " " << key << "=\"" << value << "\"";
    }
    ss << "/>";
    return ss.str();
}

std::string NormalizeTextureSizeToken(const std::string& value) {
    std::string normalized = value;
    const std::string suffix = "_LOAD_BLOCK";
    size_t index = std::string::npos;
    while ((index = normalized.find(suffix)) != std::string::npos) {
        normalized.erase(index, suffix.size());
    }
    return normalized;
}

std::string NormalizeCombineToken(const std::string& token, bool alphaSlot) {
    const auto t = TrimStringCopy(token);
    if (t.rfind("G_CCMUX_", 0) == 0 || t.rfind("G_ACMUX_", 0) == 0) {
        return t;
    }

    if (alphaSlot) {
        if (t == "COMBINED_ALPHA" || t == "COMBINED") {
            return "G_ACMUX_COMBINED";
        }
        if (t == "TEXEL0_ALPHA" || t == "TEXEL0") {
            return "G_ACMUX_TEXEL0";
        }
        if (t == "TEXEL1_ALPHA" || t == "TEXEL1") {
            return "G_ACMUX_TEXEL1";
        }
        if (t == "PRIMITIVE_ALPHA" || t == "PRIMITIVE") {
            return "G_ACMUX_PRIMITIVE";
        }
        if (t == "SHADE_ALPHA" || t == "SHADE") {
            return "G_ACMUX_SHADE";
        }
        if (t == "ENV_ALPHA" || t == "ENVIRONMENT") {
            return "G_ACMUX_ENVIRONMENT";
        }
        if (t == "LOD_FRACTION") {
            return "G_ACMUX_LOD_FRACTION";
        }
        if (t == "PRIM_LOD_FRAC") {
            return "G_ACMUX_PRIM_LOD_FRAC";
        }
        if (t == "0" || t == "1") {
            return "G_ACMUX_" + t;
        }
        return "G_ACMUX_" + t;
    }

    if (t == "0" || t == "1") {
        return "G_CCMUX_" + t;
    }
    return "G_CCMUX_" + t;
}

struct Fast64ArrayBlock {
    std::string name;
    std::string body;
};

struct Fast64VertexEntry {
    int16_t x = 0;
    int16_t y = 0;
    int16_t z = 0;
    int16_t s = 0;
    int16_t t = 0;
    int16_t r = 0;
    int16_t g = 0;
    int16_t b = 0;
    int16_t a = 0;
};

struct Fast64TextureMeta {
    std::string format;
    std::string size;
    int32_t widthArg = 0;
    int32_t line = -1;
    int32_t loadBlockLrs = -1;
    bool hasTileSize = false;
    int32_t tileLrs = 0;
    int32_t tileLrt = 0;
    int32_t tileWidthHint = 0;
    int32_t tileHeightHint = 0;
    int32_t maxObservedLrs = -1;
    int32_t maxObservedLrt = -1;
    bool hasDecodedDimensions = false;
    int32_t decodedWidth = 0;
    int32_t decodedHeight = 0;
};

struct Fast64DisplayListCommand {
    std::string tag;
    std::vector<std::pair<std::string, std::string>> attrs;
    std::string textureSymbol;
};

enum class Fast64UnsupportedSeverity {
    Hard,
    Soft,
};

struct Fast64ConversionOutput {
    std::map<std::string, std::vector<uint8_t>> resources;
    std::string rootDisplayListPath;
    size_t vertexArrayCount = 0;
    size_t displayListArrayCount = 0;
    size_t textureCount = 0;
};

struct Fast64ArchiveInspection {
    bool isZipArchive = false;
    bool hasDisplayListResources = false;
    bool hasFast64Source = false;
    std::string rootDisplayListPath;
    std::string modelIncContent;
    std::string headerContent;
    std::string objectNameHint;
};

using Fast64TextureIncludeResolver =
    std::function<bool(const std::string&, const Fast64TextureMeta&, std::vector<uint8_t>&, int32_t&, int32_t&,
                       std::string&)>;

class ExternalModsInMemoryArchive final : public Ship::Archive {
  public:
    ExternalModsInMemoryArchive(std::string archivePath, std::map<std::string, std::vector<uint8_t>> resources)
        : Ship::Archive(std::move(archivePath)), mResources(std::move(resources)) {
    }

    bool Open() override {
        for (const auto& [path, data] : mResources) {
            (void)data;
            IndexFile(path);
        }
        return true;
    }

    bool Close() override {
        return true;
    }

    bool WriteFile([[maybe_unused]] const std::string& filename,
                   [[maybe_unused]] const std::vector<uint8_t>& data) override {
        return false;
    }

    std::shared_ptr<Ship::File> LoadFile(uint64_t hash) override {
        auto* path = Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->HashToString(hash);
        if (path == nullptr) {
            return nullptr;
        }
        return LoadFile(*path);
    }

    std::shared_ptr<Ship::File> LoadFile(const std::string& filePath) override {
        const auto it = mResources.find(filePath);
        if (it == mResources.end()) {
            return nullptr;
        }

        auto file = std::make_shared<Ship::File>();
        file->Buffer = std::make_shared<std::vector<char>>();
        file->Buffer->resize(it->second.size());
        if (!it->second.empty()) {
            std::memcpy(file->Buffer->data(), it->second.data(), it->second.size());
        }
        file->IsLoaded = true;
        return file;
    }

  private:
    std::map<std::string, std::vector<uint8_t>> mResources;
};

bool ParseArrayBlocks(const std::string& text, const std::string& typeName, std::vector<Fast64ArrayBlock>& outBlocks) {
    outBlocks.clear();

    auto isIdentifierStart = [](char ch) {
        return std::isalpha(static_cast<unsigned char>(ch)) || ch == '_';
    };
    auto isIdentifier = [&](char ch) {
        return isIdentifierStart(ch) || std::isdigit(static_cast<unsigned char>(ch));
    };
    auto skipWhitespace = [&](size_t index) {
        while (index < text.size() && std::isspace(static_cast<unsigned char>(text[index]))) {
            ++index;
        }
        return index;
    };

    size_t searchPos = 0;
    while (searchPos < text.size()) {
        const size_t typePos = text.find(typeName, searchPos);
        if (typePos == std::string::npos) {
            break;
        }

        if ((typePos > 0 && isIdentifier(text[typePos - 1])) ||
            (typePos + typeName.size() < text.size() && isIdentifier(text[typePos + typeName.size()]))) {
            searchPos = typePos + 1;
            continue;
        }

        size_t cursor = typePos + typeName.size();
        if (cursor >= text.size() || !std::isspace(static_cast<unsigned char>(text[cursor]))) {
            searchPos = typePos + 1;
            continue;
        }

        cursor = skipWhitespace(cursor);
        if (cursor >= text.size() || !isIdentifierStart(text[cursor])) {
            searchPos = typePos + 1;
            continue;
        }

        const size_t nameStart = cursor;
        ++cursor;
        while (cursor < text.size() && isIdentifier(text[cursor])) {
            ++cursor;
        }
        const std::string name = text.substr(nameStart, cursor - nameStart);

        cursor = skipWhitespace(cursor);
        if (cursor >= text.size() || text[cursor] != '[') {
            searchPos = typePos + 1;
            continue;
        }

        int32_t bracketDepth = 0;
        do {
            if (text[cursor] == '[') {
                bracketDepth++;
            } else if (text[cursor] == ']') {
                bracketDepth--;
            }
            cursor++;
        } while (cursor < text.size() && bracketDepth > 0);

        if (bracketDepth != 0) {
            break;
        }

        cursor = skipWhitespace(cursor);
        if (cursor >= text.size() || text[cursor] != '=') {
            searchPos = typePos + 1;
            continue;
        }
        cursor++;
        cursor = skipWhitespace(cursor);
        if (cursor >= text.size() || text[cursor] != '{') {
            searchPos = typePos + 1;
            continue;
        }

        const size_t bodyStart = cursor + 1;
        int32_t braceDepth = 1;
        cursor++;
        while (cursor < text.size() && braceDepth > 0) {
            if (text[cursor] == '{') {
                braceDepth++;
            } else if (text[cursor] == '}') {
                braceDepth--;
            }
            cursor++;
        }

        if (braceDepth != 0) {
            break;
        }

        const size_t bodyEnd = cursor - 1;
        outBlocks.push_back({ name, text.substr(bodyStart, bodyEnd - bodyStart) });
        searchPos = cursor;
    }

    return !outBlocks.empty();
}

bool ParseVertices(const std::string& body, std::vector<Fast64VertexEntry>& outVertices, std::string& outError) {
    outVertices.clear();
    const std::regex vtxPattern(
        "\\{\\{\\s*\\{([^{}]+)\\}\\s*,\\s*[^,]+,\\s*\\{([^{}]+)\\}\\s*,\\s*\\{([^{}]+)\\}\\s*\\}\\s*\\}",
        std::regex::ECMAScript);

    std::stringstream bodyStream(body);
    std::string line;
    while (std::getline(bodyStream, line)) {
        if (line.find('{') == std::string::npos) {
            continue;
        }

        std::smatch match;
        if (!std::regex_search(line, match, vtxPattern)) {
            continue;
        }

        std::vector<int32_t> xyz;
        std::vector<int32_t> st;
        std::vector<int32_t> rgba;
        for (const auto& listAndOut : { std::pair<std::string, std::vector<int32_t>*>(match[1].str(), &xyz),
                                        std::pair<std::string, std::vector<int32_t>*>(match[2].str(), &st),
                                        std::pair<std::string, std::vector<int32_t>*>(match[3].str(), &rgba) }) {
            std::stringstream ss(listAndOut.first);
            std::string token;
            while (std::getline(ss, token, ',')) {
                int32_t value = 0;
                if (!TryParseIntToken(token, value)) {
                    outError = "invalid numeric token in Vtx array: " + TrimStringCopy(token);
                    return false;
                }
                listAndOut.second->push_back(value);
            }
        }

        if (xyz.size() != 3 || st.size() != 2 || rgba.size() != 4) {
            outError = "invalid Vtx entry layout in model.inc.c";
            return false;
        }

        Fast64VertexEntry vertex;
        vertex.x = static_cast<int16_t>(xyz[0]);
        vertex.y = static_cast<int16_t>(xyz[1]);
        vertex.z = static_cast<int16_t>(xyz[2]);
        vertex.s = static_cast<int16_t>(st[0]);
        vertex.t = static_cast<int16_t>(st[1]);
        vertex.r = static_cast<int16_t>(rgba[0]);
        vertex.g = static_cast<int16_t>(rgba[1]);
        vertex.b = static_cast<int16_t>(rgba[2]);
        vertex.a = static_cast<int16_t>(rgba[3]);
        outVertices.push_back(vertex);
    }

    if (outVertices.empty()) {
        outError = "empty or invalid Vtx array";
        return false;
    }
    return true;
}

bool TryParseSymbolAndOffset(const std::string& expression, std::string& outSymbol, int32_t& outOffset) {
    outSymbol.clear();
    outOffset = 0;
    const std::string trimmed = TrimStringCopy(expression);
    const std::regex pattern("^([A-Za-z_]\\w*)\\s*(?:\\+\\s*([0-9]+))?$");
    std::smatch match;
    if (!std::regex_match(trimmed, match, pattern)) {
        return false;
    }
    outSymbol = match[1].str();
    if (match.size() > 2 && match[2].matched) {
        int32_t offset = 0;
        if (!TryParseIntToken(match[2].str(), offset)) {
            return false;
        }
        outOffset = offset;
    }
    return true;
}

std::pair<std::string, std::string> ParseReferencePath(const std::string& expression, const std::string& objectName) {
    std::string token = TrimStringCopy(expression);
    if (token.size() >= 2 && token.front() == '"' && token.back() == '"') {
        token = token.substr(1, token.size() - 2);
    }

    std::string symbol;
    int32_t offset = 0;
    if (TryParseSymbolAndOffset(token, symbol, offset)) {
        (void)offset;
        return { "objects/" + objectName + "/" + symbol, symbol };
    }

    if (token.rfind("0x", 0) == 0) {
        return { ">" + token, "" };
    }

    if (token.rfind(">0x", 0) == 0) {
        return { token, "" };
    }

    return { token, "" };
}

int32_t FindCommandAttrIndex(const Fast64DisplayListCommand& command, const std::string& key) {
    for (size_t i = 0; i < command.attrs.size(); ++i) {
        if (command.attrs[i].first == key) {
            return static_cast<int32_t>(i);
        }
    }
    return -1;
}

bool TryGetCommandIntAttr(const Fast64DisplayListCommand& command, const std::string& key, int32_t& outValue) {
    const int32_t index = FindCommandAttrIndex(command, key);
    if (index < 0) {
        return false;
    }
    return TryParseIntToken(command.attrs[static_cast<size_t>(index)].second, outValue);
}

bool SetCommandIntAttr(Fast64DisplayListCommand& command, const std::string& key, int32_t value) {
    const int32_t index = FindCommandAttrIndex(command, key);
    if (index < 0) {
        return false;
    }
    command.attrs[static_cast<size_t>(index)].second = std::to_string(value);
    return true;
}

std::string SerializeFast64DisplayListXml(const std::vector<Fast64DisplayListCommand>& commands) {
    std::stringstream xml;
    xml << "<DisplayList Version=\"0\">\n";
    for (const auto& command : commands) {
        xml << XmlNode(command.tag, command.attrs) << "\n";
    }
    xml << "</DisplayList>\n\n";
    return xml.str();
}

bool TryParseXmlNodeLine(const std::string& line, std::string& outTag,
                         std::vector<std::pair<std::string, std::string>>& outAttrs) {
    outTag.clear();
    outAttrs.clear();

    const std::regex nodePattern(R"(^\s*<([A-Za-z0-9_]+)\s*(.*)/>\s*$)");
    std::smatch nodeMatch;
    if (!std::regex_match(line, nodeMatch, nodePattern) || nodeMatch.size() < 2) {
        return false;
    }

    outTag = nodeMatch[1].str();
    std::string attrsRaw;
    if (nodeMatch.size() >= 3) {
        attrsRaw = nodeMatch[2].str();
    }

    const std::regex attrPattern("([A-Za-z0-9_]+)\\s*=\\s*\"([^\"]*)\"");
    for (std::sregex_iterator it(attrsRaw.begin(), attrsRaw.end(), attrPattern), end; it != end; ++it) {
        outAttrs.push_back({ (*it)[1].str(), (*it)[2].str() });
    }

    return true;
}

bool IsHardUnsupportedFast64Macro(const std::string& macroName) {
    static const std::unordered_set<std::string> kHardUnsupported = {
        "gsSPVertex",
        "gsSPModifyVertex",
        "gsSP1Triangle",
        "gsSP2Triangles",
        "gsSP1Quadrangle",
        "gsSPLine3D",
        "gsSPDisplayList",
        "gsSPBranchList",
        "gsSPCullDisplayList",
        "gsSPEndDisplayList",
        "gsDPSetTextureImage",
        "gsDPSetTile",
        "gsDPSetTileSize",
        "gsDPLoadBlock",
        "gsDPLoadTile",
        "gsDPLoadTLUTCmd",
        "gsDPLoadTextureBlock",
        "gsDPLoadTextureBlock_4b",
        "gsDPLoadMultiBlock",
        "gsDPLoadMultiBlock_4b",
    };
    if (kHardUnsupported.find(macroName) != kHardUnsupported.end()) {
        return true;
    }

    if ((macroName.rfind("gsSP", 0) == 0) &&
        (macroName.find("Triangle") != std::string::npos || macroName.find("Vertex") != std::string::npos ||
         macroName.find("DisplayList") != std::string::npos || macroName.find("Branch") != std::string::npos ||
         macroName.find("Cull") != std::string::npos)) {
        return true;
    }
    if ((macroName.rfind("gsDP", 0) == 0) &&
        (macroName.find("TextureImage") != std::string::npos || macroName.find("SetTile") != std::string::npos ||
         macroName.find("LoadTile") != std::string::npos || macroName.find("LoadBlock") != std::string::npos ||
         macroName.find("LoadTLUT") != std::string::npos)) {
        return true;
    }

    return false;
}

std::string BuildUnsupportedFast64Summary(const std::vector<std::string>& entries, const std::string& title) {
    if (entries.empty()) {
        return "";
    }
    std::vector<std::string> sortedEntries = entries;
    std::sort(sortedEntries.begin(), sortedEntries.end());
    sortedEntries.erase(std::unique(sortedEntries.begin(), sortedEntries.end()), sortedEntries.end());
    std::stringstream ss;
    ss << title;
    const size_t limit = std::min<size_t>(sortedEntries.size(), 20);
    for (size_t i = 0; i < limit; ++i) {
        ss << "\n" << sortedEntries[i];
    }
    if (sortedEntries.size() > limit) {
        ss << "\n... +" << (sortedEntries.size() - limit) << " macro(s)";
    }
    return ss.str();
}

bool TryParseLightIndexToken(const std::string& token, int32_t& outIndex) {
    static const std::unordered_map<std::string, int32_t> kLightIndexAliases = {
        { "LIGHT_1", 1 }, { "LIGHT_2", 2 }, { "LIGHT_3", 3 }, { "LIGHT_4", 4 },
        { "LIGHT_5", 5 }, { "LIGHT_6", 6 }, { "LIGHT_7", 7 }, { "LIGHT_8", 8 },
    };
    const std::string normalized = TrimStringCopy(token);
    if (auto it = kLightIndexAliases.find(normalized); it != kLightIndexAliases.end()) {
        outIndex = it->second;
        return true;
    }

    int32_t numeric = 0;
    if (!TryParseIntToken(normalized, numeric)) {
        return false;
    }
    outIndex = std::clamp(numeric, 1, 8);
    return true;
}

void ApplyForcedTextureFilterToDisplayLists(std::map<std::string, std::vector<Fast64DisplayListCommand>>& displayLists,
                                            ExternalModModelTextureFilter configuredFilter, size_t& outReplacedCount,
                                            size_t& outInjectedCount) {
    outReplacedCount = 0;
    outInjectedCount = 0;
    if (configuredFilter == ExternalModModelTextureFilter::Auto) {
        return;
    }

    const int32_t forcedMode = configuredFilter == ExternalModModelTextureFilter::Point ? G_TF_POINT : G_TF_BILERP;
    const std::vector<std::pair<std::string, std::string>> attrs = { { "Mode", std::to_string(forcedMode) } };

    for (auto& [path, commands] : displayLists) {
        (void)path;
        bool hasFilter = false;
        for (auto& command : commands) {
            if (command.tag != "SetTextureFilter") {
                continue;
            }
            command.attrs = attrs;
            hasFilter = true;
            ++outReplacedCount;
        }

        if (!hasFilter) {
            size_t insertIndex = 0;
            while (insertIndex < commands.size() &&
                   (commands[insertIndex].tag == "PipeSync" || commands[insertIndex].tag == "TileSync" ||
                    commands[insertIndex].tag == "LoadSync")) {
                ++insertIndex;
            }

            Fast64DisplayListCommand injected;
            injected.tag = "SetTextureFilter";
            injected.attrs = attrs;
            commands.insert(commands.begin() + static_cast<std::ptrdiff_t>(insertIndex), std::move(injected));
            ++outInjectedCount;
        }
    }
}

int32_t DetectConservativeTileScaleFactor(const Fast64TextureMeta& meta, int32_t textureWidth, int32_t textureHeight) {
    if (textureWidth <= 0 || textureHeight <= 0) {
        return 1;
    }

    const int32_t observedWidth =
        std::max(meta.tileWidthHint, meta.maxObservedLrs >= 0 ? std::max(1, (meta.maxObservedLrs / 4) + 1) : 0);
    const int32_t observedHeight =
        std::max(meta.tileHeightHint, meta.maxObservedLrt >= 0 ? std::max(1, (meta.maxObservedLrt / 4) + 1) : 0);

    if (observedWidth <= textureWidth || observedHeight <= textureHeight) {
        return 1;
    }
    if (observedWidth % textureWidth != 0 || observedHeight % textureHeight != 0) {
        return 1;
    }

    const int32_t factorS = observedWidth / textureWidth;
    const int32_t factorT = observedHeight / textureHeight;
    if (factorS != factorT) {
        return 1;
    }

    if (factorS == 2 || factorS == 4 || factorS == 8 || factorS == 16) {
        return factorS;
    }

    return 1;
}

size_t ApplyConservativeTileScaleAdjustment(
    std::map<std::string, std::vector<Fast64DisplayListCommand>>& displayLists, const std::string& textureSymbol,
    int32_t factor) {
    if (factor <= 1) {
        return 0;
    }

    size_t updatedCommands = 0;
    for (auto& [path, commands] : displayLists) {
        (void)path;
        for (auto& command : commands) {
            if (command.textureSymbol != textureSymbol) {
                continue;
            }

            if (command.tag == "SetTileSize" || command.tag == "LoadTile") {
                int32_t uls = 0;
                int32_t ult = 0;
                int32_t lrs = 0;
                int32_t lrt = 0;
                if (!TryGetCommandIntAttr(command, "Uls", uls) || !TryGetCommandIntAttr(command, "Ult", ult) ||
                    !TryGetCommandIntAttr(command, "Lrs", lrs) || !TryGetCommandIntAttr(command, "Lrt", lrt)) {
                    continue;
                }
                if ((uls % factor) != 0 || (ult % factor) != 0 || (lrs % factor) != 0 || (lrt % factor) != 0) {
                    continue;
                }

                const bool updated = SetCommandIntAttr(command, "Uls", uls / factor) &&
                                     SetCommandIntAttr(command, "Ult", ult / factor) &&
                                     SetCommandIntAttr(command, "Lrs", lrs / factor) &&
                                     SetCommandIntAttr(command, "Lrt", lrt / factor);
                if (updated) {
                    ++updatedCommands;
                }
            } else if (command.tag == "SetTile") {
                int32_t line = 0;
                if (!TryGetCommandIntAttr(command, "Line", line) || line <= 0 || (line % factor) != 0) {
                    continue;
                }
                if (SetCommandIntAttr(command, "Line", line / factor)) {
                    ++updatedCommands;
                }
            }
        }
    }

    return updatedCommands;
}

bool ConvertFast64DisplayLists(const std::vector<Fast64ArrayBlock>& gfxArrays, const std::string& objectName,
                               std::map<std::string, std::vector<Fast64DisplayListCommand>>& outDisplayLists,
                               std::unordered_map<std::string, Fast64TextureMeta>& outTextureUsage,
                               std::unordered_set<std::string>& outCalledDisplayLists,
                               std::vector<std::string>& outHardUnsupported,
                               std::vector<std::string>& outSoftIgnored, std::string& outError) {
    outDisplayLists.clear();
    outTextureUsage.clear();
    outCalledDisplayLists.clear();
    outHardUnsupported.clear();
    outSoftIgnored.clear();

    static const std::vector<std::string> kGeometryFlags = {
        "G_SHADE", "G_LIGHTING", "G_SHADING_SMOOTH", "G_ZBUFFER", "G_TEXTURE_GEN", "G_TEXTURE_GEN_LINEAR",
        "G_CULL_BACK", "G_CULL_FRONT", "G_CULL_BOTH", "G_FOG", "G_CLIPPING",
    };

    for (const auto& gfxArray : gfxArrays) {
        std::vector<std::string> lines;
        lines.emplace_back("<DisplayList Version=\"0\">");
        std::unordered_map<size_t, std::string> commandTextureSymbols;
        std::string currentTextureSymbol;

        std::stringstream bodyStream(gfxArray.body);
        std::string rawLine;
        while (std::getline(bodyStream, rawLine)) {
            const size_t commentIndex = rawLine.find("//");
            std::string line = TrimStringCopy(commentIndex == std::string::npos ? rawLine : rawLine.substr(0, commentIndex));
            if (line.empty() || line.rfind("gs", 0) != 0) {
                continue;
            }

            while (!line.empty() && (line.back() == ',' || line.back() == ';')) {
                line.pop_back();
                line = TrimStringCopy(line);
            }

            std::string macro;
            std::vector<std::string> args;
            if (!ParseMacroInvocation(line, macro, args)) {
                continue;
            }

            auto parseArgInt = [&](size_t index, int32_t& outValue) -> bool {
                if (index >= args.size()) {
                    outError = "invalid macro argument index while converting " + gfxArray.name + ": " + macro;
                    return false;
                }
                if (!TryParseIntToken(args[index], outValue)) {
                    outError = "invalid numeric token in " + gfxArray.name + ": " + args[index];
                    return false;
                }
                return true;
            };

            if (macro == "gsDPPipeSync") {
                lines.push_back(XmlNode("PipeSync"));
            } else if (macro == "gsDPTileSync") {
                lines.push_back(XmlNode("TileSync"));
            } else if (macro == "gsDPLoadSync") {
                lines.push_back(XmlNode("LoadSync"));
            } else if (macro == "gsSPEndDisplayList") {
                lines.push_back(XmlNode("EndDisplayList"));
            } else if (macro == "gsSPDisplayList") {
                const auto [path, symbol] = ParseReferencePath(args.at(0), objectName);
                if (!symbol.empty()) {
                    outCalledDisplayLists.insert(symbol);
                }
                lines.push_back(XmlNode("CallDisplayList", { { "Path", path } }));
            } else if (macro == "gsSPVertex") {
                std::string symbol;
                int32_t offset = 0;
                if (!TryParseSymbolAndOffset(args.at(0), symbol, offset)) {
                    outError = "gsSPVertex expects symbol + optional offset in " + gfxArray.name + ": " + args.at(0);
                    return false;
                }
                int32_t count = 0;
                int32_t vbIndex = 0;
                if (!parseArgInt(1, count) || !parseArgInt(2, vbIndex)) {
                    return false;
                }
                lines.push_back(XmlNode("LoadVertices",
                                        { { "Path", "objects/" + objectName + "/" + symbol },
                                          { "VertexBufferIndex", std::to_string(vbIndex) },
                                          { "VertexOffset", std::to_string(offset) },
                                          { "Count", std::to_string(count) } }));
            } else if (macro == "gsSP2Triangles") {
                int32_t values[8] = {};
                for (size_t i = 0; i < 8; ++i) {
                    if (!parseArgInt(i, values[i])) {
                        return false;
                    }
                }
                lines.push_back(XmlNode("Triangle1",
                                        { { "V00", std::to_string(values[0]) },
                                          { "V01", std::to_string(values[1]) },
                                          { "V02", std::to_string(values[2]) },
                                          { "Flag0", std::to_string(values[3]) } }));
                lines.push_back(XmlNode("Triangle1",
                                        { { "V00", std::to_string(values[4]) },
                                          { "V01", std::to_string(values[5]) },
                                          { "V02", std::to_string(values[6]) },
                                          { "Flag0", std::to_string(values[7]) } }));
            } else if (macro == "gsSP1Triangle") {
                int32_t values[4] = {};
                for (size_t i = 0; i < 4; ++i) {
                    if (!parseArgInt(i, values[i])) {
                        return false;
                    }
                }
                lines.push_back(XmlNode("Triangle1",
                                        { { "V00", std::to_string(values[0]) },
                                          { "V01", std::to_string(values[1]) },
                                          { "V02", std::to_string(values[2]) },
                                          { "Flag0", std::to_string(values[3]) } }));
            } else if (macro == "gsSP1Quadrangle") {
                int32_t values[5] = {};
                for (size_t i = 0; i < 5; ++i) {
                    if (!parseArgInt(i, values[i])) {
                        return false;
                    }
                }
                lines.push_back(XmlNode("Triangle1",
                                        { { "V00", std::to_string(values[0]) },
                                          { "V01", std::to_string(values[1]) },
                                          { "V02", std::to_string(values[2]) },
                                          { "Flag0", std::to_string(values[4]) } }));
                lines.push_back(XmlNode("Triangle1",
                                        { { "V00", std::to_string(values[0]) },
                                          { "V01", std::to_string(values[2]) },
                                          { "V02", std::to_string(values[3]) },
                                          { "Flag0", std::to_string(values[4]) } }));
            } else if (macro == "gsSPCullDisplayList") {
                int32_t start = 0;
                int32_t end = 0;
                if (!parseArgInt(0, start) || !parseArgInt(1, end)) {
                    return false;
                }
                lines.push_back(XmlNode(
                    "CullDisplayList", { { "Start", std::to_string(start) }, { "End", std::to_string(end) } }));
            } else if (macro == "gsSPSetGeometryMode" || macro == "gsSPClearGeometryMode") {
                const auto tokens = ParseOrTokens(args.at(0));
                std::unordered_set<std::string> tokenSet(tokens.begin(), tokens.end());
                std::vector<std::pair<std::string, std::string>> attrs;
                for (const auto& flag : kGeometryFlags) {
                    if (tokenSet.find(flag) != tokenSet.end()) {
                        attrs.push_back({ flag, "1" });
                    }
                }
                lines.push_back(XmlNode(macro == "gsSPSetGeometryMode" ? "SetGeometryMode" : "ClearGeometryMode", attrs));
            } else if (macro == "gsSPGeometryMode") {
                if (args.size() < 2) {
                    outError = "gsSPGeometryMode argument count mismatch in " + gfxArray.name;
                    return false;
                }
                const auto clearTokens = ParseOrTokens(args.at(0));
                const auto setTokens = ParseOrTokens(args.at(1));
                std::unordered_set<std::string> clearTokenSet(clearTokens.begin(), clearTokens.end());
                std::unordered_set<std::string> setTokenSet(setTokens.begin(), setTokens.end());
                std::vector<std::pair<std::string, std::string>> clearAttrs;
                std::vector<std::pair<std::string, std::string>> setAttrs;
                for (const auto& flag : kGeometryFlags) {
                    if (clearTokenSet.find(flag) != clearTokenSet.end()) {
                        clearAttrs.push_back({ flag, "1" });
                    }
                    if (setTokenSet.find(flag) != setTokenSet.end()) {
                        setAttrs.push_back({ flag, "1" });
                    }
                }
                lines.push_back(XmlNode("ClearGeometryMode", clearAttrs));
                lines.push_back(XmlNode("SetGeometryMode", setAttrs));
            } else if (macro == "gsSPLoadGeometryMode") {
                const auto tokens = ParseOrTokens(args.at(0));
                std::unordered_set<std::string> tokenSet(tokens.begin(), tokens.end());
                std::vector<std::pair<std::string, std::string>> clearAttrs;
                std::vector<std::pair<std::string, std::string>> setAttrs;
                for (const auto& flag : kGeometryFlags) {
                    clearAttrs.push_back({ flag, "1" });
                    if (tokenSet.find(flag) != tokenSet.end()) {
                        setAttrs.push_back({ flag, "1" });
                    }
                }
                lines.push_back(XmlNode("ClearGeometryMode", clearAttrs));
                lines.push_back(XmlNode("SetGeometryMode", setAttrs));
            } else if (macro == "gsSPTexture") {
                int32_t s = 0;
                int32_t t = 0;
                int32_t level = 0;
                int32_t tile = 0;
                int32_t on = 0;
                if (!parseArgInt(0, s) || !parseArgInt(1, t) || !parseArgInt(2, level) || !parseArgInt(3, tile) ||
                    !parseArgInt(4, on)) {
                    return false;
                }
                lines.push_back(XmlNode("Texture",
                                        { { "S", std::to_string(s) }, { "T", std::to_string(t) },
                                          { "Level", std::to_string(level) }, { "Tile", std::to_string(tile) },
                                          { "On", std::to_string(on) } }));
            } else if (macro == "gsSPLightColor") {
                int32_t lightIndex = 0;
                if (!TryParseLightIndexToken(args.at(0), lightIndex)) {
                    outError = "invalid light index in " + gfxArray.name + ": " + args.at(0);
                    return false;
                }

                uint32_t packedColor = 0;
                const std::string colorToken = TrimStringCopy(args.at(1));
                try {
                    size_t parsedLength = 0;
                    packedColor = static_cast<uint32_t>(std::stoull(colorToken, &parsedLength, 0));
                    if (parsedLength != colorToken.size()) {
                        outError = "invalid light color in " + gfxArray.name + ": " + colorToken;
                        return false;
                    }
                } catch (...) {
                    int32_t signedColor = 0;
                    if (!TryParseIntToken(colorToken, signedColor)) {
                        outError = "invalid light color in " + gfxArray.name + ": " + colorToken;
                        return false;
                    }
                    packedColor = static_cast<uint32_t>(signedColor);
                }

                lines.push_back(XmlNode("LightColor",
                                        { { "N", std::to_string(lightIndex) },
                                          { "Col", std::to_string(static_cast<int32_t>(packedColor)) } }));
            } else if (macro == "gsDPSetPrimDepth") {
                int32_t z = 0;
                int32_t dz = 0;
                if (!parseArgInt(0, z) || !parseArgInt(1, dz)) {
                    return false;
                }
                lines.push_back(
                    XmlNode("SetPrimDepth", { { "Z", std::to_string(z) }, { "DZ", std::to_string(dz) } }));
            } else if (macro == "gsDPSetFillColor") {
                int32_t color = 0;
                if (!parseArgInt(0, color)) {
                    return false;
                }
                lines.push_back(XmlNode("SetFillColor", { { "C", std::to_string(color) } }));
            } else if (macro == "gsDPSetFogColor") {
                int32_t values[4] = {};
                for (size_t i = 0; i < 4; ++i) {
                    if (!parseArgInt(i, values[i])) {
                        return false;
                    }
                }
                lines.push_back(XmlNode("SetFogColor",
                                        { { "R", std::to_string(values[0]) }, { "G", std::to_string(values[1]) },
                                          { "B", std::to_string(values[2]) }, { "A", std::to_string(values[3]) } }));
            } else if (macro == "gsDPSetBlendColor") {
                int32_t values[4] = {};
                for (size_t i = 0; i < 4; ++i) {
                    if (!parseArgInt(i, values[i])) {
                        return false;
                    }
                }
                lines.push_back(XmlNode("SetBlendColor",
                                        { { "R", std::to_string(values[0]) }, { "G", std::to_string(values[1]) },
                                          { "B", std::to_string(values[2]) }, { "A", std::to_string(values[3]) } }));
            } else if (macro == "gsDPSetEnvColor") {
                int32_t values[4] = {};
                for (size_t i = 0; i < 4; ++i) {
                    if (!parseArgInt(i, values[i])) {
                        return false;
                    }
                }
                lines.push_back(XmlNode("SetEnvColor",
                                        { { "R", std::to_string(values[0]) }, { "G", std::to_string(values[1]) },
                                          { "B", std::to_string(values[2]) }, { "A", std::to_string(values[3]) } }));
            } else if (macro == "gsDPSetPrimColor") {
                int32_t values[6] = {};
                for (size_t i = 0; i < 6; ++i) {
                    if (!parseArgInt(i, values[i])) {
                        return false;
                    }
                }
                lines.push_back(XmlNode("SetPrimColor",
                                        { { "M", std::to_string(values[0]) }, { "L", std::to_string(values[1]) },
                                          { "R", std::to_string(values[2]) }, { "G", std::to_string(values[3]) },
                                          { "B", std::to_string(values[4]) }, { "A", std::to_string(values[5]) } }));
            } else if (macro == "gsDPSetRenderMode") {
                if (args.size() != 2) {
                    outError = "gsDPSetRenderMode argument count mismatch in " + gfxArray.name;
                    return false;
                }
                const std::string mode1 = TrimStringCopy(args[0]);
                const std::string mode2 = TrimStringCopy(args[1]);
                if (mode1.empty() || mode2.empty()) {
                    outError = "gsDPSetRenderMode has empty mode token in " + gfxArray.name;
                    return false;
                }
                lines.push_back(XmlNode("SetRenderMode", { { "Mode1", mode1 }, { "Mode2", mode2 } }));
            } else if (macro == "gsDPSetCycleType") {
                int32_t mode = 0;
                if (!parseArgInt(0, mode)) {
                    return false;
                }
                std::vector<std::pair<std::string, std::string>> attrs;
                if (mode == G_CYC_1CYCLE) {
                    attrs.push_back({ "G_CYC_1CYCLE", "1" });
                } else if (mode == G_CYC_2CYCLE) {
                    attrs.push_back({ "G_CYC_2CYCLE", "1" });
                } else if (mode == G_CYC_COPY) {
                    attrs.push_back({ "G_CYC_COPY", "1" });
                } else if (mode == G_CYC_FILL) {
                    attrs.push_back({ "G_CYC_FILL", "1" });
                } else {
                    outError = "unsupported cycle mode value in " + gfxArray.name + ": " + std::to_string(mode);
                    return false;
                }
                lines.push_back(XmlNode("SetCycleType", attrs));
            } else if (macro == "gsDPPipelineMode") {
                int32_t mode = 0;
                if (!parseArgInt(0, mode)) {
                    return false;
                }
                std::vector<std::pair<std::string, std::string>> attrs;
                if (mode == G_PM_1PRIMITIVE) {
                    attrs.push_back({ "G_PM_1PRIMITIVE", "1" });
                } else if (mode == G_PM_NPRIMITIVE) {
                    attrs.push_back({ "G_PM_NPRIMITIVE", "1" });
                } else {
                    outError = "unsupported pipeline mode value in " + gfxArray.name + ": " + std::to_string(mode);
                    return false;
                }
                lines.push_back(XmlNode("PipelineMode", attrs));
            } else if (macro == "gsDPSetTextureLUT") {
                lines.push_back(XmlNode("SetTextureLUT", { { "Mode", TrimStringCopy(args.at(0)) } }));
            } else if (macro == "gsDPLoadTLUTCmd") {
                int32_t tile = 0;
                int32_t count = 0;
                if (!parseArgInt(0, tile) || !parseArgInt(1, count)) {
                    return false;
                }
                lines.push_back(XmlNode("LoadTLUTCmd",
                                        { { "Tile", std::to_string(tile) }, { "Count", std::to_string(count) } }));
            } else if (macro == "gsDPSetTextureFilter") {
                static const std::unordered_map<std::string, int32_t> kTextureFilterModes = {
                    { "G_TF_POINT", G_TF_POINT },
                    { "G_TF_AVERAGE", G_TF_AVERAGE },
                    { "G_TF_BILERP", G_TF_BILERP },
                };
                const std::string modeToken = TrimStringCopy(args.at(0));
                int32_t mode = 0;
                if (auto it = kTextureFilterModes.find(modeToken); it != kTextureFilterModes.end()) {
                    mode = it->second;
                } else if (!TryParseIntToken(modeToken, mode)) {
                    outError = "invalid texture filter mode in " + gfxArray.name + ": " + modeToken;
                    return false;
                }
                lines.push_back(XmlNode("SetTextureFilter", { { "Mode", std::to_string(mode) } }));
            } else if (macro == "gsDPSetTextureLOD") {
                int32_t mode = 0;
                if (!parseArgInt(0, mode)) {
                    return false;
                }
                lines.push_back(XmlNode("SetTextureLOD", { { "Mode", std::to_string(mode) } }));
            } else if (macro == "gsDPSetTextureDetail") {
                int32_t type = 0;
                if (!parseArgInt(0, type)) {
                    return false;
                }
                lines.push_back(XmlNode("SetTextureDetail", { { "Type", std::to_string(type) } }));
            } else if (macro == "gsDPSetTexturePersp") {
                int32_t enable = 0;
                if (!parseArgInt(0, enable)) {
                    return false;
                }
                lines.push_back(XmlNode("SetTexturePersp", { { "Enable", std::to_string(enable) } }));
            } else if (macro == "gsDPSetColorDither") {
                int32_t type = 0;
                if (!parseArgInt(0, type)) {
                    return false;
                }
                lines.push_back(XmlNode("SetColorDither", { { "Type", std::to_string(type) } }));
            } else if (macro == "gsDPSetCombineKey") {
                int32_t type = 0;
                if (!parseArgInt(0, type)) {
                    return false;
                }
                lines.push_back(XmlNode("SetCombineKey", { { "Type", std::to_string(type) } }));
            } else if (macro == "gsDPSetDepthSource") {
                int32_t mode = 0;
                if (!parseArgInt(0, mode)) {
                    return false;
                }
                lines.push_back(XmlNode("SetDepthSource", { { "Mode", std::to_string(mode) } }));
            } else if (macro == "gsDPSetAlphaCompare") {
                int32_t mode = 0;
                if (!parseArgInt(0, mode)) {
                    return false;
                }
                lines.push_back(XmlNode("SetAlphaCompare", { { "Mode", std::to_string(mode) } }));
            } else if (macro == "gsDPSetAlphaDither") {
                int32_t mode = 0;
                if (!TryParseAlphaDitherModeToken(args.at(0), mode)) {
                    outError = "invalid alpha dither mode in " + gfxArray.name + ": " + args.at(0);
                    return false;
                }
                lines.push_back(XmlNode("SetAlphaDither", { { "Type", std::to_string(mode) } }));
            } else if (macro == "gsSPPerspNormalize") {
                int32_t value = 0;
                if (!parseArgInt(0, value)) {
                    return false;
                }
                lines.push_back(XmlNode("PerspNormalize", { { "S", std::to_string(value) } }));
            } else if (macro == "gsSPFogPosition") {
                int32_t min = 0;
                int32_t max = 0;
                if (!parseArgInt(0, min) || !parseArgInt(1, max)) {
                    return false;
                }
                lines.push_back(
                    XmlNode("FogPosition", { { "Min", std::to_string(min) }, { "Max", std::to_string(max) } }));
            } else if (macro == "gsSPFogFactor") {
                int32_t fm = 0;
                int32_t fo = 0;
                if (!parseArgInt(0, fm) || !parseArgInt(1, fo)) {
                    return false;
                }
                lines.push_back(XmlNode("FogFactor", { { "FM", std::to_string(fm) }, { "FO", std::to_string(fo) } }));
            } else if (macro == "gsSPNumLights") {
                int32_t lites = 0;
                if (!parseArgInt(0, lites)) {
                    return false;
                }
                lines.push_back(XmlNode("NumLites", { { "Lites", std::to_string(lites) } }));
            } else if (macro == "gsDPSetCombineLERP") {
                if (args.size() != 16) {
                    outError = "gsDPSetCombineLERP argument count mismatch in " + gfxArray.name;
                    return false;
                }
                static const std::array<const char*, 16> kAttrNames = {
                    "A0",  "B0",  "C0",  "D0",  "Aa0", "Ab0", "Ac0", "Ad0",
                    "A1",  "B1",  "C1",  "D1",  "Aa1", "Ab1", "Ac1", "Ad1",
                };
                const std::unordered_set<size_t> alphaSlots = { 4, 5, 6, 7, 12, 13, 14, 15 };
                std::vector<std::pair<std::string, std::string>> attrs;
                for (size_t i = 0; i < args.size(); ++i) {
                    attrs.push_back({ kAttrNames[i], NormalizeCombineToken(args[i], alphaSlots.find(i) != alphaSlots.end()) });
                }
                lines.push_back(XmlNode("SetCombineLERP", attrs));
            } else if (macro == "gsSPSetOtherMode") {
                if (args.size() < 4) {
                    outError = "gsSPSetOtherMode argument count mismatch in " + gfxArray.name;
                    return false;
                }
                int32_t cmd = 0;
                int32_t sft = 0;
                int32_t length = 0;
                if (!parseArgInt(0, cmd) || !parseArgInt(1, sft) || !parseArgInt(2, length)) {
                    return false;
                }
                auto tokens = ParseOrTokens(args[3]);
                tokens.erase(std::remove(tokens.begin(), tokens.end(), "0"), tokens.end());
                std::string alphaToken;
                std::string textureLutToken;
                std::vector<std::string> filteredTokens;
                for (const auto& token : tokens) {
                    if (token == "G_AC_NONE" || token == "G_AC_THRESHOLD" || token == "G_AC_DITHER") {
                        alphaToken = token;
                        continue;
                    }
                    if (token == "G_TT_NONE" || token == "G_TT_RGBA16" || token == "G_TT_IA16") {
                        textureLutToken = token;
                        continue;
                    }
                    filteredTokens.push_back(token);
                }

                std::vector<std::pair<std::string, std::string>> attrs = {
                    { "Cmd", std::to_string(cmd) },
                    { "Sft", std::to_string(sft) },
                    { "Length", std::to_string(length) },
                };
                for (const auto& token : filteredTokens) {
                    attrs.push_back({ token, "1" });
                }
                lines.push_back(XmlNode("SetOtherMode", attrs));

                if (!textureLutToken.empty()) {
                    lines.push_back(XmlNode("SetTextureLUT", { { "Mode", textureLutToken } }));
                }
                if (!alphaToken.empty()) {
                    const int32_t alphaMode =
                        alphaToken == "G_AC_THRESHOLD" ? 1 : (alphaToken == "G_AC_DITHER" ? 3 : 0);
                    lines.push_back(XmlNode("SetAlphaCompare", { { "Mode", std::to_string(alphaMode) } }));
                }
            } else if (macro == "gsDPSetTextureImage") {
                int32_t width = 0;
                if (!parseArgInt(2, width)) {
                    return false;
                }
                const std::string fmt = TrimStringCopy(args.at(0));
                const std::string size = TrimStringCopy(args.at(1));
                const auto [path, symbol] = ParseReferencePath(args.at(3), objectName);
                lines.push_back(XmlNode("SetTextureImage",
                                        { { "Path", path }, { "Format", fmt }, { "Size", size },
                                          { "Width", std::to_string(width) } }));
                currentTextureSymbol = symbol;
                if (!symbol.empty()) {
                    auto& meta = outTextureUsage[symbol];
                    meta.format = fmt;
                    meta.size = size;
                    meta.widthArg = width;
                }
            } else if (macro == "gsDPSetTile") {
                int32_t values[12] = {};
                for (size_t i = 0; i < 12; ++i) {
                    // fmt/siz are symbolic tokens; cms/cmt are symbolic OR'd flags.
                    if (i == 0 || i == 1 || i == 6 || i == 9) {
                        continue;
                    }
                    if (!parseArgInt(i, values[i])) {
                        return false;
                    }
                }
                const auto cmtTokens = ParseOrTokens(args.at(6));
                const auto cmsTokens = ParseOrTokens(args.at(9));
                std::vector<std::pair<std::string, std::string>> attrs = {
                    { "Format", TrimStringCopy(args.at(0)) },
                    { "Size", TrimStringCopy(args.at(1)) },
                    { "Line", std::to_string(values[2]) },
                    { "TMem", std::to_string(values[3]) },
                    { "Tile", std::to_string(values[4]) },
                    { "Palette", std::to_string(values[5]) },
                    { "Cms0", cmsTokens.size() >= 1 ? cmsTokens[0] : "0" },
                    { "Cms1", cmsTokens.size() >= 2 ? cmsTokens[1] : "0" },
                    { "Cmt0", cmtTokens.size() >= 1 ? cmtTokens[0] : "0" },
                    { "Cmt1", cmtTokens.size() >= 2 ? cmtTokens[1] : "0" },
                    { "MaskS", std::to_string(values[10]) },
                    { "ShiftS", std::to_string(values[11]) },
                    { "MaskT", std::to_string(values[7]) },
                    { "ShiftT", std::to_string(values[8]) },
                };
                const size_t commandIndex = lines.size();
                lines.push_back(XmlNode("SetTile", attrs));
                if (!currentTextureSymbol.empty()) {
                    commandTextureSymbols[commandIndex] = currentTextureSymbol;
                }

                if (!currentTextureSymbol.empty() && values[4] == 0 &&
                    TrimStringCopy(args.at(1)).find("_LOAD_BLOCK") == std::string::npos) {
                    outTextureUsage[currentTextureSymbol].line = values[2];
                }
            } else if (macro == "gsDPLoadBlock") {
                int32_t values[5] = {};
                for (size_t i = 0; i < 5; ++i) {
                    if (!parseArgInt(i, values[i])) {
                        return false;
                    }
                }
                const size_t commandIndex = lines.size();
                lines.push_back(XmlNode("LoadBlock",
                                        { { "Tile", std::to_string(values[0]) }, { "Uls", std::to_string(values[1]) },
                                          { "Ult", std::to_string(values[2]) }, { "Lrs", std::to_string(values[3]) },
                                          { "Dxt", std::to_string(values[4]) } }));
                if (!currentTextureSymbol.empty()) {
                    commandTextureSymbols[commandIndex] = currentTextureSymbol;
                }
                if (!currentTextureSymbol.empty()) {
                    outTextureUsage[currentTextureSymbol].loadBlockLrs = values[3];
                }
            } else if (macro == "gsDPLoadTile") {
                int32_t values[5] = {};
                for (size_t i = 0; i < 5; ++i) {
                    if (!parseArgInt(i, values[i])) {
                        return false;
                    }
                }
                const size_t commandIndex = lines.size();
                lines.push_back(XmlNode("LoadTile",
                                        { { "T", std::to_string(values[0]) }, { "Uls", std::to_string(values[1]) },
                                          { "Ult", std::to_string(values[2]) }, { "Lrs", std::to_string(values[3]) },
                                          { "Lrt", std::to_string(values[4]) } }));
                if (!currentTextureSymbol.empty()) {
                    commandTextureSymbols[commandIndex] = currentTextureSymbol;
                }
                if (!currentTextureSymbol.empty()) {
                    auto& meta = outTextureUsage[currentTextureSymbol];
                    const int32_t tileWidth = std::max(1, ((values[3] - values[1]) / 4) + 1);
                    const int32_t tileHeight = std::max(1, ((values[4] - values[2]) / 4) + 1);
                    meta.tileWidthHint = std::max(meta.tileWidthHint, tileWidth);
                    meta.tileHeightHint = std::max(meta.tileHeightHint, tileHeight);
                    meta.maxObservedLrs = std::max(meta.maxObservedLrs, values[3]);
                    meta.maxObservedLrt = std::max(meta.maxObservedLrt, values[4]);
                }
            } else if (macro == "gsDPSetTileSize") {
                int32_t values[5] = {};
                for (size_t i = 0; i < 5; ++i) {
                    if (!parseArgInt(i, values[i])) {
                        return false;
                    }
                }
                const size_t commandIndex = lines.size();
                lines.push_back(XmlNode("SetTileSize",
                                        { { "T", std::to_string(values[0]) }, { "Uls", std::to_string(values[1]) },
                                          { "Ult", std::to_string(values[2]) }, { "Lrs", std::to_string(values[3]) },
                                          { "Lrt", std::to_string(values[4]) } }));
                if (!currentTextureSymbol.empty()) {
                    commandTextureSymbols[commandIndex] = currentTextureSymbol;
                }
                if (!currentTextureSymbol.empty()) {
                    auto& meta = outTextureUsage[currentTextureSymbol];
                    meta.hasTileSize = true;
                    meta.tileLrs = values[3];
                    meta.tileLrt = values[4];
                    const int32_t tileWidth = std::max(1, ((values[3] - values[1]) / 4) + 1);
                    const int32_t tileHeight = std::max(1, ((values[4] - values[2]) / 4) + 1);
                    meta.tileWidthHint = std::max(meta.tileWidthHint, tileWidth);
                    meta.tileHeightHint = std::max(meta.tileHeightHint, tileHeight);
                    meta.maxObservedLrs = std::max(meta.maxObservedLrs, values[3]);
                    meta.maxObservedLrt = std::max(meta.maxObservedLrt, values[4]);
                }
            } else if (macro.rfind("gsSPSetLights", 0) == 0) {
                const auto suffix = macro.substr(std::string("gsSPSetLights").size());
                int32_t lites = 0;
                if (!suffix.empty()) {
                    if (!TryParseIntToken(suffix, lites)) {
                        outError = "unsupported lights macro suffix in " + gfxArray.name + ": " + macro;
                        return false;
                    }
                }
                lites = std::clamp(lites, 0, 7);
                lines.push_back(XmlNode("NumLites", { { "Lites", std::to_string(lites) } }));
            } else {
                const std::string unsupportedEntry = gfxArray.name + ": " + macro;
                if (IsHardUnsupportedFast64Macro(macro)) {
                    outHardUnsupported.push_back(unsupportedEntry);
                } else {
                    outSoftIgnored.push_back(unsupportedEntry);
                }
            }
        }

        lines.emplace_back("</DisplayList>");
        lines.emplace_back("");
        std::vector<Fast64DisplayListCommand> parsedCommands;
        parsedCommands.reserve(lines.size());
        for (size_t lineIndex = 0; lineIndex < lines.size(); ++lineIndex) {
            std::string tag;
            std::vector<std::pair<std::string, std::string>> attrs;
            if (!TryParseXmlNodeLine(lines[lineIndex], tag, attrs)) {
                continue;
            }

            if (tag == "DisplayList") {
                continue;
            }

            Fast64DisplayListCommand command;
            command.tag = std::move(tag);
            command.attrs = std::move(attrs);
            if (const auto symbolIt = commandTextureSymbols.find(lineIndex); symbolIt != commandTextureSymbols.end()) {
                command.textureSymbol = symbolIt->second;
            }
            parsedCommands.push_back(std::move(command));
        }
        outDisplayLists["objects/" + objectName + "/" + gfxArray.name] = std::move(parsedCommands);
    }

    return true;
}

void AppendU32LE(std::vector<uint8_t>& outBytes, uint32_t value) {
    outBytes.push_back(static_cast<uint8_t>(value & 0xFF));
    outBytes.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    outBytes.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    outBytes.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

void AppendU64LE(std::vector<uint8_t>& outBytes, uint64_t value) {
    for (size_t i = 0; i < 8; ++i) {
        outBytes.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
    }
}

std::vector<uint8_t> BuildOtexHeader(uint32_t resourceType, uint32_t version = 0, uint8_t isCustom = 0) {
    std::vector<uint8_t> header;
    header.reserve(0x40);
    header.push_back(0);
    header.push_back(isCustom);
    header.push_back(0);
    header.push_back(0);
    AppendU32LE(header, resourceType);
    AppendU32LE(header, version);
    AppendU64LE(header, 0xDEADBEEFDEADBEEFULL);
    AppendU32LE(header, 0);
    AppendU64LE(header, 0);
    AppendU32LE(header, 0);
    if (header.size() < 0x40) {
        header.resize(0x40, 0);
    }
    return header;
}

bool ParseU64ArrayBytes(const std::string& body, std::vector<uint8_t>& outBytes, std::string& outError) {
    outBytes.clear();
    const std::regex hexWord("0x[0-9A-Fa-f]+");
    for (std::sregex_iterator it(body.begin(), body.end(), hexWord), end; it != end; ++it) {
        try {
            uint64_t value = std::stoull(it->str(), nullptr, 16);
            for (int shift = 56; shift >= 0; shift -= 8) {
                outBytes.push_back(static_cast<uint8_t>((value >> shift) & 0xFF));
            }
        } catch (...) {
            outError = "failed parsing u64 texture word: " + it->str();
            return false;
        }
    }
    return true;
}

bool TryExtractIncludePathFromArrayBody(const std::string& body, std::string& outIncludePath) {
    outIncludePath.clear();
    const std::regex includePattern(R"re(#include\s*"([^"]+)")re", std::regex::ECMAScript);
    std::smatch match;
    if (!std::regex_search(body, match, includePattern) || match.size() < 2) {
        return false;
    }
    outIncludePath = TrimStringCopy(match[1].str());
    return !outIncludePath.empty();
}

std::pair<int32_t, int32_t> InferTextureDimensions(const Fast64TextureMeta& meta, size_t dataSize, int32_t bitsPerPixel) {
    auto isConsistentWithData = [&](int32_t width, int32_t height) -> bool {
        if (width <= 0 || height <= 0 || bitsPerPixel <= 0) {
            return false;
        }
        const int64_t expectedBits = static_cast<int64_t>(width) * static_cast<int64_t>(height) * bitsPerPixel;
        const int64_t actualBits = static_cast<int64_t>(dataSize) * 8;
        return expectedBits == actualBits;
    };

    auto isPowerOfTwo = [](int32_t value) -> bool {
        return value > 0 && (value & (value - 1)) == 0;
    };

    if (meta.hasDecodedDimensions && isConsistentWithData(meta.decodedWidth, meta.decodedHeight)) {
        return { meta.decodedWidth, meta.decodedHeight };
    }

    if (meta.hasTileSize) {
        const int32_t width = (meta.tileLrs / 4) + 1;
        const int32_t height = (meta.tileLrt / 4) + 1;
        if (isConsistentWithData(width, height)) {
            return { width, height };
        }
    }

    const int32_t tileWidthHint =
        std::max(meta.tileWidthHint, meta.hasTileSize ? std::max(1, (meta.tileLrs / 4) + 1) : 0);
    const int32_t tileHeightHint =
        std::max(meta.tileHeightHint, meta.hasTileSize ? std::max(1, (meta.tileLrt / 4) + 1) : 0);
    const int32_t minWidthFromObservedCoords = meta.maxObservedLrs >= 0 ? std::max(1, (meta.maxObservedLrs / 4) + 1) : 0;
    const int32_t minHeightFromObservedCoords =
        meta.maxObservedLrt >= 0 ? std::max(1, (meta.maxObservedLrt / 4) + 1) : 0;

    struct TextureDimCandidate {
        int32_t width = 0;
        int32_t height = 0;
        int32_t score = std::numeric_limits<int32_t>::min();
    };
    std::vector<TextureDimCandidate> candidates;

    auto addCandidate = [&](int32_t width, int32_t height, int32_t baseScore) {
        if (!isConsistentWithData(width, height)) {
            return;
        }
        if (minWidthFromObservedCoords > 0 && width < minWidthFromObservedCoords) {
            return;
        }
        if (minHeightFromObservedCoords > 0 && height < minHeightFromObservedCoords) {
            return;
        }

        int32_t score = baseScore;

        if (meta.widthArg > 1) {
            if (width == meta.widthArg) {
                score += 60;
            } else if (width > meta.widthArg && width % meta.widthArg == 0) {
                score += 24;
            } else if (meta.widthArg % std::max(width, 1) == 0) {
                score += 8;
            }
        }

        if (tileWidthHint > 0) {
            if (width == tileWidthHint) {
                score += 64;
            } else if (width > tileWidthHint && width % tileWidthHint == 0) {
                score += 40;
            } else if (width >= tileWidthHint) {
                score += 12;
            } else {
                score -= 64;
            }
        }

        if (tileHeightHint > 0) {
            if (height == tileHeightHint) {
                score += 64;
            } else if (height > tileHeightHint && height % tileHeightHint == 0) {
                score += 40;
            } else if (height >= tileHeightHint) {
                score += 12;
            } else {
                score -= 64;
            }
        }

        if (isPowerOfTwo(width)) {
            score += 12;
        }
        if (isPowerOfTwo(height)) {
            score += 12;
        }

        const int32_t major = std::max(width, height);
        const int32_t minor = std::max(1, std::min(width, height));
        score -= (major - minor) * 6 / minor;
        if (width >= height) {
            score += 1;
        }

        candidates.push_back({ width, height, score });
    };

    const bool hasLoadBlockSize = meta.size.find("_LOAD_BLOCK") != std::string::npos;
    const bool widthArgLooksPlaceholder = hasLoadBlockSize && meta.widthArg <= 1;
    if (meta.widthArg > 0 && bitsPerPixel > 0 && !widthArgLooksPlaceholder) {
        const int64_t totalBits = static_cast<int64_t>(dataSize) * 8;
        const int64_t rowBits = static_cast<int64_t>(meta.widthArg) * bitsPerPixel;
        if (rowBits > 0 && totalBits >= rowBits && totalBits % rowBits == 0) {
            const int32_t height = static_cast<int32_t>(totalBits / rowBits);
            addCandidate(meta.widthArg, height, 180);
        }
    }

    if (meta.line > 0 && meta.loadBlockLrs >= 0 && bitsPerPixel > 0) {
        const int32_t width = (meta.line * 64) / bitsPerPixel;
        const int32_t totalTexels = meta.loadBlockLrs + 1;
        if (width > 0 && totalTexels > 0) {
            const int32_t height = totalTexels / width;
            addCandidate(width, height, 160);
        }
    }

    if (bitsPerPixel <= 0) {
        return { static_cast<int32_t>(std::max<size_t>(1, dataSize)), 1 };
    }

    const int64_t totalPixels = (static_cast<int64_t>(dataSize) * 8) / bitsPerPixel;
    if (totalPixels <= 0) {
        return { 1, 1 };
    }

    for (int64_t width = 1; width * width <= totalPixels; ++width) {
        if (totalPixels % width != 0) {
            continue;
        }
        const int64_t height = totalPixels / width;
        if (width <= std::numeric_limits<int32_t>::max() && height <= std::numeric_limits<int32_t>::max()) {
            addCandidate(static_cast<int32_t>(width), static_cast<int32_t>(height), 0);
            if (width != height) {
                addCandidate(static_cast<int32_t>(height), static_cast<int32_t>(width), 0);
            }
        }
    }

    if (!candidates.empty()) {
        const auto best = std::max_element(candidates.begin(), candidates.end(),
                                           [](const TextureDimCandidate& a, const TextureDimCandidate& b) {
                                               if (a.score != b.score) {
                                                   return a.score < b.score;
                                               }
                                               const int32_t aDiff = std::abs(a.width - a.height);
                                               const int32_t bDiff = std::abs(b.width - b.height);
                                               if (aDiff != bDiff) {
                                                   return aDiff > bDiff;
                                               }
                                               return a.width < b.width;
                                           });
        return { best->width, best->height };
    }

    const int32_t side = static_cast<int32_t>(std::sqrt(static_cast<double>(totalPixels)));
    if (static_cast<int64_t>(side) * side == totalPixels) {
        return { side, side };
    }

    return { static_cast<int32_t>(totalPixels), 1 };
}

std::string DetermineRootDisplayListName(const std::vector<std::string>& gfxNames,
                                         const std::unordered_set<std::string>& calledDisplayLists) {
    std::vector<std::string> roots;
    roots.reserve(gfxNames.size());
    for (const auto& name : gfxNames) {
        if (calledDisplayLists.find(name) == calledDisplayLists.end()) {
            roots.push_back(name);
        }
    }

    std::vector<std::string> preferred;
    preferred.reserve(roots.size());
    for (const auto& root : roots) {
        if (root.rfind("mat_", 0) == 0 || root.find("_tri_") != std::string::npos) {
            continue;
        }
        preferred.push_back(root);
    }

    if (!preferred.empty()) {
        return preferred.back();
    }
    if (!roots.empty()) {
        return roots.back();
    }
    if (!gfxNames.empty()) {
        return gfxNames.back();
    }
    return "";
}

bool ConvertFast64SourceToResources(const std::string& modelContent, const std::string& objectName,
                                    const Fast64TextureIncludeResolver& includeResolver,
                                    ExternalModModelTextureFilter configuredTextureFilter,
                                    const std::string& debugLabel, Fast64ConversionOutput& outConversion,
                                    std::string& outError) {
    outConversion = Fast64ConversionOutput{};

    std::vector<Fast64ArrayBlock> u64Arrays;
    ParseArrayBlocks(modelContent, "u64", u64Arrays);

    std::vector<Fast64ArrayBlock> vtxArrays;
    if (!ParseArrayBlocks(modelContent, "Vtx", vtxArrays) || vtxArrays.empty()) {
        outError = "no Vtx arrays found in model.inc.c";
        return false;
    }

    std::vector<Fast64ArrayBlock> gfxArrays;
    if (!ParseArrayBlocks(modelContent, "Gfx", gfxArrays) || gfxArrays.empty()) {
        outError = "no Gfx arrays found in model.inc.c";
        return false;
    }

    outConversion.vertexArrayCount = vtxArrays.size();
    outConversion.displayListArrayCount = gfxArrays.size();

    for (const auto& vtxArray : vtxArrays) {
        std::vector<Fast64VertexEntry> vertices;
        if (!ParseVertices(vtxArray.body, vertices, outError)) {
            outError = "invalid vertex array " + vtxArray.name + ": " + outError;
            return false;
        }

        std::stringstream xml;
        xml << "<Vertex Version=\"0\">\n";
        for (const auto& vertex : vertices) {
            xml << XmlNode("Vtx",
                           { { "X", std::to_string(vertex.x) }, { "Y", std::to_string(vertex.y) },
                             { "Z", std::to_string(vertex.z) }, { "S", std::to_string(vertex.s) },
                             { "T", std::to_string(vertex.t) }, { "R", std::to_string(vertex.r) },
                             { "G", std::to_string(vertex.g) }, { "B", std::to_string(vertex.b) },
                             { "A", std::to_string(vertex.a) } })
                << "\n";
        }
        xml << "</Vertex>\n\n";
        const std::string path = "objects/" + objectName + "/" + vtxArray.name;
        const auto xmlText = xml.str();
        outConversion.resources[path] = std::vector<uint8_t>(xmlText.begin(), xmlText.end());
    }

    std::map<std::string, std::vector<Fast64DisplayListCommand>> displayListCommands;
    std::unordered_map<std::string, Fast64TextureMeta> textureUsage;
    std::unordered_set<std::string> calledDisplayLists;
    std::vector<std::string> hardUnsupportedMacros;
    std::vector<std::string> softIgnoredMacros;
    if (!ConvertFast64DisplayLists(gfxArrays, objectName, displayListCommands, textureUsage, calledDisplayLists,
                                   hardUnsupportedMacros, softIgnoredMacros, outError)) {
        return false;
    }

    if (!hardUnsupportedMacros.empty()) {
        outError = BuildUnsupportedFast64Summary(hardUnsupportedMacros, "unsupported macros while converting Fast64 model:");
        return false;
    }

    if (!softIgnoredMacros.empty()) {
        const std::string summary = BuildUnsupportedFast64Summary(
            softIgnoredMacros, "ignored non-critical macros while converting Fast64 model:");
        SPDLOG_WARN("[ExternalMods] {} [{}]{}", debugLabel.empty() ? objectName : debugLabel, objectName, "\n" + summary);
    }

    std::unordered_map<std::string, std::vector<uint8_t>> u64Map;
    std::unordered_map<std::string, std::string> includePathBySymbol;
    for (const auto& array : u64Arrays) {
        std::vector<uint8_t> bytes;
        if (!ParseU64ArrayBytes(array.body, bytes, outError)) {
            outError = "invalid u64 texture array " + array.name + ": " + outError;
            return false;
        }
        if (bytes.empty()) {
            std::string includePath;
            if (TryExtractIncludePathFromArrayBody(array.body, includePath)) {
                includePathBySymbol[array.name] = std::move(includePath);
            }
        }
        u64Map[array.name] = std::move(bytes);
    }

    static const std::unordered_map<std::string, int32_t> kTextureTypeByFormatSize = {
        { "G_IM_FMT_RGBA|G_IM_SIZ_32b", 1 }, { "G_IM_FMT_RGBA|G_IM_SIZ_16b", 2 }, { "G_IM_FMT_CI|G_IM_SIZ_4b", 3 },
        { "G_IM_FMT_CI|G_IM_SIZ_8b", 4 },    { "G_IM_FMT_I|G_IM_SIZ_4b", 5 },      { "G_IM_FMT_I|G_IM_SIZ_8b", 6 },
        { "G_IM_FMT_IA|G_IM_SIZ_4b", 7 },    { "G_IM_FMT_IA|G_IM_SIZ_8b", 8 },      { "G_IM_FMT_IA|G_IM_SIZ_16b", 9 },
    };
    static const std::unordered_map<int32_t, int32_t> kBitsPerPixelByTextureType = {
        { 1, 32 }, { 2, 16 }, { 3, 4 }, { 4, 8 }, { 5, 4 }, { 6, 8 }, { 7, 4 }, { 8, 8 }, { 9, 16 },
    };

    std::unordered_map<std::string, std::pair<int32_t, int32_t>> inferredDimensionsByTexture;
    for (auto& [symbol, meta] : textureUsage) {
        auto u64It = u64Map.find(symbol);
        if (u64It == u64Map.end()) {
            outError = "referenced texture array not found in model.inc.c: " + symbol;
            return false;
        }
        if (u64It->second.empty()) {
            const auto includeIt = includePathBySymbol.find(symbol);
            if (includeIt != includePathBySymbol.end() && includeResolver) {
                std::vector<uint8_t> resolvedBytes;
                int32_t decodedWidth = 0;
                int32_t decodedHeight = 0;
                std::string resolveError;
                if (!includeResolver(includeIt->second, meta, resolvedBytes, decodedWidth, decodedHeight,
                                    resolveError)) {
                    outError = "failed to resolve Fast64 include for " + symbol + ": " + resolveError;
                    return false;
                }
                u64It->second = std::move(resolvedBytes);
                if (decodedWidth > 0 && decodedHeight > 0) {
                    meta.hasDecodedDimensions = true;
                    meta.decodedWidth = decodedWidth;
                    meta.decodedHeight = decodedHeight;
                }
            }
        }
        if (u64It->second.empty()) {
            const auto includeIt = includePathBySymbol.find(symbol);
            if (includeIt != includePathBySymbol.end()) {
                outError = "texture array " + symbol + " resolved from include but produced no bytes: " + includeIt->second;
            } else {
                outError = "texture array " + symbol + " has no inline data and no resolvable include";
            }
            return false;
        }

        const std::string normalizedSize = NormalizeTextureSizeToken(meta.size);
        const std::string textureKey = meta.format + "|" + normalizedSize;
        const auto textureTypeIt = kTextureTypeByFormatSize.find(textureKey);
        if (textureTypeIt == kTextureTypeByFormatSize.end()) {
            outError = "unsupported texture format/size: " + textureKey + " (" + symbol + ")";
            return false;
        }
        const int32_t textureType = textureTypeIt->second;
        const int32_t bitsPerPixel =
            kBitsPerPixelByTextureType.count(textureType) != 0 ? kBitsPerPixelByTextureType.at(textureType) : 0;
        const auto [width, height] = InferTextureDimensions(meta, u64It->second.size(), bitsPerPixel);

        std::vector<uint8_t> textureData = BuildOtexHeader(0x4F544558, 0, 0);
        AppendU32LE(textureData, static_cast<uint32_t>(textureType));
        AppendU32LE(textureData, static_cast<uint32_t>(std::max(width, 1)));
        AppendU32LE(textureData, static_cast<uint32_t>(std::max(height, 1)));
        AppendU32LE(textureData, static_cast<uint32_t>(u64It->second.size()));
        textureData.insert(textureData.end(), u64It->second.begin(), u64It->second.end());

        outConversion.resources["objects/" + objectName + "/" + symbol] = std::move(textureData);
        inferredDimensionsByTexture[symbol] = { std::max(width, 1), std::max(height, 1) };
        outConversion.textureCount++;
    }

    for (const auto& [symbol, dims] : inferredDimensionsByTexture) {
        const auto metaIt = textureUsage.find(symbol);
        if (metaIt == textureUsage.end()) {
            continue;
        }
        const int32_t factor = DetectConservativeTileScaleFactor(metaIt->second, dims.first, dims.second);
        if (factor <= 1) {
            continue;
        }

        const size_t adjustedCommands = ApplyConservativeTileScaleAdjustment(displayListCommands, symbol, factor);
        if (adjustedCommands > 0) {
            SPDLOG_INFO(
                "[ExternalMods] Applied conservative texture tile auto-adjust for {} [{}]: texture={} factor={} commands={}",
                debugLabel.empty() ? objectName : debugLabel, objectName, symbol, factor, adjustedCommands);
        }
    }

    size_t replacedFilterCommands = 0;
    size_t injectedFilterCommands = 0;
    ApplyForcedTextureFilterToDisplayLists(displayListCommands, configuredTextureFilter, replacedFilterCommands,
                                           injectedFilterCommands);
    if (configuredTextureFilter != ExternalModModelTextureFilter::Auto &&
        (replacedFilterCommands > 0 || injectedFilterCommands > 0)) {
        const char* configuredFilterName =
            configuredTextureFilter == ExternalModModelTextureFilter::Point ? "point" : "bilerp";
        SPDLOG_INFO("[ExternalMods] Applied modelTextureFilter override for {} [{}]: configured={} replaced={} injected={}",
                    debugLabel.empty() ? objectName : debugLabel, objectName, configuredFilterName,
                    replacedFilterCommands, injectedFilterCommands);
    }

    for (const auto& [path, commands] : displayListCommands) {
        const std::string xmlText = SerializeFast64DisplayListXml(commands);
        outConversion.resources[path] = std::vector<uint8_t>(xmlText.begin(), xmlText.end());
    }

    std::vector<std::string> gfxNames;
    gfxNames.reserve(gfxArrays.size());
    for (const auto& gfxArray : gfxArrays) {
        gfxNames.push_back(gfxArray.name);
    }
    const std::string rootName = DetermineRootDisplayListName(gfxNames, calledDisplayLists);
    if (rootName.empty()) {
        outError = "unable to determine root display list";
        return false;
    }

    outConversion.rootDisplayListPath = "objects/" + objectName + "/" + rootName;
    return true;
}

std::string DeriveObjectNameFromPath(const std::filesystem::path& path, const std::string& fallback = "external_model") {
    std::string candidate;
    if (!path.empty()) {
        candidate = path.filename().generic_string();
        if (candidate.empty()) {
            candidate = path.stem().generic_string();
        }
    }
    if (candidate.empty()) {
        candidate = fallback;
    }

    std::string sanitized;
    sanitized.reserve(candidate.size());
    for (const unsigned char ch : candidate) {
        if (std::isalnum(ch) || ch == '_') {
            sanitized.push_back(static_cast<char>(std::tolower(ch)));
        } else if (ch == '.' || ch == '-' || ch == ' ') {
            sanitized.push_back('_');
        }
    }
    if (sanitized.empty()) {
        sanitized = "external_model";
    }
    return sanitized;
}

std::filesystem::path BuildGeneratedFast64ArchivePath(const std::string& modId, const std::string& itemId,
                                                      const std::string& objectName) {
    const std::string fileName =
        SanitizeModIdForPath(itemId.empty() ? objectName : itemId) + "_" + SanitizeModIdForPath(objectName) + ".o2r";
    return std::filesystem::path("__external_mods_generated__") / SanitizeModIdForPath(modId) / fileName;
}

std::shared_ptr<Ship::Archive> CreateGeneratedFast64Archive(const std::filesystem::path& virtualArchivePath,
                                                            std::map<std::string, std::vector<uint8_t>> resources,
                                                            std::string& outError) {
    if (resources.empty()) {
        outError = "empty resource set";
        return nullptr;
    }

    auto archive = std::make_shared<ExternalModsInMemoryArchive>(virtualArchivePath.generic_string(), std::move(resources));
    archive->Load();
    if (!archive->IsLoaded()) {
        outError = "failed to initialize in-memory archive";
        return nullptr;
    }

    return archive;
}

std::filesystem::path GetExternalModsCacheRootPath() {
    return std::filesystem::path(Ship::Context::GetPathRelativeToAppDirectory("cache/external-mod-cache", "soh"));
}

std::filesystem::path GetExternalModsLegacyCacheRootPath() {
    return std::filesystem::path(Ship::Context::GetPathRelativeToAppDirectory("mods/.external-mod-cache", "soh"));
}

bool ReadZipEntryBytesByIndex(zip_t* archive, zip_uint64_t index, uint64_t maxBytes, std::vector<uint8_t>& outBytes,
                              std::string& outError) {
    outBytes.clear();

    zip_stat_t stat;
    zip_stat_init(&stat);
    if (zip_stat_index(archive, index, ZIP_FL_ENC_GUESS, &stat) != 0) {
        outError = "failed to stat zip entry";
        return false;
    }

    if (stat.size > maxBytes) {
        outError = "zip entry exceeds max size";
        return false;
    }

    zip_file_t* file = zip_fopen_index(archive, index, ZIP_FL_ENC_GUESS);
    if (file == nullptr) {
        outError = "failed to open zip entry";
        return false;
    }

    outBytes.resize(static_cast<size_t>(stat.size));
    uint64_t totalRead = 0;
    while (totalRead < stat.size) {
        const auto toRead = static_cast<zip_uint64_t>(stat.size - totalRead);
        const auto readCount = zip_fread(file, outBytes.data() + totalRead, toRead);
        if (readCount < 0) {
            outError = "failed to read zip entry";
            zip_fclose(file);
            return false;
        }
        if (readCount == 0) {
            break;
        }
        totalRead += static_cast<uint64_t>(readCount);
    }

    zip_fclose(file);
    outBytes.resize(static_cast<size_t>(totalRead));
    return true;
}

bool IsDisplayListXml(const std::vector<uint8_t>& bytes) {
    size_t index = 0;
    while (index < bytes.size() && std::isspace(static_cast<unsigned char>(bytes[index]))) {
        ++index;
    }
    if (index >= bytes.size()) {
        return false;
    }

    static const std::string kPrefix = "<DisplayList";
    if (index + kPrefix.size() > bytes.size()) {
        return false;
    }
    for (size_t i = 0; i < kPrefix.size(); ++i) {
        if (static_cast<char>(bytes[index + i]) != kPrefix[i]) {
            return false;
        }
    }
    return true;
}

bool ParseObjectAndSymbolFromResourcePath(const std::string& path, std::string& outObjectName, std::string& outSymbol) {
    outObjectName.clear();
    outSymbol.clear();

    const std::string normalized = NormalizeZipEntryPath(path);
    if (normalized.rfind("objects/", 0) != 0) {
        return false;
    }

    const size_t objectStart = std::string("objects/").size();
    const size_t objectEnd = normalized.find('/', objectStart);
    if (objectEnd == std::string::npos || objectEnd + 1 >= normalized.size()) {
        return false;
    }

    outObjectName = normalized.substr(objectStart, objectEnd - objectStart);
    outSymbol = normalized.substr(objectEnd + 1);
    return !outObjectName.empty() && !outSymbol.empty();
}

bool InspectFast64ArchiveBytes(const std::vector<uint8_t>& archiveBytes, Fast64ArchiveInspection& outInspection,
                               std::string& outError) {
    outInspection = Fast64ArchiveInspection{};

    if (archiveBytes.empty()) {
        outError = "archive is empty";
        return false;
    }

    zip_error_t zipError;
    zip_error_init(&zipError);
    zip_source_t* source = zip_source_buffer_create(archiveBytes.data(), archiveBytes.size(), 0, &zipError);
    if (source == nullptr) {
        outError = "failed to open archive source";
        zip_error_fini(&zipError);
        return false;
    }

    zip_t* archive = zip_open_from_source(source, ZIP_RDONLY, &zipError);
    if (archive == nullptr) {
        outError = "failed to parse archive as zip";
        zip_source_free(source);
        zip_error_fini(&zipError);
        return false;
    }
    zip_error_fini(&zipError);

    outInspection.isZipArchive = true;

    std::string modelIncPath;
    zip_uint64_t modelIncIndex = 0;
    bool hasModelInc = false;
    std::string headerPath;
    zip_uint64_t headerIndex = 0;
    bool hasHeader = false;
    std::vector<std::pair<std::string, zip_uint64_t>> displayListEntries;

    const zip_int64_t totalEntries = zip_get_num_entries(archive, ZIP_FL_UNCHANGED);
    if (totalEntries < 0) {
        outError = "failed to enumerate archive entries";
        zip_close(archive);
        return false;
    }

    for (zip_uint64_t index = 0; index < static_cast<zip_uint64_t>(totalEntries); ++index) {
        const char* rawName = zip_get_name(archive, index, ZIP_FL_ENC_GUESS);
        if (rawName == nullptr) {
            continue;
        }
        const std::string normalizedName = NormalizeZipEntryPath(rawName);
        if (normalizedName.empty() || normalizedName.back() == '/') {
            continue;
        }

        if (EndsWithString(normalizedName, "/model.inc.c") || normalizedName == "model.inc.c") {
            if (!hasModelInc) {
                hasModelInc = true;
                modelIncPath = normalizedName;
                modelIncIndex = index;
            }
            continue;
        }
        if (EndsWithString(normalizedName, "/header.h") || normalizedName == "header.h") {
            if (!hasHeader) {
                hasHeader = true;
                headerPath = normalizedName;
                headerIndex = index;
            }
            continue;
        }

        if (normalizedName.rfind("objects/", 0) != 0) {
            continue;
        }

        std::vector<uint8_t> entryBytes;
        std::string readError;
        if (!ReadZipEntryBytesByIndex(archive, index, kMaxItemModelBytes, entryBytes, readError)) {
            continue;
        }
        if (IsDisplayListXml(entryBytes)) {
            displayListEntries.emplace_back(normalizedName, index);
        }
    }

    if (hasModelInc) {
        std::vector<uint8_t> bytes;
        if (!ReadZipEntryBytesByIndex(archive, modelIncIndex, kMaxItemModelBytes, bytes, outError)) {
            zip_close(archive);
            return false;
        }
        outInspection.modelIncContent.assign(bytes.begin(), bytes.end());
    }

    if (hasHeader) {
        std::vector<uint8_t> bytes;
        if (!ReadZipEntryBytesByIndex(archive, headerIndex, kMaxItemModelBytes, bytes, outError)) {
            zip_close(archive);
            return false;
        }
        outInspection.headerContent.assign(bytes.begin(), bytes.end());
    }

    if (!modelIncPath.empty()) {
        const std::filesystem::path path(modelIncPath);
        const auto parent = path.parent_path().filename().generic_string();
        if (!parent.empty()) {
            outInspection.objectNameHint = parent;
        }
    } else if (!headerPath.empty()) {
        const std::filesystem::path path(headerPath);
        const auto parent = path.parent_path().filename().generic_string();
        if (!parent.empty()) {
            outInspection.objectNameHint = parent;
        }
    }

    outInspection.hasFast64Source = !outInspection.modelIncContent.empty() && !outInspection.headerContent.empty();
    outInspection.hasDisplayListResources = !displayListEntries.empty();

    if (!displayListEntries.empty()) {
        std::unordered_map<std::string, std::vector<std::string>> displayListsByObject;
        std::unordered_map<std::string, std::unordered_set<std::string>> calledByObject;

        static const std::regex callPattern(R"regex(<CallDisplayList[^>]*Path="([^"]+)")regex");
        for (const auto& [path, index] : displayListEntries) {
            std::string objectName;
            std::string symbol;
            if (!ParseObjectAndSymbolFromResourcePath(path, objectName, symbol)) {
                continue;
            }
            displayListsByObject[objectName].push_back(symbol);

            std::vector<uint8_t> entryBytes;
            std::string readError;
            if (!ReadZipEntryBytesByIndex(archive, index, kMaxItemModelBytes, entryBytes, readError)) {
                continue;
            }
            std::string xmlText(entryBytes.begin(), entryBytes.end());
            for (std::sregex_iterator it(xmlText.begin(), xmlText.end(), callPattern), end; it != end; ++it) {
                std::string calledObjectName;
                std::string calledSymbol;
                if (ParseObjectAndSymbolFromResourcePath((*it)[1].str(), calledObjectName, calledSymbol) &&
                    calledObjectName == objectName) {
                    calledByObject[objectName].insert(calledSymbol);
                }
            }
        }

        std::string selectedObject;
        size_t selectedCount = 0;
        for (const auto& [objectName, entries] : displayListsByObject) {
            if (entries.size() > selectedCount) {
                selectedObject = objectName;
                selectedCount = entries.size();
            }
        }

        if (!selectedObject.empty()) {
            const auto& names = displayListsByObject[selectedObject];
            const auto calledIt = calledByObject.find(selectedObject);
            const std::unordered_set<std::string> emptyCalled;
            const auto& called = calledIt != calledByObject.end() ? calledIt->second : emptyCalled;
            const std::string rootName = DetermineRootDisplayListName(names, called);
            if (!rootName.empty()) {
                outInspection.rootDisplayListPath = "objects/" + selectedObject + "/" + rootName;
                if (outInspection.objectNameHint.empty()) {
                    outInspection.objectNameHint = selectedObject;
                }
            }
        }
    }

    zip_close(archive);
    return true;
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

bool ParseSingleButtonMaskToken(const std::string& rawValue, int32_t& outMask, std::string& outError) {
    const auto value = ToUpper(rawValue);
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

bool ParseButtonMask(const nlohmann::json& json, int32_t& outMask, std::string& outError) {
    if (json.is_array()) {
        if (json.empty()) {
            outError = "button mask array must not be empty";
            return false;
        }

        int64_t combinedMask = 0;
        for (size_t i = 0; i < json.size(); ++i) {
            int32_t entryMask = 0;
            if (!ParseButtonMask(json[i], entryMask, outError)) {
                outError = "button mask entry[" + std::to_string(i) + "]: " + outError;
                return false;
            }
            combinedMask |= static_cast<uint32_t>(entryMask);
        }

        if (combinedMask < 0 || combinedMask > std::numeric_limits<int32_t>::max()) {
            outError = "button mask out of range";
            return false;
        }

        outMask = static_cast<int32_t>(combinedMask);
        return true;
    }

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
        outError = "button mask must be integer, alias string, or array";
        return false;
    }

    const auto trim = [](const std::string& value) {
        size_t start = 0;
        while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
            ++start;
        }

        size_t end = value.size();
        while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
            --end;
        }

        return value.substr(start, end - start);
    };

    const auto value = json.get<std::string>();
    std::vector<std::string> tokens;
    std::string current;
    for (const char c : value) {
        if (c == '|' || c == '+') {
            const auto token = trim(current);
            if (!token.empty()) {
                tokens.push_back(token);
            }
            current.clear();
            continue;
        }
        current.push_back(c);
    }

    const auto trailingToken = trim(current);
    if (!trailingToken.empty()) {
        tokens.push_back(trailingToken);
    }

    if (tokens.empty()) {
        outError = "button mask alias is empty";
        return false;
    }

    int64_t combinedMask = 0;
    for (size_t i = 0; i < tokens.size(); ++i) {
        int32_t tokenMask = 0;
        if (!ParseSingleButtonMaskToken(tokens[i], tokenMask, outError)) {
            outError = "button mask token[" + std::to_string(i) + "]: " + outError;
            return false;
        }

        combinedMask |= static_cast<uint32_t>(tokenMask);
    }

    if (combinedMask < 0 || combinedMask > std::numeric_limits<int32_t>::max()) {
        outError = "button mask out of range";
        return false;
    }

    outMask = static_cast<int32_t>(combinedMask);
    return true;
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

bool ParseHookDispatchType(const nlohmann::json& json, ExternalModHookDispatchType& outType, std::string& outError) {
    if (!json.is_string()) {
        outError = "dispatch must be string";
        return false;
    }

    const auto dispatch = ToLower(json.get<std::string>());
    if (dispatch == "actions") {
        outType = ExternalModHookDispatchType::Actions;
        return true;
    }
    if (dispatch == "wasmexport") {
        outType = ExternalModHookDispatchType::WasmExport;
        return true;
    }

    outError = "dispatch must be one of: actions|wasmExport";
    return false;
}

bool ParseOptionalFilterInt16(const nlohmann::json& filters, const char* key, bool& hasValue, int16_t& outValue,
                              std::string& outError) {
    hasValue = false;
    if (!filters.contains(key)) {
        return true;
    }

    if (!filters[key].is_number_integer()) {
        outError = std::string("filters.") + key + " must be integer";
        return false;
    }

    const auto raw = filters[key].get<long long>();
    if (raw < std::numeric_limits<int16_t>::min() || raw > std::numeric_limits<int16_t>::max()) {
        outError = std::string("filters.") + key + " out of range";
        return false;
    }

    hasValue = true;
    outValue = static_cast<int16_t>(raw);
    return true;
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

    auto parseActorHandle = [&](uint32_t& outHandle) {
        const char* key = json.contains("actorHandle") ? "actorHandle" : (json.contains("handle") ? "handle" : nullptr);
        if (key == nullptr) {
            outError = "action requires actorHandle";
            return false;
        }
        if (!json[key].is_number_integer()) {
            outError = std::string(key) + " must be integer";
            return false;
        }
        const auto raw = json[key].get<long long>();
        if (raw <= 0 || raw > std::numeric_limits<uint32_t>::max()) {
            outError = std::string(key) + " out of range";
            return false;
        }
        outHandle = static_cast<uint32_t>(raw);
        return true;
    };

    if (actionType == "pressButton") {
        const char* key = json.contains("button") ? "button" : (json.contains("mask") ? "mask" : nullptr);
        if (key == nullptr) {
            outError = "pressButton requires button";
            return false;
        }
        outAction.type = ExternalModActionType::PressButton;
        return ParseButtonMask(json[key], outAction.buttonMask, outError);
    }

    if (actionType == "loadModScene") {
        outAction.type = ExternalModActionType::LoadModScene;
        if (!ValidateRequiredString(json, "sceneId", outAction.modSceneId, outError)) {
            return false;
        }
        if (json.contains("spawnId")) {
            if (!json["spawnId"].is_number_integer()) {
                outError = "loadModScene spawnId must be integer";
                return false;
            }
            outAction.sceneSpawnId = json["spawnId"].get<int32_t>();
        }
        return true;
    }

    if (actionType == "showEquippedItemGet" || actionType == "showEquippedItemPickup") {
        const char* key = json.contains("button") ? "button" : (json.contains("slotButton") ? "slotButton" : nullptr);
        if (key == nullptr) {
            outError = "showEquippedItemGet requires button";
            return false;
        }
        outAction.type = ExternalModActionType::ShowEquippedItemGet;
        if (!ParseButtonMask(json[key], outAction.buttonMask, outError)) {
            return false;
        }

        int32_t resolvedMask = 0;
        int32_t slotIndex = 0;
        if (!TryResolveEquippedActionButtonMask(outAction.buttonMask, resolvedMask, slotIndex)) {
            outError = "showEquippedItemGet button must include exactly one of " + BuildSupportedEquippedActionButtonList();
            return false;
        }
        return true;
    }

    if (actionType == "spawnSmoke") {
        outAction.type = ExternalModActionType::SpawnSmoke;
        return true;
    }

    if (actionType == "spawnKusa") {
        outAction.type = ExternalModActionType::SpawnKusa;
        return true;
    }

    if (actionType == "lanternLight") {
        outAction.type = ExternalModActionType::LanternLight;
        if (json.contains("itemId")) {
            return ValidateRequiredString(json, "itemId", outAction.itemId, outError);
        }
        if (json.contains("requiresItemId")) {
            return ValidateRequiredString(json, "requiresItemId", outAction.itemId, outError);
        }
        return true;
    }

    if (actionType == "igniteFrontTarget") {
        outAction.type = ExternalModActionType::IgniteFrontTarget;
        if (json.contains("itemId")) {
            return ValidateRequiredString(json, "itemId", outAction.itemId, outError);
        }
        if (json.contains("requiresItemId")) {
            return ValidateRequiredString(json, "requiresItemId", outAction.itemId, outError);
        }
        return true;
    }

    if (actionType == "spawnActor") {
        outAction.type = ExternalModActionType::SpawnActor;
        if (json.contains("actorDefinitionId")) {
            return ValidateRequiredString(json, "actorDefinitionId", outAction.actorDefinitionId, outError);
        }
        if (json.contains("definitionId")) {
            return ValidateRequiredString(json, "definitionId", outAction.actorDefinitionId, outError);
        }
        if (json.contains("id")) {
            return ValidateRequiredString(json, "id", outAction.actorDefinitionId, outError);
        }
        outError = "spawnActor requires actorDefinitionId";
        return false;
    }

    if (actionType == "despawnActor") {
        outAction.type = ExternalModActionType::DespawnActor;
        return parseActorHandle(outAction.actorHandle);
    }

    if (actionType == "setActorState") {
        outAction.type = ExternalModActionType::SetActorState;
        if (!parseActorHandle(outAction.actorHandle)) {
            return false;
        }
        const char* keyName = json.contains("stateKey") ? "stateKey" : (json.contains("key") ? "key" : nullptr);
        if (keyName == nullptr || !ValidateRequiredString(json, keyName, outAction.actorStateKey, outError)) {
            outError = "setActorState requires key";
            return false;
        }

        const char* valueName = json.contains("stateValue") ? "stateValue" : (json.contains("value") ? "value" : nullptr);
        if (valueName == nullptr) {
            outError = "setActorState requires value";
            return false;
        }
        if (json[valueName].is_string()) {
            outAction.actorStateValue = json[valueName].get<std::string>();
            return true;
        }
        if (json[valueName].is_number_integer()) {
            outAction.actorStateValue = std::to_string(json[valueName].get<long long>());
            return true;
        }
        if (json[valueName].is_number_float()) {
            outAction.actorStateValue = std::to_string(json[valueName].get<double>());
            return true;
        }
        if (json[valueName].is_boolean()) {
            outAction.actorStateValue = json[valueName].get<bool>() ? "true" : "false";
            return true;
        }
        outError = "setActorState value must be string|number|boolean";
        return false;
    }

    if (actionType == "moveActorToPathNode") {
        outAction.type = ExternalModActionType::MoveActorToPathNode;
        if (!parseActorHandle(outAction.actorHandle)) {
            return false;
        }
        const char* nodeKey = json.contains("pathNodeIndex") ? "pathNodeIndex" : (json.contains("node") ? "node" : nullptr);
        if (nodeKey == nullptr || !json[nodeKey].is_number_integer()) {
            outError = "moveActorToPathNode requires integer pathNodeIndex";
            return false;
        }
        outAction.pathNodeIndex = json[nodeKey].get<int32_t>();
        return true;
    }

    if (actionType == "openDialog") {
        outAction.type = ExternalModActionType::OpenDialog;
        const char* textKey = json.contains("dialogId") ? "dialogId" : (json.contains("textId") ? "textId" : nullptr);
        if (textKey == nullptr || !json[textKey].is_number_integer()) {
            outError = "openDialog requires integer dialogId";
            return false;
        }
        outAction.dialogId = json[textKey].get<int32_t>();
        if (json.contains("actorHandle") || json.contains("handle")) {
            return parseActorHandle(outAction.actorHandle);
        }
        return true;
    }

    if (actionType == "setSwitchFlag") {
        outAction.type = ExternalModActionType::SetSwitchFlag;
        if (!json.contains("flag") || !json["flag"].is_number_integer()) {
            outError = "setSwitchFlag requires integer flag";
            return false;
        }
        outAction.intValue = json["flag"].get<int32_t>();
        return true;
    }

    if (actionType == "clearSwitchFlag") {
        outAction.type = ExternalModActionType::ClearSwitchFlag;
        if (!json.contains("flag") || !json["flag"].is_number_integer()) {
            outError = "clearSwitchFlag requires integer flag";
            return false;
        }
        outAction.intValue = json["flag"].get<int32_t>();
        return true;
    }

    if (actionType == "setEventChkInf") {
        outAction.type = ExternalModActionType::SetEventChkInf;
        if (!json.contains("flag") || !json["flag"].is_number_integer()) {
            outError = "setEventChkInf requires integer flag";
            return false;
        }
        outAction.intValue = json["flag"].get<int32_t>();
        return true;
    }

    if (actionType == "clearEventChkInf") {
        outAction.type = ExternalModActionType::ClearEventChkInf;
        if (!json.contains("flag") || !json["flag"].is_number_integer()) {
            outError = "clearEventChkInf requires integer flag";
            return false;
        }
        outAction.intValue = json["flag"].get<int32_t>();
        return true;
    }

    if (actionType == "setInfTable") {
        outAction.type = ExternalModActionType::SetInfTable;
        if (!json.contains("flag") || !json["flag"].is_number_integer()) {
            outError = "setInfTable requires integer flag";
            return false;
        }
        outAction.intValue = json["flag"].get<int32_t>();
        return true;
    }

    if (actionType == "clearInfTable") {
        outAction.type = ExternalModActionType::ClearInfTable;
        if (!json.contains("flag") || !json["flag"].is_number_integer()) {
            outError = "clearInfTable requires integer flag";
            return false;
        }
        outAction.intValue = json["flag"].get<int32_t>();
        return true;
    }

    if (actionType == "giveRupees") {
        outAction.type = ExternalModActionType::GiveRupees;
        if (!json.contains("amount") || !json["amount"].is_number_integer()) {
            outError = "giveRupees requires integer amount";
            return false;
        }
        outAction.intValue = std::abs(json["amount"].get<int32_t>());
        return true;
    }

    if (actionType == "takeRupees") {
        outAction.type = ExternalModActionType::TakeRupees;
        if (!json.contains("amount") || !json["amount"].is_number_integer()) {
            outError = "takeRupees requires integer amount";
            return false;
        }
        outAction.intValue = std::abs(json["amount"].get<int32_t>());
        return true;
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

    if (actionType == "setVar") {
        outAction.type = ExternalModActionType::SetVar;
        if (!ValidateRequiredString(json, "scope", outAction.variableScope, outError)) {
            return false;
        }
        if (!ValidateRequiredString(json, "key", outAction.variableKey, outError)) {
            return false;
        }
        if ((json.contains("actorHandle") || json.contains("handle")) && !parseActorHandle(outAction.actorHandle)) {
            return false;
        }
        if (!json.contains("value")) {
            outError = "setVar requires value";
            return false;
        }
        if (json["value"].is_string()) {
            outAction.variableValue = json["value"].get<std::string>();
            return true;
        }
        if (json["value"].is_boolean()) {
            outAction.variableValue = json["value"].get<bool>() ? "true" : "false";
            return true;
        }
        if (json["value"].is_number()) {
            outAction.variableNumber = json["value"].get<float>();
            outAction.variableValue = std::to_string(outAction.variableNumber);
            outAction.variableHasNumber = true;
            return true;
        }
        outError = "setVar value must be string|number|boolean";
        return false;
    }

    if (actionType == "addVar") {
        outAction.type = ExternalModActionType::AddVar;
        if (!ValidateRequiredString(json, "scope", outAction.variableScope, outError)) {
            return false;
        }
        if (!ValidateRequiredString(json, "key", outAction.variableKey, outError)) {
            return false;
        }
        if ((json.contains("actorHandle") || json.contains("handle")) && !parseActorHandle(outAction.actorHandle)) {
            return false;
        }
        if (!json.contains("value") || !json["value"].is_number()) {
            outError = "addVar requires numeric value";
            return false;
        }
        outAction.variableNumber = json["value"].get<float>();
        outAction.variableHasNumber = true;
        return true;
    }

    if (actionType == "clampVar") {
        outAction.type = ExternalModActionType::ClampVar;
        if (!ValidateRequiredString(json, "scope", outAction.variableScope, outError)) {
            return false;
        }
        if (!ValidateRequiredString(json, "key", outAction.variableKey, outError)) {
            return false;
        }
        if ((json.contains("actorHandle") || json.contains("handle")) && !parseActorHandle(outAction.actorHandle)) {
            return false;
        }
        if (!json.contains("min") || !json["min"].is_number() || !json.contains("max") || !json["max"].is_number()) {
            outError = "clampVar requires numeric min and max";
            return false;
        }
        outAction.variableMin = json["min"].get<float>();
        outAction.variableMax = json["max"].get<float>();
        if (outAction.variableMin > outAction.variableMax) {
            outError = "clampVar requires min <= max";
            return false;
        }
        outAction.variableHasRange = true;
        return true;
    }

    if (actionType == "emitSignal") {
        outAction.type = ExternalModActionType::EmitSignal;
        if (!ValidateRequiredString(json, "name", outAction.signalName, outError)) {
            return false;
        }
        if (json.contains("actorHandle") || json.contains("handle")) {
            return parseActorHandle(outAction.actorHandle);
        }
        return true;
    }

    if (actionType == "callBehavior") {
        outAction.type = ExternalModActionType::CallBehavior;
        if (!ValidateRequiredString(json, "behaviorId", outAction.behaviorId, outError)) {
            return false;
        }
        if (json.contains("actorHandle") || json.contains("handle")) {
            return parseActorHandle(outAction.actorHandle);
        }
        return true;
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

bool MatchPressedButtonMask(int32_t currentButtons, int32_t previousButtons, int32_t mask) {
    if (mask == 0) {
        return false;
    }

    const int32_t newlyPressed = currentButtons & ~previousButtons;
    return ((currentButtons & mask) == mask) && ((newlyPressed & mask) != 0);
}

bool MatchReleasedButtonMask(int32_t currentButtons, int32_t previousButtons, int32_t mask) {
    if (mask == 0) {
        return false;
    }

    const int32_t newlyReleased = previousButtons & ~currentButtons;
    return ((previousButtons & mask) == mask) && ((newlyReleased & mask) != 0);
}

float GetParamOrDefault(const std::unordered_map<std::string, float>& params, const char* key, float defaultValue) {
    const auto it = params.find(key);
    if (it == params.end()) {
        return defaultValue;
    }
    return it->second;
}

struct ExternalModItemSlotConfig {
    ExternalModItemSlot slot;
    const char* slotName;
    int32_t slotIndex;
    int32_t grantItemId;
    int32_t ammoItemId;
    std::array<int32_t, 3> useItemIds;
    u8 vanillaAgeReq;
};

constexpr std::array<ExternalModItemSlotConfig, 7> kSupportedItemSlotConfigs = {{
    { ExternalModItemSlot::Hookshot, "SLOT_HOOKSHOT", SLOT_HOOKSHOT, ITEM_HOOKSHOT, ITEM_NONE,
      { ITEM_HOOKSHOT, ITEM_LONGSHOT, ITEM_NONE }, kAgeReqAdult },
    { ExternalModItemSlot::Stick, "SLOT_STICK", SLOT_STICK, ITEM_STICK, ITEM_STICK,
      { ITEM_STICK, ITEM_NONE, ITEM_NONE }, kAgeReqChild },
    { ExternalModItemSlot::Bow, "SLOT_BOW", SLOT_BOW, ITEM_BOW, ITEM_BOW,
      { ITEM_BOW, ITEM_NONE, ITEM_NONE }, kAgeReqAdult },
    { ExternalModItemSlot::FireArrow, "SLOT_ARROW_FIRE", SLOT_ARROW_FIRE, ITEM_ARROW_FIRE, ITEM_BOW,
      { ITEM_ARROW_FIRE, ITEM_BOW_ARROW_FIRE, ITEM_NONE }, kAgeReqAdult },
    { ExternalModItemSlot::IceArrow, "SLOT_ARROW_ICE", SLOT_ARROW_ICE, ITEM_ARROW_ICE, ITEM_BOW,
      { ITEM_ARROW_ICE, ITEM_BOW_ARROW_ICE, ITEM_NONE }, kAgeReqAdult },
    { ExternalModItemSlot::LightArrow, "SLOT_ARROW_LIGHT", SLOT_ARROW_LIGHT, ITEM_ARROW_LIGHT, ITEM_BOW,
      { ITEM_ARROW_LIGHT, ITEM_BOW_ARROW_LIGHT, ITEM_NONE }, kAgeReqAdult },
    { ExternalModItemSlot::Hammer, "SLOT_HAMMER", SLOT_HAMMER, ITEM_HAMMER, ITEM_NONE,
      { ITEM_HAMMER, ITEM_NONE, ITEM_NONE }, kAgeReqAdult },
}};

const ExternalModItemSlotConfig* FindItemSlotConfig(ExternalModItemSlot slot) {
    for (const auto& config : kSupportedItemSlotConfigs) {
        if (config.slot == slot) {
            return &config;
        }
    }
    return nullptr;
}

const ExternalModItemSlotConfig* FindItemSlotConfigByName(const std::string& slotName) {
    for (const auto& config : kSupportedItemSlotConfigs) {
        if (slotName == config.slotName) {
            return &config;
        }
    }
    return nullptr;
}

std::string BuildSupportedItemSlotList() {
    std::string out;
    for (size_t i = 0; i < kSupportedItemSlotConfigs.size(); ++i) {
        if (!out.empty()) {
            out += ", ";
        }
        out += kSupportedItemSlotConfigs[i].slotName;
    }
    return out;
}

bool ItemIdMatchesSlotConfig(const ExternalModItemSlotConfig& config, int32_t itemId) {
    if (itemId == config.grantItemId) {
        return true;
    }

    for (const auto candidate : config.useItemIds) {
        if (candidate != ITEM_NONE && candidate == itemId) {
            return true;
        }
    }

    return false;
}

bool ItemDefinitionMatchesUseItem(const ExternalModItemDefinition& definition, int32_t itemId) {
    const auto* config = FindItemSlotConfig(definition.slot);
    if (config == nullptr) {
        return false;
    }

    if (definition.hasGrantItemId && definition.grantItemId == itemId) {
        return true;
    }

    return ItemIdMatchesSlotConfig(*config, itemId);
}

int32_t ResolveGrantedItemId(const ExternalModItemDefinition& definition) {
    if (definition.hasGrantItemId) {
        return definition.grantItemId;
    }

    const auto* config = FindItemSlotConfig(definition.slot);
    if (config == nullptr) {
        return ITEM_NONE;
    }

    return config->grantItemId;
}

int32_t ResolveGrantedAmmoItemId(const ExternalModItemDefinition& definition) {
    const auto* config = FindItemSlotConfig(definition.slot);
    if (config == nullptr) {
        return ITEM_NONE;
    }

    return config->ammoItemId;
}

bool IsItemIconIndexValid(int32_t itemId) {
    return itemId >= 0 && itemId < static_cast<int32_t>(kItemIconTableSize);
}

void CaptureVanillaItemIconsIfNeeded() {
    if (gVanillaItemIconsCaptured) {
        return;
    }

    for (size_t i = 0; i < kItemIconTableSize; ++i) {
        gVanillaItemIcons[i] = gItemIcons[i];
    }
    gVanillaItemIconsCaptured = true;
}

void RestoreVanillaItemIcons() {
    if (!gVanillaItemIconsCaptured) {
        return;
    }

    for (size_t i = 0; i < kItemIconTableSize; ++i) {
        gItemIcons[i] = gVanillaItemIcons[i];
    }
}

void ApplyIconOverrideForItemId(int32_t itemId, const ExternalModItemDefinition& definition) {
    if (!IsItemIconIndexValid(itemId) || definition.iconRgba32.empty()) {
        return;
    }

    gItemIcons[itemId] = const_cast<uint8_t*>(definition.iconRgba32.data());
}

void ApplyIconOverrideForDefinition(const ExternalModItemDefinition& definition) {
    if (!definition.granted || definition.iconRgba32.empty()) {
        return;
    }

    const auto* config = FindItemSlotConfig(definition.slot);
    if (config == nullptr) {
        return;
    }

    ApplyIconOverrideForItemId(ResolveGrantedItemId(definition), definition);
    for (const auto itemId : config->useItemIds) {
        if (itemId != ITEM_NONE) {
            ApplyIconOverrideForItemId(itemId, definition);
        }
    }
}

bool TryDecodeItemIconPng(const std::vector<uint8_t>& iconBytes, std::vector<uint8_t>& outRgba32, std::string& outError) {
    outRgba32.clear();
    if (iconBytes.empty()) {
        outError = "icon file is empty";
        return false;
    }

    if (iconBytes.size() > static_cast<size_t>(std::numeric_limits<int>::max())) {
        outError = "icon file is too large for decoder";
        return false;
    }

    int32_t width = 0;
    int32_t height = 0;
    int32_t channels = 0;
    stbi_uc* decoded = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(iconBytes.data()),
                                             static_cast<int32_t>(iconBytes.size()), &width, &height, &channels,
                                             STBI_rgb_alpha);
    if (decoded == nullptr) {
        outError = "png decode failed";
        return false;
    }

    if (width <= 0 || height <= 0 || width > kMaxDecodedIconDimension || height > kMaxDecodedIconDimension) {
        stbi_image_free(decoded);
        outError = "decoded icon has invalid dimensions";
        return false;
    }

    outRgba32.assign(static_cast<size_t>(kItemIconSize * kItemIconSize * 4), 0);
    if (width == kItemIconSize && height == kItemIconSize) {
        std::memcpy(outRgba32.data(), decoded, outRgba32.size());
        stbi_image_free(decoded);
        return true;
    }

    for (int32_t y = 0; y < kItemIconSize; ++y) {
        const int32_t srcY = std::clamp((y * height) / kItemIconSize, 0, height - 1);
        for (int32_t x = 0; x < kItemIconSize; ++x) {
            const int32_t srcX = std::clamp((x * width) / kItemIconSize, 0, width - 1);
            const size_t srcOffset = static_cast<size_t>((srcY * width + srcX) * 4);
            const size_t dstOffset = static_cast<size_t>((y * kItemIconSize + x) * 4);
            std::memcpy(outRgba32.data() + dstOffset, decoded + srcOffset, 4);
        }
    }

    stbi_image_free(decoded);
    return true;
}

bool TryDecodePngToRgba32(const std::vector<uint8_t>& imageBytes, std::vector<uint8_t>& outRgba32, int32_t& outWidth,
                          int32_t& outHeight, std::string& outError) {
    outRgba32.clear();
    outWidth = 0;
    outHeight = 0;

    if (imageBytes.empty()) {
        outError = "image file is empty";
        return false;
    }

    if (imageBytes.size() > static_cast<size_t>(std::numeric_limits<int32_t>::max())) {
        outError = "image file is too large for decoder";
        return false;
    }

    int32_t channels = 0;
    stbi_uc* decoded = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(imageBytes.data()),
                                             static_cast<int32_t>(imageBytes.size()), &outWidth, &outHeight, &channels,
                                             STBI_rgb_alpha);
    if (decoded == nullptr) {
        outError = "png decode failed";
        return false;
    }

    if (outWidth <= 0 || outHeight <= 0 || outWidth > kMaxDecodedIconDimension || outHeight > kMaxDecodedIconDimension) {
        stbi_image_free(decoded);
        outError = "decoded image has invalid dimensions";
        return false;
    }

    outRgba32.assign(static_cast<size_t>(outWidth * outHeight * 4), 0);
    std::memcpy(outRgba32.data(), decoded, outRgba32.size());
    stbi_image_free(decoded);
    return true;
}

void ResizeRgba32Nearest(const std::vector<uint8_t>& sourceRgba32, int32_t sourceWidth, int32_t sourceHeight,
                         int32_t targetWidth, int32_t targetHeight, std::vector<uint8_t>& outRgba32) {
    outRgba32.assign(static_cast<size_t>(targetWidth * targetHeight * 4), 0);
    if (targetWidth <= 0 || targetHeight <= 0 || sourceWidth <= 0 || sourceHeight <= 0) {
        return;
    }

    for (int32_t y = 0; y < targetHeight; ++y) {
        const int32_t srcY = std::clamp((y * sourceHeight) / targetHeight, 0, sourceHeight - 1);
        for (int32_t x = 0; x < targetWidth; ++x) {
            const int32_t srcX = std::clamp((x * sourceWidth) / targetWidth, 0, sourceWidth - 1);
            const size_t srcOffset = static_cast<size_t>((srcY * sourceWidth + srcX) * 4);
            const size_t dstOffset = static_cast<size_t>((y * targetWidth + x) * 4);
            std::memcpy(outRgba32.data() + dstOffset, sourceRgba32.data() + srcOffset, 4);
        }
    }
}

void NormalizeModelTextureForOpaqueRendering(std::vector<uint8_t>& rgba32, int32_t width, int32_t height,
                                             bool fillAllTransparentPixels) {
    if (width <= 0 || height <= 0) {
        return;
    }

    const size_t pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);
    if (rgba32.size() < pixelCount * 4) {
        return;
    }

    constexpr int32_t kLocalPaddingRadius = 4;
    const auto source = rgba32;

    uint64_t sumR = 0;
    uint64_t sumG = 0;
    uint64_t sumB = 0;
    uint32_t opaqueCount = 0;

    for (size_t i = 0; i < pixelCount; ++i) {
        const size_t offset = i * 4;
        if (source[offset + 3] > 0) {
            sumR += source[offset + 0];
            sumG += source[offset + 1];
            sumB += source[offset + 2];
            ++opaqueCount;
        }
    }

    if (opaqueCount == 0) {
        for (size_t i = 0; i < pixelCount; ++i) {
            const size_t offset = i * 4;
            rgba32[offset + 0] = 255;
            rgba32[offset + 1] = 0;
            rgba32[offset + 2] = 255;
            rgba32[offset + 3] = 255;
        }
        return;
    }

    const uint8_t fallbackR = static_cast<uint8_t>(sumR / opaqueCount);
    const uint8_t fallbackG = static_cast<uint8_t>(sumG / opaqueCount);
    const uint8_t fallbackB = static_cast<uint8_t>(sumB / opaqueCount);

    if (fillAllTransparentPixels) {
        std::vector<int32_t> nearestOpaque(pixelCount, -1);
        std::queue<size_t> floodQueue;

        for (size_t i = 0; i < pixelCount; ++i) {
            const size_t alphaOffset = i * 4 + 3;
            if (source[alphaOffset] > 0) {
                nearestOpaque[i] = static_cast<int32_t>(i);
                floodQueue.push(i);
            }
        }

        auto tryVisitNeighbor = [&](size_t fromIndex, int32_t nx, int32_t ny) {
            if (nx < 0 || ny < 0 || nx >= width || ny >= height) {
                return;
            }

            const size_t neighborIndex = static_cast<size_t>(ny * width + nx);
            if (nearestOpaque[neighborIndex] >= 0) {
                return;
            }

            nearestOpaque[neighborIndex] = nearestOpaque[fromIndex];
            floodQueue.push(neighborIndex);
        };

        while (!floodQueue.empty()) {
            const size_t index = floodQueue.front();
            floodQueue.pop();

            const int32_t x = static_cast<int32_t>(index % static_cast<size_t>(width));
            const int32_t y = static_cast<int32_t>(index / static_cast<size_t>(width));
            tryVisitNeighbor(index, x - 1, y);
            tryVisitNeighbor(index, x + 1, y);
            tryVisitNeighbor(index, x, y - 1);
            tryVisitNeighbor(index, x, y + 1);
        }

        for (size_t i = 0; i < pixelCount; ++i) {
            const size_t offset = i * 4;
            if (source[offset + 3] == 0) {
                const int32_t sourceIndex = nearestOpaque[i];
                if (sourceIndex >= 0) {
                    const size_t sourceOffset = static_cast<size_t>(sourceIndex) * 4;
                    rgba32[offset + 0] = source[sourceOffset + 0];
                    rgba32[offset + 1] = source[sourceOffset + 1];
                    rgba32[offset + 2] = source[sourceOffset + 2];
                } else {
                    rgba32[offset + 0] = fallbackR;
                    rgba32[offset + 1] = fallbackG;
                    rgba32[offset + 2] = fallbackB;
                }
            } else {
                rgba32[offset + 0] = source[offset + 0];
                rgba32[offset + 1] = source[offset + 1];
                rgba32[offset + 2] = source[offset + 2];
            }
            rgba32[offset + 3] = 255;
        }

        return;
    }

    for (size_t i = 0; i < pixelCount; ++i) {
        const size_t offset = i * 4;
        if (source[offset + 3] == 0) {
            const int32_t x = static_cast<int32_t>(i % static_cast<size_t>(width));
            const int32_t y = static_cast<int32_t>(i / static_cast<size_t>(width));

            bool foundNeighbor = false;
            int32_t bestDistanceSq = std::numeric_limits<int32_t>::max();
            size_t bestNeighborOffset = 0;

            const int32_t minX = std::max(0, x - kLocalPaddingRadius);
            const int32_t maxX = std::min(width - 1, x + kLocalPaddingRadius);
            const int32_t minY = std::max(0, y - kLocalPaddingRadius);
            const int32_t maxY = std::min(height - 1, y + kLocalPaddingRadius);

            for (int32_t ny = minY; ny <= maxY; ++ny) {
                for (int32_t nx = minX; nx <= maxX; ++nx) {
                    const int32_t dx = nx - x;
                    const int32_t dy = ny - y;
                    const int32_t distanceSq = dx * dx + dy * dy;
                    if (distanceSq == 0 || distanceSq > (kLocalPaddingRadius * kLocalPaddingRadius)) {
                        continue;
                    }

                    const size_t neighborOffset =
                        static_cast<size_t>(ny * width + nx) * 4;
                    if (source[neighborOffset + 3] == 0) {
                        continue;
                    }

                    if (distanceSq < bestDistanceSq) {
                        bestDistanceSq = distanceSq;
                        bestNeighborOffset = neighborOffset;
                        foundNeighbor = true;
                    }
                }
            }

            if (foundNeighbor) {
                rgba32[offset + 0] = source[bestNeighborOffset + 0];
                rgba32[offset + 1] = source[bestNeighborOffset + 1];
                rgba32[offset + 2] = source[bestNeighborOffset + 2];
            } else {
                rgba32[offset + 0] = fallbackR;
                rgba32[offset + 1] = fallbackG;
                rgba32[offset + 2] = fallbackB;
            }
        }
        rgba32[offset + 3] = 255;
    }
}

bool TryDecodePngToRgba32FixedSize(const std::vector<uint8_t>& imageBytes, int32_t targetWidth, int32_t targetHeight,
                                   std::vector<uint8_t>& outRgba32, std::string& outError) {
    int32_t sourceWidth = 0;
    int32_t sourceHeight = 0;
    std::vector<uint8_t> sourceRgba32;
    if (!TryDecodePngToRgba32(imageBytes, sourceRgba32, sourceWidth, sourceHeight, outError)) {
        return false;
    }

    if (sourceWidth == targetWidth && sourceHeight == targetHeight) {
        outRgba32 = std::move(sourceRgba32);
        return true;
    }

    ResizeRgba32Nearest(sourceRgba32, sourceWidth, sourceHeight, targetWidth, targetHeight, outRgba32);
    return true;
}

void ConvertRgba32ToRgba16(const std::vector<uint8_t>& rgba32, std::vector<uint8_t>& outRgba16) {
    outRgba16.assign((rgba32.size() / 4) * 2, 0);
    for (size_t i = 0, j = 0; i + 3 < rgba32.size(); i += 4, j += 2) {
        const uint8_t r = rgba32[i + 0];
        const uint8_t g = rgba32[i + 1];
        const uint8_t b = rgba32[i + 2];
        const uint8_t a = rgba32[i + 3];
        const uint16_t rgba16 =
            static_cast<uint16_t>(((r >> 3) << 11) | ((g >> 3) << 6) | ((b >> 3) << 1) | (a > 0 ? 1 : 0));
        outRgba16[j + 0] = static_cast<uint8_t>((rgba16 >> 8) & 0xFF);
        outRgba16[j + 1] = static_cast<uint8_t>(rgba16 & 0xFF);
    }
}

void ConvertRgba32ToI8(const std::vector<uint8_t>& rgba32, std::vector<uint8_t>& outI8) {
    outI8.assign(rgba32.size() / 4, 0);
    for (size_t i = 0, j = 0; i + 3 < rgba32.size(); i += 4, ++j) {
        const uint8_t r = rgba32[i + 0];
        const uint8_t g = rgba32[i + 1];
        const uint8_t b = rgba32[i + 2];
        const uint8_t a = rgba32[i + 3];
        const uint8_t luminance =
            static_cast<uint8_t>((static_cast<uint32_t>(r) + static_cast<uint32_t>(g) + static_cast<uint32_t>(b)) / 3);
        outI8[j] = (a == 0) ? 0 : luminance;
    }
}

void ConvertRgba32ToCi8AndTlut(const std::vector<uint8_t>& rgba32, std::vector<uint8_t>& outCi8,
                               std::vector<uint8_t>& outTlutRgba16) {
    outCi8.assign(rgba32.size() / 4, 0);
    std::array<uint64_t, 256> sumR{};
    std::array<uint64_t, 256> sumG{};
    std::array<uint64_t, 256> sumB{};
    std::array<uint64_t, 256> sumA{};
    std::array<uint32_t, 256> count{};

    for (size_t i = 0, j = 0; i + 3 < rgba32.size(); i += 4, ++j) {
        const uint8_t r = rgba32[i + 0];
        const uint8_t g = rgba32[i + 1];
        const uint8_t b = rgba32[i + 2];
        const uint8_t a = rgba32[i + 3];

        const uint8_t index =
            static_cast<uint8_t>(((r >> 5) << 5) | ((g >> 5) << 2) | ((b >> 6) << 0));
        outCi8[j] = index;

        sumR[index] += r;
        sumG[index] += g;
        sumB[index] += b;
        sumA[index] += a;
        count[index]++;
    }

    outTlutRgba16.assign(256 * 2, 0);
    for (size_t i = 0; i < 256; ++i) {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 255;

        if (count[i] > 0) {
            r = static_cast<uint8_t>(sumR[i] / count[i]);
            g = static_cast<uint8_t>(sumG[i] / count[i]);
            b = static_cast<uint8_t>(sumB[i] / count[i]);
            a = static_cast<uint8_t>(sumA[i] / count[i]);
        } else {
            const uint8_t rBucket = static_cast<uint8_t>((i >> 5) & 0x7);
            const uint8_t gBucket = static_cast<uint8_t>((i >> 2) & 0x7);
            const uint8_t bBucket = static_cast<uint8_t>(i & 0x3);
            r = static_cast<uint8_t>((rBucket * 255) / 7);
            g = static_cast<uint8_t>((gBucket * 255) / 7);
            b = static_cast<uint8_t>((bBucket * 255) / 3);
            a = 255;
        }

        const uint16_t rgba16 =
            static_cast<uint16_t>(((r >> 3) << 11) | ((g >> 3) << 6) | ((b >> 3) << 1) | (a > 0 ? 1 : 0));
        outTlutRgba16[i * 2 + 0] = static_cast<uint8_t>((rgba16 >> 8) & 0xFF);
        outTlutRgba16[i * 2 + 1] = static_cast<uint8_t>(rgba16 & 0xFF);
    }
}

bool HasHookshotTextureAssetOverrides(const ExternalModItemDefinition& definition) {
    return !definition.hookshotMetalTextureAsset.empty() || !definition.hookshotHandleTextureAsset.empty() ||
           !definition.hookshotDesignTextureAsset.empty() || !definition.hookshotChainTextureAsset.empty() ||
           !definition.hookshotReticleTextureAsset.empty();
}

bool HasCustomGetItemModel(const ExternalModItemDefinition& definition) {
    return !definition.customModelTriangles.empty() || !definition.modelDisplayList.empty();
}

bool HasHookshotTextureDataOverrides(const ExternalModItemDefinition& definition) {
    return !definition.hookshotMetalTextureRgba16.empty() || !definition.hookshotHandleTextureCi8.empty() ||
           !definition.hookshotDesignTextureCi8.empty() || !definition.hookshotChainTextureRgba16.empty() ||
           !definition.hookshotReticleTextureI8.empty();
}

std::string TrimWhitespace(const std::string& value) {
    size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }

    size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(start, end - start);
}

bool TryExtractObjMaterialLibraryPath(const std::string& objContent, std::string& outMtlPath) {
    outMtlPath.clear();

    std::istringstream stream(objContent);
    std::string line;
    while (std::getline(stream, line)) {
        const auto commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        line = TrimWhitespace(line);
        if (line.empty()) {
            continue;
        }

        if (line.rfind("mtllib ", 0) == 0) {
            outMtlPath = TrimWhitespace(line.substr(7));
            return !outMtlPath.empty();
        }
    }

    return false;
}

bool TryExtractMtlDiffuseTexturePath(const std::string& mtlContent, std::string& outTexturePath) {
    outTexturePath.clear();

    std::istringstream stream(mtlContent);
    std::string line;
    while (std::getline(stream, line)) {
        const auto commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        line = TrimWhitespace(line);
        if (line.empty()) {
            continue;
        }

        const auto lowerLine = ToLower(line);
        if (lowerLine.rfind("map_kd ", 0) != 0) {
            continue;
        }

        const auto value = TrimWhitespace(line.substr(7));
        if (value.empty()) {
            continue;
        }

        // Basic MTL parser: prefer quoted path if present; fallback to last token.
        const auto quoteStart = value.find('"');
        if (quoteStart != std::string::npos) {
            const auto quoteEnd = value.find('"', quoteStart + 1);
            if (quoteEnd != std::string::npos && quoteEnd > quoteStart + 1) {
                outTexturePath = value.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
                return true;
            }
        }

        std::istringstream valueStream(value);
        std::string token;
        std::string lastToken;
        while (valueStream >> token) {
            lastToken = token;
        }
        if (!lastToken.empty()) {
            outTexturePath = lastToken;
            return true;
        }
    }

    return false;
}

bool TryParseObjIndex(const std::string& value, size_t count, int32_t& outIndex) {
    if (value.empty()) {
        return false;
    }

    try {
        const auto raw = std::stoll(value);
        if (raw == 0) {
            return false;
        }

        int64_t resolved = 0;
        if (raw > 0) {
            resolved = raw - 1;
        } else {
            resolved = static_cast<int64_t>(count) + raw;
        }

        if (resolved < 0 || resolved >= static_cast<int64_t>(count)) {
            return false;
        }

        outIndex = static_cast<int32_t>(resolved);
        return true;
    } catch (...) {
        return false;
    }
}

bool TryParseObjFaceVertexToken(const std::string& token, size_t positionCount, size_t texCoordCount,
                                int32_t& outPositionIndex, int32_t& outTexCoordIndex, std::string& outError) {
    outPositionIndex = -1;
    outTexCoordIndex = -1;

    std::vector<std::string> parts;
    std::string current;
    for (char c : token) {
        if (c == '/') {
            parts.push_back(current);
            current.clear();
            continue;
        }
        current.push_back(c);
    }
    parts.push_back(current);

    if (parts.empty()) {
        outError = "empty face vertex token";
        return false;
    }

    if (!TryParseObjIndex(parts[0], positionCount, outPositionIndex)) {
        outError = "invalid vertex index: " + token;
        return false;
    }

    if (parts.size() >= 2 && !parts[1].empty()) {
        if (!TryParseObjIndex(parts[1], texCoordCount, outTexCoordIndex)) {
            outError = "invalid texcoord index: " + token;
            return false;
        }
    }

    return true;
}

const char* GetModelUvOriginName(ExternalModModelUvOrigin origin) {
    switch (origin) {
        case ExternalModModelUvOrigin::Auto:
            return "auto";
        case ExternalModModelUvOrigin::BottomLeft:
            return "bottom_left";
        case ExternalModModelUvOrigin::TopLeft:
            return "top_left";
        default:
            return "bottom_left";
    }
}

const char* GetModelTextureFilterName(ExternalModModelTextureFilter filter) {
    switch (filter) {
        case ExternalModModelTextureFilter::Auto:
            return "auto";
        case ExternalModModelTextureFilter::Point:
            return "point";
        case ExternalModModelTextureFilter::Bilerp:
            return "bilerp";
        default:
            return "auto";
    }
}

ExternalModModelTextureFilter ResolveModelTextureFilter(ExternalModModelTextureFilter configuredFilter,
                                                        bool textureHasTransparency) {
    if (configuredFilter == ExternalModModelTextureFilter::Auto) {
        return textureHasTransparency ? ExternalModModelTextureFilter::Point : ExternalModModelTextureFilter::Bilerp;
    }
    return configuredFilter;
}

bool TextureHasMixedAlphaCoverage(const std::vector<uint8_t>& rgba32, int32_t width, int32_t height) {
    if (width <= 0 || height <= 0) {
        return false;
    }

    const size_t pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);
    if (rgba32.size() < pixelCount * 4) {
        return false;
    }

    bool sawTransparent = false;
    bool sawOpaque = false;
    for (size_t i = 0; i < pixelCount; ++i) {
        const uint8_t alpha = rgba32[i * 4 + 3];
        sawTransparent = sawTransparent || alpha == 0;
        sawOpaque = sawOpaque || alpha > 0;
        if (sawTransparent && sawOpaque) {
            return true;
        }
    }

    return false;
}

bool HasUsefulAlphaCoverageForUvHeuristic(const std::vector<uint8_t>* rgba32, int32_t width, int32_t height) {
    if (rgba32 == nullptr || width <= 0 || height <= 0) {
        return false;
    }

    const size_t pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);
    if (rgba32->size() < pixelCount * 4) {
        return false;
    }

    bool sawTransparent = false;
    bool sawOpaque = false;
    for (size_t i = 0; i < pixelCount; ++i) {
        const uint8_t alpha = (*rgba32)[i * 4 + 3];
        sawTransparent = sawTransparent || alpha == 0;
        sawOpaque = sawOpaque || alpha > 0;
        if (sawTransparent && sawOpaque) {
            return true;
        }
    }

    return false;
}

uint8_t SampleAlphaForUv(const std::vector<uint8_t>& rgba32, int32_t width, int32_t height, float u, float v,
                         bool invertV) {
    if (width <= 0 || height <= 0) {
        return 0;
    }

    const float clampedU = std::clamp(u, 0.0f, 1.0f);
    const float clampedV = std::clamp(v, 0.0f, 1.0f);
    const float orientedV = invertV ? (1.0f - clampedV) : clampedV;

    const int32_t maxX = std::max(width - 1, 0);
    const int32_t maxY = std::max(height - 1, 0);
    const int32_t x = std::clamp(static_cast<int32_t>(std::lround(clampedU * static_cast<float>(maxX))), 0, maxX);
    const int32_t y = std::clamp(static_cast<int32_t>(std::lround(orientedV * static_cast<float>(maxY))), 0, maxY);

    const size_t pixelIndex = static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x);
    const size_t alphaOffset = pixelIndex * 4 + 3;
    if (alphaOffset >= rgba32.size()) {
        return 0;
    }
    return rgba32[alphaOffset];
}

bool IsAlphaEdgeForUv(const std::vector<uint8_t>& rgba32, int32_t width, int32_t height, float u, float v, bool invertV) {
    if (width <= 0 || height <= 0) {
        return false;
    }

    const float clampedU = std::clamp(u, 0.0f, 1.0f);
    const float clampedV = std::clamp(v, 0.0f, 1.0f);
    const float orientedV = invertV ? (1.0f - clampedV) : clampedV;

    const int32_t maxX = std::max(width - 1, 0);
    const int32_t maxY = std::max(height - 1, 0);
    const int32_t x = std::clamp(static_cast<int32_t>(std::lround(clampedU * static_cast<float>(maxX))), 0, maxX);
    const int32_t y = std::clamp(static_cast<int32_t>(std::lround(orientedV * static_cast<float>(maxY))), 0, maxY);

    const auto getAlphaAt = [&](int32_t sampleX, int32_t sampleY) {
        if (sampleX < 0 || sampleX >= width || sampleY < 0 || sampleY >= height) {
            return static_cast<uint8_t>(0);
        }
        const size_t pixelIndex = static_cast<size_t>(sampleY) * static_cast<size_t>(width) + static_cast<size_t>(sampleX);
        const size_t alphaOffset = pixelIndex * 4 + 3;
        if (alphaOffset >= rgba32.size()) {
            return static_cast<uint8_t>(0);
        }
        return rgba32[alphaOffset];
    };

    if (getAlphaAt(x, y) == 0) {
        return false;
    }

    return getAlphaAt(x - 1, y) == 0 || getAlphaAt(x + 1, y) == 0 || getAlphaAt(x, y - 1) == 0 ||
           getAlphaAt(x, y + 1) == 0;
}

bool TryParseObjCustomModel(const std::string& objContent, float modelScale, int32_t textureWidth,
                            int32_t textureHeight, ExternalModModelUvOrigin modelUvOrigin,
                            const std::vector<uint8_t>* uvHeuristicTextureRgba32, const std::string& modId,
                            const std::string& itemId,
                            std::vector<ExternalModItemDefinition::CustomModelTriangle>& outTriangles,
                            std::string& outError) {
    outTriangles.clear();

    struct TempPos {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };
    struct TempUv {
        float u = 0.0f;
        float v = 0.0f;
    };
    struct TempFaceVertex {
        int32_t pos = -1;
        int32_t uv = -1;
    };

    std::vector<TempPos> positions;
    std::vector<TempUv> texCoords;
    std::vector<std::array<TempFaceVertex, 3>> triangles;

    std::istringstream stream(objContent);
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(stream, line)) {
        ++lineNumber;

        const auto commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        line = TrimWhitespace(line);
        if (line.empty()) {
            continue;
        }

        if (line.rfind("v ", 0) == 0) {
            std::istringstream lineStream(line.substr(2));
            TempPos pos;
            if (!(lineStream >> pos.x >> pos.y >> pos.z)) {
                outError = "OBJ parse error line " + std::to_string(lineNumber) + ": invalid vertex format";
                return false;
            }
            positions.push_back(pos);
            continue;
        }

        if (line.rfind("vt ", 0) == 0) {
            std::istringstream lineStream(line.substr(3));
            TempUv uv;
            if (!(lineStream >> uv.u >> uv.v)) {
                outError = "OBJ parse error line " + std::to_string(lineNumber) + ": invalid texcoord format";
                return false;
            }
            texCoords.push_back(uv);
            continue;
        }

        if (line.rfind("f ", 0) == 0) {
            std::istringstream lineStream(line.substr(2));
            std::vector<TempFaceVertex> faceVertices;
            std::string token;
            while (lineStream >> token) {
                TempFaceVertex faceVertex;
                std::string parseError;
                if (!TryParseObjFaceVertexToken(token, positions.size(), texCoords.size(), faceVertex.pos, faceVertex.uv,
                                                parseError)) {
                    outError =
                        "OBJ parse error line " + std::to_string(lineNumber) + ": " + parseError;
                    return false;
                }
                faceVertices.push_back(faceVertex);
            }

            if (faceVertices.size() < 3) {
                outError = "OBJ parse error line " + std::to_string(lineNumber) + ": face must have at least 3 vertices";
                return false;
            }

            for (size_t i = 1; i + 1 < faceVertices.size(); ++i) {
                if (triangles.size() >= kMaxItemModelTriangles) {
                    outError = "OBJ exceeds max triangle count (" + std::to_string(kMaxItemModelTriangles) + ")";
                    return false;
                }
                triangles.push_back({ faceVertices[0], faceVertices[i], faceVertices[i + 1] });
            }
        }
    }

    if (positions.empty()) {
        outError = "OBJ has no vertices";
        return false;
    }

    if (triangles.empty()) {
        outError = "OBJ has no faces";
        return false;
    }

    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();
    float maxZ = std::numeric_limits<float>::lowest();

    for (const auto& pos : positions) {
        minX = std::min(minX, pos.x);
        minY = std::min(minY, pos.y);
        minZ = std::min(minZ, pos.z);
        maxX = std::max(maxX, pos.x);
        maxY = std::max(maxY, pos.y);
        maxZ = std::max(maxZ, pos.z);
    }

    const float extentX = maxX - minX;
    const float extentY = maxY - minY;
    const float extentZ = maxZ - minZ;
    const float maxExtent = std::max(extentX, std::max(extentY, extentZ));
    if (maxExtent <= std::numeric_limits<float>::epsilon()) {
        outError = "OBJ has invalid bounds (zero size)";
        return false;
    }

    const float centerX = (minX + maxX) * 0.5f;
    const float centerY = (minY + maxY) * 0.5f;
    const float centerZ = (minZ + maxZ) * 0.5f;
    const float clampedModelScale = std::clamp(modelScale, 0.05f, 20.0f);
    const float normalizationScale = (2000.0f / maxExtent) * clampedModelScale;
    const int32_t effectiveTextureWidth = std::max(textureWidth, 1);
    const int32_t effectiveTextureHeight = std::max(textureHeight, 1);
    const int32_t effectiveTextureWidthForUv = std::max(effectiveTextureWidth - 1, 0);
    const int32_t effectiveTextureHeightForUv = std::max(effectiveTextureHeight - 1, 0);
    const float uvScaleS = static_cast<float>(effectiveTextureWidthForUv << 5);
    const float uvScaleT = static_cast<float>(effectiveTextureHeightForUv << 5);
    ExternalModModelUvOrigin resolvedUvOrigin = modelUvOrigin;
    uint64_t topLeftScore = 0;
    uint64_t bottomLeftScore = 0;
    size_t uvSampleCount = 0;
    const bool canUseHeuristic =
        HasUsefulAlphaCoverageForUvHeuristic(uvHeuristicTextureRgba32, effectiveTextureWidth, effectiveTextureHeight);
    if (canUseHeuristic && uvHeuristicTextureRgba32 != nullptr) {
        for (const auto& tri : triangles) {
            for (size_t i = 0; i < 3; ++i) {
                const auto& faceVertex = tri[i];
                if (faceVertex.uv < 0 || faceVertex.uv >= static_cast<int32_t>(texCoords.size())) {
                    continue;
                }

                const float u = texCoords[faceVertex.uv].u;
                const float v = texCoords[faceVertex.uv].v;
                topLeftScore +=
                    SampleAlphaForUv(*uvHeuristicTextureRgba32, effectiveTextureWidth, effectiveTextureHeight, u, v, false);
                bottomLeftScore +=
                    SampleAlphaForUv(*uvHeuristicTextureRgba32, effectiveTextureWidth, effectiveTextureHeight, u, v, true);
                ++uvSampleCount;
            }
        }
    }

    const bool hasHeuristicScores = uvSampleCount > 0;
    if (resolvedUvOrigin == ExternalModModelUvOrigin::Auto) {
        if (hasHeuristicScores && topLeftScore != bottomLeftScore) {
            resolvedUvOrigin =
                topLeftScore > bottomLeftScore ? ExternalModModelUvOrigin::TopLeft : ExternalModModelUvOrigin::BottomLeft;
        } else {
            resolvedUvOrigin = ExternalModModelUvOrigin::BottomLeft;
        }

        SPDLOG_INFO("[ExternalMods] OBJ UV orientation for {}.{} resolved to {} (auto score top_left={}, "
                    "bottom_left={}, samples={})",
                    modId, itemId, GetModelUvOriginName(resolvedUvOrigin), topLeftScore, bottomLeftScore, uvSampleCount);
    } else if (hasHeuristicScores) {
        const uint64_t configuredScore =
            resolvedUvOrigin == ExternalModModelUvOrigin::TopLeft ? topLeftScore : bottomLeftScore;
        const uint64_t oppositeScore =
            resolvedUvOrigin == ExternalModModelUvOrigin::TopLeft ? bottomLeftScore : topLeftScore;
        const ExternalModModelUvOrigin recommendedOrigin =
            topLeftScore >= bottomLeftScore ? ExternalModModelUvOrigin::TopLeft : ExternalModModelUvOrigin::BottomLeft;

        // Warn only when explicit orientation is substantially worse than the opposite score.
        if (oppositeScore > 0 && uvSampleCount >= 16 &&
            static_cast<double>(configuredScore) < static_cast<double>(oppositeScore) * 0.6) {
            SPDLOG_WARN(
                "[ExternalMods] OBJ UV orientation for {}.{} explicitly set to {} but heuristic score favors {} "
                "(configured={}, opposite={}, samples={}); consider modelUvOrigin=\"{}\" or \"auto\"",
                modId, itemId, GetModelUvOriginName(resolvedUvOrigin), GetModelUvOriginName(recommendedOrigin),
                configuredScore, oppositeScore, uvSampleCount, GetModelUvOriginName(recommendedOrigin));
        }
    }

    size_t uvOpaqueHits = 0;
    size_t uvTransparentHits = 0;
    size_t uvEdgeHits = 0;
    if (canUseHeuristic && uvHeuristicTextureRgba32 != nullptr) {
        const bool invertV = resolvedUvOrigin != ExternalModModelUvOrigin::TopLeft;
        for (const auto& tri : triangles) {
            for (size_t i = 0; i < 3; ++i) {
                const auto& faceVertex = tri[i];
                if (faceVertex.uv < 0 || faceVertex.uv >= static_cast<int32_t>(texCoords.size())) {
                    continue;
                }

                const float u = texCoords[faceVertex.uv].u;
                const float v = texCoords[faceVertex.uv].v;
                const uint8_t alpha = SampleAlphaForUv(*uvHeuristicTextureRgba32, effectiveTextureWidth,
                                                       effectiveTextureHeight, u, v, invertV);
                if (alpha > 0) {
                    ++uvOpaqueHits;
                    if (IsAlphaEdgeForUv(*uvHeuristicTextureRgba32, effectiveTextureWidth, effectiveTextureHeight, u, v,
                                         invertV)) {
                        ++uvEdgeHits;
                    }
                } else {
                    ++uvTransparentHits;
                }
            }
        }
    }
    const size_t uvCoverageSamples = uvOpaqueHits + uvTransparentHits;
    if (uvCoverageSamples > 0) {
        const double uvTransparentRate = static_cast<double>(uvTransparentHits) / static_cast<double>(uvCoverageSamples);
        const double uvEdgeRate = static_cast<double>(uvEdgeHits) / static_cast<double>(uvCoverageSamples);
        SPDLOG_INFO(
            "[ExternalMods] OBJ UV coverage for {}.{} using {}: opaqueHits={}, transparentHits={}, edgeHits={}, "
            "transparentRate={:.3f}, edgeRate={:.3f}, samples={}",
            modId, itemId, GetModelUvOriginName(resolvedUvOrigin), uvOpaqueHits, uvTransparentHits, uvEdgeHits,
            uvTransparentRate, uvEdgeRate, uvCoverageSamples);
        if (uvTransparentRate >= 0.10) {
            SPDLOG_WARN(
                "[ExternalMods] OBJ UV coverage indicates atlas transparency mismatch for {}.{} (transparentRate={:.3f}, "
                "samples={}); use an opaque atlas (alpha=255) and verify UV islands",
                modId, itemId, uvTransparentRate, uvCoverageSamples);
        }
    }

    outTriangles.reserve(triangles.size());
    for (const auto& tri : triangles) {
        ExternalModItemDefinition::CustomModelTriangle convertedTriangle;
        for (size_t i = 0; i < 3; ++i) {
            const auto& faceVertex = tri[i];
            const auto& sourcePos = positions[faceVertex.pos];

            const auto toS16 = [](float value) {
                return static_cast<int16_t>(std::clamp<int32_t>(static_cast<int32_t>(std::lround(value)),
                                                                std::numeric_limits<int16_t>::min(),
                                                                std::numeric_limits<int16_t>::max()));
            };

            convertedTriangle.vertices[i].x = toS16((sourcePos.x - centerX) * normalizationScale);
            convertedTriangle.vertices[i].y = toS16((sourcePos.y - centerY) * normalizationScale);
            convertedTriangle.vertices[i].z = toS16((sourcePos.z - centerZ) * normalizationScale);

            float u = 0.0f;
            float v = 0.0f;
            if (faceVertex.uv >= 0 && faceVertex.uv < static_cast<int32_t>(texCoords.size())) {
                u = texCoords[faceVertex.uv].u;
                v = texCoords[faceVertex.uv].v;
            }
            u = std::clamp(u, 0.0f, 1.0f);
            v = std::clamp(v, 0.0f, 1.0f);
            convertedTriangle.vertices[i].s = toS16(u * uvScaleS);
            const float orientedV = resolvedUvOrigin == ExternalModModelUvOrigin::TopLeft ? v : (1.0f - v);
            convertedTriangle.vertices[i].t = toS16(orientedV * uvScaleT);
        }
        outTriangles.push_back(convertedTriangle);
    }

    return true;
}

const ExternalModItemDefinition* FindCustomModelDefinitionForItem(const std::vector<ExternalModPackage>& packages,
                                                                  int32_t itemId) {
    const ExternalModItemDefinition* selectedDefinition = nullptr;
    int32_t selectedLoadOrder = std::numeric_limits<int32_t>::max();
    std::string selectedModId;

    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled) {
            continue;
        }

        for (const auto& definition : package.runtime.itemDefinitions) {
            if (!HasCustomGetItemModel(definition)) {
                continue;
            }
            if (!ItemDefinitionMatchesUseItem(definition, itemId)) {
                continue;
            }

            if (selectedDefinition == nullptr || package.manifest.loadOrder < selectedLoadOrder ||
                (package.manifest.loadOrder == selectedLoadOrder && package.manifest.id < selectedModId)) {
                selectedDefinition = &definition;
                selectedLoadOrder = package.manifest.loadOrder;
                selectedModId = package.manifest.id;
            }
        }
    }

    return selectedDefinition;
}

bool DrawCustomItemDefinitionModel(PlayState* play, const ExternalModItemDefinition& definition, bool loadModelViewMatrix) {
    if (play == nullptr) {
        return false;
    }

    GraphicsContext* __gfxCtx = play->state.gfxCtx;
    (void)__gfxCtx;

    bool drewCustomModel = false;
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    if (loadModelViewMatrix) {
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);
    }

    if (!definition.customModelTriangles.empty()) {
        gSPClearGeometryMode(POLY_OPA_DISP++, G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR);

        if (!definition.modelTextureRgba32.empty()) {
            gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
            gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_NONE);
            const ExternalModModelTextureFilter resolvedFilter =
                ResolveModelTextureFilter(definition.modelTextureFilter, definition.modelTextureHasTransparency);
            gDPSetTextureFilter(POLY_OPA_DISP++,
                               resolvedFilter == ExternalModModelTextureFilter::Point ? G_TF_POINT : G_TF_BILERP);
            gSPTexture(POLY_OPA_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
        } else {
            gSPTexture(POLY_OPA_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
            gDPSetCombineMode(POLY_OPA_DISP++, G_CC_SHADE, G_CC_SHADE);
        }

        bool drewAnyTriangle = false;
        if (!definition.modelTextureRgba32.empty()) {
            const int32_t textureWidth = std::max(definition.modelTextureWidth, 1);
            const int32_t textureHeight = std::max(definition.modelTextureHeight, 1);
            gDPLoadTextureTile(POLY_OPA_DISP++, definition.modelTextureRgba32.data(), G_IM_FMT_RGBA, G_IM_SIZ_16b, textureWidth,
                               textureHeight, 0, 0, textureWidth - 1, textureHeight - 1, 0,
                               G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                               G_TX_NOLOD, G_TX_NOLOD);
        }

        constexpr size_t kTrianglesPerBatch = 10;
        for (size_t triangleStart = 0; triangleStart < definition.customModelTriangles.size();
             triangleStart += kTrianglesPerBatch) {
            const size_t triangleCount = std::min(kTrianglesPerBatch, definition.customModelTriangles.size() - triangleStart);
            const size_t vertexCount = triangleCount * 3;

            auto* vertices = static_cast<Vtx*>(Graph_Alloc(play->state.gfxCtx, sizeof(Vtx) * vertexCount));
            if (vertices == nullptr) {
                break;
            }

            for (size_t i = 0; i < triangleCount; ++i) {
                const auto& triangle = definition.customModelTriangles[triangleStart + i];
                for (size_t j = 0; j < 3; ++j) {
                    const size_t index = i * 3 + j;
                    vertices[index].v.ob[0] = triangle.vertices[j].x;
                    vertices[index].v.ob[1] = triangle.vertices[j].y;
                    vertices[index].v.ob[2] = triangle.vertices[j].z;
                    vertices[index].v.flag = 0;
                    vertices[index].v.tc[0] = triangle.vertices[j].s;
                    vertices[index].v.tc[1] = triangle.vertices[j].t;
                    vertices[index].v.cn[0] = 255;
                    vertices[index].v.cn[1] = 255;
                    vertices[index].v.cn[2] = 255;
                    vertices[index].v.cn[3] = 255;
                }
            }

            gSPVertex(POLY_OPA_DISP++, reinterpret_cast<uintptr_t>(vertices), static_cast<int32_t>(vertexCount), 0);
            for (size_t i = 0; i < triangleCount; ++i) {
                const int32_t base = static_cast<int32_t>(i * 3);
                gSP1Triangle(POLY_OPA_DISP++, base + 0, base + 1, base + 2, 0);
            }
            drewAnyTriangle = true;
        }

        drewCustomModel = drewAnyTriangle;
    } else if (!definition.modelDisplayList.empty()) {
        auto* dlist = ResourceMgr_LoadGfxByName(definition.modelDisplayList.c_str());
        if (dlist != nullptr) {
            const ExternalModModelTextureFilter resolvedFilter =
                ResolveModelTextureFilter(definition.modelTextureFilter, definition.modelTextureHasTransparency);
            gDPSetTextureFilter(POLY_OPA_DISP++,
                               resolvedFilter == ExternalModModelTextureFilter::Point ? G_TF_POINT : G_TF_BILERP);
            const std::string debugKey = definition.sourceModId + "|" + definition.id + "|" + definition.modelDisplayList;
            if (gExternalModDisplayListDrawDebugLogs.insert(debugKey).second) {
                auto displayListResource = Ship::Context::GetInstance()->GetResourceManager()->LoadResource(
                    definition.modelDisplayList.c_str());
                if (displayListResource != nullptr && displayListResource->GetInitData() != nullptr &&
                    displayListResource->GetInitData()->Type == static_cast<uint32_t>(Fast::ResourceType::DisplayList)) {
                    auto displayList = std::static_pointer_cast<Fast::DisplayList>(displayListResource);
                    const size_t instructionCount = displayList != nullptr ? displayList->Instructions.size() : 0;
                    SPDLOG_INFO(
                        "[ExternalMods] Drawing model display list for {}.{}: path={} instructions={} ptr={} "
                        "custom={} modelScale={} filterConfigured={} filterEffective={}",
                        definition.sourceModId.empty() ? "<unknown>" : definition.sourceModId,
                        definition.id.empty() ? "<unknown>" : definition.id,
                        definition.modelDisplayList.empty() ? "<empty>" : definition.modelDisplayList,
                        instructionCount, static_cast<void*>(dlist),
                        displayListResource->GetInitData()->IsCustom ? 1 : 0, definition.modelScale,
                        GetModelTextureFilterName(definition.modelTextureFilter),
                        GetModelTextureFilterName(resolvedFilter));
                } else {
                    SPDLOG_WARN(
                        "[ExternalMods] Model display list pointer resolved but metadata lookup failed for {}.{}: "
                        "path={}",
                        definition.sourceModId.empty() ? "<unknown>" : definition.sourceModId,
                        definition.id.empty() ? "<unknown>" : definition.id,
                        definition.modelDisplayList.empty() ? "<empty>" : definition.modelDisplayList);
                }
            }
            const float clampedScale =
                std::clamp(definition.modelScale, kMinDisplayListModelScale, kMaxDisplayListModelScale);
            if (std::abs(clampedScale - 1.0f) > 0.0001f) {
                Matrix_Push();
                Matrix_Scale(clampedScale, clampedScale, clampedScale, MTXMODE_APPLY);
                gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_OPA_DISP++, dlist);
                Matrix_Pop();
            } else {
                gSPDisplayList(POLY_OPA_DISP++, dlist);
            }
            drewCustomModel = true;
        } else {
            const std::string warningKey = definition.sourceModId + "|" + definition.id + "|" + definition.modelDisplayList;
            if (gExternalModMissingDisplayListWarnings.insert(warningKey).second) {
                SPDLOG_WARN("[ExternalMods] Missing model display list for {}.{}: {}", definition.sourceModId.empty() ? "<unknown>" : definition.sourceModId,
                            definition.id.empty() ? "<unknown>" : definition.id,
                            definition.modelDisplayList.empty() ? "<empty>" : definition.modelDisplayList);
            }
        }
    }

    return drewCustomModel;
}

extern "C" void ExternalMods_DrawCustomGetItemModel(PlayState* play, GetItemEntry* getItemEntry) {
    if (play == nullptr || getItemEntry == nullptr) {
        return;
    }

    const auto* definition =
        FindCustomModelDefinitionForItem(ExternalModManager::Instance().GetPackages(), getItemEntry->itemId);
    if (definition == nullptr) {
        GetItem_Draw(play, getItemEntry->gid);
        return;
    }

    if (DrawCustomItemDefinitionModel(play, *definition, true)) {
        return;
    }

    GetItem_Draw(play, getItemEntry->gid);
}

extern "C" int32_t ExternalMods_DrawCustomEquippedStickModel(PlayState* play) {
    if (play == nullptr) {
        return 0;
    }

    const auto* definition = FindCustomModelDefinitionForItem(ExternalModManager::Instance().GetPackages(), ITEM_STICK);
    if (definition == nullptr) {
        return 0;
    }

    float stickLengthScale = 1.0f;
    Player* player = GET_PLAYER(play);
    if (player != nullptr && std::isfinite(player->unk_85C) && std::abs(player->unk_85C) > 0.0001f) {
        stickLengthScale = player->unk_85C;
    }

    const bool undoStickLengthScale = std::abs(stickLengthScale - 1.0f) > 0.0001f;
    if (undoStickLengthScale) {
        Matrix_Push();
        Matrix_Scale(1.0f, 1.0f / stickLengthScale, 1.0f, MTXMODE_APPLY);
    }

    const bool drew = DrawCustomItemDefinitionModel(play, *definition, true);

    if (undoStickLengthScale) {
        Matrix_Pop();
    }

    return drew ? 1 : 0;
}

void ApplyModItemIconOverrides(const std::vector<ExternalModPackage>& packages) {
    CaptureVanillaItemIconsIfNeeded();
    RestoreVanillaItemIcons();

    std::vector<const ExternalModPackage*> orderedPackages;
    orderedPackages.reserve(packages.size());

    for (const auto& package : packages) {
        if (package.runtime.enabled) {
            orderedPackages.push_back(&package);
        }
    }

    std::stable_sort(orderedPackages.begin(), orderedPackages.end(),
                     [](const ExternalModPackage* lhs, const ExternalModPackage* rhs) {
                         if (lhs->manifest.loadOrder != rhs->manifest.loadOrder) {
                             return lhs->manifest.loadOrder < rhs->manifest.loadOrder;
                         }
                         return lhs->manifest.id < rhs->manifest.id;
                     });

    for (const auto* package : orderedPackages) {
        for (const auto& item : package->runtime.itemDefinitions) {
            ApplyIconOverrideForDefinition(item);
        }
    }
}

void RestoreModHookshotTextureOverrides() {
    for (const auto& patch : gExternalModHookshotTexturePatches) {
        ResourceMgr_UnpatchGfxByName(patch.displayListPath.c_str(), patch.patchName.c_str());
    }
    gExternalModHookshotTexturePatches.clear();
    gExternalModHookshotTextureOverrideKey.clear();
}

struct HookshotDisplayListTarget {
    const char* displayListPath;
    const char* id;
};

constexpr std::array<HookshotDisplayListTarget, 5> kHookshotDisplayListTargets = {{
    { gLinkAdultRightHandHoldingHookshotNearDL, "near" },
    { gLinkAdultRightHandHoldingHookshotFarDL, "far" },
    { gLinkAdultHookshotChainDL, "chain" },
    { gLinkAdultHookshotTipDL, "tip" },
    { gLinkAdultHookshotReticleDL, "reticle" },
}};

bool PatchHookshotTextureInDisplayList(const HookshotDisplayListTarget& target, const char* sourceTexturePath,
                                       const void* replacementData, const std::string& patchPrefix) {
    if (target.displayListPath == nullptr || sourceTexturePath == nullptr || replacementData == nullptr) {
        return false;
    }

    auto resource = std::static_pointer_cast<Fast::DisplayList>(
        Ship::Context::GetInstance()->GetResourceManager()->LoadResource(target.displayListPath));
    if (resource == nullptr) {
        return false;
    }

    const uintptr_t sourcePointer = reinterpret_cast<uintptr_t>(sourceTexturePath);
    bool patchedAtLeastOne = false;

    for (size_t instructionIndex = 0; instructionIndex < resource->Instructions.size(); ++instructionIndex) {
        const auto& instruction = resource->Instructions[instructionIndex];
        const uint8_t command = static_cast<uint8_t>(_SHIFTR(instruction.words.w0, 24, 8));
        if (command != G_SETTIMG_OTR_FILEPATH) {
            continue;
        }

        if (instruction.words.w1 != sourcePointer) {
            continue;
        }

        const int32_t format = _SHIFTR(instruction.words.w0, 21, 3);
        const int32_t size = _SHIFTR(instruction.words.w0, 19, 2);
        const int32_t width = _SHIFTR(instruction.words.w0, 0, 12) + 1;
        const Gfx patchedInstruction = gsSetImage(G_SETTIMG, format, size, width, replacementData);
        const auto patchName = patchPrefix + "_" + target.id + "_" + std::to_string(instructionIndex);

        ResourceMgr_PatchCustomGfxByName(target.displayListPath, patchName.c_str(), static_cast<int>(instructionIndex),
                                         patchedInstruction);
        gExternalModHookshotTexturePatches.push_back({ target.displayListPath, patchName });
        patchedAtLeastOne = true;
    }

    return patchedAtLeastOne;
}

void ApplyModHookshotTextureOverrides(const std::vector<ExternalModPackage>& packages) {
    const ExternalModItemDefinition* selectedDefinition = nullptr;
    const ExternalModPackage* selectedPackage = nullptr;
    int32_t selectedLoadOrder = std::numeric_limits<int32_t>::max();
    std::string selectedModId;

    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled) {
            continue;
        }

        for (const auto& definition : package.runtime.itemDefinitions) {
            if (!definition.granted || definition.slot != ExternalModItemSlot::Hookshot ||
                !HasHookshotTextureDataOverrides(definition)) {
                continue;
            }

            if (selectedDefinition == nullptr || package.manifest.loadOrder < selectedLoadOrder ||
                (package.manifest.loadOrder == selectedLoadOrder && package.manifest.id < selectedModId)) {
                selectedDefinition = &definition;
                selectedPackage = &package;
                selectedLoadOrder = package.manifest.loadOrder;
                selectedModId = package.manifest.id;
            }
        }
    }

    if (selectedDefinition == nullptr || selectedPackage == nullptr) {
        if (!gExternalModHookshotTexturePatches.empty()) {
            RestoreModHookshotTextureOverrides();
        }
        return;
    }

    const std::string selectedKey = selectedPackage->manifest.id + ":" + selectedDefinition->id;
    if (!gExternalModHookshotTexturePatches.empty() && gExternalModHookshotTextureOverrideKey == selectedKey) {
        return;
    }

    RestoreModHookshotTextureOverrides();

    const auto patchPrefix = "ExternalMods_HookshotTexture_" + SanitizeCVarSegment(selectedKey);
    bool patchedAny = false;
    auto applyTextureOverride = [&](const char* sourcePath, const std::vector<uint8_t>& data) {
        if (sourcePath == nullptr || data.empty()) {
            return;
        }
        for (const auto& target : kHookshotDisplayListTargets) {
            patchedAny |= PatchHookshotTextureInDisplayList(target, sourcePath, data.data(), patchPrefix);
        }
    };

    applyTextureOverride(gLinkAdultHookshotMetalTex, selectedDefinition->hookshotMetalTextureRgba16);
    applyTextureOverride(gLinkAdultHookshotHandleTex, selectedDefinition->hookshotHandleTextureCi8);
    applyTextureOverride(object_link_boyTLUT_00CD48, selectedDefinition->hookshotHandleTextureTlutRgba16);
    applyTextureOverride(gLinkAdultHookshotDesignTex, selectedDefinition->hookshotDesignTextureCi8);
    applyTextureOverride(object_link_boyTLUT_00CB40, selectedDefinition->hookshotDesignTextureTlutRgba16);
    applyTextureOverride(gLinkAdultHookshotChainTex, selectedDefinition->hookshotChainTextureRgba16);
    applyTextureOverride(gLinkAdultHookshotReticleTex, selectedDefinition->hookshotReticleTextureI8);

    if (patchedAny) {
        gExternalModHookshotTextureOverrideKey = selectedKey;
        SPDLOG_INFO("[ExternalMods] Applied hookshot gameplay texture override from {} ({})", selectedPackage->manifest.id,
                    selectedDefinition->id);
    } else {
        RestoreModHookshotTextureOverrides();
        SPDLOG_WARN("[ExternalMods] Hookshot texture override from {} ({}) had no patchable commands",
                    selectedPackage->manifest.id, selectedDefinition->id);
    }
}

bool HasGrantedModItemForSlotByAgePolicy(const std::vector<ExternalModPackage>& packages, ExternalModItemSlot slot,
                                         ExternalModItemAgePolicy policy) {
    for (const auto& package : packages) {
        if (!package.runtime.enabled) {
            continue;
        }

        for (const auto& item : package.runtime.itemDefinitions) {
            if (item.granted && item.slot == slot && item.agePolicy == policy) {
                return true;
            }
        }
    }

    return false;
}

void ApplyModItemAgeRequirementOverrides(const std::vector<ExternalModPackage>& packages) {
    for (const auto& config : kSupportedItemSlotConfigs) {
        const bool allowChild =
            HasGrantedModItemForSlotByAgePolicy(packages, config.slot, ExternalModItemAgePolicy::AllowChild);
        const bool allowAdult =
            HasGrantedModItemForSlotByAgePolicy(packages, config.slot, ExternalModItemAgePolicy::AllowAdult);

        u8 ageRequirement = config.vanillaAgeReq;
        if ((config.vanillaAgeReq == kAgeReqAdult && allowChild) ||
            (config.vanillaAgeReq == kAgeReqChild && allowAdult)) {
            ageRequirement = kAgeReqNone;
        }

        gSlotAgeReqs[config.slotIndex] = ageRequirement;

        if (config.grantItemId != ITEM_NONE) {
            gItemAgeReqs[config.grantItemId] = ageRequirement;
        }

        for (const auto itemId : config.useItemIds) {
            if (itemId == ITEM_NONE) {
                continue;
            }
            gItemAgeReqs[itemId] = ageRequirement;
        }
    }
}

void GrantItemForDefinitionIfMissing(const ExternalModItemDefinition& definition) {
    const auto* config = FindItemSlotConfig(definition.slot);
    if (config == nullptr) {
        return;
    }

    const int32_t grantItemId = ResolveGrantedItemId(definition);
    if (grantItemId != ITEM_NONE && grantItemId >= std::numeric_limits<int8_t>::min() &&
        grantItemId <= std::numeric_limits<int8_t>::max() && gSaveContext.inventory.items[config->slotIndex] == ITEM_NONE) {
        gSaveContext.inventory.items[config->slotIndex] = static_cast<int8_t>(grantItemId);
    }

    if (definition.hasGrantAmmo) {
        const int32_t ammoItemId = ResolveGrantedAmmoItemId(definition);
        if (ammoItemId != ITEM_NONE) {
            const auto clampedAmmo = static_cast<int8_t>(std::clamp(definition.grantAmmo, 0, 99));
            if (AMMO(ammoItemId) < clampedAmmo) {
                AMMO(ammoItemId) = clampedAmmo;
            }
        }
    }
}
const ExternalModItemDefinition* FindItemDefinitionById(const ExternalModRuntime& runtime, const std::string& itemId) {
    const auto it = std::find_if(runtime.itemDefinitions.begin(), runtime.itemDefinitions.end(),
                                 [&itemId](const ExternalModItemDefinition& definition) {
                                     return definition.id == itemId;
                                 });
    if (it == runtime.itemDefinitions.end()) {
        return nullptr;
    }
    return &(*it);
}

bool IsModItemHeld(const ExternalModItemDefinition& definition, const Player* player) {
    if (player == nullptr) {
        return false;
    }

    const int32_t heldItemId = player->heldItemId;
    return heldItemId >= 0 && heldItemId < ITEM_NONE_FE && ItemDefinitionMatchesUseItem(definition, heldItemId);
}

bool IsActionItemRequirementSatisfied(const ExternalModPackage& package, const ExternalModAction& action,
                                      const Player* player) {
    if (action.itemId.empty()) {
        return true;
    }

    const auto* definition = FindItemDefinitionById(package.runtime, action.itemId);
    if (definition == nullptr || !definition->granted) {
        return false;
    }

    return IsModItemHeld(*definition, player);
}

void SpawnLanternLightAtPlayer(PlayState* play, Player* player) {
    if (play == nullptr || player == nullptr) {
        return;
    }

    Vec3f glowPos = player->actor.world.pos;
    glowPos.y += 22.0f;
    EffectSsGFire_Spawn(play, &glowPos);

    Vec3f flamePos = player->actor.world.pos;
    flamePos.y += 34.0f;
    Vec3f flameVelocity = { 0.0f, 0.2f, 0.0f };
    Vec3f flameAccel = { 0.0f, 0.02f, 0.0f };
    EffectSsDFire_SpawnFixedScale(play, &flamePos, &flameVelocity, &flameAccel, 180, 4);
}

Actor* FindIgniteTargetInFront(PlayState* play, Player* player) {
    if (play == nullptr || player == nullptr) {
        return nullptr;
    }

    constexpr float kMaxDistance = 180.0f;
    constexpr float kMaxVerticalDelta = 120.0f;
    constexpr int32_t kMaxYawDelta = 0x3000;
    const std::array<int32_t, 4> categories = { ACTORCAT_ENEMY, ACTORCAT_BOSS, ACTORCAT_PROP, ACTORCAT_BG };

    Actor* bestActor = nullptr;
    float bestScore = std::numeric_limits<float>::max();

    for (const auto category : categories) {
        for (Actor* actor = play->actorCtx.actorLists[category].head; actor != nullptr; actor = actor->next) {
            if (actor == &player->actor || actor->update == nullptr) {
                continue;
            }

            const float distance = Math_Vec3f_DistXYZ(&player->actor.world.pos, &actor->world.pos);
            if (distance > kMaxDistance) {
                continue;
            }

            const float verticalDelta = fabsf(actor->world.pos.y - player->actor.world.pos.y);
            if (verticalDelta > kMaxVerticalDelta) {
                continue;
            }

            const s16 yawToActor = Math_Vec3f_Yaw(&player->actor.world.pos, &actor->world.pos);
            const s16 yawDelta = yawToActor - player->actor.shape.rot.y;
            const int32_t absYawDelta = std::abs(static_cast<int32_t>(yawDelta));
            if (absYawDelta > kMaxYawDelta) {
                continue;
            }

            const float score = distance + (static_cast<float>(absYawDelta) * 0.0015f);
            if (score < bestScore) {
                bestScore = score;
                bestActor = actor;
            }
        }
    }

    return bestActor;
}

void IgniteActor(PlayState* play, Actor* actor) {
    if (play == nullptr || actor == nullptr) {
        return;
    }

    Vec3f flamePos = actor->world.pos;
    flamePos.y += 20.0f;
    EffectSsEnFire_SpawnVec3f(play, actor, &flamePos, 80, 0, 0, -1);
    Actor_SetColorFilter(actor, 0x4000, 0xFF, 0, 16);

    if ((actor->category == ACTORCAT_ENEMY || actor->category == ACTORCAT_BOSS) && actor->colChkInfo.health > 0) {
        actor->colChkInfo.damage = std::max<uint8_t>(actor->colChkInfo.damage, 2);
        Actor_ApplyDamage(actor);
    }
}

void SpawnIgniteMissEffect(PlayState* play, Player* player) {
    if (play == nullptr || player == nullptr) {
        return;
    }

    Vec3f missPos = player->actor.world.pos;
    missPos.x += Math_SinS(player->actor.shape.rot.y) * 95.0f;
    missPos.z += Math_CosS(player->actor.shape.rot.y) * 95.0f;
    missPos.y += 24.0f;
    EffectSsGFire_Spawn(play, &missPos);
}
bool MatchesHookFilter(const ExternalModHookFilter& filter, const ExternalModHookEventContext& context) {
    if (filter.hasScene && context.scene != filter.scene) {
        return false;
    }
    if (filter.hasActorId && context.actorId != filter.actorId) {
        return false;
    }
    if (filter.hasCategory && context.actorCategory != filter.category) {
        return false;
    }
    if (filter.hasItemId && context.itemId != filter.itemId) {
        return false;
    }
    if (filter.hasFlagType && context.flagType != filter.flagType) {
        return false;
    }
    if (filter.hasFlagId && context.flagId != filter.flagId) {
        return false;
    }
    if (filter.hasHealthDeltaRange &&
        (context.healthDelta < filter.healthDeltaMin || context.healthDelta > filter.healthDeltaMax)) {
        return false;
    }
    return true;
}

std::vector<int32_t> BuildHookWasmArgs(const ExternalModHookEventContext& context) {
    return {
        context.scene,
        context.actorId,
        context.actorCategory,
        context.itemId,
        context.flagType,
        context.flagId,
        context.healthDelta,
    };
}

const ExternalModActorDefinition* FindActorDefinition(const ExternalModRuntime& runtime, const std::string& definitionId) {
    const auto it = std::find_if(runtime.actorDefinitions.begin(), runtime.actorDefinitions.end(),
                                 [&definitionId](const ExternalModActorDefinition& definition) {
                                     return definition.id == definitionId;
                                 });
    if (it == runtime.actorDefinitions.end()) {
        return nullptr;
    }
    return &(*it);
}

ExternalModActorInstance* FindActorInstance(ExternalModRuntime& runtime, uint32_t handle) {
    const auto it = std::find_if(runtime.actorInstances.begin(), runtime.actorInstances.end(),
                                 [handle](const ExternalModActorInstance& instance) {
                                     return instance.handle == handle;
                                 });
    if (it == runtime.actorInstances.end()) {
        return nullptr;
    }
    return &(*it);
}

int32_t CountActiveActorInstances(const ExternalModRuntime& runtime, const std::string& definitionId) {
    return static_cast<int32_t>(std::count_if(runtime.actorInstances.begin(), runtime.actorInstances.end(),
                                              [&definitionId](const ExternalModActorInstance& instance) {
                                                  return instance.active && instance.definitionId == definitionId;
                                              }));
}

bool SpawnActorInstance(ExternalModPackage& package, const ExternalModActorDefinition& definition, uint32_t& outHandle,
                        std::string& outError) {
    auto& runtime = package.runtime;
    if (runtime.actorInstances.size() >= static_cast<size_t>(std::max(1, runtime.maxActorInstances))) {
        outError = "Actor instance budget exceeded for mod";
        return false;
    }

    if (CountActiveActorInstances(runtime, definition.id) >= std::max(1, definition.maxInstances)) {
        outError = "Actor definition instance limit reached: " + definition.id;
        return false;
    }

    ExternalModActorInstance instance;
    instance.handle = runtime.nextActorHandle++;
    instance.definitionId = definition.id;
    instance.active = true;
    instance.sceneId = definition.sceneId;
    instance.posX = definition.posX;
    instance.posY = definition.posY;
    instance.posZ = definition.posZ;
    instance.rotX = definition.rotX;
    instance.rotY = definition.rotY;
    instance.rotZ = definition.rotZ;

    runtime.actorInstances.push_back(instance);
    outHandle = instance.handle;

    if (!definition.exportOnInit.empty()) {
        if (!runtime.wasmRuntime) {
            outError = "Actor exportOnInit requires wasm runtime";
            runtime.actorInstances.pop_back();
            return false;
        }

        std::string wasmError;
        const std::vector<int32_t> args = { static_cast<int32_t>(instance.handle), instance.sceneId };
        if (!runtime.wasmRuntime->InvokeExport(definition.exportOnInit, args, wasmError)) {
            outError = "Actor exportOnInit failed: " + wasmError;
            runtime.actorInstances.pop_back();
            return false;
        }
    }

    return true;
}

bool DespawnActorInstance(ExternalModPackage& package, uint32_t handle, std::string& outError) {
    auto& runtime = package.runtime;
    const auto it = std::find_if(runtime.actorInstances.begin(), runtime.actorInstances.end(),
                                 [handle](const ExternalModActorInstance& instance) {
                                     return instance.handle == handle;
                                 });
    if (it == runtime.actorInstances.end()) {
        outError = "Actor handle not found: " + std::to_string(handle);
        return false;
    }

    const auto* definition = FindActorDefinition(runtime, it->definitionId);
    if (definition != nullptr && !definition->exportOnDestroy.empty()) {
        if (!runtime.wasmRuntime) {
            outError = "Actor exportOnDestroy requires wasm runtime";
            return false;
        }

        std::string wasmError;
        const std::vector<int32_t> args = { static_cast<int32_t>(it->handle), it->sceneId };
        if (!runtime.wasmRuntime->InvokeExport(definition->exportOnDestroy, args, wasmError)) {
            outError = "Actor exportOnDestroy failed: " + wasmError;
            return false;
        }
    }

    runtime.actorInstances.erase(it);
    return true;
}

const ExternalModBehaviorDefinition* FindBehaviorDefinition(const ExternalModRuntime& runtime, const std::string& behaviorId) {
    const auto it =
        std::find_if(runtime.behaviorDefinitions.begin(), runtime.behaviorDefinitions.end(),
                     [&behaviorId](const ExternalModBehaviorDefinition& definition) { return definition.id == behaviorId; });
    if (it == runtime.behaviorDefinitions.end()) {
        return nullptr;
    }
    return &(*it);
}

const ExternalModSceneDefinition* FindSceneDefinition(const ExternalModRuntime& runtime, const std::string& sceneId) {
    const auto it = std::find_if(runtime.sceneDefinitions.begin(), runtime.sceneDefinitions.end(),
                                 [&sceneId](const ExternalModSceneDefinition& definition) {
                                     return definition.id == sceneId;
                                 });
    if (it == runtime.sceneDefinitions.end()) {
        return nullptr;
    }
    return &(*it);
}

std::string NormalizeBlackboardScope(const std::string& scope) {
    const auto normalized = ToLower(scope);
    if (normalized == "global" || normalized == "global_mod" || normalized == "globalmod") {
        return "global_mod";
    }
    if (normalized == "scene" || normalized == "scene_mod" || normalized == "scene_mod_local") {
        return "scene";
    }
    if (normalized == "mod_save" || normalized == "modsave" || normalized == "save") {
        return "mod_save";
    }
    if (normalized == "actor") {
        return "actor";
    }
    return "";
}

bool TryParseFloatString(const std::string& value, float& outNumber) {
    try {
        size_t parsed = 0;
        const float parsedValue = std::stof(value, &parsed);
        if (parsed != value.size()) {
            return false;
        }
        outNumber = parsedValue;
        return std::isfinite(outNumber);
    } catch (...) {
        return false;
    }
}

std::unordered_map<std::string, std::string>* GetBlackboardScopeForAction(ExternalModPackage& package,
                                                                           ExternalModActorInstance* actorInstance,
                                                                           const std::string& rawScope) {
    const auto scope = NormalizeBlackboardScope(rawScope);
    if (scope.empty()) {
        return nullptr;
    }

    if (scope == "global_mod") {
        return &package.runtime.globalBlackboard;
    }
    if (scope == "scene" || scope == "mod_save") {
        int16_t sceneId = -1;
        if (gPlayState != nullptr) {
            sceneId = static_cast<int16_t>(gPlayState->sceneNum);
        } else if (actorInstance != nullptr) {
            sceneId = actorInstance->sceneId;
        }
        if (sceneId < 0) {
            sceneId = 0;
        }
        return &package.runtime.sceneBlackboard[sceneId];
    }
    if (scope == "actor" && actorInstance != nullptr) {
        return &actorInstance->state;
    }

    return nullptr;
}

bool EvaluateBehaviorCondition(const ExternalModPackage& package, const ExternalModBehaviorCondition& condition,
                               const ExternalModActorInstance* actorInstance) {
    const auto type = ToLower(condition.type);
    if (type.empty()) {
        return true;
    }

    auto resolveIntValue = [&](int32_t& outValue) -> bool {
        if (condition.hasNumberValue) {
            outValue = static_cast<int32_t>(std::lround(condition.numberValue));
            return true;
        }
        return TryParseIntToken(condition.value, outValue);
    };

    if (type == "ischild") {
        return gSaveContext.linkAge == LINK_AGE_CHILD;
    }
    if (type == "isadult") {
        return gSaveContext.linkAge == LINK_AGE_ADULT;
    }
    if (type == "isday") {
        return gSaveContext.dayTime >= 0x4555 && gSaveContext.dayTime < 0xC000;
    }
    if (type == "isnight") {
        return gSaveContext.dayTime >= 0xC000 || gSaveContext.dayTime < 0x4555;
    }
    if (type == "randomchance") {
        const float chance = std::clamp(condition.numberValue, 0.0f, 1.0f);
        return (Rand_ZeroOne() <= chance);
    }
    if (type == "hasitem") {
        if (condition.value.empty()) {
            return false;
        }
        const auto it =
            std::find_if(package.runtime.itemDefinitions.begin(), package.runtime.itemDefinitions.end(),
                         [&condition](const ExternalModItemDefinition& item) { return item.id == condition.value; });
        return it != package.runtime.itemDefinitions.end() && it->granted;
    }
    if (type == "distancetoplayer") {
        if (gPlayState == nullptr || actorInstance == nullptr) {
            return false;
        }
        auto* player = GET_PLAYER(gPlayState);
        if (player == nullptr) {
            return false;
        }

        const float dx = player->actor.world.pos.x - actorInstance->posX;
        const float dy = player->actor.world.pos.y - actorInstance->posY;
        const float dz = player->actor.world.pos.z - actorInstance->posZ;
        const float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
        const std::string op = condition.op.empty() ? "<=" : condition.op;
        if (op == "<") {
            return distance < condition.numberValue;
        }
        if (op == "<=") {
            return distance <= condition.numberValue;
        }
        if (op == ">") {
            return distance > condition.numberValue;
        }
        if (op == ">=") {
            return distance >= condition.numberValue;
        }
        if (op == "==" || op == "=") {
            return std::abs(distance - condition.numberValue) <= 0.0001f;
        }
        if (op == "!=") {
            return std::abs(distance - condition.numberValue) > 0.0001f;
        }
        return false;
    }

    if (type == "sceneis") {
        if (gPlayState == nullptr) {
            return false;
        }
        int32_t expected = 0;
        if (!resolveIntValue(expected)) {
            return false;
        }
        const int32_t current = static_cast<int32_t>(gPlayState->sceneNum);
        const std::string op = condition.op.empty() ? "==" : condition.op;
        if (op == "==" || op == "=") {
            return current == expected;
        }
        if (op == "!=") {
            return current != expected;
        }
        if (op == "<") {
            return current < expected;
        }
        if (op == "<=") {
            return current <= expected;
        }
        if (op == ">") {
            return current > expected;
        }
        if (op == ">=") {
            return current >= expected;
        }
        return false;
    }

    if (type == "roomis") {
        if (gPlayState == nullptr) {
            return false;
        }
        int32_t expected = 0;
        if (!resolveIntValue(expected)) {
            return false;
        }
        const int32_t current = static_cast<int32_t>(gPlayState->roomCtx.curRoom.num);
        const std::string op = condition.op.empty() ? "==" : condition.op;
        if (op == "==" || op == "=") {
            return current == expected;
        }
        if (op == "!=") {
            return current != expected;
        }
        if (op == "<") {
            return current < expected;
        }
        if (op == "<=") {
            return current <= expected;
        }
        if (op == ">") {
            return current > expected;
        }
        if (op == ">=") {
            return current >= expected;
        }
        return false;
    }

    if (type == "hasswitchflag" || type == "switchison") {
        if (gPlayState == nullptr) {
            return false;
        }
        int32_t flag = 0;
        if (!resolveIntValue(flag)) {
            return false;
        }
        const bool isSet = Flags_GetSwitch(gPlayState, flag & 0x3F);
        const std::string op = condition.op.empty() ? "==" : condition.op;
        if (op == "==" || op == "=") {
            return isSet;
        }
        if (op == "!=") {
            return !isSet;
        }
        return isSet;
    }

    if (type == "var" || type == "varequals" || type == "varcompare") {
        if (condition.key.empty()) {
            return false;
        }
        const auto scope = NormalizeBlackboardScope(condition.scope);
        const auto* actorState = actorInstance != nullptr ? &actorInstance->state : nullptr;
        const std::unordered_map<std::string, std::string>* map = nullptr;
        if (scope == "actor") {
            map = actorState;
        } else if (scope == "global_mod") {
            map = &package.runtime.globalBlackboard;
        } else if (scope == "scene" || scope == "mod_save") {
            int16_t sceneId = -1;
            if (gPlayState != nullptr) {
                sceneId = static_cast<int16_t>(gPlayState->sceneNum);
            } else if (actorInstance != nullptr) {
                sceneId = actorInstance->sceneId;
            }
            if (sceneId < 0) {
                return false;
            }
            auto sceneIt = package.runtime.sceneBlackboard.find(sceneId);
            if (sceneIt != package.runtime.sceneBlackboard.end()) {
                map = &sceneIt->second;
            }
        }

        if (map == nullptr) {
            return false;
        }
        const auto valueIt = map->find(condition.key);
        if (valueIt == map->end()) {
            return false;
        }

        const std::string op = condition.op.empty() ? "==" : condition.op;
        if (op == "==" || op == "=") {
            return valueIt->second == condition.value;
        }
        if (op == "!=") {
            return valueIt->second != condition.value;
        }

        float lhs = 0.0f;
        float rhs = 0.0f;
        if (!TryParseFloatString(valueIt->second, lhs) || !TryParseFloatString(condition.value, rhs)) {
            return false;
        }
        if (op == "<") {
            return lhs < rhs;
        }
        if (op == "<=") {
            return lhs <= rhs;
        }
        if (op == ">") {
            return lhs > rhs;
        }
        if (op == ">=") {
            return lhs >= rhs;
        }
    }

    return false;
}

bool BehaviorRuleMatches(const ExternalModPackage& package, const ExternalModBehaviorRule& rule,
                         const ExternalModActorInstance* actorInstance) {
    if (rule.randomChance < 1.0f && Rand_ZeroOne() > std::clamp(rule.randomChance, 0.0f, 1.0f)) {
        return false;
    }

    for (const auto& condition : rule.conditions) {
        if (!EvaluateBehaviorCondition(package, condition, actorInstance)) {
            return false;
        }
    }
    return true;
}

bool CollectBehaviorEventActions(const ExternalModPackage& package, const std::string& behaviorId, const std::string& eventName,
                                 const ExternalModActorInstance* actorInstance, int32_t maxSteps, int32_t& ioSteps,
                                 std::vector<ExternalModAction>& outActions, std::string& outError) {
    outActions.clear();
    outError.clear();

    const auto* behavior = FindBehaviorDefinition(package.runtime, behaviorId);
    if (behavior == nullptr) {
        outError = "unknown behaviorId: " + behaviorId;
        return false;
    }

    std::vector<std::string> candidateEvents;
    const std::string normalizedEvent = ToLower(eventName);
    candidateEvents.push_back(normalizedEvent);
    auto addAlias = [&](const std::string& alias) {
        if (!alias.empty() &&
            std::find(candidateEvents.begin(), candidateEvents.end(), alias) == candidateEvents.end()) {
            candidateEvents.push_back(alias);
        }
    };

    if (normalizedEvent == "onspawn") {
        addAlias("oninit");
    } else if (normalizedEvent == "ondestroy") {
        addAlias("ondespawn");
    } else if (normalizedEvent == "onitemgranted") {
        addAlias("onitemequipped");
    } else if (normalizedEvent == "onitemequipped") {
        addAlias("onitemgranted");
    }

    for (const auto& candidateEvent : candidateEvents) {
        const auto eventIt = behavior->events.find(candidateEvent);
        if (eventIt == behavior->events.end()) {
            continue;
        }

        for (const auto& rule : eventIt->second) {
            if (!BehaviorRuleMatches(package, rule, actorInstance)) {
                continue;
            }
            for (const auto& action : rule.actions) {
                if (ioSteps >= maxSteps) {
                    outError = "behavior action budget exceeded";
                    return false;
                }
                outActions.push_back(action);
                ++ioSteps;
            }
        }
    }
    return true;
}

} // namespace

ExternalModManager& ExternalModManager::Instance() {
    static ExternalModManager instance;
    return instance;
}

std::vector<ExternalModPackage>& ExternalModManager::GetPackages() {
    return mPackages;
}

const std::vector<ExternalModPackage>& ExternalModManager::GetPackages() const {
    return mPackages;
}

void ExternalModManager::ApplyGetItemVisualOverrides(GetItemEntry& entry) const {
    if (entry.modIndex != MOD_NONE) {
        return;
    }

    if (entry.drawFunc != nullptr) {
        return;
    }

    const auto* definition = FindCustomModelDefinitionForItem(mPackages, entry.itemId);
    if (definition == nullptr) {
        return;
    }

    entry.drawFunc = ExternalMods_DrawCustomGetItemModel;
}

std::string ExternalModManager::BuildEnabledCVarName(const std::string& modId) {
    return "gExternalMods.Enabled." + SanitizeCVarSegment(modId);
}

std::string ExternalModManager::BuildBindingCVarName(const std::string& modId, const std::string& bindingId) {
    return "gExternalMods.Input." + SanitizeCVarSegment(modId) + "." + SanitizeCVarSegment(bindingId);
}

std::vector<ExternalModInventoryCellView> ExternalModManager::GetExtraInventoryGrid() const {
    std::vector<ExternalModInventoryCellView> outCells;
    outCells.reserve(mExtraInventoryCells.size());

    for (size_t i = 0; i < mExtraInventoryCells.size(); ++i) {
        ExternalModInventoryCellView view;
        view.index = i;

        const auto& cell = mExtraInventoryCells[i];
        if (cell.modId.empty() || cell.itemId.empty()) {
            outCells.push_back(std::move(view));
            continue;
        }

        const auto packageIt = std::find_if(mPackages.begin(), mPackages.end(), [&cell](const ExternalModPackage& package) {
            return package.manifest.id == cell.modId;
        });
        if (packageIt == mPackages.end() || !packageIt->runtime.enabled) {
            outCells.push_back(std::move(view));
            continue;
        }

        const auto* definition = FindItemDefinitionById(packageIt->runtime, cell.itemId);
        if (definition == nullptr || !definition->granted) {
            outCells.push_back(std::move(view));
            continue;
        }

        view.occupied = true;
        view.modId = cell.modId;
        view.modName = packageIt->manifest.name;
        view.itemId = definition->id;
        view.displayName = definition->displayName;
        view.slot = definition->slot;
        view.granted = definition->granted;
        outCells.push_back(std::move(view));
    }

    return outCells;
}

bool ExternalModManager::MoveExtraInventoryCell(size_t fromIndex, size_t toIndex, std::string& outError) {
    outError.clear();

    if (fromIndex >= mExtraInventoryCells.size() || toIndex >= mExtraInventoryCells.size()) {
        outError = "cell index out of range";
        return false;
    }

    if (fromIndex == toIndex) {
        return true;
    }

    if (mExtraInventoryCells[fromIndex].modId.empty() || mExtraInventoryCells[fromIndex].itemId.empty()) {
        outError = "source cell is empty";
        return false;
    }

    std::swap(mExtraInventoryCells[fromIndex], mExtraInventoryCells[toIndex]);
    return true;
}

bool ExternalModManager::EquipExtraInventoryCellToButton(size_t cellIndex, int32_t cButtonIndex, std::string& outError) {
    outError.clear();

    if (cellIndex >= mExtraInventoryCells.size()) {
        outError = "cell index out of range";
        return false;
    }

    if (cButtonIndex < 0 || cButtonIndex >= static_cast<int32_t>(ARRAY_COUNT(gSaveContext.equips.cButtonSlots))) {
        outError = "c-button index out of range";
        return false;
    }

    const auto& cell = mExtraInventoryCells[cellIndex];
    if (cell.modId.empty() || cell.itemId.empty()) {
        outError = "cell is empty";
        return false;
    }

    const auto packageIt = std::find_if(mPackages.begin(), mPackages.end(), [&cell](const ExternalModPackage& package) {
        return package.manifest.id == cell.modId;
    });
    if (packageIt == mPackages.end() || !packageIt->runtime.enabled) {
        outError = "mod is not enabled";
        return false;
    }

    auto itemIt = std::find_if(packageIt->runtime.itemDefinitions.begin(), packageIt->runtime.itemDefinitions.end(),
                               [&cell](const ExternalModItemDefinition& itemDefinition) {
                                   return itemDefinition.id == cell.itemId;
                               });
    if (itemIt == packageIt->runtime.itemDefinitions.end() || !itemIt->granted) {
        outError = "item is not granted";
        return false;
    }

    const auto* slotConfig = FindItemSlotConfig(itemIt->slot);
    if (slotConfig == nullptr) {
        outError = "unsupported item slot";
        return false;
    }

    if (slotConfig->slotIndex < 0 || slotConfig->slotIndex >= static_cast<int32_t>(ARRAY_COUNT(gSaveContext.inventory.items))) {
        outError = "inventory slot index out of range";
        return false;
    }

    const int32_t grantedItemId = ResolveGrantedItemId(*itemIt);
    if (gSaveContext.inventory.items[slotConfig->slotIndex] == ITEM_NONE && grantedItemId != ITEM_NONE &&
        grantedItemId >= std::numeric_limits<int8_t>::min() && grantedItemId <= std::numeric_limits<int8_t>::max()) {
        gSaveContext.inventory.items[slotConfig->slotIndex] = static_cast<int8_t>(grantedItemId);
    }

    const int32_t equippedItem = gSaveContext.inventory.items[slotConfig->slotIndex];
    if (equippedItem == ITEM_NONE) {
        outError = "inventory slot is empty";
        return false;
    }

    if (equippedItem < std::numeric_limits<int8_t>::min() || equippedItem > std::numeric_limits<int8_t>::max()) {
        outError = "equipped item id out of range";
        return false;
    }

    gSaveContext.equips.cButtonSlots[cButtonIndex] = static_cast<int8_t>(slotConfig->slotIndex);
    gSaveContext.equips.buttonItems[cButtonIndex + 1] = static_cast<int8_t>(equippedItem);

    if (gSaveContext.linkAge == LINK_AGE_ADULT) {
        gSaveContext.adultEquips.cButtonSlots[cButtonIndex] = static_cast<int8_t>(slotConfig->slotIndex);
        gSaveContext.adultEquips.buttonItems[cButtonIndex + 1] = static_cast<int8_t>(equippedItem);
    } else {
        gSaveContext.childEquips.cButtonSlots[cButtonIndex] = static_cast<int8_t>(slotConfig->slotIndex);
        gSaveContext.childEquips.buttonItems[cButtonIndex + 1] = static_cast<int8_t>(equippedItem);
    }

    if (gPlayState != nullptr) {
        Interface_LoadItemIcon1(gPlayState, static_cast<uint16_t>(cButtonIndex + 1));
    }

    SPDLOG_INFO("[ExternalMods] Equipped extra inventory item {} from mod {} to C-button {}", itemIt->id,
                packageIt->manifest.id, cButtonIndex);
    return true;
}

void ExternalModManager::SyncExtraInventoryGrid() {
    if (mExtraInventoryCells.size() != kExternalModExtraInventoryCellCount) {
        mExtraInventoryCells.assign(kExternalModExtraInventoryCellCount, ExtraInventoryCell{});
    }

    const auto buildCellKey = [](const std::string& modId, const std::string& itemId) {
        return modId + "\x1F" + itemId;
    };

    std::unordered_set<std::string> grantedKeys;
    for (const auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }

        for (const auto& item : package.runtime.itemDefinitions) {
            if (!item.granted) {
                continue;
            }
            grantedKeys.insert(buildCellKey(package.manifest.id, item.id));
        }
    }

    std::unordered_set<std::string> placedKeys;
    for (auto& cell : mExtraInventoryCells) {
        if (cell.modId.empty() || cell.itemId.empty()) {
            continue;
        }

        const auto key = buildCellKey(cell.modId, cell.itemId);
        if (!grantedKeys.contains(key) || !placedKeys.insert(key).second) {
            cell = ExtraInventoryCell{};
        }
    }

    bool warnedNoFreeCell = false;
    for (const auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }

        for (const auto& item : package.runtime.itemDefinitions) {
            if (!item.granted) {
                continue;
            }

            const auto key = buildCellKey(package.manifest.id, item.id);
            if (placedKeys.contains(key)) {
                continue;
            }

            const auto emptyCellIt = std::find_if(mExtraInventoryCells.begin(), mExtraInventoryCells.end(),
                                                  [](const ExtraInventoryCell& cell) {
                                                      return cell.modId.empty() || cell.itemId.empty();
                                                  });
            if (emptyCellIt == mExtraInventoryCells.end()) {
                if (!warnedNoFreeCell) {
                    SPDLOG_WARN("[ExternalMods] Extra inventory grid is full ({} cells)",
                                mExtraInventoryCells.size());
                    warnedNoFreeCell = true;
                }
                break;
            }

            emptyCellIt->modId = package.manifest.id;
            emptyCellIt->itemId = item.id;
            placedKeys.insert(key);
        }
    }
}

void ExternalModManager::Shutdown() {
    UnregisterHooks();
    for (auto& package : mPackages) {
        UnmountAssetsForPackage(package);
        package.runtime.enabled = false;
        package.runtime.onGameLoadedActions.clear();
        package.runtime.onSceneInitActions.clear();
        package.runtime.frameTriggers.clear();
        package.runtime.inputBindings.clear();
        package.runtime.inputTriggers.clear();
        package.runtime.itemDefinitions.clear();
        package.runtime.hookSubscriptions.clear();
        package.runtime.actorDefinitions.clear();
        package.runtime.actorInstances.clear();
        package.runtime.nextActorHandle = 1;
        package.runtime.hookCallsThisFrame = 0;
        package.runtime.moduleSourcePath.clear();
        package.runtime.compiledModuleSizeBytes = 0;
        package.runtime.moduleCompileTimeMs = 0;
        package.runtime.moduleCompileDiagnostics.clear();
        package.runtime.wasmRuntime.reset();
    }

    mPackages.clear();
    mExtraInventoryCells.clear();
    gExternalModMissingDisplayListWarnings.clear();
    gExternalModDisplayListDrawDebugLogs.clear();
    ApplyModItemAgeRequirementOverrides(mPackages);
    ApplyModItemIconOverrides(mPackages);
    ApplyModHookshotTextureOverrides(mPackages);
}

bool ExternalModManager::ReloadPackages(std::string& outError) {
    outError.clear();
    Initialize();

    // During in-game reload, replay onLoadGame actions so granted-state-dependent overrides
    // (icons, age requirements, hookshot textures, etc.) are restored immediately.
    if (gPlayState != nullptr) {
        OnLoadGame(gSaveContext.fileNum);
    }

    size_t invalidCount = 0;
    for (const auto& package : mPackages) {
        if (!package.valid) {
            invalidCount++;
        }
    }

    if (invalidCount > 0) {
        outError = std::to_string(invalidCount) + " external mod(s) failed to load; check package details for errors.";
    }

    return true;
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
        const auto entryName = entry.path().filename().generic_string();
        if (!entryName.empty() && entryName[0] == '.') {
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

    for (auto& package : mPackages) {
        UnmountAssetsForPackage(package);
    }

    mPackages.clear();
    gExternalModMissingDisplayListWarnings.clear();
    gExternalModDisplayListDrawDebugLogs.clear();
    ApplyModItemAgeRequirementOverrides(mPackages);
    ApplyModItemIconOverrides(mPackages);
    ApplyModHookshotTextureOverrides(mPackages);

    {
        auto context = Ship::Context::GetInstance();
        if (context != nullptr && context->GetResourceManager() != nullptr &&
            context->GetResourceManager()->GetArchiveManager() != nullptr) {
            auto archiveManager = context->GetResourceManager()->GetArchiveManager();
            auto loadedArchives = archiveManager->GetArchives();
            const auto legacyRoot = ToLower(GetExternalModsLegacyCacheRootPath().lexically_normal().generic_string());
            const auto cacheRoot = ToLower(GetExternalModsCacheRootPath().lexically_normal().generic_string());

            for (const auto& archive : *loadedArchives) {
                if (archive == nullptr) {
                    continue;
                }
                const auto archivePath = ToLower(std::filesystem::path(archive->GetPath()).lexically_normal().generic_string());
                if ((!legacyRoot.empty() && archivePath.rfind(legacyRoot, 0) == 0) ||
                    (!cacheRoot.empty() && archivePath.rfind(cacheRoot, 0) == 0)) {
                    archiveManager->RemoveArchive(archive->GetPath());
                }
            }
        }
    }

    {
        std::error_code ec;
        std::filesystem::remove_all(GetExternalModsLegacyCacheRootPath(), ec);
        if (ec) {
            SPDLOG_WARN("[ExternalMods] Failed to remove legacy cache folder: {}", ec.message());
        }
    }

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

        const auto enabledCVarName = BuildEnabledCVarName(package.manifest.id);
        int32_t enabledValue = CVarGetInteger(enabledCVarName.c_str(), 1);
        enabledValue = enabledValue != 0 ? 1 : 0;
        CVarSetInteger(enabledCVarName.c_str(), enabledValue);

        package.runtime.enabled = enabledValue != 0;
        if (package.runtime.enabled) {
            runtimeCount++;
            SPDLOG_INFO("[ExternalMods] Runtime enabled: {} ({})", package.manifest.name, package.manifest.id);
        } else {
            SPDLOG_INFO("[ExternalMods] Runtime disabled by user: {} ({})", package.manifest.name, package.manifest.id);
        }
    }

    if (runtimeCount > 0) {
        RegisterHooks();
    }

    SyncExtraInventoryGrid();
    ApplyModItemAgeRequirementOverrides(mPackages);
    ApplyModItemIconOverrides(mPackages);
    ApplyModHookshotTextureOverrides(mPackages);

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

    if (json.contains("capabilities")) {
        if (!json["capabilities"].is_array()) {
            outError = "Invalid field: capabilities must be an array";
            return false;
        }

        std::unordered_set<std::string> seenCapabilities;
        for (size_t i = 0; i < json["capabilities"].size(); ++i) {
            const auto& cap = json["capabilities"][i];
            if (!cap.is_string()) {
                outError = "Invalid capabilities[" + std::to_string(i) + "]: expected string";
                return false;
            }
            const auto normalizedCap = ToLower(cap.get<std::string>());
            if (!kSupportedCapabilities.contains(normalizedCap)) {
                outError = "Unsupported capability: " + normalizedCap;
                return false;
            }
            if (seenCapabilities.insert(normalizedCap).second) {
                outManifest.capabilities.push_back(normalizedCap);
            }
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
        if (!IsSupportedRuntimeModuleExtension(normalizedRuntimeModule)) {
            outError = "Invalid runtime.module: extension must be .wasm or .wat";
            return false;
        }
        outManifest.runtimeModule = normalizedRuntimeModule.generic_string();

        outManifest.runtimeMaxMemoryKb = kDefaultRuntimeMemoryKb;
        outManifest.runtimeMaxCallMs = kDefaultRuntimeCallMs;
        outManifest.runtimeMaxFrameBudgetMs = kDefaultRuntimeFrameBudgetMs;
        outManifest.runtimeMaxHookCallsPerFrame = kDefaultRuntimeHookCallsPerFrame;
        outManifest.runtimeMaxActorInstances = kDefaultRuntimeActorInstances;

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
        if (runtime.contains("maxFrameBudgetMs")) {
            if (!runtime["maxFrameBudgetMs"].is_number_integer()) {
                outError = "runtime.maxFrameBudgetMs must be integer";
                return false;
            }
            outManifest.runtimeMaxFrameBudgetMs = runtime["maxFrameBudgetMs"].get<int32_t>();
        }
        if (runtime.contains("maxHookCallsPerFrame")) {
            if (!runtime["maxHookCallsPerFrame"].is_number_integer()) {
                outError = "runtime.maxHookCallsPerFrame must be integer";
                return false;
            }
            outManifest.runtimeMaxHookCallsPerFrame = runtime["maxHookCallsPerFrame"].get<int32_t>();
        }
        if (runtime.contains("maxActorInstances")) {
            if (!runtime["maxActorInstances"].is_number_integer()) {
                outError = "runtime.maxActorInstances must be integer";
                return false;
            }
            outManifest.runtimeMaxActorInstances = runtime["maxActorInstances"].get<int32_t>();
        }

        if (outManifest.runtimeMaxMemoryKb < 64 || outManifest.runtimeMaxMemoryKb > 4096) {
            outError = "runtime.maxMemoryKb must be in [64, 4096]";
            return false;
        }
        if (outManifest.runtimeMaxCallMs < 1 || outManifest.runtimeMaxCallMs > 4) {
            outError = "runtime.maxCallMs must be in [1, 4]";
            return false;
        }
        if (outManifest.runtimeMaxFrameBudgetMs < 1 || outManifest.runtimeMaxFrameBudgetMs > 4) {
            outError = "runtime.maxFrameBudgetMs must be in [1, 4]";
            return false;
        }
        if (outManifest.runtimeMaxHookCallsPerFrame < 1 || outManifest.runtimeMaxHookCallsPerFrame > 2048) {
            outError = "runtime.maxHookCallsPerFrame must be in [1, 2048]";
            return false;
        }
        if (outManifest.runtimeMaxActorInstances < 1 || outManifest.runtimeMaxActorInstances > 512) {
            outError = "runtime.maxActorInstances must be in [1, 512]";
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

        const bool hasExtendedHooksCapability = ManifestHasCapability(outManifest, "hooks.extended.v1");
        const bool hasActorVmCapability = ManifestHasCapability(outManifest, "actors.vm.v1");
        const bool hasActorGenericCapability = ManifestHasCapability(outManifest, "actors.generic.v1");
        const bool hasBehaviorCapability = ManifestHasCapability(outManifest, "behaviors.graph.v1");
        const bool hasSceneBundleCapability = ManifestHasCapability(outManifest, "scenes.bundle.v1");

        if (json.contains("hookDefinitions")) {
            if (!hasExtendedHooksCapability) {
                outError = "hookDefinitions requires capability hooks.extended.v1";
                return false;
            }
            if (!ValidateRequiredString(json, "hookDefinitions", outManifest.hookDefinitions, outError)) {
                outError = "Missing or invalid field: hookDefinitions";
                return false;
            }
            std::filesystem::path normalizedHooksPath;
            if (!IsSafePackageRelativePath(outManifest.hookDefinitions, normalizedHooksPath, outError)) {
                outError = "Invalid hookDefinitions: " + outError;
                return false;
            }
            outManifest.hookDefinitions = normalizedHooksPath.generic_string();
        } else if (hasExtendedHooksCapability) {
            outError = "Missing required field for hooks.extended.v1: hookDefinitions";
            return false;
        }

        if (json.contains("actorDefinitions")) {
            if (!hasActorVmCapability && !hasActorGenericCapability) {
                outError = "actorDefinitions requires capability actors.vm.v1 or actors.generic.v1";
                return false;
            }
            if (!ValidateRequiredString(json, "actorDefinitions", outManifest.actorDefinitions, outError)) {
                outError = "Missing or invalid field: actorDefinitions";
                return false;
            }
            std::filesystem::path normalizedActorsPath;
            if (!IsSafePackageRelativePath(outManifest.actorDefinitions, normalizedActorsPath, outError)) {
                outError = "Invalid actorDefinitions: " + outError;
                return false;
            }
            outManifest.actorDefinitions = normalizedActorsPath.generic_string();
        } else if (hasActorVmCapability || hasActorGenericCapability) {
            outError = "Missing required field for actors.vm.v1/actors.generic.v1: actorDefinitions";
            return false;
        }

        if (json.contains("behaviorDefinitions")) {
            if (!hasBehaviorCapability) {
                outError = "behaviorDefinitions requires capability behaviors.graph.v1";
                return false;
            }
            if (!ValidateRequiredString(json, "behaviorDefinitions", outManifest.behaviorDefinitions, outError)) {
                outError = "Missing or invalid field: behaviorDefinitions";
                return false;
            }
            std::filesystem::path normalizedBehaviorPath;
            if (!IsSafePackageRelativePath(outManifest.behaviorDefinitions, normalizedBehaviorPath, outError)) {
                outError = "Invalid behaviorDefinitions: " + outError;
                return false;
            }
            outManifest.behaviorDefinitions = normalizedBehaviorPath.generic_string();
        } else if (hasBehaviorCapability) {
            outError = "Missing required field for behaviors.graph.v1: behaviorDefinitions";
            return false;
        }

        if (json.contains("sceneDefinitions")) {
            if (!hasSceneBundleCapability) {
                outError = "sceneDefinitions requires capability scenes.bundle.v1";
                return false;
            }
            if (!ValidateRequiredString(json, "sceneDefinitions", outManifest.sceneDefinitions, outError)) {
                outError = "Missing or invalid field: sceneDefinitions";
                return false;
            }
            std::filesystem::path normalizedScenePath;
            if (!IsSafePackageRelativePath(outManifest.sceneDefinitions, normalizedScenePath, outError)) {
                outError = "Invalid sceneDefinitions: " + outError;
                return false;
            }
            outManifest.sceneDefinitions = normalizedScenePath.generic_string();
        } else if (hasSceneBundleCapability) {
            outError = "Missing required field for scenes.bundle.v1: sceneDefinitions";
            return false;
        }
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

    if (json.contains("behaviorRuntime")) {
        if (!json["behaviorRuntime"].is_object()) {
            outError = "behaviorRuntime must be object";
            return false;
        }

        const auto& behaviorRuntime = json["behaviorRuntime"];
        if (behaviorRuntime.contains("maxStepsPerActorPerFrame")) {
            if (!behaviorRuntime["maxStepsPerActorPerFrame"].is_number_integer()) {
                outError = "behaviorRuntime.maxStepsPerActorPerFrame must be integer";
                return false;
            }
            outRuntime.behaviorMaxStepsPerActorPerFrame =
                std::clamp(behaviorRuntime["maxStepsPerActorPerFrame"].get<int32_t>(), 1, 1024);
        }
        if (behaviorRuntime.contains("maxStepsPerModPerFrame")) {
            if (!behaviorRuntime["maxStepsPerModPerFrame"].is_number_integer()) {
                outError = "behaviorRuntime.maxStepsPerModPerFrame must be integer";
                return false;
            }
            outRuntime.behaviorMaxStepsPerModPerFrame =
                std::clamp(behaviorRuntime["maxStepsPerModPerFrame"].get<int32_t>(), 1, 50000);
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
        bool modelTextureWidthSpecified = false;
        bool modelTextureHeightSpecified = false;
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

        const auto* slotConfig = FindItemSlotConfigByName(slot);
        if (slotConfig == nullptr) {
            outError = "items[" + std::to_string(i) + "].slot unsupported: " + slot +
                       ". Supported slots: " + BuildSupportedItemSlotList();
            return false;
        }
        definition.slot = slotConfig->slot;
        definition.agePolicy = ExternalModItemAgePolicy::AllowChild;
        definition.useMode = definition.slot == ExternalModItemSlot::Hookshot ? ExternalModItemUseMode::Override
                                                                              : ExternalModItemUseMode::Vanilla;

        if (item.contains("iconAsset")) {
            if (!item["iconAsset"].is_string()) {
                outError = "items[" + std::to_string(i) + "].iconAsset must be string";
                return false;
            }
            definition.iconAsset = item["iconAsset"].get<std::string>();
        }

        if (item.contains("modelAsset")) {
            if (!item["modelAsset"].is_string()) {
                outError = "items[" + std::to_string(i) + "].modelAsset must be string";
                return false;
            }
            definition.modelAsset = item["modelAsset"].get<std::string>();
        }

        if (item.contains("modelTextureAsset")) {
            if (!item["modelTextureAsset"].is_string()) {
                outError = "items[" + std::to_string(i) + "].modelTextureAsset must be string";
                return false;
            }
            definition.modelTextureAsset = item["modelTextureAsset"].get<std::string>();
        }

        if (item.contains("modelDisplayList")) {
            if (!item["modelDisplayList"].is_string()) {
                outError = "items[" + std::to_string(i) + "].modelDisplayList must be string";
                return false;
            }
            definition.modelDisplayList = item["modelDisplayList"].get<std::string>();
        }

        if (item.contains("modelScale")) {
            if (!item["modelScale"].is_number()) {
                outError = "items[" + std::to_string(i) + "].modelScale must be numeric";
                return false;
            }
            definition.modelScale = item["modelScale"].get<float>();
        }

        if (item.contains("modelUvOrigin")) {
            if (!ParseModelUvOrigin(item["modelUvOrigin"], definition.modelUvOrigin, outError)) {
                outError = "items[" + std::to_string(i) + "].modelUvOrigin " + outError;
                return false;
            }
        }

        if (item.contains("modelTextureFilter")) {
            if (!ParseModelTextureFilter(item["modelTextureFilter"], definition.modelTextureFilter, outError)) {
                outError = "items[" + std::to_string(i) + "].modelTextureFilter " + outError;
                return false;
            }
        }

        if (item.contains("modelTextureWidth")) {
            if (!item["modelTextureWidth"].is_number_integer()) {
                outError = "items[" + std::to_string(i) + "].modelTextureWidth must be integer";
                return false;
            }
            definition.modelTextureTargetWidth = item["modelTextureWidth"].get<int32_t>();
            modelTextureWidthSpecified = true;
        }

        if (item.contains("modelTextureHeight")) {
            if (!item["modelTextureHeight"].is_number_integer()) {
                outError = "items[" + std::to_string(i) + "].modelTextureHeight must be integer";
                return false;
            }
            definition.modelTextureTargetHeight = item["modelTextureHeight"].get<int32_t>();
            modelTextureHeightSpecified = true;
        }

        if (item.contains("model")) {
            if (!item["model"].is_object()) {
                outError = "items[" + std::to_string(i) + "].model must be object";
                return false;
            }

            const auto& model = item["model"];
            if (model.contains("asset")) {
                if (!model["asset"].is_string()) {
                    outError = "items[" + std::to_string(i) + "].model.asset must be string";
                    return false;
                }
                definition.modelAsset = model["asset"].get<std::string>();
            }
            if (model.contains("texture")) {
                if (!model["texture"].is_string()) {
                    outError = "items[" + std::to_string(i) + "].model.texture must be string";
                    return false;
                }
                definition.modelTextureAsset = model["texture"].get<std::string>();
            }
            if (model.contains("displayList")) {
                if (!model["displayList"].is_string()) {
                    outError = "items[" + std::to_string(i) + "].model.displayList must be string";
                    return false;
                }
                definition.modelDisplayList = model["displayList"].get<std::string>();
            }
            if (model.contains("scale")) {
                if (!model["scale"].is_number()) {
                    outError = "items[" + std::to_string(i) + "].model.scale must be numeric";
                    return false;
                }
                definition.modelScale = model["scale"].get<float>();
            }
            if (model.contains("uvOrigin")) {
                if (!ParseModelUvOrigin(model["uvOrigin"], definition.modelUvOrigin, outError)) {
                    outError = "items[" + std::to_string(i) + "].model.uvOrigin " + outError;
                    return false;
                }
            }
            if (model.contains("textureFilter")) {
                if (!ParseModelTextureFilter(model["textureFilter"], definition.modelTextureFilter, outError)) {
                    outError = "items[" + std::to_string(i) + "].model.textureFilter " + outError;
                    return false;
                }
            }
            if (model.contains("textureWidth")) {
                if (!model["textureWidth"].is_number_integer()) {
                    outError = "items[" + std::to_string(i) + "].model.textureWidth must be integer";
                    return false;
                }
                definition.modelTextureTargetWidth = model["textureWidth"].get<int32_t>();
                modelTextureWidthSpecified = true;
            }
            if (model.contains("textureHeight")) {
                if (!model["textureHeight"].is_number_integer()) {
                    outError = "items[" + std::to_string(i) + "].model.textureHeight must be integer";
                    return false;
                }
                definition.modelTextureTargetHeight = model["textureHeight"].get<int32_t>();
                modelTextureHeightSpecified = true;
            }
        }

        const bool hasModelTextureWidth = modelTextureWidthSpecified;
        const bool hasModelTextureHeight = modelTextureHeightSpecified;
        if (hasModelTextureWidth != hasModelTextureHeight) {
            outError = "items[" + std::to_string(i) +
                       "].modelTextureWidth/modelTextureHeight must be provided together";
            return false;
        }

        if (hasModelTextureWidth) {
            if (definition.modelTextureTargetWidth < 1 || definition.modelTextureTargetWidth > 1024 ||
                definition.modelTextureTargetHeight < 1 || definition.modelTextureTargetHeight > 1024) {
                outError = "items[" + std::to_string(i) +
                           "].modelTextureWidth/modelTextureHeight must be in range 1..1024";
                return false;
            }
        }

        const auto parseOptionalHookshotTextureField = [&](const nlohmann::json& object, const char* key,
                                                           std::string& outputPath) -> bool {
            if (!object.contains(key)) {
                return true;
            }
            if (!object[key].is_string()) {
                outError = "items[" + std::to_string(i) + "]." + key + " must be string";
                return false;
            }
            outputPath = object[key].get<std::string>();
            return true;
        };

        if (!parseOptionalHookshotTextureField(item, "hookshotMetalTextureAsset", definition.hookshotMetalTextureAsset) ||
            !parseOptionalHookshotTextureField(item, "hookshotHandleTextureAsset", definition.hookshotHandleTextureAsset) ||
            !parseOptionalHookshotTextureField(item, "hookshotDesignTextureAsset", definition.hookshotDesignTextureAsset) ||
            !parseOptionalHookshotTextureField(item, "hookshotChainTextureAsset", definition.hookshotChainTextureAsset) ||
            !parseOptionalHookshotTextureField(item, "hookshotReticleTextureAsset",
                                               definition.hookshotReticleTextureAsset)) {
            return false;
        }

        if (item.contains("hookshotTextures")) {
            if (!item["hookshotTextures"].is_object()) {
                outError = "items[" + std::to_string(i) + "].hookshotTextures must be object";
                return false;
            }

            const auto& hookshotTextures = item["hookshotTextures"];
            if (!parseOptionalHookshotTextureField(hookshotTextures, "metal", definition.hookshotMetalTextureAsset) ||
                !parseOptionalHookshotTextureField(hookshotTextures, "handle", definition.hookshotHandleTextureAsset) ||
                !parseOptionalHookshotTextureField(hookshotTextures, "design", definition.hookshotDesignTextureAsset) ||
                !parseOptionalHookshotTextureField(hookshotTextures, "chain", definition.hookshotChainTextureAsset) ||
                !parseOptionalHookshotTextureField(hookshotTextures, "reticle",
                                                   definition.hookshotReticleTextureAsset)) {
                if (outError.rfind("items[", 0) != 0) {
                    outError = "items[" + std::to_string(i) + "].hookshotTextures." + outError;
                }
                return false;
            }
        }

        if (definition.modelAsset.empty() && !definition.modelTextureAsset.empty()) {
            outError = "items[" + std::to_string(i) + "].modelTextureAsset requires modelAsset";
            return false;
        }

        if (hasModelTextureWidth && definition.modelTextureAsset.empty()) {
            outError = "items[" + std::to_string(i) +
                       "].modelTextureWidth/modelTextureHeight requires modelTextureAsset";
            return false;
        }

        if (!definition.modelAsset.empty()) {
            const auto modelExtension = ToLower(std::filesystem::path(definition.modelAsset).extension().string());
            if (!modelExtension.empty() && modelExtension != ".obj" && modelExtension != ".otr" &&
                modelExtension != ".o2r") {
                outError =
                    "items[" + std::to_string(i) +
                    "].modelAsset extension must be .obj, .otr, .o2r, or a folder path without extension";
                return false;
            }

            if (modelExtension != ".obj" && !definition.modelTextureAsset.empty()) {
                outError = "items[" + std::to_string(i) + "].modelTextureAsset is only supported for .obj modelAsset";
                return false;
            }

            if (modelExtension != ".obj" && hasModelTextureWidth) {
                outError = "items[" + std::to_string(i) +
                           "].modelTextureWidth/modelTextureHeight is only supported for .obj modelAsset";
                return false;
            }
        }

        if (!definition.modelDisplayList.empty()) {
            if (definition.modelDisplayList.find('\0') != std::string::npos) {
                outError = "items[" + std::to_string(i) + "].modelDisplayList contains invalid characters";
                return false;
            }
        }

        if (HasHookshotTextureAssetOverrides(definition) && definition.slot != ExternalModItemSlot::Hookshot) {
            outError = "items[" + std::to_string(i) + "].hookshotTextures requires slot SLOT_HOOKSHOT";
            return false;
        }

        if (item.contains("ui")) {
            if (!item["ui"].is_object()) {
                outError = "items[" + std::to_string(i) + "].ui must be object";
                return false;
            }
            const auto& ui = item["ui"];
            if (ui.contains("displayName")) {
                if (!ui["displayName"].is_string()) {
                    outError = "items[" + std::to_string(i) + "].ui.displayName must be string";
                    return false;
                }
                definition.displayName = ui["displayName"].get<std::string>();
            }
            if (ui.contains("description")) {
                if (!ui["description"].is_string()) {
                    outError = "items[" + std::to_string(i) + "].ui.description must be string";
                    return false;
                }
                definition.description = ui["description"].get<std::string>();
            }
        }

        if (item.contains("agePolicy")) {
            if (!ParseItemAgePolicy(item["agePolicy"], definition.agePolicy, outError)) {
                outError = "items[" + std::to_string(i) + "].agePolicy " + outError;
                return false;
            }
        }

        if (item.contains("useMode")) {
            if (!ParseItemUseMode(item["useMode"], definition.useMode, outError)) {
                outError = "items[" + std::to_string(i) + "].useMode " + outError;
                return false;
            }
        }

        if (item.contains("behavior")) {
            if (!item["behavior"].is_object()) {
                outError = "items[" + std::to_string(i) + "].behavior must be object";
                return false;
            }

            const auto& behavior = item["behavior"];
            if (behavior.contains("exportOnUse")) {
                if (!behavior["exportOnUse"].is_string()) {
                    outError = "items[" + std::to_string(i) + "].behavior.exportOnUse must be string";
                    return false;
                }
                definition.onUseExport = behavior["exportOnUse"].get<std::string>();
            }

            if (behavior.contains("exportOnUpdate")) {
                if (!behavior["exportOnUpdate"].is_string()) {
                    outError = "items[" + std::to_string(i) + "].behavior.exportOnUpdate must be string";
                    return false;
                }
                definition.onUpdateExport = behavior["exportOnUpdate"].get<std::string>();
            }

            if (behavior.contains("onUseBehavior")) {
                if (!behavior["onUseBehavior"].is_string()) {
                    outError = "items[" + std::to_string(i) + "].behavior.onUseBehavior must be string";
                    return false;
                }
                definition.onUseBehavior = behavior["onUseBehavior"].get<std::string>();
            }

            if (behavior.contains("onEquipBehavior")) {
                if (!behavior["onEquipBehavior"].is_string()) {
                    outError = "items[" + std::to_string(i) + "].behavior.onEquipBehavior must be string";
                    return false;
                }
                definition.onEquipBehavior = behavior["onEquipBehavior"].get<std::string>();
            }
        }

        if (item.contains("onUseBehavior")) {
            if (!item["onUseBehavior"].is_string()) {
                outError = "items[" + std::to_string(i) + "].onUseBehavior must be string";
                return false;
            }
            definition.onUseBehavior = item["onUseBehavior"].get<std::string>();
        }
        if (item.contains("onEquipBehavior")) {
            if (!item["onEquipBehavior"].is_string()) {
                outError = "items[" + std::to_string(i) + "].onEquipBehavior must be string";
                return false;
            }
            definition.onEquipBehavior = item["onEquipBehavior"].get<std::string>();
        }
        if (item.contains("acquireTextId")) {
            if (!item["acquireTextId"].is_number_integer()) {
                outError = "items[" + std::to_string(i) + "].acquireTextId must be integer";
                return false;
            }
            definition.acquireTextId = item["acquireTextId"].get<int32_t>();
        }
        if (item.contains("persistentStateKey")) {
            if (!item["persistentStateKey"].is_string()) {
                outError = "items[" + std::to_string(i) + "].persistentStateKey must be string";
                return false;
            }
            definition.persistentStateKey = item["persistentStateKey"].get<std::string>();
        }

        if (item.contains("grant")) {
            if (!item["grant"].is_object()) {
                outError = "items[" + std::to_string(i) + "].grant must be object";
                return false;
            }

            const auto& grant = item["grant"];
            if (grant.contains("itemId")) {
                int32_t itemId = ITEM_NONE;
                if (!ParseItemIdValue(grant["itemId"], itemId, outError)) {
                    outError = "items[" + std::to_string(i) + "].grant.itemId " + outError;
                    return false;
                }
                definition.hasGrantItemId = true;
                definition.grantItemId = itemId;
            }

            if (grant.contains("ammo")) {
                if (!grant["ammo"].is_number_integer()) {
                    outError = "items[" + std::to_string(i) + "].grant.ammo must be integer";
                    return false;
                }
                definition.hasGrantAmmo = true;
                definition.grantAmmo = std::max(0, grant["ammo"].get<int32_t>());
            }
        }

        if (definition.slot == ExternalModItemSlot::Hookshot && definition.useMode != ExternalModItemUseMode::Vanilla &&
            definition.onUseExport.empty()) {
            outError = "items[" + std::to_string(i) + "].behavior.exportOnUse is required for SLOT_HOOKSHOT when useMode is override/augment";
            return false;
        }

        if (definition.slot != ExternalModItemSlot::Hookshot && definition.useMode == ExternalModItemUseMode::Override &&
            definition.onUseExport.empty()) {
            outError = "items[" + std::to_string(i) + "].behavior.exportOnUse is required for non-hookshot override useMode";
            return false;
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

bool ExternalModManager::TryParseHookDefinitions(const std::string& content, int32_t apiVersion,
                                                 std::vector<ExternalModHookSubscription>& outSubscriptions,
                                                 std::string& outError) {
    outSubscriptions.clear();

    if (apiVersion < kExternalModApiVersionV2) {
        outError = "hookDefinitions requires apiVersion 2";
        return false;
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("hooks.json parse error: ") + ex.what();
        return false;
    }

    const nlohmann::json* subscriptions = nullptr;
    if (json.is_array()) {
        subscriptions = &json;
    } else if (json.is_object() && json.contains("subscriptions") && json["subscriptions"].is_array()) {
        subscriptions = &json["subscriptions"];
    }

    if (subscriptions == nullptr) {
        outError = "hooks.json must be an array or object with subscriptions[]";
        return false;
    }

    std::unordered_set<std::string> seenIds;

    for (size_t i = 0; i < subscriptions->size(); ++i) {
        const auto& entry = (*subscriptions)[i];
        if (!entry.is_object()) {
            outError = "subscriptions[" + std::to_string(i) + "] must be object";
            return false;
        }

        ExternalModHookSubscription subscription;
        if (!ValidateRequiredString(entry, "id", subscription.id, outError)) {
            outError = "subscriptions[" + std::to_string(i) + "].id: " + outError;
            return false;
        }

        if (!seenIds.insert(subscription.id).second) {
            outError = "Duplicate hook subscription id: " + subscription.id;
            return false;
        }

        std::string hookName;
        if (!ValidateRequiredString(entry, "hook", hookName, outError)) {
            outError = "subscriptions[" + std::to_string(i) + "].hook: " + outError;
            return false;
        }
        if (!ParseHookTypeAlias(hookName, subscription.hook)) {
            outError = "subscriptions[" + std::to_string(i) + "].hook unsupported: " + hookName;
            return false;
        }

        if (entry.contains("dispatch")) {
            if (!ParseHookDispatchType(entry["dispatch"], subscription.dispatch, outError)) {
                outError = "subscriptions[" + std::to_string(i) + "].dispatch: " + outError;
                return false;
            }
        }

        subscription.cooldownFrames = 0;
        subscription.cooldownRemaining = 0;
        if (entry.contains("cooldownFrames")) {
            if (!entry["cooldownFrames"].is_number_integer()) {
                outError = "subscriptions[" + std::to_string(i) + "].cooldownFrames must be integer";
                return false;
            }
            subscription.cooldownFrames = std::max(0, entry["cooldownFrames"].get<int32_t>());
        }

        if (entry.contains("filters")) {
            if (!entry["filters"].is_object()) {
                outError = "subscriptions[" + std::to_string(i) + "].filters must be object";
                return false;
            }
            const auto& filters = entry["filters"];

            if (filters.contains("scene")) {
                if (!ParseAliasedInt16(filters["scene"], kSceneAliases, "filters.scene", subscription.filters.scene,
                                       outError)) {
                    outError = "subscriptions[" + std::to_string(i) + "]: " + outError;
                    return false;
                }
                subscription.filters.hasScene = true;
            }

            if (!ParseOptionalFilterInt16(filters, "actorId", subscription.filters.hasActorId, subscription.filters.actorId,
                                          outError) ||
                !ParseOptionalFilterInt16(filters, "category", subscription.filters.hasCategory,
                                          subscription.filters.category, outError) ||
                !ParseOptionalFilterInt16(filters, "itemId", subscription.filters.hasItemId, subscription.filters.itemId,
                                          outError) ||
                !ParseOptionalFilterInt16(filters, "flagType", subscription.filters.hasFlagType,
                                          subscription.filters.flagType, outError) ||
                !ParseOptionalFilterInt16(filters, "flagId", subscription.filters.hasFlagId, subscription.filters.flagId,
                                          outError)) {
                outError = "subscriptions[" + std::to_string(i) + "]: " + outError;
                return false;
            }

            if (filters.contains("healthDeltaRange")) {
                const auto& healthDeltaRange = filters["healthDeltaRange"];
                if (!healthDeltaRange.is_array() || healthDeltaRange.size() != 2 ||
                    !healthDeltaRange[0].is_number_integer() || !healthDeltaRange[1].is_number_integer()) {
                    outError = "subscriptions[" + std::to_string(i) + "].filters.healthDeltaRange must be [min,max] integers";
                    return false;
                }

                const auto minValue = healthDeltaRange[0].get<long long>();
                const auto maxValue = healthDeltaRange[1].get<long long>();
                if (minValue < std::numeric_limits<int16_t>::min() || minValue > std::numeric_limits<int16_t>::max() ||
                    maxValue < std::numeric_limits<int16_t>::min() || maxValue > std::numeric_limits<int16_t>::max() ||
                    minValue > maxValue) {
                    outError = "subscriptions[" + std::to_string(i) + "].filters.healthDeltaRange has invalid bounds";
                    return false;
                }

                subscription.filters.hasHealthDeltaRange = true;
                subscription.filters.healthDeltaMin = static_cast<int16_t>(minValue);
                subscription.filters.healthDeltaMax = static_cast<int16_t>(maxValue);
            }
        }

        if (subscription.dispatch == ExternalModHookDispatchType::Actions) {
            if (!entry.contains("actions")) {
                outError = "subscriptions[" + std::to_string(i) + "] requires actions for dispatch=actions";
                return false;
            }
            if (!ParseActionArray(entry["actions"], apiVersion, "actions", subscription.actions, outError)) {
                outError = "subscriptions[" + std::to_string(i) + "]: " + outError;
                return false;
            }
        } else {
            if (!ValidateRequiredString(entry, "wasmExport", subscription.wasmExport, outError)) {
                outError = "subscriptions[" + std::to_string(i) + "].wasmExport: " + outError;
                return false;
            }
        }

        outSubscriptions.push_back(std::move(subscription));
    }

    return true;
}
bool ExternalModManager::TryParseActorDefinitions(const std::string& content, int32_t apiVersion,
                                                  std::vector<ExternalModActorDefinition>& outDefinitions,
                                                  std::string& outError) {
    outDefinitions.clear();

    if (apiVersion < kExternalModApiVersionV2) {
        outError = "actorDefinitions requires apiVersion 2";
        return false;
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("actors.json parse error: ") + ex.what();
        return false;
    }

    const nlohmann::json* actors = nullptr;
    if (json.is_array()) {
        actors = &json;
    } else if (json.is_object() && json.contains("actors") && json["actors"].is_array()) {
        actors = &json["actors"];
    }

    if (actors == nullptr) {
        outError = "actors.json must be an array or object with actors[]";
        return false;
    }

    std::unordered_set<std::string> seenIds;
    for (size_t i = 0; i < actors->size(); ++i) {
        const auto& actor = (*actors)[i];
        if (!actor.is_object()) {
            outError = "actors[" + std::to_string(i) + "] must be an object";
            return false;
        }

        ExternalModActorDefinition definition;
        if (!ValidateRequiredString(actor, "id", definition.id, outError)) {
            outError = "actors[" + std::to_string(i) + "]: " + outError;
            return false;
        }
        if (!seenIds.insert(definition.id).second) {
            outError = "Duplicate actor id: " + definition.id;
            return false;
        }

        std::string archetype;
        if (!ValidateRequiredString(actor, "archetype", archetype, outError)) {
            outError = "actors[" + std::to_string(i) + "]: " + outError;
            return false;
        }
        const auto archetypeNormalized = ToLower(archetype);
        if (archetypeNormalized == "npc") {
            definition.archetype = ExternalModActorArchetype::Npc;
            definition.interactable = true;
            definition.interactDistance = 120.0f;
        } else if (archetypeNormalized == "prop") {
            definition.archetype = ExternalModActorArchetype::Prop;
        } else if (archetypeNormalized == "trigger") {
            definition.archetype = ExternalModActorArchetype::Trigger;
        } else {
            outError = "actors[" + std::to_string(i) + "].archetype must be npc|prop|trigger";
            return false;
        }

        if (!actor.contains("spawn") || !actor["spawn"].is_object()) {
            outError = "actors[" + std::to_string(i) + "].spawn must be object";
            return false;
        }
        const auto& spawn = actor["spawn"];
        if (!spawn.contains("scene") ||
            !ParseAliasedInt16(spawn["scene"], kSceneAliases, "spawn.scene", definition.sceneId, outError)) {
            outError = "actors[" + std::to_string(i) + "].spawn.scene: " + outError;
            return false;
        }
        if (!spawn.contains("position") ||
            !ParseVec3(spawn["position"], definition.posX, definition.posY, definition.posZ, "spawn.position", outError)) {
            outError = "actors[" + std::to_string(i) + "].spawn.position: " + outError;
            return false;
        }
        if (spawn.contains("rotation") &&
            !ParseVec3(spawn["rotation"], definition.rotX, definition.rotY, definition.rotZ, "spawn.rotation", outError)) {
            outError = "actors[" + std::to_string(i) + "].spawn.rotation: " + outError;
            return false;
        }

        if (actor.contains("limits")) {
            if (!actor["limits"].is_object()) {
                outError = "actors[" + std::to_string(i) + "].limits must be object";
                return false;
            }
            const auto& limits = actor["limits"];
            if (limits.contains("maxInstances")) {
                if (!limits["maxInstances"].is_number_integer()) {
                    outError = "actors[" + std::to_string(i) + "].limits.maxInstances must be integer";
                    return false;
                }
                definition.maxInstances = limits["maxInstances"].get<int32_t>();
            }
            if (limits.contains("tickRate")) {
                if (!limits["tickRate"].is_number_integer()) {
                    outError = "actors[" + std::to_string(i) + "].limits.tickRate must be integer";
                    return false;
                }
                definition.tickRate = limits["tickRate"].get<int32_t>();
            }
            if (limits.contains("lodDistance")) {
                if (!limits["lodDistance"].is_number()) {
                    outError = "actors[" + std::to_string(i) + "].limits.lodDistance must be numeric";
                    return false;
                }
                definition.lodDistance = limits["lodDistance"].get<float>();
            }
        }

        if (definition.maxInstances < 1 || definition.maxInstances > 512) {
            outError = "actors[" + std::to_string(i) + "].limits.maxInstances must be in [1, 512]";
            return false;
        }
        if (definition.tickRate < 1 || definition.tickRate > 600) {
            outError = "actors[" + std::to_string(i) + "].limits.tickRate must be in [1, 600]";
            return false;
        }
        if (definition.lodDistance <= 0.0f || definition.lodDistance > 100000.0f) {
            outError = "actors[" + std::to_string(i) + "].limits.lodDistance must be in (0, 100000]";
            return false;
        }

        if (actor.contains("behaviorId")) {
            if (!actor["behaviorId"].is_string()) {
                outError = "actors[" + std::to_string(i) + "].behaviorId must be string";
                return false;
            }
            definition.behaviorId = actor["behaviorId"].get<std::string>();
        }

        if (actor.contains("components")) {
            if (!actor["components"].is_array()) {
                outError = "actors[" + std::to_string(i) + "].components must be array";
                return false;
            }
            for (size_t componentIndex = 0; componentIndex < actor["components"].size(); ++componentIndex) {
                if (!actor["components"][componentIndex].is_string()) {
                    outError = "actors[" + std::to_string(i) + "].components[" + std::to_string(componentIndex) +
                               "] must be string";
                    return false;
                }
                const std::string component = actor["components"][componentIndex].get<std::string>();
                definition.components.push_back(component);
                if (ToLower(component) == "interactable") {
                    definition.interactable = true;
                }
            }
        }

        if (actor.contains("behavior")) {
            if (!actor["behavior"].is_object()) {
                outError = "actors[" + std::to_string(i) + "].behavior must be object";
                return false;
            }
            const auto& behavior = actor["behavior"];
            if (behavior.contains("exportOnInit") &&
                !ValidateRequiredString(behavior, "exportOnInit", definition.exportOnInit, outError)) {
                outError = "actors[" + std::to_string(i) + "].behavior.exportOnInit: " + outError;
                return false;
            }
            if (behavior.contains("exportOnUpdate") &&
                !ValidateRequiredString(behavior, "exportOnUpdate", definition.exportOnUpdate, outError)) {
                outError = "actors[" + std::to_string(i) + "].behavior.exportOnUpdate: " + outError;
                return false;
            }
            if (behavior.contains("exportOnInteract") &&
                !ValidateRequiredString(behavior, "exportOnInteract", definition.exportOnInteract, outError)) {
                outError = "actors[" + std::to_string(i) + "].behavior.exportOnInteract: " + outError;
                return false;
            }
            if (behavior.contains("exportOnDestroy") &&
                !ValidateRequiredString(behavior, "exportOnDestroy", definition.exportOnDestroy, outError)) {
                outError = "actors[" + std::to_string(i) + "].behavior.exportOnDestroy: " + outError;
                return false;
            }

            if (behavior.contains("id")) {
                if (!behavior["id"].is_string()) {
                    outError = "actors[" + std::to_string(i) + "].behavior.id must be string";
                    return false;
                }
                definition.behaviorId = behavior["id"].get<std::string>();
            }
            if (behavior.contains("interactable")) {
                if (!behavior["interactable"].is_boolean()) {
                    outError = "actors[" + std::to_string(i) + "].behavior.interactable must be boolean";
                    return false;
                }
                definition.interactable = behavior["interactable"].get<bool>();
            }
            if (behavior.contains("interactDistance")) {
                if (!behavior["interactDistance"].is_number()) {
                    outError = "actors[" + std::to_string(i) + "].behavior.interactDistance must be number";
                    return false;
                }
                definition.interactDistance = behavior["interactDistance"].get<float>();
            }
        }

        if (definition.interactDistance <= 0.0f || definition.interactDistance > 5000.0f) {
            outError = "actors[" + std::to_string(i) + "].behavior.interactDistance must be in (0, 5000]";
            return false;
        }

        outDefinitions.push_back(std::move(definition));
    }

    if (outDefinitions.empty()) {
        outError = "actors.json must define at least one actor";
        return false;
    }

    return true;
}

bool ExternalModManager::TryParseBehaviorDefinitions(const std::string& content, int32_t apiVersion,
                                                     std::vector<ExternalModBehaviorDefinition>& outDefinitions,
                                                     std::string& outError) {
    outDefinitions.clear();

    if (apiVersion < kExternalModApiVersionV2) {
        outError = "behaviorDefinitions requires apiVersion 2";
        return false;
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("behaviors.json parse error: ") + ex.what();
        return false;
    }

    const nlohmann::json* behaviors = nullptr;
    if (json.is_array()) {
        behaviors = &json;
    } else if (json.is_object() && json.contains("behaviors") && json["behaviors"].is_array()) {
        behaviors = &json["behaviors"];
    }

    if (behaviors == nullptr) {
        outError = "behaviors.json must be an array or object with behaviors[]";
        return false;
    }

    std::unordered_set<std::string> seenIds;
    for (size_t i = 0; i < behaviors->size(); ++i) {
        const auto& behavior = (*behaviors)[i];
        if (!behavior.is_object()) {
            outError = "behaviors[" + std::to_string(i) + "] must be object";
            return false;
        }

        ExternalModBehaviorDefinition definition;
        if (!ValidateRequiredString(behavior, "id", definition.id, outError)) {
            outError = "behaviors[" + std::to_string(i) + "].id: " + outError;
            return false;
        }
        if (!seenIds.insert(definition.id).second) {
            outError = "Duplicate behavior id: " + definition.id;
            return false;
        }

        if (!behavior.contains("events") || !behavior["events"].is_object()) {
            outError = "behaviors[" + std::to_string(i) + "].events must be object";
            return false;
        }

        for (const auto& [eventNameRaw, eventBody] : behavior["events"].items()) {
            if (!eventBody.is_array()) {
                outError = "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + " must be array";
                return false;
            }

            const std::string eventName = ToLower(eventNameRaw);
            std::vector<ExternalModBehaviorRule> rules;

            const bool looksLikeRuleArray =
                !eventBody.empty() && eventBody[0].is_object() &&
                (eventBody[0].contains("actions") || eventBody[0].contains("conditions") || eventBody[0].contains("chance"));

            if (looksLikeRuleArray) {
                for (size_t j = 0; j < eventBody.size(); ++j) {
                    const auto& ruleJson = eventBody[j];
                    if (!ruleJson.is_object()) {
                        outError = "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + "[" +
                                   std::to_string(j) + "] must be object";
                        return false;
                    }

                    ExternalModBehaviorRule rule;
                    if (ruleJson.contains("chance")) {
                        if (!ruleJson["chance"].is_number()) {
                            outError = "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + "[" +
                                       std::to_string(j) + "].chance must be number";
                            return false;
                        }
                        rule.randomChance = std::clamp(ruleJson["chance"].get<float>(), 0.0f, 1.0f);
                    }

                    if (ruleJson.contains("conditions")) {
                        if (!ruleJson["conditions"].is_array()) {
                            outError = "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + "[" +
                                       std::to_string(j) + "].conditions must be array";
                            return false;
                        }
                        for (size_t k = 0; k < ruleJson["conditions"].size(); ++k) {
                            const auto& conditionJson = ruleJson["conditions"][k];
                            if (!conditionJson.is_object()) {
                                outError = "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + "[" +
                                           std::to_string(j) + "].conditions[" + std::to_string(k) + "] must be object";
                                return false;
                            }

                            ExternalModBehaviorCondition condition;
                            if (conditionJson.contains("condition")) {
                                if (!conditionJson["condition"].is_string()) {
                                    outError = "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + "[" +
                                               std::to_string(j) + "].conditions[" + std::to_string(k) +
                                               "].condition must be string";
                                    return false;
                                }
                                condition.type = conditionJson["condition"].get<std::string>();
                            } else if (conditionJson.contains("type")) {
                                if (!conditionJson["type"].is_string()) {
                                    outError = "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + "[" +
                                               std::to_string(j) + "].conditions[" + std::to_string(k) +
                                               "].type must be string";
                                    return false;
                                }
                                condition.type = conditionJson["type"].get<std::string>();
                            } else {
                                outError = "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + "[" +
                                           std::to_string(j) + "].conditions[" + std::to_string(k) +
                                           "] requires condition";
                                return false;
                            }

                            if (conditionJson.contains("scope")) {
                                if (!conditionJson["scope"].is_string()) {
                                    outError = "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + "[" +
                                               std::to_string(j) + "].conditions[" + std::to_string(k) +
                                               "].scope must be string";
                                    return false;
                                }
                                condition.scope = conditionJson["scope"].get<std::string>();
                            }
                            if (conditionJson.contains("key")) {
                                if (!conditionJson["key"].is_string()) {
                                    outError = "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + "[" +
                                               std::to_string(j) + "].conditions[" + std::to_string(k) +
                                               "].key must be string";
                                    return false;
                                }
                                condition.key = conditionJson["key"].get<std::string>();
                            }
                            if (conditionJson.contains("op")) {
                                if (!conditionJson["op"].is_string()) {
                                    outError = "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + "[" +
                                               std::to_string(j) + "].conditions[" + std::to_string(k) +
                                               "].op must be string";
                                    return false;
                                }
                                condition.op = conditionJson["op"].get<std::string>();
                            }
                            if (conditionJson.contains("value")) {
                                if (conditionJson["value"].is_string()) {
                                    condition.value = conditionJson["value"].get<std::string>();
                                } else if (conditionJson["value"].is_boolean()) {
                                    condition.value = conditionJson["value"].get<bool>() ? "true" : "false";
                                } else if (conditionJson["value"].is_number()) {
                                    condition.numberValue = conditionJson["value"].get<float>();
                                    condition.hasNumberValue = true;
                                    condition.value = std::to_string(condition.numberValue);
                                } else {
                                    outError = "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + "[" +
                                               std::to_string(j) + "].conditions[" + std::to_string(k) +
                                               "].value must be string|number|boolean";
                                    return false;
                                }
                            }
                            if (conditionJson.contains("number")) {
                                if (!conditionJson["number"].is_number()) {
                                    outError = "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + "[" +
                                               std::to_string(j) + "].conditions[" + std::to_string(k) +
                                               "].number must be number";
                                    return false;
                                }
                                condition.numberValue = conditionJson["number"].get<float>();
                                condition.hasNumberValue = true;
                            }
                            if (condition.hasNumberValue && condition.value.empty()) {
                                condition.value = std::to_string(condition.numberValue);
                            }

                            rule.conditions.push_back(std::move(condition));
                        }
                    }

                    if (!ruleJson.contains("actions")) {
                        outError = "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + "[" +
                                   std::to_string(j) + "] requires actions";
                        return false;
                    }
                    if (!ParseActionArray(ruleJson["actions"], apiVersion, "actions", rule.actions, outError)) {
                        outError = "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + "[" +
                                   std::to_string(j) + "]: " + outError;
                        return false;
                    }
                    rules.push_back(std::move(rule));
                }
            } else {
                ExternalModBehaviorRule singleRule;
                if (!ParseActionArray(eventBody, apiVersion, "actions", singleRule.actions, outError)) {
                    outError =
                        "behaviors[" + std::to_string(i) + "].events." + eventNameRaw + ": " + outError;
                    return false;
                }
                rules.push_back(std::move(singleRule));
            }

            if (!rules.empty()) {
                definition.events[eventName] = std::move(rules);
            }
        }

        if (definition.events.empty()) {
            outError = "behaviors[" + std::to_string(i) + "] must define at least one event";
            return false;
        }
        outDefinitions.push_back(std::move(definition));
    }

    return true;
}

bool ExternalModManager::TryParseSceneDefinitions(const std::string& content, int32_t apiVersion,
                                                  std::vector<ExternalModSceneDefinition>& outDefinitions,
                                                  std::string& outError) {
    outDefinitions.clear();

    if (apiVersion < kExternalModApiVersionV2) {
        outError = "sceneDefinitions requires apiVersion 2";
        return false;
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("scenes.json parse error: ") + ex.what();
        return false;
    }

    const nlohmann::json* scenes = nullptr;
    if (json.is_array()) {
        scenes = &json;
    } else if (json.is_object() && json.contains("scenes") && json["scenes"].is_array()) {
        scenes = &json["scenes"];
    }
    if (scenes == nullptr) {
        outError = "scenes.json must be an array or object with scenes[]";
        return false;
    }

    std::unordered_set<std::string> seenIds;
    for (size_t i = 0; i < scenes->size(); ++i) {
        const auto& scene = (*scenes)[i];
        if (!scene.is_object()) {
            outError = "scenes[" + std::to_string(i) + "] must be object";
            return false;
        }

        ExternalModSceneDefinition definition;
        if (!ValidateRequiredString(scene, "id", definition.id, outError)) {
            outError = "scenes[" + std::to_string(i) + "].id: " + outError;
            return false;
        }
        if (!seenIds.insert(definition.id).second) {
            outError = "Duplicate scene id: " + definition.id;
            return false;
        }

        const char* entranceKey =
            scene.contains("entrance") ? "entrance" : (scene.contains("fallbackEntrance") ? "fallbackEntrance" : nullptr);
        if (entranceKey != nullptr) {
            if (!ParseAliasedInt16(scene[entranceKey], kEntranceAliases, entranceKey, definition.entranceIndex, outError)) {
                outError = "scenes[" + std::to_string(i) + "]." + entranceKey + ": " + outError;
                return false;
            }
            definition.hasEntrance = true;
        }

        outDefinitions.push_back(std::move(definition));
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
    runtime.frameBudgetMs = package.manifest.runtimeMaxFrameBudgetMs;
    runtime.maxHookCallsPerFrame = package.manifest.runtimeMaxHookCallsPerFrame;
    runtime.hookCallsThisFrame = 0;
    runtime.maxActorInstances = package.manifest.runtimeMaxActorInstances;

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

        for (size_t i = 0; i < runtime.itemDefinitions.size(); ++i) {
            auto& definition = runtime.itemDefinitions[i];
            definition.sourceModId = package.manifest.id;
            if (!definition.iconAsset.empty()) {
                std::filesystem::path iconPath;
                if (!IsSafePackageRelativePath(definition.iconAsset, iconPath, outError)) {
                    outError = "items[" + std::to_string(i) + "].iconAsset " + outError;
                    return false;
                }

                if (ToLower(iconPath.extension().string()) != ".png") {
                    outError = "items[" + std::to_string(i) + "].iconAsset must point to a .png file";
                    return false;
                }

                std::vector<uint8_t> iconBytes;
                if (!ReadBinaryFromPackage(package, iconPath, kMaxItemIconBytes, iconBytes, outError)) {
                    outError = "items[" + std::to_string(i) + "].iconAsset read failed: " + outError;
                    return false;
                }

                if (!TryDecodeItemIconPng(iconBytes, definition.iconRgba32, outError)) {
                    outError = "items[" + std::to_string(i) + "].iconAsset decode failed: " + outError;
                    return false;
                }
            }

            if (!definition.modelAsset.empty()) {
                std::filesystem::path modelPath;
                if (!IsSafePackageRelativePath(definition.modelAsset, modelPath, outError)) {
                    outError = "items[" + std::to_string(i) + "].modelAsset " + outError;
                    return false;
                }

                auto convertFast64SourceModel = [&](const std::string& modelIncContent,
                                                    const std::filesystem::path& objectPathHint) -> bool {
                    std::string conversionError;
                    Fast64ConversionOutput conversion;
                    const std::string objectName =
                        DeriveObjectNameFromPath(objectPathHint, definition.id.empty() ? "external_model" : definition.id);
                    auto resolveFast64TextureInclude = [&](const std::string& includePath, const Fast64TextureMeta& meta,
                                                           std::vector<uint8_t>& outBytes,
                                                           int32_t& outDecodedWidth, int32_t& outDecodedHeight,
                                                           std::string& resolveError) -> bool {
                        outBytes.clear();
                        outDecodedWidth = 0;
                        outDecodedHeight = 0;
                        std::vector<std::filesystem::path> candidates;
                        std::unordered_set<std::string> seen;

                        auto addCandidate = [&](const std::filesystem::path& candidate) {
                            if (candidate.empty()) {
                                return;
                            }
                            const auto normalized = candidate.lexically_normal();
                            const auto key = normalized.generic_string();
                            if (!key.empty() && seen.insert(key).second) {
                                candidates.push_back(normalized);
                            }
                        };
                        auto addPngVariants = [&](const std::filesystem::path& candidate) {
                            addCandidate(candidate);
                            std::string raw = candidate.generic_string();
                            if (EndsWithString(raw, ".inc.c")) {
                                addCandidate(std::filesystem::path(raw.substr(0, raw.size() - 6) + ".png"));
                            }
                        };

                        const std::filesystem::path includeRelativePath(includePath);
                        addPngVariants(includeRelativePath);
                        addPngVariants(objectPathHint / includeRelativePath);
                        addPngVariants(objectPathHint / includeRelativePath.filename());

                        std::string lastReadError;
                        for (const auto& candidate : candidates) {
                            if (ToLower(candidate.extension().string()) != ".png") {
                                continue;
                            }

                            std::filesystem::path safeCandidate;
                            std::string safeError;
                            if (!IsSafePackageRelativePath(candidate.generic_string(), safeCandidate, safeError)) {
                                continue;
                            }

                            std::vector<uint8_t> pngBytes;
                            std::string readError;
                            if (!ReadBinaryFromPackage(package, safeCandidate, kMaxItemModelTextureBytes, pngBytes,
                                                       readError)) {
                                lastReadError = readError;
                                continue;
                            }

                            std::vector<uint8_t> decodedRgba32;
                            int32_t decodedWidth = 0;
                            int32_t decodedHeight = 0;
                            if (!TryDecodePngToRgba32(pngBytes, decodedRgba32, decodedWidth, decodedHeight, resolveError)) {
                                resolveError = "failed decoding " + safeCandidate.generic_string() + ": " + resolveError;
                                return false;
                            }

                            const std::string normalizedSize = NormalizeTextureSizeToken(meta.size);
                            if (meta.format == "G_IM_FMT_RGBA" && normalizedSize == "G_IM_SIZ_32b") {
                                outDecodedWidth = decodedWidth;
                                outDecodedHeight = decodedHeight;
                                outBytes = std::move(decodedRgba32);
                                return true;
                            }
                            if (meta.format == "G_IM_FMT_RGBA" && normalizedSize == "G_IM_SIZ_16b") {
                                outDecodedWidth = decodedWidth;
                                outDecodedHeight = decodedHeight;
                                ConvertRgba32ToRgba16(decodedRgba32, outBytes);
                                return true;
                            }
                            if (meta.format == "G_IM_FMT_I" && normalizedSize == "G_IM_SIZ_8b") {
                                outDecodedWidth = decodedWidth;
                                outDecodedHeight = decodedHeight;
                                ConvertRgba32ToI8(decodedRgba32, outBytes);
                                return true;
                            }

                            resolveError =
                                "unsupported include texture format/size: " + meta.format + "|" + normalizedSize;
                            return false;
                        }

                        resolveError = "png not found for include " + includePath;
                        if (!lastReadError.empty()) {
                            resolveError += " (" + lastReadError + ")";
                        }
                        return false;
                    };
                    SPDLOG_INFO("[ExternalMods] Converting Fast64 source for {}.{} (object={}, bytes={})",
                                package.manifest.id, definition.id, objectName, modelIncContent.size());
                    if (!ConvertFast64SourceToResources(modelIncContent, objectName, resolveFast64TextureInclude,
                                                        definition.modelTextureFilter,
                                                        package.manifest.id + "." + definition.id, conversion,
                                                        conversionError)) {
                        outError = "items[" + std::to_string(i) + "].modelAsset conversion failed: " + conversionError;
                        return false;
                    }

                    const auto generatedArchivePath =
                        BuildGeneratedFast64ArchivePath(package.manifest.id, definition.id, objectName).lexically_normal();
                    auto generatedArchive =
                        CreateGeneratedFast64Archive(generatedArchivePath, std::move(conversion.resources), conversionError);
                    if (generatedArchive == nullptr) {
                        outError =
                            "items[" + std::to_string(i) + "].modelAsset conversion archive failed: " + conversionError;
                        return false;
                    }

                    runtime.generatedAssetArchives.push_back(generatedArchive);
                    if (definition.modelDisplayList.empty()) {
                        definition.modelDisplayList = conversion.rootDisplayListPath;
                    }
                    SPDLOG_INFO(
                        "[ExternalMods] Converted Fast64 model for {}.{} -> {} (memory archive, rootDL={}, vtxArrays={}, dlArrays={}, textures={})",
                        package.manifest.id, definition.id, generatedArchivePath.generic_string(),
                        conversion.rootDisplayListPath, conversion.vertexArrayCount, conversion.displayListArrayCount,
                        conversion.textureCount);
                    return true;
                };

                const auto modelExtension = ToLower(modelPath.extension().string());
                if (modelExtension == ".obj") {
                    std::string modelContent;
                    if (!ReadFileFromPackage(package, modelPath, kMaxItemModelBytes, modelContent, outError)) {
                        outError = "items[" + std::to_string(i) + "].modelAsset read failed: " + outError;
                        return false;
                    }
                    int32_t modelTextureWidth = kItemModelTextureSize;
                    int32_t modelTextureHeight = kItemModelTextureSize;
                    definition.modelTextureWidth = 0;
                    definition.modelTextureHeight = 0;
                    definition.modelTextureHasTransparency = false;
                    std::string resolvedModelTextureAsset = definition.modelTextureAsset;
                    std::vector<uint8_t> modelTextureRgba32ForUvHeuristic;

                    if (resolvedModelTextureAsset.empty()) {
                        std::string mtlPathFromObj;
                        if (TryExtractObjMaterialLibraryPath(modelContent, mtlPathFromObj)) {
                            const std::filesystem::path unresolvedMtlPath = modelPath.parent_path() / mtlPathFromObj;
                            std::filesystem::path mtlPath;
                            std::string resolveError;
                            if (IsSafePackageRelativePath(unresolvedMtlPath.generic_string(), mtlPath, resolveError)) {
                                std::string mtlContent;
                                if (ReadFileFromPackage(package, mtlPath, kMaxObjMaterialBytes, mtlContent, resolveError)) {
                                    std::string mapKdTexturePath;
                                    if (TryExtractMtlDiffuseTexturePath(mtlContent, mapKdTexturePath)) {
                                        const std::filesystem::path unresolvedTexturePath = mtlPath.parent_path() / mapKdTexturePath;
                                        std::filesystem::path texturePath;
                                        if (IsSafePackageRelativePath(unresolvedTexturePath.generic_string(), texturePath,
                                                                      resolveError) &&
                                            ToLower(texturePath.extension().string()) == ".png") {
                                            resolvedModelTextureAsset = texturePath.generic_string();
                                            definition.modelTextureAsset = resolvedModelTextureAsset;
                                            SPDLOG_INFO(
                                                "[ExternalMods] Resolved OBJ material texture for {}.{}: {}",
                                                package.manifest.id, definition.id, resolvedModelTextureAsset);
                                        } else {
                                            SPDLOG_WARN(
                                                "[ExternalMods] Ignored OBJ material texture for {}.{}: {}",
                                                package.manifest.id, definition.id,
                                                resolveError.empty() ? "unsupported extension (expected .png)" :
                                                                       resolveError);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (!resolvedModelTextureAsset.empty()) {
                        std::filesystem::path modelTexturePath;
                        if (!IsSafePackageRelativePath(resolvedModelTextureAsset, modelTexturePath, outError)) {
                            outError = "items[" + std::to_string(i) + "].modelTextureAsset " + outError;
                            return false;
                        }

                        if (ToLower(modelTexturePath.extension().string()) != ".png") {
                            outError = "items[" + std::to_string(i) + "].modelTextureAsset must point to a .png file";
                            return false;
                        }

                        std::vector<uint8_t> modelTextureBytes;
                        if (!ReadBinaryFromPackage(package, modelTexturePath, kMaxItemModelTextureBytes, modelTextureBytes,
                                                   outError)) {
                            outError = "items[" + std::to_string(i) + "].modelTextureAsset read failed: " + outError;
                            return false;
                        }

                        std::vector<uint8_t> decodedModelTextureRgba32;
                        int32_t decodedModelTextureWidth = 0;
                        int32_t decodedModelTextureHeight = 0;
                        if (!TryDecodePngToRgba32(modelTextureBytes, decodedModelTextureRgba32, decodedModelTextureWidth,
                                                  decodedModelTextureHeight, outError)) {
                            outError = "items[" + std::to_string(i) + "].modelTextureAsset decode failed: " + outError;
                            return false;
                        }

                        bool resizedTexture = false;
                        int32_t effectiveModelTextureWidth = decodedModelTextureWidth;
                        int32_t effectiveModelTextureHeight = decodedModelTextureHeight;
                        if (definition.modelTextureTargetWidth > 0 && definition.modelTextureTargetHeight > 0 &&
                            (definition.modelTextureTargetWidth != decodedModelTextureWidth ||
                             definition.modelTextureTargetHeight != decodedModelTextureHeight)) {
                            std::vector<uint8_t> resizedRgba32;
                            ResizeRgba32Nearest(decodedModelTextureRgba32, decodedModelTextureWidth,
                                                decodedModelTextureHeight, definition.modelTextureTargetWidth,
                                                definition.modelTextureTargetHeight, resizedRgba32);
                            decodedModelTextureRgba32 = std::move(resizedRgba32);
                            effectiveModelTextureWidth = definition.modelTextureTargetWidth;
                            effectiveModelTextureHeight = definition.modelTextureTargetHeight;
                            resizedTexture = true;
                        }

                        modelTextureRgba32ForUvHeuristic = decodedModelTextureRgba32;
                        definition.modelTextureHasTransparency =
                            TextureHasMixedAlphaCoverage(modelTextureRgba32ForUvHeuristic, effectiveModelTextureWidth,
                                                         effectiveModelTextureHeight);
                        const auto effectiveFilter =
                            ResolveModelTextureFilter(definition.modelTextureFilter, definition.modelTextureHasTransparency);
                        const bool useAggressiveOpaqueFill = false;
                        NormalizeModelTextureForOpaqueRendering(decodedModelTextureRgba32, effectiveModelTextureWidth,
                                                                effectiveModelTextureHeight, useAggressiveOpaqueFill);
                        ConvertRgba32ToRgba16(decodedModelTextureRgba32, definition.modelTextureRgba32);

                        definition.modelTextureWidth = effectiveModelTextureWidth;
                        definition.modelTextureHeight = effectiveModelTextureHeight;
                        modelTextureWidth = effectiveModelTextureWidth;
                        modelTextureHeight = effectiveModelTextureHeight;
                        SPDLOG_INFO("[ExternalMods] Loaded custom model texture for {}.{}: {}x{} from {}", package.manifest.id,
                                    definition.id, definition.modelTextureWidth, definition.modelTextureHeight,
                                    modelTexturePath.generic_string());
                        SPDLOG_INFO(
                            "[ExternalMods] Custom model texture settings for {}.{}: filter={} (configured={}, mixedAlpha={}), "
                            "effectiveSize={}x{}, resized={}, opaqueFill={}",
                            package.manifest.id, definition.id, GetModelTextureFilterName(effectiveFilter),
                            GetModelTextureFilterName(definition.modelTextureFilter),
                            definition.modelTextureHasTransparency ? "true" : "false", definition.modelTextureWidth,
                            definition.modelTextureHeight, resizedTexture ? "true" : "false",
                            useAggressiveOpaqueFill ? "aggressive" : "local");
                    }

                    if (!TryParseObjCustomModel(modelContent, definition.modelScale, modelTextureWidth,
                                                modelTextureHeight, definition.modelUvOrigin,
                                                modelTextureRgba32ForUvHeuristic.empty() ? nullptr :
                                                                                            &modelTextureRgba32ForUvHeuristic,
                                                package.manifest.id, definition.id, definition.customModelTriangles,
                                                outError)) {
                        outError = "items[" + std::to_string(i) + "].modelAsset parse failed: " + outError;
                        return false;
                    }
                } else if (modelExtension.empty()) {
                    if (!definition.modelTextureAsset.empty()) {
                        outError = "items[" + std::to_string(i) +
                                   "].modelTextureAsset is only supported for .obj modelAsset";
                        return false;
                    }

                    std::filesystem::path modelIncPath;
                    if (!IsSafePackageRelativePath((modelPath / "model.inc.c").generic_string(), modelIncPath, outError)) {
                        outError = "items[" + std::to_string(i) + "].modelAsset invalid model.inc.c path: " + outError;
                        return false;
                    }

                    std::filesystem::path headerPath;
                    if (!IsSafePackageRelativePath((modelPath / "header.h").generic_string(), headerPath, outError)) {
                        outError = "items[" + std::to_string(i) + "].modelAsset invalid header.h path: " + outError;
                        return false;
                    }

                    std::string modelIncContent;
                    if (!ReadFileFromPackage(package, modelIncPath, kMaxItemModelBytes, modelIncContent, outError)) {
                        outError = "items[" + std::to_string(i) + "].modelAsset missing model.inc.c: " + outError;
                        return false;
                    }

                    std::string headerContent;
                    if (!ReadFileFromPackage(package, headerPath, kMaxItemModelBytes, headerContent, outError)) {
                        outError = "items[" + std::to_string(i) + "].modelAsset missing header.h: " + outError;
                        return false;
                    }
                    (void)headerContent;

                    if (!convertFast64SourceModel(modelIncContent, modelPath)) {
                        return false;
                    }
                } else if (HasSupportedArchiveExtension(modelPath)) {
                    if (!definition.modelTextureAsset.empty()) {
                        outError = "items[" + std::to_string(i) +
                                   "].modelTextureAsset is only supported for .obj modelAsset";
                        return false;
                    }

                    std::vector<uint8_t> archiveBytes;
                    std::string archiveReadError;
                    if (!ReadBinaryFromPackage(package, modelPath, kMaxAssetBytes, archiveBytes, archiveReadError)) {
                        outError = "items[" + std::to_string(i) + "].modelAsset read failed: " + archiveReadError;
                        return false;
                    }

                    Fast64ArchiveInspection inspection;
                    std::string inspectError;
                    const bool inspected = InspectFast64ArchiveBytes(archiveBytes, inspection, inspectError);
                    if (!inspected && definition.modelDisplayList.empty()) {
                        outError = "items[" + std::to_string(i) +
                                   "].modelAsset inspection failed (set modelDisplayList or use Fast64 source folder): " +
                                   inspectError;
                        return false;
                    }

                    bool convertedFromSourceArchive = false;
                    if (inspected) {
                        if (definition.modelDisplayList.empty() && !inspection.rootDisplayListPath.empty()) {
                            definition.modelDisplayList = inspection.rootDisplayListPath;
                        }

                        if (inspection.hasFast64Source && !inspection.hasDisplayListResources) {
                            std::filesystem::path objectHint =
                                inspection.objectNameHint.empty() ? modelPath.stem() : std::filesystem::path(inspection.objectNameHint);
                            if (!convertFast64SourceModel(inspection.modelIncContent, objectHint)) {
                                return false;
                            }
                            convertedFromSourceArchive = true;
                        }
                    } else {
                        SPDLOG_WARN("[ExternalMods] Failed to inspect model archive for {}.{} ({}). Falling back to "
                                    "configured modelDisplayList",
                                    package.manifest.id, definition.id, inspectError);
                    }

                    if (!convertedFromSourceArchive) {
                        if (definition.modelDisplayList.empty()) {
                            outError = "items[" + std::to_string(i) +
                                       "].modelDisplayList is required when modelAsset archive has no detectable display list";
                            return false;
                        }

                        const auto normalizedArchivePath = modelPath.generic_string();
                        const auto alreadyListed =
                            std::find(package.manifest.assets.begin(), package.manifest.assets.end(), normalizedArchivePath) !=
                            package.manifest.assets.end();
                        if (!alreadyListed) {
                            package.manifest.assets.push_back(normalizedArchivePath);
                        }
                    }
                } else {
                    outError = "items[" + std::to_string(i) +
                               "].modelAsset must point to .obj, .otr/.o2r, or a folder with model.inc.c + header.h";
                    return false;
                }
            }

            if (HasHookshotTextureAssetOverrides(definition)) {
                auto decodeHookshotTexture = [&](const std::string& assetPath, const char* fieldName, int32_t targetWidth,
                                                 int32_t targetHeight, std::vector<uint8_t>& outRgba32) -> bool {
                    std::filesystem::path texturePath;
                    if (!IsSafePackageRelativePath(assetPath, texturePath, outError)) {
                        outError = "items[" + std::to_string(i) + "]." + fieldName + " " + outError;
                        return false;
                    }

                    if (ToLower(texturePath.extension().string()) != ".png") {
                        outError = "items[" + std::to_string(i) + "]." + fieldName + " must point to a .png file";
                        return false;
                    }

                    std::vector<uint8_t> textureBytes;
                    if (!ReadBinaryFromPackage(package, texturePath, kMaxHookshotTextureBytes, textureBytes, outError)) {
                        outError = "items[" + std::to_string(i) + "]." + fieldName + " read failed: " + outError;
                        return false;
                    }

                    if (!TryDecodePngToRgba32FixedSize(textureBytes, targetWidth, targetHeight, outRgba32, outError)) {
                        outError = "items[" + std::to_string(i) + "]." + fieldName + " decode failed: " + outError;
                        return false;
                    }

                    return true;
                };

                if (!definition.hookshotMetalTextureAsset.empty()) {
                    std::vector<uint8_t> rgba32;
                    if (!decodeHookshotTexture(definition.hookshotMetalTextureAsset, "hookshotMetalTextureAsset",
                                               kHookshotMetalTextureWidth, kHookshotMetalTextureHeight, rgba32)) {
                        return false;
                    }
                    ConvertRgba32ToRgba16(rgba32, definition.hookshotMetalTextureRgba16);
                }

                if (!definition.hookshotHandleTextureAsset.empty()) {
                    std::vector<uint8_t> rgba32;
                    if (!decodeHookshotTexture(definition.hookshotHandleTextureAsset, "hookshotHandleTextureAsset",
                                               kHookshotHandleTextureWidth, kHookshotHandleTextureHeight, rgba32)) {
                        return false;
                    }
                    ConvertRgba32ToCi8AndTlut(rgba32, definition.hookshotHandleTextureCi8,
                                              definition.hookshotHandleTextureTlutRgba16);
                }

                if (!definition.hookshotDesignTextureAsset.empty()) {
                    std::vector<uint8_t> rgba32;
                    if (!decodeHookshotTexture(definition.hookshotDesignTextureAsset, "hookshotDesignTextureAsset",
                                               kHookshotDesignTextureWidth, kHookshotDesignTextureHeight, rgba32)) {
                        return false;
                    }
                    ConvertRgba32ToCi8AndTlut(rgba32, definition.hookshotDesignTextureCi8,
                                              definition.hookshotDesignTextureTlutRgba16);
                }

                if (!definition.hookshotChainTextureAsset.empty()) {
                    std::vector<uint8_t> rgba32;
                    if (!decodeHookshotTexture(definition.hookshotChainTextureAsset, "hookshotChainTextureAsset",
                                               kHookshotChainTextureWidth, kHookshotChainTextureHeight, rgba32)) {
                        return false;
                    }
                    ConvertRgba32ToRgba16(rgba32, definition.hookshotChainTextureRgba16);
                }

                if (!definition.hookshotReticleTextureAsset.empty()) {
                    std::vector<uint8_t> rgba32;
                    if (!decodeHookshotTexture(definition.hookshotReticleTextureAsset, "hookshotReticleTextureAsset",
                                               kHookshotReticleTextureWidth, kHookshotReticleTextureHeight, rgba32)) {
                        return false;
                    }
                    ConvertRgba32ToI8(rgba32, definition.hookshotReticleTextureI8);
                }
            }
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

        if (ManifestHasCapability(package.manifest, "hooks.extended.v1")) {
            std::filesystem::path hooksPath;
            if (!IsSafePackageRelativePath(package.manifest.hookDefinitions, hooksPath, outError)) {
                outError = "Invalid hookDefinitions: " + outError;
                return false;
            }

            std::string hooksContent;
            if (!ReadFileFromPackage(package, hooksPath, kMaxHookDefinitionBytes, hooksContent, outError)) {
                outError = "Failed to read hookDefinitions: " + outError;
                return false;
            }

            if (!TryParseHookDefinitions(hooksContent, runtime.apiVersion, runtime.hookSubscriptions, outError)) {
                return false;
            }
        }

        if (ManifestHasCapability(package.manifest, "actors.vm.v1") ||
            ManifestHasCapability(package.manifest, "actors.generic.v1")) {
            std::filesystem::path actorsPath;
            if (!IsSafePackageRelativePath(package.manifest.actorDefinitions, actorsPath, outError)) {
                outError = "Invalid actorDefinitions: " + outError;
                return false;
            }

            std::string actorsContent;
            if (!ReadFileFromPackage(package, actorsPath, kMaxActorDefinitionBytes, actorsContent, outError)) {
                outError = "Failed to read actorDefinitions: " + outError;
                return false;
            }

            if (!TryParseActorDefinitions(actorsContent, runtime.apiVersion, runtime.actorDefinitions, outError)) {
                return false;
            }
        }

        if (ManifestHasCapability(package.manifest, "behaviors.graph.v1")) {
            std::filesystem::path behaviorPath;
            if (!IsSafePackageRelativePath(package.manifest.behaviorDefinitions, behaviorPath, outError)) {
                outError = "Invalid behaviorDefinitions: " + outError;
                return false;
            }

            std::string behaviorContent;
            if (!ReadFileFromPackage(package, behaviorPath, kMaxScriptBytes, behaviorContent, outError)) {
                outError = "Failed to read behaviorDefinitions: " + outError;
                return false;
            }

            if (!TryParseBehaviorDefinitions(behaviorContent, runtime.apiVersion, runtime.behaviorDefinitions, outError)) {
                return false;
            }
        }

        if (ManifestHasCapability(package.manifest, "scenes.bundle.v1")) {
            std::filesystem::path scenePath;
            if (!IsSafePackageRelativePath(package.manifest.sceneDefinitions, scenePath, outError)) {
                outError = "Invalid sceneDefinitions: " + outError;
                return false;
            }

            std::string sceneContent;
            if (!ReadFileFromPackage(package, scenePath, kMaxScriptBytes, sceneContent, outError)) {
                outError = "Failed to read sceneDefinitions: " + outError;
                return false;
            }

            if (!TryParseSceneDefinitions(sceneContent, runtime.apiVersion, runtime.sceneDefinitions, outError)) {
                return false;
            }
        }

        if (package.manifest.runtimeType != "wasm3-v1") {
            outError = "Unsupported runtime.type for apiVersion 2: " + package.manifest.runtimeType;
            return false;
        }

        std::filesystem::path runtimeModulePath;
        if (!IsSafePackageRelativePath(package.manifest.runtimeModule, runtimeModulePath, outError)) {
            outError = "Invalid runtime.module: " + outError;
            return false;
        }
        if (!IsSupportedRuntimeModuleExtension(runtimeModulePath)) {
            outError = "Invalid runtime.module extension (expected .wasm or .wat)";
            return false;
        }

        runtime.moduleSourcePath = runtimeModulePath.generic_string();
        runtime.moduleFormat = ResolveRuntimeModuleFormat(runtimeModulePath);
        runtime.moduleCompileTimeMs = 0;
        runtime.moduleCompileDiagnostics.clear();
        runtime.compiledModuleSizeBytes = 0;

        std::vector<uint8_t> wasmBytes;
        if (runtime.moduleFormat == ExternalModRuntimeModuleFormat::WatText) {
            std::string watContent;
            if (!ReadFileFromPackage(package, runtimeModulePath, kMaxWatSourceBytes, watContent, outError)) {
                outError = "Failed to read runtime.module: " + outError;
                return false;
            }

            int32_t compileMs = 0;
            std::string compileDiagnostics;
            if (!CompileWatToWasm(watContent, runtime.moduleSourcePath, wasmBytes, compileMs, compileDiagnostics)) {
                runtime.moduleCompileDiagnostics = compileDiagnostics;
                outError = "Failed to compile runtime.module (.wat): " + compileDiagnostics;
                return false;
            }

            if (wasmBytes.size() > kMaxWasmBytes) {
                outError = "Compiled wasm exceeds max module size";
                return false;
            }

            runtime.moduleCompileTimeMs = compileMs;
            runtime.moduleCompileDiagnostics = compileDiagnostics;
        } else {
            if (!ReadBinaryFromPackage(package, runtimeModulePath, kMaxWasmBytes, wasmBytes, outError)) {
                outError = "Failed to read runtime.module: " + outError;
                return false;
            }
        }

        runtime.compiledModuleSizeBytes = wasmBytes.size();

        runtime.wasmRuntime = std::make_unique<ExternalModWasmRuntime>();
        ExternalModWasmConfig config;
        config.modId = package.manifest.id;
        config.maxMemoryKb = package.manifest.runtimeMaxMemoryKb;
        config.maxCallMs = package.manifest.runtimeMaxCallMs;

        if (!runtime.wasmRuntime->Initialize(wasmBytes, config, outError)) {
            runtime.moduleCompileDiagnostics = outError;
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

void ExternalModManager::UnmountAssetsForPackage(ExternalModPackage& package) {
    if (!package.mountedAssets.empty()) {
        auto context = Ship::Context::GetInstance();
        if (context != nullptr && context->GetResourceManager() != nullptr &&
            context->GetResourceManager()->GetArchiveManager() != nullptr) {
            auto archiveManager = context->GetResourceManager()->GetArchiveManager();
            for (const auto& mountedPath : package.mountedAssets) {
                const auto removedCount = archiveManager->RemoveArchive(mountedPath.generic_string());
                if (removedCount == 0) {
                    SPDLOG_WARN("[ExternalMods] Attempted to unmount archive not present: {}", mountedPath.string());
                }
            }
        }
        package.mountedAssets.clear();
    }

    if (package.isZip || !package.runtime.generatedAssetPaths.empty()) {
        const std::array<std::filesystem::path, 2> cacheRoots = {
            GetExternalModsCacheRootPath(),
            GetExternalModsLegacyCacheRootPath(),
        };
        for (const auto& cacheRoot : cacheRoots) {
            std::error_code ec;
            std::filesystem::remove_all(cacheRoot / SanitizeModIdForPath(package.manifest.id), ec);
            if (ec) {
                SPDLOG_WARN("[ExternalMods] Failed to remove extracted cache for {}: {}", package.manifest.id,
                            ec.message());
            }
            ec.clear();
            std::filesystem::remove_all(cacheRoot / "_generated" / SanitizeModIdForPath(package.manifest.id), ec);
            if (ec) {
                SPDLOG_WARN("[ExternalMods] Failed to remove generated cache for {}: {}", package.manifest.id,
                            ec.message());
            }
        }
    }
    package.runtime.generatedAssetPaths.clear();
    package.runtime.generatedAssetArchives.clear();
}

bool ExternalModManager::MountAssetsForPackage(ExternalModPackage& package, std::string& outError) {
    if (package.manifest.assets.empty() && package.runtime.generatedAssetPaths.empty() &&
        package.runtime.generatedAssetArchives.empty()) {
        return true;
    }

    std::vector<std::filesystem::path> preparedAssets;
    std::vector<std::shared_ptr<Ship::Archive>> preparedGeneratedAssets;
    preparedAssets.reserve(package.manifest.assets.size() + package.runtime.generatedAssetPaths.size());
    preparedGeneratedAssets.reserve(package.runtime.generatedAssetArchives.size());
    std::unordered_set<std::string> preparedAssetSet;

    auto addPreparedAsset = [&](const std::filesystem::path& assetPath) {
        const auto normalized = assetPath.lexically_normal();
        if (preparedAssetSet.insert(normalized.generic_string()).second) {
            preparedAssets.push_back(normalized);
        }
    };
    auto addPreparedGeneratedAsset = [&](const std::shared_ptr<Ship::Archive>& archive) {
        if (archive == nullptr) {
            return;
        }
        const auto key = archive->GetPath();
        if (preparedAssetSet.insert(key).second) {
            preparedGeneratedAssets.push_back(archive);
        }
    };

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
            addPreparedAsset(fullAssetPath);
        }
    } else {
        const auto cacheRoot = GetExternalModsCacheRootPath();
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
            addPreparedAsset(extractedPath);
        }
    }

    for (const auto& generatedAssetPath : package.runtime.generatedAssetPaths) {
        if (!std::filesystem::exists(generatedAssetPath) || !std::filesystem::is_regular_file(generatedAssetPath)) {
            outError = "Generated asset not found: " + generatedAssetPath.generic_string();
            return false;
        }
        addPreparedAsset(generatedAssetPath);
    }
    for (const auto& generatedArchive : package.runtime.generatedAssetArchives) {
        if (generatedArchive == nullptr) {
            outError = "Generated in-memory asset is null";
            return false;
        }
        addPreparedGeneratedAsset(generatedArchive);
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
    for (const auto& archive : preparedGeneratedAssets) {
        if (!archive->IsLoaded()) {
            archive->Load();
        }
        const auto mountedArchive = context->GetResourceManager()->GetArchiveManager()->AddArchive(archive);
        if (mountedArchive == nullptr) {
            outError = "Failed to mount generated in-memory asset: " + archive->GetPath();
            return false;
        }
        package.mountedAssets.push_back(std::filesystem::path(archive->GetPath()));
    }

    return true;
}
void ExternalModManager::ExecuteActions(ExternalModPackage& package, const std::vector<ExternalModAction>& actions,
                                        const char* triggerName) {
    for (const auto& action : actions) {
        if (!package.runtime.enabled) {
            return;
        }

        ExternalModActorInstance* actionActorInstance = nullptr;
        if (action.actorHandle != 0) {
            actionActorInstance = FindActorInstance(package.runtime, action.actorHandle);
        }
        const bool hasActorCapability =
            ManifestHasCapability(package.manifest, "actors.vm.v1") ||
            ManifestHasCapability(package.manifest, "actors.generic.v1");

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
            case ExternalModActionType::LoadModScene: {
                const auto* sceneDefinition = FindSceneDefinition(package.runtime, action.modSceneId);
                if (sceneDefinition == nullptr || !sceneDefinition->hasEntrance) {
                    DisableRuntime(package, "loadModScene references unknown sceneId or scene without entrance: " +
                                                action.modSceneId);
                    return;
                }
                if (gPlayState != nullptr) {
                    gPlayState->nextEntranceIndex = sceneDefinition->entranceIndex;
                    gPlayState->transitionTrigger = TRANS_TRIGGER_START;
                    gPlayState->transitionType = TRANS_TYPE_FADE_BLACK;
                    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK_FAST;
                }
                break;
            }
            case ExternalModActionType::PressButton:
                if (gPlayState != nullptr && action.buttonMask != 0) {
                    auto* input = &gPlayState->state.input[0];
                    const auto mask = static_cast<uint16_t>(action.buttonMask & 0xFFFF);
                    input->press.button |= mask;
                    input->cur.button |= mask;
                }
                break;
            case ExternalModActionType::ShowEquippedItemGet:
                if (gPlayState != nullptr) {
                    int32_t resolvedButtonMask = 0;
                    int32_t slotIndex = 0;
                    if (!TryResolveEquippedActionButtonMask(action.buttonMask, resolvedButtonMask, slotIndex)) {
                        break;
                    }

                    if (slotIndex < 0 || slotIndex >= static_cast<int32_t>(ARRAY_COUNT(gSaveContext.equips.buttonItems))) {
                        break;
                    }

                    const int32_t equippedItemId = gSaveContext.equips.buttonItems[slotIndex];
                    if (equippedItemId == ITEM_NONE) {
                        break;
                    }

                    const auto getItemId = RetrieveGetItemIDFromItemID(static_cast<ItemID>(equippedItemId));
                    const int32_t getItemIdValue = static_cast<int32_t>(getItemId);
                    if (getItemIdValue <= GI_NONE || getItemIdValue >= GI_MAX) {
                        break;
                    }

                    GetItemEntry entry = ItemTable_Retrieve(static_cast<int16_t>(getItemIdValue));
                    GiveItemEntryWithoutActor(gPlayState, entry);
                }
                break;
            case ExternalModActionType::SpawnSmoke:
                if (gPlayState != nullptr) {
                    auto* smokePlayer = GET_PLAYER(gPlayState);
                    if (smokePlayer != nullptr) {
                        Vec3f smokePos = smokePlayer->actor.world.pos;
                        smokePos.y += 25.0f;
                        Vec3f smokeVel = { 0.0f, 0.8f, 0.0f };
                        Vec3f smokeAccel = { 0.0f, 0.05f, 0.0f };
                        EffectSsIceSmoke_Spawn(gPlayState, &smokePos, &smokeVel, &smokeAccel, 120);
                    }
                }
                break;
            case ExternalModActionType::SpawnKusa:
                if (gPlayState != nullptr) {
                    auto* kusaPlayer = GET_PLAYER(gPlayState);
                    if (kusaPlayer != nullptr) {
                        const Vec3f kusaPos = kusaPlayer->actor.world.pos;
                        Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_KUSA, kusaPos.x, kusaPos.y, kusaPos.z, 0,
                                    kusaPlayer->actor.shape.rot.y, 0, 0, true);
                    }
                }
                break;
            case ExternalModActionType::LanternLight:
                if (gPlayState != nullptr) {
                    auto* lanternPlayer = GET_PLAYER(gPlayState);
                    if (lanternPlayer != nullptr && IsActionItemRequirementSatisfied(package, action, lanternPlayer)) {
                        SpawnLanternLightAtPlayer(gPlayState, lanternPlayer);
                    }
                }
                break;
            case ExternalModActionType::IgniteFrontTarget:
                if (gPlayState != nullptr) {
                    auto* lanternPlayer = GET_PLAYER(gPlayState);
                    if (lanternPlayer != nullptr && IsActionItemRequirementSatisfied(package, action, lanternPlayer)) {
                        Actor* igniteTarget = FindIgniteTargetInFront(gPlayState, lanternPlayer);
                        if (igniteTarget != nullptr) {
                            IgniteActor(gPlayState, igniteTarget);
                        } else {
                            SpawnIgniteMissEffect(gPlayState, lanternPlayer);
                        }
                    }
                }
                break;
            case ExternalModActionType::SpawnActor: {
                if (!hasActorCapability) {
                    DisableRuntime(package, "spawnActor requires capability actors.vm.v1 or actors.generic.v1");
                    return;
                }
                const auto* definition = FindActorDefinition(package.runtime, action.actorDefinitionId);
                if (definition == nullptr) {
                    DisableRuntime(package, "spawnActor references unknown actorDefinitionId: " + action.actorDefinitionId);
                    return;
                }
                std::string actorError;
                uint32_t spawnedHandle = 0;
                if (!SpawnActorInstance(package, *definition, spawnedHandle, actorError)) {
                    DisableRuntime(package, "spawnActor failed: " + actorError);
                    return;
                }
                if (!definition->behaviorId.empty()) {
                    auto* spawnedInstance = FindActorInstance(package.runtime, spawnedHandle);
                    if (spawnedInstance != nullptr) {
                        int32_t stepCount = package.runtime.behaviorStepsThisFrame;
                        std::vector<ExternalModAction> onSpawnActions;
                        std::string behaviorError;
                        if (!CollectBehaviorEventActions(package, definition->behaviorId, "onspawn", spawnedInstance,
                                                         package.runtime.behaviorMaxStepsPerModPerFrame, stepCount,
                                                         onSpawnActions, behaviorError)) {
                            DisableRuntime(package, "spawnActor behavior failed: " + behaviorError);
                            return;
                        }
                        package.runtime.behaviorStepsThisFrame = stepCount;
                        if (!onSpawnActions.empty()) {
                            ExecuteActions(package, onSpawnActions, "actorOnSpawn");
                        }
                    }
                }
                SPDLOG_INFO("[ExternalMods] {} spawned virtual actor '{}' handle={} via {}", package.manifest.id,
                            definition->id, spawnedHandle, triggerName);
                break;
            }
            case ExternalModActionType::DespawnActor: {
                if (!hasActorCapability) {
                    DisableRuntime(package, "despawnActor requires capability actors.vm.v1 or actors.generic.v1");
                    return;
                }
                std::vector<ExternalModAction> onDestroyActions;
                if (actionActorInstance != nullptr) {
                    const auto* definition = FindActorDefinition(package.runtime, actionActorInstance->definitionId);
                    if (definition != nullptr && !definition->behaviorId.empty()) {
                        int32_t stepCount = package.runtime.behaviorStepsThisFrame;
                        std::string behaviorError;
                        if (!CollectBehaviorEventActions(package, definition->behaviorId, "ondestroy", actionActorInstance,
                                                         package.runtime.behaviorMaxStepsPerModPerFrame, stepCount,
                                                         onDestroyActions, behaviorError)) {
                            DisableRuntime(package, "despawnActor behavior failed: " + behaviorError);
                            return;
                        }
                        package.runtime.behaviorStepsThisFrame = stepCount;
                    }
                }
                std::string actorError;
                if (!DespawnActorInstance(package, action.actorHandle, actorError)) {
                    DisableRuntime(package, "despawnActor failed: " + actorError);
                    return;
                }
                if (!onDestroyActions.empty()) {
                    ExecuteActions(package, onDestroyActions, "actorOnDestroy");
                }
                break;
            }
            case ExternalModActionType::SetActorState: {
                if (!hasActorCapability) {
                    DisableRuntime(package, "setActorState requires capability actors.vm.v1 or actors.generic.v1");
                    return;
                }
                auto* instance = FindActorInstance(package.runtime, action.actorHandle);
                if (instance == nullptr) {
                    DisableRuntime(package,
                                   "setActorState references unknown actor handle: " + std::to_string(action.actorHandle));
                    return;
                }
                instance->state[action.actorStateKey] = action.actorStateValue;
                if (ToLower(action.actorStateKey) == "timerframes") {
                    float parsedValue = 0.0f;
                    if (TryParseFloatString(action.actorStateValue, parsedValue)) {
                        instance->timerFrames = std::max(0, static_cast<int32_t>(std::lround(parsedValue)));
                    }
                }
                break;
            }
            case ExternalModActionType::MoveActorToPathNode: {
                if (!hasActorCapability) {
                    DisableRuntime(package, "moveActorToPathNode requires capability actors.vm.v1 or actors.generic.v1");
                    return;
                }
                auto* instance = FindActorInstance(package.runtime, action.actorHandle);
                if (instance == nullptr) {
                    DisableRuntime(package, "moveActorToPathNode references unknown actor handle: " +
                                                std::to_string(action.actorHandle));
                    return;
                }
                instance->state["pathNodeIndex"] = std::to_string(action.pathNodeIndex);
                break;
            }
            case ExternalModActionType::OpenDialog:
                if (gPlayState != nullptr) {
                    if (action.actorHandle != 0 && FindActorInstance(package.runtime, action.actorHandle) == nullptr) {
                        DisableRuntime(package,
                                       "openDialog references unknown actor handle: " + std::to_string(action.actorHandle));
                        return;
                    }
                    Message_StartTextbox(gPlayState, static_cast<uint16_t>(action.dialogId & 0xFFFF), nullptr);
                }
                break;
            case ExternalModActionType::SetSwitchFlag:
                if (gPlayState != nullptr) {
                    Flags_SetSwitch(gPlayState, action.intValue & 0x3F);
                }
                break;
            case ExternalModActionType::ClearSwitchFlag:
                if (gPlayState != nullptr) {
                    Flags_UnsetSwitch(gPlayState, action.intValue & 0x3F);
                }
                break;
            case ExternalModActionType::SetEventChkInf:
                Flags_SetEventChkInf(action.intValue);
                break;
            case ExternalModActionType::ClearEventChkInf:
                Flags_UnsetEventChkInf(action.intValue);
                break;
            case ExternalModActionType::SetInfTable:
                Flags_SetInfTable(action.intValue);
                break;
            case ExternalModActionType::ClearInfTable:
                Flags_UnsetInfTable(action.intValue);
                break;
            case ExternalModActionType::GiveRupees:
                Rupees_ChangeBy(std::max(0, action.intValue));
                break;
            case ExternalModActionType::TakeRupees:
                Rupees_ChangeBy(-std::max(0, action.intValue));
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
                GrantItemForDefinitionIfMissing(*itemIt);
                if (gPlayState != nullptr && itemIt->acquireTextId > 0) {
                    Message_StartTextbox(gPlayState, static_cast<uint16_t>(itemIt->acquireTextId & 0xFFFF), nullptr);
                }
                if (!itemIt->onEquipBehavior.empty()) {
                    auto runItemEvent = [&](const std::string& eventName, const char* source) {
                        int32_t stepCount = package.runtime.behaviorStepsThisFrame;
                        std::vector<ExternalModAction> behaviorActions;
                        std::string behaviorError;
                        if (!CollectBehaviorEventActions(package, itemIt->onEquipBehavior, eventName, nullptr,
                                                         package.runtime.behaviorMaxStepsPerModPerFrame, stepCount,
                                                         behaviorActions, behaviorError)) {
                            DisableRuntime(package, std::string(source) + " failed: " + behaviorError);
                            return;
                        }
                        package.runtime.behaviorStepsThisFrame = stepCount;
                        if (!behaviorActions.empty()) {
                            ExecuteActions(package, behaviorActions, source);
                        }
                    };
                    runItemEvent("onitemgranted", "onItemGrantedBehavior");
                    if (!package.runtime.enabled) {
                        return;
                    }
                    runItemEvent("onitemequipped", "onItemEquippedBehavior");
                    if (!package.runtime.enabled) {
                        return;
                    }
                }
                auto& manager = ExternalModManager::Instance();
                manager.SyncExtraInventoryGrid();
                auto& packages = manager.GetPackages();
                ApplyModItemAgeRequirementOverrides(packages);
                ApplyModItemIconOverrides(packages);
                ApplyModHookshotTextureOverrides(packages);
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
                auto& manager = ExternalModManager::Instance();
                manager.SyncExtraInventoryGrid();
                auto& packages = manager.GetPackages();
                ApplyModItemAgeRequirementOverrides(packages);
                ApplyModItemIconOverrides(packages);
                ApplyModHookshotTextureOverrides(packages);
                break;
            }
            case ExternalModActionType::SetVar: {
                auto* blackboard = GetBlackboardScopeForAction(package, actionActorInstance, action.variableScope);
                if (blackboard == nullptr) {
                    DisableRuntime(package, "setVar uses invalid scope or actor context: " + action.variableScope);
                    return;
                }
                (*blackboard)[action.variableKey] = action.variableValue;
                break;
            }
            case ExternalModActionType::AddVar: {
                auto* blackboard = GetBlackboardScopeForAction(package, actionActorInstance, action.variableScope);
                if (blackboard == nullptr) {
                    DisableRuntime(package, "addVar uses invalid scope or actor context: " + action.variableScope);
                    return;
                }
                float currentValue = 0.0f;
                if (const auto it = blackboard->find(action.variableKey); it != blackboard->end()) {
                    TryParseFloatString(it->second, currentValue);
                }
                currentValue += action.variableNumber;
                (*blackboard)[action.variableKey] = std::to_string(currentValue);
                break;
            }
            case ExternalModActionType::ClampVar: {
                auto* blackboard = GetBlackboardScopeForAction(package, actionActorInstance, action.variableScope);
                if (blackboard == nullptr) {
                    DisableRuntime(package, "clampVar uses invalid scope or actor context: " + action.variableScope);
                    return;
                }
                float currentValue = 0.0f;
                if (const auto it = blackboard->find(action.variableKey); it != blackboard->end()) {
                    TryParseFloatString(it->second, currentValue);
                }
                currentValue = std::clamp(currentValue, action.variableMin, action.variableMax);
                (*blackboard)[action.variableKey] = std::to_string(currentValue);
                break;
            }
            case ExternalModActionType::EmitSignal:
                package.runtime.pendingSignals.emplace_back(action.actorHandle, action.signalName);
                break;
            case ExternalModActionType::CallBehavior: {
                int32_t stepCount = package.runtime.behaviorStepsThisFrame;
                std::vector<ExternalModAction> behaviorActions;
                std::string behaviorError;
                if (!CollectBehaviorEventActions(package, action.behaviorId, "manual", actionActorInstance,
                                                 package.runtime.behaviorMaxStepsPerModPerFrame, stepCount, behaviorActions,
                                                 behaviorError)) {
                    DisableRuntime(package, "callBehavior failed: " + behaviorError);
                    return;
                }
                package.runtime.behaviorStepsThisFrame = stepCount;
                if (!behaviorActions.empty()) {
                    ExecuteActions(package, behaviorActions, "callBehavior");
                }
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

void ExternalModManager::DispatchExtendedHook(ExternalModHookType hookType, const ExternalModHookEventContext& context,
                                              const char* triggerName) {
    const char* resolvedTriggerName = triggerName != nullptr ? triggerName : "extendedHook";

    for (auto& package : mPackages) {
        if (!package.runtime.enabled || !ManifestHasCapability(package.manifest, "hooks.extended.v1")) {
            continue;
        }

        for (auto& subscription : package.runtime.hookSubscriptions) {
            if (!package.runtime.enabled) {
                break;
            }
            if (subscription.hook != hookType || subscription.cooldownRemaining > 0) {
                continue;
            }
            if (!MatchesHookFilter(subscription.filters, context)) {
                continue;
            }

            if (package.runtime.hookCallsThisFrame >= package.runtime.maxHookCallsPerFrame) {
                DisableRuntime(package, "Hook call budget exceeded in current frame");
                break;
            }

            if (subscription.dispatch == ExternalModHookDispatchType::Actions) {
                ExecuteActions(package, subscription.actions, subscription.id.empty() ? resolvedTriggerName : subscription.id.c_str());
            } else {
                if (!package.runtime.wasmRuntime) {
                    DisableRuntime(package, "Hook subscription requires wasm runtime: " + subscription.id);
                    break;
                }

                std::string wasmError;
                const auto args = BuildHookWasmArgs(context);
                if (!package.runtime.wasmRuntime->InvokeExport(subscription.wasmExport, args, wasmError)) {
                    DisableRuntime(package,
                                   "Hook wasm export failed for subscription '" + subscription.id + "': " + wasmError);
                    break;
                }
            }

            subscription.cooldownRemaining = subscription.cooldownFrames;
            package.runtime.hookCallsThisFrame++;
        }
    }
}
void ExternalModManager::RegisterHooks() {
    if (GameInteractor::Instance == nullptr) {
        SPDLOG_WARN("[ExternalMods] GameInteractor is unavailable; hooks were not registered");
        return;
    }

    UnregisterHooks();

    mOnLoadGameHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>(
        [](int32_t fileNum) { ExternalModManager::Instance().OnLoadGame(fileNum); });
    mOnExitGameHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnExitGame>(
        [](int32_t fileNum) { ExternalModManager::Instance().OnExitGame(fileNum); });
    mOnSceneInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>(
        [](int16_t sceneNum) { ExternalModManager::Instance().OnSceneInit(sceneNum); });
    mAfterSceneCommandsHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::AfterSceneCommands>(
        [](int16_t sceneNum) { ExternalModManager::Instance().OnAfterSceneCommands(sceneNum); });
    mOnTransitionEndHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnTransitionEnd>(
        [](int16_t sceneNum) { ExternalModManager::Instance().OnTransitionEnd(sceneNum); });
    mOnFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>(
        [](int16_t flagType, int16_t flag) { ExternalModManager::Instance().OnFlagSet(flagType, flag); });
    mOnFlagUnsetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagUnset>(
        [](int16_t flagType, int16_t flag) { ExternalModManager::Instance().OnFlagUnset(flagType, flag); });
    mOnSceneFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>(
        [](int16_t sceneNum, int16_t flagType, int16_t flag) {
            ExternalModManager::Instance().OnSceneFlagSet(sceneNum, flagType, flag);
        });
    mOnSceneFlagUnsetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagUnset>(
        [](int16_t sceneNum, int16_t flagType, int16_t flag) {
            ExternalModManager::Instance().OnSceneFlagUnset(sceneNum, flagType, flag);
        });
    mOnPlayerUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>(
        []() { ExternalModManager::Instance().OnPlayerUpdate(); });
    mOnGameFrameHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>(
        []() { ExternalModManager::Instance().OnGameFrameUpdate(); });
    mOnPlayerUseItemHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUseItem>(
        [](void* player, int32_t itemId, bool* allowVanilla) {
            ExternalModManager::Instance().OnPlayerUseItem(player, itemId, allowVanilla);
        });
    mOnPlayerHealthChangeHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerHealthChange>(
        [](int16_t amount) { ExternalModManager::Instance().OnPlayerHealthChange(amount); });
    mOnItemReceiveHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>(
        [](GetItemEntry itemEntry) { ExternalModManager::Instance().OnItemReceive(static_cast<int16_t>(itemEntry.itemId)); });
    mOnActorInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>(
        [](void* actor) { ExternalModManager::Instance().OnActorHook(ExternalModHookType::OnActorInit, actor, "OnActorInit"); });
    mOnActorSpawnHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorSpawn>(
        [](void* actor) { ExternalModManager::Instance().OnActorHook(ExternalModHookType::OnActorSpawn, actor, "OnActorSpawn"); });
    mOnActorUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>(
        [](void* actor) { ExternalModManager::Instance().OnActorHook(ExternalModHookType::OnActorUpdate, actor, "OnActorUpdate"); });
    mOnActorKillHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorKill>(
        [](void* actor) { ExternalModManager::Instance().OnActorHook(ExternalModHookType::OnActorKill, actor, "OnActorKill"); });
    mOnActorDestroyHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorDestroy>(
        [](void* actor) {
            ExternalModManager::Instance().OnActorHook(ExternalModHookType::OnActorDestroy, actor, "OnActorDestroy");
        });
    mOnEnemyDefeatHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnEnemyDefeat>(
        [](void* actor) {
            ExternalModManager::Instance().OnActorHook(ExternalModHookType::OnEnemyDefeat, actor, "OnEnemyDefeat");
        });
    mOnBossDefeatHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnBossDefeat>(
        [](void* actor) {
            ExternalModManager::Instance().OnActorHook(ExternalModHookType::OnBossDefeat, actor, "OnBossDefeat");
        });
    mOnPlayDestroyHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayDestroy>(
        []() { ExternalModManager::Instance().OnPlayDestroy(); });
}

void ExternalModManager::UnregisterHooks() {
    if (GameInteractor::Instance != nullptr) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnLoadGame>(mOnLoadGameHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnExitGame>(mOnExitGameHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(mOnSceneInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::AfterSceneCommands>(mAfterSceneCommandsHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnTransitionEnd>(mOnTransitionEndHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagSet>(mOnFlagSetHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagUnset>(mOnFlagUnsetHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagSet>(mOnSceneFlagSetHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagUnset>(mOnSceneFlagUnsetHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(mOnPlayerUpdateHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnGameFrameUpdate>(mOnGameFrameHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUseItem>(mOnPlayerUseItemHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerHealthChange>(mOnPlayerHealthChangeHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnItemReceive>(mOnItemReceiveHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorInit>(mOnActorInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorSpawn>(mOnActorSpawnHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(mOnActorUpdateHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorKill>(mOnActorKillHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorDestroy>(mOnActorDestroyHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnEnemyDefeat>(mOnEnemyDefeatHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnBossDefeat>(mOnBossDefeatHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayDestroy>(mOnPlayDestroyHook);
    }

    mOnLoadGameHook = 0;
    mOnExitGameHook = 0;
    mOnSceneInitHook = 0;
    mAfterSceneCommandsHook = 0;
    mOnTransitionEndHook = 0;
    mOnFlagSetHook = 0;
    mOnFlagUnsetHook = 0;
    mOnSceneFlagSetHook = 0;
    mOnSceneFlagUnsetHook = 0;
    mOnPlayerUpdateHook = 0;
    mOnGameFrameHook = 0;
    mOnPlayerUseItemHook = 0;
    mOnPlayerHealthChangeHook = 0;
    mOnItemReceiveHook = 0;
    mOnActorInitHook = 0;
    mOnActorSpawnHook = 0;
    mOnActorUpdateHook = 0;
    mOnActorKillHook = 0;
    mOnActorDestroyHook = 0;
    mOnEnemyDefeatHook = 0;
    mOnBossDefeatHook = 0;
    mOnPlayDestroyHook = 0;
}

void ExternalModManager::OnLoadGame(int32_t fileNum) {
    (void)fileNum;

    for (auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }

        package.runtime.hookCallsThisFrame = 0;
        package.runtime.behaviorStepsThisFrame = 0;

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
            for (auto& hookSubscription : package.runtime.hookSubscriptions) {
                hookSubscription.cooldownRemaining = 0;
            }
            package.runtime.actorInstances.clear();
            package.runtime.nextActorHandle = 1;
            package.runtime.pendingSignals.clear();
            package.runtime.sceneBlackboard.clear();
            package.runtime.lastSceneSeen = -1;
            package.runtime.lastRoomSeen = -1;
            package.runtime.hasLastDayNight = false;
            package.runtime.lastIsNight = false;
            package.runtime.switchSnapshotInitialized = false;
            package.runtime.switchSnapshot.fill(0);

            ExecuteActions(package, package.runtime.onGameLoadedActions, "onGameLoaded");
        } catch (const std::exception& ex) {
            DisableRuntime(package, std::string("Unhandled exception on onGameLoaded: ") + ex.what());
        } catch (...) {
            DisableRuntime(package, "Unhandled exception on onGameLoaded");
        }
    }

    ExternalModHookEventContext context;
    if (gPlayState != nullptr) {
        context.scene = static_cast<int16_t>(gPlayState->sceneNum);
    }
    DispatchExtendedHook(ExternalModHookType::OnLoadGame, context, "OnLoadGame");

    SyncExtraInventoryGrid();
    ApplyModItemAgeRequirementOverrides(mPackages);
    ApplyModItemIconOverrides(mPackages);
    ApplyModHookshotTextureOverrides(mPackages);
}

void ExternalModManager::OnExitGame(int32_t fileNum) {
    (void)fileNum;

    ExternalModHookEventContext context;
    if (gPlayState != nullptr) {
        context.scene = static_cast<int16_t>(gPlayState->sceneNum);
    }
    DispatchExtendedHook(ExternalModHookType::OnExitGame, context, "OnExitGame");
}

void ExternalModManager::OnSceneInit(int16_t sceneNum) {
    for (auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }
        try {
            package.runtime.behaviorStepsThisFrame = 0;
            package.runtime.pendingSignals.clear();
            package.runtime.actorInstances.clear();
            package.runtime.nextActorHandle = 1;
            package.runtime.lastSceneSeen = sceneNum;
            package.runtime.lastRoomSeen =
                gPlayState != nullptr ? static_cast<int16_t>(gPlayState->roomCtx.curRoom.num) : static_cast<int16_t>(-1);
            package.runtime.hasLastDayNight = false;
            package.runtime.lastIsNight = false;
            package.runtime.switchSnapshotInitialized = false;
            package.runtime.switchSnapshot.fill(0);

            for (const auto& actorDefinition : package.runtime.actorDefinitions) {
                if (actorDefinition.sceneId != sceneNum) {
                    continue;
                }
                std::string actorError;
                uint32_t spawnedHandle = 0;
                if (!SpawnActorInstance(package, actorDefinition, spawnedHandle, actorError)) {
                    DisableRuntime(package, "Scene actor spawn failed: " + actorError);
                    break;
                }
                if (!actorDefinition.behaviorId.empty()) {
                    auto* spawnedInstance = FindActorInstance(package.runtime, spawnedHandle);
                    if (spawnedInstance != nullptr) {
                        int32_t stepCount = package.runtime.behaviorStepsThisFrame;
                        std::vector<ExternalModAction> onSpawnActions;
                        std::string behaviorError;
                        if (!CollectBehaviorEventActions(package, actorDefinition.behaviorId, "onspawn", spawnedInstance,
                                                         package.runtime.behaviorMaxStepsPerModPerFrame, stepCount,
                                                         onSpawnActions, behaviorError)) {
                            DisableRuntime(package, "Scene actor onSpawn behavior failed: " + behaviorError);
                            break;
                        }
                        package.runtime.behaviorStepsThisFrame = stepCount;
                        if (!onSpawnActions.empty()) {
                            ExecuteActions(package, onSpawnActions, "sceneActorOnSpawn");
                        }
                    }
                }
            }

            for (auto& trigger : package.runtime.frameTriggers) {
                trigger.wasInside = false;
                trigger.cooldownRemaining = 0;
            }
            for (const auto& sceneAction : package.runtime.onSceneInitActions) {
                if (sceneAction.sceneId == sceneNum) {
                    ExecuteActions(package, sceneAction.actions, "onSceneInit");
                }
            }

            if (!package.runtime.enabled) {
                continue;
            }

            for (auto& instance : package.runtime.actorInstances) {
                if (!instance.active || instance.sceneId != sceneNum) {
                    continue;
                }
                const auto* actorDefinition = FindActorDefinition(package.runtime, instance.definitionId);
                if (actorDefinition == nullptr || actorDefinition->behaviorId.empty()) {
                    continue;
                }

                int32_t stepCount = package.runtime.behaviorStepsThisFrame;
                std::vector<ExternalModAction> onSceneEnterActions;
                std::string behaviorError;
                if (!CollectBehaviorEventActions(package, actorDefinition->behaviorId, "onsceneenter", &instance,
                                                 package.runtime.behaviorMaxStepsPerModPerFrame, stepCount,
                                                 onSceneEnterActions, behaviorError)) {
                    DisableRuntime(package, "onSceneEnter behavior failed: " + behaviorError);
                    break;
                }
                package.runtime.behaviorStepsThisFrame = stepCount;
                if (!onSceneEnterActions.empty()) {
                    ExecuteActions(package, onSceneEnterActions, "onSceneEnter");
                }
            }
        } catch (const std::exception& ex) {
            DisableRuntime(package, std::string("Unhandled exception on onSceneInit: ") + ex.what());
        } catch (...) {
            DisableRuntime(package, "Unhandled exception on onSceneInit");
        }
    }

    ExternalModHookEventContext context;
    context.scene = sceneNum;
    DispatchExtendedHook(ExternalModHookType::OnSceneInit, context, "OnSceneInit");

    SyncExtraInventoryGrid();
    ApplyModItemAgeRequirementOverrides(mPackages);
    ApplyModItemIconOverrides(mPackages);
    ApplyModHookshotTextureOverrides(mPackages);
}

void ExternalModManager::OnAfterSceneCommands(int16_t sceneNum) {
    ExternalModHookEventContext context;
    context.scene = sceneNum;
    DispatchExtendedHook(ExternalModHookType::AfterSceneCommands, context, "AfterSceneCommands");
}

void ExternalModManager::OnTransitionEnd(int16_t sceneNum) {
    ExternalModHookEventContext context;
    context.scene = sceneNum;
    DispatchExtendedHook(ExternalModHookType::OnTransitionEnd, context, "OnTransitionEnd");
}

void ExternalModManager::OnFlagSet(int16_t flagType, int16_t flag) {
    ExternalModHookEventContext context;
    if (gPlayState != nullptr) {
        context.scene = static_cast<int16_t>(gPlayState->sceneNum);
    }
    context.flagType = flagType;
    context.flagId = flag;
    DispatchExtendedHook(ExternalModHookType::OnFlagSet, context, "OnFlagSet");
}

void ExternalModManager::OnFlagUnset(int16_t flagType, int16_t flag) {
    ExternalModHookEventContext context;
    if (gPlayState != nullptr) {
        context.scene = static_cast<int16_t>(gPlayState->sceneNum);
    }
    context.flagType = flagType;
    context.flagId = flag;
    DispatchExtendedHook(ExternalModHookType::OnFlagUnset, context, "OnFlagUnset");
}

void ExternalModManager::OnSceneFlagSet(int16_t sceneNum, int16_t flagType, int16_t flag) {
    ExternalModHookEventContext context;
    context.scene = sceneNum;
    context.flagType = flagType;
    context.flagId = flag;
    DispatchExtendedHook(ExternalModHookType::OnSceneFlagSet, context, "OnSceneFlagSet");
}

void ExternalModManager::OnSceneFlagUnset(int16_t sceneNum, int16_t flagType, int16_t flag) {
    ExternalModHookEventContext context;
    context.scene = sceneNum;
    context.flagType = flagType;
    context.flagId = flag;
    DispatchExtendedHook(ExternalModHookType::OnSceneFlagUnset, context, "OnSceneFlagUnset");
}

void ExternalModManager::OnPlayerUpdate() {
    ExternalModHookEventContext context;
    if (gPlayState != nullptr) {
        context.scene = static_cast<int16_t>(gPlayState->sceneNum);
    }
    DispatchExtendedHook(ExternalModHookType::OnPlayerUpdate, context, "OnPlayerUpdate");
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
    const bool interactPressed = MatchPressedButtonMask(input->cur.button, input->prev.button, BTN_A);

    for (auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }

        package.runtime.hookCallsThisFrame = 0;
        package.runtime.behaviorStepsThisFrame = 0;
        for (auto& hookSubscription : package.runtime.hookSubscriptions) {
            if (hookSubscription.cooldownRemaining > 0) {
                hookSubscription.cooldownRemaining--;
            }
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

                const auto bindingIt = std::find_if(package.runtime.inputBindings.begin(), package.runtime.inputBindings.end(),
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
                        active = MatchPressedButtonMask(input->cur.button, input->prev.button, effectiveMask);
                        break;
                    case ExternalModInputTriggerType::Held:
                        active = MatchButtonMask(input->cur.button, effectiveMask);
                        break;
                    case ExternalModInputTriggerType::Released:
                        active = MatchReleasedButtonMask(input->cur.button, input->prev.button, effectiveMask);
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
                const bool cooldownWasActive = item.cooldownRemaining > 0;
                if (item.cooldownRemaining > 0) {
                    item.cooldownRemaining--;
                }

                if (cooldownWasActive && item.cooldownRemaining == 0) {
                    std::unordered_set<std::string> behaviorIds;
                    if (!item.onUseBehavior.empty()) {
                        behaviorIds.insert(item.onUseBehavior);
                    }
                    if (!item.onEquipBehavior.empty()) {
                        behaviorIds.insert(item.onEquipBehavior);
                    }

                    for (const auto& behaviorId : behaviorIds) {
                        int32_t stepCount = package.runtime.behaviorStepsThisFrame;
                        std::vector<ExternalModAction> cooldownActions;
                        std::string behaviorError;
                        if (!CollectBehaviorEventActions(package, behaviorId, "oncooldownready", nullptr,
                                                         package.runtime.behaviorMaxStepsPerModPerFrame, stepCount,
                                                         cooldownActions, behaviorError)) {
                            DisableRuntime(package, "onCooldownReady behavior failed for item '" + item.id +
                                                        "': " + behaviorError);
                            break;
                        }
                        package.runtime.behaviorStepsThisFrame = stepCount;
                        if (!cooldownActions.empty()) {
                            ExecuteActions(package, cooldownActions, "itemOnCooldownReady");
                        }
                    }
                    if (!package.runtime.enabled) {
                        break;
                    }
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

            if (!package.runtime.enabled) {
                continue;
            }

            const int16_t currentRoomNum = static_cast<int16_t>(gPlayState->roomCtx.curRoom.num);
            const bool roomChanged = package.runtime.lastSceneSeen != sceneNum || package.runtime.lastRoomSeen != currentRoomNum;
            package.runtime.lastSceneSeen = sceneNum;
            package.runtime.lastRoomSeen = currentRoomNum;

            const bool isNight = gSaveContext.dayTime >= 0xC000 || gSaveContext.dayTime < 0x4555;
            bool timeOfDayChanged = false;
            if (!package.runtime.hasLastDayNight) {
                package.runtime.lastIsNight = isNight;
                package.runtime.hasLastDayNight = true;
            } else if (package.runtime.lastIsNight != isNight) {
                package.runtime.lastIsNight = isNight;
                timeOfDayChanged = true;
            }

            bool switchFlagChanged = false;
            int32_t changedSwitchFlag = -1;
            bool changedSwitchValue = false;
            if (!package.runtime.switchSnapshotInitialized) {
                for (size_t flag = 0; flag < package.runtime.switchSnapshot.size(); ++flag) {
                    package.runtime.switchSnapshot[flag] = Flags_GetSwitch(gPlayState, static_cast<int32_t>(flag)) ? 1 : 0;
                }
                package.runtime.switchSnapshotInitialized = true;
            } else {
                for (size_t flag = 0; flag < package.runtime.switchSnapshot.size(); ++flag) {
                    const uint8_t current = Flags_GetSwitch(gPlayState, static_cast<int32_t>(flag)) ? 1 : 0;
                    if (current != package.runtime.switchSnapshot[flag]) {
                        switchFlagChanged = true;
                        changedSwitchFlag = static_cast<int32_t>(flag);
                        changedSwitchValue = current != 0;
                    }
                    package.runtime.switchSnapshot[flag] = current;
                }
            }
            if (switchFlagChanged && changedSwitchFlag >= 0) {
                package.runtime.globalBlackboard["__switchFlag"] = std::to_string(changedSwitchFlag);
                package.runtime.globalBlackboard["__switchValue"] = changedSwitchValue ? "1" : "0";
            }

            for (auto& instance : package.runtime.actorInstances) {
                if (!instance.active || instance.sceneId != sceneNum) {
                    continue;
                }

                const auto* definition = FindActorDefinition(package.runtime, instance.definitionId);
                if (definition == nullptr) {
                    DisableRuntime(package, "Actor instance references missing definition: " + instance.definitionId);
                    break;
                }

                instance.tickCounter++;
                if (instance.tickCounter < std::max(1, definition->tickRate)) {
                    continue;
                }
                instance.tickCounter = 0;

                if (!definition->exportOnUpdate.empty()) {
                    if (!package.runtime.wasmRuntime) {
                        DisableRuntime(package, "Actor exportOnUpdate requires wasm runtime");
                        break;
                    }
                    std::string wasmError;
                    const std::vector<int32_t> args = { static_cast<int32_t>(instance.handle), instance.sceneId };
                    if (!package.runtime.wasmRuntime->InvokeExport(definition->exportOnUpdate, args, wasmError)) {
                        DisableRuntime(package,
                                       "Actor exportOnUpdate failed for '" + definition->id + "': " + wasmError);
                        break;
                    }
                }

                const float dx = playerPos.x - instance.posX;
                const float dy = playerPos.y - instance.posY;
                const float dz = playerPos.z - instance.posZ;
                const float distanceToPlayer = std::sqrt(dx * dx + dy * dy + dz * dz);
                const bool nearPlayer = distanceToPlayer <= definition->interactDistance;

                int32_t actorBehaviorSteps = 0;
                auto runBehaviorEvent = [&](const std::string& eventName, const char* source) -> bool {
                    if (definition->behaviorId.empty()) {
                        return true;
                    }
                    int32_t stepCount = package.runtime.behaviorStepsThisFrame;
                    std::vector<ExternalModAction> behaviorActions;
                    std::string behaviorError;
                    if (!CollectBehaviorEventActions(package, definition->behaviorId, eventName, &instance,
                                                     package.runtime.behaviorMaxStepsPerModPerFrame, stepCount,
                                                     behaviorActions, behaviorError)) {
                        DisableRuntime(package, std::string(source) + " behavior failed for actor '" + definition->id +
                                                "': " + behaviorError);
                        return false;
                    }

                    const int32_t stepDelta = stepCount - package.runtime.behaviorStepsThisFrame;
                    actorBehaviorSteps += std::max(stepDelta, 0);
                    package.runtime.behaviorStepsThisFrame = stepCount;
                    if (actorBehaviorSteps > package.runtime.behaviorMaxStepsPerActorPerFrame) {
                        DisableRuntime(package, "behavior step budget exceeded for actor '" + definition->id + "'");
                        return false;
                    }
                    if (!behaviorActions.empty()) {
                        ExecuteActions(package, behaviorActions, source);
                    }
                    return package.runtime.enabled;
                };

                if (!runBehaviorEvent("onupdate", "actorOnUpdate")) {
                    break;
                }
                if (!runBehaviorEvent("onrandomtick", "actorOnRandomTick")) {
                    break;
                }
                if (roomChanged && !runBehaviorEvent("onroomenter", "actorOnRoomEnter")) {
                    break;
                }
                if (timeOfDayChanged && !runBehaviorEvent("ontimeofdaychanged", "actorOnTimeOfDayChanged")) {
                    break;
                }
                if (switchFlagChanged && !runBehaviorEvent("onswitchflagchanged", "actorOnSwitchFlagChanged")) {
                    break;
                }

                if (nearPlayer && !instance.wasNearPlayer) {
                    if (!runBehaviorEvent("onplayernear", "actorOnPlayerNear")) {
                        break;
                    }
                } else if (!nearPlayer && instance.wasNearPlayer) {
                    if (!runBehaviorEvent("onplayerfar", "actorOnPlayerFar")) {
                        break;
                    }
                }
                instance.wasNearPlayer = nearPlayer;

                if (instance.timerFrames > 0) {
                    instance.timerFrames--;
                    if (instance.timerFrames == 0) {
                        if (!runBehaviorEvent("ontimer", "actorOnTimer")) {
                            break;
                        }
                    }
                }

                if (interactPressed && definition->interactable && nearPlayer) {
                    if (package.runtime.wasmRuntime && !definition->exportOnInteract.empty()) {
                        std::string wasmError;
                        const std::vector<int32_t> args = { static_cast<int32_t>(instance.handle), instance.sceneId };
                        if (!package.runtime.wasmRuntime->InvokeExport(definition->exportOnInteract, args, wasmError)) {
                            DisableRuntime(package,
                                           "Actor exportOnInteract failed for '" + definition->id + "': " + wasmError);
                            break;
                        }
                    }
                    if (!runBehaviorEvent("oninteract", "actorOnInteract")) {
                        break;
                    }
                }
            }

            if (!package.runtime.enabled) {
                continue;
            }

            if (!package.runtime.pendingSignals.empty()) {
                const auto pendingSignals = package.runtime.pendingSignals;
                package.runtime.pendingSignals.clear();
                for (const auto& signal : pendingSignals) {
                    package.runtime.globalBlackboard["__lastSignal"] = signal.second;
                    for (auto& instance : package.runtime.actorInstances) {
                        if (!instance.active || instance.sceneId != sceneNum) {
                            continue;
                        }
                        if (signal.first != 0 && signal.first != instance.handle) {
                            continue;
                        }
                        const auto* definition = FindActorDefinition(package.runtime, instance.definitionId);
                        if (definition == nullptr || definition->behaviorId.empty()) {
                            continue;
                        }

                        int32_t stepCount = package.runtime.behaviorStepsThisFrame;
                        std::vector<ExternalModAction> signalActions;
                        std::string behaviorError;
                        if (!CollectBehaviorEventActions(package, definition->behaviorId, "onsignal", &instance,
                                                         package.runtime.behaviorMaxStepsPerModPerFrame, stepCount,
                                                         signalActions, behaviorError)) {
                            DisableRuntime(package, "onSignal behavior failed for actor '" + definition->id +
                                                        "': " + behaviorError);
                            break;
                        }
                        package.runtime.behaviorStepsThisFrame = stepCount;
                        if (package.runtime.behaviorStepsThisFrame > package.runtime.behaviorMaxStepsPerModPerFrame) {
                            DisableRuntime(package, "behavior mod budget exceeded while dispatching signals");
                            break;
                        }
                        if (!signalActions.empty()) {
                            ExecuteActions(package, signalActions, "onSignal");
                        }
                    }
                    if (!package.runtime.enabled) {
                        break;
                    }
                }
            }
        } catch (const std::exception& ex) {
            DisableRuntime(package, std::string("Unhandled exception on onFrame: ") + ex.what());
        } catch (...) {
            DisableRuntime(package, "Unhandled exception on onFrame");
        }
    }

    ExternalModHookEventContext context;
    context.scene = sceneNum;
    DispatchExtendedHook(ExternalModHookType::OnGameFrameUpdate, context, "OnGameFrameUpdate");

    SyncExtraInventoryGrid();
    ApplyModItemAgeRequirementOverrides(mPackages);
    ApplyModItemIconOverrides(mPackages);
    ApplyModHookshotTextureOverrides(mPackages);
}

void ExternalModManager::OnPlayerUseItem(void* player, int32_t itemId, bool* allowVanilla) {
    ExternalModHookEventContext hookContext;
    if (gPlayState != nullptr) {
        hookContext.scene = static_cast<int16_t>(gPlayState->sceneNum);
    }
    hookContext.itemId = static_cast<int16_t>(itemId);
    DispatchExtendedHook(ExternalModHookType::OnPlayerUseItem, hookContext, "OnPlayerUseItem");

    if (allowVanilla == nullptr || !*allowVanilla) {
        return;
    }

    for (auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }

        for (auto& item : package.runtime.itemDefinitions) {
            if (!item.granted || !ItemDefinitionMatchesUseItem(item, itemId)) {
                continue;
            }

            if (item.cooldownRemaining > 0) {
                if (item.useMode == ExternalModItemUseMode::Override) {
                    *allowVanilla = false;
                    return;
                }
                continue;
            }

            if ((item.useMode == ExternalModItemUseMode::Override || item.useMode == ExternalModItemUseMode::Augment) &&
                package.runtime.wasmRuntime && !item.onUseExport.empty()) {
                std::vector<int32_t> args = { itemId };
                std::string wasmError;
                if (!package.runtime.wasmRuntime->InvokeExport(item.onUseExport, args, wasmError)) {
                    DisableRuntime(package, "WASM onUse failed for item '" + item.id + "': " + wasmError);
                    *allowVanilla = true;
                    return;
                }
            }

            if (!item.onUseBehavior.empty()) {
                int32_t stepCount = package.runtime.behaviorStepsThisFrame;
                std::vector<ExternalModAction> behaviorActions;
                std::string behaviorError;
                if (!CollectBehaviorEventActions(package, item.onUseBehavior, "onitemused", nullptr,
                                                 package.runtime.behaviorMaxStepsPerModPerFrame, stepCount,
                                                 behaviorActions, behaviorError)) {
                    DisableRuntime(package, "onUseBehavior failed for item '" + item.id + "': " + behaviorError);
                    *allowVanilla = true;
                    return;
                }
                package.runtime.behaviorStepsThisFrame = stepCount;
                if (!behaviorActions.empty()) {
                    ExecuteActions(package, behaviorActions, "itemOnUseBehavior");
                    if (!package.runtime.enabled) {
                        *allowVanilla = true;
                        return;
                    }
                }
            }

            if (item.slot == ExternalModItemSlot::Hookshot && item.useMode != ExternalModItemUseMode::Vanilla) {
                const float pullForce = GetParamOrDefault(item.params, "pullForce", 8.0f);
                const float speed = GetParamOrDefault(item.params, "speed", 12.0f);
                ExternalModItemRuntime::ApplySkyhookImpulse(player, pullForce, speed);
            }

            const auto cooldown = static_cast<int32_t>(
                std::max(0.0f, GetParamOrDefault(item.params, "cooldown", static_cast<float>(item.cooldownFrames))));
            item.cooldownFrames = cooldown;
            item.cooldownRemaining = cooldown;

            if (item.useMode == ExternalModItemUseMode::Override) {
                *allowVanilla = false;
                return;
            }
        }
    }
}

void ExternalModManager::OnPlayerHealthChange(int16_t amount) {
    ExternalModHookEventContext context;
    if (gPlayState != nullptr) {
        context.scene = static_cast<int16_t>(gPlayState->sceneNum);
    }
    context.healthDelta = amount;
    DispatchExtendedHook(ExternalModHookType::OnPlayerHealthChange, context, "OnPlayerHealthChange");
}

void ExternalModManager::OnItemReceive(int16_t itemId) {
    ExternalModHookEventContext context;
    if (gPlayState != nullptr) {
        context.scene = static_cast<int16_t>(gPlayState->sceneNum);
    }
    context.itemId = itemId;
    DispatchExtendedHook(ExternalModHookType::OnItemReceive, context, "OnItemReceive");
}

void ExternalModManager::OnActorHook(ExternalModHookType hookType, void* actor, const char* hookName) {
    ExternalModHookEventContext context;
    if (gPlayState != nullptr) {
        context.scene = static_cast<int16_t>(gPlayState->sceneNum);
    }

    if (actor != nullptr) {
        auto* actorPtr = static_cast<Actor*>(actor);
        context.actorId = static_cast<int16_t>(actorPtr->id);
        context.actorCategory = static_cast<int16_t>(actorPtr->category);
    }

    DispatchExtendedHook(hookType, context, hookName);
}

void ExternalModManager::OnPlayDestroy() {
    ExternalModHookEventContext context;
    if (gPlayState != nullptr) {
        context.scene = static_cast<int16_t>(gPlayState->sceneNum);
    }
    DispatchExtendedHook(ExternalModHookType::OnPlayDestroy, context, "OnPlayDestroy");
    for (auto& package : mPackages) {
        package.runtime.actorInstances.clear();
        package.runtime.nextActorHandle = 1;
        package.runtime.pendingSignals.clear();
        package.runtime.lastSceneSeen = -1;
        package.runtime.lastRoomSeen = -1;
        package.runtime.hasLastDayNight = false;
        package.runtime.lastIsNight = false;
        package.runtime.switchSnapshotInitialized = false;
        package.runtime.switchSnapshot.fill(0);
    }
    SyncExtraInventoryGrid();
    ApplyModItemAgeRequirementOverrides(mPackages);
    ApplyModItemIconOverrides(mPackages);
    ApplyModHookshotTextureOverrides(mPackages);
}
} // namespace SOH
