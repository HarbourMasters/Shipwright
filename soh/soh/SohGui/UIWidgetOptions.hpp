#ifndef UIWidgetOptions_hpp
#define UIWidgetOptions_hpp

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <stdint.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include <ship/utils/color.h>

#include "soh/ShipInit.hpp"
#include "soh/ShipUtils.h"

namespace UIWidgets {
namespace Sizes {
const ImVec2 Inline = ImVec2(0.0f, 0.0f);
const ImVec2 Fill = ImVec2(-1.0f, 0.0f);
} // namespace Sizes

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

enum InputTypes { String, Scalar };

enum LabelPositions {
    Near,
    Far,
    Above,
    None,
    Within,
};

enum ComponentAlignments {
    Left,
    Right,
};

struct WidgetOptions {
    std::string tooltip = "";
    bool disabled = false;
    std::string disabledTooltip = "";

    WidgetOptions& Tooltip(std::string tooltip_) {
        tooltip = tooltip_;
        return *this;
    }

    WidgetOptions& Disabled(bool disabled_) {
        disabled = disabled_;
        return *this;
    }

    WidgetOptions& DisabledTooltip(std::string disabledTooltip_) {
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

    ButtonOptions& Tooltip(std::string tooltip_) {
        WidgetOptions::tooltip = tooltip_;
        return *this;
    }

    ButtonOptions& Color(Colors color_) {
        color = color_;
        return *this;
    }
    ButtonOptions& DisabledTooltip(const char* disabledTooltip_) {
        WidgetOptions::disabledTooltip = disabledTooltip_;
        return *this;
    }
};

struct ColorPickerOptions : WidgetOptions {
    ImVec2 size = Sizes::Fill;
    ImVec2 padding = ImVec2(10.0f, 8.0f);
    Colors color = Colors::Gray;
    Color_RGBA8 defaultValue = { 255, 255, 255, 255 };
    bool useAlpha, showReset, showRandom, showRainbow, showLock;

    ColorPickerOptions& Size(ImVec2 size_) {
        size = size_;
        return *this;
    }

    ColorPickerOptions& Padding(ImVec2 padding_) {
        padding = padding_;
        return *this;
    }

    ColorPickerOptions& Tooltip(std::string tooltip_) {
        WidgetOptions::tooltip = tooltip_;
        return *this;
    }

    ColorPickerOptions& ShowReset(bool showReset_ = true) {
        showReset = showReset_;
        return *this;
    }

    ColorPickerOptions& ShowRandom(bool showRandom_ = true) {
        showRandom = showRandom_;
        return *this;
    }

    ColorPickerOptions& ShowRainbow(bool showRainbow_ = true) {
        showRainbow = showRainbow_;
        return *this;
    }

    ColorPickerOptions& ShowLock(bool showLock_ = true) {
        showLock = showLock_;
        return *this;
    }

    ColorPickerOptions& UseAlpha(bool useAlpha_ = true) {
        useAlpha = useAlpha_;
        return *this;
    }

    ColorPickerOptions& Color(Colors color_) {
        color = color_;
        return *this;
    }

