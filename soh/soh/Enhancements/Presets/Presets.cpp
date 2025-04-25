#include "Presets.h"
#include <variant>
#include <string>
#include <cstdint>
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>
#include "soh/SohGui/MenuTypes.h"
#include "soh/SohGui/SohMenu.h"
#include "soh/SohGui/SohGui.hpp"

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
}

void BlankButton() {
    ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0, 0, 0, 0});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0, 0, 0, 0});
    ImGui::PushStyleColor(ImGuiCol_Border, {0, 0, 0, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 8.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 5.0f);
}

void PresetCheckboxStyle(const ImVec4& color) {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(color.x, color.y, color.z, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(color.x, color.y, color.z, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(color.x, color.y, color.z, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1.0f, 1.0f, 1.0f, 0.7f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 5.0f);
}

void applyPreset(std::vector<PresetEntry> entries) {
    for (auto& [cvar, type, value] : entries) {
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
    if (selectedPresetId >= presetTypeDef.presets.size()) {
        selectedPresetId = 0;
    }
    const PresetDefinition selectedPresetDef = presetTypeDef.presets.at(selectedPresetId);
    std::string comboboxTooltip = "";
    for (auto iter = presetTypeDef.presets.begin(); iter != presetTypeDef.presets.end(); ++iter) {
        if (iter->first != 0)
            comboboxTooltip += "\n\n";
        comboboxTooltip += std::string(iter->second.label) + " - " + std::string(iter->second.description);
    }

    ImGui::Text("Presets");
    UIWidgets::PushStyleCombobox(THEME_COLOR);
    if (ImGui::BeginCombo("##PresetsComboBox", selectedPresetDef.label)) {
        for (auto iter = presetTypeDef.presets.begin(); iter != presetTypeDef.presets.end(); ++iter) {
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
        for (const char* block : presetTypeDef.blocksToClear) {
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

enum PresetSection {
    PRESET_SECTION_SETTINGS,
    PRESET_SECTION_ENHANCEMENTS,
    PRESET_SECTION_AUDIO,
    PRESET_SECTION_COSMETICS,
    PRESET_SECTION_RANDOMIZER,
    PRESET_SECTION_TRACKERS,
};

struct PresetInfo {
    std::unordered_map<std::string, CVarVariant> settings;
    std::unordered_map<std::string, CVarVariant> enhancements;
    std::unordered_map<std::string, CVarVariant> audio;
    std::unordered_map<std::string, CVarVariant> cosmetics;
    std::unordered_map<std::string, CVarVariant> rando;
    std::unordered_map<std::string, CVarVariant> trackers;

    bool applySettings = true, applyEnhancements = true, applyAudio = true, applyCosmetics = true, applyRando = true, applyTrackers = true;
};

static std::unordered_map<std::string, PresetInfo> presets = {
    { "Vanilla Plus", {
        {},
        {{"DpadEquips", 1}},
    {},
    {},
    {},
    {},
    }}
};

void PresetsCustomWidget(WidgetInfo& info) {
    UIWidgets::PushStyleTabs(THEME_COLOR);
    ImGui::PushFont(OTRGlobals::Instance->fontMonoLargest);
    if (ImGui::BeginTable("PresetWidgetTable", 9)) {
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Settings");
        ImGui::TableSetupColumn("Enhancements");
        ImGui::TableSetupColumn("Audio");
        ImGui::TableSetupColumn("Cosmetics");
        ImGui::TableSetupColumn("Randomizer");
        ImGui::TableSetupColumn("Trackers");
        ImGui::TableSetupColumn("Apply", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Delete").x + ImGui::GetStyle().FramePadding.x * 2);
        ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Delete").x + ImGui::GetStyle().FramePadding.x * 2);
        BlankButton();
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TableNextColumn();
        ImGui::Button((ICON_FA_COG + std::string("##") + "headersettings").c_str());
        UIWidgets::Tooltip("Settings");
        ImGui::TableNextColumn();
        ImGui::Button((ICON_FA_PLUS_CIRCLE + std::string("##") + "headerenhancements").c_str());
        UIWidgets::Tooltip("Enhancements");
        ImGui::TableNextColumn();
        ImGui::Button((ICON_FA_MUSIC + std::string("##") + "headeraudio").c_str());
        UIWidgets::Tooltip("Audio");
        ImGui::TableNextColumn();
        ImGui::Button((ICON_FA_PAINT_BRUSH + std::string("##") + "headercosmetics").c_str());
        UIWidgets::Tooltip("Cosmetics");
        ImGui::TableNextColumn();
        ImGui::Button((ICON_FA_RANDOM + std::string("##") + "headerrando").c_str());
        UIWidgets::Tooltip("Randomizer");
        ImGui::TableNextColumn();
        ImGui::Button((ICON_FA_MAP + std::string("##") + "headertrackers").c_str());
        UIWidgets::Tooltip("Trackers");
        UIWidgets::PopStyleButton();

        UIWidgets::CheckboxOptions checkboxOptions;
        checkboxOptions.DefaultValue(true).Padding({ 10.0f, 6.0f }).Color(THEME_COLOR);

        for (auto [name, info] : presets) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Text(name.c_str());
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            if (info.settings.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 0.7f });
                BlankButton();
                ImGui::BeginDisabled();
                ImGui::Button((ICON_FA_TIMES + std::string("##") + name + "settings").c_str());
                ImGui::EndDisabled();
                UIWidgets::PopStyleButton();
                ImGui::PopStyleColor();
            }
            else {
                UIWidgets::Checkbox(("##" + name + "settings").c_str(), &info.applySettings, UIWidgets::CheckboxOptions().DefaultValue(true));
            }
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            if (info.enhancements.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 0.7f });
                BlankButton();
                ImGui::BeginDisabled();
                ImGui::Button((ICON_FA_TIMES + std::string("##") + name + "enhancements").c_str());
                ImGui::EndDisabled();
                UIWidgets::PopStyleButton();
                ImGui::PopStyleColor();
            }
            else {
                checkboxOptions.DefaultValue(info.applyEnhancements);
                ImGui::PushFont(OTRGlobals::Instance->fontMono);
                UIWidgets::Checkbox(("##" + name + "enhancements").c_str(), &info.applyEnhancements, checkboxOptions);
                ImGui::PopFont();
            }
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            if (info.audio.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 0.7f });
                BlankButton();
                ImGui::BeginDisabled();
                ImGui::Button((ICON_FA_TIMES + std::string("##") + name + "audio").c_str());
                ImGui::EndDisabled();
                UIWidgets::PopStyleButton();
                ImGui::PopStyleColor();
            }
            else {
                UIWidgets::Checkbox(("##" + name + "audio").c_str(), &info.applyAudio, UIWidgets::CheckboxOptions().DefaultValue(true));
            }
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            if (info.cosmetics.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 0.7f });
                BlankButton();
                ImGui::BeginDisabled();
                ImGui::Button((ICON_FA_TIMES + std::string("##") + name + "cosmetics").c_str());
                ImGui::EndDisabled();
                UIWidgets::PopStyleButton();
                ImGui::PopStyleColor();
            }
            else {
                UIWidgets::Checkbox(("##" + name + "cosmetics").c_str(), &info.applyCosmetics, UIWidgets::CheckboxOptions().DefaultValue(true));
            }
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            if (info.rando.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 0.7f });
                BlankButton();
                ImGui::BeginDisabled();
                ImGui::Button((ICON_FA_TIMES + std::string("##") + name + "rando").c_str());
                ImGui::EndDisabled();
                UIWidgets::PopStyleButton();
                ImGui::PopStyleColor();
            }
            else {
                UIWidgets::Checkbox(("##" + name + "rando").c_str(), &info.applyRando, UIWidgets::CheckboxOptions().DefaultValue(true));
            }
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            if (info.trackers.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 0.7f });
                BlankButton();
                ImGui::BeginDisabled();
                ImGui::Button((ICON_FA_TIMES + std::string("##") + name + "trackers").c_str());
                ImGui::EndDisabled();
                UIWidgets::PopStyleButton();
                ImGui::PopStyleColor();
            }
            else {
                UIWidgets::Checkbox(("##" + name + "trackers").c_str(), &info.applyTrackers, UIWidgets::CheckboxOptions().DefaultValue(true));
            }
            ImGui::TableNextColumn();
            UIWidgets::PushStyleButton(THEME_COLOR);
            if (UIWidgets::Button(("Apply##" + name).c_str())) {

            }
            UIWidgets::PopStyleButton();
            ImGui::TableNextColumn();
            UIWidgets::PushStyleButton(THEME_COLOR);
            if (UIWidgets::Button(("Delete##" + name).c_str())) {
                presets.erase(name);
            }
            UIWidgets::PopStyleButton();
        }

        ImGui::EndTable();
    }
    ImGui::PopFont();
    UIWidgets::PopStyleTabs();
}

void RegisterPresetsWidgets() {
    SohGui::mSohMenu->AddSidebarEntry("Settings", "Presets", 1);
    WidgetPath path = { "Settings", "Presets", SECTION_COLUMN_1 };
    SohGui::mSohMenu->AddWidget(path, "PresetsWidget", WIDGET_CUSTOM)
        .CustomFunction(PresetsCustomWidget);
}

//static RegisterMenuUpdateFunc updateFunc(UpdateResolutionVars, "Settings", "General");
static RegisterMenuInitFunc initFunc(RegisterPresetsWidgets);
