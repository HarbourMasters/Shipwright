#pragma once

#include "item.hpp"

namespace RandoMain {
void GenerateRando(std::unordered_map<RandomizerSettingKey, uint8_t> cvarSettings, std::set<RandomizerCheck> excludedLocations, std::string seedInput);
std::array<Item, KEY_ENUM_MAX>* GetFullItemTable();
}
