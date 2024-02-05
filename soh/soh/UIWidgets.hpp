//
//  UIWidgets.hpp
//  soh
//
//  Created by David Chavez on 25.08.22.
//

#ifndef UIWidgets_hpp
#define UIWidgets_hpp

#include <string>
#include <sstream>
#include <vector>
#include <span>
#include <stdint.h>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <ImGui/imgui.h>
#include <libultraship/libultraship.h>

namespace UIWidgets {

    struct TextFilters {
        static int FilterNumbers(ImGuiInputTextCallbackData* data) {
            if (data->EventChar < 256 && strchr("1234567890", (char)data->EventChar)) {
                return 0;
            }
            return 1;
        }

        static int FilterAlphaNum(ImGuiInputTextCallbackData* data) {
            const char* alphanum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWYZ0123456789";
            if (data->EventChar < 256 && strchr(alphanum, (char)data->EventChar)) {
                return 0;
            }
            return 1;
        }
        
    };

    // MARK: - Enums

    enum class CheckboxGraphics {
        Cross,
        Checkmark,
        None
    };
    constexpr float maxSliderWidth = 260.0f;
#ifdef __SWITCH__
    constexpr float sliderButtonWidth = 42.0f;
#elif defined(__WIIU__)
    constexpr float sliderButtonWidth = 60.0f;
#else
    constexpr float sliderButtonWidth = 30.0f;
#endif

    char* WrappedText(const char* text, unsigned int charactersPerLine = 60);
    char* WrappedText(const std::string& text, unsigned int charactersPerLine);

    void SetLastItemHoverText(const std::string& text);
    void SetLastItemHoverText(const char* text);

    void InsertHelpHoverText(const std::string& text);
    void InsertHelpHoverText(const char* text);

    void Tooltip(const char* text);
    void Spacer(float height);
    void PaddedSeparator(bool padTop = true, bool padBottom = true, float extraVerticalTopPadding = 0.0f, float extraVerticalBottomPadding = 0.0f);

    void RenderCross(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float sz);
    bool CustomCheckbox(const char* label, bool* v, bool disabled = false, CheckboxGraphics disabledGraphic = CheckboxGraphics::Cross);

    void ReEnableComponent(const char* disabledTooltipText);
    void DisableComponent(const float alpha);

    bool EnhancementCheckbox(const char* text, const char* cvarName, bool disabled = false, const char* disabledTooltipText = "", CheckboxGraphics disabledGraphic = CheckboxGraphics::Cross, bool defaultValue = false);
    bool PaddedEnhancementCheckbox(const char* text, const char* cvarName, bool padTop = true, bool padBottom = true, bool disabled = false, const char* disabledTooltipText = "", CheckboxGraphics disabledGraphic = CheckboxGraphics::Cross, bool defaultValue = false);
    
    bool EnhancementCombobox(const char* cvarName, std::span<const char*, std::dynamic_extent> comboArray, uint8_t defaultIndex, bool disabled = false, const char* disabledTooltipText = "", uint8_t disabledValue = -1);
    bool LabeledRightAlignedEnhancementCombobox(const char* label, const char* cvarName, std::span<const char*, std::dynamic_extent> comboArray, uint8_t defaultIndex, bool disabled = false, const char* disabledTooltipText = "", uint8_t disabledValue = -1);
    
    void PaddedText(const char* text, bool padTop = true, bool padBottom = true);
    
