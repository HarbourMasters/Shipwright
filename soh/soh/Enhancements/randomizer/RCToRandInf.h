#pragma once

#include <map>
#include "soh/Enhancements/randomizer/location.h"
#include "soh/Enhancements/randomizer/randomizerEnums.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"

// There has been some talk about potentially just using the RC identifier to store flags rather than randomizer inf, so
// for now we're not going to store randomzierInf in the randomizer check objects, we're just going to map them 1:1 here
extern std::map<RandomizerCheck, RandomizerInf> rcToRandomizerInf;

bool IdentifyCheck(CheckIdentity* id, Rando::Location* loc, bool allowUnknown = true);