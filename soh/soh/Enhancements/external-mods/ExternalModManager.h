#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct GetItemEntry;

namespace SOH {

class ExternalModWasmRuntime;

enum class ExternalModInputTriggerType {
    Pressed,
    Held,
    Released,
};

enum class ExternalModItemSlot {
    Hookshot,
    Stick,
    Bow,
    FireArrow,
    IceArrow,
    LightArrow,
    Hammer,
};

enum class ExternalModItemAgePolicy {
    RespectVanilla,
    AllowChild,
    AllowAdult,
};

enum class ExternalModItemUseMode {
    Vanilla,
    Override,
    Augment,
};

enum class ExternalModActorArchetype {
    Npc,
    Prop,
    Trigger,
};

enum class ExternalModRuntimeModuleFormat {
    WasmBinary,
    WatText,
};

struct ExternalModManifest {
    std::string id;
    std::string name;
    std::string version;
    int32_t apiVersion = 0;
    std::string gameVersionMin;
    std::string entryScript;
    std::vector<std::string> assets;
    std::vector<std::string> dependencies;
    int32_t loadOrder = 0;

    std::string runtimeType;
    std::string runtimeModule;
    int32_t runtimeMaxMemoryKb = 1024;
    int32_t runtimeMaxCallMs = 2;
    int32_t runtimeMaxFrameBudgetMs = 2;
    int32_t runtimeMaxHookCallsPerFrame = 256;
    int32_t runtimeMaxActorInstances = 64;
    std::string itemDefinitions;
    std::string inputDefinitions;
    std::string hookDefinitions;
    std::string actorDefinitions;
    std::vector<std::string> capabilities;
};

enum class ExternalModActionType {
    ShowNotification,
    TeleportToEntrance,
    PressButton,
    SpawnSmoke,
    SpawnKusa,
    LanternLight,
    IgniteFrontTarget,
    SpawnActor,
    DespawnActor,
    SetActorState,
    MoveActorToPathNode,
    OpenDialog,
    GrantModItem,
    RevokeModItem,
    InvokeWasm,
};

struct ExternalModAction {
    ExternalModActionType type = ExternalModActionType::ShowNotification;
    std::string text;
    int16_t entranceIndex = 0;
    int32_t buttonMask = 0;
    std::string itemId;
    std::string exportName;
    std::vector<int32_t> args;

