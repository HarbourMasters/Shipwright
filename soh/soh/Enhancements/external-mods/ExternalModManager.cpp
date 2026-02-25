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
#include <ship/controller/controldeck/ControlDeck.h>
#include <ship/controller/controldevice/controller/Controller.h>
#include <ship/controller/controldevice/controller/ControllerButton.h>
#include <ship/controller/controldevice/controller/mapping/keyboard/KeyboardKeyToButtonMapping.h>
#include <ship/controller/controldevice/controller/mapping/keyboard/KeyboardScancodes.h>
#include <ship/resource/File.h>
#include <ship/resource/archive/Archive.h>
#include <fast/resource/type/DisplayList.h>
#include <fast/resource/ResourceType.h>

#include "ExternalModItemRuntime.h"
#include "ExternalModContentRegistry.h"
#include "ExternalModInterop.h"
#include "ExternalModWatCompiler.h"
#include "ExternalModWasmRuntime.h"
#include "soh/SaveManager.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Notification/Notification.h"
#include "soh/OTRGlobals.h"
#include "soh/ResourceManagerHelpers.h"
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_gi_shield_2/object_gi_shield_2.h"
#include "src/overlays/actors/ovl_En_Kusa/z_en_kusa.h"
#include "src/overlays/actors/ovl_En_Wood02/z_en_wood02.h"

extern "C" {
#include <z64.h>
#include "macros.h"
#include "variables.h"
#include "functions.h"

GetItemEntry ItemTable_Retrieve(int16_t getItemID);
GetItemID RetrieveGetItemIDFromItemID(ItemID itemID);

extern SaveContext gSaveContext;
extern PlayState* gPlayState;
void Interface_DrawItemIconTexture(PlayState* play, void* texture, s16 button);
void Interface_DrawAmmoCount(PlayState* play, s16 button, s16 alpha);
void Player_StartMode_Idle(PlayState* play, Player* thisx);
}

namespace SOH {

namespace {
constexpr int32_t kExternalModApiVersionMin = 3;
constexpr int32_t kExternalModApiVersionMax = 3;
constexpr int32_t kExternalModApiVersionV3 = 3;
constexpr uint64_t kMaxManifestBytes = 256 * 1024;
constexpr uint64_t kMaxScriptBytes = 1024 * 1024;
constexpr uint64_t kMaxAssetBytes = 512ull * 1024ull * 1024ull;
constexpr uint64_t kMaxWasmBytes = 4ull * 1024ull * 1024ull;
constexpr uint64_t kMaxWatSourceBytes = 4ull * 1024ull * 1024ull;
constexpr uint64_t kMaxItemDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxInputDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxActorDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxHookDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxStatusDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxDamageDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxTargetingDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxUseProfileDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxProjectileDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxAoEDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxMovementDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxCameraDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxVanillaPatchDefinitionBytes = 256 * 1024;
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
constexpr int32_t kAimReticleTextureWidth = 64;
constexpr int32_t kAimReticleTextureHeight = 64;
constexpr size_t kMaxItemModelTriangles = 4096;
constexpr float kMinDisplayListModelScale = 0.05f;
constexpr float kMaxDisplayListModelScale = 2000.0f;
constexpr int32_t kDefaultTriggerCooldownFrames = 90;
constexpr int32_t kDefaultRuntimeMemoryKb = 1024;
constexpr int32_t kDefaultRuntimeCallMs = 2;
constexpr int32_t kDefaultRuntimeFrameBudgetMs = 2;
constexpr int32_t kDefaultRuntimeHookCallsPerFrame = 256;
constexpr int32_t kDefaultRuntimeActorInstances = 64;
constexpr int32_t kDefaultRuntimeActiveStatuses = 256;
constexpr u8 kAgeReqAdult = LINK_AGE_ADULT;
constexpr u8 kAgeReqChild = LINK_AGE_CHILD;
constexpr u8 kAgeReqNone = 9;
constexpr size_t kItemIconTableSize = sizeof(gItemIcons) / sizeof(gItemIcons[0]);
constexpr size_t kExternalModInventoryCellsPerPage = 24;
constexpr size_t kExternalModInventoryMaxCellCount = 4096;
constexpr const char* kExternalModsInventorySaveSectionName = "externalModsInventory";
constexpr int32_t kExternalModsInventorySaveVersion = 1;
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
bool gExternalModPlayerStatusOverridesApplied = false;

void LoadExternalModsInventorySection() {
    ExternalModManager::Instance().LoadPersistentInventoryState();
}

void SaveExternalModsInventorySection(SaveContext* saveContext, int sectionID, bool fullSave) {
    (void)saveContext;
    (void)sectionID;
    (void)fullSave;
    ExternalModManager::Instance().SavePersistentInventoryState();
}

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

int16_t ResolveSceneIdForEntranceIndex(int16_t entranceIndex) {
    const auto isValidEntranceIndex = [](int32_t index) {
        return index >= 0 && index < static_cast<int32_t>(ARRAY_COUNT(gEntranceTable));
    };

    const int32_t setupAdjustedIndex = static_cast<int32_t>(entranceIndex) + static_cast<int32_t>(gSaveContext.sceneSetupIndex);
    if (isValidEntranceIndex(setupAdjustedIndex)) {
        return gEntranceTable[setupAdjustedIndex].scene;
    }

    const int32_t directIndex = static_cast<int32_t>(entranceIndex);
    if (isValidEntranceIndex(directIndex)) {
        return gEntranceTable[directIndex].scene;
    }

    return static_cast<int16_t>(-1);
}

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

const std::unordered_map<std::string, Ship::KbScancode> kKeyboardKeyAliases = {
    { "A", Ship::LUS_KB_A },   { "B", Ship::LUS_KB_B },   { "C", Ship::LUS_KB_C },   { "D", Ship::LUS_KB_D },
    { "E", Ship::LUS_KB_E },   { "F", Ship::LUS_KB_F },   { "G", Ship::LUS_KB_G },   { "H", Ship::LUS_KB_H },
    { "I", Ship::LUS_KB_I },   { "J", Ship::LUS_KB_J },   { "K", Ship::LUS_KB_K },   { "L", Ship::LUS_KB_L },
    { "M", Ship::LUS_KB_M },   { "N", Ship::LUS_KB_N },   { "O", Ship::LUS_KB_O },   { "P", Ship::LUS_KB_P },
    { "Q", Ship::LUS_KB_Q },   { "R", Ship::LUS_KB_R },   { "S", Ship::LUS_KB_S },   { "T", Ship::LUS_KB_T },
    { "U", Ship::LUS_KB_U },   { "V", Ship::LUS_KB_V },   { "W", Ship::LUS_KB_W },   { "X", Ship::LUS_KB_X },
    { "Y", Ship::LUS_KB_Y },   { "Z", Ship::LUS_KB_Z },   { "TAB", Ship::LUS_KB_TAB },
    { "F1", Ship::LUS_KB_F1 }, { "F2", Ship::LUS_KB_F2 }, { "F3", Ship::LUS_KB_F3 }, { "F4", Ship::LUS_KB_F4 },
    { "F5", Ship::LUS_KB_F5 }, { "F6", Ship::LUS_KB_F6 }, { "F7", Ship::LUS_KB_F7 }, { "F8", Ship::LUS_KB_F8 },
    { "F9", Ship::LUS_KB_F9 }, { "F10", Ship::LUS_KB_F10 }, { "F11", Ship::LUS_KB_F11 }, { "F12", Ship::LUS_KB_F12 },
};

const std::unordered_set<Ship::KbScancode> kReservedDefaultKeyboardScancodes = {
    Ship::LUS_KB_F1, Ship::LUS_KB_F5, Ship::LUS_KB_F6, Ship::LUS_KB_F7, Ship::LUS_KB_F9, Ship::LUS_KB_TAB, Ship::LUS_KB_I,
};
const std::unordered_map<std::string, ExternalModAimMouseButton> kAimMouseButtonAliases = {
    { "left", ExternalModAimMouseButton::Left },
    { "middle", ExternalModAimMouseButton::Middle },
    { "right", ExternalModAimMouseButton::Right },
    { "backward", ExternalModAimMouseButton::Backward },
    { "forward", ExternalModAimMouseButton::Forward },
};

const std::unordered_map<std::string, ExternalModAimMouseFireMode> kAimMouseFireModeAliases = {
    { "both", ExternalModAimMouseFireMode::Both },
    { "firstperson", ExternalModAimMouseFireMode::FirstPerson },
    { "first_person", ExternalModAimMouseFireMode::FirstPerson },
    { "overshoulder", ExternalModAimMouseFireMode::OverShoulder },
    { "over_shoulder", ExternalModAimMouseFireMode::OverShoulder },
};

const std::unordered_map<std::string, ExternalModAimReticleVisibility> kAimReticleVisibilityAliases = {
    { "aim_only", ExternalModAimReticleVisibility::AimOnly },
    { "aimonly", ExternalModAimReticleVisibility::AimOnly },
    { "button_hold", ExternalModAimReticleVisibility::ButtonHold },
    { "buttonhold", ExternalModAimReticleVisibility::ButtonHold },
    { "selected", ExternalModAimReticleVisibility::Selected },
};

const std::unordered_set<ExternalModActionType> kSupportedCameraHotkeyActions = {
    ExternalModActionType::ToggleAimCameraMode,
    ExternalModActionType::SetAimCameraMode,
    ExternalModActionType::SetAimCameraProfile,
};
constexpr const char* kAimCameraOverShoulderCVar = "gExternalMods.AimCamera.OverShoulderEnabled";
constexpr const char* kCoreDefaultAimCameraProfileId = "core:ots_default";

const ExternalModAimCameraProfile& GetCoreDefaultAimCameraProfile() {
    static ExternalModAimCameraProfile profile = [] {
        ExternalModAimCameraProfile p;
        p.id = kCoreDefaultAimCameraProfileId;
        p.contextsMask = 0x1F;
        p.cUpFirstPersonMode = CAM_MODE_FIRSTPERSON;
        p.bowFirstPersonMode = CAM_MODE_BOWARROW;
        p.hookshotFirstPersonMode = CAM_MODE_HOOKSHOT;
        p.slingshotFirstPersonMode = CAM_MODE_SLINGSHOT;
        p.boomerangFirstPersonMode = CAM_MODE_BOWARROW;
        p.cUpOverShoulderMode = CAM_MODE_BOWARROWZ;
        p.bowOverShoulderMode = CAM_MODE_BOWARROWZ;
        p.hookshotOverShoulderMode = CAM_MODE_BOWARROWZ;
        p.slingshotOverShoulderMode = CAM_MODE_BOWARROWZ;
        p.boomerangOverShoulderMode = CAM_MODE_BOWARROWZ;
        p.shoulder = "right";
        p.aimRay = "camera_center";
        p.reticleX = 0.5f;
        p.reticleY = 0.5f;
        return p;
    }();
    return profile;
}

std::string ResolveProfileIdForMod(const std::string& modId, const std::string& profileId) {
    if (profileId.empty()) {
        return {};
    }
    if (profileId.find(':') != std::string::npos || modId.empty()) {
        return profileId;
    }
    return modId + ":" + profileId;
}

std::string GetProfileOwnerModId(const std::string& profileId, const std::string& fallbackModId) {
    const auto separator = profileId.find(':');
    if (separator == std::string::npos || separator == 0) {
        return fallbackModId;
    }
    return profileId.substr(0, separator);
}

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

std::string NormalizeKeyboardKeyToken(std::string value) {
    value = ToUpper(value);
    value.erase(std::remove_if(value.begin(), value.end(),
                               [](unsigned char c) { return c == '_' || c == '-' || std::isspace(c) != 0; }),
                value.end());
    return value;
}

bool ParseKeyboardKeyToken(const std::string& value, int32_t& outScancode, std::string& outError) {
    if (value.empty()) {
        outError = "keyboard key token must not be empty";
        return false;
    }

    auto token = NormalizeKeyboardKeyToken(value);
    if (token.rfind("LUSKB", 0) == 0) {
        token = token.substr(5);
    } else if (token.rfind("KB", 0) == 0) {
        token = token.substr(2);
    }

    const auto it = kKeyboardKeyAliases.find(token);
    if (it == kKeyboardKeyAliases.end()) {
        outError = "unsupported keyboard key token: " + value;
        return false;
    }

    outScancode = static_cast<int32_t>(it->second);
    return true;
}

bool IsReservedDefaultKeyboardScancode(int32_t scancode) {
    return kReservedDefaultKeyboardScancodes.contains(static_cast<Ship::KbScancode>(scancode));
}

bool ParseAimCameraContextToken(const std::string& value, ExternalModAimCameraContext& outContext) {
    const auto normalized = ToLower(value);
    if (normalized == "cup" || normalized == "c_up" || normalized == "c-up" || normalized == "firstperson") {
        outContext = ExternalModAimCameraContext::CUp;
        return true;
    }
    if (normalized == "bow" || normalized == "bowarrow") {
        outContext = ExternalModAimCameraContext::Bow;
        return true;
    }
    if (normalized == "hookshot" || normalized == "longshot") {
        outContext = ExternalModAimCameraContext::Hookshot;
        return true;
    }
    if (normalized == "slingshot") {
        outContext = ExternalModAimCameraContext::Slingshot;
        return true;
    }
    if (normalized == "boomerang") {
        outContext = ExternalModAimCameraContext::Boomerang;
        return true;
    }
    return false;
}

uint8_t AimCameraContextToMask(ExternalModAimCameraContext context) {
    switch (context) {
        case ExternalModAimCameraContext::CUp:
            return 1 << 0;
        case ExternalModAimCameraContext::Bow:
            return 1 << 1;
        case ExternalModAimCameraContext::Hookshot:
            return 1 << 2;
        case ExternalModAimCameraContext::Slingshot:
            return 1 << 3;
        case ExternalModAimCameraContext::Boomerang:
            return 1 << 4;
        default:
            return 0;
    }
}

bool ParseAimCameraModeToken(const std::string& value, ExternalModAimCameraMode& outMode) {
    const auto normalized = ToLower(value);
    if (normalized == "firstperson" || normalized == "first_person") {
        outMode = ExternalModAimCameraMode::FirstPerson;
        return true;
    }
    if (normalized == "overshoulder" || normalized == "over_shoulder" || normalized == "thirdperson" ||
        normalized == "third_person") {
        outMode = ExternalModAimCameraMode::OverShoulder;
        return true;
    }
    return false;
}

bool ParseCameraModeTypeToken(const std::string& value, int16_t& outMode) {
    const auto normalized = ToLower(value);
    if (normalized == "firstperson" || normalized == "first_person") {
        outMode = CAM_MODE_FIRSTPERSON;
        return true;
    }
    if (normalized == "bowarrow" || normalized == "bow_arrow") {
        outMode = CAM_MODE_BOWARROW;
        return true;
    }
    if (normalized == "bowarrowz" || normalized == "bow_arrow_z" || normalized == "bowarrow_z") {
        outMode = CAM_MODE_BOWARROWZ;
        return true;
    }
    if (normalized == "hookshot") {
        outMode = CAM_MODE_HOOKSHOT;
        return true;
    }
    if (normalized == "slingshot") {
        outMode = CAM_MODE_SLINGSHOT;
        return true;
    }
    if (normalized == "boomerang") {
        outMode = CAM_MODE_BOWARROW;
        return true;
    }
    return false;
}

int16_t GetCameraModeForContext(const ExternalModAimCameraProfile& profile, ExternalModAimCameraContext context,
                                bool overShoulder) {
    if (overShoulder) {
        switch (context) {
            case ExternalModAimCameraContext::CUp:
                return profile.cUpOverShoulderMode;
            case ExternalModAimCameraContext::Bow:
                return profile.bowOverShoulderMode;
            case ExternalModAimCameraContext::Hookshot:
                return profile.hookshotOverShoulderMode;
            case ExternalModAimCameraContext::Slingshot:
                return profile.slingshotOverShoulderMode;
            case ExternalModAimCameraContext::Boomerang:
                return profile.boomerangOverShoulderMode;
            default:
                return CAM_MODE_BOWARROWZ;
        }
    }

    switch (context) {
        case ExternalModAimCameraContext::CUp:
            return profile.cUpFirstPersonMode;
        case ExternalModAimCameraContext::Bow:
            return profile.bowFirstPersonMode;
        case ExternalModAimCameraContext::Hookshot:
            return profile.hookshotFirstPersonMode;
        case ExternalModAimCameraContext::Slingshot:
            return profile.slingshotFirstPersonMode;
        case ExternalModAimCameraContext::Boomerang:
            return profile.boomerangFirstPersonMode;
        default:
            return CAM_MODE_FIRSTPERSON;
    }
}

bool TryResolveAimContextFromHeldItemAction(int32_t heldItemAction, ExternalModAimCameraContext& outContext) {
    switch (heldItemAction) {
        case PLAYER_IA_BOW:
        case PLAYER_IA_BOW_FIRE:
        case PLAYER_IA_BOW_ICE:
        case PLAYER_IA_BOW_LIGHT:
        case PLAYER_IA_BOW_0C:
        case PLAYER_IA_BOW_0D:
        case PLAYER_IA_BOW_0E:
            outContext = ExternalModAimCameraContext::Bow;
            return true;
        case PLAYER_IA_HOOKSHOT:
        case PLAYER_IA_LONGSHOT:
            outContext = ExternalModAimCameraContext::Hookshot;
            return true;
        case PLAYER_IA_SLINGSHOT:
            outContext = ExternalModAimCameraContext::Slingshot;
            return true;
        case PLAYER_IA_BOOMERANG:
            outContext = ExternalModAimCameraContext::Boomerang;
            return true;
        default:
            return false;
    }
}

bool ParseAimMouseButtonToken(const std::string& value, ExternalModAimMouseButton& outButton) {
    const auto normalized = ToLower(value);
    const auto it = kAimMouseButtonAliases.find(normalized);
    if (it == kAimMouseButtonAliases.end()) {
        return false;
    }
    outButton = it->second;
    return true;
}

bool ParseAimMouseFireModeToken(const std::string& value, ExternalModAimMouseFireMode& outMode) {
    const auto normalized = ToLower(value);
    const auto it = kAimMouseFireModeAliases.find(normalized);
    if (it == kAimMouseFireModeAliases.end()) {
        return false;
    }
    outMode = it->second;
    return true;
}

bool ParseAimReticleVisibilityToken(const std::string& value, ExternalModAimReticleVisibility& outVisibility) {
    const auto normalized = ToLower(value);
    const auto it = kAimReticleVisibilityAliases.find(normalized);
    if (it == kAimReticleVisibilityAliases.end()) {
        return false;
    }
    outVisibility = it->second;
    return true;
}

Ship::MouseBtn ToShipMouseButton(ExternalModAimMouseButton button) {
    switch (button) {
        case ExternalModAimMouseButton::Left:
            return Ship::LUS_MOUSE_BTN_LEFT;
        case ExternalModAimMouseButton::Middle:
            return Ship::LUS_MOUSE_BTN_MIDDLE;
        case ExternalModAimMouseButton::Right:
            return Ship::LUS_MOUSE_BTN_RIGHT;
        case ExternalModAimMouseButton::Backward:
            return Ship::LUS_MOUSE_BTN_BACKWARD;
        case ExternalModAimMouseButton::Forward:
            return Ship::LUS_MOUSE_BTN_FORWARD;
        default:
            return Ship::LUS_MOUSE_BTN_LEFT;
    }
}

bool IsItemIdEquippedOnActionButtons(int16_t itemId) {
    if (itemId == ITEM_NONE || itemId == ITEM_NONE_FE) {
        return false;
    }

    if (B_BTN_ITEM == itemId) {
        return true;
    }
    for (int32_t i = 0; i < 3; ++i) {
        if (C_BTN_ITEM(i) == itemId) {
            return true;
        }
    }
    if (CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0) != 0) {
        for (int32_t i = 0; i < 4; ++i) {
            if (DPAD_ITEM(i) == itemId) {
                return true;
            }
        }
    }

    return false;
}

bool IsSingleModActionMask(int32_t inputMask, int32_t& outResolvedMask) {
    constexpr int32_t kModActionMask = BTN_CUSTOM_MOD_ACTION1 | BTN_CUSTOM_MOD_ACTION2 | BTN_CUSTOM_MOD_ACTION3 |
                                       BTN_CUSTOM_MOD_ACTION4 | BTN_CUSTOM_MOD_ACTION5 | BTN_CUSTOM_MOD_ACTION6 |
                                       BTN_CUSTOM_MOD_ACTION7;

    const int32_t masked = inputMask & kModActionMask;
    if (masked == 0 || (inputMask & ~kModActionMask) != 0) {
        return false;
    }

    const uint32_t bits = static_cast<uint32_t>(masked);
    if ((bits & (bits - 1u)) != 0u) {
        return false;
    }

    outResolvedMask = masked;
    return true;
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
    "statuses.catalog.v1",
    "combat.damage.v1",
    "combat.targeting.v1",
    "combat.projectiles.v1",
    "combat.aoe.v1",
    "movement.profiles.v1",
    "camera.aim_profiles.v1",
    "world.queries.v1",
    "items.use_profiles.v1",
    "patches.vanilla_items.v1",
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

const std::unordered_map<std::string, ExternalModItemUseTrigger> kItemUseTriggerAliases = {
    { "onuse", ExternalModItemUseTrigger::OnUse },
    { "on_use", ExternalModItemUseTrigger::OnUse },
    { "hammergroundimpact", ExternalModItemUseTrigger::HammerGroundImpact },
    { "hammer_ground_impact", ExternalModItemUseTrigger::HammerGroundImpact },
};

const std::unordered_map<std::string, ExternalModItemPlacement> kItemPlacementAliases = {
    { "legacy", ExternalModItemPlacement::Legacy },
    { "virtual", ExternalModItemPlacement::Virtual },
};

const std::unordered_map<std::string, ExternalModAoETargetScope> kAoETargetScopeAliases = {
    { "all_non_player", ExternalModAoETargetScope::AllNonPlayer },
    { "allnonplayer", ExternalModAoETargetScope::AllNonPlayer },
    { "enemies_bosses", ExternalModAoETargetScope::EnemiesBosses },
    { "enemiesbosses", ExternalModAoETargetScope::EnemiesBosses },
    { "enemies_bosses_props", ExternalModAoETargetScope::EnemiesBossesProps },
    { "enemiesbossesprops", ExternalModAoETargetScope::EnemiesBossesProps },
};

const std::unordered_map<std::string, int32_t> kAssignableButtonAliases = {
    { "c_left", 1 },  { "cleft", 1 },  { "c-left", 1 },  { "btn_cleft", 1 }, { "c_down", 2 },
    { "cdown", 2 },   { "c-down", 2 }, { "btn_cdown", 2 }, { "c_right", 3 }, { "cright", 3 },
    { "c-right", 3 }, { "btn_cright", 3 }, { "d_up", 4 }, { "dup", 4 },      { "d-up", 4 },
    { "btn_dup", 4 }, { "d_down", 5 }, { "ddown", 5 }, { "d-down", 5 }, { "btn_ddown", 5 },
    { "d_left", 6 },  { "dleft", 6 },  { "d-left", 6 }, { "btn_dleft", 6 },  { "d_right", 7 },
    { "dright", 7 },  { "d-right", 7 }, { "btn_dright", 7 },
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
    { "ITEM_STICK", ITEM_STICK },
    { "ITEM_DEKU_STICK", ITEM_STICK },
    { "ITEM_SLINGSHOT", ITEM_SLINGSHOT },
    { "ITEM_FAIRY_SLINGSHOT", ITEM_SLINGSHOT },
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

bool ParseItemUseTrigger(const nlohmann::json& value, ExternalModItemUseTrigger& outTrigger, std::string& outError) {
    if (!value.is_string()) {
        outError = "must be string";
        return false;
    }

    const auto normalized = ToLower(value.get<std::string>());
    const auto it = kItemUseTriggerAliases.find(normalized);
    if (it == kItemUseTriggerAliases.end()) {
        outError = "unsupported value: " + value.get<std::string>() + " (expected onUse|hammerGroundImpact)";
        return false;
    }

    outTrigger = it->second;
    return true;
}

bool ParseItemPlacement(const nlohmann::json& value, ExternalModItemPlacement& outPlacement, std::string& outError) {
    if (!value.is_string()) {
        outError = "must be string";
        return false;
    }

    const auto normalized = ToLower(value.get<std::string>());
    const auto it = kItemPlacementAliases.find(normalized);
    if (it == kItemPlacementAliases.end()) {
        outError = "unsupported value: " + value.get<std::string>() + " (expected legacy|virtual)";
        return false;
    }

    outPlacement = it->second;
    return true;
}

bool ParseAoETargetScopeToken(const std::string& value, ExternalModAoETargetScope& outScope) {
    const auto normalized = ToLower(value);
    const auto it = kAoETargetScopeAliases.find(normalized);
    if (it == kAoETargetScopeAliases.end()) {
        return false;
    }

    outScope = it->second;
    return true;
}

uint8_t MakeButtonMask(int32_t buttonIndex) {
    if (buttonIndex < 1 || buttonIndex > 7) {
        return 0;
    }
    return static_cast<uint8_t>(1u << (buttonIndex - 1));
}

bool ParseAssignableButtonName(const std::string& buttonName, int32_t& outButtonIndex) {
    const auto it = kAssignableButtonAliases.find(ToLower(buttonName));
    if (it == kAssignableButtonAliases.end()) {
        return false;
    }
    outButtonIndex = it->second;
    return true;
}

std::string BuildAssignableButtonsList() {
    return "C_LEFT,C_DOWN,C_RIGHT,D_UP,D_DOWN,D_LEFT,D_RIGHT";
}

bool ApplySingleJsonPatchOp(nlohmann::json& target, const nlohmann::json& op, std::string& outError) {
    try {
        nlohmann::json patchArray = nlohmann::json::array();
        patchArray.push_back(op);
        target = target.patch(patchArray);
        return true;
    } catch (const std::exception& ex) {
        outError = ex.what();
        return false;
    }
}

bool ApplyVanillaPatchMergeOp(nlohmann::json& target, const nlohmann::json& op, std::string& outError) {
    if (!op.contains("path") || !op["path"].is_string()) {
        outError = "merge operation requires string path";
        return false;
    }
    if (!op.contains("value") || !op["value"].is_object()) {
        outError = "merge operation requires object value";
        return false;
    }

    const std::string pointerPath = op["path"].get<std::string>();
    const nlohmann::json patchValue = op["value"];
    const nlohmann::json::json_pointer pointer(pointerPath);

    if (pointerPath.empty() || pointerPath == "/") {
        if (!target.is_object()) {
            outError = "merge at root requires object target";
            return false;
        }
        target.merge_patch(patchValue);
        return true;
    }

    if (target.contains(pointer)) {
        auto& node = target[pointer];
        if (!node.is_object()) {
            node = nlohmann::json::object();
        }
        node.merge_patch(patchValue);
    } else {
        target[pointer] = patchValue;
    }

    return true;
}

bool TryApplyVanillaItemPatchesToJson(nlohmann::json& itemsJson, const nlohmann::json& patchJson, std::string& outError) {
    nlohmann::json operations;
    if (patchJson.is_array()) {
        operations = patchJson;
    } else if (patchJson.is_object()) {
        if (patchJson.contains("ops")) {
            operations = patchJson["ops"];
        } else if (patchJson.contains("operations")) {
            operations = patchJson["operations"];
        } else if (patchJson.contains("op")) {
            operations = nlohmann::json::array({ patchJson });
        } else {
            outError = "patch object must define ops/operations array";
            return false;
        }
    } else {
        outError = "patch document must be object or array";
        return false;
    }

    if (!operations.is_array()) {
        outError = "patch operations must be array";
        return false;
    }

    for (size_t i = 0; i < operations.size(); ++i) {
        const auto& operation = operations[i];
        if (!operation.is_object()) {
            outError = "patch op[" + std::to_string(i) + "] must be object";
            return false;
        }
        if (!operation.contains("op") || !operation["op"].is_string()) {
            outError = "patch op[" + std::to_string(i) + "] missing string field: op";
            return false;
        }

        const std::string opName = ToLower(operation["op"].get<std::string>());
        std::string applyError;
        if (opName == "merge") {
            if (!ApplyVanillaPatchMergeOp(itemsJson, operation, applyError)) {
                outError = "patch op[" + std::to_string(i) + "] merge failed: " + applyError;
                return false;
            }
            continue;
        }

        if (opName != "add" && opName != "remove" && opName != "replace") {
            outError = "patch op[" + std::to_string(i) + "] unsupported operation: " + opName;
            return false;
        }

        nlohmann::json normalized = operation;
        normalized["op"] = opName;
        if (!ApplySingleJsonPatchOp(itemsJson, normalized, applyError)) {
            outError = "patch op[" + std::to_string(i) + "] failed: " + applyError;
            return false;
        }
    }

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

    std::string stringValue = value.get<std::string>();
    auto trimWhitespace = [](std::string& text) {
        size_t start = 0;
        while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start]))) {
            ++start;
        }
        size_t end = text.size();
        while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1]))) {
            --end;
        }
        text = text.substr(start, end - start);
    };

    trimWhitespace(stringValue);

    while (!stringValue.empty()) {
        const auto* bytes = reinterpret_cast<const uint8_t*>(stringValue.data());
        const size_t size = stringValue.size();

        if (size >= 3 && bytes[0] == 0xEF && bytes[1] == 0xBB && bytes[2] == 0xBF) {
            stringValue.erase(0, 3);
            continue;
        }
        if (size >= 2 && ((bytes[0] == 0xFF && bytes[1] == 0xFE) || (bytes[0] == 0xFE && bytes[1] == 0xFF))) {
            stringValue.erase(0, 2);
            continue;
        }
        if (bytes[0] == 0x00) {
            stringValue.erase(0, 1);
            continue;
        }
        break;
    }

    trimWhitespace(stringValue);

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

bool ParseStatusIdToken(const std::string& value, ExternalModStatusType& outStatusType) {
    const auto normalized = ToLower(value);
    if (normalized == "core:burning" || normalized == "core:fire" || normalized == "fire") {
        outStatusType = ExternalModStatusType::Fire;
        return true;
    }
    if (normalized == "core:freeze" || normalized == "freeze" || normalized == "core:frozen") {
        outStatusType = ExternalModStatusType::Freeze;
        return true;
    }
    if (normalized == "core:stun" || normalized == "stun") {
        outStatusType = ExternalModStatusType::Stun;
        return true;
    }
    if (normalized == "core:poison" || normalized == "poison") {
        outStatusType = ExternalModStatusType::Poison;
        return true;
    }
    if (normalized == "core:blind" || normalized == "blind") {
        outStatusType = ExternalModStatusType::Blind;
        return true;
    }
    if (normalized == "core:speed" || normalized == "speed") {
        outStatusType = ExternalModStatusType::Speed;
        return true;
    }
    if (normalized == "core:slow" || normalized == "slow") {
        outStatusType = ExternalModStatusType::Slow;
        return true;
    }
    if (normalized == "core:high_jump" || normalized == "highjump" || normalized == "high_jump") {
        outStatusType = ExternalModStatusType::HighJump;
        return true;
    }
    if (normalized == "core:strength" || normalized == "strength") {
        outStatusType = ExternalModStatusType::Strength;
        return true;
    }
    if (normalized == "core:weakness" || normalized == "weakness") {
        outStatusType = ExternalModStatusType::Weakness;
        return true;
    }
    if (normalized.find(':') != std::string::npos) {
        outStatusType = ExternalModStatusType::Custom;
        return true;
    }
    return false;
}

bool IsNamespacedCatalogId(const std::string& id) {
    const auto separatorIndex = id.find(':');
    if (separatorIndex == std::string::npos || separatorIndex == 0 || separatorIndex >= (id.size() - 1)) {
        return false;
    }
    return true;
}

bool ParseStatusTargetToken(const std::string& value, ExternalModStatusTarget& outTarget) {
    const auto normalized = ToLower(value);
    if (normalized == "fronttarget" || normalized == "front_target" || normalized == "front") {
        outTarget = ExternalModStatusTarget::FrontTarget;
        return true;
    }
    if (normalized == "self") {
        outTarget = ExternalModStatusTarget::Self;
        return true;
    }
    if (normalized == "player") {
        outTarget = ExternalModStatusTarget::Player;
        return true;
    }
    if (normalized == "actorhandle" || normalized == "actor_handle") {
        outTarget = ExternalModStatusTarget::ActorHandle;
        return true;
    }
    return false;
}

bool ParseTargetingModeToken(const std::string& value, ExternalModTargetingMode& outMode) {
    const auto normalized = ToLower(value);
    if (normalized == "fronttarget" || normalized == "front_target" || normalized == "front") {
        outMode = ExternalModTargetingMode::FrontTarget;
        return true;
    }
    if (normalized == "lockedontarget" || normalized == "locked_on_target" || normalized == "lockedon") {
        outMode = ExternalModTargetingMode::LockedOnTarget;
        return true;
    }
    if (normalized == "raycast" || normalized == "ray") {
        outMode = ExternalModTargetingMode::Raycast;
        return true;
    }
    if (normalized == "cone") {
        outMode = ExternalModTargetingMode::Cone;
        return true;
    }
    if (normalized == "sphere") {
        outMode = ExternalModTargetingMode::Sphere;
        return true;
    }
    if (normalized == "self") {
        outMode = ExternalModTargetingMode::Self;
        return true;
    }
    if (normalized == "player") {
        outMode = ExternalModTargetingMode::Player;
        return true;
    }
    return false;
}

