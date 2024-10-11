#pragma once
#include "soh/Enhancements/randomizer/item.h"

#include <set>
namespace RandoMain {
void GenerateRando(std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks, std::string seedInput);
}
