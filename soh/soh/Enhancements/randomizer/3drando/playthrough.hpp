#pragma once

#ifndef RANDOPLAYTHROUGH_H
#define RANDOPLAYTHROUGH_H

#include <string>
#include <set>
#include "../SeedContext.h"

namespace Playthrough {
int Playthrough_Init(uint32_t seed, std::set<RandomizerCheck> excludedLocations,
                     std::set<RandomizerTrick> enabledTricks);
int Playthrough_Repeat(std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks,
                       int count = 1);
} // namespace Playthrough
#endif //RANDOPLAYTHROUGH_H