bool ParseMovementModeToken(const std::string& value, ExternalModMovementMode& outMode) {
    const auto normalized = ToLower(value);
    if (normalized == "modifier") {
        outMode = ExternalModMovementMode::Modifier;
        return true;
    }
    if (normalized == "surf") {
        outMode = ExternalModMovementMode::Surf;
        return true;
    }
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

    if (apiVersion < kExternalModApiVersionV3) {
        outError = "Unsupported action for apiVersion 3: " + actionType;
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

    if (actionType == "igniteFrontTarget" || actionType == "freezeFrontTarget") {
        outError = actionType + " was removed in apiVersion 3; use applyStatus with statusId";
        return false;
    }

    if (actionType == "applyStatus") {
        outAction.type = ExternalModActionType::ApplyStatus;
        outAction.statusTarget = ExternalModStatusTarget::FrontTarget;
        outAction.hasStatusTarget = true;

        if (!ValidateRequiredString(json, "status", outAction.statusId, outError)) {
            return false;
        }
        if (!ParseStatusIdToken(outAction.statusId, outAction.statusType)) {
            outError = "applyStatus.status unsupported: " + outAction.statusId;
            return false;
        }

        if (json.contains("target")) {
            if (!json["target"].is_string()) {
                outError = "applyStatus.target must be string";
                return false;
            }
            if (!ParseStatusTargetToken(json["target"].get<std::string>(), outAction.statusTarget)) {
                outError = "applyStatus.target must be frontTarget|self|player|actorHandle";
                return false;
            }
        }

        if ((json.contains("actorHandle") || json.contains("handle")) &&
            !parseActorHandle(outAction.actorHandle)) {
            return false;
        }
        if (outAction.statusTarget == ExternalModStatusTarget::ActorHandle && outAction.actorHandle == 0) {
            outError = "applyStatus.target=actorHandle requires actorHandle";
            return false;
        }

        if (json.contains("itemId") && !ValidateRequiredString(json, "itemId", outAction.itemId, outError)) {
            return false;
        }
        if (json.contains("requiresItemId") &&
            !ValidateRequiredString(json, "requiresItemId", outAction.itemId, outError)) {
            return false;
        }
        if (json.contains("durationFrames")) {
            if (!json["durationFrames"].is_number_integer()) {
                outError = "applyStatus.durationFrames must be integer";
                return false;
            }
            outAction.durationFrames = std::max(1, json["durationFrames"].get<int32_t>());
        }
        if (json.contains("tickFrames")) {
            if (!json["tickFrames"].is_number_integer()) {
                outError = "applyStatus.tickFrames must be integer";
                return false;
            }
            outAction.tickFrames = std::max(1, json["tickFrames"].get<int32_t>());
        }
        if (json.contains("damagePerTick")) {
            if (!json["damagePerTick"].is_number_integer()) {
                outError = "applyStatus.damagePerTick must be integer";
                return false;
            }
            outAction.damagePerTick = std::max(0, json["damagePerTick"].get<int32_t>());
        }
        if (json.contains("shakeFrames")) {
            if (!json["shakeFrames"].is_number_integer()) {
                outError = "applyStatus.shakeFrames must be integer";
                return false;
            }
            outAction.shakeFrames = std::max(0, json["shakeFrames"].get<int32_t>());
        }
        if (json.contains("range")) {
            if (!json["range"].is_number()) {
                outError = "applyStatus.range must be numeric";
                return false;
            }
            outAction.freezeRange = std::max(1.0f, json["range"].get<float>());
            outAction.range = outAction.freezeRange;
        }
        if (json.contains("intensity")) {
            if (!json["intensity"].is_number_integer()) {
                outError = "applyStatus.intensity must be integer";
                return false;
            }
            outAction.intensity = std::clamp(json["intensity"].get<int32_t>(), 0, 255);
        }
        return true;
    }

    if (actionType == "clearStatus") {
        outAction.type = ExternalModActionType::ClearStatus;
        outAction.statusTarget = ExternalModStatusTarget::FrontTarget;
        outAction.hasStatusTarget = true;
        if (json.contains("status")) {
            if (!ValidateRequiredString(json, "status", outAction.statusId, outError)) {
                return false;
            }
            if (!ParseStatusIdToken(outAction.statusId, outAction.statusType)) {
                outError = "clearStatus.status unsupported: " + outAction.statusId;
                return false;
            }
        }
        if (json.contains("target")) {
            if (!json["target"].is_string()) {
                outError = "clearStatus.target must be string";
                return false;
            }
            if (!ParseStatusTargetToken(json["target"].get<std::string>(), outAction.statusTarget)) {
                outError = "clearStatus.target must be frontTarget|self|player|actorHandle";
                return false;
            }
        }
        if ((json.contains("actorHandle") || json.contains("handle")) &&
            !parseActorHandle(outAction.actorHandle)) {
            return false;
        }
        return true;
    }

    if (actionType == "clearAllStatuses") {
        outAction.type = ExternalModActionType::ClearAllStatuses;
        outAction.statusTarget = ExternalModStatusTarget::FrontTarget;
        outAction.hasStatusTarget = true;
        if (json.contains("target")) {
            if (!json["target"].is_string()) {
                outError = "clearAllStatuses.target must be string";
                return false;
            }
            if (!ParseStatusTargetToken(json["target"].get<std::string>(), outAction.statusTarget)) {
                outError = "clearAllStatuses.target must be frontTarget|self|player|actorHandle";
                return false;
            }
        }
        if ((json.contains("actorHandle") || json.contains("handle")) &&
            !parseActorHandle(outAction.actorHandle)) {
            return false;
        }
        return true;
    }

    if (actionType == "useItemProfile") {
        outAction.type = ExternalModActionType::UseItemProfile;
        if (json.contains("profile")) {
            return ValidateRequiredString(json, "profile", outAction.itemUseProfileId, outError);
        }
        if (json.contains("useProfile")) {
            return ValidateRequiredString(json, "useProfile", outAction.itemUseProfileId, outError);
        }
        if (json.contains("profileId")) {
            return ValidateRequiredString(json, "profileId", outAction.itemUseProfileId, outError);
        }
        outError = "useItemProfile requires profile";
        return false;
    }

    if (actionType == "dealDamage") {
        outAction.type = ExternalModActionType::DealDamage;
        if (json.contains("profile")) {
            if (!ValidateRequiredString(json, "profile", outAction.damageProfileId, outError)) {
                return false;
            }
        } else if (json.contains("damageProfileId")) {
            if (!ValidateRequiredString(json, "damageProfileId", outAction.damageProfileId, outError)) {
                return false;
            }
        } else {
            outError = "dealDamage requires profile";
            return false;
        }
        if (json.contains("target") && json["target"].is_string()) {
            ParseStatusTargetToken(json["target"].get<std::string>(), outAction.statusTarget);
        }
        if ((json.contains("actorHandle") || json.contains("handle")) &&
            !parseActorHandle(outAction.actorHandle)) {
            return false;
        }
        return true;
    }

    if (actionType == "spawnProjectile") {
        outAction.type = ExternalModActionType::SpawnProjectile;
        if (json.contains("projectile")) {
            return ValidateRequiredString(json, "projectile", outAction.projectileProfileId, outError);
        }
        if (json.contains("profile")) {
            return ValidateRequiredString(json, "profile", outAction.projectileProfileId, outError);
        }
        if (json.contains("projectileProfileId")) {
            return ValidateRequiredString(json, "projectileProfileId", outAction.projectileProfileId, outError);
        }
        outError = "spawnProjectile requires projectile profile";
        return false;
    }

    if (actionType == "spawnAoE") {
        outAction.type = ExternalModActionType::SpawnAoE;
        if (json.contains("aoe")) {
            return ValidateRequiredString(json, "aoe", outAction.aoeProfileId, outError);
        }
        if (json.contains("profile")) {
            return ValidateRequiredString(json, "profile", outAction.aoeProfileId, outError);
        }
        if (json.contains("aoeProfileId")) {
            return ValidateRequiredString(json, "aoeProfileId", outAction.aoeProfileId, outError);
        }
        outError = "spawnAoE requires aoe profile";
        return false;
    }

    if (actionType == "applyMovementProfile") {
        outAction.type = ExternalModActionType::ApplyMovementProfile;
        if (json.contains("movement")) {
            if (!ValidateRequiredString(json, "movement", outAction.movementProfileId, outError)) {
                return false;
            }
        } else if (json.contains("profile")) {
            if (!ValidateRequiredString(json, "profile", outAction.movementProfileId, outError)) {
                return false;
            }
        } else if (json.contains("movementProfileId")) {
            if (!ValidateRequiredString(json, "movementProfileId", outAction.movementProfileId, outError)) {
                return false;
            }
        } else {
            outError = "applyMovementProfile requires movement profile";
            return false;
        }
        if (json.contains("durationFrames")) {
            if (!json["durationFrames"].is_number_integer()) {
                outError = "applyMovementProfile.durationFrames must be integer";
                return false;
            }
            outAction.durationFrames = std::max(0, json["durationFrames"].get<int32_t>());
        }
        return true;
    }

    if (actionType == "applyImpulse") {
        outAction.type = ExternalModActionType::ApplyImpulse;
        if (json.contains("mode") && !ValidateRequiredString(json, "mode", outAction.variableValue, outError)) {
            return false;
        }
        if (json.contains("strength")) {
            if (!json["strength"].is_number()) {
                outError = "applyImpulse.strength must be numeric";
                return false;
            }
            outAction.impulseStrength = json["strength"].get<float>();
        }
        return true;
    }

    if (actionType == "getGroundInfo") {
        outAction.type = ExternalModActionType::GetGroundInfo;
        return true;
    }

    if (actionType == "raycast") {
        outAction.type = ExternalModActionType::Raycast;
        if (json.contains("range")) {
            if (!json["range"].is_number()) {
                outError = "raycast.range must be numeric";
                return false;
            }
            outAction.range = std::max(1.0f, json["range"].get<float>());
        }
        return true;
    }

    if (actionType == "raycastAll") {
        outAction.type = ExternalModActionType::RaycastAll;
        if (json.contains("range")) {
            if (!json["range"].is_number()) {
                outError = "raycastAll.range must be numeric";
                return false;
            }
            outAction.range = std::max(1.0f, json["range"].get<float>());
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

    if (actionType == "toggleAimCameraMode") {
        outAction.type = ExternalModActionType::ToggleAimCameraMode;
        if (json.contains("profileId")) {
            if (!ValidateRequiredString(json, "profileId", outAction.aimCameraProfileId, outError)) {
                return false;
            }
        }
        if (json.contains("itemId") && !ValidateRequiredString(json, "itemId", outAction.itemId, outError)) {
            return false;
        }
        if (json.contains("requiresItemId") &&
            !ValidateRequiredString(json, "requiresItemId", outAction.itemId, outError)) {
            return false;
        }
        return true;
    }

    if (actionType == "setAimCameraMode") {
        outAction.type = ExternalModActionType::SetAimCameraMode;
        std::string modeToken;
        if (!ValidateRequiredString(json, "mode", modeToken, outError)) {
            return false;
        }
        if (!ParseAimCameraModeToken(modeToken, outAction.aimCameraMode)) {
            outError = "setAimCameraMode.mode must be firstPerson|overShoulder";
            return false;
        }
        if (json.contains("profileId")) {
            if (!ValidateRequiredString(json, "profileId", outAction.aimCameraProfileId, outError)) {
                return false;
            }
        }
        if (json.contains("itemId") && !ValidateRequiredString(json, "itemId", outAction.itemId, outError)) {
            return false;
        }
        if (json.contains("requiresItemId") &&
            !ValidateRequiredString(json, "requiresItemId", outAction.itemId, outError)) {
            return false;
        }
        return true;
    }

    if (actionType == "setAimCameraProfile") {
        outAction.type = ExternalModActionType::SetAimCameraProfile;
        if (!ValidateRequiredString(json, "profileId", outAction.aimCameraProfileId, outError)) {
            return false;
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

int32_t GetIntParamOrDefault(const std::unordered_map<std::string, float>& params, const char* key, int32_t defaultValue,
                             int32_t minValue, int32_t maxValue) {
    const float value = GetParamOrDefault(params, key, static_cast<float>(defaultValue));
    return std::clamp(static_cast<int32_t>(std::lround(value)), minValue, maxValue);
}

Actor* ResolveActorHitByPlayerMeleeQuad(const ColliderQuad& quad) {
    if ((quad.base.atFlags & AT_HIT) == 0) {
        return nullptr;
    }

    Actor* hitActor = nullptr;
    if (quad.info.atHit != nullptr) {
        hitActor = quad.info.atHit->actor;
    }
    if (hitActor == nullptr) {
        hitActor = quad.base.at;
    }
    if (hitActor == nullptr || hitActor->id == ACTOR_PLAYER || hitActor->update == nullptr) {
        return nullptr;
    }

    return hitActor;
}

bool IsPlayerUsingDekuStick(const Player* player) {
    if (player == nullptr) {
        return false;
    }

    return player->heldItemId == ITEM_STICK || player->heldItemAction == PLAYER_IA_DEKU_STICK ||
           player->itemAction == PLAYER_IA_DEKU_STICK;
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

constexpr std::array<ExternalModItemSlotConfig, 24> kSupportedItemSlotConfigs = {{
    { ExternalModItemSlot::Stick, "SLOT_STICK", SLOT_STICK, ITEM_STICK, ITEM_STICK,
      { ITEM_STICK, ITEM_NONE, ITEM_NONE }, kAgeReqChild },
    { ExternalModItemSlot::Nut, "SLOT_NUT", SLOT_NUT, ITEM_NUT, ITEM_NUT,
      { ITEM_NUT, ITEM_NONE, ITEM_NONE }, kAgeReqNone },
    { ExternalModItemSlot::Bomb, "SLOT_BOMB", SLOT_BOMB, ITEM_BOMB, ITEM_BOMB,
      { ITEM_BOMB, ITEM_NONE, ITEM_NONE }, kAgeReqNone },
    { ExternalModItemSlot::Bow, "SLOT_BOW", SLOT_BOW, ITEM_BOW, ITEM_BOW,
      { ITEM_BOW, ITEM_NONE, ITEM_NONE }, kAgeReqAdult },
    { ExternalModItemSlot::FireArrow, "SLOT_ARROW_FIRE", SLOT_ARROW_FIRE, ITEM_ARROW_FIRE, ITEM_BOW,
      { ITEM_ARROW_FIRE, ITEM_BOW_ARROW_FIRE, ITEM_NONE }, kAgeReqAdult },
    { ExternalModItemSlot::DinsFire, "SLOT_DINS_FIRE", SLOT_DINS_FIRE, ITEM_DINS_FIRE, ITEM_NONE,
      { ITEM_DINS_FIRE, ITEM_NONE, ITEM_NONE }, kAgeReqNone },
    { ExternalModItemSlot::Slingshot, "SLOT_SLINGSHOT", SLOT_SLINGSHOT, ITEM_SLINGSHOT, ITEM_SLINGSHOT,
      { ITEM_SLINGSHOT, ITEM_NONE, ITEM_NONE }, kAgeReqChild },
    { ExternalModItemSlot::Ocarina, "SLOT_OCARINA", SLOT_OCARINA, ITEM_OCARINA_TIME, ITEM_NONE,
      { ITEM_OCARINA_FAIRY, ITEM_OCARINA_TIME, ITEM_NONE }, kAgeReqNone },
    { ExternalModItemSlot::Bombchu, "SLOT_BOMBCHU", SLOT_BOMBCHU, ITEM_BOMBCHU, ITEM_BOMBCHU,
      { ITEM_BOMBCHU, ITEM_NONE, ITEM_NONE }, kAgeReqNone },
    { ExternalModItemSlot::Hookshot, "SLOT_HOOKSHOT", SLOT_HOOKSHOT, ITEM_HOOKSHOT, ITEM_NONE,
      { ITEM_HOOKSHOT, ITEM_LONGSHOT, ITEM_NONE }, kAgeReqAdult },
    { ExternalModItemSlot::IceArrow, "SLOT_ARROW_ICE", SLOT_ARROW_ICE, ITEM_ARROW_ICE, ITEM_BOW,
      { ITEM_ARROW_ICE, ITEM_BOW_ARROW_ICE, ITEM_NONE }, kAgeReqAdult },
    { ExternalModItemSlot::FaroresWind, "SLOT_FARORES_WIND", SLOT_FARORES_WIND, ITEM_FARORES_WIND, ITEM_NONE,
      { ITEM_FARORES_WIND, ITEM_NONE, ITEM_NONE }, kAgeReqNone },
    { ExternalModItemSlot::Boomerang, "SLOT_BOOMERANG", SLOT_BOOMERANG, ITEM_BOOMERANG, ITEM_NONE,
      { ITEM_BOOMERANG, ITEM_NONE, ITEM_NONE }, kAgeReqChild },
    { ExternalModItemSlot::Lens, "SLOT_LENS", SLOT_LENS, ITEM_LENS, ITEM_NONE,
      { ITEM_LENS, ITEM_NONE, ITEM_NONE }, kAgeReqNone },
    { ExternalModItemSlot::Bean, "SLOT_BEAN", SLOT_BEAN, ITEM_BEAN, ITEM_BEAN,
      { ITEM_BEAN, ITEM_NONE, ITEM_NONE }, kAgeReqChild },
    { ExternalModItemSlot::Hammer, "SLOT_HAMMER", SLOT_HAMMER, ITEM_HAMMER, ITEM_NONE,
      { ITEM_HAMMER, ITEM_NONE, ITEM_NONE }, kAgeReqAdult },
    { ExternalModItemSlot::LightArrow, "SLOT_ARROW_LIGHT", SLOT_ARROW_LIGHT, ITEM_ARROW_LIGHT, ITEM_BOW,
      { ITEM_ARROW_LIGHT, ITEM_BOW_ARROW_LIGHT, ITEM_NONE }, kAgeReqAdult },
    { ExternalModItemSlot::NayrusLove, "SLOT_NAYRUS_LOVE", SLOT_NAYRUS_LOVE, ITEM_NAYRUS_LOVE, ITEM_NONE,
      { ITEM_NAYRUS_LOVE, ITEM_NONE, ITEM_NONE }, kAgeReqNone },
    { ExternalModItemSlot::Bottle1, "SLOT_BOTTLE_1", SLOT_BOTTLE_1, ITEM_BOTTLE, ITEM_NONE,
      { ITEM_BOTTLE, ITEM_NONE, ITEM_NONE }, kAgeReqNone },
    { ExternalModItemSlot::Bottle2, "SLOT_BOTTLE_2", SLOT_BOTTLE_2, ITEM_BOTTLE, ITEM_NONE,
      { ITEM_BOTTLE, ITEM_NONE, ITEM_NONE }, kAgeReqNone },
    { ExternalModItemSlot::Bottle3, "SLOT_BOTTLE_3", SLOT_BOTTLE_3, ITEM_BOTTLE, ITEM_NONE,
      { ITEM_BOTTLE, ITEM_NONE, ITEM_NONE }, kAgeReqNone },
    { ExternalModItemSlot::Bottle4, "SLOT_BOTTLE_4", SLOT_BOTTLE_4, ITEM_BOTTLE, ITEM_NONE,
      { ITEM_BOTTLE, ITEM_NONE, ITEM_NONE }, kAgeReqNone },
    { ExternalModItemSlot::TradeAdult, "SLOT_TRADE_ADULT", SLOT_TRADE_ADULT, ITEM_TRADE_ADULT, ITEM_NONE,
      { ITEM_TRADE_ADULT, ITEM_CLAIM_CHECK, ITEM_NONE }, kAgeReqAdult },
    { ExternalModItemSlot::TradeChild, "SLOT_TRADE_CHILD", SLOT_TRADE_CHILD, ITEM_TRADE_CHILD, ITEM_NONE,
      { ITEM_TRADE_CHILD, ITEM_MASK_TRUTH, ITEM_NONE }, kAgeReqChild },
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

    const auto normalized = ToLower(slotName);
    if (normalized == "slot_deku_stick") {
        return FindItemSlotConfig(ExternalModItemSlot::Stick);
    }
    if (normalized == "slot_deku_nut") {
        return FindItemSlotConfig(ExternalModItemSlot::Nut);
    }
    if (normalized == "slot_lens_of_truth") {
        return FindItemSlotConfig(ExternalModItemSlot::Lens);
    }
    if (normalized == "slot_magic_bean") {
        return FindItemSlotConfig(ExternalModItemSlot::Bean);
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

const uint8_t* ResolveItemIconPointerForId(int32_t itemId, bool preferVanilla) {
    if (!IsItemIconIndexValid(itemId)) {
        return nullptr;
    }

    const size_t iconIndex = static_cast<size_t>(itemId);
    if (preferVanilla && gVanillaItemIconsCaptured && gVanillaItemIcons[iconIndex] != nullptr) {
        return reinterpret_cast<const uint8_t*>(gVanillaItemIcons[iconIndex]);
    }

    if (gItemIcons[iconIndex] == nullptr) {
        return nullptr;
    }

    return reinterpret_cast<const uint8_t*>(gItemIcons[iconIndex]);
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
    int32_t selectedLoadPriority = std::numeric_limits<int32_t>::min();
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

            if (selectedDefinition == nullptr || package.manifest.loadPriority > selectedLoadPriority ||
                (package.manifest.loadPriority == selectedLoadPriority && package.manifest.id < selectedModId)) {
                selectedDefinition = &definition;
                selectedLoadPriority = package.manifest.loadPriority;
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
                         if (lhs->manifest.loadPriority != rhs->manifest.loadPriority) {
                             return lhs->manifest.loadPriority > rhs->manifest.loadPriority;
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
    int32_t selectedLoadPriority = std::numeric_limits<int32_t>::min();
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

            if (selectedDefinition == nullptr || package.manifest.loadPriority > selectedLoadPriority ||
                (package.manifest.loadPriority == selectedLoadPriority && package.manifest.id < selectedModId)) {
                selectedDefinition = &definition;
                selectedPackage = &package;
                selectedLoadPriority = package.manifest.loadPriority;
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

    for (const auto& package : packages) {
        if (!package.runtime.enabled) {
            continue;
        }
        for (const auto& definition : package.runtime.itemDefinitions) {
            if (!definition.granted || !definition.hasGrantItemId) {
                continue;
            }

            u8 ageRequirement = kAgeReqNone;
            if (definition.agePolicy == ExternalModItemAgePolicy::RespectVanilla && definition.hasSlot) {
                const auto* config = FindItemSlotConfig(definition.slot);
                if (config != nullptr) {
                    ageRequirement = config->vanillaAgeReq;
                }
            }

            // gItemAgeReqs only covers the equippable item range through ITEM_SWORD_KNIFE.
            const int32_t grantItemId = definition.grantItemId;
            if (grantItemId >= ITEM_STICK && grantItemId <= ITEM_SWORD_KNIFE) {
                gItemAgeReqs[grantItemId] = ageRequirement;
            }
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

    const auto* definition = ExternalModContentRegistry::FindItemDefinitionById(package.runtime, action.itemId);
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

std::string StatusTypeToString(ExternalModStatusType statusType) {
    switch (statusType) {
        case ExternalModStatusType::Fire:
            return "fire";
        case ExternalModStatusType::Freeze:
            return "freeze";
        case ExternalModStatusType::Stun:
            return "stun";
        case ExternalModStatusType::Poison:
            return "poison";
        case ExternalModStatusType::Blind:
            return "blind";
        case ExternalModStatusType::Speed:
            return "speed";
        case ExternalModStatusType::Slow:
            return "slow";
        case ExternalModStatusType::HighJump:
            return "high_jump";
        case ExternalModStatusType::Strength:
            return "strength";
        case ExternalModStatusType::Weakness:
            return "weakness";
        case ExternalModStatusType::Custom:
            return "custom";
        default:
            return "unknown";
    }
}

bool IsImportantNpcForStatus(int16_t actorId) {
    switch (actorId) {
        case ACTOR_EN_GS:
        case ACTOR_EN_GE1:
        case ACTOR_EN_KZ:
        case ACTOR_EN_DU:
            return true;
        default:
            return false;
    }
}

bool ShouldProtectStatusTarget(const Actor* actor, ExternalModStatusType statusType) {
    if (actor == nullptr) {
        return true;
    }
    if (actor->category == ACTORCAT_PLAYER || actor->category == ACTORCAT_BG) {
        return true;
    }

    int32_t protectImportantNpcs = CVarGetInteger("ExternalMods.StatusProtectImportantNpcs", 1);
    if (statusType == ExternalModStatusType::Freeze || statusType == ExternalModStatusType::Stun) {
        protectImportantNpcs = CVarGetInteger("ExternalMods.StatusProtectImportantNpcs",
                                              CVarGetInteger("ExternalMods.FreezeProtectImportantNpcs", 0));
    }
    return protectImportantNpcs != 0 && IsImportantNpcForStatus(actor->id);
}

void ApplyStatusColorFilter(Actor* actor, ExternalModStatusType statusType, int32_t intensity) {
    if (actor == nullptr) {
        return;
    }
    const int16_t clampedIntensity = static_cast<int16_t>(std::clamp(intensity, 0, 255));
    const uint16_t filterType =
        (statusType == ExternalModStatusType::Freeze || statusType == ExternalModStatusType::Blind) ? 0x0000 : 0x4000;
    Actor_SetColorFilter(actor, filterType, clampedIntensity, 0, 2);
}

Actor* FindActorByAddress(PlayState* play, uintptr_t actorAddress, int16_t expectedActorId) {
    if (play == nullptr || actorAddress == 0) {
        return nullptr;
    }

    Actor* expectedActor = reinterpret_cast<Actor*>(actorAddress);
    for (size_t category = 0; category < ARRAY_COUNT(play->actorCtx.actorLists); ++category) {
        for (Actor* actor = play->actorCtx.actorLists[category].head; actor != nullptr; actor = actor->next) {
            if (actor != expectedActor) {
                continue;
            }
            if (expectedActorId >= 0 && actor->id != expectedActorId) {
                return nullptr;
            }
            return actor;
        }
    }

    return nullptr;
}

Actor* FindStatusTargetInFront(PlayState* play, Player* player, float range, ExternalModStatusType statusType) {
    if (play == nullptr || player == nullptr) {
        return nullptr;
    }

    constexpr float kMaxVerticalDelta = 120.0f;
    constexpr int32_t kMaxYawDelta = 0x3000;
    const float maxDistance = std::max(range, 1.0f);

    Actor* bestActor = nullptr;
    float bestScore = std::numeric_limits<float>::max();

    for (size_t category = 0; category < ARRAY_COUNT(play->actorCtx.actorLists); ++category) {
        if (category == ACTORCAT_PLAYER || category == ACTORCAT_BG) {
            continue;
        }

        for (Actor* actor = play->actorCtx.actorLists[category].head; actor != nullptr; actor = actor->next) {
            if (actor == &player->actor || actor->update == nullptr || ShouldProtectStatusTarget(actor, statusType)) {
                continue;
            }

            const float distance = Math_Vec3f_DistXYZ(&player->actor.world.pos, &actor->world.pos);
            if (distance > maxDistance) {
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

ExternalModRuntime::StatusEffectState* FindStatusStateByActor(ExternalModRuntime& runtime, Actor* actor,
                                                              ExternalModStatusType statusType, bool isPlayerTarget,
                                                              const std::string& statusId,
                                                              const std::string& sourceModId) {
    if (actor == nullptr) {
        return nullptr;
    }

    const auto actorAddress = reinterpret_cast<uintptr_t>(actor);
    const auto it = std::find_if(
        runtime.statusEffects.begin(), runtime.statusEffects.end(),
        [actorAddress, statusType, isPlayerTarget, &statusId,
         &sourceModId](const ExternalModRuntime::StatusEffectState& state) {
            return state.actorAddress == actorAddress && state.statusType == statusType &&
                   state.isPlayerTarget == isPlayerTarget && state.statusId == statusId &&
                   state.sourceModId == sourceModId;
        });
    if (it == runtime.statusEffects.end()) {
        return nullptr;
    }
    return &(*it);
}

void BeginStatusOnActor(ExternalModRuntime& runtime, PlayState* play, Actor* actor, const ExternalModAction& action,
                        ExternalModStatusType statusType, bool isPlayerTarget, const std::string& statusId,
                        const std::string& sourceModId) {
    if (play == nullptr || actor == nullptr) {
        return;
    }

    const int32_t durationFrames = std::clamp(action.durationFrames, 1, 36000);
    const int32_t shakeFrames = std::clamp(action.shakeFrames, 0, durationFrames);
    const int32_t intensity = std::clamp(action.intensity, 0, 255);

    auto* state = FindStatusStateByActor(runtime, actor, statusType, isPlayerTarget, statusId, sourceModId);
    if (state == nullptr) {
        if (runtime.statusEffects.size() >= static_cast<size_t>(std::max(1, runtime.maxActiveStatusEffects))) {
            SPDLOG_WARN("[ExternalMods] status effect cap reached ({}). Ignoring status {}", runtime.maxActiveStatusEffects,
                        StatusTypeToString(statusType));
            return;
        }

        ExternalModRuntime::StatusEffectState statusState;
        statusState.actorAddress = reinterpret_cast<uintptr_t>(actor);
        statusState.actorId = actor->id;
        statusState.statusId = statusId;
        statusState.sourceModId = sourceModId;
        statusState.statusType = statusType;
        statusState.isPlayerTarget = isPlayerTarget;
        statusState.baseX = actor->world.pos.x;
        statusState.baseY = actor->world.pos.y;
        statusState.baseZ = actor->world.pos.z;
        statusState.baseRotY = actor->shape.rot.y;
        runtime.statusEffects.push_back(statusState);
        state = &runtime.statusEffects.back();
    }
    state->statusId = statusId;
    state->sourceModId = sourceModId;

    state->framesRemaining = durationFrames;
    state->totalDurationFrames = durationFrames;
    state->tickFrames = std::clamp(action.tickFrames, 1, 36000);
    state->tickCountdown = state->tickFrames;
    state->damagePerTick = std::clamp(action.damagePerTick, 0, 255);
    state->shakeFrames = shakeFrames;
    state->intensity = intensity;
    state->speedMultiplier = std::clamp(action.speedMultiplier, 0.05f, 8.0f);
    state->jumpMultiplier = std::clamp(action.jumpMultiplier, 0.1f, 8.0f);
    state->strengthMultiplier = std::clamp(action.strengthMultiplier, 0.1f, 12.0f);
    state->weaknessMultiplier = std::clamp(action.weaknessMultiplier, 0.1f, 12.0f);
    state->blindSkipChance = std::clamp(action.blindSkipChance, 0.0f, 1.0f);
    state->blindYawJitterDeg = std::clamp(action.blindYawJitterDeg, 0.0f, 180.0f);
    state->baseX = actor->world.pos.x;
    state->baseY = actor->world.pos.y;
    state->baseZ = actor->world.pos.z;
    state->baseRotY = actor->shape.rot.y;
    state->fallbackLogged = false;

    if (statusType == ExternalModStatusType::Freeze) {
        actor->freezeTimer = static_cast<uint16_t>(std::clamp(durationFrames, 1, 0xFFFF));
    }
    if (statusType == ExternalModStatusType::Stun) {
        actor->freezeTimer = static_cast<uint16_t>(std::clamp(std::min(durationFrames, 8), 1, 0xFFFF));
    }

    ApplyStatusColorFilter(actor, statusType, intensity);
}

void ApplyStatusDamage(PlayState* play, Actor* actor, int32_t damagePerTick) {
    if (play == nullptr || actor == nullptr || damagePerTick <= 0) {
        return;
    }

    if (actor->id == ACTOR_PLAYER) {
        Health_ChangeBy(play, -std::max(1, damagePerTick * 4));
        return;
    }

    if (actor->colChkInfo.health > 0) {
        actor->colChkInfo.damage = static_cast<uint8_t>(std::clamp(damagePerTick, 1, 255));
        Actor_ApplyDamage(actor);
    }
}

void RestoreStatusState(ExternalModRuntime::StatusEffectState& statusState, Actor* actor, PlayState* play, bool spawnEndEffect) {
    if (actor == nullptr) {
        return;
    }

    if (statusState.statusType == ExternalModStatusType::Freeze) {
        actor->world.pos.x = statusState.baseX;
        actor->world.pos.y = statusState.baseY;
        actor->world.pos.z = statusState.baseZ;
        actor->freezeTimer = 0;
        actor->colorFilterTimer = 0;
        actor->colorFilterParams = 0;
        if (spawnEndEffect && play != nullptr) {
            Vec3f burstPos = actor->world.pos;
            EffectSsIcePiece_SpawnBurst(play, &burstPos, std::max(actor->scale.x, 0.01f));
            Audio_PlayActorSound2(actor, NA_SE_EV_ICE_BROKEN);
        }
        return;
    }

    if (statusState.statusType == ExternalModStatusType::Stun) {
        actor->freezeTimer = 0;
        actor->colorFilterTimer = 0;
        actor->colorFilterParams = 0;
        return;
    }

    actor->colorFilterTimer = 0;
    actor->colorFilterParams = 0;
}

void ClearStatusEffects(ExternalModRuntime& runtime, PlayState* play, bool spawnEndEffects) {
    if (runtime.statusEffects.empty()) {
        return;
    }

    for (auto& statusState : runtime.statusEffects) {
        Actor* actor = FindActorByAddress(play, statusState.actorAddress, statusState.actorId);
        RestoreStatusState(statusState, actor, play, spawnEndEffects);
    }
    runtime.statusEffects.clear();
}

void ClearStatusEffectsOnActor(ExternalModRuntime& runtime, PlayState* play, Actor* actor, bool hasStatusFilter,
                               ExternalModStatusType statusFilter, bool spawnEndEffects) {
    if (actor == nullptr || runtime.statusEffects.empty()) {
        return;
    }

    const auto actorAddress = reinterpret_cast<uintptr_t>(actor);
    for (size_t i = 0; i < runtime.statusEffects.size();) {
        auto& statusState = runtime.statusEffects[i];
        const bool actorMatch = statusState.actorAddress == actorAddress;
        const bool statusMatch = !hasStatusFilter || statusState.statusType == statusFilter;
        if (!actorMatch || !statusMatch) {
            ++i;
            continue;
        }

        RestoreStatusState(statusState, actor, play, spawnEndEffects);
        runtime.statusEffects.erase(runtime.statusEffects.begin() + static_cast<std::ptrdiff_t>(i));
    }
}

void ClearStatusEffectsOnActorById(ExternalModRuntime& runtime, PlayState* play, Actor* actor,
                                   const std::string& statusId, bool spawnEndEffects) {
    if (actor == nullptr || runtime.statusEffects.empty() || statusId.empty()) {
        return;
    }

    const auto actorAddress = reinterpret_cast<uintptr_t>(actor);
    for (size_t i = 0; i < runtime.statusEffects.size();) {
        auto& statusState = runtime.statusEffects[i];
        if (statusState.actorAddress != actorAddress || statusState.statusId != statusId) {
            ++i;
            continue;
        }

        RestoreStatusState(statusState, actor, play, spawnEndEffects);
        runtime.statusEffects.erase(runtime.statusEffects.begin() + static_cast<std::ptrdiff_t>(i));
    }
}

void ClearStatusEffectsByType(ExternalModRuntime& runtime, PlayState* play, ExternalModStatusType statusType,
                              bool spawnEndEffects) {
    if (runtime.statusEffects.empty()) {
        return;
    }

    for (size_t i = 0; i < runtime.statusEffects.size();) {
        auto& statusState = runtime.statusEffects[i];
        if (statusState.statusType != statusType) {
            ++i;
            continue;
        }
        Actor* actor = FindActorByAddress(play, statusState.actorAddress, statusState.actorId);
        RestoreStatusState(statusState, actor, play, spawnEndEffects);
        runtime.statusEffects.erase(runtime.statusEffects.begin() + static_cast<std::ptrdiff_t>(i));
    }
}

void ClearStatusEffectsById(ExternalModRuntime& runtime, PlayState* play, const std::string& statusId,
                            bool spawnEndEffects) {
    if (statusId.empty() || runtime.statusEffects.empty()) {
        return;
    }

    for (size_t i = 0; i < runtime.statusEffects.size();) {
        auto& statusState = runtime.statusEffects[i];
        if (statusState.statusId != statusId) {
            ++i;
            continue;
        }
        Actor* actor = FindActorByAddress(play, statusState.actorAddress, statusState.actorId);
        RestoreStatusState(statusState, actor, play, spawnEndEffects);
        runtime.statusEffects.erase(runtime.statusEffects.begin() + static_cast<std::ptrdiff_t>(i));
    }
}

bool HasActiveStatusOnTarget(const ExternalModRuntime& runtime, uintptr_t actorAddress, const std::string& statusId) {
    if (actorAddress == 0 || statusId.empty()) {
        return false;
    }

    return std::any_of(runtime.statusEffects.begin(), runtime.statusEffects.end(),
                       [actorAddress, &statusId](const ExternalModRuntime::StatusEffectState& statusState) {
                           return statusState.actorAddress == actorAddress && statusState.statusId == statusId &&
                                  statusState.framesRemaining > 0;
                       });
}

int32_t GetStatusRemainingOnTarget(const ExternalModRuntime& runtime, uintptr_t actorAddress, const std::string& statusId) {
    if (actorAddress == 0 || statusId.empty()) {
        return 0;
    }

    int32_t bestRemaining = 0;
    for (const auto& statusState : runtime.statusEffects) {
        if (statusState.actorAddress != actorAddress || statusState.statusId != statusId ||
            statusState.framesRemaining <= 0) {
            continue;
        }
        bestRemaining = std::max(bestRemaining, statusState.framesRemaining);
    }
    return bestRemaining;
}

void TickStatusEffects(ExternalModRuntime& runtime, PlayState* play) {
    if (play == nullptr || runtime.statusEffects.empty()) {
        return;
    }

    for (size_t i = 0; i < runtime.statusEffects.size();) {
        auto& statusState = runtime.statusEffects[i];
        Actor* actor = FindActorByAddress(play, statusState.actorAddress, statusState.actorId);
        if (actor == nullptr || actor->update == nullptr) {
            runtime.statusEffects.erase(runtime.statusEffects.begin() + static_cast<std::ptrdiff_t>(i));
            continue;
        }

        statusState.framesRemaining = std::max(statusState.framesRemaining - 1, 0);
        if (statusState.framesRemaining <= 0) {
            RestoreStatusState(statusState, actor, play, true);
            runtime.statusEffects.erase(runtime.statusEffects.begin() + static_cast<std::ptrdiff_t>(i));
            continue;
        }

        switch (statusState.statusType) {
            case ExternalModStatusType::Freeze: {
                actor->freezeTimer = static_cast<uint16_t>(std::clamp(statusState.framesRemaining, 1, 0xFFFF));
                ApplyStatusColorFilter(actor, statusState.statusType, statusState.intensity);
                actor->world.pos.x = statusState.baseX;
                actor->world.pos.y = statusState.baseY;
                actor->world.pos.z = statusState.baseZ;
                if (statusState.shakeFrames > 0 && statusState.framesRemaining <= statusState.shakeFrames) {
                    const int32_t shakeFrameIndex = statusState.shakeFrames - statusState.framesRemaining;
                    const float shakeProgress =
                        1.0f - (static_cast<float>(statusState.framesRemaining) / static_cast<float>(std::max(statusState.shakeFrames, 1)));
                    const float amplitude = 0.6f + (2.2f * std::clamp(shakeProgress, 0.0f, 1.0f));
                    actor->world.pos.x = statusState.baseX + (sinf(static_cast<float>(shakeFrameIndex) * 1.9f) * amplitude);
                    actor->world.pos.z = statusState.baseZ + (cosf(static_cast<float>(shakeFrameIndex) * 2.5f) * amplitude);
                }
                break;
            }
            case ExternalModStatusType::Stun:
                actor->freezeTimer = static_cast<uint16_t>(std::clamp(std::min(statusState.framesRemaining, 6), 1, 6));
                ApplyStatusColorFilter(actor, statusState.statusType, statusState.intensity);
                break;
            case ExternalModStatusType::Fire:
            case ExternalModStatusType::Poison:
                ApplyStatusColorFilter(actor, statusState.statusType, statusState.intensity);
                statusState.tickCountdown = std::max(0, statusState.tickCountdown - 1);
                if (statusState.tickCountdown <= 0) {
                    statusState.tickCountdown = std::max(1, statusState.tickFrames);
                    if (statusState.statusType == ExternalModStatusType::Fire) {
                        Vec3f flamePos = actor->world.pos;
                        flamePos.y += 20.0f;
                        EffectSsEnFire_SpawnVec3f(play, actor, &flamePos, 70, 0, 0, -1);
                    }
                    ApplyStatusDamage(play, actor, statusState.damagePerTick);
                }
                break;
            case ExternalModStatusType::Blind:
                ApplyStatusColorFilter(actor, statusState.statusType, statusState.intensity);
                if (statusState.blindYawJitterDeg > 0.0f) {
                    const float jitterAmount = Rand_CenteredFloat(statusState.blindYawJitterDeg);
                    actor->shape.rot.y = statusState.baseRotY + static_cast<int16_t>(jitterAmount * (32768.0f / 180.0f));
                }
                if (statusState.blindSkipChance > 0.0f && Rand_ZeroOne() < statusState.blindSkipChance) {
                    actor->freezeTimer = 1;
                }
                break;
            default:
                ApplyStatusColorFilter(actor, statusState.statusType, statusState.intensity / 2);
                break;
        }

        ++i;
    }
}

void SpawnStatusMissEffect(PlayState* play, Player* player, ExternalModStatusType statusType) {
    if (play == nullptr || player == nullptr) {
        return;
    }

    Vec3f missPos = player->actor.world.pos;
    missPos.x += Math_SinS(player->actor.shape.rot.y) * 95.0f;
    missPos.z += Math_CosS(player->actor.shape.rot.y) * 95.0f;
    missPos.y += 24.0f;
    if (statusType == ExternalModStatusType::Fire) {
        EffectSsGFire_Spawn(play, &missPos);
    } else {
        Vec3f smokeVelocity = { 0.0f, 0.15f, 0.0f };
        Vec3f smokeAccel = { 0.0f, 0.02f, 0.0f };
        EffectSsIceSmoke_Spawn(play, &missPos, &smokeVelocity, &smokeAccel, 55);
    }
}

Actor* ResolveStatusActionTarget(const ExternalModPackage& package, const ExternalModAction& action, PlayState* play,
                                 Player* player, ExternalModStatusType statusType, bool& outCanMiss) {
    outCanMiss = false;
    if (play == nullptr || player == nullptr) {
        return nullptr;
    }

    switch (action.statusTarget) {
        case ExternalModStatusTarget::FrontTarget:
            outCanMiss = true;
            return FindStatusTargetInFront(play, player, std::max(action.freezeRange, action.range), statusType);
        case ExternalModStatusTarget::Self:
        case ExternalModStatusTarget::Player:
            return &player->actor;
        case ExternalModStatusTarget::ActorHandle: {
            outCanMiss = true;
            const auto actorIt = std::find_if(
                package.runtime.actorInstances.begin(), package.runtime.actorInstances.end(),
                [&action](const ExternalModActorInstance& instance) { return instance.active && instance.handle == action.actorHandle; });
            if (actorIt == package.runtime.actorInstances.end()) {
                return nullptr;
            }
            Vec3f origin = { actorIt->posX, actorIt->posY, actorIt->posZ };
            Actor* bestActor = nullptr;
            float bestDistance = std::numeric_limits<float>::max();
            for (size_t category = 0; category < ARRAY_COUNT(play->actorCtx.actorLists); ++category) {
                if (category == ACTORCAT_PLAYER || category == ACTORCAT_BG) {
                    continue;
                }
                for (Actor* actor = play->actorCtx.actorLists[category].head; actor != nullptr; actor = actor->next) {
                    if (actor == nullptr || actor->update == nullptr || ShouldProtectStatusTarget(actor, statusType)) {
                        continue;
                    }
                    const float distance = Math_Vec3f_DistXYZ(&origin, &actor->world.pos);
                    if (distance > std::max(action.freezeRange, action.range) || distance >= bestDistance) {
                        continue;
                    }
                    bestDistance = distance;
                    bestActor = actor;
                }
            }
            return bestActor;
        }
        default:
            return nullptr;
    }
}

void ApplyGlobalPlayerStatusModifiers(std::vector<ExternalModPackage>& packages, PlayState* play) {
    if (play == nullptr) {
        if (gExternalModPlayerStatusOverridesApplied) {
            GameInteractor::State::MovementSpeedMultiplier = 1.0f;
            GameInteractor::State::GravityLevel = GI_GRAVITY_LEVEL_NORMAL;
            GameInteractor::State::DefenseModifier = 0;
            gExternalModPlayerStatusOverridesApplied = false;
        }
        return;
    }
    auto* player = GET_PLAYER(play);
    if (player == nullptr) {
        return;
    }

    const auto playerAddress = reinterpret_cast<uintptr_t>(&player->actor);
    float movementMultiplier = 1.0f;
    float strengthMultiplier = 1.0f;
    float weaknessMultiplier = 1.0f;
    bool highJumpActive = false;
    bool blindActive = false;
    int32_t blindIntensity = 0;
    float blindJitterDeg = 0.0f;
    int32_t freezeFramesRemaining = 0;
    int32_t freezeIntensity = 0;
    bool hasAnyModifier = false;

    for (const auto& package : packages) {
        if (!package.runtime.enabled) {
            continue;
        }
        for (const auto& statusState : package.runtime.statusEffects) {
            if (!statusState.isPlayerTarget || statusState.actorAddress != playerAddress || statusState.framesRemaining <= 0) {
                continue;
            }
            hasAnyModifier = true;
            switch (statusState.statusType) {
                case ExternalModStatusType::Speed:
                case ExternalModStatusType::Slow:
                    movementMultiplier *= std::clamp(statusState.speedMultiplier, 0.05f, 8.0f);
                    break;
                case ExternalModStatusType::HighJump:
                    highJumpActive = true;
                    break;
                case ExternalModStatusType::Strength:
                    strengthMultiplier *= std::clamp(statusState.strengthMultiplier, 0.1f, 12.0f);
                    break;
                case ExternalModStatusType::Weakness:
                    weaknessMultiplier *= std::clamp(statusState.weaknessMultiplier, 0.1f, 12.0f);
                    break;
                case ExternalModStatusType::Blind:
                    blindActive = true;
                    blindIntensity = std::max(blindIntensity, statusState.intensity);
                    blindJitterDeg = std::max(blindJitterDeg, statusState.blindYawJitterDeg);
                    break;
                case ExternalModStatusType::Freeze:
                case ExternalModStatusType::Stun:
                    freezeFramesRemaining = std::max(freezeFramesRemaining, statusState.framesRemaining);
                    freezeIntensity = std::max(freezeIntensity, statusState.intensity);
                    break;
                default:
                    break;
            }
        }
    }

    if (!hasAnyModifier) {
        if (gExternalModPlayerStatusOverridesApplied) {
            GameInteractor::State::MovementSpeedMultiplier = 1.0f;
            GameInteractor::State::GravityLevel = GI_GRAVITY_LEVEL_NORMAL;
            GameInteractor::State::DefenseModifier = 0;
            player->ivanDamageMultiplier = 1;
            gExternalModPlayerStatusOverridesApplied = false;
        }
        return;
    }

    gExternalModPlayerStatusOverridesApplied = true;
    GameInteractor::State::MovementSpeedMultiplier = std::clamp(movementMultiplier, 0.05f, 8.0f);
    GameInteractor::State::GravityLevel = highJumpActive ? GI_GRAVITY_LEVEL_LIGHT : GI_GRAVITY_LEVEL_NORMAL;
    player->ivanDamageMultiplier = static_cast<uint8_t>(std::clamp(static_cast<int32_t>(std::lround(strengthMultiplier)), 1, 20));
    if (weaknessMultiplier > 1.0f) {
        GameInteractor::State::DefenseModifier =
            -std::clamp(static_cast<int32_t>(std::lround(weaknessMultiplier)), 1, 20);
    } else {
        GameInteractor::State::DefenseModifier = 0;
    }

    if (blindActive) {
        ApplyStatusColorFilter(&player->actor, ExternalModStatusType::Blind, std::clamp(blindIntensity, 0, 255));
        if (blindJitterDeg > 0.0f) {
            const float jitterAmount = Rand_CenteredFloat(blindJitterDeg);
            player->actor.shape.rot.y += static_cast<int16_t>(jitterAmount * (32768.0f / 180.0f));
        }
    }

    if (freezeFramesRemaining > 0) {
        player->actor.freezeTimer = static_cast<uint16_t>(std::clamp(freezeFramesRemaining, 1, 0xFFFF));
        ApplyStatusColorFilter(&player->actor, ExternalModStatusType::Freeze, std::clamp(freezeIntensity, 0, 255));
    }
}

void ClearSurfState(ExternalModRuntime& runtime) {
    runtime.surfState = ExternalModRuntime::SurfState{};
}

void ActivateSurfState(ExternalModPackage& package, const ExternalModMovementProfile& profile,
                       const ExternalModItemDefinition* sourceItem, Player* player, int32_t durationOverrideFrames) {
    auto& surfState = package.runtime.surfState;
    surfState.active = true;
    surfState.sourceModId = package.manifest.id;
    surfState.movementProfileId = profile.id;
    surfState.sourceItemId = sourceItem == nullptr ? std::string{} : sourceItem->id;
    surfState.framesRemaining = durationOverrideFrames > 0 ? durationOverrideFrames : profile.durationFrames;
    surfState.speed = player == nullptr ? 0.0f : std::max(player->actor.speedXZ, 0.0f);
    surfState.headingYaw = player == nullptr ? 0 : player->actor.shape.rot.y;
    surfState.boardScale = profile.boardScale;
    surfState.boostCooldownRemaining = 0;
    surfState.idle = true;

    SPDLOG_INFO(
        "[ExternalMods] Surf activated for {} profile={} forwardAccel={:.3f} boostMask={} boostAccel={:.3f} "
        "boostMaxSpeed={:.3f} boostCooldown={}",
        package.manifest.id, profile.id, profile.surfForwardAccel, profile.surfBoostButtonMask, profile.surfBoostAccel,
        profile.surfBoostMaxSpeed, profile.surfBoostCooldownFrames);
}

void TickSurfState(ExternalModPackage& package, PlayState* play, Player* player) {
    if (play == nullptr || player == nullptr) {
        return;
    }

    auto& surfState = package.runtime.surfState;
    if (!surfState.active) {
        return;
    }

    const auto* profile = ExternalModContentRegistry::FindMovementProfileById(package.runtime, surfState.movementProfileId);
    if (profile == nullptr || profile->mode != ExternalModMovementMode::Surf) {
        ClearSurfState(package.runtime);
        return;
    }

    if (surfState.framesRemaining > 0) {
        surfState.framesRemaining--;
        if (surfState.framesRemaining <= 0) {
            ClearSurfState(package.runtime);
            return;
        }
    }

    Input* input = &play->state.input[0];
    const float stickX = static_cast<float>(input->rel.stick_x);
    const float stickY = static_cast<float>(input->rel.stick_y);
    const float stickMagnitude = std::sqrt((stickX * stickX) + (stickY * stickY));
    const float forwardInput = std::clamp(stickY / 60.0f, -1.0f, 1.0f);
    const bool boostHeld =
        profile->surfBoostButtonMask != 0 && MatchButtonMask(static_cast<int32_t>(input->cur.button), profile->surfBoostButtonMask);
    if (surfState.boostCooldownRemaining > 0) {
        surfState.boostCooldownRemaining--;
    }
    if (stickMagnitude > 4.0f) {
        const float turnStepDeg = std::clamp(stickX / 60.0f, -1.0f, 1.0f) * profile->surfTurnRateDeg;
        const float yawUnitsPerDeg = 32768.0f / 180.0f;
        surfState.headingYaw += static_cast<int16_t>(turnStepDeg * yawUnitsPerDeg);
    }

    player->actor.shape.rot.y = surfState.headingYaw;
    player->actor.world.rot.y = surfState.headingYaw;

    float nx = 0.0f;
    float ny = 1.0f;
    float nz = 0.0f;
    const bool onGround = (player->actor.bgCheckFlags & 1) != 0 && player->actor.floorPoly != nullptr;
    if (onGround) {
        nx = COLPOLY_GET_NORMAL(player->actor.floorPoly->normal.x);
        ny = COLPOLY_GET_NORMAL(player->actor.floorPoly->normal.y);
        nz = COLPOLY_GET_NORMAL(player->actor.floorPoly->normal.z);
    }

    const float slopeHorizontal = std::sqrt((nx * nx) + (nz * nz));
    float downhillX = 0.0f;
    float downhillZ = 0.0f;
    if (slopeHorizontal > 0.0001f) {
        downhillX = nx / slopeHorizontal;
        downhillZ = nz / slopeHorizontal;
    }

    const float forwardX = Math_SinS(surfState.headingYaw);
    const float forwardZ = Math_CosS(surfState.headingYaw);
    const float downhillDot = (forwardX * downhillX) + (forwardZ * downhillZ);

    if (onGround) {
        const float downhillGain = std::max(0.0f, downhillDot) * slopeHorizontal * profile->surfDownhillAccel;
        const float uphillLoss = std::max(0.0f, -downhillDot) * slopeHorizontal * profile->surfUphillBrake;
        surfState.speed += downhillGain;
        surfState.speed = std::max(0.0f, surfState.speed - uphillLoss);
        surfState.speed *= (1.0f - profile->surfFlatDrag);
    } else {
        surfState.speed *= (1.0f - profile->surfFlatDrag * 0.35f);
    }

    if (forwardInput > 0.0f) {
        surfState.speed += forwardInput * profile->surfForwardAccel;
    }
    if (boostHeld && surfState.boostCooldownRemaining <= 0) {
        surfState.speed += profile->surfBoostAccel;
        surfState.boostCooldownRemaining = std::max(1, profile->surfBoostCooldownFrames);
    }

    const float effectiveMaxSpeed = std::max(profile->surfMaxSpeed, profile->surfBoostMaxSpeed);
    surfState.speed = std::clamp(surfState.speed, 0.0f, effectiveMaxSpeed);
    surfState.idle = surfState.speed <= profile->idleSpeedThreshold && !boostHeld && forwardInput <= 0.0f;

    if (surfState.idle && profile->idleLock) {
        surfState.speed = 0.0f;
        player->actor.speedXZ = 0.0f;
        player->linearVelocity = 0.0f;
        player->actor.velocity.x = 0.0f;
        player->actor.velocity.z = 0.0f;
        if (profile->idlePose == "stand") {
            Player_StartMode_Idle(play, player);
        }
    } else {
        player->actor.speedXZ = surfState.speed;
        player->linearVelocity = surfState.speed;
        player->actor.velocity.x = forwardX * surfState.speed;
        player->actor.velocity.z = forwardZ * surfState.speed;
        player->actor.world.pos.x += player->actor.velocity.x;
        player->actor.world.pos.z += player->actor.velocity.z;
    }

    if (onGround) {
        player->actor.world.pos.y = player->actor.floorHeight + profile->riderHeightOffset;
        player->actor.velocity.y = 0.0f;
    }

    const float rightX = Math_CosS(surfState.headingYaw);
    const float rightZ = -Math_SinS(surfState.headingYaw);
    surfState.boardPosX =
        player->actor.world.pos.x + (forwardX * profile->boardForwardOffset) + (rightX * profile->boardRightOffset);
    surfState.boardPosY =
        (onGround ? player->actor.floorHeight : player->actor.world.pos.y) + profile->boardHeightOffset + profile->boardUpOffset;
    surfState.boardPosZ =
        player->actor.world.pos.z + (forwardZ * profile->boardForwardOffset) + (rightZ * profile->boardRightOffset);
    surfState.boardRotY = surfState.headingYaw;
    surfState.boardRotX = 0;
    surfState.boardRotZ = 0;
    surfState.boardScale = profile->boardScale;

    if (profile->boardPitchRollFromGround && onGround) {
        const float safeNy = std::max(0.001f, ny);
        constexpr float kRadToBinang = 32768.0f / 3.14159265358979323846f;
        const float pitch = std::atan2(-nz, safeNy);
        const float roll = std::atan2(nx, safeNy);
        surfState.boardRotX = static_cast<int16_t>(pitch * kRadToBinang);
        surfState.boardRotZ = static_cast<int16_t>(roll * kRadToBinang);
    }
}

bool CompareIntegerByOperator(int32_t lhs, const std::string& op, int32_t rhs) {
    const auto normalizedOp = op.empty() ? "==" : op;
    if (normalizedOp == "==" || normalizedOp == "=") {
        return lhs == rhs;
    }
    if (normalizedOp == "!=") {
        return lhs != rhs;
    }
    if (normalizedOp == "<") {
        return lhs < rhs;
    }
    if (normalizedOp == "<=") {
        return lhs <= rhs;
    }
    if (normalizedOp == ">") {
        return lhs > rhs;
    }
    if (normalizedOp == ">=") {
        return lhs >= rhs;
    }
    return false;
}

ExternalModStatusType ResolveStatusTypeFromId(const ExternalModPackage& package, const std::string& statusId) {
    ExternalModStatusType parsedType = ExternalModStatusType::Custom;
    if (ParseStatusIdToken(statusId, parsedType) && parsedType != ExternalModStatusType::Custom) {
        return parsedType;
    }

    const auto* definition = ExternalModContentRegistry::FindStatusDefinitionById(package.runtime, statusId);
    if (definition != nullptr) {
        if (definition->baseStatusType != ExternalModStatusType::Custom) {
            return definition->baseStatusType;
        }
        if (!definition->baseStatusId.empty()) {
            ExternalModStatusType baseType = ExternalModStatusType::Custom;
            if (ParseStatusIdToken(definition->baseStatusId, baseType)) {
                return baseType;
            }
        }
    }

    return ExternalModStatusType::Custom;
}

void ApplyDamageAmountToActor(PlayState* play, Actor* target, int32_t damageAmount) {
    if (play == nullptr || target == nullptr || damageAmount <= 0) {
        return;
    }

    if (target->id == ACTOR_PLAYER) {
        Health_ChangeBy(play, -std::max(1, damageAmount * 4));
        return;
    }

    if (target->colChkInfo.health > 0) {
        target->colChkInfo.damage = static_cast<uint8_t>(std::clamp(damageAmount, 1, 255));
        Actor_ApplyDamage(target);
    }
}

void ApplyDamageProfilePropInteraction(PlayState* play, Actor* target, const ExternalModDamageProfile& profile) {
    if (play == nullptr || target == nullptr) {
        return;
    }

    const std::string interaction = ToLower(profile.propInteraction);
    if (interaction.empty() || interaction == "none") {
        return;
    }

    if (target->id == ACTOR_EN_KUSA) {
        auto* kusa = reinterpret_cast<EnKusa*>(target);
        kusa->collider.base.acFlags |= AC_HIT;
        return;
    }

    if (target->id == ACTOR_EN_WOOD02) {
        if (interaction == "hard_kill") {
            Actor_Kill(target);
            return;
        }

        auto* wood = reinterpret_cast<EnWood02*>(target);
        wood->collider.base.acFlags |= AC_HIT;
        if (wood->unk_14C >= -1) {
            wood->unk_14C = -0x15;
        }
        wood->actor.home.rot.y = target->yawTowardsPlayer;
        if (wood->actor.home.rot.y == 0) {
            wood->actor.home.rot.y = 1;
        }
    }
}

void ApplyDamageProfileToActor(PlayState* play, Actor* target, const ExternalModDamageProfile& profile) {
    ApplyDamageAmountToActor(play, target, std::max(profile.amount, 0));

    const auto normalizedType = ToLower(profile.type);
    if (normalizedType == "fire" && play != nullptr && target != nullptr) {
        Vec3f flamePos = target->world.pos;
        flamePos.y += 20.0f;
        EffectSsEnFire_SpawnVec3f(play, target, &flamePos, 70, 0, 0, -1);
        Actor_SetColorFilter(target, 0x4000, 0xFF, 0, 4);
    } else if ((normalizedType == "ice" || normalizedType == "freeze") && target != nullptr) {
        Actor_SetColorFilter(target, 0x0000, 0xFF, 0, 4);
    }

    ApplyDamageProfilePropInteraction(play, target, profile);
}

bool ResolveForwardRaycastHits(PlayState* play, Player* player, float range, bool stopOnWall, bool includeProps,
                               bool includeEnemies, std::vector<Actor*>& outHits) {
    outHits.clear();
    if (play == nullptr || player == nullptr) {
        return false;
    }

    const float clampedRange = std::max(range, 1.0f);
    Vec3f rayStart = player->actor.world.pos;
    rayStart.y += 30.0f;

    Vec3f forward = { Math_SinS(player->actor.shape.rot.y), 0.0f, Math_CosS(player->actor.shape.rot.y) };
    const float forwardLenSq = (forward.x * forward.x) + (forward.y * forward.y) + (forward.z * forward.z);
    if (forwardLenSq <= 0.0001f) {
        return true;
    }
    const float invForwardLen = 1.0f / std::sqrt(forwardLenSq);
    forward.x *= invForwardLen;
    forward.y *= invForwardLen;
    forward.z *= invForwardLen;

    Vec3f rayEnd = { rayStart.x + forward.x * clampedRange, rayStart.y + forward.y * clampedRange,
                     rayStart.z + forward.z * clampedRange };

    float effectiveRange = clampedRange;
    if (stopOnWall) {
        Vec3f wallHitPos = rayEnd;
        CollisionPoly* wallPoly = nullptr;
        if (BgCheck_AnyLineTest1(&play->colCtx, &rayStart, &rayEnd, &wallHitPos, &wallPoly, true)) {
            effectiveRange = std::max(1.0f, Math_Vec3f_DistXYZ(&rayStart, &wallHitPos));
        }
    }

    struct RayHitCandidate {
        Actor* actor = nullptr;
        float projection = 0.0f;
        float lateralDistance = 0.0f;
    };
    std::vector<RayHitCandidate> candidates;

    for (size_t category = 0; category < ARRAY_COUNT(play->actorCtx.actorLists); ++category) {
        if (category == ACTORCAT_PLAYER || category == ACTORCAT_BG) {
            continue;
        }
        if (!includeProps && category == ACTORCAT_PROP) {
            continue;
        }
        if (!includeEnemies && (category == ACTORCAT_ENEMY || category == ACTORCAT_BOSS)) {
            continue;
        }

        for (Actor* actor = play->actorCtx.actorLists[category].head; actor != nullptr; actor = actor->next) {
            if (actor == nullptr || actor == &player->actor || actor->update == nullptr) {
                continue;
            }

            Vec3f delta = { actor->world.pos.x - rayStart.x, actor->world.pos.y - rayStart.y,
                            actor->world.pos.z - rayStart.z };
            const float projection = (delta.x * forward.x) + (delta.y * forward.y) + (delta.z * forward.z);
            if (projection < 0.0f || projection > effectiveRange) {
                continue;
            }

            Vec3f closestPoint = { rayStart.x + forward.x * projection, rayStart.y + forward.y * projection,
                                   rayStart.z + forward.z * projection };
            const float lateralDistance = Math_Vec3f_DistXYZ(&actor->world.pos, &closestPoint);
            const float actorRadius = std::max(20.0f, std::max(actor->scale.x, actor->scale.z) * 60.0f);
            if (lateralDistance > actorRadius) {
                continue;
            }

            if (stopOnWall) {
                Vec3f hitPos = actor->world.pos;
                CollisionPoly* hitPoly = nullptr;
                if (BgCheck_AnyLineTest1(&play->colCtx, &rayStart, &actor->world.pos, &hitPos, &hitPoly, true)) {
                    const float hitDistance = Math_Vec3f_DistXYZ(&rayStart, &hitPos);
                    if (hitDistance + 1.0f < projection) {
                        continue;
                    }
                }
            }

            candidates.push_back({ actor, projection, lateralDistance });
        }
    }

    std::sort(candidates.begin(), candidates.end(), [](const RayHitCandidate& lhs, const RayHitCandidate& rhs) {
        if (lhs.projection != rhs.projection) {
            return lhs.projection < rhs.projection;
        }
        return lhs.lateralDistance < rhs.lateralDistance;
    });

    outHits.reserve(candidates.size());
    for (const auto& candidate : candidates) {
        outHits.push_back(candidate.actor);
    }
    return true;
}

bool ResolveTargetsForTargetingProfile(const ExternalModTargetingProfile& profile, PlayState* play, Player* player,
                                       std::vector<Actor*>& outTargets) {
    outTargets.clear();
    if (play == nullptr || player == nullptr) {
        return false;
    }

    const auto pushUniqueTarget = [&outTargets](Actor* actor) {
        if (actor == nullptr) {
            return;
        }
        if (std::find(outTargets.begin(), outTargets.end(), actor) == outTargets.end()) {
            outTargets.push_back(actor);
        }
    };

    switch (profile.mode) {
        case ExternalModTargetingMode::Self:
        case ExternalModTargetingMode::Player:
            pushUniqueTarget(&player->actor);
            return true;
        case ExternalModTargetingMode::LockedOnTarget:
        case ExternalModTargetingMode::FrontTarget:
        case ExternalModTargetingMode::Raycast: {
            if (profile.mode == ExternalModTargetingMode::Raycast) {
                std::vector<Actor*> rayHits;
                ResolveForwardRaycastHits(play, player, profile.range, profile.stopOnWall, profile.includeProps,
                                          profile.includeEnemies, rayHits);
                if (!rayHits.empty()) {
                    pushUniqueTarget(rayHits.front());
                }
                return true;
            }
            Actor* target = FindStatusTargetInFront(play, player, std::max(profile.range, 1.0f), ExternalModStatusType::Custom);
            if (target != nullptr) {
                pushUniqueTarget(target);
            }
            return true;
        }
        case ExternalModTargetingMode::Sphere:
        case ExternalModTargetingMode::Cone: {
            const float maxRange = std::max(profile.range, 1.0f);
            const float maxRadius = std::max(profile.radius, 1.0f);
            const float coneHalfAngleDeg = std::clamp(profile.angle * 0.5f, 0.0f, 180.0f);
            constexpr float kPi = 3.14159265358979323846f;
            const float coneHalfAngleRad = coneHalfAngleDeg * (kPi / 180.0f);

            for (size_t category = 0; category < ARRAY_COUNT(play->actorCtx.actorLists); ++category) {
                if (category == ACTORCAT_PLAYER || category == ACTORCAT_BG) {
                    continue;
                }
                if (!profile.includeProps && category == ACTORCAT_PROP) {
                    continue;
                }
                if (!profile.includeEnemies && (category == ACTORCAT_ENEMY || category == ACTORCAT_BOSS)) {
                    continue;
                }

                for (Actor* actor = play->actorCtx.actorLists[category].head; actor != nullptr; actor = actor->next) {
                    if (actor == nullptr || actor == &player->actor || actor->update == nullptr) {
                        continue;
                    }

                    const float distance = Math_Vec3f_DistXYZ(&player->actor.world.pos, &actor->world.pos);
                    if (distance > maxRange) {
                        continue;
                    }

                    if (profile.mode == ExternalModTargetingMode::Sphere) {
                        if (distance <= maxRadius) {
                            pushUniqueTarget(actor);
                        }
                        continue;
                    }

                    Vec3f delta = { actor->world.pos.x - player->actor.world.pos.x,
                                    actor->world.pos.y - player->actor.world.pos.y,
                                    actor->world.pos.z - player->actor.world.pos.z };
                    const float deltaLen =
                        std::sqrt((delta.x * delta.x) + (delta.y * delta.y) + (delta.z * delta.z));
                    if (deltaLen <= 0.001f) {
                        continue;
                    }
                    const float invLen = 1.0f / deltaLen;
                    delta.x *= invLen;
                    delta.y *= invLen;
                    delta.z *= invLen;

                    Vec3f facing = { Math_SinS(player->actor.shape.rot.y), 0.0f, Math_CosS(player->actor.shape.rot.y) };
                    const float dot = std::clamp((delta.x * facing.x) + (delta.y * facing.y) + (delta.z * facing.z),
                                                 -1.0f, 1.0f);
                    const float angle = std::acos(dot);
                    if (angle <= coneHalfAngleRad) {
                        pushUniqueTarget(actor);
                    }
                }
            }
            return true;
        }
        default:
            return false;
    }
}

bool ResolveStatusActionData(ExternalModPackage& package, const ExternalModAction& action,
                             ExternalModAction& outResolvedAction, ExternalModStatusType& outResolvedType) {
    outResolvedAction = action;
    if (outResolvedAction.statusId.empty()) {
        outResolvedAction.statusId = "core:" + StatusTypeToString(action.statusType);
    }

    outResolvedType = ResolveStatusTypeFromId(package, outResolvedAction.statusId);
    const auto* statusDefinition = ExternalModContentRegistry::FindStatusDefinitionById(package.runtime, outResolvedAction.statusId);
    if (statusDefinition != nullptr) {
        if (outResolvedAction.durationFrames <= 0) {
            outResolvedAction.durationFrames = statusDefinition->durationFrames;
        }
        if (outResolvedAction.tickFrames <= 0) {
            outResolvedAction.tickFrames = statusDefinition->tickFrames;
        }
        if (outResolvedAction.damagePerTick <= 0) {
            outResolvedAction.damagePerTick = statusDefinition->damagePerTick;
        }
        if (outResolvedAction.shakeFrames <= 0) {
            outResolvedAction.shakeFrames = statusDefinition->shakeFrames;
        }
        if (outResolvedAction.intensity <= 0) {
            outResolvedAction.intensity = statusDefinition->intensity;
        }
        if (outResolvedType == ExternalModStatusType::Custom &&
            statusDefinition->baseStatusType != ExternalModStatusType::Custom) {
            outResolvedType = statusDefinition->baseStatusType;
        }
    }

    return true;
}

bool ShouldIncludeAoEActorCategory(ExternalModAoETargetScope scope, size_t category) {
    if (category == ACTORCAT_PLAYER || category == ACTORCAT_BG) {
        return false;
    }

    switch (scope) {
        case ExternalModAoETargetScope::AllNonPlayer:
            return true;
        case ExternalModAoETargetScope::EnemiesBosses:
            return category == ACTORCAT_ENEMY || category == ACTORCAT_BOSS;
        case ExternalModAoETargetScope::EnemiesBossesProps:
            return category == ACTORCAT_ENEMY || category == ACTORCAT_BOSS || category == ACTORCAT_PROP;
        default:
            return true;
    }
}

bool ExecuteUseProfileEffects(ExternalModPackage& package, const std::vector<ExternalModUseProfileEffect>& effects,
                              const std::vector<Actor*>& targets, PlayState* play, Player* player,
                              const ExternalModItemDefinition* sourceItem, std::string& outError);

bool ExecuteUseProfileById(ExternalModPackage& package, const std::string& profileId, PlayState* play, Player* player,
                           const ExternalModItemDefinition* sourceItem, std::string& outError) {
    const auto* profile = ExternalModContentRegistry::FindItemUseProfileById(package.runtime, profileId);
    if (profile == nullptr) {
        outError = "Unknown use profile id: " + profileId;
        return false;
    }

    const auto* targetingProfile = ExternalModContentRegistry::FindTargetingProfileById(package.runtime, profile->targetingProfileId);
    if (targetingProfile == nullptr) {
        outError = "Use profile has unknown targeting profile: " + profile->targetingProfileId;
        return false;
    }

    std::vector<Actor*> targets;
    ResolveTargetsForTargetingProfile(*targetingProfile, play, player, targets);
    package.runtime.useProfileSpawnedShockwave = false;
    if (!ExecuteUseProfileEffects(package, profile->effects, targets, play, player, sourceItem, outError)) {
        return false;
    }
    return true;
}

bool ExecuteUseProfileEffects(ExternalModPackage& package, const std::vector<ExternalModUseProfileEffect>& effects,
                              const std::vector<Actor*>& targets, PlayState* play, Player* player,
                              const ExternalModItemDefinition* sourceItem, std::string& outError) {
    for (const auto& effect : effects) {
        const auto effectAction = ToLower(effect.action);
        if (effectAction == "dealdamage") {
            const auto* profile = ExternalModContentRegistry::FindDamageProfileById(package.runtime, effect.damageProfileId);
            if (profile == nullptr) {
                outError = "Unknown damage profile: " + effect.damageProfileId;
                return false;
            }
            for (Actor* target : targets) {
                if (target == nullptr) {
                    continue;
                }
                ApplyDamageProfileToActor(play, target, *profile);
            }
            continue;
        }

        if (effectAction == "applystatus") {
            ExternalModAction statusAction;
            statusAction.statusId = effect.statusId;
            statusAction.durationFrames = effect.durationFrames > 0 ? effect.durationFrames : 90;
            statusAction.tickFrames = effect.tickFrames > 0 ? effect.tickFrames : 15;
            statusAction.damagePerTick = std::max(effect.damagePerTick, 0);
            statusAction.intensity = std::clamp(effect.intensity, 0, 255);

            ExternalModStatusType statusType = ExternalModStatusType::Custom;
            ResolveStatusActionData(package, statusAction, statusAction, statusType);
            for (Actor* target : targets) {
                if (target == nullptr || ShouldProtectStatusTarget(target, statusType)) {
                    continue;
                }
                const bool isPlayerTarget = target->id == ACTOR_PLAYER;
                BeginStatusOnActor(package.runtime, play, target, statusAction, statusType, isPlayerTarget,
                                   statusAction.statusId, package.manifest.id);
            }
            continue;
        }

        if (effectAction == "spawnprojectile") {
            const auto* projectileProfile = ExternalModContentRegistry::FindProjectileProfileById(package.runtime, effect.projectileProfileId);
            if (projectileProfile == nullptr) {
                outError = "Unknown projectile profile: " + effect.projectileProfileId;
                return false;
            }
            const auto* damageProfile = ExternalModContentRegistry::FindDamageProfileById(package.runtime, projectileProfile->damageProfileId);

            std::vector<std::pair<float, Actor*>> sortedTargets;
            for (Actor* target : targets) {
                if (target == nullptr) {
                    continue;
                }
                const float distance = Math_Vec3f_DistXYZ(&player->actor.world.pos, &target->world.pos);
                sortedTargets.emplace_back(distance, target);
            }
            std::sort(sortedTargets.begin(), sortedTargets.end(),
                      [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });

            int32_t hitsRemaining = std::max(1, projectileProfile->maxHits);
            for (const auto& hit : sortedTargets) {
                if (hitsRemaining <= 0) {
                    break;
                }
                Actor* target = hit.second;
                if (damageProfile != nullptr) {
                    ApplyDamageProfileToActor(play, target, *damageProfile);
                }
                for (const auto& statusId : projectileProfile->applyStatuses) {
                    ExternalModAction statusAction;
                    statusAction.statusId = statusId;
                    statusAction.durationFrames = effect.durationFrames > 0 ? effect.durationFrames : 90;
                    statusAction.tickFrames = effect.tickFrames > 0 ? effect.tickFrames : 15;
                    statusAction.damagePerTick = std::max(effect.damagePerTick, 0);
                    statusAction.intensity = std::clamp(effect.intensity, 0, 255);
                    ExternalModStatusType statusType = ExternalModStatusType::Custom;
                    ResolveStatusActionData(package, statusAction, statusAction, statusType);
                    if (!ShouldProtectStatusTarget(target, statusType)) {
                        const bool isPlayerTarget = target->id == ACTOR_PLAYER;
                        BeginStatusOnActor(package.runtime, play, target, statusAction, statusType, isPlayerTarget,
                                           statusAction.statusId, package.manifest.id);
                    }
                }
                --hitsRemaining;
            }
            continue;
        }

        if (effectAction == "spawnshockwave") {
            if (play == nullptr || player == nullptr) {
                outError = "spawnShockwave requires active play/player";
                return false;
            }

            Vec3f origin = player->actor.world.pos;
            if (effect.shockwaveOrigin == "impact" && package.runtime.hasEffectImpactPosition) {
                origin.x = package.runtime.effectImpactPosX;
                origin.y = package.runtime.effectImpactPosY;
                origin.z = package.runtime.effectImpactPosZ;
            }

            Vec3f shockwaveVelocity = { 0.0f, 0.0f, 0.0f };
            Vec3f shockwaveAccel = { 0.0f, 0.0f, 0.0f };
            Color_RGBA8 primColor = { effect.shockwavePrimColor[0], effect.shockwavePrimColor[1],
                                      effect.shockwavePrimColor[2], effect.shockwavePrimColor[3] };
            Color_RGBA8 envColor = { effect.shockwaveEnvColor[0], effect.shockwaveEnvColor[1], effect.shockwaveEnvColor[2],
                                     effect.shockwaveEnvColor[3] };

            EffectSsBlast_SpawnShockwave(play, &origin, &shockwaveVelocity, &shockwaveAccel,
                                         &primColor, &envColor, static_cast<s16>(std::clamp(effect.shockwaveLife, 1, 120)));

            if (effect.shockwaveSpawnIceSmoke) {
                constexpr float kTau = 6.28318530717958647692f;
                constexpr int32_t kSmokeCount = 8;
                constexpr float kSmokeRingRadius = 35.0f;
                for (int32_t smokeIndex = 0; smokeIndex < kSmokeCount; ++smokeIndex) {
                    const float ratio = static_cast<float>(smokeIndex) / static_cast<float>(kSmokeCount);
                    const float angle = ratio * kTau;
                    Vec3f smokePos = { origin.x + std::cos(angle) * kSmokeRingRadius, origin.y + 5.0f,
                                       origin.z + std::sin(angle) * kSmokeRingRadius };
                    Vec3f smokeVelocity = { std::cos(angle) * 0.7f, 1.1f, std::sin(angle) * 0.7f };
                    Vec3f smokeAccel = { 0.0f, 0.06f, 0.0f };
                    EffectSsIceSmoke_Spawn(play, &smokePos, &smokeVelocity, &smokeAccel, 120);
                }
            }

            package.runtime.useProfileSpawnedShockwave = true;
            continue;
        }

        if (effectAction == "spawnaoe") {
            const auto* aoeProfile = ExternalModContentRegistry::FindAoEProfileById(package.runtime, effect.aoeProfileId);
            if (aoeProfile == nullptr) {
                outError = "Unknown aoe profile: " + effect.aoeProfileId;
                return false;
            }

            std::vector<Actor*> aoeTargets;
            const float radius = std::max(aoeProfile->radius, aoeProfile->range);
            for (size_t category = 0; category < ARRAY_COUNT(play->actorCtx.actorLists); ++category) {
                if (!ShouldIncludeAoEActorCategory(aoeProfile->targetScope, category)) {
                    continue;
                }
                for (Actor* actor = play->actorCtx.actorLists[category].head; actor != nullptr; actor = actor->next) {
                    if (actor == nullptr || actor->update == nullptr) {
                        continue;
                    }
                    const float distance = Math_Vec3f_DistXYZ(&player->actor.world.pos, &actor->world.pos);
                    if (distance <= radius) {
                        aoeTargets.push_back(actor);
                    }
                }
            }

            if (!ExecuteUseProfileEffects(package, aoeProfile->onEnter, aoeTargets, play, player, sourceItem, outError)) {
                return false;
            }
            if (aoeProfile->durationFrames > 1 && !aoeProfile->onTick.empty()) {
                if (!ExecuteUseProfileEffects(package, aoeProfile->onTick, aoeTargets, play, player, sourceItem, outError)) {
                    return false;
                }
            }
            if (!aoeProfile->onExit.empty()) {
                if (!ExecuteUseProfileEffects(package, aoeProfile->onExit, aoeTargets, play, player, sourceItem, outError)) {
                    return false;
                }
            }
            continue;
        }

        if (effectAction == "applymovementprofile") {
            const auto* movementProfile = ExternalModContentRegistry::FindMovementProfileById(package.runtime, effect.movementProfileId);
            if (movementProfile == nullptr) {
                outError = "Unknown movement profile: " + effect.movementProfileId;
                return false;
            }

            if (movementProfile->mode == ExternalModMovementMode::Surf) {
                auto& surfState = package.runtime.surfState;
                if (surfState.active && surfState.movementProfileId == movementProfile->id) {
                    ClearSurfState(package.runtime);
                } else {
                    ActivateSurfState(package, *movementProfile, sourceItem, player, effect.durationFrames);
                }
                continue;
            }

            ExternalModAction movementAction;
            movementAction.durationFrames =
                effect.durationFrames > 0 ? effect.durationFrames : std::max(1, movementProfile->durationFrames);
            movementAction.tickFrames = std::max(1, movementAction.durationFrames);
            movementAction.intensity = 128;
            movementAction.speedMultiplier = movementProfile->speedMultiplier;
            movementAction.jumpMultiplier =
                movementProfile->gravityScale < 1.0f ? (1.0f / movementProfile->gravityScale) : 1.0f;

            if (movementProfile->speedMultiplier > 1.0f) {
                movementAction.statusId = "core:speed";
                movementAction.damagePerTick = 0;
                BeginStatusOnActor(package.runtime, play, &player->actor, movementAction, ExternalModStatusType::Speed, true,
                                   movementAction.statusId, package.manifest.id);
            } else if (movementProfile->speedMultiplier < 1.0f) {
                movementAction.statusId = "core:slow";
                movementAction.damagePerTick = 0;
                BeginStatusOnActor(package.runtime, play, &player->actor, movementAction, ExternalModStatusType::Slow, true,
                                   movementAction.statusId, package.manifest.id);
            }

            if (movementProfile->gravityScale < 1.0f) {
                movementAction.statusId = "core:high_jump";
                movementAction.damagePerTick = 0;
                BeginStatusOnActor(package.runtime, play, &player->actor, movementAction, ExternalModStatusType::HighJump,
                                   true, movementAction.statusId, package.manifest.id);
            }
            continue;
        }
    }

    return true;
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
    if (type == "hasstatus" || type == "statusremaining") {
        if (gPlayState == nullptr) {
            return false;
        }

        auto* player = GET_PLAYER(gPlayState);
        if (player == nullptr) {
            return false;
        }

        uintptr_t targetAddress = reinterpret_cast<uintptr_t>(&player->actor);
        const auto normalizedScope = ToLower(condition.scope);
        if (normalizedScope == "fronttarget" || normalizedScope == "front_target") {
            Actor* frontTarget = FindStatusTargetInFront(gPlayState, player, 220.0f, ExternalModStatusType::Custom);
            if (frontTarget == nullptr) {
                return false;
            }
            targetAddress = reinterpret_cast<uintptr_t>(frontTarget);
        }

        const std::string statusId = condition.key.empty() ? condition.value : condition.key;
        if (statusId.empty()) {
            return false;
        }

        if (type == "hasstatus") {
            const bool hasStatus = HasActiveStatusOnTarget(package.runtime, targetAddress, statusId);
            if (condition.op.empty()) {
                return hasStatus;
            }
            if (condition.op == "!=") {
                return !hasStatus;
            }
            return hasStatus;
        }

        int32_t compareValue = 0;
        if (condition.hasNumberValue) {
            compareValue = static_cast<int32_t>(std::lround(condition.numberValue));
        } else if (!TryParseIntToken(condition.value, compareValue)) {
            compareValue = 0;
        }
        const int32_t remaining = GetStatusRemainingOnTarget(package.runtime, targetAddress, statusId);
        return CompareIntegerByOperator(remaining, condition.op, compareValue);
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

bool ExternalModManager::TryConsumePendingSceneLoadRequest(int16_t sceneId, ExternalModPendingSceneLoadRequest& outRequest) {
    if (!mPendingSceneLoadRequest.pending) {
        return false;
    }

    if (mPendingSceneLoadRequest.expectedHostSceneId >= 0 && sceneId != mPendingSceneLoadRequest.expectedHostSceneId) {
        return false;
    }

    outRequest = mPendingSceneLoadRequest;
    mPendingSceneLoadRequest = ExternalModPendingSceneLoadRequest{};
    return true;
}

void ExternalModManager::HandlePendingSceneLoadSuccess(const ExternalModPendingSceneLoadRequest& request) {
    SPDLOG_INFO("[ExternalMods] Loaded namespaced scene for {}.{}: resource={} spawn={}", request.modId, request.sceneId,
                request.sceneResourcePath, request.spawnId);
}

void ExternalModManager::HandlePendingSceneLoadFailure(const ExternalModPendingSceneLoadRequest& request,
                                                       const std::string& error) {
    if (request.fallbackPlayable) {
        SPDLOG_WARN(
            "[ExternalMods] Namespaced scene load failed for {}.{} (resource={}): {}. Falling back to host entrance.",
            request.modId, request.sceneId, request.sceneResourcePath, error);
        return;
    }

    auto packageIt = std::find_if(mPackages.begin(), mPackages.end(), [&request](const ExternalModPackage& package) {
        return package.manifest.id == request.modId;
    });
    if (packageIt == mPackages.end()) {
        SPDLOG_WARN(
            "[ExternalMods] Namespaced scene load failed for {}.{} (resource={}): {}. Runtime package not found.",
            request.modId, request.sceneId, request.sceneResourcePath, error);
        return;
    }

    DisableRuntime(*packageIt, "namespaced scene load failed for " + request.sceneId + ": " + error);
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

std::string ExternalModManager::BuildCameraHotkeyScancodeCVarName(const std::string& modId,
                                                                   const std::string& hotkeyId) {
    return "gExternalMods.Hotkeys." + SanitizeCVarSegment(modId) + "." + SanitizeCVarSegment(hotkeyId) + ".Scancode";
}

void ExternalModManager::ApplyDefaultKeyboardMappingsForPackage(const ExternalModPackage& package) const {
    if (!package.runtime.enabled) {
        return;
    }

    auto context = Ship::Context::GetInstance();
    if (context == nullptr || context->GetControlDeck() == nullptr) {
        return;
    }

    auto controller = context->GetControlDeck()->GetControllerByPort(0);
    if (controller == nullptr) {
        return;
    }

    bool changedAnyMapping = false;
    for (const auto& binding : package.runtime.inputBindings) {
        if (binding.defaultKeyboardScancodes.empty()) {
            continue;
        }

        int32_t resolvedModActionMask = 0;
        if (!IsSingleModActionMask(binding.defaultMask, resolvedModActionMask)) {
            SPDLOG_WARN(
                "[ExternalMods] Ignored defaultKeyboardKeys for {}.{}: defaultMask must have exactly one MOD_ACTION bit",
                package.manifest.id, binding.id);
            continue;
        }

        const auto buttonBitmask = static_cast<CONTROLLERBUTTONS_T>(resolvedModActionMask);
        auto controllerButton = controller->GetButtonByBitmask(buttonBitmask);
        if (controllerButton == nullptr) {
            SPDLOG_WARN("[ExternalMods] Ignored defaultKeyboardKeys for {}.{}: unresolved controller button mask {}",
                        package.manifest.id, binding.id, resolvedModActionMask);
            continue;
        }

        auto existingMappings = controllerButton->GetAllButtonMappings();
        bool changedBinding = false;
        for (const auto scancodeValue : binding.defaultKeyboardScancodes) {
            const auto scancode = static_cast<Ship::KbScancode>(scancodeValue);
            if (IsReservedDefaultKeyboardScancode(scancodeValue)) {
                SPDLOG_WARN("[ExternalMods] Skipping reserved default keyboard key for {}.{} (scancode={})",
                            package.manifest.id, binding.id, scancodeValue);
                continue;
            }

            const std::string mappingId = "P" + std::to_string(controller->GetPortIndex()) + "-B" +
                                          std::to_string(static_cast<int32_t>(buttonBitmask)) + "-KB" +
                                          std::to_string(scancodeValue);
            if (existingMappings.contains(mappingId)) {
                continue;
            }

            auto mapping = std::make_shared<Ship::KeyboardKeyToButtonMapping>(controller->GetPortIndex(), buttonBitmask,
                                                                               scancode);
            controllerButton->AddButtonMapping(mapping);
            mapping->SaveToConfig();
            existingMappings[mappingId] = mapping;
            changedBinding = true;
            changedAnyMapping = true;
            SPDLOG_INFO("[ExternalMods] Applied default keyboard mapping for {}.{}: key={} -> mask={}",
                        package.manifest.id, binding.id, scancodeValue, resolvedModActionMask);
        }

        if (changedBinding) {
            controllerButton->SaveButtonMappingIdsToConfig();
        }
    }

    if (changedAnyMapping) {
        context->GetConsoleVariables()->Save();
    }
}

const ExternalModAimCameraProfile* ExternalModManager::FindAimCameraProfileById(const std::string& modId,
                                                                                 const std::string& profileId) const {
    if (profileId.empty()) {
        return nullptr;
    }

    const std::string resolvedProfileId = ResolveProfileIdForMod(modId, profileId);
    if (resolvedProfileId == kCoreDefaultAimCameraProfileId) {
        return &GetCoreDefaultAimCameraProfile();
    }

    if (!modId.empty()) {
        const auto* preferredPackage = FindPackageByModId(modId);
        if (preferredPackage != nullptr && preferredPackage->runtime.enabled) {
            if (const auto* profile =
                    ExternalModContentRegistry::FindAimCameraProfileById(preferredPackage->runtime, resolvedProfileId);
                profile != nullptr) {
                return profile;
            }
        }
    }

    for (const auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }
        if (const auto* profile = ExternalModContentRegistry::FindAimCameraProfileById(package.runtime, resolvedProfileId);
            profile != nullptr) {
            return profile;
        }
    }

    return nullptr;
}

const ExternalModAimCameraProfile* ExternalModManager::ResolveActiveAimCameraProfile() const {
    if (!mAimCameraState.activeProfileId.empty()) {
        if (const auto* profile =
                FindAimCameraProfileById(mAimCameraState.activeProfileOwnerModId, mAimCameraState.activeProfileId);
            profile != nullptr) {
            return profile;
        }
    }

    return &GetCoreDefaultAimCameraProfile();
}

const ExternalModAimCameraProfile* ExternalModManager::ResolveAimCameraProfileForContext(
    ExternalModAimCameraContext context, bool requireMouseFire) const {
    const uint8_t contextMask = AimCameraContextToMask(context);
    const bool hasExplicitActiveProfile = !mAimCameraState.activeProfileId.empty();
    if (hasExplicitActiveProfile) {
        if (const auto* activeProfile =
                FindAimCameraProfileById(mAimCameraState.activeProfileOwnerModId, mAimCameraState.activeProfileId);
            activeProfile != nullptr && (activeProfile->contextsMask & contextMask) != 0 &&
            (!requireMouseFire || activeProfile->mouseFireEnabled)) {
            return activeProfile;
        }
    }

    const ExternalModAimCameraProfile* selectedProfile = nullptr;
    int32_t selectedPriority = std::numeric_limits<int32_t>::min();
    std::string selectedModId;
    for (const auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }

        for (const auto& profile : package.runtime.cameraProfiles) {
            if ((profile.contextsMask & contextMask) == 0) {
                continue;
            }
            if (requireMouseFire && !profile.mouseFireEnabled) {
                continue;
            }

            const bool shouldReplace = selectedProfile == nullptr || package.manifest.loadPriority > selectedPriority ||
                                       (package.manifest.loadPriority == selectedPriority &&
                                        package.manifest.id < selectedModId);
            if (shouldReplace) {
                selectedProfile = &profile;
                selectedPriority = package.manifest.loadPriority;
                selectedModId = package.manifest.id;
            }
        }
    }

    if (selectedProfile != nullptr) {
        return selectedProfile;
    }

    return requireMouseFire ? nullptr : &GetCoreDefaultAimCameraProfile();
}

void ExternalModManager::PruneAimCameraStateForUnavailableProfiles() {
    if (mAimCameraState.activeProfileId.empty()) {
        return;
    }

    if (FindAimCameraProfileById(mAimCameraState.activeProfileOwnerModId, mAimCameraState.activeProfileId) != nullptr) {
        return;
    }

    SPDLOG_WARN("[ExternalMods] Clearing unavailable active aim camera profile '{}'", mAimCameraState.activeProfileId);
    mAimCameraState.activeProfileId.clear();
    mAimCameraState.activeProfileOwnerModId.clear();
}

int16_t ExternalModManager::ResolveAimCameraMode(::PlayState* play, ::Player* player, int16_t defaultMode,
                                                 ExternalModAimCameraContext context) const {
    (void)play;
    (void)player;

    if (!mAimCameraState.overShoulderEnabled) {
        return defaultMode;
    }

    const auto* profile = ResolveActiveAimCameraProfile();
    if (profile == nullptr) {
        return defaultMode;
    }

    const uint8_t contextMask = AimCameraContextToMask(context);
    if ((profile->contextsMask & contextMask) == 0) {
        return defaultMode;
    }

    return GetCameraModeForContext(*profile, context, true);
}

bool ExternalModManager::HandleCameraHotkeyScancode(int32_t scancode) {
    if (scancode <= static_cast<int32_t>(Ship::LUS_KB_UNKNOWN)) {
        return false;
    }

    ExternalModPackage* selectedPackage = nullptr;
    const ExternalModCameraHotkeyDefinition* selectedHotkey = nullptr;

    for (auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }

        for (const auto& hotkey : package.runtime.cameraHotkeys) {
            if (hotkey.defaultKeyboardScancodes.empty()) {
                continue;
            }

            const int32_t defaultScancode = hotkey.defaultKeyboardScancodes.front();
            int32_t effectiveScancode = defaultScancode;
            if (hotkey.allowUserRemap) {
                const auto cvarName = BuildCameraHotkeyScancodeCVarName(package.manifest.id, hotkey.id);
                effectiveScancode = CVarGetInteger(cvarName.c_str(), defaultScancode);
                if (effectiveScancode <= static_cast<int32_t>(Ship::LUS_KB_UNKNOWN)) {
                    effectiveScancode = defaultScancode;
                }
            }

            if (effectiveScancode != scancode) {
                continue;
            }

            const bool shouldReplace = selectedPackage == nullptr ||
                                       package.manifest.loadPriority > selectedPackage->manifest.loadPriority ||
                                       (package.manifest.loadPriority == selectedPackage->manifest.loadPriority &&
                                        (package.manifest.id < selectedPackage->manifest.id ||
                                         (package.manifest.id == selectedPackage->manifest.id &&
                                          hotkey.id < selectedHotkey->id)));
            if (shouldReplace) {
                selectedPackage = &package;
                selectedHotkey = &hotkey;
            }
        }
    }

    if (selectedPackage == nullptr || selectedHotkey == nullptr) {
        return false;
    }

    ExecuteActions(*selectedPackage, { selectedHotkey->action }, selectedHotkey->id.c_str());
    return true;
}

bool ExternalModManager::IsAimMouseFireHeld(::PlayState* play, ::Player* player, int32_t heldItemAction) const {
    const bool debugLogs = CVarGetInteger("gExternalMods.AimMouseFireDebug", 0) != 0;
    const bool allowFrameLog = debugLogs && (play != nullptr) && ((play->state.frames % 120) == 0);

    if (play == nullptr || player == nullptr) {
        if (allowFrameLog) {
            SPDLOG_DEBUG("[ExternalMods] Aim mouse-fire rejected: missing play/player");
        }
        return false;
    }

    ExternalModAimCameraContext context = ExternalModAimCameraContext::CUp;
    if (!TryResolveAimContextFromHeldItemAction(heldItemAction, context)) {
        if (allowFrameLog) {
            SPDLOG_DEBUG("[ExternalMods] Aim mouse-fire rejected: unsupported held item action={}", heldItemAction);
        }
        return false;
    }

    const auto* profile = ResolveAimCameraProfileForContext(context, true);
    if (profile == nullptr) {
        if (allowFrameLog) {
            SPDLOG_DEBUG("[ExternalMods] Aim mouse-fire rejected: no profile with mouseFire for context={}",
                         static_cast<int32_t>(context));
        }
        return false;
    }

    if (profile->mouseFireMode == ExternalModAimMouseFireMode::FirstPerson && mAimCameraState.overShoulderEnabled) {
        if (allowFrameLog) {
            SPDLOG_DEBUG("[ExternalMods] Aim mouse-fire rejected: mode=firstPerson while OTS enabled");
        }
        return false;
    }
    if (profile->mouseFireMode == ExternalModAimMouseFireMode::OverShoulder && !mAimCameraState.overShoulderEnabled) {
        if (allowFrameLog) {
            SPDLOG_DEBUG("[ExternalMods] Aim mouse-fire rejected: mode=overShoulder while OTS disabled");
        }
        return false;
    }

    auto ctx = Ship::Context::GetInstance();
    if (ctx == nullptr || ctx->GetWindow() == nullptr) {
        if (allowFrameLog) {
            SPDLOG_DEBUG("[ExternalMods] Aim mouse-fire rejected: missing window context");
        }
        return false;
    }

    const bool held = ctx->GetWindow()->GetMouseState(ToShipMouseButton(profile->mouseFireButton));
    if (allowFrameLog) {
        SPDLOG_DEBUG("[ExternalMods] Aim mouse-fire context={} profile='{}' held={}", static_cast<int32_t>(context),
                     profile->id, held ? 1 : 0);
    }

    return held;
}

bool ExternalModManager::IsAimOverShoulderEnabled() const {
    return mAimCameraState.overShoulderEnabled;
}

bool ExternalModManager::DrawAimReticleIfActive(::PlayState* play, ::Player* player,
                                                ExternalModAimCameraContext context) const {
    if (play == nullptr || player == nullptr) {
        return false;
    }

    if (context != ExternalModAimCameraContext::Slingshot) {
        return false;
    }

    const ExternalModItemDefinition* selectedDefinition = nullptr;
    int32_t selectedLoadPriority = std::numeric_limits<int32_t>::min();
    std::string selectedModId;
    for (const auto& package : mPackages) {
        if (!package.valid || !package.runtime.enabled) {
            continue;
        }
        for (const auto& definition : package.runtime.itemDefinitions) {
            if (!definition.granted || definition.slot != ExternalModItemSlot::Slingshot ||
                definition.aimReticleTextureI8.empty()) {
                continue;
            }

            if (selectedDefinition == nullptr || package.manifest.loadPriority > selectedLoadPriority ||
                (package.manifest.loadPriority == selectedLoadPriority && package.manifest.id < selectedModId)) {
                selectedDefinition = &definition;
                selectedLoadPriority = package.manifest.loadPriority;
                selectedModId = package.manifest.id;
            }
        }
    }

    if (selectedDefinition == nullptr) {
        return false;
    }

    const auto* profile = ResolveAimCameraProfileForContext(context, false);
    if (profile == nullptr) {
        return false;
    }

    const bool slingshotInHand = Player_HoldsSlingshot(player);
    bool shouldDraw = false;
    switch (profile->reticleVisibility) {
        case ExternalModAimReticleVisibility::AimOnly:
            shouldDraw = slingshotInHand && (player->stateFlags1 & PLAYER_STATE1_READY_TO_FIRE) != 0;
            break;
        case ExternalModAimReticleVisibility::ButtonHold: {
            shouldDraw = false;
            const int32_t buttonIndex = player->heldItemButton;
            if (slingshotInHand && buttonIndex >= 0 && buttonIndex < 8) {
                static constexpr std::array<int32_t, 8> kButtonMasks = {
                    BTN_B, BTN_CLEFT, BTN_CDOWN, BTN_CRIGHT, BTN_DUP, BTN_DDOWN, BTN_DLEFT, BTN_DRIGHT,
                };
                shouldDraw = MatchButtonMask(play->state.input[0].cur.button, kButtonMasks[buttonIndex]);
            }
            break;
        }
        case ExternalModAimReticleVisibility::Selected:
            shouldDraw = slingshotInHand && IsItemIdEquippedOnActionButtons(ITEM_SLINGSHOT);
            break;
        default:
            break;
    }
    if (!shouldDraw) {
        return false;
    }

    const uint8_t* reticleTexture =
        selectedDefinition->aimReticleTextureI8.empty()
            ? reinterpret_cast<const uint8_t*>(gLinkAdultHookshotReticleTex)
            : selectedDefinition->aimReticleTextureI8.data();
    if (reticleTexture == nullptr) {
        return false;
    }

    const float normalizedX = std::clamp(profile->reticleX, 0.0f, 1.0f);
    const float normalizedY = std::clamp(profile->reticleY, 0.0f, 1.0f);

    const int32_t screenWidth = std::max(gScreenWidth, SCREEN_WIDTH);
    const int32_t screenHeight = std::max(gScreenHeight, SCREEN_HEIGHT);
    constexpr int32_t kReticleSize = 64;
    int32_t drawLeft = static_cast<int32_t>(std::lround(normalizedX * static_cast<float>(screenWidth - 1))) - (kReticleSize / 2);
    int32_t drawTop = static_cast<int32_t>(std::lround(normalizedY * static_cast<float>(screenHeight - 1))) - (kReticleSize / 2);
    drawLeft = std::clamp(drawLeft, 0, std::max(0, screenWidth - kReticleSize));
    drawTop = std::clamp(drawTop, 0, std::max(0, screenHeight - kReticleSize));

    GraphicsContext* __gfxCtx = play->state.gfxCtx;
    (void)__gfxCtx;
    OVERLAY_DISP = Gfx_SetupDL(OVERLAY_DISP, 0x07);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(OVERLAY_DISP++, 255, 255, 255, 255);
    gDPLoadTextureBlock(OVERLAY_DISP++, reticleTexture, G_IM_FMT_I, G_IM_SIZ_8b, kAimReticleTextureWidth,
                        kAimReticleTextureHeight, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                        G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPWideTextureRectangle(OVERLAY_DISP++, drawLeft << 2, drawTop << 2, (drawLeft + kReticleSize) << 2,
                            (drawTop + kReticleSize) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

    return true;
}

bool ExternalModManager::HasCustomEquippedSlingshotModel() const {
    if (gPlayState == nullptr) {
        return false;
    }

    auto* player = GET_PLAYER(gPlayState);
    if (player == nullptr || !Player_HoldsSlingshot(player)) {
        return false;
    }

    const auto* definition = FindCustomModelDefinitionForItem(mPackages, ITEM_SLINGSHOT);
    return definition != nullptr && definition->granted;
}

bool ExternalModManager::DrawCustomEquippedSlingshotModel(::PlayState* play) const {
    if (play == nullptr) {
        return false;
    }

    auto* player = GET_PLAYER(play);
    if (player == nullptr || !Player_HoldsSlingshot(player)) {
        return false;
    }

    const auto* definition = FindCustomModelDefinitionForItem(mPackages, ITEM_SLINGSHOT);
    if (definition == nullptr || !definition->granted) {
        return false;
    }

    return DrawCustomItemDefinitionModel(play, *definition, true);
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

        const auto* definition = ExternalModContentRegistry::FindItemDefinitionById(packageIt->runtime, cell.itemId);
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
        view.hasSlot = definition->hasSlot;
        view.placement = definition->placement;
        view.granted = definition->granted;
        view.assignableButtonsMask = definition->assignableButtonsMask;
        if (!definition->iconRgba32.empty()) {
            view.iconRgba32 = definition->iconRgba32.data();
        } else {
            const int32_t fallbackItemId = ResolveGrantedItemId(*definition);
            view.iconRgba32 = ResolveItemIconPointerForId(fallbackItemId, true);
        }
        for (int32_t buttonIndex = 1; buttonIndex <= 7; ++buttonIndex) {
            const auto& assignment = mActionButtonAssignments[buttonIndex];
            if (assignment.source == ActionButtonSource::Mod && assignment.modId == view.modId &&
                assignment.itemId == view.itemId) {
                view.assignedButtonsMask = static_cast<uint8_t>(view.assignedButtonsMask | MakeButtonMask(buttonIndex));
            }
        }
        outCells.push_back(std::move(view));
    }

    return outCells;
}

int32_t ExternalModManager::GetExtraInventoryPageCount() const {
    const size_t cellCount = mExtraInventoryCells.size();
    const size_t pageCount = (cellCount + (kExternalModInventoryCellsPerPage - 1)) / kExternalModInventoryCellsPerPage;
    return static_cast<int32_t>(std::max<size_t>(1, pageCount));
}

bool ExternalModManager::GetExtraInventoryPageCell(int32_t pageIndex, int32_t pageCellIndex,
                                                   ExternalModInventoryCellView& outCell) const {
    outCell = {};
    if (pageIndex < 0 || pageCellIndex < 0 || pageCellIndex >= static_cast<int32_t>(kExternalModInventoryCellsPerPage)) {
        return false;
    }

    const int64_t absoluteIndex = static_cast<int64_t>(pageIndex) * static_cast<int64_t>(kExternalModInventoryCellsPerPage) +
                                  static_cast<int64_t>(pageCellIndex);
    if (absoluteIndex < 0 || absoluteIndex >= static_cast<int64_t>(mExtraInventoryCells.size())) {
        return false;
    }

    const auto allCells = GetExtraInventoryGrid();
    if (absoluteIndex >= static_cast<int64_t>(allCells.size())) {
        return false;
    }

    outCell = allCells[static_cast<size_t>(absoluteIndex)];
    return true;
}

bool ExternalModManager::MoveExtraInventoryCell(size_t fromIndex, size_t toIndex, std::string& outError) {
    outError.clear();

    if (mExtraInventoryCells.empty()) {
        outError = "inventory is empty";
        return false;
    }

    if (fromIndex >= mExtraInventoryCells.size()) {
        outError = "cell index out of range";
        return false;
    }

    if (toIndex >= mExtraInventoryCells.size()) {
        toIndex = mExtraInventoryCells.size() - 1;
    }

    if (fromIndex == toIndex) {
        return true;
    }

    if (mExtraInventoryCells[fromIndex].modId.empty() || mExtraInventoryCells[fromIndex].itemId.empty()) {
        outError = "source cell is empty";
        return false;
    }

    std::swap(mExtraInventoryCells[fromIndex], mExtraInventoryCells[toIndex]);
    MarkPersistentInventoryDirty();
    return true;
}

void ExternalModManager::OnVanillaButtonEquipped(int32_t buttonIndex) {
    if (buttonIndex < 1 || buttonIndex > 7) {
        return;
    }

    auto& assignment = mActionButtonAssignments[buttonIndex];
    if (assignment.source == ActionButtonSource::Vanilla && assignment.modId.empty() && assignment.itemId.empty()) {
        return;
    }

    assignment.source = ActionButtonSource::Vanilla;
    assignment.modId.clear();
    assignment.itemId.clear();
    MarkPersistentInventoryDirty();
}

bool ExternalModManager::TryDrawButtonOverrideIcon(PlayState* play, int32_t buttonIndex, int32_t alpha) const {
    if (play == nullptr || buttonIndex < 1 || buttonIndex > 7) {
        return false;
    }

    const auto& assignment = mActionButtonAssignments[buttonIndex];
    if (assignment.source != ActionButtonSource::Mod || assignment.modId.empty() || assignment.itemId.empty()) {
        return false;
    }

    const auto* definition = FindItemByAssignment(assignment);
    if (definition == nullptr || !definition->granted) {
        return false;
    }

    const uint8_t* iconTexture = definition->iconRgba32.empty()
                                     ? ResolveItemIconPointerForId(ResolveGrantedItemId(*definition), true)
                                     : definition->iconRgba32.data();
    if (iconTexture == nullptr) {
        return false;
    }

    Interface_DrawItemIconTexture(play, const_cast<uint8_t*>(iconTexture), static_cast<s16>(buttonIndex));

    const int32_t ammoItemId = ResolveGrantedAmmoItemId(*definition);
    if (ammoItemId != ITEM_NONE && ammoItemId >= std::numeric_limits<int8_t>::min() &&
        ammoItemId <= std::numeric_limits<int8_t>::max() &&
        buttonIndex < static_cast<int32_t>(ARRAY_COUNT(gSaveContext.equips.buttonItems))) {
        const int8_t previousButtonItem = gSaveContext.equips.buttonItems[buttonIndex];
        gSaveContext.equips.buttonItems[buttonIndex] = static_cast<int8_t>(ammoItemId);
        Interface_DrawAmmoCount(play, static_cast<s16>(buttonIndex), static_cast<s16>(alpha));
        gSaveContext.equips.buttonItems[buttonIndex] = previousButtonItem;
    }

    return true;
}

bool ExternalModManager::DrawSurfBoardIfActive(PlayState* play, Player* player) {
    if (play == nullptr || player == nullptr) {
        return false;
    }
    GraphicsContext* __gfxCtx = play->state.gfxCtx;
    (void)__gfxCtx;

    ExternalModPackage* selectedSurfPackage = nullptr;
    for (auto& package : mPackages) {
        if (!package.runtime.enabled || !package.runtime.surfState.active) {
            continue;
        }
        if (selectedSurfPackage == nullptr || package.manifest.loadPriority > selectedSurfPackage->manifest.loadPriority ||
            (package.manifest.loadPriority == selectedSurfPackage->manifest.loadPriority &&
             package.manifest.id < selectedSurfPackage->manifest.id)) {
            selectedSurfPackage = &package;
        }
    }

    if (selectedSurfPackage == nullptr) {
        return false;
    }

    auto& surfState = selectedSurfPackage->runtime.surfState;
    const auto* profile = ExternalModContentRegistry::FindMovementProfileById(selectedSurfPackage->runtime, surfState.movementProfileId);
    if (profile == nullptr || profile->mode != ExternalModMovementMode::Surf) {
        ClearSurfState(selectedSurfPackage->runtime);
        return false;
    }

    if (!profile->boardRequired) {
        return false;
    }
    if (surfState.idle && !profile->boardVisibleWhenIdle) {
        return false;
    }

    constexpr float kBinangToRad = 3.14159265358979323846f / 32768.0f;
    constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;
    Matrix_Push();
    Matrix_Translate(surfState.boardPosX, surfState.boardPosY, surfState.boardPosZ, MTXMODE_NEW);
    Matrix_RotateY(static_cast<float>(surfState.boardRotY) * kBinangToRad, MTXMODE_APPLY);
    Matrix_RotateX(static_cast<float>(surfState.boardRotX) * kBinangToRad, MTXMODE_APPLY);
    Matrix_RotateZ(static_cast<float>(surfState.boardRotZ) * kBinangToRad, MTXMODE_APPLY);
    Matrix_RotateX(profile->boardPitchOffsetDeg * kDegToRad, MTXMODE_APPLY);
    Matrix_RotateY(profile->boardYawOffsetDeg * kDegToRad, MTXMODE_APPLY);
    Matrix_RotateZ(profile->boardRollOffsetDeg * kDegToRad, MTXMODE_APPLY);
    Matrix_Scale(surfState.boardScale, surfState.boardScale, surfState.boardScale, MTXMODE_APPLY);

    bool drewBoard = false;
    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    if (!profile->boardModelAsset.empty()) {
        if (auto* boardDisplayList = ResourceMgr_LoadGfxByName(profile->boardModelAsset.c_str()); boardDisplayList != nullptr) {
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);
            gSPDisplayList(POLY_OPA_DISP++, boardDisplayList);
            drewBoard = true;
        }
    }

    if (!drewBoard && !surfState.sourceItemId.empty()) {
        const auto itemIt = std::find_if(selectedSurfPackage->runtime.itemDefinitions.begin(),
                                         selectedSurfPackage->runtime.itemDefinitions.end(),
                                         [&surfState](const ExternalModItemDefinition& item) {
                                             return item.id == surfState.sourceItemId && HasCustomGetItemModel(item);
                                         });
        if (itemIt != selectedSurfPackage->runtime.itemDefinitions.end()) {
            drewBoard = DrawCustomItemDefinitionModel(play, *itemIt, true);
        }
    }

    if (!drewBoard) {
        if (auto* fallbackDisplayList = ResourceMgr_LoadGfxByName(gGiHylianShieldDL); fallbackDisplayList != nullptr) {
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);
            gSPDisplayList(POLY_OPA_DISP++, fallbackDisplayList);
            drewBoard = true;
        }
    }

    Matrix_Pop();
    return drewBoard;
}

void ExternalModManager::LoadPersistentInventoryState() {
    mExtraInventoryCells.clear();
    mExtraInventoryPage = 0;
    mExtraInventoryCursor = 0;
    for (auto& assignment : mActionButtonAssignments) {
        assignment = ActionButtonAssignment{};
    }

    auto* saveManager = SaveManager::Instance;
    if (saveManager == nullptr) {
        return;
    }

    int32_t page = 0;
    int32_t cursor = 0;
    saveManager->LoadData("page", page, 0);
    saveManager->LoadData("cursor", cursor, 0);
    mExtraInventoryPage = std::max(0, page);
    mExtraInventoryCursor = std::max(0, cursor);

    size_t cellCount = 0;
    saveManager->LoadData("cellCount", cellCount, static_cast<size_t>(0));
    cellCount = std::min(cellCount, kExternalModInventoryMaxCellCount);
    std::unordered_set<std::string> seenCells;
    saveManager->LoadArray("cells", cellCount, [&](size_t i) {
        (void)i;
        saveManager->LoadStruct("", [&]() {
            std::string modId;
            std::string itemId;
            saveManager->LoadData("modId", modId, std::string{});
            saveManager->LoadData("itemId", itemId, std::string{});
            if (modId.empty() || itemId.empty()) {
                return;
            }
            const auto key = modId + "\x1F" + itemId;
            if (!seenCells.insert(key).second) {
                return;
            }
            mExtraInventoryCells.push_back({ modId, itemId });
        });
    });

    size_t assignmentCount = 0;
    saveManager->LoadData("assignmentCount", assignmentCount, static_cast<size_t>(0));
    assignmentCount = std::min<size_t>(assignmentCount, 7);
    saveManager->LoadArray("assignments", assignmentCount, [&](size_t i) {
        saveManager->LoadStruct("", [&]() {
            int32_t loadedButtonIndex = static_cast<int32_t>(i + 1);
            int32_t source = 0;
            std::string modId;
            std::string itemId;
            saveManager->LoadData("buttonIndex", loadedButtonIndex, loadedButtonIndex);
            saveManager->LoadData("source", source, 0);
            saveManager->LoadData("modId", modId, std::string{});
            saveManager->LoadData("itemId", itemId, std::string{});
            if (loadedButtonIndex < 1 || loadedButtonIndex > 7) {
                return;
            }
            auto& assignment = mActionButtonAssignments[loadedButtonIndex];
            if (source != 0 && !modId.empty() && !itemId.empty()) {
                assignment.source = ActionButtonSource::Mod;
                assignment.modId = std::move(modId);
                assignment.itemId = std::move(itemId);
            } else {
                assignment = ActionButtonAssignment{};
            }
        });
    });

    mPersistentInventoryDirty = false;
}

void ExternalModManager::SavePersistentInventoryState() const {
    auto* saveManager = SaveManager::Instance;
    if (saveManager == nullptr) {
        return;
    }

    saveManager->SaveData("page", mExtraInventoryPage);
    saveManager->SaveData("cursor", mExtraInventoryCursor);

    saveManager->SaveData("cellCount", mExtraInventoryCells.size());
    saveManager->SaveArray("cells", mExtraInventoryCells.size(), [&](size_t i) {
        saveManager->SaveStruct("", [&]() {
            saveManager->SaveData("modId", mExtraInventoryCells[i].modId);
            saveManager->SaveData("itemId", mExtraInventoryCells[i].itemId);
        });
    });

    constexpr size_t kAssignmentCount = 7;
    saveManager->SaveData("assignmentCount", kAssignmentCount);
    saveManager->SaveArray("assignments", kAssignmentCount, [&](size_t i) {
        const int32_t indexedButton = static_cast<int32_t>(i + 1);
        const auto& assignment = mActionButtonAssignments[indexedButton];
        saveManager->SaveStruct("", [&]() {
            saveManager->SaveData("buttonIndex", indexedButton);
            saveManager->SaveData("source", assignment.source == ActionButtonSource::Mod ? 1 : 0);
            saveManager->SaveData("modId", assignment.modId);
            saveManager->SaveData("itemId", assignment.itemId);
        });
    });
}

const ExternalModPackage* ExternalModManager::FindPackageByModId(const std::string& modId) const {
    const auto packageIt = std::find_if(mPackages.begin(), mPackages.end(), [&modId](const ExternalModPackage& package) {
        return package.manifest.id == modId;
    });
    if (packageIt == mPackages.end()) {
        return nullptr;
    }
    return &(*packageIt);
}

ExternalModPackage* ExternalModManager::FindPackageByModId(const std::string& modId) {
    const auto packageIt = std::find_if(mPackages.begin(), mPackages.end(), [&modId](const ExternalModPackage& package) {
        return package.manifest.id == modId;
    });
    if (packageIt == mPackages.end()) {
        return nullptr;
    }
    return &(*packageIt);
}

int32_t ExternalModManager::WasmHostUseItemProfile(const std::string& modId, const std::string& itemOrProfileId) {
    auto* package = FindPackageByModId(modId);
    if (package == nullptr || !package->runtime.enabled || gPlayState == nullptr) {
        return -1;
    }

    auto* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return -2;
    }

    const ExternalModItemDefinition* sourceItem = nullptr;
    std::string useProfileId = itemOrProfileId;
    const auto* directProfile = ExternalModContentRegistry::FindItemUseProfileById(package->runtime, itemOrProfileId);
    if (directProfile == nullptr) {
        for (const auto& item : package->runtime.itemDefinitions) {
            if (item.id != itemOrProfileId) {
                continue;
            }
            sourceItem = &item;
            useProfileId = item.useProfile;
            break;
        }
    }

    if (useProfileId.empty()) {
        return -3;
    }

    std::string useProfileError;
    if (!ExecuteUseProfileById(*package, useProfileId, gPlayState, player, sourceItem, useProfileError)) {
        SPDLOG_WARN("[ExternalMods][WASM:{}] host_useItemProfile failed: {}", modId, useProfileError);
        return -4;
    }

    return 0;
}

int32_t ExternalModManager::WasmHostResolveTarget(const std::string& modId, const std::string& profileId,
                                                  std::vector<int32_t>& outHandles) {
    outHandles.clear();

    auto* package = FindPackageByModId(modId);
    if (package == nullptr || !package->runtime.enabled || gPlayState == nullptr) {
        return -1;
    }
    auto* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return -2;
    }

    const auto* profile = ExternalModContentRegistry::FindTargetingProfileById(package->runtime, profileId);
    if (profile == nullptr) {
        return -3;
    }

    std::vector<Actor*> targets;
    ResolveTargetsForTargetingProfile(*profile, gPlayState, player, targets);
    if (targets.empty()) {
        return 0;
    }

    if (package->runtime.wasmTargetHandles.size() > 4096) {
        package->runtime.wasmTargetHandles.clear();
    }

    for (Actor* actor : targets) {
        if (actor == nullptr) {
            continue;
        }
        int32_t handle = package->runtime.wasmNextTargetHandle++;
        if (handle <= 0) {
            package->runtime.wasmNextTargetHandle = 1;
            handle = package->runtime.wasmNextTargetHandle++;
        }
        package->runtime.wasmTargetHandles[handle] = reinterpret_cast<uintptr_t>(actor);
        outHandles.push_back(handle);
    }

    return static_cast<int32_t>(outHandles.size());
}

int32_t ExternalModManager::WasmHostDealDamage(const std::string& modId, int32_t targetHandle,
                                               const std::string& damageProfileId) {
    auto* package = FindPackageByModId(modId);
    if (package == nullptr || !package->runtime.enabled || gPlayState == nullptr) {
        return -1;
    }

    const auto* damageProfile = ExternalModContentRegistry::FindDamageProfileById(package->runtime, damageProfileId);
    if (damageProfile == nullptr) {
        return -2;
    }

    Actor* target = nullptr;
    if (targetHandle == 0) {
        auto* player = GET_PLAYER(gPlayState);
        target = player != nullptr ? &player->actor : nullptr;
    } else {
        const auto handleIt = package->runtime.wasmTargetHandles.find(targetHandle);
        if (handleIt == package->runtime.wasmTargetHandles.end()) {
            return -3;
        }
        target = FindActorByAddress(gPlayState, handleIt->second, -1);
    }

    if (target == nullptr) {
        return -4;
    }

    ApplyDamageProfileToActor(gPlayState, target, *damageProfile);
    return 0;
}

int32_t ExternalModManager::WasmHostApplyStatus(const std::string& modId, int32_t targetHandle, const std::string& statusId,
                                                int32_t durationOverrideFrames) {
    auto* package = FindPackageByModId(modId);
    if (package == nullptr || !package->runtime.enabled || gPlayState == nullptr) {
        return -1;
    }

    auto* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return -2;
    }

    Actor* target = nullptr;
    if (targetHandle == 0) {
        target = &player->actor;
    } else {
        const auto handleIt = package->runtime.wasmTargetHandles.find(targetHandle);
        if (handleIt == package->runtime.wasmTargetHandles.end()) {
            return -3;
        }
        target = FindActorByAddress(gPlayState, handleIt->second, -1);
    }
    if (target == nullptr) {
        return -4;
    }

    ExternalModAction action;
    action.statusId = statusId;
    if (durationOverrideFrames > 0) {
        action.durationFrames = durationOverrideFrames;
    }
    action.tickFrames = 15;
    action.damagePerTick = 0;
    action.intensity = 255;
    action.shakeFrames = 12;

    ExternalModAction resolvedAction;
    ExternalModStatusType resolvedType = ExternalModStatusType::Custom;
    if (!ResolveStatusActionData(*package, action, resolvedAction, resolvedType)) {
        return -5;
    }
    if (ShouldProtectStatusTarget(target, resolvedType)) {
        return -6;
    }

    const bool isPlayerTarget = (target == &player->actor);
    BeginStatusOnActor(package->runtime, gPlayState, target, resolvedAction, resolvedType, isPlayerTarget,
                       resolvedAction.statusId, package->manifest.id);
    return 0;
}

int32_t ExternalModManager::WasmHostSpawnProjectile(const std::string& modId, const std::string& profileId,
                                                    const std::string& overridesJson) {
    auto* package = FindPackageByModId(modId);
    if (package == nullptr || !package->runtime.enabled || gPlayState == nullptr) {
        return -1;
    }
    auto* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return -2;
    }

    float range = 180.0f;
    if (!overridesJson.empty()) {
        try {
            const auto json = nlohmann::json::parse(overridesJson);
            if (json.contains("range") && json["range"].is_number()) {
                range = std::max(1.0f, json["range"].get<float>());
            }
        } catch (...) {
        }
    }

    ExternalModUseProfileEffect effect;
    effect.action = "spawnProjectile";
    effect.projectileProfileId = profileId;

    ExternalModTargetingProfile targetingProfile = {};
    targetingProfile.mode = ExternalModTargetingMode::FrontTarget;
    targetingProfile.range = range;
    std::vector<Actor*> targets;
    ResolveTargetsForTargetingProfile(targetingProfile, gPlayState, player, targets);

    std::string executeError;
    if (!ExecuteUseProfileEffects(*package, { effect }, targets, gPlayState, player, nullptr, executeError)) {
        SPDLOG_WARN("[ExternalMods][WASM:{}] host_spawnProjectile failed: {}", modId, executeError);
        return -3;
    }
    return 0;
}

int32_t ExternalModManager::WasmHostSpawnAoE(const std::string& modId, const std::string& profileId,
                                             const std::string& originJson) {
    (void)originJson;
    auto* package = FindPackageByModId(modId);
    if (package == nullptr || !package->runtime.enabled || gPlayState == nullptr) {
        return -1;
    }
    auto* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return -2;
    }

    ExternalModUseProfileEffect effect;
    effect.action = "spawnAoE";
    effect.aoeProfileId = profileId;

    std::vector<Actor*> targets = { &player->actor };
    std::string executeError;
    if (!ExecuteUseProfileEffects(*package, { effect }, targets, gPlayState, player, nullptr, executeError)) {
        SPDLOG_WARN("[ExternalMods][WASM:{}] host_spawnAoE failed: {}", modId, executeError);
        return -3;
    }
    return 0;
}

int32_t ExternalModManager::WasmHostApplyMovementProfile(const std::string& modId, const std::string& profileId,
                                                         int32_t durationFrames) {
    auto* package = FindPackageByModId(modId);
    if (package == nullptr || !package->runtime.enabled || gPlayState == nullptr) {
        return -1;
    }
    auto* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return -2;
    }

    ExternalModUseProfileEffect effect;
    effect.action = "applyMovementProfile";
    effect.movementProfileId = profileId;
    effect.durationFrames = std::max(durationFrames, 0);

    std::vector<Actor*> targets = { &player->actor };
    std::string executeError;
    if (!ExecuteUseProfileEffects(*package, { effect }, targets, gPlayState, player, nullptr, executeError)) {
        SPDLOG_WARN("[ExternalMods][WASM:{}] host_applyMovementProfile failed: {}", modId, executeError);
        return -3;
    }
    return 0;
}

int32_t ExternalModManager::WasmHostApplyImpulse(const std::string& modId, int32_t mode, float strength, float x, float y,
                                                 float z) {
    (void)modId;
    if (gPlayState == nullptr) {
        return -1;
    }
    auto* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return -2;
    }

    const float clampedStrength = std::clamp(strength, 0.5f, 35.0f);
    if (mode == 1) {
        player->actor.velocity.x += x * clampedStrength;
        player->actor.velocity.y += y * clampedStrength;
        player->actor.velocity.z += z * clampedStrength;
        player->linearVelocity = std::max(player->linearVelocity, clampedStrength);
    } else {
        ExternalModItemRuntime::ApplySkyhookImpulse(player, clampedStrength, clampedStrength);
    }

    return 0;
}

int32_t ExternalModManager::WasmHostGetGroundInfo(const std::string& modId, ExternalModWasmGroundInfo& outInfo) {
    (void)modId;
    constexpr float kRadToDeg = 57.29577951308232f;
    if (gPlayState == nullptr) {
        return -1;
    }
    auto* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return -2;
    }

    if (player->actor.floorPoly != nullptr) {
        outInfo.normalX = COLPOLY_GET_NORMAL(player->actor.floorPoly->normal.x);
        outInfo.normalY = COLPOLY_GET_NORMAL(player->actor.floorPoly->normal.y);
        outInfo.normalZ = COLPOLY_GET_NORMAL(player->actor.floorPoly->normal.z);
        outInfo.surfaceType =
            static_cast<int32_t>(SurfaceType_GetSlope(&gPlayState->colCtx, player->actor.floorPoly, player->actor.floorBgId));
    } else {
        outInfo.normalX = 0.0f;
        outInfo.normalY = 1.0f;
        outInfo.normalZ = 0.0f;
        outInfo.surfaceType = 0;
    }
    const float clampedNormalY = std::clamp(outInfo.normalY, -1.0f, 1.0f);
    outInfo.slopeAngleDeg = acosf(clampedNormalY) * kRadToDeg;
    outInfo.isOnGround = ((player->actor.bgCheckFlags & 1) != 0) ? 1 : 0;
    return 0;
}

int32_t ExternalModManager::WasmHostRaycast(const std::string& modId, const std::string& queryJson,
                                            ExternalModWasmRaycastHit& outHit) {
    std::vector<ExternalModWasmRaycastHit> hits;
    const int32_t result = WasmHostRaycastAll(modId, queryJson, 1, hits);
    if (result <= 0 || hits.empty()) {
        outHit = {};
        return result;
    }
    outHit = hits.front();
    return 1;
}

int32_t ExternalModManager::WasmHostRaycastAll(const std::string& modId, const std::string& queryJson, int32_t outCapacity,
                                               std::vector<ExternalModWasmRaycastHit>& outHits) {
    outHits.clear();
    auto* package = FindPackageByModId(modId);
    if (package == nullptr || !package->runtime.enabled || gPlayState == nullptr) {
        return -1;
    }
    auto* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return -2;
    }

    float range = 180.0f;
    bool includeProps = true;
    bool includeEnemies = true;
    bool stopOnWall = true;

    if (!queryJson.empty()) {
        try {
            const auto json = nlohmann::json::parse(queryJson);
            if (json.contains("range") && json["range"].is_number()) {
                range = std::max(1.0f, json["range"].get<float>());
            }
            if (json.contains("includeProps") && json["includeProps"].is_boolean()) {
                includeProps = json["includeProps"].get<bool>();
            }
            if (json.contains("includeEnemies") && json["includeEnemies"].is_boolean()) {
                includeEnemies = json["includeEnemies"].get<bool>();
            }
            if (json.contains("stopOnWall") && json["stopOnWall"].is_boolean()) {
                stopOnWall = json["stopOnWall"].get<bool>();
            }
        } catch (...) {
        }
    }

    std::vector<Actor*> actorHits;
    ResolveForwardRaycastHits(gPlayState, player, range, includeProps, includeEnemies, stopOnWall, actorHits);
    if (actorHits.empty()) {
        return 0;
    }

    if (package->runtime.wasmTargetHandles.size() > 4096) {
        package->runtime.wasmTargetHandles.clear();
    }

    const int32_t clampedCapacity = std::max(0, outCapacity);
    const int32_t maxCount = std::min<int32_t>(clampedCapacity, static_cast<int32_t>(actorHits.size()));
    outHits.reserve(static_cast<size_t>(maxCount));

    for (int32_t i = 0; i < maxCount; ++i) {
        Actor* actor = actorHits[static_cast<size_t>(i)];
        if (actor == nullptr) {
            continue;
        }

        int32_t handle = package->runtime.wasmNextTargetHandle++;
        if (handle <= 0) {
            package->runtime.wasmNextTargetHandle = 1;
            handle = package->runtime.wasmNextTargetHandle++;
        }
        package->runtime.wasmTargetHandles[handle] = reinterpret_cast<uintptr_t>(actor);

        ExternalModWasmRaycastHit hit;
        hit.hasHit = 1;
        hit.actorHandle = handle;
        hit.distance = Math_Vec3f_DistXYZ(&player->actor.world.pos, &actor->world.pos);
        hit.posX = actor->world.pos.x;
        hit.posY = actor->world.pos.y;
        hit.posZ = actor->world.pos.z;
        outHits.push_back(hit);
    }

    return static_cast<int32_t>(outHits.size());
}

const ExternalModItemDefinition* ExternalModManager::FindItemByAssignment(const ActionButtonAssignment& assignment) const {
    if (assignment.modId.empty() || assignment.itemId.empty()) {
        return nullptr;
    }

    const auto* package = FindPackageByModId(assignment.modId);
    if (package == nullptr || !package->runtime.enabled) {
        return nullptr;
    }

    return ExternalModContentRegistry::FindItemDefinitionById(package->runtime, assignment.itemId);
}

ExternalModItemDefinition* ExternalModManager::FindItemByAssignment(ActionButtonAssignment& assignment) {
    if (assignment.modId.empty() || assignment.itemId.empty()) {
        return nullptr;
    }

    auto* package = FindPackageByModId(assignment.modId);
    if (package == nullptr || !package->runtime.enabled) {
        return nullptr;
    }

    auto itemIt = std::find_if(package->runtime.itemDefinitions.begin(), package->runtime.itemDefinitions.end(),
                               [&assignment](const ExternalModItemDefinition& definition) {
                                   return definition.id == assignment.itemId;
                               });
    if (itemIt == package->runtime.itemDefinitions.end()) {
        return nullptr;
    }
    return &(*itemIt);
}

bool ExternalModManager::IsAssignmentValid(const ActionButtonAssignment& assignment) const {
    const auto* definition = FindItemByAssignment(assignment);
    return definition != nullptr && definition->granted;
}

void ExternalModManager::SyncButtonAssignments() {
    for (int32_t buttonIndex = 1; buttonIndex <= 7; ++buttonIndex) {
        auto& assignment = mActionButtonAssignments[buttonIndex];
        if (assignment.source != ActionButtonSource::Mod) {
            continue;
        }

        const auto* definition = FindItemByAssignment(assignment);
        if (definition == nullptr || !definition->granted ||
            (definition->assignableButtonsMask & MakeButtonMask(buttonIndex)) == 0) {
            assignment.source = ActionButtonSource::Vanilla;
            assignment.modId.clear();
            assignment.itemId.clear();
            MarkPersistentInventoryDirty();
            continue;
        }

        if (buttonIndex >= 0 && buttonIndex < static_cast<int32_t>(ARRAY_COUNT(gSaveContext.equips.buttonItems))) {
            gSaveContext.equips.buttonItems[buttonIndex] = ITEM_NONE;
            gSaveContext.adultEquips.buttonItems[buttonIndex] = ITEM_NONE;
            gSaveContext.childEquips.buttonItems[buttonIndex] = ITEM_NONE;
        }
        if ((buttonIndex - 1) >= 0 &&
            (buttonIndex - 1) < static_cast<int32_t>(ARRAY_COUNT(gSaveContext.equips.cButtonSlots))) {
            gSaveContext.equips.cButtonSlots[buttonIndex - 1] = SLOT_NONE;
            gSaveContext.adultEquips.cButtonSlots[buttonIndex - 1] = SLOT_NONE;
            gSaveContext.childEquips.cButtonSlots[buttonIndex - 1] = SLOT_NONE;
        }
    }
}

void ExternalModManager::MarkPersistentInventoryDirty() {
    mPersistentInventoryDirty = true;

    if (!mSaveSectionRegistered || mPersistentInventorySectionId < 0 || SaveManager::Instance == nullptr) {
        return;
    }

    if (gSaveContext.fileNum < 0 || gSaveContext.fileNum >= SaveManager::MaxFiles) {
        return;
    }

    if (!SaveManager::Instance->SaveFile_Exist(gSaveContext.fileNum)) {
        return;
    }

    SaveManager::Instance->SaveSection(gSaveContext.fileNum, mPersistentInventorySectionId, true);
    mPersistentInventoryDirty = false;
}

bool ExternalModManager::TryInvokeAssignedModItem(int32_t buttonIndex, PlayState* play, Player* player) {
    if (buttonIndex < 1 || buttonIndex > 7 || play == nullptr || player == nullptr) {
        return false;
    }

    auto& assignment = mActionButtonAssignments[buttonIndex];
    if (assignment.source != ActionButtonSource::Mod) {
        return false;
    }

    auto* package = FindPackageByModId(assignment.modId);
    if (package == nullptr || !package->runtime.enabled) {
        return false;
    }

    auto* definition = FindItemByAssignment(assignment);
    if (definition == nullptr || !definition->granted) {
        return false;
    }

    if ((definition->assignableButtonsMask & MakeButtonMask(buttonIndex)) == 0) {
        return false;
    }

    if (definition->cooldownRemaining > 0) {
        return true;
    }

    const int32_t resolvedItemId = ResolveGrantedItemId(*definition);
    if (package->runtime.wasmRuntime && !definition->onUseExport.empty()) {
        std::vector<int32_t> args = { resolvedItemId };
        std::string wasmError;
        if (!package->runtime.wasmRuntime->InvokeExport(definition->onUseExport, args, wasmError)) {
            DisableRuntime(*package, "WASM onUse failed for assigned item '" + definition->id + "': " + wasmError);
            return true;
        }
    }

    if (!definition->onUseBehavior.empty()) {
        int32_t stepCount = package->runtime.behaviorStepsThisFrame;
        std::vector<ExternalModAction> behaviorActions;
        std::string behaviorError;
        if (!CollectBehaviorEventActions(*package, definition->onUseBehavior, "onitemused", nullptr,
                                         package->runtime.behaviorMaxStepsPerModPerFrame, stepCount, behaviorActions,
                                         behaviorError)) {
            DisableRuntime(*package, "onUseBehavior failed for assigned item '" + definition->id + "': " + behaviorError);
            return true;
        }
        package->runtime.behaviorStepsThisFrame = stepCount;
        if (!behaviorActions.empty()) {
            ExecuteActions(*package, behaviorActions, "buttonAssignedItemOnUse");
            if (!package->runtime.enabled) {
                return true;
            }
        }
    }

    if (!definition->useProfile.empty()) {
        std::string useProfileError;
        if (!ExecuteUseProfileById(*package, definition->useProfile, play, player, definition, useProfileError)) {
            DisableRuntime(*package, "item useProfile failed for assigned item '" + definition->id + "': " + useProfileError);
            return true;
        }
    }

    if (definition->hasSlot && definition->slot == ExternalModItemSlot::Hookshot &&
        definition->useMode != ExternalModItemUseMode::Vanilla) {
        const float pullForce = GetParamOrDefault(definition->params, "pullForce", 8.0f);
        const float speed = GetParamOrDefault(definition->params, "speed", 12.0f);
        ExternalModItemRuntime::ApplySkyhookImpulse(player, pullForce, speed);
    }

    int32_t defaultCooldown = definition->cooldownFrames;
    if (!definition->useProfile.empty()) {
        const auto* useProfile = ExternalModContentRegistry::FindItemUseProfileById(package->runtime, definition->useProfile);
        if (useProfile != nullptr) {
            defaultCooldown = useProfile->cooldownFrames;
        }
    }
    const auto cooldown = static_cast<int32_t>(
        std::max(0.0f, GetParamOrDefault(definition->params, "cooldown", static_cast<float>(defaultCooldown))));
    definition->cooldownFrames = cooldown;
    definition->cooldownRemaining = cooldown;
    return true;
}

void ExternalModManager::ProcessAssignedActionButtons(PlayState* play, Player* player, void* inputPtr) {
    if (play == nullptr || player == nullptr || inputPtr == nullptr) {
        return;
    }
    auto* input = static_cast<Input*>(inputPtr);

    for (const auto& mapping : kEquippedActionButtonMappings) {
        if (!MatchPressedButtonMask(input->cur.button, input->prev.button, mapping.mask)) {
            continue;
        }

        if (!TryInvokeAssignedModItem(mapping.slotIndex, play, player)) {
            continue;
        }

        input->cur.button &= ~mapping.mask;
        input->press.button &= ~mapping.mask;
    }
}

bool ExternalModManager::EquipExtraInventoryCellToButton(size_t cellIndex, int32_t buttonIndex, std::string& outError) {
    outError.clear();

    if (cellIndex >= mExtraInventoryCells.size()) {
        outError = "cell index out of range";
        return false;
    }

    if (buttonIndex < 1 || buttonIndex > 7) {
        outError = "button index out of range";
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

    const auto buttonMask = MakeButtonMask(buttonIndex);
    if ((itemIt->assignableButtonsMask & buttonMask) == 0) {
        outError = "item is not assignable to this button";
        return false;
    }

    const bool isLegacyVanillaSlot = itemIt->placement == ExternalModItemPlacement::Legacy && itemIt->hasSlot;
    const bool isCButton = buttonIndex >= 1 && buttonIndex <= 3;
    if (isLegacyVanillaSlot && isCButton) {
        const auto* slotConfig = FindItemSlotConfig(itemIt->slot);
        if (slotConfig == nullptr) {
            outError = "legacy item slot is unsupported";
            return false;
        }

        const int32_t resolvedItemId = ResolveGrantedItemId(*itemIt);
        if (resolvedItemId == ITEM_NONE || resolvedItemId < std::numeric_limits<int8_t>::min() ||
            resolvedItemId > std::numeric_limits<int8_t>::max()) {
            outError = "legacy item has invalid resolved item id";
            return false;
        }

        auto& assignment = mActionButtonAssignments[buttonIndex];
        assignment.source = ActionButtonSource::Vanilla;
        assignment.modId.clear();
        assignment.itemId.clear();

        gSaveContext.equips.buttonItems[buttonIndex] = static_cast<int8_t>(resolvedItemId);
        gSaveContext.adultEquips.buttonItems[buttonIndex] = static_cast<int8_t>(resolvedItemId);
        gSaveContext.childEquips.buttonItems[buttonIndex] = static_cast<int8_t>(resolvedItemId);
        gSaveContext.equips.cButtonSlots[buttonIndex - 1] = static_cast<int8_t>(slotConfig->slotIndex);
        gSaveContext.adultEquips.cButtonSlots[buttonIndex - 1] = static_cast<int8_t>(slotConfig->slotIndex);
        gSaveContext.childEquips.cButtonSlots[buttonIndex - 1] = static_cast<int8_t>(slotConfig->slotIndex);

        if (gPlayState != nullptr) {
            Interface_LoadItemIcon1(gPlayState, static_cast<uint16_t>(buttonIndex));
        }

        SPDLOG_INFO("[ExternalMods] Equipped legacy item {} from mod {} as vanilla to C-button {}", itemIt->id,
                    packageIt->manifest.id, buttonIndex);
        MarkPersistentInventoryDirty();
        return true;
    }

    mActionButtonAssignments[buttonIndex].source = ActionButtonSource::Mod;
    mActionButtonAssignments[buttonIndex].modId = cell.modId;
    mActionButtonAssignments[buttonIndex].itemId = cell.itemId;

    if (buttonIndex >= 0 && buttonIndex < static_cast<int32_t>(ARRAY_COUNT(gSaveContext.equips.buttonItems))) {
        gSaveContext.equips.buttonItems[buttonIndex] = ITEM_NONE;
    }
    if (buttonIndex >= 0 && buttonIndex < static_cast<int32_t>(ARRAY_COUNT(gSaveContext.adultEquips.buttonItems))) {
        gSaveContext.adultEquips.buttonItems[buttonIndex] = ITEM_NONE;
        gSaveContext.childEquips.buttonItems[buttonIndex] = ITEM_NONE;
    }
    if ((buttonIndex - 1) >= 0 &&
        (buttonIndex - 1) < static_cast<int32_t>(ARRAY_COUNT(gSaveContext.equips.cButtonSlots))) {
        gSaveContext.equips.cButtonSlots[buttonIndex - 1] = SLOT_NONE;
        gSaveContext.adultEquips.cButtonSlots[buttonIndex - 1] = SLOT_NONE;
        gSaveContext.childEquips.cButtonSlots[buttonIndex - 1] = SLOT_NONE;
    }

    if (gPlayState != nullptr) {
        Interface_LoadItemIcon1(gPlayState, static_cast<uint16_t>(buttonIndex));
    }

    if (isLegacyVanillaSlot && !isCButton) {
        SPDLOG_INFO("[ExternalMods] Legacy item {} from mod {} assigned to D-button {} as mod-only", itemIt->id,
                    packageIt->manifest.id, buttonIndex);
    }

    SPDLOG_INFO("[ExternalMods] Equipped virtual inventory item {} from mod {} to button {}", itemIt->id,
                packageIt->manifest.id, buttonIndex);
    MarkPersistentInventoryDirty();
    return true;
}

void ExternalModManager::SyncExtraInventoryGrid() {
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
    for (auto cellIt = mExtraInventoryCells.begin(); cellIt != mExtraInventoryCells.end();) {
        const auto key = buildCellKey(cellIt->modId, cellIt->itemId);
        const bool keep = !cellIt->modId.empty() && !cellIt->itemId.empty() && grantedKeys.contains(key) && !placedKeys.contains(key);
        if (!keep) {
            cellIt = mExtraInventoryCells.erase(cellIt);
            continue;
        }
        placedKeys.insert(key);
        ++cellIt;
    }

    bool warnedExcess = false;
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

            if (mExtraInventoryCells.size() >= kExternalModInventoryMaxCellCount) {
                if (!warnedExcess) {
                    SPDLOG_WARN("[ExternalMods] Virtual inventory reached max capacity ({} cells); additional items were skipped",
                                mExtraInventoryCells.size());
                    warnedExcess = true;
                }
                break;
            }

            mExtraInventoryCells.push_back({ package.manifest.id, item.id });
            placedKeys.insert(key);
        }
    }

    const size_t populatedCount = mExtraInventoryCells.size();
    const size_t minimumCellCount =
        std::max(kExternalModInventoryCellsPerPage,
                 ((populatedCount + (kExternalModInventoryCellsPerPage - 1)) / kExternalModInventoryCellsPerPage) *
                     kExternalModInventoryCellsPerPage);
    mExtraInventoryCells.resize(minimumCellCount, ExtraInventoryCell{});

    SyncButtonAssignments();

    if (mExtraInventoryCells.empty()) {
        mExtraInventoryPage = 0;
        mExtraInventoryCursor = 0;
    } else {
        const int32_t pageCount = GetExtraInventoryPageCount();
        mExtraInventoryPage = std::clamp(mExtraInventoryPage, 0, std::max(0, pageCount - 1));
        mExtraInventoryCursor =
            std::clamp(mExtraInventoryCursor, 0, static_cast<int32_t>(kExternalModInventoryCellsPerPage - 1));
    }
}

void ExternalModManager::Shutdown() {
    UnregisterHooks();
    mPendingSceneLoadRequest = ExternalModPendingSceneLoadRequest{};
    Player* player = gPlayState != nullptr ? GET_PLAYER(gPlayState) : nullptr;
    for (auto& package : mPackages) {
        ClearStatusEffects(package.runtime, gPlayState, false);
        ClearSurfState(package.runtime);
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
    mPendingSceneLoadRequest = ExternalModPendingSceneLoadRequest{};
    mAimCameraState.overShoulderEnabled =
        CVarGetInteger(kAimCameraOverShoulderCVar, mAimCameraState.overShoulderEnabled ? 1 : 0) != 0;

    if (!mSaveSectionRegistered && SaveManager::Instance != nullptr) {
        SaveManager::Instance->AddLoadFunction(kExternalModsInventorySaveSectionName, kExternalModsInventorySaveVersion,
                                               LoadExternalModsInventorySection);
        mPersistentInventorySectionId = SaveManager::Instance->AddSaveFunction(
            kExternalModsInventorySaveSectionName, kExternalModsInventorySaveVersion, SaveExternalModsInventorySection,
            true, SECTION_PARENT_NONE);
        mSaveSectionRegistered = true;
    }

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
        if (lhs.loadPriority != rhs.loadPriority) {
            return lhs.loadPriority > rhs.loadPriority;
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

        for (const auto& hotkey : package.runtime.cameraHotkeys) {
            if (hotkey.defaultKeyboardScancodes.empty()) {
                continue;
            }
            const auto cvarName = BuildCameraHotkeyScancodeCVarName(package.manifest.id, hotkey.id);
            const int32_t defaultScancode = hotkey.defaultKeyboardScancodes.front();
            int32_t effectiveScancode = CVarGetInteger(cvarName.c_str(), defaultScancode);
            if (effectiveScancode <= static_cast<int32_t>(Ship::LUS_KB_UNKNOWN)) {
                effectiveScancode = defaultScancode;
            }
            CVarSetInteger(cvarName.c_str(), effectiveScancode);
        }

        const auto enabledCVarName = BuildEnabledCVarName(package.manifest.id);
        int32_t enabledValue = CVarGetInteger(enabledCVarName.c_str(), 1);
        enabledValue = enabledValue != 0 ? 1 : 0;
        CVarSetInteger(enabledCVarName.c_str(), enabledValue);

        package.runtime.enabled = enabledValue != 0;
        if (package.runtime.enabled) {
            ApplyDefaultKeyboardMappingsForPackage(package);
            runtimeCount++;
            SPDLOG_INFO("[ExternalMods] Runtime enabled: {} ({})", package.manifest.name, package.manifest.id);
        } else {
            SPDLOG_INFO("[ExternalMods] Runtime disabled by user: {} ({})", package.manifest.name, package.manifest.id);
        }
    }

    PruneAimCameraStateForUnavailableProfiles();

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

    const bool hasLoadOrder = json.contains("loadOrder");
    const bool hasLoadPriority = json.contains("loadPriority");
    if (hasLoadOrder) {
        if (!json["loadOrder"].is_number_integer()) {
            outError = "Invalid field: loadOrder must be integer";
            return false;
        }
        outManifest.loadOrder = json["loadOrder"].get<int32_t>();
    }
    if (hasLoadPriority) {
        if (!json["loadPriority"].is_number_integer()) {
            outError = "Invalid field: loadPriority must be integer";
            return false;
        }
        outManifest.loadPriority = json["loadPriority"].get<int32_t>();
    } else if (hasLoadOrder) {
        // Backward compatibility: old loadOrder sorted ascending. New loadPriority is descending.
        outManifest.loadPriority = -outManifest.loadOrder;
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

    if (outManifest.apiVersion >= kExternalModApiVersionV3) {
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
        outManifest.runtimeMaxActiveStatuses = kDefaultRuntimeActiveStatuses;

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
        if (runtime.contains("maxActiveStatuses")) {
            if (!runtime["maxActiveStatuses"].is_number_integer()) {
                outError = "runtime.maxActiveStatuses must be integer";
                return false;
            }
            outManifest.runtimeMaxActiveStatuses = runtime["maxActiveStatuses"].get<int32_t>();
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
        if (outManifest.runtimeMaxActiveStatuses < 1 || outManifest.runtimeMaxActiveStatuses > 4096) {
            outError = "runtime.maxActiveStatuses must be in [1, 4096]";
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
        const bool hasStatusCatalogCapability = ManifestHasCapability(outManifest, "statuses.catalog.v1");
        const bool hasDamageCatalogCapability = ManifestHasCapability(outManifest, "combat.damage.v1");
        const bool hasTargetingCatalogCapability = ManifestHasCapability(outManifest, "combat.targeting.v1");
        const bool hasProjectileCatalogCapability = ManifestHasCapability(outManifest, "combat.projectiles.v1");
        const bool hasAoECatalogCapability = ManifestHasCapability(outManifest, "combat.aoe.v1");
        const bool hasMovementCatalogCapability = ManifestHasCapability(outManifest, "movement.profiles.v1");
        const bool hasAimCameraCatalogCapability = ManifestHasCapability(outManifest, "camera.aim_profiles.v1");
        const bool hasUseProfilesCapability = ManifestHasCapability(outManifest, "items.use_profiles.v1");
        const bool hasVanillaPatchesCapability = ManifestHasCapability(outManifest, "patches.vanilla_items.v1");

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

        auto parseCapabilityPath = [&](const char* fieldName, const char* capability, bool hasCapability,
                                       std::string& outPathValue) -> bool {
            if (json.contains(fieldName)) {
                if (!hasCapability) {
                    outError = std::string(fieldName) + " requires capability " + capability;
                    return false;
                }
                if (!ValidateRequiredString(json, fieldName, outPathValue, outError)) {
                    outError = std::string("Missing or invalid field: ") + fieldName;
                    return false;
                }
                std::filesystem::path normalizedPath;
                if (!IsSafePackageRelativePath(outPathValue, normalizedPath, outError)) {
                    outError = std::string("Invalid ") + fieldName + ": " + outError;
                    return false;
                }
                outPathValue = normalizedPath.generic_string();
                return true;
            }

            if (hasCapability) {
                outError = std::string("Missing required field for ") + capability + ": " + fieldName;
                return false;
            }
            return true;
        };

        if (!parseCapabilityPath("statusDefinitions", "statuses.catalog.v1", hasStatusCatalogCapability,
                                 outManifest.statusDefinitions) ||
            !parseCapabilityPath("damageDefinitions", "combat.damage.v1", hasDamageCatalogCapability,
                                 outManifest.damageDefinitions) ||
            !parseCapabilityPath("targetingDefinitions", "combat.targeting.v1", hasTargetingCatalogCapability,
                                 outManifest.targetingDefinitions) ||
            !parseCapabilityPath("projectileDefinitions", "combat.projectiles.v1", hasProjectileCatalogCapability,
                                 outManifest.projectileDefinitions) ||
            !parseCapabilityPath("aoeDefinitions", "combat.aoe.v1", hasAoECatalogCapability,
                                 outManifest.aoeDefinitions) ||
            !parseCapabilityPath("movementDefinitions", "movement.profiles.v1", hasMovementCatalogCapability,
                                 outManifest.movementDefinitions) ||
            !parseCapabilityPath("cameraDefinitions", "camera.aim_profiles.v1", hasAimCameraCatalogCapability,
                                 outManifest.cameraDefinitions) ||
            !parseCapabilityPath("itemUseProfiles", "items.use_profiles.v1", hasUseProfilesCapability,
                                 outManifest.itemUseProfiles) ||
            !parseCapabilityPath("vanillaItemPatches", "patches.vanilla_items.v1", hasVanillaPatchesCapability,
                                 outManifest.vanillaItemPatches)) {
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
        if (apiVersion < kExternalModApiVersionV3) {
            outError = "onInput requires apiVersion 3";
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

    const std::unordered_set<std::string> kAllowedParams = { "range", "speed", "cooldown", "pullForce", "infiniteAmmo" };
    const std::unordered_set<std::string> kRemovedLegacyParams = { "freezeOnMeleeHit", "freezeOnHitDuration",
                                                                    "freezeOnHitShake", "freezeOnHitIntensity" };

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

        if (item.contains("placement")) {
            if (!ParseItemPlacement(item["placement"], definition.placement, outError)) {
                outError = "items[" + std::to_string(i) + "].placement " + outError;
                return false;
            }
        }

        definition.hasSlot = item.contains("slot");
        if (definition.placement == ExternalModItemPlacement::Legacy && !definition.hasSlot) {
            outError = "items[" + std::to_string(i) + "].slot is required for placement=legacy";
            return false;
        }

        if (definition.hasSlot) {
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
        }

        definition.agePolicy = ExternalModItemAgePolicy::AllowChild;
        if (definition.placement == ExternalModItemPlacement::Virtual) {
            definition.useMode = ExternalModItemUseMode::Override;
        } else {
            definition.useMode = definition.hasSlot && definition.slot == ExternalModItemSlot::Hookshot
                                     ? ExternalModItemUseMode::Override
                                     : ExternalModItemUseMode::Vanilla;
        }

        if (item.contains("assignableButtons")) {
            const auto& assignableButtons = item["assignableButtons"];
            if (!assignableButtons.is_array()) {
                outError = "items[" + std::to_string(i) + "].assignableButtons must be array";
                return false;
            }

            uint8_t parsedMask = 0;
            for (size_t buttonIndex = 0; buttonIndex < assignableButtons.size(); ++buttonIndex) {
                if (!assignableButtons[buttonIndex].is_string()) {
                    outError = "items[" + std::to_string(i) + "].assignableButtons[" +
                               std::to_string(buttonIndex) + "] must be string";
                    return false;
                }

                int32_t resolvedButton = 0;
                const auto buttonName = assignableButtons[buttonIndex].get<std::string>();
                if (!ParseAssignableButtonName(buttonName, resolvedButton)) {
                    outError = "items[" + std::to_string(i) + "].assignableButtons[" +
                               std::to_string(buttonIndex) + "] unsupported: " + buttonName +
                               ". Supported buttons: " + BuildAssignableButtonsList();
                    return false;
                }

                parsedMask = static_cast<uint8_t>(parsedMask | MakeButtonMask(resolvedButton));
            }

            if (parsedMask == 0) {
                outError = "items[" + std::to_string(i) + "].assignableButtons cannot be empty";
                return false;
            }

            definition.assignableButtonsMask = parsedMask;
        }

        if (item.contains("iconAsset")) {
            if (!item["iconAsset"].is_string()) {
                outError = "items[" + std::to_string(i) + "].iconAsset must be string";
                return false;
            }
            definition.iconAsset = item["iconAsset"].get<std::string>();
        }

        if (item.contains("aimReticleTextureAsset")) {
            if (!item["aimReticleTextureAsset"].is_string()) {
                outError = "items[" + std::to_string(i) + "].aimReticleTextureAsset must be string";
                return false;
            }
            definition.aimReticleTextureAsset = item["aimReticleTextureAsset"].get<std::string>();
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

        if (HasHookshotTextureAssetOverrides(definition) &&
            (!definition.hasSlot || definition.slot != ExternalModItemSlot::Hookshot)) {
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

        if (item.contains("useTrigger")) {
            if (!ParseItemUseTrigger(item["useTrigger"], definition.useTrigger, outError)) {
                outError = "items[" + std::to_string(i) + "].useTrigger " + outError;
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
        if (item.contains("useProfile")) {
            if (!item["useProfile"].is_string()) {
                outError = "items[" + std::to_string(i) + "].useProfile must be string";
                return false;
            }
            definition.useProfile = item["useProfile"].get<std::string>();
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

        if (definition.hasSlot && definition.slot == ExternalModItemSlot::Hookshot &&
            definition.useMode != ExternalModItemUseMode::Vanilla && definition.onUseExport.empty() &&
            definition.useProfile.empty()) {
            outError = "items[" + std::to_string(i) + "].behavior.exportOnUse is required for SLOT_HOOKSHOT when useMode is override/augment";
            return false;
        }

        if ((!definition.hasSlot || definition.slot != ExternalModItemSlot::Hookshot) &&
            definition.useMode == ExternalModItemUseMode::Override && definition.onUseExport.empty() &&
            definition.useProfile.empty()) {
            outError = "items[" + std::to_string(i) + "].behavior.exportOnUse is required for non-hookshot override useMode";
            return false;
        }

        if (definition.useTrigger == ExternalModItemUseTrigger::HammerGroundImpact) {
            if (!definition.hasSlot || definition.slot != ExternalModItemSlot::Hammer) {
                outError = "items[" + std::to_string(i) + "].useTrigger hammerGroundImpact requires slot SLOT_HAMMER";
                return false;
            }
            if (definition.useProfile.empty()) {
                outError = "items[" + std::to_string(i) + "].useTrigger hammerGroundImpact requires useProfile";
                return false;
            }
        }

        if (item.contains("params")) {
            if (!item["params"].is_object()) {
                outError = "items[" + std::to_string(i) + "].params must be object";
                return false;
            }
            for (const auto& [key, value] : item["params"].items()) {
                if (kRemovedLegacyParams.contains(key)) {
                    outError = "items[" + std::to_string(i) + "].params." + key +
                               " was removed in apiVersion 3; migrate to items/use_profiles.json + applyStatus(core:freeze)";
                    return false;
                }
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

    return true;
}

bool ExternalModManager::TryParseInputDefinitions(const std::string& content,
                                                  std::vector<ExternalModInputBinding>& outBindings,
                                                  std::vector<ExternalModCameraHotkeyDefinition>& outCameraHotkeys,
                                                  std::string& outError) {
    outBindings.clear();
    outCameraHotkeys.clear();

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

    auto parseDefaultKeyboardKeys = [&](const nlohmann::json& item, const std::string& basePath,
                                        std::vector<int32_t>& outScancodes) -> bool {
        outScancodes.clear();
        if (!item.contains("defaultKeyboardKeys")) {
            return true;
        }
        if (!item["defaultKeyboardKeys"].is_array()) {
            outError = basePath + ".defaultKeyboardKeys must be array";
            return false;
        }

        for (size_t keyIndex = 0; keyIndex < item["defaultKeyboardKeys"].size(); ++keyIndex) {
            const auto& keyValue = item["defaultKeyboardKeys"][keyIndex];
            if (!keyValue.is_string()) {
                outError = basePath + ".defaultKeyboardKeys[" + std::to_string(keyIndex) + "] must be string";
                return false;
            }

            int32_t keyScancode = 0;
            std::string keyError;
            if (!ParseKeyboardKeyToken(keyValue.get<std::string>(), keyScancode, keyError)) {
                outError = basePath + ".defaultKeyboardKeys[" + std::to_string(keyIndex) + "]: " + keyError;
                return false;
            }
            outScancodes.push_back(keyScancode);
        }

        std::sort(outScancodes.begin(), outScancodes.end());
        outScancodes.erase(std::unique(outScancodes.begin(), outScancodes.end()), outScancodes.end());
        return true;
    };

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

        if (!parseDefaultKeyboardKeys(item, "bindings[" + std::to_string(i) + "]", binding.defaultKeyboardScancodes)) {
            return false;
        }

        outBindings.push_back(std::move(binding));
    }

    if (json.is_object() && json.contains("cameraHotkeys")) {
        if (!json["cameraHotkeys"].is_array()) {
            outError = "cameraHotkeys must be array";
            return false;
        }

        std::unordered_set<std::string> seenHotkeyIds;
        for (size_t i = 0; i < json["cameraHotkeys"].size(); ++i) {
            const auto& hotkey = json["cameraHotkeys"][i];
            if (!hotkey.is_object()) {
                outError = "cameraHotkeys[" + std::to_string(i) + "] must be object";
                return false;
            }

            ExternalModCameraHotkeyDefinition definition;
            if (!ValidateRequiredString(hotkey, "id", definition.id, outError)) {
                outError = "cameraHotkeys[" + std::to_string(i) + "]: " + outError;
                return false;
            }
            if (!seenHotkeyIds.insert(definition.id).second) {
                outError = "Duplicate cameraHotkeys id: " + definition.id;
                return false;
            }

            if (hotkey.contains("allowUserRemap")) {
                if (!hotkey["allowUserRemap"].is_boolean()) {
                    outError = "cameraHotkeys[" + std::to_string(i) + "].allowUserRemap must be boolean";
                    return false;
                }
                definition.allowUserRemap = hotkey["allowUserRemap"].get<bool>();
            }

            if (!parseDefaultKeyboardKeys(hotkey, "cameraHotkeys[" + std::to_string(i) + "]",
                                          definition.defaultKeyboardScancodes)) {
                return false;
            }
            if (definition.defaultKeyboardScancodes.empty()) {
                outError = "cameraHotkeys[" + std::to_string(i) + "].defaultKeyboardKeys must contain at least one key";
                return false;
            }

            if (!hotkey.contains("action")) {
                outError = "cameraHotkeys[" + std::to_string(i) + "] requires action";
                return false;
            }

            std::string actionError;
            if (!ParseAction(hotkey, kExternalModApiVersionV3, definition.action, actionError)) {
                outError = "cameraHotkeys[" + std::to_string(i) + "]: " + actionError;
                return false;
            }
            if (!kSupportedCameraHotkeyActions.contains(definition.action.type)) {
                outError = "cameraHotkeys[" + std::to_string(i) +
                           "].action must be toggleAimCameraMode|setAimCameraMode|setAimCameraProfile";
                return false;
            }

            outCameraHotkeys.push_back(std::move(definition));
        }
    }

    return true;
}

bool ExternalModManager::TryParseHookDefinitions(const std::string& content, int32_t apiVersion,
                                                 std::vector<ExternalModHookSubscription>& outSubscriptions,
                                                 std::string& outError) {
    outSubscriptions.clear();

    if (apiVersion < kExternalModApiVersionV3) {
        outError = "hookDefinitions requires apiVersion 3";
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

    if (apiVersion < kExternalModApiVersionV3) {
        outError = "actorDefinitions requires apiVersion 3";
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

    if (apiVersion < kExternalModApiVersionV3) {
        outError = "behaviorDefinitions requires apiVersion 3";
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

    if (apiVersion < kExternalModApiVersionV3) {
        outError = "sceneDefinitions requires apiVersion 3";
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

        if (scene.contains("sceneResource")) {
            if (!ValidateRequiredString(scene, "sceneResource", definition.sceneResourcePath, outError)) {
                outError = "scenes[" + std::to_string(i) + "].sceneResource: " + outError;
                return false;
            }

            std::filesystem::path normalizedSceneResource;
            if (!IsSafePackageRelativePath(definition.sceneResourcePath, normalizedSceneResource, outError)) {
                outError = "scenes[" + std::to_string(i) + "].sceneResource: " + outError;
                return false;
            }
            definition.sceneResourcePath = normalizedSceneResource.generic_string();
            definition.useNamespacedScene = true;

            if (!scene.contains("hostEntrance")) {
                outError = "scenes[" + std::to_string(i) + "].hostEntrance is required when sceneResource is set";
                return false;
            }

            if (!ParseAliasedInt16(scene["hostEntrance"], kEntranceAliases, "hostEntrance", definition.hostEntranceIndex,
                                   outError)) {
                outError = "scenes[" + std::to_string(i) + "].hostEntrance: " + outError;
                return false;
            }
            definition.hasHostEntrance = true;
            definition.hasEntrance = true;
            definition.entranceIndex = definition.hostEntranceIndex;

            if (scene.contains("fallbackEntrance")) {
                if (!ParseAliasedInt16(scene["fallbackEntrance"], kEntranceAliases, "fallbackEntrance",
                                       definition.fallbackEntranceIndex, outError)) {
                    outError = "scenes[" + std::to_string(i) + "].fallbackEntrance: " + outError;
                    return false;
                }
            } else {
                definition.fallbackEntranceIndex = definition.hostEntranceIndex;
            }
            definition.hasFallbackEntrance = true;

            if (scene.contains("fallbackPlayable")) {
                if (!scene["fallbackPlayable"].is_boolean()) {
                    outError = "scenes[" + std::to_string(i) + "].fallbackPlayable must be boolean";
                    return false;
                }
                definition.fallbackPlayable = scene["fallbackPlayable"].get<bool>();
            }
        } else {
            const char* entranceKey =
                scene.contains("entrance") ? "entrance" : (scene.contains("fallbackEntrance") ? "fallbackEntrance" : nullptr);
            if (entranceKey != nullptr) {
                if (!ParseAliasedInt16(scene[entranceKey], kEntranceAliases, entranceKey, definition.entranceIndex, outError)) {
                    outError = "scenes[" + std::to_string(i) + "]." + entranceKey + ": " + outError;
                    return false;
                }
                definition.hasEntrance = true;
            }
        }

        outDefinitions.push_back(std::move(definition));
    }

    return true;
}

bool TryParseUseProfileEffectObject(const nlohmann::json& effect, const std::string& contextPath,
                                    ExternalModUseProfileEffect& outEffect, std::string& outError) {
    if (!effect.is_object()) {
        outError = contextPath + " must be object";
        return false;
    }
    if (!ValidateRequiredString(effect, "action", outEffect.action, outError)) {
        outError = contextPath + ".action: " + outError;
        return false;
    }

    const auto parseOptionalString = [&](const char* key, std::string& output) -> bool {
        if (!effect.contains(key)) {
            return true;
        }
        if (!effect[key].is_string()) {
            outError = contextPath + "." + key + " must be string";
            return false;
        }
        output = effect[key].get<std::string>();
        return true;
    };
    const auto parseOptionalInt = [&](const char* key, int32_t& output) -> bool {
        if (!effect.contains(key)) {
            return true;
        }
        if (!effect[key].is_number_integer()) {
            outError = contextPath + "." + key + " must be integer";
            return false;
        }
        output = effect[key].get<int32_t>();
        return true;
    };
    const auto parseOptionalBool = [&](const char* key, bool& output) -> bool {
        if (!effect.contains(key)) {
            return true;
        }
        if (!effect[key].is_boolean()) {
            outError = contextPath + "." + key + " must be boolean";
            return false;
        }
        output = effect[key].get<bool>();
        return true;
    };
    const auto parseOptionalColor = [&](const char* key, std::array<uint8_t, 4>& output) -> bool {
        if (!effect.contains(key)) {
            return true;
        }
        if (!effect[key].is_array() || effect[key].size() != 4) {
            outError = contextPath + "." + key + " must be array[4]";
            return false;
        }
        for (size_t channel = 0; channel < 4; ++channel) {
            const auto& value = effect[key][channel];
            if (!value.is_number_integer()) {
                outError = contextPath + "." + key + "[" + std::to_string(channel) + "] must be integer";
                return false;
            }
            output[channel] = static_cast<uint8_t>(std::clamp(value.get<int32_t>(), 0, 255));
        }
        return true;
    };

    if (!parseOptionalString("damageProfileId", outEffect.damageProfileId) ||
        !parseOptionalString("profile", outEffect.damageProfileId) ||
        !parseOptionalString("status", outEffect.statusId) ||
        !parseOptionalString("projectile", outEffect.projectileProfileId) ||
        !parseOptionalString("projectileProfileId", outEffect.projectileProfileId) ||
        !parseOptionalString("aoe", outEffect.aoeProfileId) ||
        !parseOptionalString("aoeProfileId", outEffect.aoeProfileId) ||
        !parseOptionalString("movement", outEffect.movementProfileId) ||
        !parseOptionalString("movementProfileId", outEffect.movementProfileId) ||
        !parseOptionalInt("durationFrames", outEffect.durationFrames) ||
        !parseOptionalInt("tickFrames", outEffect.tickFrames) ||
        !parseOptionalInt("damagePerTick", outEffect.damagePerTick) ||
        !parseOptionalInt("intensity", outEffect.intensity)) {
        return false;
    }

    const auto actionToken = ToLower(outEffect.action);
    if (actionToken == "spawnshockwave") {
        if (!parseOptionalString("origin", outEffect.shockwaveOrigin) ||
            !parseOptionalColor("primColor", outEffect.shockwavePrimColor) ||
            !parseOptionalColor("envColor", outEffect.shockwaveEnvColor) ||
            !parseOptionalInt("life", outEffect.shockwaveLife) ||
            !parseOptionalBool("spawnIceSmoke", outEffect.shockwaveSpawnIceSmoke)) {
            return false;
        }

        const auto originToken = ToLower(outEffect.shockwaveOrigin);
        if (originToken != "player" && originToken != "impact") {
            outError = contextPath + ".origin must be player|impact";
            return false;
        }

        outEffect.shockwaveOrigin = originToken;
        outEffect.shockwaveLife = std::clamp(outEffect.shockwaveLife, 1, 120);
    }

    return true;
}

bool ExternalModManager::TryParseStatusDefinitions(const std::string& content, int32_t apiVersion,
                                                   std::vector<ExternalModStatusDefinition>& outDefinitions,
                                                   std::string& outError) {
    outDefinitions.clear();
    if (apiVersion < kExternalModApiVersionV3) {
        outError = "statusDefinitions requires apiVersion 3";
        return false;
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("statuses.json parse error: ") + ex.what();
        return false;
    }

    if (!json.is_object() || !json.contains("statuses") || !json["statuses"].is_array()) {
        outError = "statuses.json must be object with statuses[]";
        return false;
    }
    if (!json.contains("schemaVersion") || !json["schemaVersion"].is_number_integer() ||
        json["schemaVersion"].get<int32_t>() != 1) {
        outError = "statuses.json schemaVersion must be 1";
        return false;
    }

    std::unordered_set<std::string> seenIds;
    const auto& statuses = json["statuses"];
    for (size_t i = 0; i < statuses.size(); ++i) {
        const auto& status = statuses[i];
        if (!status.is_object()) {
            outError = "statuses[" + std::to_string(i) + "] must be object";
            return false;
        }

        ExternalModStatusDefinition definition;
        if (!ValidateRequiredString(status, "id", definition.id, outError)) {
            outError = "statuses[" + std::to_string(i) + "].id: " + outError;
            return false;
        }
        if (!IsNamespacedCatalogId(definition.id)) {
            outError = "statuses[" + std::to_string(i) + "].id must be namespaced";
            return false;
        }
        if (!seenIds.insert(definition.id).second) {
            outError = "Duplicate status id: " + definition.id;
            return false;
        }

        if (status.contains("displayName")) {
            if (!status["displayName"].is_string()) {
                outError = "statuses[" + std::to_string(i) + "].displayName must be string";
                return false;
            }
            definition.displayName = status["displayName"].get<std::string>();
        }
        if (status.contains("baseStatus")) {
            if (!status["baseStatus"].is_string()) {
                outError = "statuses[" + std::to_string(i) + "].baseStatus must be string";
                return false;
            }
            definition.baseStatusId = status["baseStatus"].get<std::string>();
            ExternalModStatusType baseType = ExternalModStatusType::Custom;
            if (!ParseStatusIdToken(definition.baseStatusId, baseType)) {
                outError = "statuses[" + std::to_string(i) + "].baseStatus unsupported: " + definition.baseStatusId;
                return false;
            }
            definition.baseStatusType = baseType;
        } else {
            ExternalModStatusType parsedType = ExternalModStatusType::Custom;
            if (ParseStatusIdToken(definition.id, parsedType)) {
                definition.baseStatusType = parsedType;
            }
        }
        if (status.contains("durationFrames")) {
            if (!status["durationFrames"].is_number_integer()) {
                outError = "statuses[" + std::to_string(i) + "].durationFrames must be integer";
                return false;
            }
            definition.durationFrames = std::clamp(status["durationFrames"].get<int32_t>(), 1, 36000);
        }
        if (status.contains("tickFrames")) {
            if (!status["tickFrames"].is_number_integer()) {
                outError = "statuses[" + std::to_string(i) + "].tickFrames must be integer";
                return false;
            }
            definition.tickFrames = std::clamp(status["tickFrames"].get<int32_t>(), 1, 36000);
        }
        if (status.contains("damagePerTick")) {
            if (!status["damagePerTick"].is_number_integer()) {
                outError = "statuses[" + std::to_string(i) + "].damagePerTick must be integer";
                return false;
            }
            definition.damagePerTick = std::clamp(status["damagePerTick"].get<int32_t>(), 0, 255);
        }
        if (status.contains("intensity")) {
            if (!status["intensity"].is_number_integer()) {
                outError = "statuses[" + std::to_string(i) + "].intensity must be integer";
                return false;
            }
            definition.intensity = std::clamp(status["intensity"].get<int32_t>(), 0, 255);
        }
        if (status.contains("shakeFrames")) {
            if (!status["shakeFrames"].is_number_integer()) {
                outError = "statuses[" + std::to_string(i) + "].shakeFrames must be integer";
                return false;
            }
            definition.shakeFrames = std::clamp(status["shakeFrames"].get<int32_t>(), 0, 36000);
        }
        if (status.contains("stacking")) {
            if (!status["stacking"].is_object()) {
                outError = "statuses[" + std::to_string(i) + "].stacking must be object";
                return false;
            }
            const auto& stacking = status["stacking"];
            if (stacking.contains("mode")) {
                if (!stacking["mode"].is_string()) {
                    outError = "statuses[" + std::to_string(i) + "].stacking.mode must be string";
                    return false;
                }
                definition.stackingMode = ToLower(stacking["mode"].get<std::string>());
            }
            if (stacking.contains("maxStacks")) {
                if (!stacking["maxStacks"].is_number_integer()) {
                    outError = "statuses[" + std::to_string(i) + "].stacking.maxStacks must be integer";
                    return false;
                }
                definition.maxStacks = std::clamp(stacking["maxStacks"].get<int32_t>(), 1, 32);
            }
        }
        if (status.contains("effects")) {
            if (!status["effects"].is_object()) {
                outError = "statuses[" + std::to_string(i) + "].effects must be object";
                return false;
            }
            const auto& effects = status["effects"];
            if (effects.contains("onApply") &&
                !ParseActionArray(effects["onApply"], apiVersion, "onApply", definition.onApply, outError)) {
                outError = "statuses[" + std::to_string(i) + "].effects.onApply: " + outError;
                return false;
            }
            if (effects.contains("onTick") &&
                !ParseActionArray(effects["onTick"], apiVersion, "onTick", definition.onTick, outError)) {
                outError = "statuses[" + std::to_string(i) + "].effects.onTick: " + outError;
                return false;
            }
            if (effects.contains("onExpire") &&
                !ParseActionArray(effects["onExpire"], apiVersion, "onExpire", definition.onExpire, outError)) {
                outError = "statuses[" + std::to_string(i) + "].effects.onExpire: " + outError;
                return false;
            }
        }

        outDefinitions.push_back(std::move(definition));
    }

    return true;
}

bool ExternalModManager::TryParseDamageDefinitions(const std::string& content, int32_t apiVersion,
                                                   std::vector<ExternalModDamageProfile>& outDefinitions,
                                                   std::string& outError) {
    outDefinitions.clear();
    if (apiVersion < kExternalModApiVersionV3) {
        outError = "damageDefinitions requires apiVersion 3";
        return false;
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("damage_profiles.json parse error: ") + ex.what();
        return false;
    }

    if (!json.is_object() || !json.contains("profiles") || !json["profiles"].is_array()) {
        outError = "damage_profiles.json must be object with profiles[]";
        return false;
    }
    if (!json.contains("schemaVersion") || !json["schemaVersion"].is_number_integer() ||
        json["schemaVersion"].get<int32_t>() != 1) {
        outError = "damage_profiles.json schemaVersion must be 1";
        return false;
    }

    std::unordered_set<std::string> seenIds;
    const auto& profiles = json["profiles"];
    for (size_t i = 0; i < profiles.size(); ++i) {
        const auto& profile = profiles[i];
        if (!profile.is_object()) {
            outError = "profiles[" + std::to_string(i) + "] must be object";
            return false;
        }

        ExternalModDamageProfile definition;
        if (!ValidateRequiredString(profile, "id", definition.id, outError)) {
            outError = "profiles[" + std::to_string(i) + "].id: " + outError;
            return false;
        }
        if (!IsNamespacedCatalogId(definition.id)) {
            outError = "profiles[" + std::to_string(i) + "].id must be namespaced";
            return false;
        }
        if (!seenIds.insert(definition.id).second) {
            outError = "Duplicate damage profile id: " + definition.id;
            return false;
        }

        if (profile.contains("amount")) {
            if (!profile["amount"].is_number_integer()) {
                outError = "profiles[" + std::to_string(i) + "].amount must be integer";
                return false;
            }
            definition.amount = std::clamp(profile["amount"].get<int32_t>(), 0, 255);
        }
        if (profile.contains("type")) {
            if (!profile["type"].is_string()) {
                outError = "profiles[" + std::to_string(i) + "].type must be string";
                return false;
            }
            definition.type = profile["type"].get<std::string>();
        }
        if (profile.contains("propInteraction")) {
            if (!profile["propInteraction"].is_string()) {
                outError = "profiles[" + std::to_string(i) + "].propInteraction must be string";
                return false;
            }
            definition.propInteraction = ToLower(profile["propInteraction"].get<std::string>());
            if (definition.propInteraction != "none" && definition.propInteraction != "vanilla_hit" &&
                definition.propInteraction != "hard_kill") {
                outError = "profiles[" + std::to_string(i) + "].propInteraction must be none|vanilla_hit|hard_kill";
                return false;
            }
        }
        if (profile.contains("iframes")) {
            if (!profile["iframes"].is_object()) {
                outError = "profiles[" + std::to_string(i) + "].iframes must be object";
                return false;
            }
            const auto& iframes = profile["iframes"];
            if (iframes.contains("frames")) {
                if (!iframes["frames"].is_number_integer()) {
                    outError = "profiles[" + std::to_string(i) + "].iframes.frames must be integer";
                    return false;
                }
                definition.iframesFrames = std::clamp(iframes["frames"].get<int32_t>(), 0, 1024);
            }
        }

        outDefinitions.push_back(std::move(definition));
    }

    return true;
}

bool ExternalModManager::TryParseTargetingDefinitions(const std::string& content, int32_t apiVersion,
                                                      std::vector<ExternalModTargetingProfile>& outDefinitions,
                                                      std::string& outError) {
    outDefinitions.clear();
    if (apiVersion < kExternalModApiVersionV3) {
        outError = "targetingDefinitions requires apiVersion 3";
        return false;
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("targeting_profiles.json parse error: ") + ex.what();
        return false;
    }

    if (!json.is_object() || !json.contains("profiles") || !json["profiles"].is_array()) {
        outError = "targeting_profiles.json must be object with profiles[]";
        return false;
    }
    if (!json.contains("schemaVersion") || !json["schemaVersion"].is_number_integer() ||
        json["schemaVersion"].get<int32_t>() != 1) {
        outError = "targeting_profiles.json schemaVersion must be 1";
        return false;
    }

    std::unordered_set<std::string> seenIds;
    const auto& profiles = json["profiles"];
    for (size_t i = 0; i < profiles.size(); ++i) {
        const auto& profile = profiles[i];
        if (!profile.is_object()) {
            outError = "profiles[" + std::to_string(i) + "] must be object";
            return false;
        }

        ExternalModTargetingProfile definition;
        if (!ValidateRequiredString(profile, "id", definition.id, outError)) {
            outError = "profiles[" + std::to_string(i) + "].id: " + outError;
            return false;
        }
        if (!IsNamespacedCatalogId(definition.id)) {
            outError = "profiles[" + std::to_string(i) + "].id must be namespaced";
            return false;
        }
        if (!seenIds.insert(definition.id).second) {
            outError = "Duplicate targeting profile id: " + definition.id;
            return false;
        }
        std::string modeToken;
        if (!ValidateRequiredString(profile, "mode", modeToken, outError)) {
            outError = "profiles[" + std::to_string(i) + "].mode: " + outError;
            return false;
        }
        if (!ParseTargetingModeToken(modeToken, definition.mode)) {
            outError = "profiles[" + std::to_string(i) + "].mode unsupported: " + modeToken;
            return false;
        }
        if (profile.contains("range")) {
            if (!profile["range"].is_number()) {
                outError = "profiles[" + std::to_string(i) + "].range must be numeric";
                return false;
            }
            definition.range = std::max(1.0f, profile["range"].get<float>());
        }
        if (profile.contains("radius")) {
            if (!profile["radius"].is_number()) {
                outError = "profiles[" + std::to_string(i) + "].radius must be numeric";
                return false;
            }
            definition.radius = std::max(0.0f, profile["radius"].get<float>());
        }
        if (profile.contains("angle")) {
            if (!profile["angle"].is_number()) {
                outError = "profiles[" + std::to_string(i) + "].angle must be numeric";
                return false;
            }
            definition.angle = std::clamp(profile["angle"].get<float>(), 0.0f, 360.0f);
        }
        if (profile.contains("filters")) {
            if (!profile["filters"].is_object()) {
                outError = "profiles[" + std::to_string(i) + "].filters must be object";
                return false;
            }
            const auto& filters = profile["filters"];
            if (filters.contains("stopOnWall")) {
                if (!filters["stopOnWall"].is_boolean()) {
                    outError = "profiles[" + std::to_string(i) + "].filters.stopOnWall must be boolean";
                    return false;
                }
                definition.stopOnWall = filters["stopOnWall"].get<bool>();
            }
            if (filters.contains("includeProps")) {
                if (!filters["includeProps"].is_boolean()) {
                    outError = "profiles[" + std::to_string(i) + "].filters.includeProps must be boolean";
                    return false;
                }
                definition.includeProps = filters["includeProps"].get<bool>();
            }
            if (filters.contains("includeEnemies")) {
                if (!filters["includeEnemies"].is_boolean()) {
                    outError = "profiles[" + std::to_string(i) + "].filters.includeEnemies must be boolean";
                    return false;
                }
                definition.includeEnemies = filters["includeEnemies"].get<bool>();
            }
        }

        outDefinitions.push_back(std::move(definition));
    }

    return true;
}

bool ExternalModManager::TryParseItemUseProfiles(const std::string& content, int32_t apiVersion,
                                                 std::vector<ExternalModItemUseProfile>& outDefinitions,
                                                 std::string& outError) {
    outDefinitions.clear();
    if (apiVersion < kExternalModApiVersionV3) {
        outError = "itemUseProfiles requires apiVersion 3";
        return false;
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("use_profiles.json parse error: ") + ex.what();
        return false;
    }

    if (!json.is_object() || !json.contains("useProfiles") || !json["useProfiles"].is_array()) {
        outError = "use_profiles.json must be object with useProfiles[]";
        return false;
    }
    if (!json.contains("schemaVersion") || !json["schemaVersion"].is_number_integer() ||
        json["schemaVersion"].get<int32_t>() != 1) {
        outError = "use_profiles.json schemaVersion must be 1";
        return false;
    }

    std::unordered_set<std::string> seenIds;
    const auto& useProfiles = json["useProfiles"];
    for (size_t i = 0; i < useProfiles.size(); ++i) {
        const auto& useProfile = useProfiles[i];
        if (!useProfile.is_object()) {
            outError = "useProfiles[" + std::to_string(i) + "] must be object";
            return false;
        }

        ExternalModItemUseProfile definition;
        if (!ValidateRequiredString(useProfile, "id", definition.id, outError)) {
            outError = "useProfiles[" + std::to_string(i) + "].id: " + outError;
            return false;
        }
        if (!IsNamespacedCatalogId(definition.id)) {
            outError = "useProfiles[" + std::to_string(i) + "].id must be namespaced";
            return false;
        }
        if (!seenIds.insert(definition.id).second) {
            outError = "Duplicate use profile id: " + definition.id;
            return false;
        }
        if (!ValidateRequiredString(useProfile, "targetingProfile", definition.targetingProfileId, outError)) {
            outError = "useProfiles[" + std::to_string(i) + "].targetingProfile: " + outError;
            return false;
        }
        if (useProfile.contains("cooldownFrames")) {
            if (!useProfile["cooldownFrames"].is_number_integer()) {
                outError = "useProfiles[" + std::to_string(i) + "].cooldownFrames must be integer";
                return false;
            }
            definition.cooldownFrames = std::clamp(useProfile["cooldownFrames"].get<int32_t>(), 0, 36000);
        }
        if (!useProfile.contains("effects") || !useProfile["effects"].is_array()) {
            outError = "useProfiles[" + std::to_string(i) + "].effects must be array";
            return false;
        }

        for (size_t effectIndex = 0; effectIndex < useProfile["effects"].size(); ++effectIndex) {
            ExternalModUseProfileEffect effectDefinition;
            const std::string effectPath =
                "useProfiles[" + std::to_string(i) + "].effects[" + std::to_string(effectIndex) + "]";
            if (!TryParseUseProfileEffectObject(useProfile["effects"][effectIndex], effectPath, effectDefinition,
                                                outError)) {
                return false;
            }
            definition.effects.push_back(std::move(effectDefinition));
        }

        outDefinitions.push_back(std::move(definition));
    }

    return true;
}

bool ExternalModManager::TryParseProjectileDefinitions(const std::string& content, int32_t apiVersion,
                                                       std::vector<ExternalModProjectileProfile>& outDefinitions,
                                                       std::string& outError) {
    outDefinitions.clear();
    if (apiVersion < kExternalModApiVersionV3) {
        outError = "projectileDefinitions requires apiVersion 3";
        return false;
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("projectiles.json parse error: ") + ex.what();
        return false;
    }

    if (!json.is_object() || !json.contains("projectiles") || !json["projectiles"].is_array()) {
        outError = "projectiles.json must be object with projectiles[]";
        return false;
    }
    if (!json.contains("schemaVersion") || !json["schemaVersion"].is_number_integer() ||
        json["schemaVersion"].get<int32_t>() != 1) {
        outError = "projectiles.json schemaVersion must be 1";
        return false;
    }

    std::unordered_set<std::string> seenIds;
    const auto& projectiles = json["projectiles"];
    for (size_t i = 0; i < projectiles.size(); ++i) {
        const auto& projectile = projectiles[i];
        if (!projectile.is_object()) {
            outError = "projectiles[" + std::to_string(i) + "] must be object";
            return false;
        }

        ExternalModProjectileProfile definition;
        if (!ValidateRequiredString(projectile, "id", definition.id, outError)) {
            outError = "projectiles[" + std::to_string(i) + "].id: " + outError;
            return false;
        }
        if (!IsNamespacedCatalogId(definition.id)) {
            outError = "projectiles[" + std::to_string(i) + "].id must be namespaced";
            return false;
        }
        if (!seenIds.insert(definition.id).second) {
            outError = "Duplicate projectile profile id: " + definition.id;
            return false;
        }

        if (projectile.contains("shape")) {
            if (!projectile["shape"].is_string()) {
                outError = "projectiles[" + std::to_string(i) + "].shape must be string";
                return false;
            }
            definition.shape = projectile["shape"].get<std::string>();
        }
        if (projectile.contains("speed")) {
            if (!projectile["speed"].is_number()) {
                outError = "projectiles[" + std::to_string(i) + "].speed must be numeric";
                return false;
            }
            definition.speed = projectile["speed"].get<float>();
        }
        if (projectile.contains("gravityScale")) {
            if (!projectile["gravityScale"].is_number()) {
                outError = "projectiles[" + std::to_string(i) + "].gravityScale must be numeric";
                return false;
            }
            definition.gravityScale = projectile["gravityScale"].get<float>();
        }
        if (projectile.contains("lifetimeFrames")) {
            if (!projectile["lifetimeFrames"].is_number_integer()) {
                outError = "projectiles[" + std::to_string(i) + "].lifetimeFrames must be integer";
                return false;
            }
            definition.lifetimeFrames = std::clamp(projectile["lifetimeFrames"].get<int32_t>(), 1, 36000);
        }
        if (projectile.contains("radius")) {
            if (!projectile["radius"].is_number()) {
                outError = "projectiles[" + std::to_string(i) + "].radius must be numeric";
                return false;
            }
            definition.radius = std::max(0.0f, projectile["radius"].get<float>());
        }
        if (projectile.contains("pierce")) {
            if (!projectile["pierce"].is_object()) {
                outError = "projectiles[" + std::to_string(i) + "].pierce must be object";
                return false;
            }
            const auto& pierce = projectile["pierce"];
            if (pierce.contains("maxHits")) {
                if (!pierce["maxHits"].is_number_integer()) {
                    outError = "projectiles[" + std::to_string(i) + "].pierce.maxHits must be integer";
                    return false;
                }
                definition.maxHits = std::clamp(pierce["maxHits"].get<int32_t>(), 1, 64);
            }
        }
        if (projectile.contains("collision")) {
            if (!projectile["collision"].is_object()) {
                outError = "projectiles[" + std::to_string(i) + "].collision must be object";
                return false;
            }
            const auto& collision = projectile["collision"];
            if (collision.contains("stopOnWall")) {
                if (!collision["stopOnWall"].is_boolean()) {
                    outError = "projectiles[" + std::to_string(i) + "].collision.stopOnWall must be boolean";
                    return false;
                }
                definition.stopOnWall = collision["stopOnWall"].get<bool>();
            }
        }
        if (projectile.contains("onHit")) {
            if (!projectile["onHit"].is_object()) {
                outError = "projectiles[" + std::to_string(i) + "].onHit must be object";
                return false;
            }
            const auto& onHit = projectile["onHit"];
            if (onHit.contains("damageProfile")) {
                if (!onHit["damageProfile"].is_string()) {
                    outError = "projectiles[" + std::to_string(i) + "].onHit.damageProfile must be string";
                    return false;
                }
                definition.damageProfileId = onHit["damageProfile"].get<std::string>();
            }
            if (onHit.contains("applyStatuses")) {
                if (!onHit["applyStatuses"].is_array()) {
                    outError = "projectiles[" + std::to_string(i) + "].onHit.applyStatuses must be array";
                    return false;
                }
                for (size_t statusIndex = 0; statusIndex < onHit["applyStatuses"].size(); ++statusIndex) {
                    if (!onHit["applyStatuses"][statusIndex].is_string()) {
                        outError = "projectiles[" + std::to_string(i) + "].onHit.applyStatuses[" +
                                   std::to_string(statusIndex) + "] must be string";
                        return false;
                    }
                    definition.applyStatuses.push_back(onHit["applyStatuses"][statusIndex].get<std::string>());
                }
            }
        }

        outDefinitions.push_back(std::move(definition));
    }

    return true;
}

bool ExternalModManager::TryParseAoEDefinitions(const std::string& content, int32_t apiVersion,
                                                std::vector<ExternalModAoEProfile>& outDefinitions,
                                                std::string& outError) {
    outDefinitions.clear();
    if (apiVersion < kExternalModApiVersionV3) {
        outError = "aoeDefinitions requires apiVersion 3";
        return false;
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("aoe_profiles.json parse error: ") + ex.what();
        return false;
    }

    if (!json.is_object() || !json.contains("aoe") || !json["aoe"].is_array()) {
        outError = "aoe_profiles.json must be object with aoe[]";
        return false;
    }
    if (!json.contains("schemaVersion") || !json["schemaVersion"].is_number_integer() ||
        json["schemaVersion"].get<int32_t>() != 1) {
        outError = "aoe_profiles.json schemaVersion must be 1";
        return false;
    }

    std::unordered_set<std::string> seenIds;
    const auto& aoeList = json["aoe"];
    for (size_t i = 0; i < aoeList.size(); ++i) {
        const auto& aoe = aoeList[i];
        if (!aoe.is_object()) {
            outError = "aoe[" + std::to_string(i) + "] must be object";
            return false;
        }

        ExternalModAoEProfile definition;
        if (!ValidateRequiredString(aoe, "id", definition.id, outError)) {
            outError = "aoe[" + std::to_string(i) + "].id: " + outError;
            return false;
        }
        if (!IsNamespacedCatalogId(definition.id)) {
            outError = "aoe[" + std::to_string(i) + "].id must be namespaced";
            return false;
        }
        if (!seenIds.insert(definition.id).second) {
            outError = "Duplicate aoe profile id: " + definition.id;
            return false;
        }

        if (aoe.contains("shape")) {
            if (!aoe["shape"].is_string()) {
                outError = "aoe[" + std::to_string(i) + "].shape must be string";
                return false;
            }
            definition.shape = aoe["shape"].get<std::string>();
        }
        if (aoe.contains("targetScope")) {
            if (!aoe["targetScope"].is_string()) {
                outError = "aoe[" + std::to_string(i) + "].targetScope must be string";
                return false;
            }

            if (!ParseAoETargetScopeToken(aoe["targetScope"].get<std::string>(), definition.targetScope)) {
                outError = "aoe[" + std::to_string(i) +
                           "].targetScope unsupported (expected all_non_player|enemies_bosses|enemies_bosses_props)";
                return false;
            }
        }
        if (aoe.contains("range")) {
            if (!aoe["range"].is_number()) {
                outError = "aoe[" + std::to_string(i) + "].range must be numeric";
                return false;
            }
            definition.range = std::max(0.0f, aoe["range"].get<float>());
        }
        if (aoe.contains("radius")) {
            if (!aoe["radius"].is_number()) {
                outError = "aoe[" + std::to_string(i) + "].radius must be numeric";
                return false;
            }
            definition.radius = std::max(0.0f, aoe["radius"].get<float>());
        }
        if (aoe.contains("angle")) {
            if (!aoe["angle"].is_number()) {
                outError = "aoe[" + std::to_string(i) + "].angle must be numeric";
                return false;
            }
            definition.angle = std::clamp(aoe["angle"].get<float>(), 0.0f, 360.0f);
        }
        if (aoe.contains("durationFrames")) {
            if (!aoe["durationFrames"].is_number_integer()) {
                outError = "aoe[" + std::to_string(i) + "].durationFrames must be integer";
                return false;
            }
            definition.durationFrames = std::clamp(aoe["durationFrames"].get<int32_t>(), 1, 36000);
        }
        if (aoe.contains("tickFrames")) {
            if (!aoe["tickFrames"].is_number_integer()) {
                outError = "aoe[" + std::to_string(i) + "].tickFrames must be integer";
                return false;
            }
            definition.tickFrames = std::clamp(aoe["tickFrames"].get<int32_t>(), 0, 36000);
        }
        if (aoe.contains("effects")) {
            if (!aoe["effects"].is_object()) {
                outError = "aoe[" + std::to_string(i) + "].effects must be object";
                return false;
            }
            const auto& effects = aoe["effects"];
            const auto parseEffectList = [&](const char* key, std::vector<ExternalModUseProfileEffect>& output) -> bool {
                if (!effects.contains(key)) {
                    return true;
                }
                if (!effects[key].is_array()) {
                    outError = "aoe[" + std::to_string(i) + "].effects." + key + " must be array";
                    return false;
                }
                for (size_t effectIndex = 0; effectIndex < effects[key].size(); ++effectIndex) {
                    ExternalModUseProfileEffect effectDefinition;
                    const std::string effectPath =
                        "aoe[" + std::to_string(i) + "].effects." + key + "[" + std::to_string(effectIndex) + "]";
                    if (!TryParseUseProfileEffectObject(effects[key][effectIndex], effectPath, effectDefinition, outError)) {
                        return false;
                    }
                    output.push_back(std::move(effectDefinition));
                }
                return true;
            };

            if (!parseEffectList("onEnter", definition.onEnter) || !parseEffectList("onTick", definition.onTick) ||
                !parseEffectList("onExit", definition.onExit)) {
                return false;
            }
        }

        outDefinitions.push_back(std::move(definition));
    }

    return true;
}

bool ExternalModManager::TryParseMovementDefinitions(const std::string& content, int32_t apiVersion,
                                                     std::vector<ExternalModMovementProfile>& outDefinitions,
                                                     std::string& outError) {
    outDefinitions.clear();
    if (apiVersion < kExternalModApiVersionV3) {
        outError = "movementDefinitions requires apiVersion 3";
        return false;
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("movement_profiles.json parse error: ") + ex.what();
        return false;
    }

    if (!json.is_object() || !json.contains("profiles") || !json["profiles"].is_array()) {
        outError = "movement_profiles.json must be object with profiles[]";
        return false;
    }
    if (!json.contains("schemaVersion") || !json["schemaVersion"].is_number_integer() ||
        json["schemaVersion"].get<int32_t>() != 1) {
        outError = "movement_profiles.json schemaVersion must be 1";
        return false;
    }

    std::unordered_set<std::string> seenIds;
    const auto& profiles = json["profiles"];
    for (size_t i = 0; i < profiles.size(); ++i) {
        const auto& profile = profiles[i];
        if (!profile.is_object()) {
            outError = "profiles[" + std::to_string(i) + "] must be object";
            return false;
        }

        ExternalModMovementProfile definition;
        if (!ValidateRequiredString(profile, "id", definition.id, outError)) {
            outError = "profiles[" + std::to_string(i) + "].id: " + outError;
            return false;
        }
        if (!IsNamespacedCatalogId(definition.id)) {
            outError = "profiles[" + std::to_string(i) + "].id must be namespaced";
            return false;
        }
        if (!seenIds.insert(definition.id).second) {
            outError = "Duplicate movement profile id: " + definition.id;
            return false;
        }
        if (!ValidateRequiredString(profile, "mode", definition.idlePose, outError)) {
            outError = "profiles[" + std::to_string(i) + "].mode: " + outError;
            return false;
        }
        if (!ParseMovementModeToken(definition.idlePose, definition.mode)) {
            outError = "profiles[" + std::to_string(i) + "].mode must be modifier|surf";
            return false;
        }
        // Reset idlePose to default after temporary parse use above.
        definition.idlePose = "stand";

        if (profile.contains("durationFrames")) {
            if (!profile["durationFrames"].is_number_integer()) {
                outError = "profiles[" + std::to_string(i) + "].durationFrames must be integer";
                return false;
            }
            definition.durationFrames = std::clamp(profile["durationFrames"].get<int32_t>(), 0, 36000);
        }
        if (definition.mode == ExternalModMovementMode::Modifier) {
            if (profile.contains("speedMultiplier")) {
                if (!profile["speedMultiplier"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].speedMultiplier must be numeric";
                    return false;
                }
                definition.speedMultiplier = std::clamp(profile["speedMultiplier"].get<float>(), 0.05f, 16.0f);
            }
            if (profile.contains("accelMultiplier")) {
                if (!profile["accelMultiplier"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].accelMultiplier must be numeric";
                    return false;
                }
                definition.accelMultiplier = std::clamp(profile["accelMultiplier"].get<float>(), 0.05f, 16.0f);
            }
            if (profile.contains("gravityScale")) {
                if (!profile["gravityScale"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].gravityScale must be numeric";
                    return false;
                }
                definition.gravityScale = std::clamp(profile["gravityScale"].get<float>(), 0.05f, 8.0f);
            }
        } else {
            if (profile.contains("boardRequired")) {
                if (!profile["boardRequired"].is_boolean()) {
                    outError = "profiles[" + std::to_string(i) + "].boardRequired must be boolean";
                    return false;
                }
                definition.boardRequired = profile["boardRequired"].get<bool>();
            }
            if (profile.contains("boardSpawnMode")) {
                if (!profile["boardSpawnMode"].is_string()) {
                    outError = "profiles[" + std::to_string(i) + "].boardSpawnMode must be string";
                    return false;
                }
                definition.boardSpawnMode = ToLower(profile["boardSpawnMode"].get<std::string>());
                if (definition.boardSpawnMode != "persistent_under_player") {
                    outError = "profiles[" + std::to_string(i) + "].boardSpawnMode must be persistent_under_player";
                    return false;
                }
            }
            if (profile.contains("idlePose")) {
                if (!profile["idlePose"].is_string()) {
                    outError = "profiles[" + std::to_string(i) + "].idlePose must be string";
                    return false;
                }
                definition.idlePose = ToLower(profile["idlePose"].get<std::string>());
                if (definition.idlePose != "stand" && definition.idlePose != "tpose") {
                    outError = "profiles[" + std::to_string(i) + "].idlePose must be stand|tpose";
                    return false;
                }
            }
            if (profile.contains("idleLock")) {
                if (!profile["idleLock"].is_boolean()) {
                    outError = "profiles[" + std::to_string(i) + "].idleLock must be boolean";
                    return false;
                }
                definition.idleLock = profile["idleLock"].get<bool>();
            }
            if (profile.contains("boardHeightOffset")) {
                if (!profile["boardHeightOffset"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].boardHeightOffset must be numeric";
                    return false;
                }
                definition.boardHeightOffset = std::clamp(profile["boardHeightOffset"].get<float>(), -100.0f, 100.0f);
            }
            if (profile.contains("boardPitchRollFromGround")) {
                if (!profile["boardPitchRollFromGround"].is_boolean()) {
                    outError = "profiles[" + std::to_string(i) + "].boardPitchRollFromGround must be boolean";
                    return false;
                }
                definition.boardPitchRollFromGround = profile["boardPitchRollFromGround"].get<bool>();
            }
            if (profile.contains("boardVisibleWhenIdle")) {
                if (!profile["boardVisibleWhenIdle"].is_boolean()) {
                    outError = "profiles[" + std::to_string(i) + "].boardVisibleWhenIdle must be boolean";
                    return false;
                }
                definition.boardVisibleWhenIdle = profile["boardVisibleWhenIdle"].get<bool>();
            }
            if (profile.contains("boardModelAsset")) {
                if (!profile["boardModelAsset"].is_string()) {
                    outError = "profiles[" + std::to_string(i) + "].boardModelAsset must be string";
                    return false;
                }
                definition.boardModelAsset = profile["boardModelAsset"].get<std::string>();
            }
            if (profile.contains("boardScale")) {
                if (!profile["boardScale"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].boardScale must be numeric";
                    return false;
                }
                definition.boardScale = std::clamp(profile["boardScale"].get<float>(), 0.05f, 20.0f);
            }
            if (profile.contains("boardPitchOffsetDeg")) {
                if (!profile["boardPitchOffsetDeg"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].boardPitchOffsetDeg must be numeric";
                    return false;
                }
                definition.boardPitchOffsetDeg = std::clamp(profile["boardPitchOffsetDeg"].get<float>(), -180.0f, 180.0f);
            }
            if (profile.contains("boardYawOffsetDeg")) {
                if (!profile["boardYawOffsetDeg"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].boardYawOffsetDeg must be numeric";
                    return false;
                }
                definition.boardYawOffsetDeg = std::clamp(profile["boardYawOffsetDeg"].get<float>(), -180.0f, 180.0f);
            }
            if (profile.contains("boardRollOffsetDeg")) {
                if (!profile["boardRollOffsetDeg"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].boardRollOffsetDeg must be numeric";
                    return false;
                }
                definition.boardRollOffsetDeg = std::clamp(profile["boardRollOffsetDeg"].get<float>(), -180.0f, 180.0f);
            }
            if (profile.contains("boardForwardOffset")) {
                if (!profile["boardForwardOffset"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].boardForwardOffset must be numeric";
                    return false;
                }
                definition.boardForwardOffset = std::clamp(profile["boardForwardOffset"].get<float>(), -200.0f, 200.0f);
            }
            if (profile.contains("boardRightOffset")) {
                if (!profile["boardRightOffset"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].boardRightOffset must be numeric";
                    return false;
                }
                definition.boardRightOffset = std::clamp(profile["boardRightOffset"].get<float>(), -200.0f, 200.0f);
            }
            if (profile.contains("boardUpOffset")) {
                if (!profile["boardUpOffset"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].boardUpOffset must be numeric";
                    return false;
                }
                definition.boardUpOffset = std::clamp(profile["boardUpOffset"].get<float>(), -200.0f, 200.0f);
            }
            if (profile.contains("riderHeightOffset")) {
                if (!profile["riderHeightOffset"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].riderHeightOffset must be numeric";
                    return false;
                }
                definition.riderHeightOffset = std::clamp(profile["riderHeightOffset"].get<float>(), -20.0f, 40.0f);
            }
            if (profile.contains("surfMaxSpeed")) {
                if (!profile["surfMaxSpeed"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].surfMaxSpeed must be numeric";
                    return false;
                }
                definition.surfMaxSpeed = std::clamp(profile["surfMaxSpeed"].get<float>(), 0.1f, 50.0f);
            }
            if (profile.contains("surfForwardAccel")) {
                if (!profile["surfForwardAccel"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].surfForwardAccel must be numeric";
                    return false;
                }
                definition.surfForwardAccel = std::clamp(profile["surfForwardAccel"].get<float>(), 0.0f, 5.0f);
            }
            if (profile.contains("surfBoostButton")) {
                if (!ParseButtonMask(profile["surfBoostButton"], definition.surfBoostButtonMask, outError)) {
                    outError = "profiles[" + std::to_string(i) + "].surfBoostButton: " + outError;
                    return false;
                }
            }
            if (profile.contains("surfBoostAccel")) {
                if (!profile["surfBoostAccel"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].surfBoostAccel must be numeric";
                    return false;
                }
                definition.surfBoostAccel = std::clamp(profile["surfBoostAccel"].get<float>(), 0.0f, 10.0f);
            }
            if (profile.contains("surfBoostMaxSpeed")) {
                if (!profile["surfBoostMaxSpeed"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].surfBoostMaxSpeed must be numeric";
                    return false;
                }
                definition.surfBoostMaxSpeed = std::clamp(profile["surfBoostMaxSpeed"].get<float>(), 0.1f, 80.0f);
            }
            if (profile.contains("surfBoostCooldownFrames")) {
                if (!profile["surfBoostCooldownFrames"].is_number_integer()) {
                    outError = "profiles[" + std::to_string(i) + "].surfBoostCooldownFrames must be integer";
                    return false;
                }
                definition.surfBoostCooldownFrames =
                    std::clamp(profile["surfBoostCooldownFrames"].get<int32_t>(), 1, 600);
            }
            if (profile.contains("surfDownhillAccel")) {
                if (!profile["surfDownhillAccel"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].surfDownhillAccel must be numeric";
                    return false;
                }
                definition.surfDownhillAccel = std::clamp(profile["surfDownhillAccel"].get<float>(), 0.0f, 10.0f);
            }
            if (profile.contains("surfUphillBrake")) {
                if (!profile["surfUphillBrake"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].surfUphillBrake must be numeric";
                    return false;
                }
                definition.surfUphillBrake = std::clamp(profile["surfUphillBrake"].get<float>(), 0.0f, 10.0f);
            }
            if (profile.contains("surfFlatDrag")) {
                if (!profile["surfFlatDrag"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].surfFlatDrag must be numeric";
                    return false;
                }
                definition.surfFlatDrag = std::clamp(profile["surfFlatDrag"].get<float>(), 0.0f, 0.95f);
            }
            if (profile.contains("surfTurnRateDeg")) {
                if (!profile["surfTurnRateDeg"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].surfTurnRateDeg must be numeric";
                    return false;
                }
                definition.surfTurnRateDeg = std::clamp(profile["surfTurnRateDeg"].get<float>(), 0.0f, 45.0f);
            }
            if (profile.contains("idleSpeedThreshold")) {
                if (!profile["idleSpeedThreshold"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].idleSpeedThreshold must be numeric";
                    return false;
                }
                definition.idleSpeedThreshold = std::clamp(profile["idleSpeedThreshold"].get<float>(), 0.0f, 5.0f);
            }
        }

        outDefinitions.push_back(std::move(definition));
    }

    return true;
}

bool ExternalModManager::TryParseCameraDefinitions(const std::string& content, int32_t apiVersion,
                                                   std::vector<ExternalModAimCameraProfile>& outDefinitions,
                                                   std::string& outError) {
    outDefinitions.clear();
    if (apiVersion < kExternalModApiVersionV3) {
        outError = "cameraDefinitions requires apiVersion 3";
        return false;
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(content);
    } catch (const std::exception& ex) {
        outError = std::string("camera_profiles.json parse error: ") + ex.what();
        return false;
    }

    if (!json.is_object() || !json.contains("profiles") || !json["profiles"].is_array()) {
        outError = "camera_profiles.json must be object with profiles[]";
        return false;
    }
    if (!json.contains("schemaVersion") || !json["schemaVersion"].is_number_integer() ||
        json["schemaVersion"].get<int32_t>() != 1) {
        outError = "camera_profiles.json schemaVersion must be 1";
        return false;
    }

    std::unordered_set<std::string> seenIds;
    const auto& profiles = json["profiles"];
    for (size_t i = 0; i < profiles.size(); ++i) {
        const auto& profile = profiles[i];
        if (!profile.is_object()) {
            outError = "profiles[" + std::to_string(i) + "] must be object";
            return false;
        }

        ExternalModAimCameraProfile definition;
        if (!ValidateRequiredString(profile, "id", definition.id, outError)) {
            outError = "profiles[" + std::to_string(i) + "].id: " + outError;
            return false;
        }
        if (!IsNamespacedCatalogId(definition.id)) {
            outError = "profiles[" + std::to_string(i) + "].id must be namespaced";
            return false;
        }
        if (!seenIds.insert(definition.id).second) {
            outError = "Duplicate camera profile id: " + definition.id;
            return false;
        }

        if (!profile.contains("contexts") || !profile["contexts"].is_array()) {
            outError = "profiles[" + std::to_string(i) + "].contexts must be array";
            return false;
        }
        definition.contextsMask = 0;
        for (size_t contextIndex = 0; contextIndex < profile["contexts"].size(); ++contextIndex) {
            const auto& contextValue = profile["contexts"][contextIndex];
            if (!contextValue.is_string()) {
                outError = "profiles[" + std::to_string(i) + "].contexts[" + std::to_string(contextIndex) +
                           "] must be string";
                return false;
            }
            ExternalModAimCameraContext context;
            if (!ParseAimCameraContextToken(contextValue.get<std::string>(), context)) {
                outError = "profiles[" + std::to_string(i) + "].contexts[" + std::to_string(contextIndex) +
                           "] unsupported context token";
                return false;
            }
            definition.contextsMask |= AimCameraContextToMask(context);
        }
        if (definition.contextsMask == 0) {
            outError = "profiles[" + std::to_string(i) + "].contexts must contain at least one entry";
            return false;
        }

        auto applyModeForContext = [&](ExternalModAimCameraContext context, int16_t mode, bool overShoulder) {
            if (overShoulder) {
                switch (context) {
                    case ExternalModAimCameraContext::CUp:
                        definition.cUpOverShoulderMode = mode;
                        break;
                    case ExternalModAimCameraContext::Bow:
                        definition.bowOverShoulderMode = mode;
                        break;
                    case ExternalModAimCameraContext::Hookshot:
                        definition.hookshotOverShoulderMode = mode;
                        break;
                    case ExternalModAimCameraContext::Slingshot:
                        definition.slingshotOverShoulderMode = mode;
                        break;
                    case ExternalModAimCameraContext::Boomerang:
                        definition.boomerangOverShoulderMode = mode;
                        break;
                    default:
                        break;
                }
            } else {
                switch (context) {
                    case ExternalModAimCameraContext::CUp:
                        definition.cUpFirstPersonMode = mode;
                        break;
                    case ExternalModAimCameraContext::Bow:
                        definition.bowFirstPersonMode = mode;
                        break;
                    case ExternalModAimCameraContext::Hookshot:
                        definition.hookshotFirstPersonMode = mode;
                        break;
                    case ExternalModAimCameraContext::Slingshot:
                        definition.slingshotFirstPersonMode = mode;
                        break;
                    case ExternalModAimCameraContext::Boomerang:
                        definition.boomerangFirstPersonMode = mode;
                        break;
                    default:
                        break;
                }
            }
        };

        auto parseContextModes = [&](const char* fieldName, bool overShoulder) -> bool {
            if (!profile.contains(fieldName)) {
                return true;
            }
            const auto& modeJson = profile[fieldName];
            if (modeJson.is_string()) {
                int16_t mode = 0;
                if (!ParseCameraModeTypeToken(modeJson.get<std::string>(), mode)) {
                    outError = "profiles[" + std::to_string(i) + "]." + fieldName + " has unsupported camera mode token";
                    return false;
                }
                for (const auto context :
                     { ExternalModAimCameraContext::CUp, ExternalModAimCameraContext::Bow, ExternalModAimCameraContext::Hookshot,
                       ExternalModAimCameraContext::Slingshot, ExternalModAimCameraContext::Boomerang }) {
                    applyModeForContext(context, mode, overShoulder);
                }
                return true;
            }

            if (!modeJson.is_object()) {
                outError = "profiles[" + std::to_string(i) + "]." + fieldName + " must be string or object";
                return false;
            }

            for (const auto& [contextToken, modeTokenJson] : modeJson.items()) {
                if (!modeTokenJson.is_string()) {
                    outError = "profiles[" + std::to_string(i) + "]." + fieldName + "." + contextToken + " must be string";
                    return false;
                }

                ExternalModAimCameraContext context;
                if (!ParseAimCameraContextToken(contextToken, context)) {
                    outError = "profiles[" + std::to_string(i) + "]." + fieldName + "." + contextToken +
                               " has unsupported context";
                    return false;
                }

                int16_t mode = 0;
                if (!ParseCameraModeTypeToken(modeTokenJson.get<std::string>(), mode)) {
                    outError = "profiles[" + std::to_string(i) + "]." + fieldName + "." + contextToken +
                               " has unsupported camera mode token";
                    return false;
                }

                applyModeForContext(context, mode, overShoulder);
            }

            return true;
        };

        if (!parseContextModes("firstPersonModeByContext", false) ||
            !parseContextModes("overShoulderModeByContext", true)) {
            return false;
        }

        if (profile.contains("shoulder")) {
            if (!profile["shoulder"].is_string()) {
                outError = "profiles[" + std::to_string(i) + "].shoulder must be string";
                return false;
            }
            definition.shoulder = ToLower(profile["shoulder"].get<std::string>());
            if (definition.shoulder != "right" && definition.shoulder != "left") {
                outError = "profiles[" + std::to_string(i) + "].shoulder must be right|left";
                return false;
            }
        }

        if (profile.contains("aimRay")) {
            if (!profile["aimRay"].is_string()) {
                outError = "profiles[" + std::to_string(i) + "].aimRay must be string";
                return false;
            }
            definition.aimRay = ToLower(profile["aimRay"].get<std::string>());
            if (definition.aimRay != "camera_center") {
                outError = "profiles[" + std::to_string(i) + "].aimRay must be camera_center";
                return false;
            }
        }

        if (profile.contains("reticle")) {
            if (!profile["reticle"].is_object()) {
                outError = "profiles[" + std::to_string(i) + "].reticle must be object";
                return false;
            }
            const auto& reticle = profile["reticle"];
            if (reticle.contains("x")) {
                if (!reticle["x"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].reticle.x must be numeric";
                    return false;
                }
                definition.reticleX = std::clamp(reticle["x"].get<float>(), 0.0f, 1.0f);
            }
            if (reticle.contains("y")) {
                if (!reticle["y"].is_number()) {
                    outError = "profiles[" + std::to_string(i) + "].reticle.y must be numeric";
                    return false;
                }
                definition.reticleY = std::clamp(reticle["y"].get<float>(), 0.0f, 1.0f);
            }
        }

        if (profile.contains("mouseFireEnabled")) {
            if (!profile["mouseFireEnabled"].is_boolean()) {
                outError = "profiles[" + std::to_string(i) + "].mouseFireEnabled must be boolean";
                return false;
            }
            definition.mouseFireEnabled = profile["mouseFireEnabled"].get<bool>();
        }

        if (profile.contains("mouseFireButton")) {
            if (!profile["mouseFireButton"].is_string()) {
                outError = "profiles[" + std::to_string(i) + "].mouseFireButton must be string";
                return false;
            }
            if (!ParseAimMouseButtonToken(profile["mouseFireButton"].get<std::string>(), definition.mouseFireButton)) {
                outError = "profiles[" + std::to_string(i) + "].mouseFireButton must be left|right|middle|backward|forward";
                return false;
            }
        }

        if (profile.contains("mouseFireMode")) {
            if (!profile["mouseFireMode"].is_string()) {
                outError = "profiles[" + std::to_string(i) + "].mouseFireMode must be string";
                return false;
            }
            if (!ParseAimMouseFireModeToken(profile["mouseFireMode"].get<std::string>(), definition.mouseFireMode)) {
                outError = "profiles[" + std::to_string(i) + "].mouseFireMode must be both|firstPerson|overShoulder";
                return false;
            }
        }

        if (profile.contains("reticleVisibility")) {
            if (!profile["reticleVisibility"].is_string()) {
                outError = "profiles[" + std::to_string(i) + "].reticleVisibility must be string";
                return false;
            }
            if (!ParseAimReticleVisibilityToken(profile["reticleVisibility"].get<std::string>(),
                                                definition.reticleVisibility)) {
                outError = "profiles[" + std::to_string(i) + "].reticleVisibility must be aim_only|button_hold|selected";
                return false;
            }
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
    runtime.maxActiveStatusEffects = package.manifest.runtimeMaxActiveStatuses;

    if (package.manifest.apiVersion >= kExternalModApiVersionV3) {
        std::filesystem::path itemsPath;
        if (!IsSafePackageRelativePath(package.manifest.itemDefinitions, itemsPath, outError)) {
            return false;
        }
        std::string itemContent;
        if (!ReadFileFromPackage(package, itemsPath, kMaxItemDefinitionBytes, itemContent, outError)) {
            return false;
        }

        nlohmann::json itemDefinitionsJson;
        try {
            itemDefinitionsJson = nlohmann::json::parse(itemContent);
        } catch (const std::exception& ex) {
            outError = std::string("itemDefinitions parse error: ") + ex.what();
            return false;
        }

        if (ManifestHasCapability(package.manifest, "patches.vanilla_items.v1")) {
            std::filesystem::path patchPath;
            if (!IsSafePackageRelativePath(package.manifest.vanillaItemPatches, patchPath, outError)) {
                outError = "Invalid vanillaItemPatches: " + outError;
                return false;
            }

            std::string patchContent;
            if (!ReadFileFromPackage(package, patchPath, kMaxVanillaPatchDefinitionBytes, patchContent, outError)) {
                outError = "Failed to read vanillaItemPatches: " + outError;
                return false;
            }

            nlohmann::json patchJson;
            try {
                patchJson = nlohmann::json::parse(patchContent);
            } catch (const std::exception& ex) {
                outError = std::string("vanillaItemPatches parse error: ") + ex.what();
                return false;
            }

            std::string patchError;
            if (!TryApplyVanillaItemPatchesToJson(itemDefinitionsJson, patchJson, patchError)) {
                outError = "vanillaItemPatches apply failed: " + patchError;
                return false;
            }
            SPDLOG_INFO("[ExternalMods] Applied vanilla item patches for {}", package.manifest.id);
        }

        if (!TryParseItemDefinitions(itemDefinitionsJson.dump(), runtime.itemDefinitions, outError)) {
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

            if (!definition.aimReticleTextureAsset.empty()) {
                std::filesystem::path reticlePath;
                if (!IsSafePackageRelativePath(definition.aimReticleTextureAsset, reticlePath, outError)) {
                    outError = "items[" + std::to_string(i) + "].aimReticleTextureAsset " + outError;
                    return false;
                }

                if (ToLower(reticlePath.extension().string()) != ".png") {
                    outError = "items[" + std::to_string(i) + "].aimReticleTextureAsset must point to a .png file";
                    return false;
                }

                std::vector<uint8_t> reticleBytes;
                if (!ReadBinaryFromPackage(package, reticlePath, kMaxHookshotTextureBytes, reticleBytes, outError)) {
                    outError = "items[" + std::to_string(i) + "].aimReticleTextureAsset read failed: " + outError;
                    return false;
                }

                std::vector<uint8_t> reticleRgba32;
                if (!TryDecodePngToRgba32FixedSize(reticleBytes, kAimReticleTextureWidth, kAimReticleTextureHeight,
                                                   reticleRgba32, outError)) {
                    outError = "items[" + std::to_string(i) + "].aimReticleTextureAsset decode failed: " + outError;
                    return false;
                }
                ConvertRgba32ToI8(reticleRgba32, definition.aimReticleTextureI8);
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
        if (!TryParseInputDefinitions(inputContent, runtime.inputBindings, runtime.cameraHotkeys, outError)) {
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

        if (ManifestHasCapability(package.manifest, "statuses.catalog.v1")) {
            std::filesystem::path statusPath;
            if (!IsSafePackageRelativePath(package.manifest.statusDefinitions, statusPath, outError)) {
                outError = "Invalid statusDefinitions: " + outError;
                return false;
            }

            std::string statusContent;
            if (!ReadFileFromPackage(package, statusPath, kMaxStatusDefinitionBytes, statusContent, outError)) {
                outError = "Failed to read statusDefinitions: " + outError;
                return false;
            }
            if (!TryParseStatusDefinitions(statusContent, runtime.apiVersion, runtime.statusDefinitions, outError)) {
                return false;
            }
        }

        if (ManifestHasCapability(package.manifest, "combat.damage.v1")) {
            std::filesystem::path damagePath;
            if (!IsSafePackageRelativePath(package.manifest.damageDefinitions, damagePath, outError)) {
                outError = "Invalid damageDefinitions: " + outError;
                return false;
            }

            std::string damageContent;
            if (!ReadFileFromPackage(package, damagePath, kMaxDamageDefinitionBytes, damageContent, outError)) {
                outError = "Failed to read damageDefinitions: " + outError;
                return false;
            }
            if (!TryParseDamageDefinitions(damageContent, runtime.apiVersion, runtime.damageProfiles, outError)) {
                return false;
            }
        }

        if (ManifestHasCapability(package.manifest, "combat.targeting.v1")) {
            std::filesystem::path targetingPath;
            if (!IsSafePackageRelativePath(package.manifest.targetingDefinitions, targetingPath, outError)) {
                outError = "Invalid targetingDefinitions: " + outError;
                return false;
            }

            std::string targetingContent;
            if (!ReadFileFromPackage(package, targetingPath, kMaxTargetingDefinitionBytes, targetingContent, outError)) {
                outError = "Failed to read targetingDefinitions: " + outError;
                return false;
            }
            if (!TryParseTargetingDefinitions(targetingContent, runtime.apiVersion, runtime.targetingProfiles, outError)) {
                return false;
            }
        }

        if (ManifestHasCapability(package.manifest, "items.use_profiles.v1")) {
            std::filesystem::path useProfilesPath;
            if (!IsSafePackageRelativePath(package.manifest.itemUseProfiles, useProfilesPath, outError)) {
                outError = "Invalid itemUseProfiles: " + outError;
                return false;
            }

            std::string useProfilesContent;
            if (!ReadFileFromPackage(package, useProfilesPath, kMaxUseProfileDefinitionBytes, useProfilesContent,
                                     outError)) {
                outError = "Failed to read itemUseProfiles: " + outError;
                return false;
            }
            if (!TryParseItemUseProfiles(useProfilesContent, runtime.apiVersion, runtime.itemUseProfiles, outError)) {
                return false;
            }
        }

        if (ManifestHasCapability(package.manifest, "combat.projectiles.v1")) {
            std::filesystem::path projectilePath;
            if (!IsSafePackageRelativePath(package.manifest.projectileDefinitions, projectilePath, outError)) {
                outError = "Invalid projectileDefinitions: " + outError;
                return false;
            }

            std::string projectileContent;
            if (!ReadFileFromPackage(package, projectilePath, kMaxProjectileDefinitionBytes, projectileContent, outError)) {
                outError = "Failed to read projectileDefinitions: " + outError;
                return false;
            }
            if (!TryParseProjectileDefinitions(projectileContent, runtime.apiVersion, runtime.projectileProfiles,
                                               outError)) {
                return false;
            }
        }

        if (ManifestHasCapability(package.manifest, "combat.aoe.v1")) {
            std::filesystem::path aoePath;
            if (!IsSafePackageRelativePath(package.manifest.aoeDefinitions, aoePath, outError)) {
                outError = "Invalid aoeDefinitions: " + outError;
                return false;
            }

            std::string aoeContent;
            if (!ReadFileFromPackage(package, aoePath, kMaxAoEDefinitionBytes, aoeContent, outError)) {
                outError = "Failed to read aoeDefinitions: " + outError;
                return false;
            }
            if (!TryParseAoEDefinitions(aoeContent, runtime.apiVersion, runtime.aoeProfiles, outError)) {
                return false;
            }
        }

        if (ManifestHasCapability(package.manifest, "movement.profiles.v1")) {
            std::filesystem::path movementPath;
            if (!IsSafePackageRelativePath(package.manifest.movementDefinitions, movementPath, outError)) {
                outError = "Invalid movementDefinitions: " + outError;
                return false;
            }

            std::string movementContent;
            if (!ReadFileFromPackage(package, movementPath, kMaxMovementDefinitionBytes, movementContent, outError)) {
                outError = "Failed to read movementDefinitions: " + outError;
                return false;
            }
            if (!TryParseMovementDefinitions(movementContent, runtime.apiVersion, runtime.movementProfiles, outError)) {
                return false;
            }
        }

        if (ManifestHasCapability(package.manifest, "camera.aim_profiles.v1")) {
            std::filesystem::path cameraPath;
            if (!IsSafePackageRelativePath(package.manifest.cameraDefinitions, cameraPath, outError)) {
                outError = "Invalid cameraDefinitions: " + outError;
                return false;
            }

            std::string cameraContent;
            if (!ReadFileFromPackage(package, cameraPath, kMaxCameraDefinitionBytes, cameraContent, outError)) {
                outError = "Failed to read cameraDefinitions: " + outError;
                return false;
            }
            if (!TryParseCameraDefinitions(cameraContent, runtime.apiVersion, runtime.cameraProfiles, outError)) {
                return false;
            }
        }

        const auto hasStatusRef = [&](const std::string& statusId) -> bool {
            if (statusId.empty()) {
                return true;
            }
            ExternalModStatusType builtinType = ExternalModStatusType::Custom;
            if (ParseStatusIdToken(statusId, builtinType) && builtinType != ExternalModStatusType::Custom) {
                return true;
            }
            return ExternalModContentRegistry::FindStatusDefinitionById(runtime, statusId) != nullptr;
        };
        const auto hasDamageRef = [&](const std::string& damageId) {
            return damageId.empty() || ExternalModContentRegistry::FindDamageProfileById(runtime, damageId) != nullptr;
        };
        const auto hasTargetingRef = [&](const std::string& targetingId) {
            return targetingId.empty() || ExternalModContentRegistry::FindTargetingProfileById(runtime, targetingId) != nullptr;
        };
        const auto hasProjectileRef = [&](const std::string& projectileId) {
            return projectileId.empty() || ExternalModContentRegistry::FindProjectileProfileById(runtime, projectileId) != nullptr;
        };
        const auto hasAoERef = [&](const std::string& aoeId) {
            return aoeId.empty() || ExternalModContentRegistry::FindAoEProfileById(runtime, aoeId) != nullptr;
        };
        const auto hasMovementRef = [&](const std::string& movementId) {
            return movementId.empty() || ExternalModContentRegistry::FindMovementProfileById(runtime, movementId) != nullptr;
        };

        for (const auto& profile : runtime.itemUseProfiles) {
            if (!hasTargetingRef(profile.targetingProfileId)) {
                outError = "use profile references unknown targetingProfile: " + profile.id + " -> " +
                           profile.targetingProfileId;
                return false;
            }
            for (const auto& effect : profile.effects) {
                if (!hasDamageRef(effect.damageProfileId) || !hasStatusRef(effect.statusId) ||
                    !hasProjectileRef(effect.projectileProfileId) || !hasAoERef(effect.aoeProfileId) ||
                    !hasMovementRef(effect.movementProfileId)) {
                    outError = "use profile references missing catalog id: " + profile.id;
                    return false;
                }
            }
        }
        for (const auto& projectile : runtime.projectileProfiles) {
            if (!hasDamageRef(projectile.damageProfileId)) {
                outError = "projectile references unknown damageProfile: " + projectile.id + " -> " +
                           projectile.damageProfileId;
                return false;
            }
            for (const auto& statusId : projectile.applyStatuses) {
                if (!hasStatusRef(statusId)) {
                    outError = "projectile references unknown status: " + projectile.id + " -> " + statusId;
                    return false;
                }
            }
        }
        for (const auto& aoe : runtime.aoeProfiles) {
            const auto validateEffects = [&](const std::vector<ExternalModUseProfileEffect>& effects,
                                             const char* listName) -> bool {
                for (const auto& effect : effects) {
                    if (!hasDamageRef(effect.damageProfileId) || !hasStatusRef(effect.statusId) ||
                        !hasProjectileRef(effect.projectileProfileId) || !hasAoERef(effect.aoeProfileId) ||
                        !hasMovementRef(effect.movementProfileId)) {
                        outError = "aoe profile references missing catalog id: " + aoe.id + "." + listName;
                        return false;
                    }
                }
                return true;
            };
            if (!validateEffects(aoe.onEnter, "onEnter") || !validateEffects(aoe.onTick, "onTick") ||
                !validateEffects(aoe.onExit, "onExit")) {
                return false;
            }
        }
        for (const auto& itemDefinition : runtime.itemDefinitions) {
            if (!itemDefinition.useProfile.empty() && ExternalModContentRegistry::FindItemUseProfileById(runtime, itemDefinition.useProfile) == nullptr) {
                outError = "item references unknown useProfile: " + itemDefinition.id + " -> " + itemDefinition.useProfile;
                return false;
            }
        }

        if (package.manifest.runtimeType != "wasm3-v1") {
            outError = "Unsupported runtime.type for apiVersion 3: " + package.manifest.runtimeType;
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
        config.maxFrameBudgetMs = package.manifest.runtimeMaxFrameBudgetMs;
        config.hostApi.useItemProfile = [modId = package.manifest.id](const std::string& itemOrProfileId) {
            return ExternalModManager::Instance().WasmHostUseItemProfile(modId, itemOrProfileId);
        };
        config.hostApi.resolveTarget = [modId = package.manifest.id](const std::string& profileId,
                                                                     std::vector<int32_t>& outHandles) {
            return ExternalModManager::Instance().WasmHostResolveTarget(modId, profileId, outHandles);
        };
        config.hostApi.dealDamage = [modId = package.manifest.id](int32_t targetHandle, const std::string& damageProfileId) {
            return ExternalModManager::Instance().WasmHostDealDamage(modId, targetHandle, damageProfileId);
        };
        config.hostApi.applyStatus =
            [modId = package.manifest.id](int32_t targetHandle, const std::string& statusId, int32_t durationOverrideFrames) {
                return ExternalModManager::Instance().WasmHostApplyStatus(modId, targetHandle, statusId,
                                                                          durationOverrideFrames);
            };
        config.hostApi.spawnProjectile =
            [modId = package.manifest.id](const std::string& profileId, const std::string& overridesJson) {
                return ExternalModManager::Instance().WasmHostSpawnProjectile(modId, profileId, overridesJson);
            };
        config.hostApi.spawnAoE = [modId = package.manifest.id](const std::string& profileId, const std::string& originJson) {
            return ExternalModManager::Instance().WasmHostSpawnAoE(modId, profileId, originJson);
        };
        config.hostApi.applyMovementProfile =
            [modId = package.manifest.id](const std::string& profileId, int32_t durationFrames) {
                return ExternalModManager::Instance().WasmHostApplyMovementProfile(modId, profileId, durationFrames);
            };
        config.hostApi.applyImpulse = [modId = package.manifest.id](int32_t mode, float strength, float x, float y, float z) {
            return ExternalModManager::Instance().WasmHostApplyImpulse(modId, mode, strength, x, y, z);
        };
        config.hostApi.getGroundInfo = [modId = package.manifest.id](ExternalModWasmGroundInfo& outInfo) {
            return ExternalModManager::Instance().WasmHostGetGroundInfo(modId, outInfo);
        };
        config.hostApi.raycast =
            [modId = package.manifest.id](const std::string& queryJson, ExternalModWasmRaycastHit& outHit) {
                return ExternalModManager::Instance().WasmHostRaycast(modId, queryJson, outHit);
            };
        config.hostApi.raycastAll = [modId = package.manifest.id](const std::string& queryJson, int32_t outCapacity,
                                                                  std::vector<ExternalModWasmRaycastHit>& outHits) {
            return ExternalModManager::Instance().WasmHostRaycastAll(modId, queryJson, outCapacity, outHits);
        };

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
                if (sceneDefinition == nullptr) {
                    DisableRuntime(package, "loadModScene references unknown sceneId: " + action.modSceneId);
                    return;
                }

                int16_t transitionEntrance = 0;
                if (sceneDefinition->useNamespacedScene) {
                    if (!sceneDefinition->hasHostEntrance) {
                        DisableRuntime(package, "loadModScene namespaced scene missing hostEntrance: " + action.modSceneId);
                        return;
                    }

                    transitionEntrance = sceneDefinition->hostEntranceIndex;

                    auto& manager = ExternalModManager::Instance();
                    auto& pendingRequest = manager.mPendingSceneLoadRequest;
                    if (pendingRequest.pending) {
                        SPDLOG_WARN(
                            "[ExternalMods] Overwriting pending namespaced scene request {}.{} with {}.{}",
                            pendingRequest.modId, pendingRequest.sceneId, package.manifest.id, sceneDefinition->id);
                    }

                    pendingRequest = ExternalModPendingSceneLoadRequest{};
                    pendingRequest.pending = true;
                    pendingRequest.modId = package.manifest.id;
                    pendingRequest.sceneId = sceneDefinition->id;
                    pendingRequest.sceneResourcePath = sceneDefinition->sceneResourcePath;
                    pendingRequest.expectedHostSceneId = ResolveSceneIdForEntranceIndex(sceneDefinition->hostEntranceIndex);
                    pendingRequest.hasHostEntrance = sceneDefinition->hasHostEntrance;
                    pendingRequest.hostEntranceIndex = sceneDefinition->hostEntranceIndex;
                    pendingRequest.hasFallbackEntrance = sceneDefinition->hasFallbackEntrance;
                    pendingRequest.fallbackEntranceIndex = sceneDefinition->fallbackEntranceIndex;
                    pendingRequest.fallbackPlayable = sceneDefinition->fallbackPlayable;
                    pendingRequest.spawnId = action.sceneSpawnId;

                    SPDLOG_INFO(
                        "[ExternalMods] Queued namespaced scene request for {}.{}: resource={} hostEntrance={} "
                        "fallbackEntrance={} fallbackPlayable={} spawnId={} expectedHostScene={}",
                        package.manifest.id, sceneDefinition->id, pendingRequest.sceneResourcePath,
                        pendingRequest.hostEntranceIndex, pendingRequest.fallbackEntranceIndex,
                        pendingRequest.fallbackPlayable ? "true" : "false", pendingRequest.spawnId,
                        pendingRequest.expectedHostSceneId);
                } else {
                    if (!sceneDefinition->hasEntrance) {
                        DisableRuntime(package, "loadModScene references scene without entrance: " + action.modSceneId);
                        return;
                    }
                    transitionEntrance = sceneDefinition->entranceIndex;
                }

                if (gPlayState != nullptr) {
                    gPlayState->nextEntranceIndex = transitionEntrance;
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
            case ExternalModActionType::ApplyStatus: {
                if (gPlayState == nullptr) {
                    break;
                }
                auto* actionPlayer = GET_PLAYER(gPlayState);
                if (actionPlayer == nullptr || !IsActionItemRequirementSatisfied(package, action, actionPlayer)) {
                    break;
                }

                ExternalModAction resolvedAction = action;
                ExternalModStatusType resolvedStatusType = action.statusType;
                if (!ResolveStatusActionData(package, action, resolvedAction, resolvedStatusType)) {
                    DisableRuntime(package, "applyStatus failed resolving status data");
                    return;
                }

                bool canMiss = false;
                Actor* target =
                    ResolveStatusActionTarget(package, resolvedAction, gPlayState, actionPlayer, resolvedStatusType, canMiss);
                if (target == nullptr) {
                    if (canMiss) {
                        SpawnStatusMissEffect(gPlayState, actionPlayer, resolvedStatusType);
                    }
                    break;
                }

                if (ShouldProtectStatusTarget(target, resolvedStatusType)) {
                    break;
                }

                const bool isPlayerTarget = target->id == ACTOR_PLAYER;
                BeginStatusOnActor(package.runtime, gPlayState, target, resolvedAction, resolvedStatusType, isPlayerTarget,
                                   resolvedAction.statusId, package.manifest.id);
                break;
            }
            case ExternalModActionType::ClearStatus: {
                if (gPlayState == nullptr) {
                    break;
                }
                auto* actionPlayer = GET_PLAYER(gPlayState);
                if (actionPlayer == nullptr) {
                    break;
                }

                bool canMiss = false;
                Actor* target =
                    ResolveStatusActionTarget(package, action, gPlayState, actionPlayer, action.statusType, canMiss);
                if (target == nullptr) {
                    break;
                }
                if (!action.statusId.empty()) {
                    ClearStatusEffectsOnActorById(package.runtime, gPlayState, target, action.statusId, true);
                } else {
                    ClearStatusEffectsOnActor(package.runtime, gPlayState, target, true, action.statusType, true);
                }
                break;
            }
            case ExternalModActionType::ClearAllStatuses: {
                if (gPlayState == nullptr) {
                    break;
                }
                auto* actionPlayer = GET_PLAYER(gPlayState);
                if (actionPlayer == nullptr) {
                    break;
                }
                bool canMiss = false;
                Actor* target =
                    ResolveStatusActionTarget(package, action, gPlayState, actionPlayer, action.statusType, canMiss);
                if (target != nullptr) {
                    ClearStatusEffectsOnActor(package.runtime, gPlayState, target, false, ExternalModStatusType::Custom,
                                              true);
                }
                break;
            }
            case ExternalModActionType::DealDamage: {
                if (gPlayState == nullptr) {
                    break;
                }
                auto* actionPlayer = GET_PLAYER(gPlayState);
                if (actionPlayer == nullptr) {
                    break;
                }

                const auto* damageProfile = ExternalModContentRegistry::FindDamageProfileById(package.runtime, action.damageProfileId);
                if (damageProfile == nullptr) {
                    DisableRuntime(package, "dealDamage references unknown profile: " + action.damageProfileId);
                    return;
                }

                bool canMiss = false;
                Actor* target =
                    ResolveStatusActionTarget(package, action, gPlayState, actionPlayer, ExternalModStatusType::Custom, canMiss);
                if (target != nullptr) {
                    ApplyDamageProfileToActor(gPlayState, target, *damageProfile);
                }
                break;
            }
            case ExternalModActionType::UseItemProfile: {
                if (gPlayState == nullptr) {
                    break;
                }
                auto* actionPlayer = GET_PLAYER(gPlayState);
                if (actionPlayer == nullptr) {
                    break;
                }
                std::string executeError;
                if (!ExecuteUseProfileById(package, action.itemUseProfileId, gPlayState, actionPlayer, nullptr,
                                           executeError)) {
                    DisableRuntime(package, "useItemProfile failed: " + executeError);
                    return;
                }
                break;
            }
            case ExternalModActionType::SpawnProjectile: {
                if (gPlayState == nullptr) {
                    break;
                }
                auto* actionPlayer = GET_PLAYER(gPlayState);
                if (actionPlayer == nullptr) {
                    break;
                }
                ExternalModUseProfileEffect effect;
                effect.action = "spawnProjectile";
                effect.projectileProfileId = action.projectileProfileId;
                std::vector<Actor*> targets;
                const ExternalModTargetingProfile targetingProfile = { .mode = ExternalModTargetingMode::FrontTarget,
                                                                       .range = std::max(action.range, 1.0f) };
                ResolveTargetsForTargetingProfile(targetingProfile, gPlayState, actionPlayer, targets);
                std::string executeError;
                if (!ExecuteUseProfileEffects(package, { effect }, targets, gPlayState, actionPlayer, nullptr,
                                             executeError)) {
                    DisableRuntime(package, "spawnProjectile failed: " + executeError);
                    return;
                }
                break;
            }
            case ExternalModActionType::SpawnAoE: {
                if (gPlayState == nullptr) {
                    break;
                }
                auto* actionPlayer = GET_PLAYER(gPlayState);
                if (actionPlayer == nullptr) {
                    break;
                }
                ExternalModUseProfileEffect effect;
                effect.action = "spawnAoE";
                effect.aoeProfileId = action.aoeProfileId;
                std::vector<Actor*> targets = { &actionPlayer->actor };
                std::string executeError;
                if (!ExecuteUseProfileEffects(package, { effect }, targets, gPlayState, actionPlayer, nullptr,
                                             executeError)) {
                    DisableRuntime(package, "spawnAoE failed: " + executeError);
                    return;
                }
                break;
            }
            case ExternalModActionType::ApplyMovementProfile: {
                if (gPlayState == nullptr) {
                    break;
                }
                auto* actionPlayer = GET_PLAYER(gPlayState);
                if (actionPlayer == nullptr) {
                    break;
                }
                ExternalModUseProfileEffect effect;
                effect.action = "applyMovementProfile";
                effect.movementProfileId = action.movementProfileId;
                effect.durationFrames = action.durationFrames;
                std::vector<Actor*> targets = { &actionPlayer->actor };
                std::string executeError;
                if (!ExecuteUseProfileEffects(package, { effect }, targets, gPlayState, actionPlayer, nullptr,
                                             executeError)) {
                    DisableRuntime(package, "applyMovementProfile failed: " + executeError);
                    return;
                }
                break;
            }
            case ExternalModActionType::ApplyImpulse:
                if (gPlayState != nullptr) {
                    auto* actionPlayer = GET_PLAYER(gPlayState);
                    if (actionPlayer != nullptr) {
                        const float impulse = action.impulseStrength == 0.0f ? 8.0f : action.impulseStrength;
                        ExternalModItemRuntime::ApplySkyhookImpulse(actionPlayer, impulse, impulse);
                    }
                }
                break;
            case ExternalModActionType::GetGroundInfo:
                if (gPlayState != nullptr) {
                    auto* actionPlayer = GET_PLAYER(gPlayState);
                    if (actionPlayer != nullptr) {
                        package.runtime.globalBlackboard["__groundY"] = std::to_string(actionPlayer->actor.floorHeight);
                        package.runtime.globalBlackboard["__grounded"] =
                            ((actionPlayer->actor.bgCheckFlags & 1) != 0) ? "1" : "0";
                    }
                }
                break;
            case ExternalModActionType::Raycast:
            case ExternalModActionType::RaycastAll:
                if (gPlayState != nullptr) {
                    auto* actionPlayer = GET_PLAYER(gPlayState);
                    if (actionPlayer != nullptr) {
                        std::vector<Actor*> rayHits;
                        ResolveForwardRaycastHits(gPlayState, actionPlayer, std::max(action.range, 1.0f), true, true, true,
                                                  rayHits);

                        if (action.type == ExternalModActionType::Raycast && rayHits.size() > 1) {
                            rayHits.resize(1);
                        }

                        package.runtime.globalBlackboard["__raycastHit"] = rayHits.empty() ? "0" : "1";
                        package.runtime.globalBlackboard["__raycastHitCount"] = std::to_string(rayHits.size());
                        package.runtime.globalBlackboard["__raycastActorId"] =
                            rayHits.empty() ? "-1" : std::to_string(rayHits.front()->id);

                        std::ostringstream actorIdList;
                        for (size_t i = 0; i < rayHits.size(); ++i) {
                            if (i > 0) {
                                actorIdList << ",";
                            }
                            actorIdList << rayHits[i]->id;
                        }
                        package.runtime.globalBlackboard["__raycastActorIds"] = actorIdList.str();
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
            case ExternalModActionType::ToggleAimCameraMode:
            case ExternalModActionType::SetAimCameraMode:
            case ExternalModActionType::SetAimCameraProfile: {
                auto* player = gPlayState != nullptr ? GET_PLAYER(gPlayState) : nullptr;
                if (!IsActionItemRequirementSatisfied(package, action, player)) {
                    break;
                }

                auto* manager = &ExternalModManager::Instance();
                auto resolveActionProfileId = [&]() -> std::string {
                    if (action.aimCameraProfileId.empty()) {
                        return {};
                    }
                    return ResolveProfileIdForMod(package.manifest.id, action.aimCameraProfileId);
                };

                auto applyProfile = [&](const std::string& resolvedProfileId) -> bool {
                    if (resolvedProfileId.empty()) {
                        return true;
                    }

                    const std::string ownerModId = GetProfileOwnerModId(resolvedProfileId, package.manifest.id);
                    if (manager->FindAimCameraProfileById(ownerModId, resolvedProfileId) == nullptr) {
                        DisableRuntime(package, "aim camera action references unknown profile id: " + resolvedProfileId);
                        return false;
                    }

                    manager->mAimCameraState.activeProfileId = resolvedProfileId;
                    manager->mAimCameraState.activeProfileOwnerModId = ownerModId;
                    return true;
                };

                const std::string resolvedProfileId = resolveActionProfileId();
                if (action.type == ExternalModActionType::SetAimCameraProfile) {
                    if (!applyProfile(resolvedProfileId)) {
                        return;
                    }
                    break;
                }

                if (!applyProfile(resolvedProfileId)) {
                    return;
                }

                bool targetEnabled = manager->mAimCameraState.overShoulderEnabled;
                if (action.type == ExternalModActionType::ToggleAimCameraMode) {
                    targetEnabled = !targetEnabled;
                } else {
                    targetEnabled = action.aimCameraMode == ExternalModAimCameraMode::OverShoulder;
                }

                manager->mAimCameraState.overShoulderEnabled = targetEnabled;
                CVarSetInteger(kAimCameraOverShoulderCVar, targetEnabled ? 1 : 0);
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
                package.runtime.wasmCallsThisFrame = package.runtime.wasmRuntime->GetCallsThisFrame();
                package.runtime.wasmBudgetDropsThisFrame = package.runtime.wasmRuntime->GetBudgetDropsThisFrame();
                break;
            }
            default:
                break;
        }
    }
}

void ExternalModManager::DisableRuntime(ExternalModPackage& package, const std::string& reason) {
    ClearStatusEffects(package.runtime, gPlayState, false);
    ClearSurfState(package.runtime);
    package.runtime.wasmTargetHandles.clear();
    package.runtime.wasmNextTargetHandle = 1;
    package.runtime.enabled = false;
    package.valid = false;
    package.error = reason;
    ExternalModManager::Instance().PruneAimCameraStateForUnavailableProfiles();
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
                package.runtime.wasmCallsThisFrame = package.runtime.wasmRuntime->GetCallsThisFrame();
                package.runtime.wasmBudgetDropsThisFrame = package.runtime.wasmRuntime->GetBudgetDropsThisFrame();
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
            ClearStatusEffects(package.runtime, gPlayState, false);
            ClearSurfState(package.runtime);
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
            package.runtime.wasmTargetHandles.clear();
            package.runtime.wasmNextTargetHandle = 1;
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

    PruneAimCameraStateForUnavailableProfiles();
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

    if (mPersistentInventoryDirty) {
        MarkPersistentInventoryDirty();
    }
}

void ExternalModManager::OnSceneInit(int16_t sceneNum) {
    for (auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }
        try {
            ClearStatusEffects(package.runtime, gPlayState, false);
            ClearSurfState(package.runtime);
            package.runtime.behaviorStepsThisFrame = 0;
            package.runtime.pendingSignals.clear();
            package.runtime.actorInstances.clear();
            package.runtime.nextActorHandle = 1;
            package.runtime.wasmTargetHandles.clear();
            package.runtime.wasmNextTargetHandle = 1;
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
    PruneAimCameraStateForUnavailableProfiles();

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
    if (gPlayState != nullptr) {
        auto* player = GET_PLAYER(gPlayState);
        if (player != nullptr) {
            ExternalModPackage* selectedSurfPackage = nullptr;
            for (auto& package : mPackages) {
                if (!package.runtime.enabled || !package.runtime.surfState.active) {
                    continue;
                }
                if (selectedSurfPackage == nullptr || package.manifest.loadPriority > selectedSurfPackage->manifest.loadPriority ||
                    (package.manifest.loadPriority == selectedSurfPackage->manifest.loadPriority &&
                     package.manifest.id < selectedSurfPackage->manifest.id)) {
                    selectedSurfPackage = &package;
                }
            }
            if (selectedSurfPackage != nullptr) {
                TickSurfState(*selectedSurfPackage, gPlayState, player);
            }
        }
    }

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

    ProcessAssignedActionButtons(gPlayState, player, input);

    for (auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }

        package.runtime.hookCallsThisFrame = 0;
        package.runtime.behaviorStepsThisFrame = 0;
        package.runtime.wasmCallsThisFrame = 0;
        package.runtime.wasmBudgetDropsThisFrame = 0;
        package.runtime.wasmTargetHandles.clear();
        if (package.runtime.wasmRuntime) {
            package.runtime.wasmRuntime->BeginFrame();
        }
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

            TickStatusEffects(package.runtime, gPlayState);
            if (!package.runtime.enabled) {
                break;
            }

            for (auto& item : package.runtime.itemDefinitions) {
                if (!item.granted) {
                    continue;
                }

                if (GetParamOrDefault(item.params, "infiniteAmmo", 0.0f) > 0.0f) {
                    int32_t ammoItemId = ResolveGrantedAmmoItemId(item);
                    if (ammoItemId == ITEM_NONE && item.hasGrantItemId) {
                        ammoItemId = item.grantItemId;
                    }
                    if (ammoItemId != ITEM_NONE) {
                        const int32_t refillTarget = std::max(item.hasGrantAmmo ? item.grantAmmo : 0, 30);
                        AMMO(ammoItemId) = static_cast<int8_t>(std::clamp(refillTarget, 0, 99));
                    }
                }

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

            if (package.runtime.wasmRuntime) {
                package.runtime.wasmCallsThisFrame = package.runtime.wasmRuntime->GetCallsThisFrame();
                package.runtime.wasmBudgetDropsThisFrame = package.runtime.wasmRuntime->GetBudgetDropsThisFrame();
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

    ApplyGlobalPlayerStatusModifiers(mPackages, gPlayState);

    SyncExtraInventoryGrid();
    ApplyModItemAgeRequirementOverrides(mPackages);
    ApplyModItemIconOverrides(mPackages);
    ApplyModHookshotTextureOverrides(mPackages);
}

bool ExternalModManager::OnHammerGroundImpact(PlayState* play, Player* player, float impactX, float impactY, float impactZ) {
    if (play == nullptr || player == nullptr) {
        return false;
    }

    ExternalModPackage* selectedPackage = nullptr;
    ExternalModItemDefinition* selectedItem = nullptr;
    int32_t selectedPriority = std::numeric_limits<int32_t>::min();
    std::string selectedModId;
    const int32_t heldItemId = player->heldItemId;

    for (auto& package : mPackages) {
        if (!package.runtime.enabled) {
            continue;
        }

        for (auto& item : package.runtime.itemDefinitions) {
            if (!item.granted || item.useTrigger != ExternalModItemUseTrigger::HammerGroundImpact || !item.hasSlot ||
                item.slot != ExternalModItemSlot::Hammer || item.useProfile.empty() || item.cooldownRemaining > 0 ||
                !ItemDefinitionMatchesUseItem(item, heldItemId)) {
                continue;
            }

            const bool shouldReplace = selectedPackage == nullptr || package.manifest.loadPriority > selectedPriority ||
                                       (package.manifest.loadPriority == selectedPriority &&
                                        package.manifest.id < selectedModId);
            if (shouldReplace) {
                selectedPackage = &package;
                selectedItem = &item;
                selectedPriority = package.manifest.loadPriority;
                selectedModId = package.manifest.id;
            }
        }
    }

    if (selectedPackage == nullptr || selectedItem == nullptr) {
        return false;
    }

    auto& runtime = selectedPackage->runtime;
    runtime.hasEffectImpactPosition = true;
    runtime.effectImpactPosX = impactX;
    runtime.effectImpactPosY = impactY;
    runtime.effectImpactPosZ = impactZ;
    runtime.useProfileSpawnedShockwave = false;

    const auto clearImpactContext = [&runtime]() {
        runtime.hasEffectImpactPosition = false;
        runtime.effectImpactPosX = 0.0f;
        runtime.effectImpactPosY = 0.0f;
        runtime.effectImpactPosZ = 0.0f;
        runtime.useProfileSpawnedShockwave = false;
    };

    std::string useProfileError;
    if (!ExecuteUseProfileById(*selectedPackage, selectedItem->useProfile, play, player, selectedItem, useProfileError)) {
        clearImpactContext();
        DisableRuntime(*selectedPackage, "hammerGroundImpact useProfile failed for '" + selectedItem->id +
                                            "': " + useProfileError);
        return false;
    }

    int32_t defaultCooldown = selectedItem->cooldownFrames;
    if (const auto* useProfile =
            ExternalModContentRegistry::FindItemUseProfileById(selectedPackage->runtime, selectedItem->useProfile);
        useProfile != nullptr) {
        defaultCooldown = useProfile->cooldownFrames;
    }
    const auto cooldown = static_cast<int32_t>(
        std::max(0.0f, GetParamOrDefault(selectedItem->params, "cooldown", static_cast<float>(defaultCooldown))));
    selectedItem->cooldownFrames = cooldown;
    selectedItem->cooldownRemaining = cooldown;

    const bool handledShockwave = runtime.useProfileSpawnedShockwave;
    clearImpactContext();
    return handledShockwave;
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

            if (item.useTrigger == ExternalModItemUseTrigger::HammerGroundImpact) {
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

            if (!item.useProfile.empty() && gPlayState != nullptr) {
                auto* actionPlayer = static_cast<Player*>(player);
                std::string useProfileError;
                if (!ExecuteUseProfileById(package, item.useProfile, gPlayState, actionPlayer, &item, useProfileError)) {
                    DisableRuntime(package, "item useProfile failed for '" + item.id + "': " + useProfileError);
                    *allowVanilla = true;
                    return;
                }
            }

            if (item.slot == ExternalModItemSlot::Hookshot && item.useMode != ExternalModItemUseMode::Vanilla) {
                const float pullForce = GetParamOrDefault(item.params, "pullForce", 8.0f);
                const float speed = GetParamOrDefault(item.params, "speed", 12.0f);
                ExternalModItemRuntime::ApplySkyhookImpulse(player, pullForce, speed);
            }

            int32_t defaultCooldown = item.cooldownFrames;
            if (!item.useProfile.empty()) {
                const auto* useProfile = ExternalModContentRegistry::FindItemUseProfileById(package.runtime, item.useProfile);
                if (useProfile != nullptr) {
                    defaultCooldown = useProfile->cooldownFrames;
                }
            }
            const auto cooldown = static_cast<int32_t>(
                std::max(0.0f, GetParamOrDefault(item.params, "cooldown", static_cast<float>(defaultCooldown))));
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
        ClearStatusEffects(package.runtime, gPlayState, false);
        ClearSurfState(package.runtime);
        package.runtime.hasEffectImpactPosition = false;
        package.runtime.effectImpactPosX = 0.0f;
        package.runtime.effectImpactPosY = 0.0f;
        package.runtime.effectImpactPosZ = 0.0f;
        package.runtime.useProfileSpawnedShockwave = false;
        package.runtime.actorInstances.clear();
        package.runtime.nextActorHandle = 1;
        package.runtime.pendingSignals.clear();
        package.runtime.wasmTargetHandles.clear();
        package.runtime.wasmNextTargetHandle = 1;
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

extern "C" {
int32_t ExternalMods_GetVirtualInventoryPageCount(void) {
    return SOH::ExternalModManager::Instance().GetExtraInventoryPageCount();
}

int32_t ExternalMods_GetVirtualInventoryCellCount(void) {
    return static_cast<int32_t>(SOH::ExternalModManager::Instance().GetExtraInventoryGrid().size());
}

int32_t ExternalMods_GetVirtualInventoryPageCell(int32_t pageIndex, int32_t cellIndex,
                                                 ExternalModsPauseCellInfo* outInfo) {
    if (outInfo == nullptr) {
        return 0;
    }

    SOH::ExternalModInventoryCellView cell;
    if (!SOH::ExternalModManager::Instance().GetExtraInventoryPageCell(pageIndex, cellIndex, cell)) {
        return 0;
    }

    static thread_local std::array<std::string, 24> sCellDisplayNames;
    const int32_t safeCellIndex = (cellIndex >= 0 && cellIndex < 24) ? cellIndex : 0;
    sCellDisplayNames[safeCellIndex] = cell.displayName.empty() ? cell.itemId : cell.displayName;

    outInfo->occupied = cell.occupied ? 1 : 0;
    outInfo->absoluteIndex = static_cast<int32_t>(cell.index);
    outInfo->iconRgba32 = cell.iconRgba32;
    outInfo->displayName = sCellDisplayNames[safeCellIndex].c_str();
    outInfo->assignableButtonsMask = cell.assignableButtonsMask;
    outInfo->assignedButtonsMask = cell.assignedButtonsMask;
    return 1;
}

int32_t ExternalMods_MoveVirtualInventoryCell(int32_t fromAbsoluteIndex, int32_t toAbsoluteIndex) {
    std::string error;
    const bool ok = SOH::ExternalModManager::Instance().MoveExtraInventoryCell(
        static_cast<size_t>(std::max(0, fromAbsoluteIndex)), static_cast<size_t>(std::max(0, toAbsoluteIndex)), error);
    return ok ? 1 : 0;
}

int32_t ExternalMods_AssignVirtualInventoryCellToButton(int32_t absoluteIndex, int32_t buttonIndex) {
    std::string error;
    const bool ok = SOH::ExternalModManager::Instance().EquipExtraInventoryCellToButton(
        static_cast<size_t>(std::max(0, absoluteIndex)), buttonIndex, error);
    return ok ? 1 : 0;
}

void ExternalMods_OnVanillaButtonEquipped(int32_t buttonIndex) {
    SOH::ExternalModManager::Instance().OnVanillaButtonEquipped(buttonIndex);
}

int32_t ExternalMods_TryDrawButtonOverrideIcon(PlayState* play, int32_t buttonIndex, int32_t alpha) {
    return SOH::ExternalModManager::Instance().TryDrawButtonOverrideIcon(play, buttonIndex, alpha) ? 1 : 0;
}

int32_t ExternalMods_DrawSurfBoardIfActive(PlayState* play, Player* player) {
    return SOH::ExternalModManager::Instance().DrawSurfBoardIfActive(play, player) ? 1 : 0;
}

int32_t ExternalMods_OnHammerGroundImpact(PlayState* play, Player* player, float impactX, float impactY, float impactZ) {
    return SOH::ExternalModManager::Instance().OnHammerGroundImpact(play, player, impactX, impactY, impactZ) ? 1 : 0;
}

int32_t ExternalMods_ResolveAimCameraMode(PlayState* play, Player* player, int32_t defaultMode, int32_t context) {
    SOH::ExternalModAimCameraContext aimContext = SOH::ExternalModAimCameraContext::CUp;
    switch (context) {
        case EXTERNAL_MODS_AIM_CONTEXT_BOW:
            aimContext = SOH::ExternalModAimCameraContext::Bow;
            break;
        case EXTERNAL_MODS_AIM_CONTEXT_HOOKSHOT:
            aimContext = SOH::ExternalModAimCameraContext::Hookshot;
            break;
        case EXTERNAL_MODS_AIM_CONTEXT_SLINGSHOT:
            aimContext = SOH::ExternalModAimCameraContext::Slingshot;
            break;
        case EXTERNAL_MODS_AIM_CONTEXT_BOOMERANG:
            aimContext = SOH::ExternalModAimCameraContext::Boomerang;
            break;
        case EXTERNAL_MODS_AIM_CONTEXT_CUP:
        default:
            aimContext = SOH::ExternalModAimCameraContext::CUp;
            break;
    }

    return static_cast<int32_t>(
        SOH::ExternalModManager::Instance().ResolveAimCameraMode(play, player, static_cast<int16_t>(defaultMode), aimContext));
}

int32_t ExternalMods_IsAimOverShoulderEnabled(void) {
    return SOH::ExternalModManager::Instance().IsAimOverShoulderEnabled() ? 1 : 0;
}

int32_t ExternalMods_DrawAimReticleIfActive(PlayState* play, Player* player, int32_t context) {
    SOH::ExternalModAimCameraContext aimContext = SOH::ExternalModAimCameraContext::CUp;
    switch (context) {
        case EXTERNAL_MODS_AIM_CONTEXT_BOW:
            aimContext = SOH::ExternalModAimCameraContext::Bow;
            break;
        case EXTERNAL_MODS_AIM_CONTEXT_HOOKSHOT:
            aimContext = SOH::ExternalModAimCameraContext::Hookshot;
            break;
        case EXTERNAL_MODS_AIM_CONTEXT_SLINGSHOT:
            aimContext = SOH::ExternalModAimCameraContext::Slingshot;
            break;
        case EXTERNAL_MODS_AIM_CONTEXT_BOOMERANG:
            aimContext = SOH::ExternalModAimCameraContext::Boomerang;
            break;
        case EXTERNAL_MODS_AIM_CONTEXT_CUP:
        default:
            aimContext = SOH::ExternalModAimCameraContext::CUp;
            break;
    }

    return SOH::ExternalModManager::Instance().DrawAimReticleIfActive(play, player, aimContext) ? 1 : 0;
}

int32_t ExternalMods_IsAimMouseFireHeld(PlayState* play, Player* player, int32_t heldItemAction) {
    return SOH::ExternalModManager::Instance().IsAimMouseFireHeld(play, player, heldItemAction) ? 1 : 0;
}

int32_t ExternalMods_HasCustomEquippedSlingshotModel(void) {
    return SOH::ExternalModManager::Instance().HasCustomEquippedSlingshotModel() ? 1 : 0;
}

int32_t ExternalMods_DrawCustomEquippedSlingshotModel(PlayState* play) {
    return SOH::ExternalModManager::Instance().DrawCustomEquippedSlingshotModel(play) ? 1 : 0;
}
}

