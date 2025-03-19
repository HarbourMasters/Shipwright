#ifndef UIWidgets2_hpp
#define UIWidgets2_hpp

#include <string>
#include <vector>
#include <span>
#include <stdint.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <libultraship/libultraship.h>
#include <unordered_map>
#include "soh/ShipUtils.h"
#include "soh/ShipInit.hpp"

namespace UIWidgets {

    using SectionFunc = void(*)();

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

    std::string WrappedText(const char* text, unsigned int charactersPerLine = 80);
    std::string WrappedText(const std::string& text, unsigned int charactersPerLine = 80);
    void PaddedSeparator(bool padTop = true, bool padBottom = true, float extraVerticalTopPadding = 0.0f, float extraVerticalBottomPadding = 0.0f);
    void Tooltip(const char* text);

    typedef enum ColorPickerModifiers {
        ColorPickerResetButton = 1,
        ColorPickerRandomButton = 2,
        ColorPickerRainbowCheck = 4,
        ColorPickerLockCheck = 8,
    } ColorPickerModifiers;

    // mostly in order for colors usable by the menu without custom text color
    enum Colors {
        Red,
        DarkRed,
        Orange,
        Green,
        DarkGreen,
        LightBlue,
        Blue,
        DarkBlue,
        Indigo,
        Violet,
        Purple,
        Brown,
        Gray,
        DarkGray,
        // not suitable for menu theme use
        Pink,
        Yellow,
        Cyan,
        Black,
        LightGray,
        White,
        NoColor
    };

    enum InputTypes {
        String,
        Scalar
    };

