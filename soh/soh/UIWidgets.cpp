//
//  UIWidgets.cpp
//  soh
//
//  Created by David Chavez on 25.08.22.
//

#include "UIWidgets.hpp"

#include <ImGui/imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imgui_internal.h>
#include <libultraship/ImGuiImpl.h>
#include <libultraship/Cvar.h>

namespace UIWidgets {

    // MARK: - Layout Helper

    // Automatically adds newlines to break up text longer than a specified number of characters
    // Manually included newlines will still be respected and reset the line length
    // If line is midword when it hits the limit, text should break at the last encountered space
    char* WrappedText(const char* text, unsigned int charactersPerLine) {
        std::string newText(text);
        const size_t tipLength = newText.length();
        int lastSpace = -1;
        size_t currentLineLength = 0;
        for (unsigned int currentCharacter = 0; currentCharacter < tipLength; currentCharacter++) {
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

    char* WrappedText(const std::string& text, unsigned int charactersPerLine) {
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

    void PaddedSeparator(bool padTop, bool padBottom, float extraVerticalTopPadding, float extraVerticalBottomPadding) {
        if (padTop)
            Spacer(0);

        ImGui::Separator();

        if (padBottom)
            Spacer(0);
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

    void EnhancementCheckbox(const char* text, const char* cvarName, bool disabled, const char* disabledTooltipText, CheckboxGraphics disabledGraphic) {
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

    void PaddedEnhancementCheckbox(const char* text, const char* cvarName, bool padTop, bool padBottom, bool disabled, const char* disabledTooltipText, CheckboxGraphics disabledGraphic) {
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

    void PaddedText(const char* text, bool padTop, bool padBottom) {
        if (padTop)
            Spacer(0);

        ImGui::Text("%s", text);

        if (padBottom)
            Spacer(0);
    }

    void EnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue, bool PlusMinusButton) {
        int val = CVar_GetS32(cvarName, defaultValue);
        ImGui::Text(text, val);
        if(PlusMinusButton) {
            std::string MinusBTNName = " - ##";
            MinusBTNName += cvarName;
            if (ImGui::Button(MinusBTNName.c_str())) {
                val--;
                CVar_SetS32(cvarName, val);
                SohImGui::RequestCvarSaveOnNextTick();
            }
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
        }

        if (ImGui::SliderInt(id, &val, min, max, format))
        {
            CVar_SetS32(cvarName, val);
            SohImGui::RequestCvarSaveOnNextTick();
        }

        if(PlusMinusButton) {
            std::string PlusBTNName = " + ##";
            PlusBTNName += cvarName;
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
            if (ImGui::Button(PlusBTNName.c_str())) {
                val++;
                CVar_SetS32(cvarName, val);
                SohImGui::RequestCvarSaveOnNextTick();
            }
        }

        if (val < min)
        {
            val = min;
            CVar_SetS32(cvarName, val);
            SohImGui::RequestCvarSaveOnNextTick();
        }

        if (val > max)
        {
            val = max;
            CVar_SetS32(cvarName, val);
            SohImGui::RequestCvarSaveOnNextTick();
        }
    }

    void PaddedEnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue, bool PlusMinusButton, bool padTop, bool padBottom) {
        if (padTop)
            Spacer(0);

        EnhancementSliderInt(text, id, cvarName, min, max, format, defaultValue, PlusMinusButton);

        if (padBottom)
            Spacer(0);
    }

    void EnhancementRadioButton(const char* text, const char* cvarName, int id) {
        /*Usage :
        EnhancementRadioButton("My Visible Name","gMyCVarName", MyID);
        First arg is the visible name of the Radio button
        Second is the cvar name where MyID will be saved.
        Note: the CVar name should be the same to each Buddies.
        Example :
            EnhancementRadioButton("English", "gLanguages", 0);
            EnhancementRadioButton("German", "gLanguages", 1);
            EnhancementRadioButton("French", "gLanguages", 2);
        */
        std::string make_invisible = "##";
        make_invisible += text;
        make_invisible += cvarName;

        int val = CVar_GetS32(cvarName, 0);
        if (ImGui::RadioButton(make_invisible.c_str(), id == val)) {
            CVar_SetS32(cvarName, id);
            SohImGui::RequestCvarSaveOnNextTick();
        }
        ImGui::SameLine();
        ImGui::Text("%s", text);
    }
}
