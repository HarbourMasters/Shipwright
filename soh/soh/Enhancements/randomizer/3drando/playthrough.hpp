#pragma once
#include <string>
#include <set>
#include "../context.h"

namespace Playthrough {
    int Playthrough_Init(uint32_t seed, std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks);
    int Playthrough_Repeat(std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks, int count = 1);
}
