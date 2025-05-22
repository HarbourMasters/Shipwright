#pragma once

#include <string>
#include <unordered_map>
#include <set>
#include "soh/Enhancements/randomizer/randomizerTypes.h"

#define OPTION_SUB_MENU 1
#define SUB_MENU 2
#define GENERATE_MODE 3
#define LOAD_PRESET 4
#define SAVE_PRESET 5
#define DELETE_PRESET 6
#define RESET_TO_DEFAULTS 8

bool GenerateRandomizer(std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks,
                        std::string seedInput);