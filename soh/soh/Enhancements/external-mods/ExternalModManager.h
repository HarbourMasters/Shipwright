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
struct PlayState;
struct Player;
#include "ExternalModTypes.h"

namespace SOH {

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
    int32_t GetExtraInventoryPageCount() const;
    bool GetExtraInventoryPageCell(int32_t pageIndex, int32_t pageCellIndex, ExternalModInventoryCellView& outCell) const;
    bool MoveExtraInventoryCell(size_t fromIndex, size_t toIndex, std::string& outError);
    bool EquipExtraInventoryCellToButton(size_t cellIndex, int32_t buttonIndex, std::string& outError);
    void OnVanillaButtonEquipped(int32_t buttonIndex);
    bool TryDrawButtonOverrideIcon(::PlayState* play, int32_t buttonIndex, int32_t alpha) const;
    static std::string BuildEnabledCVarName(const std::string& modId);
    static std::string BuildBindingCVarName(const std::string& modId, const std::string& bindingId);
    bool TryConsumePendingSceneLoadRequest(int16_t sceneId, ExternalModPendingSceneLoadRequest& outRequest);
    void HandlePendingSceneLoadSuccess(const ExternalModPendingSceneLoadRequest& request);
    void HandlePendingSceneLoadFailure(const ExternalModPendingSceneLoadRequest& request, const std::string& error);
    void LoadPersistentInventoryState();
    void SavePersistentInventoryState() const;

  private:
    struct ExtraInventoryCell {
        std::string modId;
        std::string itemId;
    };

    enum class ActionButtonSource {
        Vanilla,
        Mod,
    };

    struct ActionButtonAssignment {
        ActionButtonSource source = ActionButtonSource::Vanilla;
        std::string modId;
        std::string itemId;
    };

    std::vector<ExternalModPackage> mPackages;
    ExternalModPendingSceneLoadRequest mPendingSceneLoadRequest;
    std::vector<ExtraInventoryCell> mExtraInventoryCells;
    std::array<ActionButtonAssignment, 8> mActionButtonAssignments{};
    int32_t mExtraInventoryPage = 0;
    int32_t mExtraInventoryCursor = 0;
    bool mPersistentInventoryDirty = false;
    bool mSaveSectionRegistered = false;
    int32_t mPersistentInventorySectionId = -1;
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
    static bool TryParseBehaviorDefinitions(const std::string& content, int32_t apiVersion,
                                            std::vector<ExternalModBehaviorDefinition>& outDefinitions,
                                            std::string& outError);
    static bool TryParseSceneDefinitions(const std::string& content, int32_t apiVersion,
                                         std::vector<ExternalModSceneDefinition>& outDefinitions,
                                         std::string& outError);
    static bool TryParseStatusDefinitions(const std::string& content, int32_t apiVersion,
                                          std::vector<ExternalModStatusDefinition>& outDefinitions,
                                          std::string& outError);
    static bool TryParseDamageDefinitions(const std::string& content, int32_t apiVersion,
                                          std::vector<ExternalModDamageProfile>& outDefinitions,
                                          std::string& outError);
    static bool TryParseTargetingDefinitions(const std::string& content, int32_t apiVersion,
                                             std::vector<ExternalModTargetingProfile>& outDefinitions,
                                             std::string& outError);
    static bool TryParseItemUseProfiles(const std::string& content, int32_t apiVersion,
                                        std::vector<ExternalModItemUseProfile>& outDefinitions,
                                        std::string& outError);
    static bool TryParseProjectileDefinitions(const std::string& content, int32_t apiVersion,
                                              std::vector<ExternalModProjectileProfile>& outDefinitions,
                                              std::string& outError);
    static bool TryParseAoEDefinitions(const std::string& content, int32_t apiVersion,
                                       std::vector<ExternalModAoEProfile>& outDefinitions, std::string& outError);
    static bool TryParseMovementDefinitions(const std::string& content, int32_t apiVersion,
                                            std::vector<ExternalModMovementProfile>& outDefinitions,
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
    void SyncButtonAssignments();
    bool TryInvokeAssignedModItem(int32_t buttonIndex, ::PlayState* play, ::Player* player);
    const ExternalModPackage* FindPackageByModId(const std::string& modId) const;
    ExternalModPackage* FindPackageByModId(const std::string& modId);
    const ExternalModItemDefinition* FindItemByAssignment(const ActionButtonAssignment& assignment) const;
    ExternalModItemDefinition* FindItemByAssignment(ActionButtonAssignment& assignment);
    bool IsAssignmentValid(const ActionButtonAssignment& assignment) const;
    void MarkPersistentInventoryDirty();
    void ProcessAssignedActionButtons(::PlayState* play, ::Player* player, void* input);

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
