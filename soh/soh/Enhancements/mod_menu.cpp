#include <map>
#include <ranges>
#include <vector>

#include <libultraship/classes.h>
#include <ship/utils/StringHelper.h>

#include "mod_menu.h"
#include "soh/OTRGlobals.h"
#include "soh/resource/type/Skeleton.h"
#include "soh/SohGui/MenuTypes.h"
#include "soh/SohGui/SohMenu.h"
#include "soh/SohGui/SohGui.hpp"

typedef enum ExtensionType {
    EXT_ENABLED,
    EXT_DISABLED,
    EXT_UNSUPPORTED,
} ExtensionType;

std::vector<std::string> enabledModFiles;
std::vector<std::string> disabledModFiles;
std::vector<std::string> unsupportedFiles;
std::map<std::string, std::filesystem::path> filePaths;
std::map<std::string, std::string> extChanges;
static int dragSourceIndex = -1;
static int dragTargetIndex = -1;

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
}

static WidgetInfo enableModsWidget;

#define CVAR_ENABLED_MODS_NAME CVAR_SETTING("EnabledMods")
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

void AfterModChange() {
    // disabled mods are always sorted
    std::sort(disabledModFiles.begin(), disabledModFiles.end(), [](const std::string& a, const std::string& b) {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(),
                                            [](char c1, char c2) { return std::tolower(c1) < std::tolower(c2); });
    });
}

void ModsPostDragAndDrop() {
    if (dragTargetIndex != -1) {
        std::string file = enabledModFiles[dragSourceIndex];
        enabledModFiles.erase(enabledModFiles.begin() + dragSourceIndex);
        enabledModFiles.insert(enabledModFiles.begin() + dragTargetIndex, file);
        dragTargetIndex = dragSourceIndex = -1;
        AfterModChange();
    }
}