    std::string actorDefinitionId;
    uint32_t actorHandle = 0;
    std::string actorStateKey;
    std::string actorStateValue;
    int32_t pathNodeIndex = 0;
    int32_t dialogId = 0;
};

struct ExternalModSceneAction {
    int16_t sceneId = 0;
    std::vector<ExternalModAction> actions;
};

struct ExternalModTriggerVolume {
    std::string id;
    int16_t sceneId = 0;
    float minX = 0.0f;
    float minY = 0.0f;
    float minZ = 0.0f;
    float maxX = 0.0f;
    float maxY = 0.0f;
    float maxZ = 0.0f;
    int32_t cooldownFrames = 0;
    int32_t cooldownRemaining = 0;
    bool wasInside = false;
    std::vector<ExternalModAction> actions;
};

struct ExternalModInputBinding {
    std::string id;
    int32_t defaultMask = 0;
    ExternalModInputTriggerType defaultTrigger = ExternalModInputTriggerType::Pressed;
    bool allowUserRemap = true;
};

struct ExternalModInputActionTrigger {
    std::string id;
    std::string bindingId;
    ExternalModInputTriggerType trigger = ExternalModInputTriggerType::Pressed;
    int32_t cooldownFrames = 0;
    int32_t cooldownRemaining = 0;
    std::vector<ExternalModAction> actions;
};

struct ExternalModItemDefinition {
    std::string id;
    std::string displayName;
    ExternalModItemSlot slot = ExternalModItemSlot::Hookshot;
    std::string iconAsset;
    std::vector<uint8_t> iconRgba32;
    std::string modelAsset;
    std::string modelTextureAsset;
    float modelScale = 1.0f;
    struct CustomModelVertex {
        int16_t x = 0;
        int16_t y = 0;
        int16_t z = 0;
        int16_t s = 0;
        int16_t t = 0;
    };
    struct CustomModelTriangle {
        std::array<CustomModelVertex, 3> vertices{};
    };
    std::vector<CustomModelTriangle> customModelTriangles;
    std::vector<uint8_t> modelTextureRgba32;
    int32_t modelTextureWidth = 0;
    int32_t modelTextureHeight = 0;
    std::string description;
    std::string onUseExport;
    std::string onUpdateExport;
    ExternalModItemAgePolicy agePolicy = ExternalModItemAgePolicy::AllowChild;
    ExternalModItemUseMode useMode = ExternalModItemUseMode::Vanilla;
    bool hasGrantItemId = false;
    int32_t grantItemId = -1;
    bool hasGrantAmmo = false;
    int32_t grantAmmo = 0;
    std::unordered_map<std::string, float> params;
    bool granted = false;
    int32_t cooldownFrames = 0;
    int32_t cooldownRemaining = 0;
};

struct ExternalModActorDefinition {
    std::string id;
    ExternalModActorArchetype archetype = ExternalModActorArchetype::Npc;
    int16_t sceneId = -1;
    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;
    float rotX = 0.0f;
    float rotY = 0.0f;
    float rotZ = 0.0f;
    int32_t maxInstances = 1;
    int32_t tickRate = 1;
    float lodDistance = 5000.0f;
    std::string exportOnInit;
    std::string exportOnUpdate;
    std::string exportOnInteract;
    std::string exportOnDestroy;
};

struct ExternalModActorInstance {
    uint32_t handle = 0;
    std::string definitionId;
    bool active = false;
    int16_t sceneId = -1;
    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;
    float rotX = 0.0f;
    float rotY = 0.0f;
    float rotZ = 0.0f;
    int32_t tickCounter = 0;
    std::unordered_map<std::string, std::string> state;
};

enum class ExternalModHookDispatchType {
    Actions,
    WasmExport,
};

enum class ExternalModHookType {
    OnLoadGame,
    OnExitGame,
    OnSceneInit,
    AfterSceneCommands,
    OnTransitionEnd,
    OnFlagSet,
    OnFlagUnset,
    OnSceneFlagSet,
    OnSceneFlagUnset,
    OnPlayerUpdate,
    OnPlayerUseItem,
    OnPlayerHealthChange,
    OnItemReceive,
    OnActorInit,
    OnActorSpawn,
    OnActorUpdate,
    OnActorKill,
    OnActorDestroy,
    OnEnemyDefeat,
    OnBossDefeat,
    OnPlayDestroy,
    OnGameFrameUpdate,
};

struct ExternalModHookFilter {
    bool hasScene = false;
    int16_t scene = 0;
    bool hasActorId = false;
    int16_t actorId = 0;
    bool hasCategory = false;
    int16_t category = 0;
    bool hasItemId = false;
    int16_t itemId = 0;
    bool hasFlagType = false;
    int16_t flagType = 0;
    bool hasFlagId = false;
    int16_t flagId = 0;
    bool hasHealthDeltaRange = false;
    int16_t healthDeltaMin = 0;
    int16_t healthDeltaMax = 0;
};

struct ExternalModHookSubscription {
    std::string id;
    ExternalModHookType hook = ExternalModHookType::OnGameFrameUpdate;
    ExternalModHookDispatchType dispatch = ExternalModHookDispatchType::Actions;
    std::vector<ExternalModAction> actions;
    std::string wasmExport;
    int32_t cooldownFrames = 0;
    int32_t cooldownRemaining = 0;
    ExternalModHookFilter filters;
};

struct ExternalModRuntime {
    bool enabled = false;
    int32_t apiVersion = 1;
    std::vector<ExternalModAction> onGameLoadedActions;
    std::vector<ExternalModSceneAction> onSceneInitActions;
    std::vector<ExternalModTriggerVolume> frameTriggers;
    std::vector<ExternalModInputBinding> inputBindings;
    std::vector<ExternalModInputActionTrigger> inputTriggers;
    std::vector<ExternalModItemDefinition> itemDefinitions;
    std::vector<ExternalModHookSubscription> hookSubscriptions;
    std::vector<ExternalModActorDefinition> actorDefinitions;
    std::vector<ExternalModActorInstance> actorInstances;
    uint32_t nextActorHandle = 1;
    int32_t frameBudgetMs = 2;
    int32_t maxHookCallsPerFrame = 256;
    int32_t hookCallsThisFrame = 0;
    int32_t maxActorInstances = 64;
    ExternalModRuntimeModuleFormat moduleFormat = ExternalModRuntimeModuleFormat::WasmBinary;
    std::string moduleSourcePath;
    size_t compiledModuleSizeBytes = 0;
    int32_t moduleCompileTimeMs = 0;
    std::string moduleCompileDiagnostics;
    std::unique_ptr<ExternalModWasmRuntime> wasmRuntime;
};

struct ExternalModPackage {
    std::filesystem::path sourcePath;
    bool isZip = false;
    bool valid = false;
    std::string error;
    ExternalModManifest manifest;
    std::vector<std::filesystem::path> mountedAssets;
    ExternalModRuntime runtime;
};

struct ExternalModHookEventContext {
    int16_t scene = -1;
    int16_t actorId = -1;
    int16_t actorCategory = -1;
    int16_t itemId = -1;
    int16_t flagType = -1;
    int16_t flagId = -1;
    int16_t healthDelta = 0;
};

struct ExternalModInventoryCellView {
    size_t index = 0;
    bool occupied = false;
    std::string modId;
    std::string modName;
    std::string itemId;
    std::string displayName;
    ExternalModItemSlot slot = ExternalModItemSlot::Hookshot;
    bool granted = false;
};

class ExternalModManager {
  public:
    static ExternalModManager& Instance();

