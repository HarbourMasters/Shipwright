//
//  UIWidgets.hpp
//  soh
//
//  Created by David Chavez on 25.08.22.
//

#ifndef UIWidgets_hpp
#define UIWidgets_hpp

#include <string>
#include <vector>
#include <stdint.h>
#include <ImGui/imgui.h>

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

    char* WrappedText(const char* text, unsigned int charactersPerLine = 60);
    char* WrappedText(const std::string& text, unsigned int charactersPerLine);

    void SetLastItemHoverText(const std::string& text);
    void SetLastItemHoverText(const char* text);

    void InsertHelpHoverText(const std::string& text);
    void InsertHelpHoverText(const char* text);

    void Tooltip(const char* text);
    void Spacer(float height);

    void PaddedSeparator(bool padTop = true, bool padBottom = true, float extraVerticalTopPadding = 0.0f, float extraVerticalBottomPadding = 0.0f);
    bool EnhancementCheckbox(const char* text, const char* cvarName, bool disabled = false, const char* disabledTooltipText = "", CheckboxGraphics disabledGraphic = CheckboxGraphics::Cross, bool defaultValue = false);
    bool PaddedEnhancementCheckbox(const char* text, const char* cvarName, bool padTop = true, bool padBottom = true, bool disabled = false, const char* disabledTooltipText = "", CheckboxGraphics disabledGraphic = CheckboxGraphics::Cross, bool defaultValue = false);
    void EnhancementCombo(const std::string& name, const char* cvarName, const std::vector<std::string>& items, int defaultValue = 0);
    bool EnhancementCombobox(const char* name, const char* ComboArray[], size_t arraySize, uint8_t FirstTimeValue, bool disabled = false, const char* disabledTooltipText = "", uint8_t disabledValue = -1);
    void PaddedText(const char* text, bool padTop = true, bool padBottom = true);
    bool EnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue = 0, bool PlusMinusButton = true, bool disabled = false, const char* disabledTooltipText = "");
    bool PaddedEnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue = 0, bool PlusMinusButton = true, bool padTop = true, bool padBottom = true, bool disabled = false, const char* disabledTooltipText = "");
    bool EnhancementSliderFloat(const char* text, const char* id, const char* cvarName, float min, float max, const char* format, float defaultValue, bool isPercentage, bool PlusMinusButton = true, bool disabled = false, const char* disabledTooltipText = "");
    bool PaddedEnhancementSliderFloat(const char* text, const char* id, const char* cvarName, float min, float max, const char* format, float defaultValue, bool isPercentage, bool PlusMinusButton = true, bool padTop = true, bool padBottom = true, bool disabled = false, const char* disabledTooltipText = "");
    bool EnhancementRadioButton(const char* text, const char* cvarName, int id);

    bool EnhancementColor(const char* text, const char* cvarName, ImVec4 ColorRGBA, ImVec4 default_colors, bool allow_rainbow = true, bool has_alpha=false, bool TitleSameLine=false);
    void DrawFlagArray32(const std::string& name, uint32_t& flags);
    void DrawFlagArray16(const std::string& name, uint16_t& flags);
    void DrawFlagArray8(const std::string& name, uint8_t& flags);
}

#endif /* UIWidgets_hpp */
