#include "mod_menu.h"
#include "utils/StringHelper.h"
#include <libultraship/classes.h>
#include "soh/OTRGlobals.h"
#include <map>
#include <ranges>

std::shared_ptr<Ship::ArchiveManager> GetArchiveManager() {
    return Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager();
}

std::map<std::string, bool> modFiles;

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

void UpdateModFiles() {
    modFiles.clear();
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
                    modFiles.emplace(p.path().generic_string(), false);
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

void ModMenuWindow::DrawElement() {
    if (ImGui::Button("Update")) {
        UpdateModFiles();
    }

    if (ImGui::BeginTable("tableMods", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Enabled Mods", ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::TableSetupColumn("Disabled Mods", ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::TableHeadersRow();
        ImGui::PopItemFlag();
        ImGui::TableNextRow();

        ImGui::TableNextColumn();

        if (ImGui::BeginChild("Enabled Mods", ImVec2(0, -8))) {
            std::vector<std::string> enabledMods = GetEnabledModFiles();
            if (!enabledMods.empty()) {
                for (std::string file : enabledMods) {
                    if (ImGui::Button(("Disable##" + file).c_str())) {
                        modFiles[file] = false;
                        Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->RemoveArchive(file);
                    }
                    ImGui::SameLine();
                    ImGui::Text(file.c_str());
                }
            } else {
                ImGui::Text("<None>");
            }

            ImGui::EndChild();
        }

        ImGui::TableNextColumn();

        if (ImGui::BeginChild("Disabled Mods", ImVec2(0, -8))) {
            std::vector<std::string> disabledMods = GetDisabledModFiles();
            if (!disabledMods.empty()) {
                for (std::string file : disabledMods) {
                    if (ImGui::Button(("Enable##" + file).c_str())) {
                        modFiles[file] = true;
                        Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->AddArchive(file);
                    }
                    ImGui::SameLine();
                    ImGui::Text(file.c_str());
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
    UpdateModFiles();
}