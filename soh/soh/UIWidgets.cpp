//
//  UIWidgets.cpp
//  soh
//
//  Created by David Chavez on 25.08.22.
//

#include "UIWidgets.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imgui_internal.h>
#include <libultraship/libultraship.h>

#include <libultraship/libultra/types.h>
#include "soh/Enhancements/cosmetics/CosmeticsEditor.h"

namespace UIWidgets {

    // MARK: - Layout Helper

    // Automatically adds newlines to break up text longer than a specified number of characters
    // Manually included newlines will still be respected and reset the line length
    // If line is midword when it hits the limit, text should break at the last encountered space
    char* WrappedText(const char* text, unsigned int charactersPerLine) {
        std::string newText(text);
        const size_t tipLength = newText.length();
        int lastSpace = -1;
        int currentLineLength = 0;
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

    void SetLastItemHoverText(const std::string& text) {
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", WrappedText(text, 60));
            ImGui::EndTooltip();
        }
    }

    void SetLastItemHoverText(const char* text) {
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", WrappedText(text, 60));
            ImGui::EndTooltip();
        }
    }

    // Adds a "?" next to the previous ImGui item with a custom tooltip
    void InsertHelpHoverText(const std::string& text) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "?");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", WrappedText(text, 60));
            ImGui::EndTooltip();
        }
    }

    void InsertHelpHoverText(const char* text) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "?");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", WrappedText(text, 60));
            ImGui::EndTooltip();
        }
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
        if (padTop) {
            Spacer(extraVerticalTopPadding);
        }
        ImGui::Separator();
        if (padBottom) {
            Spacer(extraVerticalBottomPadding);
        }
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
        if (window->SkipItems) {
            return false;
        }

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
        if (g.LogEnabled) {
            ImGui::LogRenderedText(&label_pos, mixed_value ? "[~]" : *v ? "[x]" : "[ ]");
        }
        if (label_size.x > 0.0f) {
            ImGui::RenderText(label_pos, label);
        }

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
        return pressed;
    }

    void ReEnableComponent(const char* disabledTooltipText) {
        // End of disable region of previous component
        ImGui::PopStyleVar(1);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && strcmp(disabledTooltipText, "") != 0) {
            ImGui::SetTooltip("%s", disabledTooltipText);
        }
        ImGui::PopItemFlag();
    }

    void DisableComponent(const float alpha) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
    }

    bool EnhancementCheckbox(const char* text, const char* cvarName, bool disabled, const char* disabledTooltipText, CheckboxGraphics disabledGraphic, bool defaultValue) {
        bool changed = false;
        if (disabled) {
            DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
        }

        bool val = (bool)CVarGetInteger(cvarName, defaultValue);
        if (CustomCheckbox(text, &val, disabled, disabledGraphic)) {
            CVarSetInteger(cvarName, val);
            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            changed = true;
        }

        if (disabled) {
            ReEnableComponent(disabledTooltipText);
        }
        return changed;
    }

    bool PaddedEnhancementCheckbox(const char* text, const char* cvarName, bool padTop, bool padBottom, bool disabled, const char* disabledTooltipText, CheckboxGraphics disabledGraphic, bool defaultValue) {
        ImGui::BeginGroup();
        if (padTop) Spacer(0);

        bool changed = EnhancementCheckbox(text, cvarName, disabled, disabledTooltipText, disabledGraphic, defaultValue);

        if (padBottom) Spacer(0);
        ImGui::EndGroup();
        return changed;
    }

    bool EnhancementCombobox(const char* cvarName, std::span<const char*, std::dynamic_extent> comboArray, uint8_t defaultIndex, bool disabled, const char* disabledTooltipText, uint8_t disabledValue) {
        bool changed = false;
        if (defaultIndex <= 0) {
            defaultIndex = 0;
        }

        if (disabled) {
            DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
        }

        uint8_t selected = CVarGetInteger(cvarName, defaultIndex);
        std::string comboName = std::string("##") + std::string(cvarName);
        if (ImGui::BeginCombo(comboName.c_str(), comboArray[selected])) {
            for (uint8_t i = 0; i < comboArray.size(); i++) {
                if (strlen(comboArray[i]) > 1) {
                    if (ImGui::Selectable(comboArray[i], i == selected)) {
                        CVarSetInteger(cvarName, i);
                        selected = i;
                        changed = true;
                        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                    }
                }
            }
            ImGui::EndCombo();
        }

        if (disabled) {
            ReEnableComponent(disabledTooltipText);

            if (disabledValue >= 0 && selected != disabledValue) {
                CVarSetInteger(cvarName, disabledValue);
                changed = true;
                LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            }
        }

        return changed;
    }

    bool LabeledRightAlignedEnhancementCombobox(const char* label, const char* cvarName, std::span<const char*, std::dynamic_extent> comboArray, uint8_t defaultIndex, bool disabled, const char* disabledTooltipText, uint8_t disabledValue) {
        ImGui::Text("%s", label);
        s32 currentValue = CVarGetInteger(cvarName, defaultIndex);

#ifdef __WIIU__
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - (ImGui::CalcTextSize(comboArray[currentValue]).x + 40.0f));
        ImGui::PushItemWidth(ImGui::CalcTextSize(comboArray[currentValue]).x + 60.0f);
