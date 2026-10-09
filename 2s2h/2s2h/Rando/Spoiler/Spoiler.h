#ifndef RANDO_SPOILER_H
#define RANDO_SPOILER_H

#include <vector>
#include <string>
#include "nlohmann/json.hpp"

namespace Rando {

namespace Spoiler {

extern std::vector<std::string> spoilerOptions;
void RefreshOptions();
void SelectSpoiler(s32 index);
nlohmann::json GenerateFromSaveContext();
void SaveToFile(const std::string& fileName, nlohmann::json spoiler);
nlohmann::json LoadFromFile(const std::string& filePath);
void ApplyToSaveContext(nlohmann::json spoiler);
bool HandleFileDropped(char* path);

} // namespace Spoiler

} // namespace Rando

#endif