    bool EnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue = 0, bool PlusMinusButton = true, bool disabled = false, const char* disabledTooltipText = "");
    bool PaddedEnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue = 0, bool PlusMinusButton = true, bool padTop = true, bool padBottom = true, bool disabled = false, const char* disabledTooltipText = "");
    bool EnhancementSliderFloat(const char* text, const char* id, const char* cvarName, float min, float max, const char* format, float defaultValue, bool isPercentage, bool PlusMinusButton = true, bool disabled = false, const char* disabledTooltipText = "");
    bool PaddedEnhancementSliderFloat(const char* text, const char* id, const char* cvarName, float min, float max, const char* format, float defaultValue, bool isPercentage, bool PlusMinusButton = true, bool padTop = true, bool padBottom = true, bool disabled = false, const char* disabledTooltipText = "");
    
    bool EnhancementRadioButton(const char* text, const char* cvarName, int id);

    bool DrawResetColorButton(const char* cvarName, ImVec4* colors, ImVec4 defaultcolors, bool has_alpha);
    bool DrawRandomizeColorButton(const char* cvarName, ImVec4* colors);
    void DrawLockColorCheckbox(const char* cvarName);
    void RainbowColor(const char* cvarName, ImVec4* colors);

    void LoadPickersColors(ImVec4& ColorArray, const char* cvarname, const ImVec4& default_colors, bool has_alpha);
    bool EnhancementColor(const char* text, const char* cvarName, ImVec4 ColorRGBA, ImVec4 default_colors, bool allow_rainbow = true, bool has_alpha = false, bool TitleSameLine = false);

    void DrawFlagArray32(const std::string& name, uint32_t& flags);
    void DrawFlagArray16(const std::string& name, uint16_t& flags);
    void DrawFlagArray8(const std::string& name, uint8_t& flags);
    bool StateButton(const char* str_id, const char* label);

    // UIWidgets V2

    namespace Colors {
        const ImVec4 White = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        const ImVec4 Gray = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        const ImVec4 DarkGray = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        const ImVec4 Indigo = ImVec4(0.24f, 0.31f, 0.71f, 1.0f);
        const ImVec4 Red = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);
        const ImVec4 DarkRed = ImVec4(0.3f, 0.0f, 0.0f, 1.0f);
        const ImVec4 LightGreen = ImVec4(0.0f, 0.7f, 0.0f, 1.0f);
        const ImVec4 Green = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
        const ImVec4 DarkGreen = ImVec4(0.0f, 0.3f, 0.0f, 1.0f);
        const ImVec4 Yellow = ImVec4(1.0f, 0.627f, 0.0f, 1.0f);
        const ImVec4 Blue = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
        const ImVec4 Purple = ImVec4(0.5f, 0.0f, 1.0f, 1.0f);
    };

    namespace Sizes {
        const ImVec2 Inline = ImVec2(0.0f, 0.0f);
        const ImVec2 Fill = ImVec2(-1.0f, 0.0f);
    }

    enum LabelPosition {
        Near,
        Far,
        Above,
        None,
        Within,
    };

    enum ComponentAlignment {
        Left,
        Right,
    };

    void PushStyleMenu(const ImVec4& color = Colors::Indigo);
    void PopStyleMenu();
    bool BeginMenu(const char* label, const ImVec4& color = Colors::Indigo);

    void PushStyleMenuItem(const ImVec4& color = Colors::Indigo);
    void PopStyleMenuItem();
    bool MenuItem(const char* label, const char* shortcut = NULL, const ImVec4& color = Colors::Indigo);

    struct ButtonOptions {
        const ImVec4 color = Colors::Gray;
        const ImVec2 size = Sizes::Fill;
        const char* tooltip = "";
        bool disabled = false;
        const char* disabledTooltip = "";
    };

    void PushStyleButton(const ImVec4& color = Colors::Gray);
    void PopStyleButton();
    bool Button(const char* label, const ButtonOptions& options = {});
    bool WindowButton(const char* label, const char* cvarName, std::shared_ptr<LUS::GuiWindow> windowPtr, const ButtonOptions& options = {});

    struct CheckboxOptions {
        const ImVec4 color = Colors::Indigo;
        const char* tooltip = "";
        bool disabled = false;
        const char* disabledTooltip = "";
        bool defaultValue = false; // Only applicable to CVarCheckbox
        ComponentAlignment alignment = ComponentAlignment::Left;
        LabelPosition labelPosition = LabelPosition::Near;
    };

    void PushStyleCheckbox(const ImVec4& color = Colors::Indigo);
    void PopStyleCheckbox();
    bool Checkbox(const char* label, bool* v, const CheckboxOptions& options = {});
    bool CVarCheckbox(const char* label, const char* cvarName, const CheckboxOptions& options = {});

    struct ComboboxOptions {
        const ImVec4 color = Colors::Indigo;
        const char* tooltip = "";
        bool disabled = false;
        const char* disabledTooltip = "";
        uint32_t defaultIndex = 0; // Only applicable to CVarCombobox
        ComponentAlignment alignment = ComponentAlignment::Left;
        LabelPosition labelPosition = LabelPosition::Above;
        ImGuiComboFlags flags = 0;
    };

    void PushStyleCombobox(const ImVec4& color = Colors::Indigo);
    void PopStyleCombobox();

    template <typename T>
    bool Combobox(const char* label, T* value, const std::unordered_map<T, const char*>& comboMap, const ComboboxOptions& options = {}) {
        bool dirty = false;
        float startX = ImGui::GetCursorPosX();
        std::string invisibleLabelStr = "##" + std::string(label);
        const char* invisibleLabel = invisibleLabelStr.c_str();
        ImGui::PushID(label);
        ImGui::BeginGroup();
        ImGui::BeginDisabled(options.disabled);
        PushStyleCombobox(options.color);
        if (options.alignment == ComponentAlignment::Left) {
            if (options.labelPosition == LabelPosition::Above) {
                ImGui::Text(label);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Near) {
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x - ImGui::GetStyle().ItemSpacing.x * 2);
            } else if (options.labelPosition == LabelPosition::Far || options.labelPosition == LabelPosition::None) {
                ImGui::SetNextItemWidth(ImGui::CalcTextSize(comboMap.at(*value)).x + ImGui::GetStyle().FramePadding.x * 4 + ImGui::GetStyle().ItemSpacing.x);
            }
        } else if (options.alignment == ComponentAlignment::Right) {
            if (options.labelPosition == LabelPosition::Above) {
                ImGui::NewLine();
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x);
                ImGui::Text(label);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Near) {
                ImGui::SameLine(ImGui::CalcTextSize(label).x + ImGui::GetStyle().ItemSpacing.x * 2);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Far || options.labelPosition == LabelPosition::None) {
                float width = ImGui::CalcTextSize(comboMap.at(*value)).x + ImGui::GetStyle().FramePadding.x * 4;
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - width);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            }
        }
        if (ImGui::BeginCombo(invisibleLabel, comboMap.at(*value), options.flags)) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f));
            for (const auto& pair : comboMap) {
                if (strlen(pair.second) > 1) {
                    if (ImGui::Selectable(pair.second, pair.first == *value)) {
                        *value = pair.first;
                        dirty = true;
                    }
                }
            }
            ImGui::PopStyleVar();
            ImGui::EndCombo();
        }
        if (options.alignment == ComponentAlignment::Left) {
            if (options.labelPosition == LabelPosition::Near) {
                ImGui::SameLine();
                ImGui::Text(label);
            } else if (options.labelPosition == LabelPosition::Far) {
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x);
                ImGui::Text(label);
            }
        } else if (options.alignment == ComponentAlignment::Right) {
            if (options.labelPosition == LabelPosition::Near || options.labelPosition == LabelPosition::Far) {
                ImGui::SameLine(startX);
                ImGui::Text(label);
            }
        }
        PopStyleCombobox();
        ImGui::EndDisabled();
        ImGui::EndGroup();
        if (options.disabled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && strcmp(options.disabledTooltip, "") != 0) {
            ImGui::SetTooltip("%s", WrappedText(options.disabledTooltip));
        } else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && strcmp(options.tooltip, "") != 0) {
            ImGui::SetTooltip("%s", WrappedText(options.tooltip));
        }
        ImGui::PopID();
        return dirty;
    }

    template <typename T = size_t>
    bool Combobox(const char* label, T* value, const std::vector<const char*>& comboVector, const ComboboxOptions& options = {}) {
        bool dirty = false;
        float startX = ImGui::GetCursorPosX();
        size_t currentValueIndex = static_cast<size_t>(*value);
        std::string invisibleLabelStr = "##" + std::string(label);
        const char* invisibleLabel = invisibleLabelStr.c_str();
        ImGui::PushID(label);
        ImGui::BeginGroup();
        ImGui::BeginDisabled(options.disabled);
        PushStyleCombobox(options.color);
        if (options.alignment == ComponentAlignment::Left) {
            if (options.labelPosition == LabelPosition::Above) {
                ImGui::Text(label);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Near) {
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x - ImGui::GetStyle().ItemSpacing.x * 2);
            } else if (options.labelPosition == LabelPosition::Far || options.labelPosition == LabelPosition::None) {
                ImGui::SetNextItemWidth(ImGui::CalcTextSize(comboVector.at(currentValueIndex)).x + ImGui::GetStyle().FramePadding.x * 4 + ImGui::GetStyle().ItemSpacing.x);
            }
        } else if (options.alignment == ComponentAlignment::Right) {
            if (options.labelPosition == LabelPosition::Above) {
                ImGui::NewLine();
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x);
                ImGui::Text(label);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Near) {
                ImGui::SameLine(ImGui::CalcTextSize(label).x + ImGui::GetStyle().ItemSpacing.x * 2);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Far || options.labelPosition == LabelPosition::None) {
                float width = ImGui::CalcTextSize(comboVector.at(currentValueIndex)).x + ImGui::GetStyle().FramePadding.x * 4;
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - width);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            }
        }
        if (ImGui::BeginCombo(invisibleLabel, comboVector.at(currentValueIndex), options.flags)) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f));
            for (size_t i = 0; i < comboVector.size(); ++i) {
                auto newValue = static_cast<T>(i);
                if (strlen(comboVector.at(i)) > 1) {
                    if (ImGui::Selectable(comboVector.at(i), newValue == *value)) {
                        *value = newValue;
                        dirty = true;
                    }
                }
            }
            ImGui::PopStyleVar();
            ImGui::EndCombo();
        }
        if (options.alignment == ComponentAlignment::Left) {
            if (options.labelPosition == LabelPosition::Near) {
                ImGui::SameLine();
                ImGui::Text(label);
            } else if (options.labelPosition == LabelPosition::Far) {
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x);
                ImGui::Text(label);
            }
        } else if (options.alignment == ComponentAlignment::Right) {
            if (options.labelPosition == LabelPosition::Near || options.labelPosition == LabelPosition::Far) {
                ImGui::SameLine(startX);
                ImGui::Text(label);
            }
        }
        PopStyleCombobox();
        ImGui::EndDisabled();
        ImGui::EndGroup();
        if (options.disabled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && strcmp(options.disabledTooltip, "") != 0) {
            ImGui::SetTooltip("%s", WrappedText(options.disabledTooltip));
        } else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && strcmp(options.tooltip, "") != 0) {
            ImGui::SetTooltip("%s", WrappedText(options.tooltip));
        }
        ImGui::PopID();
        return dirty;
    }

    template <typename T = size_t, size_t N>
    bool Combobox(const char* label, T* value, const char* (&comboArray)[N], const ComboboxOptions& options = {}) {
        bool dirty = false;
        float startX = ImGui::GetCursorPosX();
        size_t currentValueIndex = static_cast<size_t>(*value);
        if (currentValueIndex >= N) {
            currentValueIndex = 0;
        }
        std::string invisibleLabelStr = "##" + std::string(label);
        const char* invisibleLabel = invisibleLabelStr.c_str();
        ImGui::PushID(label);
        ImGui::BeginGroup();
        ImGui::BeginDisabled(options.disabled);
        PushStyleCombobox(options.color);
        if (options.alignment == ComponentAlignment::Left) {
            if (options.labelPosition == LabelPosition::Above) {
                ImGui::Text(label);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Near) {
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x - ImGui::GetStyle().ItemSpacing.x * 2);
            } else if (options.labelPosition == LabelPosition::Far || options.labelPosition == LabelPosition::None) {
                ImGui::SetNextItemWidth(ImGui::CalcTextSize(comboArray[currentValueIndex]).x + ImGui::GetStyle().FramePadding.x * 4 + ImGui::GetStyle().ItemSpacing.x);
            }
        } else if (options.alignment == ComponentAlignment::Right) {
            if (options.labelPosition == LabelPosition::Above) {
                ImGui::NewLine();
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x);
                ImGui::Text(label);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Near) {
                ImGui::SameLine(ImGui::CalcTextSize(label).x + ImGui::GetStyle().ItemSpacing.x * 2);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            } else if (options.labelPosition == LabelPosition::Far || options.labelPosition == LabelPosition::None) {
                float width = ImGui::CalcTextSize(comboArray[currentValueIndex]).x + ImGui::GetStyle().FramePadding.x * 4;
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - width);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            }
        }
        if (ImGui::BeginCombo(invisibleLabel, comboArray[currentValueIndex], options.flags)) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f));
            for (size_t i = 0; i < N; ++i) {
                auto newValue = static_cast<T>(i);
                if (strlen(comboArray[i]) > 1) {
                    if (ImGui::Selectable(comboArray[i], newValue == *value)) {
                        *value = newValue;
                        dirty = true;
                    }
                }
            }
            ImGui::PopStyleVar();
            ImGui::EndCombo();
        }
        if (options.alignment == ComponentAlignment::Left) {
            if (options.labelPosition == LabelPosition::Near) {
                ImGui::SameLine();
                ImGui::Text(label);
            } else if (options.labelPosition == LabelPosition::Far) {
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(label).x);
                ImGui::Text(label);
            }
        } else if (options.alignment == ComponentAlignment::Right) {
            if (options.labelPosition == LabelPosition::Near || options.labelPosition == LabelPosition::Far) {
                ImGui::SameLine(startX);
                ImGui::Text(label);
            }
        }
        PopStyleCombobox();
        ImGui::EndDisabled();
        ImGui::EndGroup();
        if (options.disabled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && strcmp(options.disabledTooltip, "") != 0) {
            ImGui::SetTooltip("%s", WrappedText(options.disabledTooltip));
        } else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && strcmp(options.tooltip, "") != 0) {
            ImGui::SetTooltip("%s", WrappedText(options.tooltip));
        }
        ImGui::PopID();
        return dirty;
    }

    template <typename T = int32_t>
    bool CVarCombobox(const char* label, const char* cvarName, const std::unordered_map<T, const char*>& comboMap, const ComboboxOptions& options = {}) {
        bool dirty = false;
        int32_t value = CVarGetInteger(cvarName, options.defaultIndex);
        if (Combobox<T>(label, &value, comboMap, options)) {
            CVarSetInteger(cvarName, value);
            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            dirty = true;
        }
        return dirty;
    }

    template <typename T = int32_t>
    bool CVarCombobox(const char* label, const char* cvarName, const std::vector<const char*>& comboVector, const ComboboxOptions& options = {}) {
        bool dirty = false;
        int32_t value = CVarGetInteger(cvarName, options.defaultIndex);
        if (Combobox<T>(label, &value, comboVector, options)) {
            CVarSetInteger(cvarName, value);
            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            dirty = true;
        }
        return dirty;
    }

    template <typename T = int32_t, size_t N>
    bool CVarCombobox(const char* label, const char* cvarName, const char* (&comboArray)[N], const ComboboxOptions& options = {}) {
        bool dirty = false;
        int32_t value = CVarGetInteger(cvarName, options.defaultIndex);
        if (Combobox<T>(label, &value, comboArray, options)) {
            CVarSetInteger(cvarName, value);
            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            dirty = true;
        }
        return dirty;
    }

    struct IntSliderOptions {
        const ImVec4 color = Colors::Gray;
        const char* tooltip = "";
        bool disabled = false;
        const char* disabledTooltip = "";
        bool showButtons = true;
        ImGuiSliderFlags flags = 0;
        const char* format = "%d";
        const uint32_t step = 1;
        ComponentAlignment alignment = ComponentAlignment::Left;
        LabelPosition labelPosition = LabelPosition::Above;
    };

    struct FloatSliderOptions {
        const ImVec4 color = Colors::Gray;
        const char* tooltip = "";
        bool disabled = false;
        const char* disabledTooltip = "";
        bool showButtons = true;
        ImGuiSliderFlags flags = 0;
        const char* format = "%f";
        const float step = 0.01f;
        bool isPercentage = false; // Multiplies visual value by 100
        ComponentAlignment alignment = ComponentAlignment::Left;
        LabelPosition labelPosition = LabelPosition::Above;
    };

    void PushStyleSlider(const ImVec4& color = Colors::Indigo);
    void PopStyleSlider();
    bool SliderInt(const char* label, int32_t* value, int32_t min, int32_t max, const IntSliderOptions& options = {});
    bool CVarSliderInt(const char* label, const char* cvarName, int32_t min, int32_t max, const int32_t defaultValue, const IntSliderOptions& options = {});
    bool SliderFloat(const char* label, float* value, float min, float max, const FloatSliderOptions& options = {});
    bool CVarSliderFloat(const char* label, const char* cvarName, float min, float max, const float defaultValue, const FloatSliderOptions& options = {});
}

#endif /* UIWidgets_hpp */
