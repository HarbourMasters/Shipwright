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

#define RESET   "\x1b[0m"
#define DIM     "\x1b[2m"

#define BLACK   "\x1b[30m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MEGANTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1b[37m"

std::string GenerateRandomizer(std::unordered_map<RandomizerSettingKey, uint8_t> cvarSetting, std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks, std::string seedInput);