    ColorPickerOptions& DefaultValue(Color_RGBA8 defaultValue_) {
        defaultValue = defaultValue_;
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

    WindowButtonOptions& Tooltip(std::string tooltip_) {
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
    ComponentAlignments alignment = ComponentAlignments::Left;
    LabelPositions labelPosition = LabelPositions::Near;
    ImVec2 padding = ImVec2(10.0f, 8.0f);
    Colors color = Colors::LightBlue;

    CheckboxOptions& DefaultValue(bool defaultValue_) {
        defaultValue = defaultValue_;
        return *this;
    }

    CheckboxOptions& ComponentAlignment(ComponentAlignments alignment_) {
        alignment = alignment_;
        return *this;
    }

    CheckboxOptions& LabelPosition(LabelPositions labelPosition_) {
        labelPosition = labelPosition_;
        return *this;
    }

    CheckboxOptions& Tooltip(std::string tooltip_) {
        WidgetOptions::tooltip = tooltip_;
        return *this;
    }

    CheckboxOptions& Color(Colors color_) {
        color = color_;
        return *this;
    }

    CheckboxOptions& DisabledTooltip(std::string disabledTooltip_) {
        WidgetOptions::disabledTooltip = disabledTooltip_;
        return *this;
    }

    CheckboxOptions& Padding(ImVec2 padding_) {
        padding = padding_;
        return *this;
    }
};

struct ComboboxOptions : WidgetOptions {
    std::map<int32_t, const char*> comboMap = {};
    uint32_t defaultIndex = 0; // Only applicable to CVarCombobox
    ComponentAlignments alignment = ComponentAlignments::Left;
    LabelPositions labelPosition = LabelPositions::Above;
    ImGuiComboFlags flags = 0;
    Colors color = Colors::LightBlue;

    ComboboxOptions& ComboMap(std::map<int32_t, const char*> comboMap_) {
        comboMap = comboMap_;
        return *this;
    }

    ComboboxOptions& DefaultIndex(uint32_t defaultIndex_) {
        defaultIndex = defaultIndex_;
        return *this;
    }

    ComboboxOptions& ComponentAlignment(ComponentAlignments alignment_) {
        alignment = alignment_;
        return *this;
    }

    ComboboxOptions& LabelPosition(LabelPositions labelPosition_) {
        labelPosition = labelPosition_;
        return *this;
    }

    ComboboxOptions& Tooltip(std::string tooltip_) {
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
    ComponentAlignments alignment = ComponentAlignments::Left;
    LabelPositions labelPosition = LabelPositions::Above;
    Colors color = Colors::Gray;
    ImGuiSliderFlags flags = 0;
    ImVec2 size = { 0, 0 };

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

    IntSliderOptions& ComponentAlignment(ComponentAlignments alignment_) {
        alignment = alignment_;
        return *this;
    }

    IntSliderOptions& LabelPosition(LabelPositions labelPosition_) {
        labelPosition = labelPosition_;
        return *this;
    }

    IntSliderOptions& Tooltip(std::string tooltip_) {
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
    ComponentAlignments alignment = ComponentAlignments::Left;
    LabelPositions labelPosition = LabelPositions::Above;
    Colors color = Colors::Gray;
    ImGuiSliderFlags flags = 0;
    ImVec2 size = { 0, 0 };

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

    FloatSliderOptions& ComponentAlignment(ComponentAlignments alignment_) {
        alignment = alignment_;
        return *this;
    }

    FloatSliderOptions& LabelPosition(LabelPositions labelPosition_) {
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

    FloatSliderOptions& Tooltip(std::string tooltip_) {
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

struct BtnSelectorOptions : WidgetOptions {
    s32 defaultValue = 0;
    ComponentAlignments alignment = ComponentAlignments::Left;
    LabelPositions labelPosition = LabelPositions::Above;
    Colors color = Colors::Gray;

    BtnSelectorOptions& DefaultValue(int32_t defaultValue_) {
        defaultValue = defaultValue_;
        return *this;
    }

    BtnSelectorOptions& ComponentAlignment(ComponentAlignments alignment_) {
        alignment = alignment_;
        return *this;
    }

    BtnSelectorOptions& LabelPosition(LabelPositions labelPosition_) {
        labelPosition = labelPosition_;
        return *this;
    }

    BtnSelectorOptions& Tooltip(const char* tooltip_) {
        WidgetOptions::tooltip = tooltip_;
        return *this;
    }

    BtnSelectorOptions& Color(Colors color_) {
        color = color_;
        return *this;
    }
};

struct RadioButtonsOptions : WidgetOptions {
    std::map<int32_t, const char*> buttonMap;
    int32_t defaultIndex = 0;
    Colors color = Colors::LightBlue;

    RadioButtonsOptions& ButtonMap(std::map<int32_t, const char*> buttonMap_) {
        buttonMap = buttonMap_;
        return *this;
    }

    RadioButtonsOptions& Tooltip(std::string tooltip_) {
        WidgetOptions::tooltip = tooltip_;
        return *this;
    }

    RadioButtonsOptions& Color(Colors color_) {
        color = color_;
        return *this;
    }

    RadioButtonsOptions& DefaultIndex(int32_t defaultIndex_) {
        defaultIndex = defaultIndex_;
        return *this;
    }
};

struct InputOptions : WidgetOptions {
    ComponentAlignments alignment = ComponentAlignments::Left;
    LabelPositions labelPosition = LabelPositions::Above;
    Colors color = Colors::Gray;
    ImVec2 size = { 0, 0 };
    std::string placeholder = "";
    InputTypes type = InputTypes::String;
    std::string defaultValue = "";
    bool secret = false;
    ImGuiInputFlags addedFlags = 0;
    bool hasError = false;
    std::string errorText = "";

    InputOptions& Tooltip(std::string tooltip_) {
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

    InputOptions& LabelPosition(LabelPositions labelPosition_) {
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

    InputOptions& ComponentAlignment(ComponentAlignments alignment_) {
        alignment = alignment_;
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

    InputOptions& HasError(bool error_ = false) {
        hasError = error_;
        return *this;
    }

    InputOptions& ErrorText(std::string errorText_) {
        errorText = errorText_;
        return *this;
    }
};
};     // namespace UIWidgets
#endif /* UIWidgetOptions_hpp */
