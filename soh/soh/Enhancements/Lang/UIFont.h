#pragma once

#include <imgui.h>

namespace Lang {

// Call once during startup after archives are mounted. Storage remains alive
// until the atlas builds, and covers all UI locales before any language switch.
const ImWchar* GetUIFontGlyphRanges();

} // namespace Lang
