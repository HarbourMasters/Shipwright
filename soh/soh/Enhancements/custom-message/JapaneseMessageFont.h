#pragma once

#include <cstdint>

// Character mapping only. The native Font loader owns the texture resources.
namespace JapaneseMessageFont {
// Returns zero for an unavailable glyph. Ordinary ASCII is not promoted to a
// full-width alias: mixed rendering uses Font_LoadChar for that native font.
uint16_t ToShiftJIS(uint32_t scalar);
} // namespace JapaneseMessageFont
