#pragma once

#include <nlohmann/json_fwd.hpp>
#include <string>
#include <utility>
#include <vector>

enum PresetSection {
    PRESET_SECTION_SETTINGS,
    PRESET_SECTION_ENHANCEMENTS,
    PRESET_SECTION_AUDIO,
    PRESET_SECTION_COSMETICS,
    PRESET_SECTION_RANDOMIZER,
    PRESET_SECTION_TRACKERS,
    PRESET_SECTION_NETWORK,
    PRESET_SECTION_MAX,
};

void DrawPresetSelector(std::vector<PresetSection> includeSections, std::string currentIndex, bool disabled);
void applyPreset(std::string presetName, std::vector<PresetSection> includeSections = {});

// Same as applyPreset, but works on a copy of the cvar blocks instead of the live config, and only touches blocks the
// copy already holds. For callers that want a preset's result without changing the player's settings.
nlohmann::json applyPresetToBlocks(std::string presetName, nlohmann::json blocks,
                                   std::vector<PresetSection> includeSections = {});

// {display name, preset name} of every preset whose file is named "Speedrun - <display name>"
std::vector<std::pair<std::string, std::string>> GetSpeedrunPresets();
