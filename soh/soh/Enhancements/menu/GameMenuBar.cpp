//
//  GameMenuBar.cpp
//  soh
//
//  Created by David Chavez on 24.08.22.
//

#include "GameMenuBar.hpp"

#include <spdlog/spdlog.h>
#include <ImGui/imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imgui_internal.h>
#include <libultraship/ImGuiImpl.h>
#include <libultraship/Cvar.h>

namespace UIWidgets {

    // MARK: - Enums

    enum class CheckboxGraphics {
        Cross,
        Checkmark,
        None
    };

    // MARK: - Layout Helper

    // Automatically adds newlines to break up text longer than a specified number of characters
    // Manually included newlines will still be respected and reset the line length
    // If line is midword when it hits the limit, text should break at the last encountered space
    char* WrappedText(const char* text, uint charactersPerLine = 60) {
        std::string newText(text);
        const size_t tipLength = newText.length();
        int lastSpace = -1;
        size_t currentLineLength = 0;
        for (uint currentCharacter = 0; currentCharacter < tipLength; currentCharacter++) {
            if (newText[currentCharacter] == '\n') {
                currentLineLength = 0;
                lastSpace = -1;
                continue;
            } else if (newText[currentCharacter] == ' ') {
                lastSpace = currentCharacter;
            }

            if ((currentLineLength >= charactersPerLine) && (lastSpace >= 0)) {
                newText[lastSpace] = '\n';
                currentLineLength = currentCharacter - lastSpace - 1;
                lastSpace = -1;
            }
            currentLineLength++;
        }
        
        return strdup(newText.c_str());
    }

    char* WrappedText(const std::string& text, uint charactersPerLine) {
        return WrappedText(text.c_str(), charactersPerLine);
    }


    // MARK: - UI Elements

