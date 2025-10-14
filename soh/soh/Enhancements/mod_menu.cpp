#include "mod_menu.h"
#include <ship/utils/StringHelper.h>
#include <libultraship/classes.h>
#include "soh/SohGui/SohGui.hpp"
#include "soh/OTRGlobals.h"
#include "soh/resource/type/Skeleton.h"
#include <map>
#include <ranges>
#include <vector>

std::vector<std::string> enabledModFiles;
std::vector<std::string> disabledModFiles;

#define CVAR_ENABLED_MODS_NAME CVAR_GENERAL("EnabledMods")
#define CVAR_ENABLED_MODS_DEFAULT ""
#define CVAR_ENABLED_MODS_VALUE CVarGetString(CVAR_ENABLED_MODS_NAME, CVAR_ENABLED_MODS_DEFAULT)

// "|" was chosen as the separator due to
// it being an invalid character in NTFS
// and being rarely used in ext4
// it is also an ASCII character
// improving portability

// if being an ASCII character is not a requirement,
// other possible candidates include:
// - U+FFFF: non-character
// - any private use character
#define SEPARATOR "|"

void SetEnabledModsCVarValue() {
    std::string s = "";

    for (auto& modPath : enabledModFiles) {
        s += modPath + SEPARATOR;
    }

    // remove trailing separator if present
    if (s.length() != 0) {
        s.pop_back();
    }

    CVarSetString(CVAR_ENABLED_MODS_NAME, s.c_str());
}

std::vector<std::string> GetEnabledModsFromCVar() {
    std::string enabledModsCVarValue = CVAR_ENABLED_MODS_VALUE;
    return StringHelper::Split(enabledModsCVarValue, SEPARATOR);
}

std::vector<std::string>& GetModFiles(bool enabled) {
    return enabled ? enabledModFiles : disabledModFiles;
}

std::shared_ptr<Ship::ArchiveManager> GetArchiveManager() {
    return Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager();
}

void UpdateModFiles(bool init = false) {
    if (init) {
        enabledModFiles.clear();
    }
    disabledModFiles.clear();
    std::vector<std::string> enabledMods = GetEnabledModsFromCVar();
    std::string modsPath = Ship::Context::LocateFileAcrossAppDirs("mods", appShortName);
    if (modsPath.length() > 0 && std::filesystem::exists(modsPath)) {
        if (std::filesystem::is_directory(modsPath)) {
            for (const std::filesystem::directory_entry& p : std::filesystem::recursive_directory_iterator(
                     modsPath, std::filesystem::directory_options::follow_directory_symlink)) {
                std::string extension = p.path().extension().string();
                if (
#ifndef EXCLUDE_MPQ_SUPPORT
                    StringHelper::IEquals(extension, ".otr") || StringHelper::IEquals(extension, ".mpq") ||
#endif
                    StringHelper::IEquals(extension, ".o2r") || StringHelper::IEquals(extension, ".zip")) {
                    std::string path = p.path().generic_string();
                    bool shouldBeEnabled = std::find(enabledMods.begin(), enabledMods.end(), path) != enabledMods.end();

                    if (shouldBeEnabled) {
                        if (init) {
                            enabledModFiles.push_back(path);
                            GetArchiveManager()->AddArchive(path);
                        }
                    } else {
                        disabledModFiles.push_back(path);
                    }
                }
            }
        }
    }
}

extern "C" void gfx_texture_cache_clear();

void AfterModChange() {
    SetEnabledModsCVarValue();
    // TODO: runtime changes
    /*
    gfx_texture_cache_clear();
    SOH::SkeletonPatcher::ClearSkeletons();
    */
    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();

    // disabled mods are always sorted
    std::sort(disabledModFiles.begin(), disabledModFiles.end(), [](const std::string& a, const std::string& b) {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(),
                                            [](char c1, char c2) { return std::tolower(c1) < std::tolower(c2); });
    });
}

