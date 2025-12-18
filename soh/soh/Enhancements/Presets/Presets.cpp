#include "Presets.h"
#include <variant>
#include <string>
#include <fstream>
#include <ship/config/Config.h>
#include <libultraship/classes.h>
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include <ship/resource/type/Json.h>
#include "soh/OTRGlobals.h"
#include "soh/SohGui/MenuTypes.h"
#include "soh/SohGui/SohMenu.h"
#include "soh/SohGui/SohGui.hpp"
#include "soh/Enhancements/randomizer/randomizer_settings_window.h"
#include "soh/Enhancements/randomizer/randomizer_check_tracker.h"
#include "soh/Enhancements/randomizer/randomizer_entrance_tracker.h"
#include "soh/Enhancements/randomizer/randomizer_item_tracker.h"

namespace fs = std::filesystem;

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
extern std::shared_ptr<RandomizerSettingsWindow> mRandomizerSettingsWindow;
} // namespace SohGui

struct PresetInfo {
    nlohmann::json presetValues;
    std::string fileName;
    bool apply[PRESET_SECTION_MAX];
    bool isBuiltIn = false;
};

struct BlockInfo {
    std::vector<std::string> sections;
    const char* icon;
    std::string names[2];
};

static std::map<std::string, PresetInfo> presets;
static std::string presetFolder;

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

static BlockInfo blockInfo[PRESET_SECTION_MAX] = {
    { { CVAR_PREFIX_SETTING, CVAR_PREFIX_WINDOW, CVAR_PREFIX_GAMEPLAY_STATS },
      ICON_FA_COG,
      { "Settings", "settings" } },
    { { CVAR_PREFIX_ENHANCEMENT, CVAR_PREFIX_RANDOMIZER_ENHANCEMENT, CVAR_PREFIX_CHEAT },
      ICON_FA_PLUS_CIRCLE,
      { "Enhancements", "enhancements" } },
    { { CVAR_PREFIX_AUDIO }, ICON_FA_MUSIC, { "Audio", "audio" } },
    { { CVAR_PREFIX_COSMETIC }, ICON_FA_PAINT_BRUSH, { "Cosmetics", "cosmetics" } },
    { { CVAR_PREFIX_RANDOMIZER_SETTING }, ICON_FA_RANDOM, { "Rando Settings", "rando" } },
    { { CVAR_PREFIX_TRACKER }, ICON_FA_MAP, { "Trackers", "trackers" } },
    { { CVAR_PREFIX_REMOTE }, ICON_FA_WIFI, { "Network", "network" } },
};

std::string FormatPresetPath(std::string name) {
    return fmt::format("{}/{}.json", presetFolder, name);
}

void applyPreset(std::string presetName, std::vector<PresetSection> includeSections) {
    auto& info = presets[presetName];
    for (int i = PRESET_SECTION_SETTINGS; i < PRESET_SECTION_MAX; i++) {
        if (info.apply[i] && info.presetValues["blocks"].contains(blockInfo[i].names[1])) {
            if (!includeSections.empty() &&
                std::find(includeSections.begin(), includeSections.end(), i) == includeSections.end()) {
                continue;
            }
            if (i == PRESET_SECTION_TRACKERS) {
                ItemTracker_LoadFromPreset(info.presetValues["blocks"][blockInfo[i].names[1]]["windows"]);
                if (info.presetValues["blocks"][blockInfo[i].names[1]]["windows"].contains("Check Tracker")) {
                    CheckTracker::CheckTracker_LoadFromPreset(
                        info.presetValues["blocks"][blockInfo[i].names[1]]["windows"]["Check Tracker"]);
                }
                if (info.presetValues["blocks"][blockInfo[i].names[1]]["windows"].contains("Entrance Tracker")) {
                    EntranceTracker_LoadFromPreset(
                        info.presetValues["blocks"][blockInfo[i].names[1]]["windows"]["Entrance Tracker"]);
                }
            }
            auto section = info.presetValues["blocks"][blockInfo[i].names[1]];
            std::string sectionStrategy = "overwrite";
            if (info.presetValues.contains("blockStrategy") &&
                info.presetValues["blockStrategy"].contains(blockInfo[i].names[1])) {
                sectionStrategy = info.presetValues["blockStrategy"][blockInfo[i].names[1]];
            }

            for (auto& item : section.items()) {
                if (section[item.key()].is_null()) {
                    CVarClearBlock(item.key().c_str());
                } else {
                    auto block = item.value();
                    if (sectionStrategy == "merge") {
                        auto currentJson = Ship::Context::GetInstance()->GetConfig()->GetNestedJson();
                        if (currentJson.contains("CVars") && currentJson["CVars"].contains(item.key())) {
                            block = currentJson["CVars"][item.key()];
                            // Recursively merge the two json objects
                            block.update(item.value(), true);
                        }
                    }

                    Ship::Context::GetInstance()->GetConfig()->SetBlock(fmt::format("{}.{}", "CVars", item.key()),
                                                                        block);
                    Ship::Context::GetInstance()->GetConsoleVariables()->Load();
                }
            }
            if (i == PRESET_SECTION_RANDOMIZER) {
                SohGui::mRandomizerSettingsWindow->SetNeedsUpdate();
            }
        }
    }
    ShipInit::InitAll();
    OTRGlobals::Instance->ScaleImGui();
}

