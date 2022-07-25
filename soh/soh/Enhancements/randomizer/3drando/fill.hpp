#pragma once

#include "keys.hpp"

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

std::vector<uint32_t> GetAccessibleLocations(const std::vector<uint32_t>& allowedLocations,
                                             SearchMode mode = SearchMode::ReachabilitySearch, std::string ignore = "",
                                             bool checkPoeCollectorAccess = false,
                                             bool checkOtherEntranceAccess = false);