void EnableMod(std::string file) {
    disabledModFiles.erase(std::find(disabledModFiles.begin(), disabledModFiles.end(), file));
    enabledModFiles.insert(enabledModFiles.begin(), file);

    // TODO: runtime changes
    // GetArchiveManager()->AddArchive(file);
    AfterModChange();
}

void DisableMod(std::string file) {
    enabledModFiles.erase(std::find(enabledModFiles.begin(), enabledModFiles.end(), file));
    disabledModFiles.insert(disabledModFiles.begin(), file);

    // TODO: runtime changes
    // GetArchiveManager()->RemoveArchive(file);
    AfterModChange();
}

void DrawModInfo(std::string file) {
    ImGui::SameLine();
    ImGui::Text("%s", file.c_str());
}

void DrawMods(bool enabled) {
    std::vector<std::string>& selectedModFiles = GetModFiles(enabled);
    if (selectedModFiles.empty()) {
        return;
    }

    bool madeAnyChange = false;
    int switchFromIndex = -1;
    int switchToIndex = -1;

    for (int i = 0; i < selectedModFiles.size(); i += 1) {
        std::string file = selectedModFiles[i];
        if (UIWidgets::StateButton((file + "_left_right").c_str(), enabled ? ICON_FA_ARROW_LEFT : ICON_FA_ARROW_RIGHT,
                                   ImVec2(25, 25), UIWidgets::ButtonOptions().Color(THEME_COLOR))) {
            if (enabled) {
                DisableMod(file);
            } else {
                EnableMod(file);
            }
        }

        // it's not relevant to reorder disabled mods
        if (enabled) {
            ImGui::SameLine();
            if (i == 0) {
                ImGui::BeginDisabled();
            }
            if (UIWidgets::StateButton((file + "_up").c_str(), ICON_FA_ARROW_UP, ImVec2(25, 25),
                                       UIWidgets::ButtonOptions().Color(THEME_COLOR))) {
                madeAnyChange = true;
                switchFromIndex = i;
                switchToIndex = i - 1;
            }
            if (i == 0) {
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            if (i == selectedModFiles.size() - 1) {
                ImGui::BeginDisabled();
            }
            if (UIWidgets::StateButton((file + "_down").c_str(), ICON_FA_ARROW_DOWN, ImVec2(25, 25),
                                       UIWidgets::ButtonOptions().Color(THEME_COLOR))) {
                madeAnyChange = true;
                switchFromIndex = i;
                switchToIndex = i + 1;
            }
            if (i == selectedModFiles.size() - 1) {
                ImGui::EndDisabled();
            }
        }

        DrawModInfo(file);
    }

    if (madeAnyChange) {
        std::iter_swap(selectedModFiles.begin() + switchFromIndex, selectedModFiles.begin() + switchToIndex);
        AfterModChange();
    }
}

void ModMenuWindow::DrawElement() {
    ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));

    const ImVec4 yellow = ImVec4(1, 1, 0, 1);

    ImGui::TextColored(
        yellow, "Mods are currently not reloaded at runtime.\nClose and re-open Ship for the changes to take effect.");

    if (UIWidgets::Button("Update", UIWidgets::ButtonOptions().Size(ImVec2(250.0f, 0.0f)).Color(THEME_COLOR))) {
        UpdateModFiles();
    }
    UIWidgets::Tooltip("Re-check the mods folder for new files");

    if (ImGui::BeginTable("tableMods", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Disabled Mods", ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::TableSetupColumn("Enabled Mods", ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::TableHeadersRow();
        ImGui::PopItemFlag();
        ImGui::TableNextRow();

        ImGui::TableNextColumn();

        if (ImGui::BeginChild("Disabled Mods", ImVec2(0, -8))) {
            DrawMods(false);

            ImGui::EndChild();
        }

        ImGui::TableNextColumn();

        if (ImGui::BeginChild("Enabled Mods", ImVec2(0, -8))) {
            DrawMods(true);

            ImGui::EndChild();
        }

        ImGui::EndTable();
    }

    ImGui::EndDisabled();
}

void ModMenuWindow::InitElement() {
    UpdateModFiles(true);
}