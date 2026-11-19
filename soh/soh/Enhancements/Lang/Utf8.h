#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

// Shared scalar decoding for UTF-8 UI labels and native message adapters.
// Malformed input consumes one byte and yields U+FFFD. No allocation or I/O.
namespace Lang::Utf8 {
inline std::size_t ReadScalar(std::string_view text, std::size_t offset, uint32_t& codepoint) {
    codepoint = 0xFFFD;
    if (offset >= text.size())
        return 0;
    const auto first = static_cast<unsigned char>(text[offset]);
    if (first < 0x80) {
        codepoint = first;
        return 1;
    }
    const int length = first >= 0xC2 && first <= 0xDF   ? 2
                       : first >= 0xE0 && first <= 0xEF ? 3
                       : first >= 0xF0 && first <= 0xF4 ? 4
                                                        : 0;
    if (length == 0 || offset + length > text.size())
        return 1;
    uint32_t value = first & ((1u << (7 - length)) - 1);
    for (int index = 1; index < length; ++index) {
        const auto byte = static_cast<unsigned char>(text[offset + index]);
        if ((byte & 0xC0) != 0x80)
            return 1;
        value = (value << 6) | (byte & 0x3F);
    }
    if ((length == 2 && value < 0x80) || (length == 3 && value < 0x800) || (length == 4 && value < 0x10000) ||
        value > 0x10FFFF || (value >= 0xD800 && value <= 0xDFFF))
        return 1;
    codepoint = value;
    return length;
}

} // namespace Lang::Utf8
