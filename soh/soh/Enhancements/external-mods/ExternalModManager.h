#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace SOH {

class ExternalModWasmRuntime;

enum class ExternalModInputTriggerType {
    Pressed,
    Held,
    Released,
};

enum class ExternalModItemSlot {
    Hookshot,
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
    std::string itemDefinitions;
    std::string inputDefinitions;
};

enum class ExternalModActionType {
    ShowNotification,
    TeleportToEntrance,
    PressButton,
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
    std::string onUseExport;
    std::string onUpdateExport;
    std::unordered_map<std::string, float> params;
    bool granted = false;
    int32_t cooldownFrames = 0;
    int32_t cooldownRemaining = 0;
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

class ExternalModManager {
  public:
    static ExternalModManager& Instance();

    void Initialize();
    void Shutdown();
    void DiscoverPackages();
    const std::vector<ExternalModPackage>& GetPackages() const;
    static std::string BuildBindingCVarName(const std::string& modId, const std::string& bindingId);

  private:
    std::vector<ExternalModPackage> mPackages;
    uint32_t mOnLoadGameHook = 0;
    uint32_t mOnSceneInitHook = 0;
    uint32_t mOnGameFrameHook = 0;
    uint32_t mOnPlayerUseItemHook = 0;

    static bool TryParseManifest(const std::string& content, ExternalModManifest& outManifest, std::string& outError);
    static bool TryParseEntryScript(const std::string& content, int32_t apiVersion, ExternalModRuntime& outRuntime,
                                    std::string& outError);
    static bool TryParseItemDefinitions(const std::string& content, std::vector<ExternalModItemDefinition>& outItems,
                                        std::string& outError);
    static bool TryParseInputDefinitions(const std::string& content, std::vector<ExternalModInputBinding>& outBindings,
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

    static bool MountAssetsForPackage(ExternalModPackage& package, std::string& outError);
    static bool LoadRuntimeForPackage(ExternalModPackage& package, std::string& outError);
    static bool IsSafePackageRelativePath(const std::string& pathValue, std::filesystem::path& outNormalizedPath,
                                          std::string& outError);
    static void ExecuteActions(ExternalModPackage& package, const std::vector<ExternalModAction>& actions,
                               const char* triggerName);
    static void DisableRuntime(ExternalModPackage& package, const std::string& reason);

    void RegisterHooks();
    void UnregisterHooks();
    void OnLoadGame(int32_t fileNum);
    void OnSceneInit(int16_t sceneNum);
    void OnGameFrameUpdate();
    void OnPlayerUseItem(void* player, int32_t itemId, bool* allowVanilla);
};

} // namespace SOH