void DrawPresetSelector(std::vector<PresetSection> includeSections, std::string presetLoc, bool disabled) {
    std::vector<std::string> includedPresets;
    for (auto& [name, info] : presets) {
        for (auto& section : includeSections) {
            if (info.apply[section]) {
                includedPresets.push_back(name);
            }
        }
    }
    ImGui::Text("Presets");
    if (includedPresets.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Text, UIWidgets::ColorValues.at(UIWidgets::Colors::Orange));
        ImGui::Text("No presets with rando options. Make some in Settings -> Presets");
        ImGui::PopStyleColor();
        return;
    }
    std::string selectorCvar = fmt::format(CVAR_GENERAL("{}SelectedPreset"), presetLoc);
    std::string currentIndex = CVarGetString(selectorCvar.c_str(), includedPresets[0].c_str());
    if (!presets.contains(currentIndex)) {
        currentIndex = *includedPresets.begin();
        CVarSetString(selectorCvar.c_str(), currentIndex.c_str());
    }
    UIWidgets::PushStyleCombobox(THEME_COLOR);
    if (ImGui::BeginCombo("##PresetsComboBox", currentIndex.c_str())) {
        for (auto iter = includedPresets.begin(); iter != includedPresets.end(); ++iter) {
            if (ImGui::Selectable(iter->c_str(), *iter == currentIndex)) {
                CVarSetString(selectorCvar.c_str(), iter->c_str());
                currentIndex = *iter;
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            }
        }

        ImGui::EndCombo();
    }
    UIWidgets::PopStyleCombobox();
    // UIWidgets::Tooltip(comboboxTooltip.c_str());

    UIWidgets::PushStyleButton(THEME_COLOR);
    if (UIWidgets::Button(
            ("Apply Preset##" + selectorCvar).c_str(),
            UIWidgets::ButtonOptions({ { .disabled = disabled } }).Color(THEME_COLOR).Size(UIWidgets::Sizes::Inline))) {
        applyPreset(currentIndex, includeSections);
    }
    UIWidgets::PopStyleButton();
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

void ParsePreset(nlohmann::json& json, std::string name) {
    try {
        presets[json["presetName"]].presetValues = json;
        presets[json["presetName"]].fileName = name;
        if (json.contains("isBuiltIn")) {
            presets[json["presetName"]].isBuiltIn = json["isBuiltIn"];
        }
        for (int i = 0; i < PRESET_SECTION_MAX; i++) {
            if (presets[json["presetName"]].presetValues["blocks"].contains(blockInfo[i].names[1])) {
                presets[json["presetName"]].apply[i] = true;
            }
        }
    } catch (...) {}
}

void LoadPresets() {
    if (!presets.empty()) {
        presets.clear();
    }
    if (fs::exists(presetFolder)) {
        for (auto const& preset : fs::directory_iterator(presetFolder)) {
            std::ifstream ifs(preset.path());

            auto json = nlohmann::json::parse(ifs);
            if (!json.contains("presetName")) {
                spdlog::error(fmt::format("Attempted to load file {} as a preset, but was not a preset file.",
                                          preset.path().filename().string()));
            } else {
                ParsePreset(json, preset.path().filename().stem().string());
            }
            ifs.close();
        }
    }
    auto initData = std::make_shared<Ship::ResourceInitData>();
    initData->Format = RESOURCE_FORMAT_BINARY;
    initData->Type = static_cast<uint32_t>(Ship::ResourceType::Json);
    initData->ResourceVersion = 0;
    std::string folder = "presets/*";
    auto builtIns = Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->ListFiles(folder);
    size_t start = std::string(folder).size() - 1;
    for (size_t i = 0; i < builtIns->size(); i++) {
        std::string filePath = builtIns->at(i);
        auto json = std::static_pointer_cast<Ship::Json>(
            Ship::Context::GetInstance()->GetResourceManager()->LoadResource(filePath, true, initData));

        std::string fileName = filePath.substr(start, filePath.size() - start - 5); // 5 for length of ".json"
        ParsePreset(json->Data, fileName);
    }
}

