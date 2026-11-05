#pragma once

#include <imgui.h>
#include <array>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace Lang {
// Legacy menu glyphs cover Latin-1; other Unicode text needs the atlas renderer.
bool RequiresUnicodeUIFont(std::string_view text);
// Immutable I8 copies of atlas glyphs for the game's texture-rectangle renderer.
// Own this cache for the life of the font/renderer: display lists retain pointers
// into its textures. UI language changes do not rebuild the shared font atlas.
class UIFontGlyphs {
  public:
    struct Glyph {
        int width = 0;
        int height = 0;
        float x = 0;
        float y = 0;
        float advance = 0;
        std::vector<unsigned char> pixels;
    };
    // Native text uses square 16x16 I4 cells, never variable-size atlas tiles.
    struct Cell {
        std::array<unsigned char, 128> pixels{};
        float advance = 16;
    };
    const Cell& GetCell(unsigned int codepoint);
    struct Placement {
        const Glyph* glyph;
        float x;
        float y;
    };
    explicit UIFontGlyphs(ImFont* font);
    const Glyph& Get(unsigned int codepoint);
    std::vector<Placement> Layout(std::string_view text);
    float Measure(std::string_view text);
    float FontSize() const;
    float CellEmSize() const;

  private:
    ImFont* font;
    std::map<unsigned int, Glyph> glyphs;
    std::map<unsigned int, Cell> cells;
};
} // namespace Lang