#else
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - (ImGui::CalcTextSize(comboArray[currentValue]).x + 20.0f));
        ImGui::PushItemWidth(ImGui::CalcTextSize(comboArray[currentValue]).x + 30.0f);
#endif

        bool changed = EnhancementCombobox(cvarName, comboArray, defaultIndex, disabled, disabledTooltipText, disabledValue);

        ImGui::PopItemWidth();
        return changed;
    }

    void PaddedText(const char* text, bool padTop, bool padBottom) {
        if (padTop) Spacer(0);

        ImGui::Text("%s", text);

        if (padBottom) Spacer(0);
    }

    bool EnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue, bool PlusMinusButton, bool disabled, const char* disabledTooltipText) {
        bool changed = false;
        int val = CVarGetInteger(cvarName, defaultValue);

        if (disabled) {
            DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
        }

        ImGui::Text(text, val);
        Spacer(0);

        ImGui::BeginGroup();
        if (PlusMinusButton) {
            std::string MinusBTNName = " - ##" + std::string(cvarName);
            if (ImGui::Button(MinusBTNName.c_str())) {
                val--;
                changed = true;
            }
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
        }

        ImGui::PushItemWidth(std::min((ImGui::GetContentRegionAvail().x - (PlusMinusButton ? sliderButtonWidth : 0.0f)), maxSliderWidth));
        if (ImGui::SliderInt(id, &val, min, max, format, ImGuiSliderFlags_AlwaysClamp))
        {
            changed = true;
        }
        ImGui::PopItemWidth();
        
        if (PlusMinusButton) {
            std::string PlusBTNName = " + ##" + std::string(cvarName);
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
            if (ImGui::Button(PlusBTNName.c_str())) {
                val++;
                changed = true;
            }
        }
        ImGui::EndGroup();

        if (disabled) {
            ReEnableComponent(disabledTooltipText);
        }

        if (val < min) {
            val = min;
            changed = true;
        }

        if (val > max) {
            val = max;
            changed = true;
        }

        if (changed) {
            CVarSetInteger(cvarName, val);
            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }

        return changed;
    }

    bool EnhancementSliderFloat(const char* text, const char* id, const char* cvarName, float min, float max, const char* format, float defaultValue, bool isPercentage, bool PlusMinusButton, bool disabled, const char* disabledTooltipText) {
        bool changed = false;
        float val = CVarGetFloat(cvarName, defaultValue);

        if (disabled) {
            DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
        }

        if (!isPercentage) {
            ImGui::Text(text, val);
        } else {
            ImGui::Text(text, static_cast<int>(100 * val));
        }
        Spacer(0);

        ImGui::BeginGroup();
        if (PlusMinusButton) {
            std::string MinusBTNName = " - ##" + std::string(cvarName);
            if (ImGui::Button(MinusBTNName.c_str())) {
                if (isPercentage) {
                    val -= 0.01f;
                } else {
                    val -= 0.1f;
                }
                changed = true;
            }
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
        }

        ImGui::PushItemWidth(std::min((ImGui::GetContentRegionAvail().x - (PlusMinusButton ? sliderButtonWidth : 0.0f)), maxSliderWidth));
        if (ImGui::SliderFloat(id, &val, min, max, format, ImGuiSliderFlags_AlwaysClamp)) {
            if (isPercentage) {
                val = roundf(val * 100) / 100;
            }
            changed = true;
        }
        ImGui::PopItemWidth();
        
        if (PlusMinusButton) {
            std::string PlusBTNName = " + ##" + std::string(cvarName);
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
            if (ImGui::Button(PlusBTNName.c_str())) {
                if (isPercentage) {
                    val += 0.01f;
                } else {
                    val += 0.1f;
                }
                changed = true;
            }
        }
        ImGui::EndGroup();

        if (disabled) {
            ReEnableComponent(disabledTooltipText);
        }

        if (val < min) {
            val = min;
            changed = true;
        }

        if (val > max) {
            val = max;
            changed = true;
        }

        if (changed) {
            CVarSetFloat(cvarName, val);
            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }

        return changed;
    }

    bool PaddedEnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue, bool PlusMinusButton, bool padTop, bool padBottom, bool disabled, const char* disabledTooltipText) {
        bool changed = false;
        ImGui::BeginGroup();
        if (padTop) Spacer(0);

        changed = EnhancementSliderInt(text, id, cvarName, min, max, format, defaultValue, PlusMinusButton, disabled, disabledTooltipText);

        if (padBottom) Spacer(0);
        ImGui::EndGroup();
        return changed;
    }

    bool PaddedEnhancementSliderFloat(const char* text, const char* id, const char* cvarName, float min, float max, const char* format, float defaultValue, bool isPercentage, bool PlusMinusButton, bool padTop, bool padBottom, bool disabled, const char* disabledTooltipText) {
        bool changed = false;
        ImGui::BeginGroup();
        if (padTop) Spacer(0);

        changed = EnhancementSliderFloat(text, id, cvarName, min, max, format, defaultValue, isPercentage, PlusMinusButton, disabled, disabledTooltipText);

        if (padBottom) Spacer(0);
        ImGui::EndGroup();
        return changed;
    }

    bool EnhancementRadioButton(const char* text, const char* cvarName, int id) {
        /*Usage :
        EnhancementRadioButton("My Visible Name","gMyCVarName", MyID);
        First arg is the visible name of the Radio button
        Second is the cvar name where MyID will be saved.
        Note: the CVar name should be the same to each Buddies.
        Example :
            EnhancementRadioButton("English", "gLanguages", LANGUAGE_ENG);
            EnhancementRadioButton("German", "gLanguages", LANGUAGE_GER);
            EnhancementRadioButton("French", "gLanguages", LANGUAGE_FRA);
        */
        std::string make_invisible = "##" + std::string(text) + std::string(cvarName);

        bool ret = false;
        int val = CVarGetInteger(cvarName, 0);
        if (ImGui::RadioButton(make_invisible.c_str(), id == val)) {
            CVarSetInteger(cvarName, id);
            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            ret = true;
        }
        ImGui::SameLine();
        ImGui::Text("%s", text);
        
        return ret;
    }

    bool DrawResetColorButton(const char* cvarName, ImVec4* colors, ImVec4 defaultcolors, bool has_alpha) {
        bool changed = false;
        std::string Cvar_RBM = std::string(cvarName) + "RBM";
        std::string MakeInvisible = "Reset##" + std::string(cvarName) + "Reset";
        if (ImGui::Button(MakeInvisible.c_str())) {
            colors->x = defaultcolors.x;
            colors->y = defaultcolors.y;
            colors->z = defaultcolors.z;
            colors->w = has_alpha ? defaultcolors.w : 255.0f;

            Color_RGBA8 colorsRGBA;
            colorsRGBA.r = defaultcolors.x;
            colorsRGBA.g = defaultcolors.y;
            colorsRGBA.b = defaultcolors.z;
            colorsRGBA.a = has_alpha ? defaultcolors.w : 255.0f;

            CVarSetColor(cvarName, colorsRGBA);
            CVarSetInteger(Cvar_RBM.c_str(), 0); //On click disable rainbow mode.
            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            changed = true;
        }
        Tooltip("Revert colors to the game's original colors (GameCube version)\nOverwrites previously chosen color");
        return changed;
    }

    bool DrawRandomizeColorButton(const char* cvarName, ImVec4* colors) {
        bool changed = false;
        Color_RGBA8 NewColors = {0,0,0,255};
        std::string Cvar_RBM = std::string(cvarName) + "RBM";
        std::string FullName = "Random##" + std::string(cvarName) + "Random";
        if (ImGui::Button(FullName.c_str())) {
#if defined(__SWITCH__) || defined(__WIIU__)
            srand(time(NULL));
#endif
            ImVec4 color = GetRandomValue(255);
            colors->x = color.x;
            colors->y = color.y;
            colors->z = color.z;
            NewColors.r = fmin(fmax(colors->x * 255, 0), 255);
            NewColors.g = fmin(fmax(colors->y * 255, 0), 255);
            NewColors.b = fmin(fmax(colors->z * 255, 0), 255);
            CVarSetColor(cvarName, NewColors);
            CVarSetInteger(Cvar_RBM.c_str(), 0); // On click disable rainbow mode.
            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            changed = true;
        }
        Tooltip("Chooses a random color\nOverwrites previously chosen color");
        return changed;
    }

    void DrawLockColorCheckbox(const char* cvarName) {
        std::string Cvar_Lock = std::string(cvarName) + "Lock";
        s32 lock = CVarGetInteger(Cvar_Lock.c_str(), 0);
        std::string FullName = "Lock##" + Cvar_Lock;
        EnhancementCheckbox(FullName.c_str(), Cvar_Lock.c_str());
        Tooltip("Prevents this color from being changed upon selecting \"Randomize all\"");
    }

    void RainbowColor(const char* cvarName, ImVec4* colors) {
        std::string Cvar_RBM = std::string(cvarName) + "RBM";
        std::string MakeInvisible = "Rainbow##" + std::string(cvarName) + "Rainbow";

        EnhancementCheckbox(MakeInvisible.c_str(), Cvar_RBM.c_str());
        Tooltip("Cycles through colors on a timer\nOverwrites previously chosen color");
    }

    void LoadPickersColors(ImVec4& ColorArray, const char* cvarname, const ImVec4& default_colors, bool has_alpha) {
        Color_RGBA8 defaultColors;
        defaultColors.r = default_colors.x;
        defaultColors.g = default_colors.y;
        defaultColors.b = default_colors.z;
        defaultColors.a = default_colors.w;

        Color_RGBA8 cvarColor = CVarGetColor(cvarname, defaultColors);

        ColorArray.x = cvarColor.r / 255.0;
        ColorArray.y = cvarColor.g / 255.0;
        ColorArray.z = cvarColor.b / 255.0;
        ColorArray.w = cvarColor.a / 255.0;
    }

    bool EnhancementColor(const char* text, const char* cvarName, ImVec4 ColorRGBA, ImVec4 default_colors, bool allow_rainbow, bool has_alpha, bool TitleSameLine) {
        bool changed = false;
        LoadPickersColors(ColorRGBA, cvarName, default_colors, has_alpha);

        ImGuiColorEditFlags flags = ImGuiColorEditFlags_None;

        if (!TitleSameLine) {
            ImGui::Text("%s", text);
            flags = ImGuiColorEditFlags_NoLabel;
        }

        ImGui::PushID(cvarName);

        if (!has_alpha) {
            if (ImGui::ColorEdit3(text, (float*)&ColorRGBA, flags))
            {
                Color_RGBA8 colors;
                colors.r = ColorRGBA.x * 255.0;
                colors.g = ColorRGBA.y * 255.0;
                colors.b = ColorRGBA.z * 255.0;
                colors.a = 255.0;

                CVarSetColor(cvarName, colors);
                LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                changed = true;
            }
        }
        else
        {
            flags |= ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview;
            if (ImGui::ColorEdit4(text, (float*)&ColorRGBA, flags))
            {
                Color_RGBA8 colors;
                colors.r = ColorRGBA.x * 255.0;
                colors.g = ColorRGBA.y * 255.0;
                colors.b = ColorRGBA.z * 255.0;
                colors.a = ColorRGBA.w * 255.0;

                CVarSetColor(cvarName, colors);
                LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                changed = true;
            }
        }

        ImGui::PopID();

        //ImGui::SameLine(); // Removing that one to gain some width spacing on the HUD editor
        ImGui::PushItemWidth(-FLT_MIN);
        if (DrawResetColorButton(cvarName, &ColorRGBA, default_colors, has_alpha)) {
            changed = true;
        }
        ImGui::SameLine();
        if (DrawRandomizeColorButton(cvarName, &ColorRGBA)) {
            changed = true;
        }
        if (allow_rainbow) {
            if (ImGui::GetContentRegionAvail().x > 185) {
                ImGui::SameLine();
            }
            RainbowColor(cvarName, &ColorRGBA);
        }
        DrawLockColorCheckbox(cvarName);
        ImGui::NewLine();
        ImGui::PopItemWidth();

        return changed;
    }

    void DrawFlagArray32(const std::string& name, uint32_t& flags) {
        ImGui::PushID(name.c_str());
        for (int32_t flagIndex = 0; flagIndex < 32; flagIndex++) {
            if ((flagIndex % 8) != 0) {
                ImGui::SameLine();
            }
            ImGui::PushID(flagIndex);
            uint32_t bitMask = 1 << flagIndex;
            bool flag = (flags & bitMask) != 0;
            if (ImGui::Checkbox("##check", &flag)) {
                if (flag) {
                    flags |= bitMask;
                } else {
                    flags &= ~bitMask;
                }
            }
            ImGui::PopID();
        }
        ImGui::PopID();
    }

    void DrawFlagArray16(const std::string& name, uint16_t& flags) {
        ImGui::PushID(name.c_str());
        for (int16_t flagIndex = 0; flagIndex < 16; flagIndex++) {
            if ((flagIndex % 8) != 0) {
                ImGui::SameLine();
            }
            ImGui::PushID(flagIndex);
            uint16_t bitMask = 1 << flagIndex;
            bool flag = (flags & bitMask) != 0;
            if (ImGui::Checkbox("##check", &flag)) {
                if (flag) {
                    flags |= bitMask;
                } else {
                    flags &= ~bitMask;
                }
            }
            ImGui::PopID();
        }
        ImGui::PopID();
    }

    void DrawFlagArray8(const std::string& name, uint8_t& flags) {
        ImGui::PushID(name.c_str());
        for (int8_t flagIndex = 0; flagIndex < 8; flagIndex++) {
            if ((flagIndex % 8) != 0) {
                ImGui::SameLine();
            }
            ImGui::PushID(flagIndex);
            uint8_t bitMask = 1 << flagIndex;
            bool flag = (flags & bitMask) != 0;
            if (ImGui::Checkbox("##check", &flag)) {
                if (flag) {
                    flags |= bitMask;
                } else {
                    flags &= ~bitMask;
                }
            }
            ImGui::PopID();
        }
        ImGui::PopID();
    }
}