void SavePreset(std::string& presetName) {
    if (!fs::exists(presetFolder)) {
        fs::create_directory(presetFolder);
    }
    presets[presetName].presetValues["presetName"] = presetName;
    presets[presetName].presetValues["fileType"] = FILE_TYPE_PRESET;
    std::ofstream file(
        fmt::format("{}/{}.json", Ship::Context::GetInstance()->LocateFileAcrossAppDirs("presets"), presetName));
    file << presets[presetName].presetValues.dump(4);
    file.close();
    LoadPresets();
}

static std::string newPresetName;
static bool saveSection[PRESET_SECTION_MAX];

void DrawNewPresetPopup() {
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
    bool noneSelected = true;
    for (int i = PRESET_SECTION_SETTINGS; i < PRESET_SECTION_MAX; i++) {
        if (saveSection[i]) {
            noneSelected = false;
            break;
        }
    }
    const char* disabledTooltip =
        (newPresetName.empty() ? "Preset name is empty"
                               : (noneSelected ? "No sections selected" : "Preset name already exists"));
    for (int i = PRESET_SECTION_SETTINGS; i < PRESET_SECTION_MAX; i++) {
        UIWidgets::Checkbox(fmt::format("Save {}", blockInfo[i].names[0]).c_str(), &saveSection[i],
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR).Padding({ 6.0f, 6.0f }));
    }
    if (UIWidgets::Button(
            "Save", UIWidgets::ButtonOptions({ { .disabled = (nameExists || noneSelected || newPresetName.empty()),
                                                 .disabledTooltip = disabledTooltip } })
                        .Padding({ 6.0f, 6.0f })
                        .Color(THEME_COLOR))) {
        presets[newPresetName] = {};
        auto config = Ship::Context::GetInstance()->GetConfig()->GetNestedJson();
        for (int i = PRESET_SECTION_SETTINGS; i < PRESET_SECTION_MAX; i++) {
            if (saveSection[i]) {
                for (int j = 0; j < blockInfo[i].sections.size(); j++) {
                    presets[newPresetName].presetValues["blocks"][blockInfo[i].names[1]][blockInfo[i].sections[j]] =
                        config["CVars"][blockInfo[i].sections[j]];
                }
            }
        }
        if (saveSection[PRESET_SECTION_TRACKERS]) {
            for (auto id : itemTrackerWindowIDs) {
                auto window = ImGui::FindWindowByName(id);
                if (window != nullptr) {
                    auto size = window->Size;
                    auto pos = window->Pos;
                    presets[newPresetName].presetValues["blocks"][blockInfo[PRESET_SECTION_TRACKERS].names[1]]
                                                       ["windows"][id]["size"]["width"] = size.x;
                    presets[newPresetName].presetValues["blocks"][blockInfo[PRESET_SECTION_TRACKERS].names[1]]
                                                       ["windows"][id]["size"]["height"] = size.y;
                    presets[newPresetName].presetValues["blocks"][blockInfo[PRESET_SECTION_TRACKERS].names[1]]
                                                       ["windows"][id]["pos"]["x"] = pos.x;
                    presets[newPresetName].presetValues["blocks"][blockInfo[PRESET_SECTION_TRACKERS].names[1]]
                                                       ["windows"][id]["pos"]["y"] = pos.y;
                }
            }

            auto window = ImGui::FindWindowByName("Entrance Tracker");
            if (window != nullptr) {
                auto size = window->Size;
                auto pos = window->Pos;
                presets[newPresetName].presetValues["blocks"][blockInfo[PRESET_SECTION_TRACKERS].names[1]]["windows"]
                                                   ["Entrance Tracker"]["size"]["width"] = size.x;
                presets[newPresetName].presetValues["blocks"][blockInfo[PRESET_SECTION_TRACKERS].names[1]]["windows"]
                                                   ["Entrance Tracker"]["size"]["height"] = size.y;
                presets[newPresetName].presetValues["blocks"][blockInfo[PRESET_SECTION_TRACKERS].names[1]]["windows"]
                                                   ["Entrance Tracker"]["pos"]["x"] = pos.x;
                presets[newPresetName].presetValues["blocks"][blockInfo[PRESET_SECTION_TRACKERS].names[1]]["windows"]
                                                   ["Entrance Tracker"]["pos"]["y"] = pos.y;
            }

            window = ImGui::FindWindowByName("Check Tracker");
            if (window != nullptr) {
                auto size = window->Size;
                auto pos = window->Pos;
                presets[newPresetName].presetValues["blocks"][blockInfo[PRESET_SECTION_TRACKERS].names[1]]["windows"]
                                                   ["Check Tracker"]["size"]["width"] = size.x;
                presets[newPresetName].presetValues["blocks"][blockInfo[PRESET_SECTION_TRACKERS].names[1]]["windows"]
                                                   ["Check Tracker"]["size"]["height"] = size.y;
                presets[newPresetName].presetValues["blocks"][blockInfo[PRESET_SECTION_TRACKERS].names[1]]["windows"]
                                                   ["Check Tracker"]["pos"]["x"] = pos.x;
                presets[newPresetName].presetValues["blocks"][blockInfo[PRESET_SECTION_TRACKERS].names[1]]["windows"]
                                                   ["Check Tracker"]["pos"]["y"] = pos.y;
            }
        }
        presets[newPresetName].fileName = newPresetName;
        std::fill_n(presets[newPresetName].apply, PRESET_SECTION_MAX, true);
        SavePreset(newPresetName);
        newPresetName = "";
        ImGui::CloseCurrentPopup();
    }
    if (UIWidgets::Button("Cancel", UIWidgets::ButtonOptions().Padding({ 6.0f, 6.0f }).Color(THEME_COLOR))) {
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
}

