#pragma once

#include "../randomizerTypes.h"

#include <vector>
#include <string>

enum class SearchMode {
  ReachabilitySearch,
  GeneratePlaythrough,
  CheckBeatable,
  AllLocationsReachable,
  ValidateWorld,
  TimePassAccess,
  TempleOfTimeAccess,
  ValidStartingRegion,
  PoeCollectorAccess,
};

void ClearProgress();
void VanillaFill();
int Fill();

std::vector<RandomizerCheck> GetEmptyLocations(std::vector<RandomizerCheck> allowedLocations);

std::vector<RandomizerCheck> GetAccessibleLocations(const std::vector<RandomizerCheck>& allowedLocations,
                                             SearchMode mode = SearchMode::ReachabilitySearch, RandomizerGet ignore = RG_NONE,
                                             bool checkPoeCollectorAccess = false,
                                             bool checkOtherEntranceAccess = false);