    void Initialize();
    void Shutdown();
    bool ReloadPackages(std::string& outError);
    void DiscoverPackages();
    std::vector<ExternalModPackage>& GetPackages();
    const std::vector<ExternalModPackage>& GetPackages() const;
    void ApplyGetItemVisualOverrides(::GetItemEntry& entry) const;
    std::vector<ExternalModInventoryCellView> GetExtraInventoryGrid() const;
    bool MoveExtraInventoryCell(size_t fromIndex, size_t toIndex, std::string& outError);
    bool EquipExtraInventoryCellToButton(size_t cellIndex, int32_t cButtonIndex, std::string& outError);
    static std::string BuildEnabledCVarName(const std::string& modId);
    static std::string BuildBindingCVarName(const std::string& modId, const std::string& bindingId);

  private:
    struct ExtraInventoryCell {
        std::string modId;
        std::string itemId;
    };

    std::vector<ExternalModPackage> mPackages;
    std::vector<ExtraInventoryCell> mExtraInventoryCells;
    uint32_t mOnLoadGameHook = 0;
    uint32_t mOnExitGameHook = 0;
    uint32_t mOnSceneInitHook = 0;
    uint32_t mAfterSceneCommandsHook = 0;
    uint32_t mOnTransitionEndHook = 0;
    uint32_t mOnFlagSetHook = 0;
    uint32_t mOnFlagUnsetHook = 0;
    uint32_t mOnSceneFlagSetHook = 0;
    uint32_t mOnSceneFlagUnsetHook = 0;
    uint32_t mOnPlayerUpdateHook = 0;
    uint32_t mOnGameFrameHook = 0;
    uint32_t mOnPlayerUseItemHook = 0;
    uint32_t mOnPlayerHealthChangeHook = 0;
    uint32_t mOnItemReceiveHook = 0;
    uint32_t mOnActorInitHook = 0;
    uint32_t mOnActorSpawnHook = 0;
    uint32_t mOnActorUpdateHook = 0;
    uint32_t mOnActorKillHook = 0;
    uint32_t mOnActorDestroyHook = 0;
    uint32_t mOnEnemyDefeatHook = 0;
    uint32_t mOnBossDefeatHook = 0;
    uint32_t mOnPlayDestroyHook = 0;

