#pragma once

#include <string>
#include <vector>

#include "ExternalModTypes.h"

namespace SOH {

class ExternalModContentRegistry {
  public:
    static const ExternalModItemDefinition* FindItemDefinitionById(const ExternalModRuntime& runtime,
                                                                   const std::string& itemId);
    static const ExternalModStatusDefinition* FindStatusDefinitionById(const ExternalModRuntime& runtime,
                                                                       const std::string& statusId);
    static const ExternalModDamageProfile* FindDamageProfileById(const ExternalModRuntime& runtime,
                                                                 const std::string& profileId);
    static const ExternalModTargetingProfile* FindTargetingProfileById(const ExternalModRuntime& runtime,
                                                                       const std::string& profileId);
    static const ExternalModItemUseProfile* FindItemUseProfileById(const ExternalModRuntime& runtime,
                                                                   const std::string& profileId);
    static const ExternalModProjectileProfile* FindProjectileProfileById(const ExternalModRuntime& runtime,
                                                                         const std::string& profileId);
    static const ExternalModAoEProfile* FindAoEProfileById(const ExternalModRuntime& runtime,
                                                           const std::string& profileId);
    static const ExternalModMovementProfile* FindMovementProfileById(const ExternalModRuntime& runtime,
                                                                     const std::string& profileId);
    static const ExternalModAimCameraProfile* FindAimCameraProfileById(const ExternalModRuntime& runtime,
                                                                       const std::string& profileId);

    // Sort by loadPriority DESC and modId ASC (deterministic tie-breaker).
    static void SortPackagesByPriority(std::vector<ExternalModPackage*>& packages);
};

} // namespace SOH
