#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <vector>

#include <libultraship/classes.h>
#include <ship/utils/StringHelper.h>

#include "mod_menu.h"
#include "soh/OTRGlobals.h"
#include "soh/resource/type/Skeleton.h"
#include "soh/SohGui/MenuTypes.h"
#include "soh/SohGui/SohMenu.h"
#include "soh/SohGui/SohGui.hpp"

std::vector<std::string> enabledModFiles;
std::vector<std::string> disabledModFiles;
std::vector<std::string> unsupportedFiles;
std::map<std::string, std::filesystem::path> filePaths;
static int dragSourceIndex = -1;
static std::set<std::string> selectedEnabledModFiles;
static int lastSelectedModIndex = -1;
static bool boxSelectingMods = false;
static ImVec2 boxSelectStart;

struct ModRowBounds {
    std::string file;
    size_t index;
    ImVec2 min;
    ImVec2 max;
};

static std::vector<ModRowBounds> modRowBounds;

bool PointInRow(const ImVec2& point, const ModRowBounds& row) {
    return point.x >= row.min.x && point.x <= row.max.x && point.y >= row.min.y && point.y <= row.max.y;
}

bool RectIntersectsRow(const ImVec2& min, const ImVec2& max, const ModRowBounds& row) {
    return min.x <= row.max.x && max.x >= row.min.x && min.y <= row.max.y && max.y >= row.min.y;
}

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
}

static WidgetInfo enableModsWidget;
static WidgetInfo tabHotkeyWidget;

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
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

void AfterModChange() {
    // disabled mods are always sorted
    std::sort(disabledModFiles.begin(), disabledModFiles.end(), [](const std::string& a, const std::string& b) {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(),
                                            [](char c1, char c2) { return std::tolower(c1) < std::tolower(c2); });
    });
}

void ClearSelectedMods() {
    selectedEnabledModFiles.clear();
    lastSelectedModIndex = -1;
}

void SelectOnlyMod(const std::string& file, int index) {
    selectedEnabledModFiles.clear();
    selectedEnabledModFiles.insert(file);
    lastSelectedModIndex = index;
}

void HandleModSelection(size_t index, const std::string& file) {
    const ImGuiIO& io = ImGui::GetIO();

    if (io.KeyShift && lastSelectedModIndex >= 0 && lastSelectedModIndex < static_cast<int>(enabledModFiles.size())) {
        auto [startIndex, endIndex] = std::minmax(static_cast<size_t>(lastSelectedModIndex), index);
        selectedEnabledModFiles.clear();
        for (size_t i = startIndex; i <= endIndex; i++)
            selectedEnabledModFiles.insert(enabledModFiles[i]);
    } else if (io.KeyCtrl) {
        if (selectedEnabledModFiles.erase(file) == 0)
            selectedEnabledModFiles.insert(file);
        lastSelectedModIndex = static_cast<int>(index);
    } else {
        SelectOnlyMod(file, static_cast<int>(index));
    }
}

void UpdateBoxSelection() {
    if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && !boxSelectingMods)
        return;

    ImGuiIO& io = ImGui::GetIO();
    if (!boxSelectingMods && ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
        !std::any_of(modRowBounds.begin(), modRowBounds.end(),
                     [&](const ModRowBounds& row) { return PointInRow(io.MousePos, row); })) {
        boxSelectingMods = true;
        boxSelectStart = io.MousePos;
        selectedEnabledModFiles.clear();
    }

    if (!boxSelectingMods)
        return;

    ImVec2 selectionMin(std::min(boxSelectStart.x, io.MousePos.x), std::min(boxSelectStart.y, io.MousePos.y));
    ImVec2 selectionMax(std::max(boxSelectStart.x, io.MousePos.x), std::max(boxSelectStart.y, io.MousePos.y));

    selectedEnabledModFiles.clear();
    for (const auto& row : modRowBounds)
        if (RectIntersectsRow(selectionMin, selectionMax, row)) {
            selectedEnabledModFiles.insert(row.file);
            lastSelectedModIndex = static_cast<int>(row.index);
        }

    ImGui::GetWindowDrawList()->AddRectFilled(selectionMin, selectionMax, IM_COL32(80, 145, 220, 35));
    ImGui::GetWindowDrawList()->AddRect(selectionMin, selectionMax, IM_COL32(80, 145, 220, 180));
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        boxSelectingMods = false;
}

