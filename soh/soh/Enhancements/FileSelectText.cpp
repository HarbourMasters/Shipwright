#include "FileSelectEnhancements.h"
#include "soh/Enhancements/Lang/Lang.h"
#include "soh/Enhancements/Lang/UIFontGlyphs.h"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/Lang/Utf8.h"
#include <string_view>
#include "soh/frame_interpolation.h"
#include <algorithm>
#include <cmath>

extern "C" {
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "regs.h"
#include "variables.h"
}

const char* SohFileSelect_GetSettingText(u8 optionIndex) {
    static constexpr const char* keys[] = {
        "ui.file_select.start_randomizer",
        "ui.file_select.generate_seed",
        "ui.file_select.open_settings",
        "ui.file_select.generating",
#if defined(__WIIU__) || defined(__SWITCH__)
        "ui.file_select.no_seed_console",
#else
        "ui.file_select.no_seed_desktop",
#endif
    };
    static_assert(sizeof(keys) / sizeof(keys[0]) == RSM_MAX);
    return optionIndex < RSM_MAX ? Lang::Translate(keys[optionIndex]).c_str() : "";
}

void SohFileSelect_DrawSettingText(GraphicsContext* gfx, u8 optionIndex, s16 x, s16 y, u8 red, u8 green, u8 blue,
                                   u8 alpha, float scale, bool shadow) {
    const std::string_view text = SohFileSelect_GetSettingText(optionIndex);
    // Preserve the original glyphs/spacing for the three existing Western locales.
    // The native line renderer cannot decode UTF-8 Japanese or Unicode punctuation.
    if (Lang::GetUIResourceLanguage() != LANGUAGE_JPN && !Lang::RequiresUnicodeUIFont(text)) {
        Interface_DrawTextLine(gfx, const_cast<char*>(text.data()), x, y, red, green, blue, alpha, scale, shadow);
        return;
    }
    ImFont* font = OTRGlobals::Instance->fontJapanese;
    if (font == nullptr || scale <= 0) {
        return;
    }
    static Lang::UIFontGlyphs glyphs(font);
    float pixelScale = 16.0f * (R_TEXT_CHAR_SCALE / 100.0f) * scale / glyphs.CellEmSize();
    const float width = glyphs.Measure(text);
    // Keep translated warning lines inside the original 320px menu canvas.
    if (width > 0) {
        pixelScale = std::min(pixelScale, (314.0f - x) / width);
    }
    if (pixelScale <= 0) {
        return;
    }
    // Use the same fixed-cell I4 text renderer as the rest of this menu.
    // Atlas padding and variable tile dimensions must not affect glyph shape.
    const float nativeScale = pixelScale * glyphs.CellEmSize() / (16.0f * (R_TEXT_CHAR_SCALE / 100.0f));
    float penX = x, penY = y;
    for (size_t cursor = 0; cursor < text.size();) {
        uint32_t scalar;
        const size_t bytes = Lang::Utf8::ReadScalar(text, cursor, scalar);
        cursor += bytes;
        if (scalar == '\n') {
            penX = x;
            penY += glyphs.CellEmSize() * pixelScale;
            continue;
        }
        const auto& cell = glyphs.GetCell(scalar);
        Interface_DrawTextCharacter(gfx, static_cast<s16>(std::lround(penX)), static_cast<s16>(std::lround(penY)),
                                    const_cast<unsigned char*>(cell.pixels.data()), red, green, blue, alpha,
                                    nativeScale, shadow);
        penX += cell.advance * (R_TEXT_CHAR_SCALE / 100.0f) * nativeScale;
    }
}
