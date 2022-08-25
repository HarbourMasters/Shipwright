//
//  UIWidgets.hpp
//  soh
//
//  Created by David Chavez on 25.08.22.
//

#ifndef UIWidgets_hpp
#define UIWidgets_hpp

#include <string>
#include <stdint.h>

namespace UIWidgets {

    // MARK: - Enums

    enum class CheckboxGraphics {
        Cross,
        Checkmark,
        None
    };

    char* WrappedText(const char* text, unsigned int charactersPerLine = 60);
    char* WrappedText(const std::string& text, unsigned int charactersPerLine);

    void Tooltip(const char* text);
    void Spacer(float height);

    void PaddedSeparator(bool padTop = true, bool padBottom = true, float extraVerticalTopPadding = 0.0f, float extraVerticalBottomPadding = 0.0f);
    void EnhancementCheckbox(const char* text, const char* cvarName, bool disabled = false, const char* disabledTooltipText = "", CheckboxGraphics disabledGraphic = CheckboxGraphics::Cross);
    void PaddedEnhancementCheckbox(const char* text, const char* cvarName, bool padTop = true, bool padBottom = true, bool disabled = false, const char* disabledTooltipText = "", CheckboxGraphics disabledGraphic = CheckboxGraphics::Cross);
    void EnhancementCombobox(const char* name, const char* ComboArray[], size_t arraySize, uint8_t FirstTimeValue);
    void PaddedText(const char* text, bool padTop = true, bool padBottom = true);
    void EnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue = 0, bool PlusMinusButton = false);
    void PaddedEnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue = 0, bool PlusMinusButton = false, bool padTop = true, bool padBottom = true);
    void EnhancementSliderFloat(const char* text, const char* id, const char* cvarName, float min, float max, const char* format, float defaultValue, bool isPercentage, bool PlusMinusButton = false);
    void EnhancementRadioButton(const char* text, const char* cvarName, int id);
}

#endif /* UIWidgets_hpp */