void MoveSelectedModsToInsertionIndex(size_t insertionIndex) {
    if (dragSourceIndex < 0 || dragSourceIndex >= static_cast<int>(enabledModFiles.size()))
        return;

    insertionIndex = std::min(insertionIndex, enabledModFiles.size());

    if (!selectedEnabledModFiles.contains(enabledModFiles[dragSourceIndex]))
        SelectOnlyMod(enabledModFiles[dragSourceIndex], dragSourceIndex);

    std::vector<std::string> movedFiles;
    for (size_t i = enabledModFiles.size(); i-- > 0;) {
        if (selectedEnabledModFiles.contains(enabledModFiles[i])) {
            movedFiles.push_back(enabledModFiles[i]);
            enabledModFiles.erase(enabledModFiles.begin() + i);
            insertionIndex -= i < insertionIndex;
        }
    }

    std::reverse(movedFiles.begin(), movedFiles.end());
    insertionIndex = std::min(insertionIndex, enabledModFiles.size());
    enabledModFiles.insert(enabledModFiles.begin() + insertionIndex, movedFiles.begin(), movedFiles.end());
    lastSelectedModIndex = -1;
}

std::vector<std::string> GetEnabledModsFromCVar() {
    std::string enabledModsCVarValue = CVAR_ENABLED_MODS_VALUE;
    if (enabledModsCVarValue.empty())
        return {};
    return StringHelper::Split(enabledModsCVarValue, SEPARATOR);
}

std::vector<std::string>& GetModFiles(bool enabled) {
    return enabled ? enabledModFiles : disabledModFiles;
}

std::shared_ptr<Ship::ArchiveManager> GetArchiveManager() {
    return Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager();
}

bool IsValidExtension(std::string extension) {
    if (
#ifdef INCLUDE_MPQ_SUPPORT
        // .mpq doesn't make sense to support because all tools to make such mods output OTR
        StringHelper::IEquals(extension, ".otr") /*|| StringHelper::IEquals(extension, ".mpq")*/ ||
#endif
        // .zip needs to be excluded because mods are most often distributed in zip archives
        // and thus could contain .otr/o2r files
        StringHelper::IEquals(extension, ".o2r") /*|| StringHelper::IEquals(extension, ".zip")*/) {
        return true;
    }
    return false;
}

void UpdateModFiles(bool init = false, bool reset = false) {
    if (init || reset) {
        enabledModFiles.clear();
        enabledModFiles = GetEnabledModsFromCVar();
        ClearSelectedMods();
    }
    disabledModFiles.clear();
    unsupportedFiles.clear();
    filePaths.clear();
    bool changed = false;
    std::string modsPath = Ship::Context::LocateFileAcrossAppDirs("mods", appShortName);
    std::map<std::string, std::string> tempMods;
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
                if (!IsValidExtension(extension)) {
                    continue;
                }
                bool enabled =
                    std::find(enabledModFiles.begin(), enabledModFiles.end(), filename) != enabledModFiles.end();
                if (!enabled) {
                    tempMods.emplace(p.path().lexically_normal().generic_string(), filename);
                }
                filePaths.emplace(filename, p.path());
            }
            if (tempMods.size() > 0) {
                changed = true;
                for (auto [path, name] : tempMods) {
                    enabledModFiles.push_back(name);
                }
                tempMods.clear();
            }
            if (init) {
                std::vector<std::string> enabledTemp(enabledModFiles);
                for (std::string mod : enabledTemp) {
                    if (filePaths.contains(mod)) {
                        GetArchiveManager()->AddArchive(filePaths.at(mod).generic_string());
                    } else {
                        enabledModFiles.erase(std::find(enabledModFiles.begin(), enabledModFiles.end(), mod));
                        changed = true;
                    }
                }
            }
        }
        if (changed) {
            SetEnabledModsCVarValue();
        }
    }
}

extern "C" void gfx_texture_cache_clear();

void EnableMod(std::string file) {
    disabledModFiles.erase(std::find(disabledModFiles.begin(), disabledModFiles.end(), file));
    enabledModFiles.insert(enabledModFiles.begin(), file);

    // TODO: runtime changes
    // GetArchiveManager()->AddArchive(file);
    ClearSelectedMods();
    AfterModChange();
}

void DisableMod(std::string file) {
    enabledModFiles.erase(std::find(enabledModFiles.begin(), enabledModFiles.end(), file));
    disabledModFiles.insert(disabledModFiles.begin(), file);

    // TODO: runtime changes
    // GetArchiveManager()->RemoveArchive(file);
    ClearSelectedMods();
    AfterModChange();
}