    void Tooltip(const char* text) {
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", WrappedText(text));
        }
    }

    void Spacer(float height) {
        ImGui::Dummy(ImVec2(0.0f, height));
    }

    void Separator(float paddingTop = 0, float paddingBottom = 0) {
        if (paddingTop > 0)
            Spacer(paddingTop);

        ImGui::Separator();

        if (paddingBottom > 0)
            Spacer(paddingBottom);
    }

    void RenderCross(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float sz) {
        float thickness = ImMax(sz / 5.0f, 1.0f);
        sz -= thickness * 0.5f;
        pos += ImVec2(thickness * 0.25f, thickness * 0.25f);

        draw_list->PathLineTo(ImVec2(pos.x, pos.y));
        draw_list->PathLineTo(ImVec2(pos.x + sz, pos.y + sz));
        draw_list->PathStroke(col, 0, thickness);

        draw_list->PathLineTo(ImVec2(pos.x + sz, pos.y));
        draw_list->PathLineTo(ImVec2(pos.x, pos.y + sz));
        draw_list->PathStroke(col, 0, thickness);
    }

    bool CustomCheckbox(const char* label, bool* v, bool disabled, CheckboxGraphics disabledGraphic) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

        const float square_sz = ImGui::GetFrameHeight();
        const ImVec2 pos = window->DC.CursorPos;
        const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
        ImGui::ItemSize(total_bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(total_bb, id)) {
            IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
            return false;
        }

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
        if (pressed) {
            *v = !(*v);
            ImGui::MarkItemEdited(id);
        }

        const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
        ImGui::RenderNavHighlight(total_bb, id);
        ImGui::RenderFrame(check_bb.Min, check_bb.Max, ImGui::GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
        ImU32 check_col = ImGui::GetColorU32(ImGuiCol_CheckMark);
        ImU32 cross_col = ImGui::GetColorU32(ImVec4(0.50f, 0.50f, 0.50f, 1.00f));
        bool mixed_value = (g.LastItemData.InFlags & ImGuiItemFlags_MixedValue) != 0;
        if (mixed_value) {
            // Undocumented tristate/mixed/indeterminate checkbox (#2644)
            // This may seem awkwardly designed because the aim is to make ImGuiItemFlags_MixedValue supported by all widgets (not just checkbox)
            ImVec2 pad(ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)), ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)));
            window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col, style.FrameRounding);
        } else if ((!disabled && *v) || (disabled && disabledGraphic == CheckboxGraphics::Checkmark)) {
            const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
            ImGui::RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), check_col, square_sz - pad * 2.0f);
        } else if (disabled && disabledGraphic == CheckboxGraphics::Cross) {
            const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
            RenderCross(window->DrawList, check_bb.Min + ImVec2(pad, pad), cross_col, square_sz - pad * 2.0f);
        }

        ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y);
        if (g.LogEnabled)
            ImGui::LogRenderedText(&label_pos, mixed_value ? "[~]" : *v ? "[x]" : "[ ]");
        if (label_size.x > 0.0f)
            ImGui::RenderText(label_pos, label);

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
        return pressed;
    }

    void EnhancementCheckbox(const char* text, const char* cvarName, bool disabled = false, const char* disabledTooltipText = "", CheckboxGraphics disabledGraphic = CheckboxGraphics::Cross) {
        if (disabled) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        bool val = (bool)CVar_GetS32(cvarName, 0);
        if (CustomCheckbox(text, &val, disabled, disabledGraphic)) {
            CVar_SetS32(cvarName, val);
            SohImGui::RequestCvarSaveOnNextTick();
        }

        if (disabled) {
            ImGui::PopStyleVar(1);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && strcmp(disabledTooltipText, "") != 0) {
                ImGui::SetTooltip("%s", disabledTooltipText);
            }
            ImGui::PopItemFlag();
        }
    }

    void PaddedEnhancementCheckbox(const char* text, const char* cvarName, bool padTop = true, bool padBottom = true, bool disabled = false, const char* disabledTooltipText = "", CheckboxGraphics disabledGraphic = CheckboxGraphics::Cross) {
        if (padTop) Spacer(0);

        EnhancementCheckbox(text, cvarName, disabled, disabledTooltipText, disabledGraphic);

        if (padBottom) Spacer(0);
    }

    void EnhancementCombobox(const char* name, const char* ComboArray[], size_t arraySize, uint8_t FirstTimeValue) {
        if (FirstTimeValue <= 0) {
            FirstTimeValue = 0;
        }
        uint8_t selected = CVar_GetS32(name, FirstTimeValue);
        uint8_t DefaultValue = selected;
        std::string comboName = std::string("##") + std::string(name);
        if (ImGui::BeginCombo(comboName.c_str(), ComboArray[DefaultValue])) {
            for (uint8_t i = 0; i < arraySize; i++) {
                if (strlen(ComboArray[i]) > 1) {
                    if (ImGui::Selectable(ComboArray[i], i == selected)) {
                        CVar_SetS32(name, i);
                        selected = i;
                        SohImGui::RequestCvarSaveOnNextTick();
                    }
                }
            }
            ImGui::EndCombo();
        }
    }
}

namespace GameMenuBar {

    // MARK: - Properties


    // MARK: - Helpers

    std::string GetWindowButtonText(const char* text, bool menuOpen) {
        char buttonText[100] = "";
        if(menuOpen) { strcat(buttonText,"> "); }
        strcat(buttonText, text);
        if (!menuOpen) { strcat(buttonText, "  "); }
        return buttonText;
    }

    // MARK: - Delegates

