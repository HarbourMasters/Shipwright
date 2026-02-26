#include "ExternalModContentRegistry.h"

#include <algorithm>

namespace SOH {

namespace {
template <typename T>
const T* FindById(const std::vector<T>& entries, const std::string& id) {
    const auto it = std::find_if(entries.begin(), entries.end(),
                                 [&id](const T& entry) {
                                     return entry.id == id;
                                 });
    if (it == entries.end()) {
        return nullptr;
    }
    return &(*it);
}
} // namespace

const ExternalModItemDefinition* ExternalModContentRegistry::FindItemDefinitionById(const ExternalModRuntime& runtime,
                                                                                     const std::string& itemId) {
    return FindById(runtime.itemDefinitions, itemId);
}

const ExternalModStatusDefinition* ExternalModContentRegistry::FindStatusDefinitionById(const ExternalModRuntime& runtime,
                                                                                         const std::string& statusId) {
    return FindById(runtime.statusDefinitions, statusId);
}

const ExternalModDamageProfile* ExternalModContentRegistry::FindDamageProfileById(const ExternalModRuntime& runtime,
                                                                                   const std::string& profileId) {
    return FindById(runtime.damageProfiles, profileId);
}

const ExternalModTargetingProfile* ExternalModContentRegistry::FindTargetingProfileById(const ExternalModRuntime& runtime,
                                                                                         const std::string& profileId) {
    return FindById(runtime.targetingProfiles, profileId);
}

const ExternalModItemUseProfile* ExternalModContentRegistry::FindItemUseProfileById(const ExternalModRuntime& runtime,
                                                                                     const std::string& profileId) {
    return FindById(runtime.itemUseProfiles, profileId);
}

const ExternalModProjectileProfile* ExternalModContentRegistry::FindProjectileProfileById(
    const ExternalModRuntime& runtime, const std::string& profileId) {
    return FindById(runtime.projectileProfiles, profileId);
}

const ExternalModAoEProfile* ExternalModContentRegistry::FindAoEProfileById(const ExternalModRuntime& runtime,
                                                                             const std::string& profileId) {
    return FindById(runtime.aoeProfiles, profileId);
}

const ExternalModMovementProfile* ExternalModContentRegistry::FindMovementProfileById(const ExternalModRuntime& runtime,
                                                                                       const std::string& profileId) {
    return FindById(runtime.movementProfiles, profileId);
}

const ExternalModAimCameraProfile* ExternalModContentRegistry::FindAimCameraProfileById(
    const ExternalModRuntime& runtime, const std::string& profileId) {
    return FindById(runtime.cameraProfiles, profileId);
}

void ExternalModContentRegistry::SortPackagesByPriority(std::vector<ExternalModPackage*>& packages) {
    std::sort(packages.begin(), packages.end(),
              [](const ExternalModPackage* lhs, const ExternalModPackage* rhs) {
                  if (lhs->manifest.loadPriority != rhs->manifest.loadPriority) {
                      return lhs->manifest.loadPriority > rhs->manifest.loadPriority;
                  }
                  return lhs->manifest.id < rhs->manifest.id;
              });
}

} // namespace SOH