void HandleModDropBoundaries() {
    const ImGuiPayload* payload = ImGui::GetDragDropPayload();
    if (modRowBounds.empty() || payload == nullptr || !payload->IsDataType("DragMove"))
        return;

    ImVec2 mousePos = ImGui::GetIO().MousePos;
    float hitPadding = std::max(4.0f, ImGui::GetTextLineHeightWithSpacing() * 0.35f);
    float lineStartX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x;
    float lineEndX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    int hoveredInsertionIndex = -1;
    float hoveredLineY = 0.0f;

    auto testBoundary = [&](size_t insertionIndex, float lineY) {
        if (mousePos.x < lineStartX || mousePos.x > lineEndX || mousePos.y < lineY - hitPadding ||
            mousePos.y > lineY + hitPadding) {
            return;
        }

        hoveredInsertionIndex = static_cast<int>(insertionIndex);
        hoveredLineY = lineY;
    };

    testBoundary(modRowBounds.front().index + 1, modRowBounds.front().min.y);
    for (size_t i = 0; i + 1 < modRowBounds.size(); i++)
        testBoundary(modRowBounds[i].index, (modRowBounds[i].max.y + modRowBounds[i + 1].min.y) * 0.5f);
    testBoundary(modRowBounds.back().index, modRowBounds.back().max.y);

    if (hoveredInsertionIndex == -1)
        return;

    ImGui::GetWindowDrawList()->AddLine(ImVec2(lineStartX, hoveredLineY), ImVec2(lineEndX, hoveredLineY),
                                        IM_COL32(255, 211, 96, 255), 2.0f);
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        IM_ASSERT(payload->DataSize == sizeof(int));
        dragSourceIndex = *(const int*)payload->Data;
        MoveSelectedModsToInsertionIndex(static_cast<size_t>(hoveredInsertionIndex));
        dragSourceIndex = -1;
        AfterModChange();
    }
}

void DrawMods(bool enabled) {
    std::vector<std::string>& selectedModFiles = GetModFiles(enabled);
    if (selectedModFiles.empty()) {
        return;
    }

    bool madeAnyChange = false;
    int switchFromIndex = -1;
    int switchToIndex = -1;

    if (enabled) {
        UpdateBoxSelection();
        modRowBounds.clear();
    }

    for (size_t i = selectedModFiles.size() - 1; i != SIZE_MAX; i--) {
        std::string file = selectedModFiles[i];
        if (enabled) {
            ImGui::BeginGroup();
        }
        // if (UIWidgets::StateButton((file + "_left_right").c_str(), enabled ? ICON_FA_ARROW_RIGHT :
        // ICON_FA_ARROW_LEFT,
        //                            ImVec2(25, 25), UIWidgets::ButtonOptions().Color(THEME_COLOR))) {
        //     if (enabled) {
        //         DisableMod(file);
        //     } else {
        //         EnableMod(file);
        //     }
        // }

        // it's not relevant to reorder disabled mods
        if (enabled) {
            // ImGui::SameLine();
            if (i == selectedModFiles.size() - 1) {
                ImGui::BeginDisabled();
            }
            if (UIWidgets::StateButton((file + "_up").c_str(), ICON_FA_ARROW_UP, ImVec2(25, 25),
                                       UIWidgets::ButtonOptions().Color(THEME_COLOR))) {
                madeAnyChange = true;
                switchFromIndex = i;
                switchToIndex = i + 1;
            }
            if (i == selectedModFiles.size() - 1) {
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            if (i == 0) {
                ImGui::BeginDisabled();
            }
            if (UIWidgets::StateButton((file + "_down").c_str(), ICON_FA_ARROW_DOWN, ImVec2(25, 25),
                                       UIWidgets::ButtonOptions().Color(THEME_COLOR))) {
                madeAnyChange = true;
                switchFromIndex = i;
                switchToIndex = i - 1;
            }
            if (i == 0) {
                ImGui::EndDisabled();
            }
        }

        ImGui::SameLine();
        std::string displayName = filePaths.at(file).filename().generic_string();
        if (enabled) {
            ImGui::PushID(file.c_str());
            float selectableWidth =
                ImGui::CalcTextSize(displayName.c_str()).x + ImGui::GetStyle().FramePadding.x * 2.0f;
            if (ImGui::Selectable(displayName.c_str(), selectedEnabledModFiles.contains(file), 0,
                                  ImVec2(selectableWidth, 0.0f)))
                HandleModSelection(i, file);
            ImGui::PopID();
        } else {
            ImGui::Text("%s", displayName.c_str());
        }

        if (enabled) {
            ImGui::EndGroup();
            modRowBounds.push_back({ file, i, ImGui::GetItemRectMin(), ImGui::GetItemRectMax() });
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                int sourceIndex = static_cast<int>(i);
                if (!selectedEnabledModFiles.contains(file))
                    SelectOnlyMod(file, sourceIndex);
                ImGui::SetDragDropPayload("DragMove", &sourceIndex, sizeof(int));
                if (selectedEnabledModFiles.size() == 1) {
                    ImGui::Text("Move %s", file.c_str());
                } else {
                    ImGui::Text("Move %zu mods", selectedEnabledModFiles.size());
                }
                ImGui::EndDragDropSource();
            }
        }
    }

    if (enabled) {
        HandleModDropBoundaries();
    }

    if (madeAnyChange) {
        std::iter_swap(selectedModFiles.begin() + switchFromIndex, selectedModFiles.begin() + switchToIndex);
        ClearSelectedMods();
        AfterModChange();
    }
}