void PresetsCustomWidget(WidgetInfo& info) {
    ImGui::PushFont(OTRGlobals::Instance->fontMonoLargest);
    if (UIWidgets::Button("New Preset", UIWidgets::ButtonOptions(
                                            { { .disabled = (CVarGetInteger(CVAR_SETTING("DisableChanges"), 0) != 0),
                                                .disabledTooltip = "Disabled because of race lockout" } })
                                            .Size(UIWidgets::Sizes::Inline)
                                            .Color(THEME_COLOR))) {
        ImGui::OpenPopup("newPreset");
    }
    if (ImGui::BeginPopup("newPreset", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize |
                                           ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                           ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar)) {
        DrawNewPresetPopup();
    }
    ImGui::SameLine();
    UIWidgets::CVarCheckbox("Hide built-in presets", CVAR_GENERAL("HideBuiltInPresets"),
                            UIWidgets::CheckboxOptions().Color(THEME_COLOR));
    bool hideBuiltIn = CVarGetInteger(CVAR_GENERAL("HideBuiltInPresets"), 0);
    UIWidgets::PushStyleTabs(THEME_COLOR);
    if (ImGui::BeginTable("PresetWidgetTable", PRESET_SECTION_MAX + 3)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 250);
        for (int i = PRESET_SECTION_SETTINGS; i < PRESET_SECTION_MAX; i++) {
            ImGui::TableSetupColumn(blockInfo[i].names[0].c_str());
        }
        ImGui::TableSetupColumn("Apply", ImGuiTableColumnFlags_WidthFixed,
                                ImGui::CalcTextSize("Apply").x + ImGui::GetStyle().FramePadding.x * 2);
        ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed,
                                ImGui::CalcTextSize("Delete").x + ImGui::GetStyle().FramePadding.x * 2);
        BlankButton();
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        for (int i = PRESET_SECTION_SETTINGS; i < PRESET_SECTION_MAX; i++) {
            ImGui::TableNextColumn();
            ImGui::Button(fmt::format("{}##header{}", blockInfo[i].icon, blockInfo[i].names[1]).c_str());
            UIWidgets::Tooltip(blockInfo[i].names[0].c_str());
        }
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
            if (hideBuiltIn && info.isBuiltIn) {
                continue;
            }
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", name.c_str());
            for (int i = PRESET_SECTION_SETTINGS; i < PRESET_SECTION_MAX; i++) {
                ImGui::TableNextColumn();
                DrawSectionCheck(name, !info.presetValues["blocks"].contains(blockInfo[i].names[1]), &info.apply[i],
                                 blockInfo[i].names[1]);
            }
            ImGui::TableNextColumn();
            UIWidgets::PushStyleButton(THEME_COLOR);
            if (UIWidgets::Button(
                    ("Apply##" + name).c_str(),
                    UIWidgets::ButtonOptions({ { .disabled = (CVarGetInteger(CVAR_SETTING("DisableChanges"), 0) != 0),
                                                 .disabledTooltip = "Disabled because of race lockout" } })
                        .Padding({ 6.0f, 6.0f }))) {
                applyPreset(name);
            }
            UIWidgets::PopStyleButton();
            ImGui::TableNextColumn();
            UIWidgets::PushStyleButton(THEME_COLOR);
            if (!info.isBuiltIn) {
                if (UIWidgets::Button(("Delete##" + name).c_str(),
                                      UIWidgets::ButtonOptions().Padding({ 6.0f, 6.0f }))) {
                    auto path = FormatPresetPath(info.fileName);
                    if (fs::exists(path)) {
                        fs::remove(path);
                    }
                    presets.erase(name);
                    UIWidgets::PopStyleButton();
                    break;
                }
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
        .CustomFunction(PresetsCustomWidget)
        .HideInSearch(true);
    presetFolder = Ship::Context::GetInstance()->GetPathRelativeToAppDirectory("presets");
    std::fill_n(saveSection, PRESET_SECTION_MAX, true);
    LoadPresets();
}

static RegisterMenuInitFunc menuInitFunc(RegisterPresetsWidgets);