    const std::unordered_map<Colors, ImVec4> ColorValues = {
        { Colors::Pink, ImVec4(0.87f, 0.3f, 0.87f, 1.0f) },
        { Colors::Red, ImVec4(0.55f, 0.0f, 0.0f, 1.0f) },
        { Colors::DarkRed, ImVec4(0.3f, 0.0f, 0.0f, 1.0f) },
        { Colors::Orange, ImVec4(0.85f, 0.55f, 0.0f, 1.0f) },
        { Colors::Yellow, ImVec4(0.95f, 0.95f, 0.0f, 1.0f) },
        { Colors::Green, ImVec4(0.0f, 0.55f, 0.0f, 1.0f) },
        { Colors::DarkGreen, ImVec4(0.0f, 0.3f, 0.0f, 1.0f) },
        { Colors::Cyan, ImVec4(0.0f, 0.9f, 0.9f, 1.0f) },
        { Colors::LightBlue, ImVec4(0.0f, 0.24f, 0.8f, 1.0f) },
        { Colors::Blue, ImVec4(0.08f, 0.03f, 0.65f, 1.0f) },
        { Colors::DarkBlue, ImVec4(0.03f, 0.0f, 0.5f, 1.0f) },
        { Colors::Indigo, ImVec4(0.35f, 0.0f, 0.87f, 1.0f) },
        { Colors::Violet, ImVec4(0.5f, 0.0f, 0.9f, 1.0f) },
        { Colors::Purple, ImVec4(0.31f, 0.0f, 0.67f, 1.0f) },
        { Colors::Brown, ImVec4(0.37f, 0.18f, 0.0f, 1.0f) },
        { Colors::LightGray, ImVec4(0.75f, 0.75f, 0.75f, 1.0f) },
        { Colors::Gray, ImVec4(0.45f, 0.45f, 0.45f, 1.0f) },
        { Colors::DarkGray, ImVec4(0.15f, 0.15f, 0.15f, 1.0f) },
        { Colors::Black, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)},
        { Colors::White, ImVec4(1.0f, 1.0f, 1.0f, 1.0f) },
        { Colors::NoColor, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)},
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

    struct WidgetOptions{
        const char* tooltip = "";
        bool disabled = false;
        const char* disabledTooltip = "";

        WidgetOptions& Tooltip(const char* tooltip_) {
            tooltip = tooltip_;
            return *this;
        }
        WidgetOptions& Disabled(bool disabled_) {
            disabled = disabled_;
            return *this;
        }
        WidgetOptions& DisabledTooltip(const char* disabledTooltip_) {
            disabledTooltip = disabledTooltip_;
            return *this;
        }
    };

    struct TextOptions : WidgetOptions {
        Colors color = Colors::NoColor;

        TextOptions& Color(Colors color_) {
            color = color_;
            return *this;
        }
    };

    struct ButtonOptions : WidgetOptions {
        ImVec2 size = Sizes::Fill;
        ImVec2 padding = ImVec2(10.0f, 8.0f);
        Colors color = Colors::Gray;

        ButtonOptions& Size(ImVec2 size_) {
            size = size_;
            return *this;
        }
        ButtonOptions& Padding(ImVec2 padding_) {
            padding = padding_;
            return *this;
        }
        ButtonOptions& Tooltip(const char* tooltip_) {
            WidgetOptions::tooltip = tooltip_;
            return *this;
        }
        ButtonOptions& Color(Colors color_) {
            color = color_;
            return *this;
        }
    };

    struct WindowButtonOptions : WidgetOptions {
        ImVec2 size = Sizes::Inline;
        ImVec2 padding = ImVec2(10.0f, 8.0f);
        Colors color = Colors::Gray;
        bool showButton = true;
        bool embedWindow = true;

        WindowButtonOptions& Size(ImVec2 size_) {
            size = size_;
            return *this;
        }
        WindowButtonOptions& Padding(ImVec2 padding_) {
            padding = padding_;
            return *this;
        }
        WindowButtonOptions& Tooltip(const char* tooltip_) {
            WidgetOptions::tooltip = tooltip_;
            return *this;
        }
        WindowButtonOptions& Color(Colors color_) {
            color = color_;
            return *this;
        }
        WindowButtonOptions& ShowButton(bool showButton_) {
            showButton = showButton_;
            return *this;
        }
        WindowButtonOptions& EmbedWindow(bool embedWindow_) {
            embedWindow = embedWindow_;
            return *this;
        }
    };

    struct CheckboxOptions : WidgetOptions {
        bool defaultValue = false; // Only applicable to CVarCheckbox
        ComponentAlignment alignment = ComponentAlignment::Left;
        LabelPosition labelPosition = LabelPosition::Near;
        Colors color = Colors::LightBlue;

        CheckboxOptions& DefaultValue(bool defaultValue_) {
            defaultValue = defaultValue_;
            return *this;
        }
        CheckboxOptions& ComponentAlignment(ComponentAlignment alignment_) {
            alignment = alignment_;
            return *this;
        }
        CheckboxOptions& LabelPosition(LabelPosition labelPosition_) {
            labelPosition = labelPosition_;
            return *this;
        }
        CheckboxOptions& Tooltip(const char* tooltip_) {
            WidgetOptions::tooltip = tooltip_;
            return *this;
        }
        CheckboxOptions& Color(Colors color_) {
            color = color_;
            return *this;
        }
        CheckboxOptions& DisabledTooltip(const char* disabledTooltip_) {
            WidgetOptions::disabledTooltip = disabledTooltip_;
            return *this;
        }
    };

    struct ComboboxOptions : WidgetOptions {
        std::unordered_map<int32_t, const char*> comboMap = {};
        uint32_t defaultIndex = 0; // Only applicable to CVarCombobox
        ComponentAlignment alignment = ComponentAlignment::Left;
        LabelPosition labelPosition = LabelPosition::Above;
        ImGuiComboFlags flags = 0;
        Colors color = Colors::LightBlue;

        ComboboxOptions& ComboMap(std::unordered_map<int32_t, const char*> comboMap_) {
            comboMap = comboMap_;
            return *this;
        }
        ComboboxOptions& DefaultIndex(uint32_t defaultIndex_) {
            defaultIndex = defaultIndex_;
            return *this;
        }
        ComboboxOptions& ComponentAlignment(ComponentAlignment alignment_) {
            alignment = alignment_;
            return *this;
        }
        ComboboxOptions& LabelPosition(LabelPosition labelPosition_) {
            labelPosition = labelPosition_;
            return *this;
        }
        ComboboxOptions& Tooltip(const char* tooltip_) {
            WidgetOptions::tooltip = tooltip_;
            return *this;
        }
        ComboboxOptions& Color(Colors color_) {
            color = color_;
            return *this;
        }
    };

    struct IntSliderOptions : WidgetOptions {
        bool showButtons = true;
        const char* format = "%d";
        int32_t step = 1;
        int32_t min = 1;
        int32_t max = 10;
        int32_t defaultValue = 1;
        bool clamp = true;
        ComponentAlignment alignment = ComponentAlignment::Left;
        LabelPosition labelPosition = LabelPosition::Above;
        Colors color = Colors::Gray;
        ImGuiSliderFlags flags = 0;
        ImVec2 size = {0,0};

        IntSliderOptions& ShowButtons(bool showButtons_) {
            showButtons = showButtons_;
            return *this;
        }
        IntSliderOptions& Format(const char* format_) {
            format = format_;
            return *this;
        }
        IntSliderOptions& Step(int32_t step_) {
            step = step_;
            return *this;
        }
        IntSliderOptions& Min(int32_t min_) {
            min = min_;
            return *this;
        }
        IntSliderOptions& Max(int32_t max_) {
            max = max_;
            return *this;
        }
        IntSliderOptions& DefaultValue(int32_t defaultValue_) {
            defaultValue = defaultValue_;
            return *this;
        }
        IntSliderOptions& ComponentAlignment(ComponentAlignment alignment_) {
            alignment = alignment_;
            return *this;
        }
        IntSliderOptions& LabelPosition(LabelPosition labelPosition_) {
            labelPosition = labelPosition_;
            return *this;
        }
        IntSliderOptions& Tooltip(const char* tooltip_) {
            WidgetOptions::tooltip = tooltip_;
            return *this;
        }
        IntSliderOptions& Color(Colors color_) {
            color = color_;
            return *this;
        }
        IntSliderOptions& Size(ImVec2 size_) {
            size = size_;
            return *this;
        }
        IntSliderOptions& Clamp(bool clamp_) {
            clamp = clamp_;
            return *this;
        }
    };

    struct FloatSliderOptions : WidgetOptions {
        bool showButtons = true;
        const char* format = "%f";
        float step = 0.01f;
        float min = 0.01f;
        float max = 10.0f;
        float defaultValue = 1.0f;
        bool clamp = true;
        bool isPercentage = false; // Multiplies visual value by 100
        ComponentAlignment alignment = ComponentAlignment::Left;
        LabelPosition labelPosition = LabelPosition::Above;
        Colors color = Colors::Gray;
        ImGuiSliderFlags flags = 0;
        ImVec2 size = {0,0};

        FloatSliderOptions& ShowButtons(bool showButtons_) {
            showButtons = showButtons_;
            return *this;
        }
        FloatSliderOptions& Format(const char* format_) {
            format = format_;
            return *this;
        }
        FloatSliderOptions& Step(float step_) {
            step = step_;
            return *this;
        }
        FloatSliderOptions& Min(float min_) {
            min = min_;
            return *this;
        }
        FloatSliderOptions& Max(float max_) {
            max = max_;
            return *this;
        }
        FloatSliderOptions& DefaultValue(float defaultValue_) {
            defaultValue = defaultValue_;
            return *this;
        }
        FloatSliderOptions& ComponentAlignment(ComponentAlignment alignment_) {
            alignment = alignment_;
            return *this;
        }
        FloatSliderOptions& LabelPosition(LabelPosition labelPosition_) {
            labelPosition = labelPosition_;
            return *this;
        }
        FloatSliderOptions& IsPercentage(bool isPercentage_ = true) {
            isPercentage = isPercentage_;
            format = "%.0f%%";
            min = 0.0f;
            max = 1.0f;
            return *this;
        }
        FloatSliderOptions& Tooltip(const char* tooltip_) {
            WidgetOptions::tooltip = tooltip_;
            return *this;
        }
        FloatSliderOptions& Color(Colors color_) {
            color = color_;
            return *this;
        }
        FloatSliderOptions& Size(ImVec2 size_) {
            size = size_;
            return *this;
        }
        FloatSliderOptions& Clamp(bool clamp_) {
            clamp = clamp_;
            return *this;
        }
    };

    struct RadioButtonsOptions : WidgetOptions {
        std::unordered_map<int32_t, const char*> buttonMap;
        Colors color = Colors::LightBlue;
        
        RadioButtonsOptions& ButtonMap(std::unordered_map<int32_t, const char*> buttonMap_) {
            buttonMap = buttonMap_;
            return *this;
        }
        RadioButtonsOptions& Tooltip(const char* tooltip_) {
            WidgetOptions::tooltip = tooltip_;
            return *this;
        }
        RadioButtonsOptions& Color(Colors color_) {
            color = color_;
            return *this;
        }
    };

    struct InputOptions : WidgetOptions {
        ComponentAlignment alignment = ComponentAlignment::Left;
        LabelPosition labelPosition = LabelPosition::Above;
        Colors color = Colors::Gray;
        ImVec2 size = {0,0};
        std::string placeholder = "";
        InputTypes type = InputTypes::String;
        std::string defaultValue = "";
        bool secret = false;
        ImGuiInputFlags addedFlags = 0;

        InputOptions& Tooltip(const char* tooltip_) {
            WidgetOptions::tooltip = tooltip_;
            return *this;
        }
        InputOptions& Color(Colors color_) {
            color = color_;
            return *this;
        }
        InputOptions& Size(ImVec2 size_) {
            size = size_;
            return *this;
        }

        InputOptions& LabelPosition(LabelPosition labelPosition_) {
            labelPosition = labelPosition_;
            return *this;
        }

        InputOptions& PlaceholderText(std::string&& placeholder_) {
            placeholder = std::move(placeholder_);
            return *this;
        }

        InputOptions& PlaceholderText(std::string& placeholder_) {
            placeholder = placeholder_;
            return *this;
        }

        InputOptions& InputType(InputTypes type_) {
            type = type_;
            return *this;
        }

        InputOptions& DefaultValue(std::string defaultValue_) {
            defaultValue = defaultValue_;
            return *this;
        }
        
        InputOptions& IsSecret(bool secret_ = false) {
            secret = secret_;
            return *this;
        }
    };

    void PushStyleMenu(const ImVec4& color);
    void PushStyleMenu(Colors color = Colors::LightBlue);
    void PopStyleMenu();
    bool BeginMenu(const char* label, Colors color = Colors::LightBlue);

    void PushStyleMenuItem(const ImVec4& color);
    void PushStyleMenuItem(Colors color = Colors::LightBlue);
    void PopStyleMenuItem();
    bool MenuItem(const char* label, const char* shortcut = NULL, Colors color = Colors::LightBlue);

    void PushStyleButton(const ImVec4& color, ImVec2 padding = ImVec2(10.0f, 8.0f));
    void PushStyleButton(Colors color = Colors::Gray, ImVec2 padding = ImVec2(10.0f, 8.0f));
    void PopStyleButton();
    bool Button(const char* label, const ButtonOptions& options = {});
    bool WindowButton(const char* label, const char* cvarName, std::shared_ptr<Ship::GuiWindow> windowPtr, const WindowButtonOptions& options = {});

    void PushStyleCheckbox(const ImVec4& color);
    void PushStyleCheckbox(Colors color = Colors::LightBlue);
    void PopStyleCheckbox();
    void RenderText(ImVec2 pos, const char* text, const char* text_end, bool hide_text_after_hash);
    bool Checkbox(const char* label, bool* v, const CheckboxOptions& options = {});
    bool CVarCheckbox(const char* label, const char* cvarName, const CheckboxOptions& options = {});

    void PushStyleCombobox(const ImVec4& color);
    void PushStyleCombobox(Colors color = Colors::LightBlue);
    void PopStyleCombobox();

    void PushStyleTabs(const ImVec4& color);
    void PushStyleTabs(Colors color = Colors::LightBlue);
    void PopStyleTabs();

    void PushStyleInput(const ImVec4& color);
    void PushStyleInput(Colors color = Colors::LightBlue);
    void PopStyleInput();

    void PushStyleHeader(const ImVec4& color);
    void PushStyleHeader(Colors color = Colors::LightBlue);
    void PopStyleHeader();

    void Spacer(float height = 0.0f);
    void Separator(bool padTop = true, bool padBottom = true, float extraVerticalTopPadding = 0.0f,
                   float extraVerticalBottomPadding = 0.0f);

    float CalcComboWidth(const char* preview_value, ImGuiComboFlags flags);

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

        const char* longest;
        int length = 0;
        for (auto& [index, string] : comboMap) {
            int len = strlen(string);
            if (len > length) {
                longest = string;
                length = len;
            }
        }
        float comboWidth = CalcComboWidth(longest, options.flags);

        ImGui::AlignTextToFramePadding();
        if (options.labelPosition != LabelPosition::None) {
            if (options.alignment == ComponentAlignment::Right) {
                ImGui::Text(label);
                if (options.labelPosition == LabelPosition::Above) {
                    ImGui::NewLine();
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
                } else if (options.labelPosition == LabelPosition::Near) {
                    ImGui::SameLine();
                } else if (options.labelPosition == LabelPosition::Far) {
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
                }
            } else if (options.alignment == ComponentAlignment::Left) {
                if (options.labelPosition == LabelPosition::Above) {
                    ImGui::Text(label);
                }
            }
        }

        ImGui::SetNextItemWidth(comboWidth);
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

        if (options.labelPosition != LabelPosition::None) {
            if (options.alignment == ComponentAlignment::Left) {
                if (options.labelPosition == LabelPosition::Near) {
                    ImGui::SameLine();
                    ImGui::Text("%s", label);
                } else if (options.labelPosition == LabelPosition::Far) {
                    float width = ImGui::CalcTextSize(comboMap.at(*value)).x + ImGui::GetStyle().FramePadding.x * 2;
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - width);
                    ImGui::Text("%s", label);
                }
            }
        }
        PopStyleCombobox();
        ImGui::EndDisabled();
        ImGui::EndGroup();
        if (options.disabled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !Ship_IsCStringEmpty(options.disabledTooltip)) {
            ImGui::SetTooltip("%s", WrappedText(options.disabledTooltip).c_str());
        } else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !Ship_IsCStringEmpty(options.tooltip)) {
            ImGui::SetTooltip("%s", WrappedText(options.tooltip).c_str());
        }
        ImGui::PopID();
        return dirty;
    }

    template <typename T = size_t>
    bool Combobox(const char* label, T* value, const std::vector<const char*>& comboVector, const ComboboxOptions& options = {}) {
        bool dirty = false;
        size_t currentValueIndex = static_cast<size_t>(*value);
        std::string invisibleLabelStr = "##" + std::string(label);
        const char* invisibleLabel = invisibleLabelStr.c_str();
        ImGui::PushID(label);
        ImGui::BeginGroup();
        ImGui::BeginDisabled(options.disabled);
        PushStyleCombobox(options.color);

        const char* longest;
        int length = 0;
        for (auto& string : comboVector) {
            int len = strlen(string);
            if (len > length) {
                longest = string;
                length = len;
            }
        }
        float comboWidth = CalcComboWidth(longest, options.flags);

        ImGui::AlignTextToFramePadding();
        if (options.labelPosition != LabelPosition::None) {
            if (options.alignment == ComponentAlignment::Right) {
                ImGui::Text("%s", label);
                if (options.labelPosition == LabelPosition::Above) {
                    ImGui::NewLine();
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
                } else if (options.labelPosition == LabelPosition::Near) {
                    ImGui::SameLine();
                } else if (options.labelPosition == LabelPosition::Far) {
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
                }
            } else if (options.alignment == ComponentAlignment::Left) {
                if (options.labelPosition == LabelPosition::Above) {
                    ImGui::Text("%s", label);
                }
            }
        }

        ImGui::SetNextItemWidth(comboWidth);
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

        if (options.labelPosition != LabelPosition::None) {
            if (options.alignment == ComponentAlignment::Left) {
                if (options.labelPosition == LabelPosition::Near) {
                    ImGui::SameLine();
                    ImGui::Text("%s", label);
                } else if (options.labelPosition == LabelPosition::Far) {
                    float width = ImGui::CalcTextSize(comboVector.at(*value)).x + ImGui::GetStyle().FramePadding.x * 2;
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - width);
                    ImGui::Text("%s", label);
                }
            }
        }

        PopStyleCombobox();
        ImGui::EndDisabled();
        ImGui::EndGroup();
        if (options.disabled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !Ship_IsCStringEmpty(options.disabledTooltip)) {
            ImGui::SetTooltip("%s", WrappedText(options.disabledTooltip).c_str());
        } else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !Ship_IsCStringEmpty(options.tooltip)) {
            ImGui::SetTooltip("%s", WrappedText(options.tooltip).c_str());
        }
        ImGui::PopID();
        return dirty;
    }

    template <typename T = size_t>
    bool Combobox(const char* label, T* value, const std::vector<std::string>& comboVector, const ComboboxOptions& options = {}) {
        bool dirty = false;
        size_t currentValueIndex = static_cast<size_t>(*value);
        std::string invisibleLabelStr = "##" + std::string(label);
        const char* invisibleLabel = invisibleLabelStr.c_str();
        ImGui::PushID(label);
        ImGui::BeginGroup();
        ImGui::BeginDisabled(options.disabled);
        PushStyleCombobox(options.color);

        const char* longest;
        int length = 0;
        for (auto& string : comboVector) {
            int len = string.length();
            if (len > length) {
                longest = string.c_str();
                length = len;
            }
        }
        float comboWidth = CalcComboWidth(longest, options.flags);

        ImGui::AlignTextToFramePadding();
        if (options.labelPosition != LabelPosition::None) {
            if (options.alignment == ComponentAlignment::Right) {
                ImGui::Text("%s", label);
                if (options.labelPosition == LabelPosition::Above) {
                    ImGui::NewLine();
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
                } else if (options.labelPosition == LabelPosition::Near) {
                    ImGui::SameLine();
                } else if (options.labelPosition == LabelPosition::Far) {
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
                }
            } else if (options.alignment == ComponentAlignment::Left) {
                if (options.labelPosition == LabelPosition::Above) {
                    ImGui::Text("%s", label);
                }
            }
        }

        ImGui::SetNextItemWidth(comboWidth);
        if (ImGui::BeginCombo(invisibleLabel, comboVector.at(currentValueIndex).c_str(), options.flags)) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f));
            for (size_t i = 0; i < comboVector.size(); ++i) {
                auto newValue = static_cast<T>(i);
                if (comboVector.at(i).length() > 1) {
                    if (ImGui::Selectable(comboVector.at(i).c_str(), newValue == *value)) {
                        *value = newValue;
                        dirty = true;
                    }
                }
            }
            ImGui::PopStyleVar();
            ImGui::EndCombo();
        }

        if (options.labelPosition != LabelPosition::None) {
            if (options.alignment == ComponentAlignment::Left) {
                if (options.labelPosition == LabelPosition::Near) {
                    ImGui::SameLine();
                    ImGui::Text("%s", label);
                } else if (options.labelPosition == LabelPosition::Far) {
                    float width = ImGui::CalcTextSize(comboVector.at(*value).c_str()).x + ImGui::GetStyle().FramePadding.x * 2;
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - width);
                    ImGui::Text("%s", label);
                }
            }
        }

        PopStyleCombobox();
        ImGui::EndDisabled();
        ImGui::EndGroup();
        if (options.disabled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !Ship_IsCStringEmpty(options.disabledTooltip)) {
            ImGui::SetTooltip("%s", WrappedText(options.disabledTooltip).c_str());
        } else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !Ship_IsCStringEmpty(options.tooltip)) {
            ImGui::SetTooltip("%s", WrappedText(options.tooltip).c_str());
        }
        ImGui::PopID();
        return dirty;
    }

    template <typename T = size_t, size_t N>
    bool Combobox(const char* label, T* value, const char* (&comboArray)[N], const ComboboxOptions& options = {}) {
        bool dirty = false;
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

        const char* longest;
        int length = 0;
        for (size_t i = 0; i < N; i++) {
            int len = strlen(comboArray[i]);
            if (len > length) {
                longest = comboArray[i];
                length = len;
            }
        }
        float comboWidth = CalcComboWidth(longest, options.flags);

        ImGui::AlignTextToFramePadding();
        if (options.labelPosition != LabelPosition::None) {
            if (options.alignment == ComponentAlignment::Right) {
                ImGui::Text("%s", label);
                if (options.labelPosition == LabelPosition::Above) {
                    ImGui::NewLine();
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
                } else if (options.labelPosition == LabelPosition::Near) {
                    ImGui::SameLine();
                } else if (options.labelPosition == LabelPosition::Far) {
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
                }
            } else if (options.alignment == ComponentAlignment::Left) {
                if (options.labelPosition == LabelPosition::Above) {
                    ImGui::Text("%s", label);
                }
            }
        }

        ImGui::SetNextItemWidth(comboWidth);
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

        if (options.labelPosition != LabelPosition::None) {
            if (options.alignment == ComponentAlignment::Left) {
                if (options.labelPosition == LabelPosition::Near) {
                    ImGui::SameLine();
                    ImGui::Text("%s", label);
                } else if (options.labelPosition == LabelPosition::Far) {
                    float width = ImGui::CalcTextSize(comboArray[*value]).x + ImGui::GetStyle().FramePadding.x * 2;
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - width);
                    ImGui::Text("%s", label);
                }
            }
        }
        PopStyleCombobox();
        ImGui::EndDisabled();
        ImGui::EndGroup();
        if (options.disabled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !Ship_IsCStringEmpty(options.disabledTooltip)) {
            ImGui::SetTooltip("%s", WrappedText(options.disabledTooltip).c_str());
        } else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !Ship_IsCStringEmpty(options.tooltip)) {
            ImGui::SetTooltip("%s", WrappedText(options.tooltip).c_str());
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
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            ShipInit::Init(cvarName);
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
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            ShipInit::Init(cvarName);
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
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            ShipInit::Init(cvarName);
            dirty = true;
        }
        return dirty;
    }

    void PushStyleSlider(Colors color = Colors::LightBlue);
    void PopStyleSlider();
    bool SliderInt(const char* label, int32_t* value, const IntSliderOptions& options = {});
    bool CVarSliderInt(const char* label, const char* cvarName, const IntSliderOptions& options = {});
    bool SliderFloat(const char* label, float* value, const FloatSliderOptions& options = {});
    bool CVarSliderFloat(const char* label, const char* cvarName, const FloatSliderOptions& options = {});
    bool InputString(const char* label, std::string* value, const InputOptions& options = {});
    bool CVarInputString(const char* label, const char* cvarName, const InputOptions& options = {});
    bool InputInt(const char* label, int32_t* value, const InputOptions& options = {});
    bool CVarInputInt(const char* label, const char* cvarName, const InputOptions& options = {});
    bool CVarColorPicker(const char* label, const char* cvarName, Color_RGBA8 defaultColor, bool hasAlpha = false, uint8_t modifiers = 0, UIWidgets::Colors themeColor = UIWidgets::Colors::LightBlue);
    bool RadioButton(const char* label, bool active);
    bool CVarRadioButton(const char* text, const char* cvarName, int32_t id, const RadioButtonsOptions& options);
    bool StateButton(const char* str_id, const char* label, ImVec2 size, UIWidgets::ButtonOptions options, ImGuiButtonFlags flags = ImGuiButtonFlags_None);
    void DrawFlagArray32(const std::string& name, uint32_t& flags, Colors color = Colors::LightBlue);
    void DrawFlagArray16(const std::string& name, uint16_t& flags, Colors color = Colors::LightBlue);
    void DrawFlagArray8(const std::string& name, uint8_t& flags, Colors color = Colors::LightBlue);
    void DrawFlagArray8Mask(const std::string& name, uint8_t& flags, Colors color = Colors::LightBlue);

    void InsertHelpHoverText(const std::string& text);
    void InsertHelpHoverText(const char* text);
}
ImVec4 GetRandomValue();

Color_RGBA8 RGBA8FromVec(ImVec4 vec);
ImVec4 VecFromRGBA8(Color_RGBA8 color);

#endif /* UIWidgets_hpp */
