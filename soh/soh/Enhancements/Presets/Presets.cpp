#include "Presets.h"
#include <variant>
#include <string>
#include <cstdint>
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>
#include "soh/SohGui/MenuTypes.h"
#include "soh/SohGui/SohMenu.h"
#include "soh/SohGui/SohGui.hpp"

std::string FormatLocations(std::vector<RandomizerCheck> locs) {
    std::string locString = "";
    for (auto loc: locs) {
        locString += std::to_string(loc) + ",";
    }
    return locString;
}

void applyPreset(std::vector<PresetEntry> entries) {
    for(auto& [cvar, type, value] : entries) {
        switch (type) {
            case PRESET_ENTRY_TYPE_S32:
                CVarSetInteger(cvar, std::get<int32_t>(value));
                break;
            case PRESET_ENTRY_TYPE_FLOAT:
                CVarSetFloat(cvar, std::get<float>(value));
                break;
            case PRESET_ENTRY_TYPE_STRING:
                CVarSetString(cvar, std::get<const char*>(value));
                break;
            case PRESET_ENTRY_TYPE_CPP_STRING:
                CVarSetString(cvar, std::get<std::string>(value).c_str());
                break;
        }
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    ShipInit::Init("*");
}

void DrawPresetSelector(PresetType presetTypeId) {
    const std::string presetTypeCvar = CVAR_GENERAL("SelectedPresets.") + std::to_string(presetTypeId);
    const PresetTypeDefinition presetTypeDef = presetTypes.at(presetTypeId);
    uint16_t selectedPresetId = CVarGetInteger(presetTypeCvar.c_str(), 0);
    if(selectedPresetId >= presetTypeDef.presets.size()){
        selectedPresetId = 0;
    }
    const PresetDefinition selectedPresetDef = presetTypeDef.presets.at(selectedPresetId);
    std::string comboboxTooltip = "";
    for ( auto iter = presetTypeDef.presets.begin(); iter != presetTypeDef.presets.end(); ++iter ) {
        if (iter->first != 0) comboboxTooltip += "\n\n";
        comboboxTooltip += std::string(iter->second.label) + " - " + std::string(iter->second.description);
    }

    ImGui::Text("Presets");
    UIWidgets::PushStyleCombobox(THEME_COLOR);
    if (ImGui::BeginCombo("##PresetsComboBox", selectedPresetDef.label)) {
        for ( auto iter = presetTypeDef.presets.begin(); iter != presetTypeDef.presets.end(); ++iter ) {
            if (ImGui::Selectable(iter->second.label, iter->first == selectedPresetId)) {
                CVarSetInteger(presetTypeCvar.c_str(), iter->first);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            }
        }

        ImGui::EndCombo();
    }
    UIWidgets::PopStyleCombobox();
    UIWidgets::Tooltip(comboboxTooltip.c_str());

    UIWidgets::PushStyleButton(THEME_COLOR);
    if (ImGui::Button(("Apply Preset##" + presetTypeCvar).c_str())) {
        for(const char* block : presetTypeDef.blocksToClear) {
            CVarClearBlock(block);
        }
        if (selectedPresetId != 0) {
            applyPreset(selectedPresetDef.entries);
        }
        CVarSetInteger(presetTypeCvar.c_str(), selectedPresetId);
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    UIWidgets::PopStyleButton();
}