    void Draw() {
        if (ImGui::BeginMenu("Developer Tools"))
        {
            UIWidgets::EnhancementCheckbox("OoT Debug Mode", "gDebugEnabled");
            UIWidgets::Tooltip("Enables Debug Mode, allowing you to select maps with L + R + Z, noclip with L + D-pad Right, and open the debug menu with L on the pause screen");
            UIWidgets::PaddedEnhancementCheckbox("OoT Skulltula Debug", "gSkulltulaDebugEnabled", true, false);
            UIWidgets::Tooltip("Enables Skulltula Debug, when moving the cursor in the menu above various map icons (boss key, compass, map screen locations, etc) will set the GS bits in that area.\nUSE WITH CAUTION AS IT DOES NOT UPDATE THE GS COUNT.");
            UIWidgets::PaddedEnhancementCheckbox("Fast File Select", "gSkipLogoTitle", true, false);
            UIWidgets::Tooltip("Load the game to the selected menu or file\n\"Zelda Map Select\" require debug mode else you will fallback to File choose menu\nUsing a file number that don't have save will create a save file only if you toggle on \"Create a new save if none ?\" else it will bring you to the File choose menu");
            if (CVar_GetS32("gSkipLogoTitle", 0)) {
                const char* FastFileSelect[5] = {
                    "File N.1",
                    "File N.2",
                    "File N.3",
                    "File select",
                    "Zelda Map Select (require OoT Debug Mode)"
                };
                ImGui::Text("Loading :");
                UIWidgets::EnhancementCombobox("gSaveFileID", FastFileSelect, 5, 0);
                UIWidgets::PaddedEnhancementCheckbox("Create a new save if none", "gCreateNewSave", true, false);
                UIWidgets::Tooltip("Enable the creation of a new save file if none exist in the File number selected\nNo file name will be assigned please do in Save editor once you see the first text else your save file name will be named \"00000000\"\nIf disabled you will fall back in File select menu");
            };
            UIWidgets::Separator();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0,0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
            static ImVec2 buttonSize(160.0f, 0.0f);
            if (ImGui::Button(GetWindowButtonText("Stats", CVar_GetS32("gStatsEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gStatsEnabled", 0);
                CVar_SetS32("gStatsEnabled", !currentValue);
                SohImGui::ToggleStatisticsWindow(true);
                SohImGui::RequestCvarSaveOnNextTick();
            }
            UIWidgets::Tooltip("Shows the stats window, with your FPS and frametimes, and the OS you're playing on");
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Console", CVar_GetS32("gConsoleEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gConsoleEnabled", 0);
                CVar_SetS32("gConsoleEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::ToggleConsoleWindow(!currentValue);
            }
            UIWidgets::Tooltip("Enables the console window, allowing you to input commands, type help for some examples");
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Save Editor", CVar_GetS32("gSaveEditorEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gSaveEditorEnabled", 0);
                CVar_SetS32("gSaveEditorEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Save Editor", CVar_GetS32("gSaveEditorEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Collision Viewer", CVar_GetS32("gCollisionViewerEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gCollisionViewerEnabled", 0);
                CVar_SetS32("gCollisionViewerEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Collision Viewer", CVar_GetS32("gCollisionViewerEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Actor Viewer", CVar_GetS32("gActorViewerEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gActorViewerEnabled", 0);
                CVar_SetS32("gActorViewerEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Actor Viewer", CVar_GetS32("gActorViewerEnabled", 0));
            }
            ImGui::PopStyleVar(3);
            ImGui::PopStyleColor(1);

            ImGui::EndMenu();
        }

        ImGui::SetCursorPosY(0.0f);

        if (ImGui::BeginMenu("Randomizer"))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
            static ImVec2 buttonSize(200.0f, 0.0f);
            if (ImGui::Button(GetWindowButtonText("Randomizer Settings", CVar_GetS32("gRandomizerSettingsEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gRandomizerSettingsEnabled", 0);
                CVar_SetS32("gRandomizerSettingsEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Randomizer Settings", CVar_GetS32("gRandomizerSettingsEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Item Tracker", CVar_GetS32("gItemTrackerEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gItemTrackerEnabled", 0);
                CVar_SetS32("gItemTrackerEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Item Tracker", CVar_GetS32("gItemTrackerEnabled", 0));
            }
            ImGui::PopStyleVar(3);
            ImGui::PopStyleColor(1);

            ImGui::EndMenu();
        }
    }
}
