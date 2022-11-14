#include "presets.h"
#include <variant>
#include <string>
#include <cstdint>
#include <ImGuiImpl.h>
#include <Cvar.h>
#include "soh/UIWidgets.hpp"

void clearCvars(std::vector<const char*> cvarsToClear) {
    for(const char* cvar : cvarsToClear) {
        CVar_Clear(cvar);
    }
}

void applyPreset(std::vector<PresetEntry> entries) {
    for(auto& [cvar, type, value] : entries) {
        switch (type) {
            case PRESET_ENTRY_TYPE_S32:
                CVar_SetS32(cvar, std::get<int32_t>(value));
                break;
            case PRESET_ENTRY_TYPE_FLOAT:
                CVar_SetFloat(cvar, std::get<float>(value));
                break;
            case PRESET_ENTRY_TYPE_STRING:
                CVar_SetString(cvar, std::get<const char*>(value));
                break;
        }
    }
}

void DrawPresetSelector(PresetType presetTypeId) {
    const std::string presetTypeCvar = "gPreset" + std::to_string(presetTypeId);
    const PresetTypeDefinition presetTypeDef = presetTypes.at(presetTypeId);
    // I don't love having the hardcoded length here, but combobox needs a const and this can be a dynamic length so for
    // the time being this should be the length of the largest amount of presets under any preset type + 1 for "default" option
    const char* comboboxOptions[4];
    comboboxOptions[0] = "Default";
    std::string comboboxTooltip = "Default - Reset all options to their default values.";
    for ( auto iter = presetTypeDef.presets.begin(); iter != presetTypeDef.presets.end(); ++iter ) {
        comboboxOptions[iter->first + 1] = iter->second.label;
        comboboxTooltip += "\n\n" + std::string(iter->second.label) + " - " + std::string(iter->second.description);
    }

    UIWidgets::PaddedText("Presets", false, true);
    UIWidgets::EnhancementCombobox(presetTypeCvar.c_str(), comboboxOptions, presetTypeDef.presets.size() + 1, 0);
    UIWidgets::Tooltip(comboboxTooltip.c_str());

    UIWidgets::Spacer(0);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 4.0f));
    if (ImGui::Button(("Apply Preset##" + presetTypeCvar).c_str())) {
        uint16_t presetSelectionId = CVar_GetS32(presetTypeCvar.c_str(), 0);
        clearCvars(presetTypeDef.cvarsToClear);
        if (presetSelectionId > 0) { 
            PresetDefinition presetDef = presetTypeDef.presets.at(presetSelectionId - 1);
            applyPreset(presetDef.entries);
        }
        SohImGui::RequestCvarSaveOnNextTick();
    }
    ImGui::PopStyleVar(1);
}
