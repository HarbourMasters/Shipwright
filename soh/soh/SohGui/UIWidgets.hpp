#ifndef UIWidgets2_hpp
#define UIWidgets2_hpp

#include "UIWidgetOptions.hpp"

#include <libultraship/bridge/consolevariablebridge.h>
#include <ship/Context.h>
#include <ship/window/gui/GuiWindow.h>
#include <ship/window/Window.h>
#include "soh/ShipInit.hpp"

namespace UIWidgets {

using SectionFunc = void (*)();

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
void PaddedSeparator(bool padTop = true, bool padBottom = true, float extraVerticalTopPadding = 0.0f,
                     float extraVerticalBottomPadding = 0.0f);
void Tooltip(std::string text);

typedef enum ColorPickerModifiers {
    ColorPickerResetButton = 1,
    ColorPickerRandomButton = 2,
    ColorPickerRainbowCheck = 4,
    ColorPickerLockCheck = 8,
} ColorPickerModifiers;

const std::unordered_map<Colors, ImVec4> ColorValues = {
    { Colors::Pink, ImVec4(0.87f, 0.3f, 0.87f, 1.0f) },     { Colors::Red, ImVec4(0.55f, 0.0f, 0.0f, 1.0f) },
    { Colors::DarkRed, ImVec4(0.3f, 0.0f, 0.0f, 1.0f) },    { Colors::Orange, ImVec4(0.85f, 0.55f, 0.0f, 1.0f) },
    { Colors::Yellow, ImVec4(0.95f, 0.95f, 0.0f, 1.0f) },   { Colors::Green, ImVec4(0.0f, 0.55f, 0.0f, 1.0f) },
    { Colors::DarkGreen, ImVec4(0.0f, 0.3f, 0.0f, 1.0f) },  { Colors::Cyan, ImVec4(0.0f, 0.9f, 0.9f, 1.0f) },
    { Colors::LightBlue, ImVec4(0.0f, 0.24f, 0.8f, 1.0f) }, { Colors::Blue, ImVec4(0.08f, 0.03f, 0.65f, 1.0f) },
    { Colors::DarkBlue, ImVec4(0.03f, 0.0f, 0.5f, 1.0f) },  { Colors::Indigo, ImVec4(0.35f, 0.0f, 0.87f, 1.0f) },
    { Colors::Violet, ImVec4(0.5f, 0.0f, 0.9f, 1.0f) },     { Colors::Purple, ImVec4(0.31f, 0.0f, 0.67f, 1.0f) },
    { Colors::Brown, ImVec4(0.37f, 0.18f, 0.0f, 1.0f) },    { Colors::LightGray, ImVec4(0.75f, 0.75f, 0.75f, 1.0f) },
    { Colors::Gray, ImVec4(0.45f, 0.45f, 0.45f, 1.0f) },    { Colors::DarkGray, ImVec4(0.15f, 0.15f, 0.15f, 1.0f) },
    { Colors::Black, ImVec4(0.0f, 0.0f, 0.0f, 1.0f) },      { Colors::White, ImVec4(1.0f, 1.0f, 1.0f, 1.0f) },
    { Colors::NoColor, ImVec4(0.0f, 0.0f, 0.0f, 0.0f) },
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
bool WindowButton(const char* label, const char* cvarName, std::shared_ptr<Ship::GuiWindow> windowPtr,
                  const WindowButtonOptions& options = {});

void PushStyleCheckbox(const ImVec4& color, ImVec2 padding = ImVec2(10.0f, 6.0f));
void PushStyleCheckbox(Colors color = Colors::LightBlue, ImVec2 padding = ImVec2(10.0f, 6.0f));
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
bool Combobox(std::string label, T* value, const std::map<T, const char*>& comboMap,
              const ComboboxOptions& options = {}) {
    bool dirty = false;
    float startX = ImGui::GetCursorPosX();
    std::string invisibleLabelStr = "##" + std::string(label);
    const char* invisibleLabel = invisibleLabelStr.c_str();
    std::string trueLabel = label.substr(0, label.find("#"));
    ImGui::PushID(label.c_str());
    ImGui::BeginGroup();
    ImGui::BeginDisabled(options.disabled);
    PushStyleCombobox(options.color);

    const char* longest;
    size_t length = 0;
    for (auto& [index, string] : comboMap) {
        size_t len = strlen(string);
        if (len > length) {
            longest = string;
            length = len;
        }
    }
    float comboWidth = CalcComboWidth(longest, options.flags);

    ImGui::AlignTextToFramePadding();
    if (options.labelPosition != LabelPositions::None) {
        if (options.alignment == ComponentAlignments::Right) {
            ImGui::Text("%s", trueLabel.c_str());
            if (options.labelPosition == LabelPositions::Above) {
                ImGui::NewLine();
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
            } else if (options.labelPosition == LabelPositions::Near) {
                ImGui::SameLine();
            } else if (options.labelPosition == LabelPositions::Far) {
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
            }
        } else if (options.alignment == ComponentAlignments::Left) {
            if (options.labelPosition == LabelPositions::Above) {
                ImGui::Text("%s", trueLabel.c_str());
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

    if (options.labelPosition != LabelPositions::None) {
        if (options.alignment == ComponentAlignments::Left) {
            if (options.labelPosition == LabelPositions::Near) {
                ImGui::SameLine();
                ImGui::Text("%s", trueLabel.c_str());
            } else if (options.labelPosition == LabelPositions::Far) {
                float width = ImGui::CalcTextSize(comboMap.at(*value)).x + ImGui::GetStyle().FramePadding.x * 2;
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - width);
                ImGui::Text("%s", trueLabel.c_str());
            }
        }
    }
    PopStyleCombobox();
    ImGui::EndDisabled();
    ImGui::EndGroup();
    if (options.disabled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) &&
        !options.disabledTooltip.empty()) {
        ImGui::SetTooltip("%s", WrappedText(options.disabledTooltip).c_str());
    } else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !options.tooltip.empty()) {
        ImGui::SetTooltip("%s", WrappedText(options.tooltip).c_str());
    }
    ImGui::PopID();
    return dirty;
}

template <typename T = size_t>
bool Combobox(std::string label, T* value, const std::vector<const char*>& comboVector,
              const ComboboxOptions& options = {}) {
    bool dirty = false;
    size_t currentValueIndex = static_cast<size_t>(*value);
    std::string invisibleLabelStr = "##" + std::string(label);
    const char* invisibleLabel = invisibleLabelStr.c_str();
    std::string trueLabel = label.substr(0, label.find("#"));
    ImGui::PushID(label.c_str());
    ImGui::BeginGroup();
    ImGui::BeginDisabled(options.disabled);
    PushStyleCombobox(options.color);

    const char* longest;
    size_t length = 0;
    for (auto& string : comboVector) {
        size_t len = strlen(string);
        if (len > length) {
            longest = string;
            length = len;
        }
    }
    float comboWidth = CalcComboWidth(longest, options.flags);

    ImGui::AlignTextToFramePadding();
    if (options.labelPosition != LabelPositions::None) {
        if (options.alignment == ComponentAlignments::Right) {
            ImGui::Text("%s", trueLabel.c_str());
            if (options.labelPosition == LabelPositions::Above) {
                ImGui::NewLine();
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
            } else if (options.labelPosition == LabelPositions::Near) {
                ImGui::SameLine();
            } else if (options.labelPosition == LabelPositions::Far) {
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
            }
        } else if (options.alignment == ComponentAlignments::Left) {
            if (options.labelPosition == LabelPositions::Above) {
                ImGui::Text("%s", trueLabel.c_str());
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

    if (options.labelPosition != LabelPositions::None) {
        if (options.alignment == ComponentAlignments::Left) {
            if (options.labelPosition == LabelPositions::Near) {
                ImGui::SameLine();
                ImGui::Text("%s", trueLabel.c_str());
            } else if (options.labelPosition == LabelPositions::Far) {
                float width = ImGui::CalcTextSize(comboVector.at(*value)).x + ImGui::GetStyle().FramePadding.x * 2;
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - width);
                ImGui::Text("%s", trueLabel.c_str());
            }
        }
    }

    PopStyleCombobox();
    ImGui::EndDisabled();
    ImGui::EndGroup();
    if (options.disabled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) &&
        !options.disabledTooltip.empty()) {
        ImGui::SetTooltip("%s", WrappedText(options.disabledTooltip).c_str());
    } else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !options.tooltip.empty()) {
        ImGui::SetTooltip("%s", WrappedText(options.tooltip).c_str());
    }
    ImGui::PopID();
    return dirty;
}

template <typename T = size_t>
bool Combobox(std::string label, T* value, const std::vector<std::string>& comboVector,
              const ComboboxOptions& options = {}) {
    bool dirty = false;
    size_t currentValueIndex = static_cast<size_t>(*value);
    std::string invisibleLabelStr = "##" + std::string(label);
    const char* invisibleLabel = invisibleLabelStr.c_str();
    std::string trueLabel = label.substr(0, label.find("#"));
    ImGui::PushID(label.c_str());
    ImGui::BeginGroup();
    ImGui::BeginDisabled(options.disabled);
    PushStyleCombobox(options.color);

    const char* longest = "";
    size_t length = 0;
    for (auto& string : comboVector) {
        size_t len = string.length();
        if (len > length) {
            longest = string.c_str();
            length = len;
        }
    }
    float comboWidth = CalcComboWidth(longest, options.flags);

    ImGui::AlignTextToFramePadding();
    if (options.labelPosition != LabelPositions::None) {
        if (options.alignment == ComponentAlignments::Right) {
            ImGui::Text("%s", trueLabel.c_str());
            if (options.labelPosition == LabelPositions::Above) {
                ImGui::NewLine();
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
            } else if (options.labelPosition == LabelPositions::Near) {
                ImGui::SameLine();
            } else if (options.labelPosition == LabelPositions::Far) {
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
            }
        } else if (options.alignment == ComponentAlignments::Left) {
            if (options.labelPosition == LabelPositions::Above) {
                ImGui::Text("%s", trueLabel.c_str());
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

    if (options.labelPosition != LabelPositions::None) {
        if (options.alignment == ComponentAlignments::Left) {
            if (options.labelPosition == LabelPositions::Near) {
                ImGui::SameLine();
                ImGui::Text("%s", trueLabel.c_str());
            } else if (options.labelPosition == LabelPositions::Far) {
                float width =
                    ImGui::CalcTextSize(comboVector.at(*value).c_str()).x + ImGui::GetStyle().FramePadding.x * 2;
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - width);
                ImGui::Text("%s", trueLabel.c_str());
            }
        }
    }

    PopStyleCombobox();
    ImGui::EndDisabled();
    ImGui::EndGroup();
    if (options.disabled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) &&
        !options.disabledTooltip.empty()) {
        ImGui::SetTooltip("%s", WrappedText(options.disabledTooltip).c_str());
    } else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !options.tooltip.empty()) {
        ImGui::SetTooltip("%s", WrappedText(options.tooltip).c_str());
    }
    ImGui::PopID();
    return dirty;
}

template <typename T = size_t, size_t N>
bool Combobox(std::string label, T* value, const char* (&comboArray)[N], const ComboboxOptions& options = {}) {
    bool dirty = false;
    size_t currentValueIndex = static_cast<size_t>(*value);
    if (currentValueIndex >= N) {
        currentValueIndex = 0;
    }
    std::string invisibleLabelStr = "##" + std::string(label);
    const char* invisibleLabel = invisibleLabelStr.c_str();
    std::string trueLabel = label.substr(0, label.find("#"));
    ImGui::PushID(label.c_str());
    ImGui::BeginGroup();
    ImGui::BeginDisabled(options.disabled);
    PushStyleCombobox(options.color);

    const char* longest;
    size_t length = 0;
    for (size_t i = 0; i < N; i++) {
        size_t len = strlen(comboArray[i]);
        if (len > length) {
            longest = comboArray[i];
            length = len;
        }
    }
    float comboWidth = CalcComboWidth(longest, options.flags);

    ImGui::AlignTextToFramePadding();
    if (options.labelPosition != LabelPositions::None) {
        if (options.alignment == ComponentAlignments::Right) {
            ImGui::Text("%s", trueLabel.c_str());
            if (options.labelPosition == LabelPositions::Above) {
                ImGui::NewLine();
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
            } else if (options.labelPosition == LabelPositions::Near) {
                ImGui::SameLine();
            } else if (options.labelPosition == LabelPositions::Far) {
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - comboWidth);
            }
        } else if (options.alignment == ComponentAlignments::Left) {
            if (options.labelPosition == LabelPositions::Above) {
                ImGui::Text("%s", trueLabel.c_str());
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

    if (options.labelPosition != LabelPositions::None) {
        if (options.alignment == ComponentAlignments::Left) {
            if (options.labelPosition == LabelPositions::Near) {
                ImGui::SameLine();
                ImGui::Text("%s", trueLabel.c_str());
            } else if (options.labelPosition == LabelPositions::Far) {
                float width = ImGui::CalcTextSize(comboArray[*value]).x + ImGui::GetStyle().FramePadding.x * 2;
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - width);
                ImGui::Text("%s", trueLabel.c_str());
            }
        }
    }
    PopStyleCombobox();
    ImGui::EndDisabled();
    ImGui::EndGroup();
    if (options.disabled && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) &&
        !options.disabledTooltip.empty()) {
        ImGui::SetTooltip("%s", WrappedText(options.disabledTooltip).c_str());
    } else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !options.tooltip.empty()) {
        ImGui::SetTooltip("%s", WrappedText(options.tooltip).c_str());
    }
    ImGui::PopID();
    return dirty;
}

template <typename T = int32_t>
bool CVarCombobox(const char* label, const char* cvarName, const std::map<T, const char*>& comboMap,
                  const ComboboxOptions& options = {}) {
    bool dirty = false;
    int32_t value = CVarGetInteger(cvarName, options.defaultIndex);
    if (Combobox<T>(label, &value, comboMap, options)) {
        CVarSetInteger(cvarName, value);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        ShipInit::Init(cvarName);
        dirty = true;
    }
    return dirty;
}

template <typename T = int32_t>
bool CVarCombobox(const char* label, const char* cvarName, const std::vector<const char*>& comboVector,
                  const ComboboxOptions& options = {}) {
    bool dirty = false;
    int32_t value = CVarGetInteger(cvarName, options.defaultIndex);
    if (Combobox<T>(label, &value, comboVector, options)) {
        CVarSetInteger(cvarName, value);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        ShipInit::Init(cvarName);
        dirty = true;
    }
    return dirty;
}

template <typename T = int32_t, size_t N>
bool CVarCombobox(const char* label, const char* cvarName, const char* (&comboArray)[N],
                  const ComboboxOptions& options = {}) {
    bool dirty = false;
    int32_t value = CVarGetInteger(cvarName, options.defaultIndex);
    if (Combobox<T>(label, &value, comboArray, options)) {
        CVarSetInteger(cvarName, value);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
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
bool CVarColorPicker(const char* label, const char* cvarName, Color_RGBA8 defaultColor, bool hasAlpha = false,
                     uint8_t modifiers = 0, UIWidgets::Colors themeColor = UIWidgets::Colors::LightBlue);
bool RadioButton(const char* label, bool active);
bool CVarRadioButton(const char* text, const char* cvarName, int32_t id, const RadioButtonsOptions& options);
bool StateButton(const char* str_id, const char* label, ImVec2 size, UIWidgets::ButtonOptions options,
                 ImGuiButtonFlags flags = ImGuiButtonFlags_None);
void DrawFlagArray32(const std::string& name, uint32_t& flags, Colors color = Colors::LightBlue);
void DrawFlagArray16(const std::string& name, uint16_t& flags, Colors color = Colors::LightBlue);
void DrawFlagArray8(const std::string& name, uint8_t& flags, Colors color = Colors::LightBlue);
void DrawFlagArray8Mask(const std::string& name, uint8_t& flags, Colors color = Colors::LightBlue);
bool BtnSelector(const char* label, int32_t* value, const BtnSelectorOptions& options);
bool CVarBtnSelector(const char* label, const char* cvarName, const BtnSelectorOptions& options);

void InsertHelpHoverText(const std::string& text);
void InsertHelpHoverText(const char* text);
} // namespace UIWidgets

ImVec4 GetRandomValue(uint64_t* state = nullptr);

Color_RGBA8 RGBA8FromVec(ImVec4 vec);
ImVec4 VecFromRGBA8(Color_RGBA8 color);

#endif /* UIWidgets_hpp */
