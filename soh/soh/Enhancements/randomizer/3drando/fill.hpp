#pragma once

#include "../randomizerTypes.h"

#include <vector>
#include <string>

void ClearProgress();
void VanillaFill();
int Fill();

std::vector<RandomizerCheck> GetEmptyLocations(std::vector<RandomizerCheck> allowedLocations);

std::vector<RandomizerCheck> ReachabilitySearch(const std::vector<RandomizerCheck>& allowedLocations, RandomizerGet ignore=RG_NONE);

void GeneratePlaythrough();

bool CheckBeatable(RandomizerGet ignore=RG_NONE);

void ValidateEntrances(bool checkPoeCollectorAccess, bool checkOtherEntranceAccess);