#pragma once
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
// Only active for a custom UTF-8 buffer. Native Japanese ROM text stays on its
// existing decoder. Each page owns advance metadata indexed by decoded position.
void GameMessage_BeginPage(void);
// Returns zero for native Latin/button glyphs; the caller uses Font_LoadChar.
size_t GameMessage_DecodeGlyph(const char* input, size_t length, uint8_t* texture, uint16_t decodedIndex);
float GameMessage_GetAdvance(uint16_t decodedIndex, float legacyAdvance);
int16_t GameMessage_GetDrawOffset(uint16_t decodedIndex);
// Uniform texture scale for native Latin glyphs in Japanese custom pages.
// Full-width glyphs, buttons, other languages and ROM text keep scale 1.
float GameMessage_GetDrawScale(uint16_t decodedIndex);
float GameMessage_GetScaledAdvance(uint16_t decodedIndex, float legacyAdvance, float scale);
// Finalize Latin glyph metrics and measure the fully decoded page once, after
// names/numbers are substituted. Metrics also apply to left-aligned/icon pages.
// Only Japanese custom UTF-8 pages without an item icon receive alignment.
void GameMessage_AlignPage(const uint8_t* decoded, size_t length, const float* legacyWidths, size_t legacyWidthCount,
                           float scale, int16_t spaceAdvance, int16_t baseX);
int16_t GameMessage_GetLineStart(uint16_t decodedIndex, int16_t legacyX);
int16_t GameMessage_GetChoiceX(uint8_t choiceIndex, int16_t legacyX);
#ifdef __cplusplus
}
#endif
