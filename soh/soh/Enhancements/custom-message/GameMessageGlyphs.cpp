#include "GameMessageGlyphs.h"
#include "UnicodeMessage.h"
#include "JapaneseMessageFont.h"
#include "JapaneseMessageLayout.h"
#include "soh/Enhancements/Lang/MessageLanguage.h"
#include "soh/Enhancements/Lang/UIFontGlyphs.h"
#include "soh/OTRGlobals.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <limits>

extern "C" {
#include "functions.h"
#include "message_data_fmt.h"
#include "textures/nes_font_static/nes_font_static.h"
}

namespace {
std::array<float, 200> advances{};
std::array<uint32_t, 200> japaneseScalars{};
std::array<bool, 200> latinGlyphs{};
constexpr int16_t Unaligned = std::numeric_limits<int16_t>::min();
std::array<int16_t, 200> lineStarts;
std::array<int16_t, 3> choiceShifts;
bool aligned = false;

void ResetAlignment() {
    aligned = false;
    lineStarts.fill(Unaligned);
    choiceShifts.fill(Unaligned);
}
} // namespace
void GameMessage_BeginPage(void) {
    advances.fill(0);
    japaneseScalars.fill(0);
    latinGlyphs.fill(false);
    ResetAlignment();
}

size_t GameMessage_DecodeGlyph(const char* input, size_t length, uint8_t* texture, uint16_t decodedIndex) {
    if (!length || decodedIndex >= advances.size())
        return 0;
    const auto first = static_cast<unsigned char>(input[0]);
    if (JapaneseMessageLayout::IsAscii(first) || (first >= 0x80 && first <= 0xAB))
        return 0; // Native Latin/button glyph: the caller uses Font_LoadChar.
    uint32_t scalar;
    const auto bytes = UnicodeMessage::ReadScalar(std::string_view(input, length), 0, scalar);
    std::memset(texture, 0, 128);
    advances[decodedIndex] = 16;
    if (MessageLanguage_Get() == LANGUAGE_JPN) {
        japaneseScalars[decodedIndex] = scalar;
        const uint16_t character = JapaneseMessageFont::ToShiftJIS(scalar);
        // Reuse the same resource reference as Font_LoadCharWide, including
        // native texture replacements. Never rasterize a UI font for Japanese.
        const char* path = Font_GetCharWideTexture(character ? character : 0x81AC); // geta mark
        if (path == nullptr)
            path = gMsgChar3FQuestionMarkTex; // Japanese assets unavailable (PAL-only).
        std::memcpy(texture, path, std::strlen(path) + 1);
        return bytes;
    }
    if (OTRGlobals::Instance != nullptr && OTRGlobals::Instance->fontJapanese != nullptr) {
        static Lang::UIFontGlyphs glyphs(OTRGlobals::Instance->fontJapanese);
        const auto& cell = glyphs.GetCell(scalar);
        std::memcpy(texture, cell.pixels.data(), cell.pixels.size());
        advances[decodedIndex] = cell.advance;
    }
    return bytes;
}
float GameMessage_GetAdvance(uint16_t decodedIndex, float legacyAdvance) {
    return decodedIndex < advances.size() && advances[decodedIndex] > 0 ? advances[decodedIndex] : legacyAdvance;
}
int16_t GameMessage_GetDrawOffset(uint16_t decodedIndex) {
    return decodedIndex < japaneseScalars.size() && japaneseScalars[decodedIndex]
               ? static_cast<int16_t>(JapaneseMessageLayout::DrawOffset(japaneseScalars[decodedIndex]))
               : 0;
}
float GameMessage_GetDrawScale(uint16_t decodedIndex) {
    return MessageLanguage_UsesUnicode() && MessageLanguage_UsesJapaneseLayout() && decodedIndex < latinGlyphs.size() &&
                   latinGlyphs[decodedIndex]
               ? JapaneseMessageLayout::LatinGlyphScale
               : 1.0f;
}
float GameMessage_GetScaledAdvance(uint16_t decodedIndex, float legacyAdvance, float scale) {
    return decodedIndex < japaneseScalars.size() && japaneseScalars[decodedIndex]
               ? static_cast<float>(JapaneseMessageLayout::Advance(japaneseScalars[decodedIndex], scale))
               : GameMessage_GetAdvance(decodedIndex, legacyAdvance) * scale * GameMessage_GetDrawScale(decodedIndex);
}

