#include "JapaneseMessageFont.h"
#include <algorithm>
#include <iterator>

namespace JapaneseMessageFont {
namespace {
struct Character {
    uint16_t unicode;
    uint16_t shiftJIS;
};
constexpr Character characters[] = {
#include "JapaneseMessageFontTable.inc"
};
} // namespace

uint16_t ToShiftJIS(uint32_t scalar) {
    // JIS uses curly quotation marks and U+301C/U+2212 instead of the
    // corresponding full-width straight-quote and Windows forms.
    switch (scalar) {
        case 0xFF02:
            scalar = 0x201D;
            break;
        case 0xFF07:
            scalar = 0x2019;
            break;
        case 0xFF0D:
            scalar = 0x2212;
            break;
        case 0xFF5E:
            scalar = 0x301C;
            break;
        default:
            break;
    }
    const auto* found =
        std::lower_bound(std::begin(characters), std::end(characters), scalar,
                         [](const Character& character, uint32_t value) { return character.unicode < value; });
    return found != std::end(characters) && found->unicode == scalar ? found->shiftJIS : 0;
}
} // namespace JapaneseMessageFont