bool editing = false;

void ModMenuWindow::DrawElement() {
    SohGui::mSohMenu->MenuDrawItem(enableModsWidget, 200, THEME_COLOR);
    ImGui::SameLine();
    SohGui::mSohMenu->MenuDrawItem(tabHotkeyWidget, 200, THEME_COLOR);

    ImGui::TextColored(
        UIWidgets::ColorValues.at(UIWidgets::Colors::Yellow),
        "Mods are currently not reloaded at runtime. Close and re-open Ship for the changes to take effect.\n"
        "Drag ordering for the enabled list is available.\nMod priority is top to bottom. They override mods listed "
        "below them.");

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
            UpdateModFiles(false, true);
        }
        ImGui::SameLine();
        if (UIWidgets::Button("Clear List", UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline))) {
            SohGui::RegisterPopup("Clear List",
                                  "Clear the current mod list and force a rebuild on next boot.\nClick Apply & Close "
                                  "to save this change.",
                                  "Clear", "Cancel", [&]() {
                                      enabledModFiles.clear();
                                      ClearSelectedMods();
                                      AfterModChange();
                                  });
        }
        ImGui::SameLine();
        if (UIWidgets::Button("Apply & Close",
                              UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline).Color(THEME_COLOR))) {
            SohGui::RegisterPopup("Apply & Close",
                                  "Application currently requires a restart. Save the mod info and close SoH?", "Close",
                                  "Cancel", [&]() {
                                      // TODO: runtime changes
                                      SetEnabledModsCVarValue();
                                      // TODO: runtime changes
                                      /*
                                      gfx_texture_cache_clear();
                                      SOH::SkeletonPatcher::ClearSkeletons();
                                      */
                                      Ship::Context::GetRawInstance()->GetConsoleVariables()->Save();
                                      Ship::Context::GetRawInstance()->GetWindow()->Close();
                                  });
        }
    }
    ImGui::BeginDisabled(!editing);
    if (ImGui::BeginTable("tableMods", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Enabled Mods", ImGuiTableColumnFlags_WidthStretch, 200.0f);
        // ImGui::TableSetupColumn("Disabled Mods", ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::TableHeadersRow();
        ImGui::PopItemFlag();
        ImGui::TableNextRow();

        ImGui::TableNextColumn();

        if (ImGui::BeginChild("Enabled Mods", ImVec2(0, -8))) {
            DrawMods(true);

            ImGui::EndChild();
        }

        /*ImGui::TableNextColumn();

        if (ImGui::BeginChild("Disabled Mods", ImVec2(0, -8))) {
            DrawMods(false);

            ImGui::EndChild();
        }*/

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
        .RaceDisable(false)
        .Options(UIWidgets::CheckboxOptions({ { .disabledTooltip = "Temporarily disabled while editing mods list." } })
                     .Color(THEME_COLOR)
                     .Tooltip("Toggle mods. For graphics mods, this means toggling between default and mod graphics.")
                     .DefaultValue(true))
        .PreFunc([&](WidgetInfo& info) {
            auto options = std::static_pointer_cast<UIWidgets::CheckboxOptions>(info.options);
            options->disabled = editing;
        });
    SohGui::mSohMenu->AddSearchWidget({ enableModsWidget, "Settings", "Mod Menu", "Top", "alternate assets" });

    tabHotkeyWidget = { .name = "Mods Tab Hotkey", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    tabHotkeyWidget.CVar(CVAR_SETTING("Mods.AlternateAssetsHotkey"))
        .RaceDisable(false)
        .Options(UIWidgets::CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("Allows pressing the Tab key to toggle mods")
                     .DefaultValue(true));
    SohGui::mSohMenu->AddSearchWidget(
        { tabHotkeyWidget, "Settings", "Mod Menu", "Top", "alternate assets tab hotkey" });
}

static RegisterMenuInitFunc menuInitFunc(RegisterModMenuWidgets);