    static bool TryParseManifest(const std::string& content, ExternalModManifest& outManifest, std::string& outError);
    static bool TryParseEntryScript(const std::string& content, int32_t apiVersion, ExternalModRuntime& outRuntime,
                                    std::string& outError);
    static bool TryParseItemDefinitions(const std::string& content, std::vector<ExternalModItemDefinition>& outItems,
                                        std::string& outError);
    static bool TryParseInputDefinitions(const std::string& content, std::vector<ExternalModInputBinding>& outBindings,
                                         std::string& outError);
    static bool TryParseHookDefinitions(const std::string& content, int32_t apiVersion,
                                        std::vector<ExternalModHookSubscription>& outSubscriptions,
                                        std::string& outError);
    static bool TryParseActorDefinitions(const std::string& content, int32_t apiVersion,
                                         std::vector<ExternalModActorDefinition>& outDefinitions,
                                         std::string& outError);

    static bool ReadManifestFromDirectory(const std::filesystem::path& dirPath, std::string& outContent,
                                          std::string& outError);
    static bool ReadManifestFromZip(const std::filesystem::path& zipPath, std::string& outContent, std::string& outError);

    static bool ReadFileFromDirectory(const std::filesystem::path& filePath, uint64_t maxBytes, std::string& outContent,
                                      std::string& outError);
    static bool ReadFileFromZip(const std::filesystem::path& zipPath, const std::filesystem::path& packageRelativePath,
                                uint64_t maxBytes, std::vector<char>& outBytes, std::string& outError);
    static bool ReadFileFromPackage(const ExternalModPackage& package, const std::filesystem::path& packageRelativePath,
                                    uint64_t maxBytes, std::string& outContent, std::string& outError);
    static bool ReadBinaryFromPackage(const ExternalModPackage& package, const std::filesystem::path& packageRelativePath,
                                      uint64_t maxBytes, std::vector<uint8_t>& outBytes, std::string& outError);

    static void UnmountAssetsForPackage(ExternalModPackage& package);
    static bool MountAssetsForPackage(ExternalModPackage& package, std::string& outError);
    static bool LoadRuntimeForPackage(ExternalModPackage& package, std::string& outError);
    static bool IsSafePackageRelativePath(const std::string& pathValue, std::filesystem::path& outNormalizedPath,
                                          std::string& outError);
    static void ExecuteActions(ExternalModPackage& package, const std::vector<ExternalModAction>& actions,
                               const char* triggerName);
    static void DisableRuntime(ExternalModPackage& package, const std::string& reason);
    void SyncExtraInventoryGrid();

    void DispatchExtendedHook(ExternalModHookType hookType, const ExternalModHookEventContext& context,
                              const char* triggerName);

    void RegisterHooks();
    void UnregisterHooks();
    void OnLoadGame(int32_t fileNum);
    void OnExitGame(int32_t fileNum);
    void OnSceneInit(int16_t sceneNum);
    void OnAfterSceneCommands(int16_t sceneNum);
    void OnTransitionEnd(int16_t sceneNum);
    void OnFlagSet(int16_t flagType, int16_t flag);
    void OnFlagUnset(int16_t flagType, int16_t flag);
    void OnSceneFlagSet(int16_t sceneNum, int16_t flagType, int16_t flag);
    void OnSceneFlagUnset(int16_t sceneNum, int16_t flagType, int16_t flag);
    void OnPlayerUpdate();
    void OnGameFrameUpdate();
    void OnPlayerUseItem(void* player, int32_t itemId, bool* allowVanilla);
    void OnPlayerHealthChange(int16_t amount);
    void OnItemReceive(int16_t itemId);
    void OnActorHook(ExternalModHookType hookType, void* actor, const char* hookName);
    void OnPlayDestroy();
};

} // namespace SOH
