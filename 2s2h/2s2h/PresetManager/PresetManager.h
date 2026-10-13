
#ifndef PRESET_MANAGER_H
#define PRESET_MANAGER_H

#include <string>
#include <vector>

bool PresetManager_HandleFileDropped(const std::string& filePath);
void PresetManager_Draw();

std::vector<std::string> PresetManager_GetPresetNames();
bool PresetManager_ApplyPresetByName(const std::string& name);

#endif // PRESET_MANAGER_H