void ModsHandleDragAndDrop(std::vector<std::string>& objectList, int targetIndex, const std::string& itemName,
                           ImGuiDragDropFlags flags = ImGuiDragDropFlags_SourceAllowNullID) {
    if (ImGui::BeginDragDropSource(flags)) {
        ImGui::SetDragDropPayload("DragMove", &targetIndex, sizeof(uint32_t));
        ImGui::Text("Move %s", itemName.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragMove")) {
            IM_ASSERT(payload->DataSize == sizeof(uint32_t));
            dragSourceIndex = *(const int*)payload->Data;
            dragTargetIndex = targetIndex;
        }
        ImGui::EndDragDropTarget();
    }
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

ExtensionType GetExtensionType(std::string extension) {
    if (
#ifdef INCLUDE_MPQ_SUPPORT
        // .mpq doesn't make sense to support because all tools to make such mods output OTR
        StringHelper::IEquals(extension, ".otr") /*|| StringHelper::IEquals(extension, ".mpq")*/ ||
#endif
        // .zip needs to be excluded because mods are most often distributed in zip archives
        // and thus could contain .otr/o2r files
        StringHelper::IEquals(extension, ".o2r") /*|| StringHelper::IEquals(extension, ".zip")*/) {
        return EXT_ENABLED;
    } else if (
#ifdef INCLUDE_MPQ_SUPPORT
        StringHelper::IEquals(extension, ".disabled1") ||
#endif
        StringHelper::IEquals(extension, ".disabled2")) {
        return EXT_DISABLED;
    }
    return EXT_UNSUPPORTED;
}

void UpdateModFiles(bool init = false, bool reset = false) {
    if (init || reset) {
        enabledModFiles.clear();
        enabledModFiles = GetEnabledModsFromCVar();
    }
    disabledModFiles.clear();
    unsupportedFiles.clear();
    filePaths.clear();
    std::string modsPath = Ship::Context::LocateFileAcrossAppDirs("mods", appShortName);
    bool changed = false;
    if (modsPath.length() > 0 && std::filesystem::exists(modsPath)) {
        std::vector<std::filesystem::path> enabledFiles;
        if (std::filesystem::is_directory(modsPath)) {
            for (const std::filesystem::directory_entry& p : std::filesystem::recursive_directory_iterator(
                     modsPath, std::filesystem::directory_options::follow_directory_symlink)) {
                if (p.is_directory()) {
                    continue;
                }
                std::string filename =
                    p.path().filename().generic_string().substr(0, p.path().filename().generic_string().rfind("."));
                std::string extension = p.path().extension().generic_string();
                ExtensionType extType = GetExtensionType(extension);
                bool enabled =
                    std::find(enabledModFiles.begin(), enabledModFiles.end(), filename) != enabledModFiles.end();
                if (extType == EXT_ENABLED) {
                    if (!enabled) {
                        enabledModFiles.push_back(filename);
                        changed = true;
                    }
                } else if (extType == EXT_DISABLED) {
                    disabledModFiles.push_back(filename);
                } else {
                    unsupportedFiles.push_back(p.path().filename().generic_string());
                }
                filePaths.emplace(filename, p.path());
            }
            if (init) {
                std::vector<std::string> enabledTemp(enabledModFiles);
                for (std::string mod : enabledTemp) {
                    // auto archives = GetArchiveManager()->GetArchives();
                    //  TODO ensure archives don't get added multiple times. breaks the renaming on close
                    if (filePaths.contains(mod)) {
                        // if (init/* &&
                        // !GetArchiveManager()->HasFile(filePaths.at(mod).lexically_normal().generic_string())*/) {
                        GetArchiveManager()->AddArchive(filePaths.at(mod).generic_string());
                        //}
                    } else {
                        enabledModFiles.erase(std::find(enabledModFiles.begin(), enabledModFiles.end(), mod));
                        //    // if (!init /*&&
                        //    // GetArchiveManager()->HasFile(filePaths.at(mod).lexically_normal().generic_string())*/)
                        //    {
                        //    //
                        //    GetArchiveManager()->RemoveArchive(filePaths.at(mod).lexically_normal().generic_string());
                        //    // }
                        changed = true;
                    }
                }
            }
        }
    }
    if (changed) {
        SetEnabledModsCVarValue();
    }
}

extern "C" void gfx_texture_cache_clear();

void EnableMod(std::string file) {
    disabledModFiles.erase(std::find(disabledModFiles.begin(), disabledModFiles.end(), file));
    enabledModFiles.insert(enabledModFiles.begin(), file);
    std::string newExt;
    auto& path = filePaths.at(file);
    if (path.extension() == ".disabled1") {
        newExt = ".otr";
    } else {
        newExt = ".o2r";
    }
    std::string oldPath = path.generic_string();
    path.replace_extension(newExt);
    std::string newPath = path.generic_string();
    if (!extChanges.contains(oldPath)) {
        if (extChanges.contains(newPath)) {
            extChanges.erase(newPath);
        } else {
            extChanges.emplace(oldPath, newPath);
        }
    }

    // TODO: runtime changes
    // GetArchiveManager()->AddArchive(file);
    AfterModChange();
}

void DisableMod(std::string file) {
    enabledModFiles.erase(std::find(enabledModFiles.begin(), enabledModFiles.end(), file));
    disabledModFiles.insert(disabledModFiles.begin(), file);
    std::string newExt;
    auto& path = filePaths.at(file);
    if (path.extension() == ".otr") {
        newExt = ".disabled1";
    } else {
        newExt = ".disabled2";
    }
    std::string oldPath = path.generic_string();
    path.replace_extension(newExt);
    std::string newPath = path.generic_string();
    if (!extChanges.contains(oldPath)) {
        if (extChanges.contains(newPath)) {
            extChanges.erase(newPath);
        } else {
            extChanges.emplace(oldPath, newPath);
        }
    }

    // TODO: runtime changes
    // GetArchiveManager()->RemoveArchive(file);
    AfterModChange();
}

void DrawModInfo(std::string file) {
    ImGui::SameLine();
    ImGui::Text("%s", file.c_str());
}

void RemoveExtension(std::string& file) {
}

void DrawMods(bool enabled) {
    std::vector<std::string>& selectedModFiles = GetModFiles(enabled);
    if (selectedModFiles.empty()) {
        return;
    }

    bool madeAnyChange = false;
    int switchFromIndex = -1;
    int switchToIndex = -1;
    uint32_t index = 0;

    for (int i = 0; i < selectedModFiles.size(); i += 1) {
        std::string file = selectedModFiles[i];
        if (enabled) {
            ImGui::BeginGroup();
        }
        if (UIWidgets::StateButton((file + "_left_right").c_str(), enabled ? ICON_FA_ARROW_RIGHT : ICON_FA_ARROW_LEFT,
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

        DrawModInfo(filePaths.at(file).filename().generic_string());
        if (enabled) {
            ImGui::EndGroup();
            ModsHandleDragAndDrop(selectedModFiles, i, file);
        }
    }

    if (enabled) {
        ModsPostDragAndDrop();
    }

    if (madeAnyChange) {
        std::iter_swap(selectedModFiles.begin() + switchFromIndex, selectedModFiles.begin() + switchToIndex);
        AfterModChange();
    }
}

bool editing = false;

void ModMenuWindow::DrawElement() {
    SohGui::mSohMenu->MenuDrawItem(enableModsWidget, 200, THEME_COLOR);

    ImGui::TextColored(
        UIWidgets::ColorValues.at(UIWidgets::Colors::Yellow),
        "Mods are currently not reloaded at runtime. Close and re-open Ship for the changes to take effect.\n"
        "Drag ordering for the enabled list is available.\nMod load order is top to bottom. Mods at the top are loaded "
        "first.");

    // if (UIWidgets::Button(
    //         "Update", UIWidgets::ButtonOptions({ { .disabled = editing, .disabledTooltip = "Currently editing..." }
    //         })
    //                       .Size(UIWidgets::Sizes::Inline)
    //                       .Color(THEME_COLOR))) {
    //     UpdateModFiles();
    // }
    // ImGui::SameLine();
    if (UIWidgets::Button("Edit",
                          UIWidgets::ButtonOptions({ { .disabled = editing, .disabledTooltip = "Already editing..." } })
                              .Size(UIWidgets::Sizes::Inline)
                              .Color(THEME_COLOR))) {
        editing = true;
    }
    if (editing) {
        ImGui::SameLine();
        if (UIWidgets::Button("Cancel", UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline))) {
            editing = false;
            extChanges.clear();
            UpdateModFiles(false, true);
        }
        ImGui::SameLine();
        if (UIWidgets::Button("Apply & Close",
                              UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline).Color(THEME_COLOR))) {
            SohGui::RegisterPopup("Apply & Close",
                                  "Application currently requires a restart. Save the mod info and close SoH?", "Close",
                                  "Cancel", [&]() {
                                      // TODO: runtime changes
                                      // GetArchiveManager()->RemoveArchive(file);
                                      for (auto& [op, np] : extChanges) {
                                          GetArchiveManager()->RemoveArchive(op);
                                          std::filesystem::rename(op, np);
                                      }
                                      extChanges.clear();
                                      SetEnabledModsCVarValue();
                                      // TODO: runtime changes
                                      /*
                                      gfx_texture_cache_clear();
                                      SOH::SkeletonPatcher::ClearSkeletons();
                                      */
                                      Ship::Context::GetInstance()->GetConsoleVariables()->Save();
                                      Ship::Context::GetInstance()->GetWindow()->Close();
                                  });
        }
    }
    ImGui::BeginDisabled(!editing);
    if (ImGui::BeginTable("tableMods", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Enabled Mods", ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::TableSetupColumn("Disabled Mods", ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::TableHeadersRow();
        ImGui::PopItemFlag();
        ImGui::TableNextRow();

        ImGui::TableNextColumn();

        if (ImGui::BeginChild("Enabled Mods", ImVec2(0, -8))) {
            DrawMods(true);

            ImGui::EndChild();
        }

        ImGui::TableNextColumn();

        if (ImGui::BeginChild("Disabled Mods", ImVec2(0, -8))) {
            DrawMods(false);

            ImGui::EndChild();
        }

        ImGui::EndTable();
    }
    ImGui::EndDisabled();
}

void ModMenuWindow::InitElement() {
    UpdateModFiles(true);
}

void RegisterModMenuWidgets() {
    enableModsWidget = { .name = "Enable Mods", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    enableModsWidget.CVar(CVAR_SETTING("AltAssets"))
        .Options(UIWidgets::CheckboxOptions({ { .disabledTooltip = "Temporarily disabled while editing mods list." } })
                     .Color(THEME_COLOR)
                     .Tooltip("Toggle mods. For graphics mods, this means toggling between default and mod graphics."))
        .PreFunc([&](WidgetInfo& info) {
            auto options = std::static_pointer_cast<UIWidgets::CheckboxOptions>(info.options);
            options->disabled = editing;
        });
    SohGui::mSohMenu->AddSearchWidget({ enableModsWidget, "Enhancements", "Mod Menu", "Top", "alternat assets" });
}

static RegisterMenuInitFunc menuInitFunc(RegisterModMenuWidgets);
