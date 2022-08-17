#pragma once

namespace RandoMain {
void GenerateRando(std::unordered_map<RandomizerSettingKey, uint8_t> cvarSettings, std::set<RandomizerCheck> excludedLocations);
}
