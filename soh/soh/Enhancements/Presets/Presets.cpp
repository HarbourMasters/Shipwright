#include "Presets.h"
#include <variant>
#include <string>
#include <cstdint>
#include <fstream>
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>
#include <spdlog/fmt/fmt.h>
#include "soh/SohGui/MenuTypes.h"
#include "soh/SohGui/SohMenu.h"
#include "soh/SohGui/SohGui.hpp"

namespace fs = std::filesystem;

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
}

void BlankButton() {
    ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0, 0, 0, 0 });
    ImGui::PushStyleColor(ImGuiCol_Border, { 0, 0, 0, 0 });
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
    nlohmann::json presetValues;
    std::string fileName;
    bool applySettings = true, applyEnhancements = true, applyAudio = true, applyCosmetics = true, applyRando = true,
         applyTrackers = true;
};

static std::map<std::string, PresetInfo> presets;
static std::string presetFolder;

std::string FormatPresetPath(std::string name) {
    return fmt::format("{}/{}.json", presetFolder, name);
}

void DrawSectionCheck(const std::string& name, bool empty, bool* pointer, std::string section) {
    ImGui::AlignTextToFramePadding();
    if (empty) {
        ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 0.7f });
        BlankButton();
        ImGui::BeginDisabled();
        ImGui::Button((ICON_FA_TIMES + std::string("##") + name + section).c_str());
        ImGui::EndDisabled();
        UIWidgets::PopStyleButton();
        ImGui::PopStyleColor();
    } else {
        ImGui::PushFont(OTRGlobals::Instance->fontMono);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetStyle().FramePadding.y));
        UIWidgets::Checkbox(("##" + name + section).c_str(), pointer,
                            { .defaultValue = true, .padding = { 6.0f, 6.0f }, .color = THEME_COLOR });
        ImGui::PopFont();
    }
}

void LoadPresets() {
    if (!fs::exists(presetFolder)) {
        return;
    }
    for (auto const& preset : fs::directory_iterator(presetFolder)) {
        std::ifstream ifs(preset.path());

        auto json = nlohmann::json::parse(ifs);
        try {
            if (!json.contains("presetName")) {
                spdlog::error(fmt::format("Attempted to load file {} as a preset, but was not a preset file.",
                                          preset.path().filename().string()));
                return;
            }
            presets[json["presetName"]].presetValues = json;
            presets[json["presetName"]].fileName = preset.path().filename().stem().string();
        } catch (...) {}
        ifs.close();
    }
}

void SavePreset(std::string& presetName) {
    if (!fs::exists(presetFolder)) {
        fs::create_directory(presetFolder);
    }
    presets[presetName].presetValues["presetName"] = presetName;
    std::ofstream file(fmt::format("{}/{}.json", presetFolder, presetName));
    file << presets[presetName].presetValues.dump(4);
    file.close();
}

std::vector<std::string> sections = {
    CVAR_PREFIX_SETTING, CVAR_PREFIX_WINDOW,   CVAR_PREFIX_ENHANCEMENT,        CVAR_PREFIX_RANDOMIZER_ENHANCEMENT,
    CVAR_PREFIX_AUDIO,   CVAR_PREFIX_COSMETIC, CVAR_PREFIX_RANDOMIZER_SETTING, CVAR_PREFIX_TRACKER,
    CVAR_PREFIX_CHEAT
};

static std::string newPresetName;
static bool newPresetSettings = true, newPresetEnhancements = true, newPresetAudio = true, newPresetCosmetics = true,
            newPresetRando = true, newPresetTrackers = true;

