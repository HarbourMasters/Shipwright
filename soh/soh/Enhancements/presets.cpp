#include "presets.h"
#include <variant>
#include <string>
#include <cstdint>
#include <libultraship/bridge.h>
#include "soh/UIWidgets.hpp"
#include <libultraship/libultraship.h>

#include "soh/SohMenuBar.h"

void clearCvars(std::vector<const char*> cvarsToClear) {
    for(const char* cvar : cvarsToClear) {
        CVarClear(cvar);
    }
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
        }
    }
}

void DrawPresetSelector(PresetType presetTypeId) {
    const std::string presetTypeCvar = "gPreset" + std::to_string(presetTypeId);
    const PresetTypeDefinition presetTypeDef = presetTypes.at(presetTypeId);
    const uint16_t selectedPresetId = CVarGetInteger(presetTypeCvar.c_str(), 0);
    const PresetDefinition selectedPresetDef = presetTypeDef.presets.at(selectedPresetId);
    
    CVarSetInteger(presetTypeCvar.c_str(), CVarGetInteger("gPreset", 0));
    
    if (UIWidgets::Button("Apply Preset", {
        .color = colorChoice,    
    })) {
        clearCvars(presetTypeDef.cvarsToClear);
        if (selectedPresetId != 0) {
            applyPreset(selectedPresetDef.entries);
        }
        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
}
