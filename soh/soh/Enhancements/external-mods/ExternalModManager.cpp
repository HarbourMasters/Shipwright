#include "ExternalModManager.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cctype>
#include <cstring>
#include <fstream>
#include <limits>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <zip.h>
#include <stb_image.h>

#include <ship/Context.h>

#include "ExternalModItemRuntime.h"
#include "ExternalModWatCompiler.h"
#include "ExternalModWasmRuntime.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Notification/Notification.h"
#include "soh/OTRGlobals.h"

extern "C" {
#include <z64.h>
#include "macros.h"
#include "variables.h"
#include "functions.h"

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
constexpr int32_t kItemIconSize = 32;
constexpr int32_t kItemModelTextureSize = 32;
constexpr int32_t kMaxDecodedIconDimension = 2048;
constexpr size_t kMaxItemModelTriangles = 4096;
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

bool TryParseObjCustomModel(const std::string& objContent, float modelScale,
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
    const float uvScale = static_cast<float>(kItemModelTextureSize << 5);

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
            convertedTriangle.vertices[i].s = toS16(u * uvScale);
            convertedTriangle.vertices[i].t = toS16((1.0f - v) * uvScale);
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
            if (definition.customModelTriangles.empty()) {
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

extern "C" void ExternalMods_DrawCustomGetItemModel(PlayState* play, GetItemEntry* getItemEntry) {
    if (play == nullptr || getItemEntry == nullptr) {
        return;
    }

    const auto* definition =
        FindCustomModelDefinitionForItem(ExternalModManager::Instance().GetPackages(), getItemEntry->itemId);
    if (definition == nullptr || definition->customModelTriangles.empty()) {
        GetItem_Draw(play, getItemEntry->gid);
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPClearGeometryMode(POLY_OPA_DISP++, G_CULL_BACK | G_LIGHTING);

    if (!definition->modelTextureRgba32.empty()) {
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
        gSPTexture(POLY_OPA_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
        gDPLoadTextureBlock(POLY_OPA_DISP++, definition->modelTextureRgba32.data(), G_IM_FMT_RGBA, G_IM_SIZ_32b,
                            definition->modelTextureWidth, definition->modelTextureHeight, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
    } else {
        gSPTexture(POLY_OPA_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_SHADE, G_CC_SHADE);
    }

    constexpr size_t kTrianglesPerBatch = 10;
    for (size_t triangleStart = 0; triangleStart < definition->customModelTriangles.size();
         triangleStart += kTrianglesPerBatch) {
        const size_t triangleCount = std::min(kTrianglesPerBatch, definition->customModelTriangles.size() - triangleStart);
        const size_t vertexCount = triangleCount * 3;

        auto* vertices = static_cast<Vtx*>(Graph_Alloc(play->state.gfxCtx, sizeof(Vtx) * vertexCount));
        if (vertices == nullptr) {
            break;
        }

        for (size_t i = 0; i < triangleCount; ++i) {
            const auto& triangle = definition->customModelTriangles[triangleStart + i];
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

        gSPVertex(POLY_OPA_DISP++, vertices, static_cast<int32_t>(vertexCount), 0);
        for (size_t i = 0; i < triangleCount; ++i) {
            const int32_t base = static_cast<int32_t>(i * 3);
            gSP1Triangle(POLY_OPA_DISP++, base + 0, base + 1, base + 2, 0);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
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
    ApplyModItemAgeRequirementOverrides(mPackages);
    ApplyModItemIconOverrides(mPackages);
}

bool ExternalModManager::ReloadPackages(std::string& outError) {
    outError.clear();
    Initialize();

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
    ApplyModItemAgeRequirementOverrides(mPackages);
    ApplyModItemIconOverrides(mPackages);

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
            if (!hasActorVmCapability) {
                outError = "actorDefinitions requires capability actors.vm.v1";
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
        } else if (hasActorVmCapability) {
            outError = "Missing required field for actors.vm.v1: actorDefinitions";
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

        if (item.contains("modelScale")) {
            if (!item["modelScale"].is_number()) {
                outError = "items[" + std::to_string(i) + "].modelScale must be numeric";
                return false;
            }
            definition.modelScale = item["modelScale"].get<float>();
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
            if (model.contains("scale")) {
                if (!model["scale"].is_number()) {
                    outError = "items[" + std::to_string(i) + "].model.scale must be numeric";
                    return false;
                }
                definition.modelScale = model["scale"].get<float>();
            }
        }

        if (definition.modelAsset.empty() && !definition.modelTextureAsset.empty()) {
            outError = "items[" + std::to_string(i) + "].modelTextureAsset requires modelAsset";
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
        }

        outDefinitions.push_back(std::move(definition));
    }

    if (outDefinitions.empty()) {
        outError = "actors.json must define at least one actor";
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

            if (definition.modelAsset.empty()) {
                continue;
            }

            std::filesystem::path modelPath;
            if (!IsSafePackageRelativePath(definition.modelAsset, modelPath, outError)) {
                outError = "items[" + std::to_string(i) + "].modelAsset " + outError;
                return false;
            }

            if (ToLower(modelPath.extension().string()) != ".obj") {
                outError = "items[" + std::to_string(i) + "].modelAsset must point to a .obj file";
                return false;
            }

            std::string modelContent;
            if (!ReadFileFromPackage(package, modelPath, kMaxItemModelBytes, modelContent, outError)) {
                outError = "items[" + std::to_string(i) + "].modelAsset read failed: " + outError;
                return false;
            }

            if (!TryParseObjCustomModel(modelContent, definition.modelScale, definition.customModelTriangles, outError)) {
                outError = "items[" + std::to_string(i) + "].modelAsset parse failed: " + outError;
                return false;
            }

            if (!definition.modelTextureAsset.empty()) {
                std::filesystem::path modelTexturePath;
                if (!IsSafePackageRelativePath(definition.modelTextureAsset, modelTexturePath, outError)) {
                    outError = "items[" + std::to_string(i) + "].modelTextureAsset " + outError;
                    return false;
                }

                if (ToLower(modelTexturePath.extension().string()) != ".png") {
                    outError = "items[" + std::to_string(i) + "].modelTextureAsset must point to a .png file";
                    return false;
                }

                std::vector<uint8_t> modelTextureBytes;
                if (!ReadBinaryFromPackage(package, modelTexturePath, kMaxItemIconBytes, modelTextureBytes, outError)) {
                    outError = "items[" + std::to_string(i) + "].modelTextureAsset read failed: " + outError;
                    return false;
                }

                if (!TryDecodeItemIconPng(modelTextureBytes, definition.modelTextureRgba32, outError)) {
                    outError = "items[" + std::to_string(i) + "].modelTextureAsset decode failed: " + outError;
                    return false;
                }

                definition.modelTextureWidth = kItemModelTextureSize;
                definition.modelTextureHeight = kItemModelTextureSize;
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

        if (ManifestHasCapability(package.manifest, "actors.vm.v1")) {
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
    if (package.mountedAssets.empty()) {
        return;
    }

    auto context = Ship::Context::GetInstance();
    if (context == nullptr || context->GetResourceManager() == nullptr ||
        context->GetResourceManager()->GetArchiveManager() == nullptr) {
        package.mountedAssets.clear();
        return;
    }

    auto* archiveManager = context->GetResourceManager()->GetArchiveManager();
    for (const auto& mountedPath : package.mountedAssets) {
        const auto removedCount = archiveManager->RemoveArchive(mountedPath.generic_string());
        if (removedCount == 0) {
            SPDLOG_WARN("[ExternalMods] Attempted to unmount archive not present: {}", mountedPath.string());
        }
    }

    if (package.isZip) {
        std::error_code ec;
        const auto cacheRoot = std::filesystem::path(Ship::Context::GetPathRelativeToAppDirectory("mods/.external-mod-cache", "soh"));
        std::filesystem::remove_all(cacheRoot / SanitizeModIdForPath(package.manifest.id), ec);
        if (ec) {
            SPDLOG_WARN("[ExternalMods] Failed to remove extracted cache for {}: {}", package.manifest.id, ec.message());
        }
    }

    package.mountedAssets.clear();
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
                if (!ManifestHasCapability(package.manifest, "actors.vm.v1")) {
                    DisableRuntime(package, "spawnActor requires capability actors.vm.v1");
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
                SPDLOG_INFO("[ExternalMods] {} spawned virtual actor '{}' handle={} via {}", package.manifest.id,
                            definition->id, spawnedHandle, triggerName);
                break;
            }
            case ExternalModActionType::DespawnActor: {
                if (!ManifestHasCapability(package.manifest, "actors.vm.v1")) {
                    DisableRuntime(package, "despawnActor requires capability actors.vm.v1");
                    return;
                }
                std::string actorError;
                if (!DespawnActorInstance(package, action.actorHandle, actorError)) {
                    DisableRuntime(package, "despawnActor failed: " + actorError);
                    return;
                }
                break;
            }
            case ExternalModActionType::SetActorState: {
                if (!ManifestHasCapability(package.manifest, "actors.vm.v1")) {
                    DisableRuntime(package, "setActorState requires capability actors.vm.v1");
                    return;
                }
                auto* instance = FindActorInstance(package.runtime, action.actorHandle);
                if (instance == nullptr) {
                    DisableRuntime(package,
                                   "setActorState references unknown actor handle: " + std::to_string(action.actorHandle));
                    return;
                }
                instance->state[action.actorStateKey] = action.actorStateValue;
                break;
            }
            case ExternalModActionType::MoveActorToPathNode: {
                if (!ManifestHasCapability(package.manifest, "actors.vm.v1")) {
                    DisableRuntime(package, "moveActorToPathNode requires capability actors.vm.v1");
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
                SyncExtraInventoryGrid();
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
                SyncExtraInventoryGrid();
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
            package.runtime.actorInstances.clear();
            package.runtime.nextActorHandle = 1;

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

    for (auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }

        package.runtime.hookCallsThisFrame = 0;
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

            if (!package.runtime.enabled) {
                continue;
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
    }
    SyncExtraInventoryGrid();
    ApplyModItemAgeRequirementOverrides(mPackages);
    ApplyModItemIconOverrides(mPackages);
}
} // namespace SOH
