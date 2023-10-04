#pragma once
#include "soh/Enhancements/randomizer/item.h"
namespace RandoMain {
void GenerateRando(std::unordered_map<RandomizerSettingKey, uint8_t> cvarSettings, std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks, std::string seedInput);
}