void GameMessage_AlignPage(const uint8_t* decoded, size_t length, const float* legacyWidths, size_t legacyWidthCount,
                           float scale, int16_t spaceAdvance, int16_t baseX) {
    ResetAlignment();
    latinGlyphs.fill(false);
    // The native Japanese ROM decoder never enters this path, even when the
    // selected game language is Japanese. English fallback pages also opt out.
    if (!MessageLanguage_UsesUnicode() || !MessageLanguage_UsesJapaneseLayout() || decoded == nullptr ||
        legacyWidths == nullptr || length == 0 || length > lineStarts.size())
        return;
    // Inspect the final decoded bytes so player names and numeric variables
    // receive the same scale as authored Latin text. Never classify control operands
    // or the '?' placeholder used for a full-width Unicode glyph as Latin.
    for (size_t index = 0; index < length;) {
        const auto code = decoded[index];
        const auto size = code < 0x20 && code != MESSAGE_BACKGROUND ? UnicodeMessage::ControlSize(code) : 1;
        if (size > length - index)
            break;
        if (code == MESSAGE_END || code == MESSAGE_BOX_BREAK || code == MESSAGE_BOX_BREAK_DELAYED ||
            code == MESSAGE_TEXTID || code == MESSAGE_EVENT)
            break;
        // The native Latin font also contains legacy accent bytes 0x80-0x9E.
        // Controller symbols start at 0x9F and retain their native geometry.
        latinGlyphs[index] = !japaneseScalars[index] && code > ' ' && code < 0x9F;
        index += size;
    }
    const auto alignment = MessageLanguage_GetAlignment();
    if (alignment == MESSAGE_ALIGNMENT_LEFT)
        return; // Preserve native margins, choice indentation and authored SHIFT.

    size_t lineStart = 0;
    int pen = 0, left = 0, right = 0;
    bool visible = false;
    int choice = -1;
    const auto finishLine = [&] {
        const int width = right - left;
        const int remaining = std::max(0, JapaneseMessageLayout::LineWidth(false) - width);
        const auto start =
            static_cast<int16_t>(baseX + (alignment == MESSAGE_ALIGNMENT_RIGHT ? remaining : remaining / 2) - left);
        lineStarts[lineStart] = start;
        if (choice >= 0 && choice < static_cast<int>(choiceShifts.size())) {
            // Preserve the native distance between the cursor and the option's
            // indented origin. Each option follows its own aligned line.
            choiceShifts[static_cast<size_t>(choice++)] = static_cast<int16_t>(start + left - (baseX + 32));
        }
    };
    for (size_t index = 0; index < length;) {
        const auto code = decoded[index];
        // BACKGROUND operands have already been consumed by the decoder and
        // stored on MessageContext. Other retained controls keep their operands.
        const auto size = code < 0x20 && code != MESSAGE_BACKGROUND ? UnicodeMessage::ControlSize(code) : 1;
        if (size > length - index || code == MESSAGE_ITEM_ICON) {
            ResetAlignment();
            return;
        }
        if (code == MESSAGE_END || code == MESSAGE_BOX_BREAK || code == MESSAGE_BOX_BREAK_DELAYED ||
            code == MESSAGE_TEXTID || code == MESSAGE_EVENT)
            break;
        if (code == MESSAGE_NEWLINE) {
            finishLine();
            lineStart = index + 1;
            pen = left = right = 0;
            visible = false;
        } else if (code == MESSAGE_SHIFT) {
            pen += decoded[index + 1];
        } else if (code == MESSAGE_TWO_CHOICE || code == MESSAGE_THREE_CHOICE) {
            if (alignment == MESSAGE_ALIGNMENT_AUTO) {
                ResetAlignment();
                return;
            }
            choice = 0;
        } else if (code == ' ') {
            pen += spaceAdvance;
        } else if (code == MESSAGE_BACKGROUND) {
            pen += 32;
        } else if (code == MESSAGE_NAME || code >= ' ') {
            if (!visible) {
                left = pen;
                visible = true;
            }
            // Japanese filename glyphs retain MESSAGE_NAME in the decoded
            // buffer; all other variables are already ordinary glyphs here.
            const size_t legacyIndex = code - ' ';
            const float legacy = legacyIndex < legacyWidthCount ? legacyWidths[legacyIndex] : 16;
            pen += code == MESSAGE_NAME
                       ? static_cast<int>(16 * scale)
                       : static_cast<int>(GameMessage_GetScaledAdvance(static_cast<uint16_t>(index), legacy, scale));
            right = pen;
        }
        index += size;
    }
    if (lineStart < length)
        finishLine();
    aligned = true;
}

int16_t GameMessage_GetLineStart(uint16_t decodedIndex, int16_t legacyX) {
    return aligned && MessageLanguage_UsesUnicode() && MessageLanguage_UsesJapaneseLayout() &&
                   decodedIndex < lineStarts.size() && lineStarts[decodedIndex] != Unaligned
               ? lineStarts[decodedIndex]
               : legacyX;
}

int16_t GameMessage_GetChoiceX(uint8_t choiceIndex, int16_t legacyX) {
    return aligned && MessageLanguage_UsesUnicode() && MessageLanguage_UsesJapaneseLayout() &&
                   choiceIndex < choiceShifts.size() && choiceShifts[choiceIndex] != Unaligned
               ? static_cast<int16_t>(legacyX + choiceShifts[choiceIndex])
               : legacyX;
}