void PresetsCustomWidget(WidgetInfo& info) {
    ImGui::PushFont(OTRGlobals::Instance->fontMonoLargest);
    if (UIWidgets::Button("New Preset", UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline).Color(THEME_COLOR))) {
        ImGui::OpenPopup("newPreset");
    }
    ImGui::SetNextWindowSize({ 400, 400 });
    if (ImGui::BeginPopup("newPreset", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize |
                                           ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                           ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar)) {
        bool nameExists = presets.contains(newPresetName);
        UIWidgets::InputString("Preset Name", &newPresetName,
                               UIWidgets::InputOptions()
                                   .Color(THEME_COLOR)
                                   .Size({ 200, 40 })
                                   .ComponentAlignment(UIWidgets::ComponentAlignments::Right)
                                   .LabelPosition(UIWidgets::LabelPositions::Near)
                                   .ErrorText("Preset name already exists")
                                   .HasError(nameExists));
        nameExists = presets.contains(newPresetName);
        bool noneSelected = !newPresetSettings && !newPresetEnhancements && !newPresetAudio && !newPresetCosmetics &&
                            !newPresetRando && !newPresetTrackers;
        const char* disabledTooltip =
            (newPresetName.empty() ? "Preset name is empty"
                                   : (noneSelected ? "No sections selected" : "Preset name already exists"));
        UIWidgets::Checkbox("Save Settings", &newPresetSettings,
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR).Padding({ 6.0f, 6.0f }));
        UIWidgets::Checkbox("Save Enhancements", &newPresetEnhancements,
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR).Padding({ 6.0f, 6.0f }));
        UIWidgets::Checkbox("Save Audio", &newPresetAudio,
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR).Padding({ 6.0f, 6.0f }));
        UIWidgets::Checkbox("Save Cosmetics", &newPresetCosmetics,
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR).Padding({ 6.0f, 6.0f }));
        UIWidgets::Checkbox("Save Rando Settings", &newPresetRando,
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR).Padding({ 6.0f, 6.0f }));
        UIWidgets::Checkbox("Save Trackers", &newPresetTrackers,
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR).Padding({ 6.0f, 6.0f }));
        if (UIWidgets::Button(
                "Save", UIWidgets::ButtonOptions({ .disabled = nameExists || noneSelected || newPresetName.empty(),
                                                   .disabledTooltip = disabledTooltip })
                            .Padding({ 6.0f, 6.0f })
                            .Color(THEME_COLOR))) {
            presets[newPresetName] = {};
            auto config = Ship::Context::GetInstance()->GetConfig()->GetNestedJson();
            if (newPresetSettings) {
                presets[newPresetName].presetValues["blocks"]["settings"][CVAR_PREFIX_SETTING] =
                    config["CVars"][CVAR_PREFIX_SETTING];
                presets[newPresetName].presetValues["blocks"]["windows"][CVAR_PREFIX_WINDOW] =
                    config["CVars"][CVAR_PREFIX_WINDOW];
            }
            if (newPresetEnhancements) {
                presets[newPresetName].presetValues["blocks"]["enhancements"][CVAR_PREFIX_ENHANCEMENT] =
                    config["CVars"][CVAR_PREFIX_ENHANCEMENT];
                presets[newPresetName].presetValues["blocks"]["randoEnhancements"][CVAR_PREFIX_RANDOMIZER_ENHANCEMENT] =
                    config["CVars"][CVAR_PREFIX_RANDOMIZER_ENHANCEMENT];
            }
            if (newPresetAudio) {
                presets[newPresetName].presetValues["blocks"]["audio"][CVAR_PREFIX_AUDIO] =
                    config["CVars"][CVAR_PREFIX_AUDIO];
            }
            if (newPresetCosmetics) {
                presets[newPresetName].presetValues["blocks"]["cosmetics"][CVAR_PREFIX_COSMETIC] =
                    config["CVars"][CVAR_PREFIX_COSMETIC];
            }
            if (newPresetRando) {
                presets[newPresetName].presetValues["blocks"]["rando"][CVAR_PREFIX_RANDOMIZER_SETTING] =
                    config["CVars"][CVAR_PREFIX_RANDOMIZER_SETTING];
            }
            if (newPresetTrackers) {
                presets[newPresetName].presetValues["blocks"]["trackers"][CVAR_PREFIX_TRACKER] =
                    config["CVars"][CVAR_PREFIX_TRACKER];
            }
            presets[newPresetName].fileName = newPresetName;
            SavePreset(newPresetName);
            newPresetName = "";
            ImGui::CloseCurrentPopup();
        }
        if (UIWidgets::Button("Cancel", UIWidgets::ButtonOptions().Padding({ 6.0f, 6.0f }).Color(THEME_COLOR))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    UIWidgets::PushStyleTabs(THEME_COLOR);
    if (ImGui::BeginTable("PresetWidgetTable", 9)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 200);
        ImGui::TableSetupColumn("Settings");
        ImGui::TableSetupColumn("Enhancements");
        ImGui::TableSetupColumn("Audio");
        ImGui::TableSetupColumn("Cosmetics");
        ImGui::TableSetupColumn("Randomizer");
        ImGui::TableSetupColumn("Trackers");
        ImGui::TableSetupColumn("Apply", ImGuiTableColumnFlags_WidthFixed,
                                ImGui::CalcTextSize("Apply").x + ImGui::GetStyle().FramePadding.x * 2);
        ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed,
                                ImGui::CalcTextSize("Delete").x + ImGui::GetStyle().FramePadding.x * 2);
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

        if (presets.empty()) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("No presets found.");
            ImGui::EndTable();
            UIWidgets::PopStyleTabs();
            ImGui::PopFont();
            return;
        }
        for (auto& [name, info] : presets) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Text(name.c_str());
            ImGui::TableNextColumn();
            DrawSectionCheck(name, !info.presetValues["blocks"].contains("settings"), &info.applySettings, "settings");
            ImGui::TableNextColumn();
            DrawSectionCheck(name, !info.presetValues["blocks"].contains("enhancements"), &info.applyEnhancements,
                             "enhancements");
            ImGui::TableNextColumn();
            DrawSectionCheck(name, !info.presetValues["blocks"].contains("audio"), &info.applyAudio, "audio");
            ImGui::TableNextColumn();
            DrawSectionCheck(name, !info.presetValues["blocks"].contains("cosmetics"), &info.applyCosmetics,
                             "cosmetics");
            ImGui::TableNextColumn();
            DrawSectionCheck(name, !info.presetValues["blocks"].contains("rando"), &info.applyRando, "rando");
            ImGui::TableNextColumn();
            DrawSectionCheck(name, !info.presetValues["blocks"].contains("trackers"), &info.applyTrackers, "trackers");
            ImGui::TableNextColumn();
            UIWidgets::PushStyleButton(THEME_COLOR);
            if (UIWidgets::Button(("Apply##" + name).c_str(), UIWidgets::ButtonOptions().Padding({ 6.0f, 6.0f }))) {
                for (auto& section : info.presetValues["blocks"]) {
                    for (auto& item : section.items()) {
                        if (section[item.key()].is_null()) {
                            CVarClearBlock(item.key().c_str());
                        } else {
                            Ship::Context::GetInstance()->GetConfig()->SetBlock(
                                fmt::format("{}.{}", "CVars", item.key()), item.value());
                            Ship::Context::GetInstance()->GetConsoleVariables()->Load();
                        }
                    }
                }
            }
            UIWidgets::PopStyleButton();
            ImGui::TableNextColumn();
            UIWidgets::PushStyleButton(THEME_COLOR);
            if (UIWidgets::Button(("Delete##" + name).c_str(), UIWidgets::ButtonOptions().Padding({ 6.0f, 6.0f }))) {
                auto path = FormatPresetPath(info.fileName);
                if (fs::exists(path)) {
                    fs::remove(path);
                }
                presets.erase(name);
                UIWidgets::PopStyleButton();
                break;
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
    SohGui::mSohMenu->AddWidget(path, "PresetsWidget", WIDGET_CUSTOM).CustomFunction(PresetsCustomWidget);
    presetFolder = Ship::Context::GetInstance()->GetPathRelativeToAppDirectory("presets");
    LoadPresets();
}

// static RegisterMenuUpdateFunc updateFunc(UpdateResolutionVars, "Settings", "General");
static RegisterMenuInitFunc initFunc(RegisterPresetsWidgets);
