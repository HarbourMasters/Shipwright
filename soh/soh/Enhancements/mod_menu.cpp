#include "mod_menu.h"
#include "utils/StringHelper.h"
#include <libultraship/classes.h>
#include "soh/OTRGlobals.h"
#include "soh/resource/type/Skeleton.h"
#include <map>
#include <ranges>
extern "C" void gfx_texture_cache_clear();

std::shared_ptr<Ship::ArchiveManager> GetArchiveManager() {
    return Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager();
}

std::map<std::string, bool> modFiles;

#define CVAR_ENABLED_MODS_NAME CVAR_GENERAL("EnabledMods")
#define CVAR_ENABLED_MODS_DEFAULT ""
#define CVAR_ENABLED_MODS_VALUE CVarGetString(CVAR_ENABLED_MODS_NAME, CVAR_ENABLED_MODS_DEFAULT)

#define SEPARATOR "|"

void SaveEnabledModsCVarValue() {
    std::string s = "";

    for (auto& [modPath, enabled] : modFiles) {
        if (enabled) {
            s += modPath + SEPARATOR;
        }
    }

    //remove trailing separator if present
    if (s.length() != 0) {
        s.pop_back();
    }

    CVarSetString(CVAR_ENABLED_MODS_NAME, s.c_str());
}

std::vector<std::string> GetEnabledModsFromCVar() {
    std::string enabledModsCVarValue = CVAR_ENABLED_MODS_VALUE;
    return StringHelper::Split(enabledModsCVarValue, SEPARATOR);
}

bool is_enabled(const std::pair<std::string, bool>& p) {
    return p.second;
}

std::vector<std::string> GetEnabledModFiles() {
    std::map<std::string, bool> enabledMods;

    std::copy_if(modFiles.begin(), modFiles.end(), std::inserter(enabledMods, enabledMods.begin()), is_enabled);

    auto ks = std::views::keys(enabledMods);
    std::vector<std::string> keys{ ks.begin(), ks.end() };

    return keys;
}

bool is_disabled(const std::pair<std::string, bool>& p) {
    return !p.second;
}

std::vector<std::string> GetDisabledModFiles() {
    std::map<std::string, bool> disabledMods;

    std::copy_if(modFiles.begin(), modFiles.end(), std::inserter(disabledMods, disabledMods.begin()), is_disabled);

    auto ks = std::views::keys(disabledMods);
    std::vector<std::string> keys{ ks.begin(), ks.end() };

    return keys;
}

void UpdateModFiles(bool init = false) {
    modFiles.clear();
    std::vector<std::string> enabledMods = GetEnabledModsFromCVar();
    std::string modsPath = Ship::Context::LocateFileAcrossAppDirs("mods", appShortName);
    if (modsPath.length() > 0 && std::filesystem::exists(modsPath)) {
        if (std::filesystem::is_directory(modsPath)) {
            for (const std::filesystem::directory_entry& p : std::filesystem::recursive_directory_iterator(modsPath, std::filesystem::directory_options::follow_directory_symlink)) {
                std::string extension = p.path().extension().string();
                if (
                    StringHelper::IEquals(extension, ".otr") ||
                    StringHelper::IEquals(extension, ".mpq") ||
                    StringHelper::IEquals(extension, ".o2r") ||
                    StringHelper::IEquals(extension, ".zip")
                ) {
                    std::string path = p.path().generic_string();
                    bool shouldBeEnabled = std::find(enabledMods.begin(), enabledMods.end(), path) != enabledMods.end();
                    if (init && shouldBeEnabled) {
                        GetArchiveManager()->AddArchive(path);
                    }
                    modFiles.emplace(path, shouldBeEnabled);
                }
            }
        }
    }

    /*
    std::sort(modFiles.begin(), modFiles.end(), [](const std::string& a, const std::string& b) {
        return std::lexicographical_compare(
            a.begin(), a.end(),
            b.begin(), b.end(),
            [](char c1, char c2) {
                return std::tolower(c1) < std::tolower(c2);
            }
        );
    });
    */
}

void AfterModChange() {
    SaveEnabledModsCVarValue();
    gfx_texture_cache_clear();
    SOH::SkeletonPatcher::UpdateSkeletons();
    Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

void DrawModInfo(std::string file) {
    ImGui::Text(file.c_str());
}

void ModMenuWindow::DrawElement() {
    if (ImGui::Button("Update")) {
        UpdateModFiles();
    }

    if (ImGui::BeginTable("tableMods", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Disabled Mods", ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::TableSetupColumn("Enabled Mods", ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::TableHeadersRow();
        ImGui::PopItemFlag();
        ImGui::TableNextRow();

        ImGui::TableNextColumn();

        if (ImGui::BeginChild("Disabled Mods", ImVec2(0, -8))) {
            std::vector<std::string> disabledMods = GetDisabledModFiles();
            if (!disabledMods.empty()) {
                for (std::string file : disabledMods) {
                    if (ImGui::ArrowButton(file.c_str(), ImGuiDir_Right)) {
                        modFiles[file] = true;
                        GetArchiveManager()->AddArchive(file);
                        AfterModChange();
                    }
                    ImGui::SameLine();
                    DrawModInfo(file);
                }
            } else {
                ImGui::Text("<None>");
            }

            ImGui::EndChild();
        }

        ImGui::TableNextColumn();

        if (ImGui::BeginChild("Enabled Mods", ImVec2(0, -8))) {
            std::vector<std::string> enabledMods = GetEnabledModFiles();
            if (!enabledMods.empty()) {
                for (std::string file : enabledMods) {
                    if (ImGui::ArrowButton(file.c_str(), ImGuiDir_Left)) {
                        modFiles[file] = false;
                        GetArchiveManager()->RemoveArchive(file);
                        AfterModChange();
                    }
                    ImGui::SameLine();
                    DrawModInfo(file);
                }
            } else {
                ImGui::Text("<None>");
            }

            ImGui::EndChild();
        }

        ImGui::EndTable();
    }
}

void ModMenuWindow::InitElement() {
    UpdateModFiles(true);
}