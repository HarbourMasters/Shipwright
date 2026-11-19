#pragma once

#include <string>
#include <vector>

namespace Lang {
class TranslationCatalog;
}

namespace CustomMessages {

// Text for an explicit locale, independent of both language selectors.
// Missing entries fall back to English; explicit empty/legacy marker values
// are preserved for the existing Text/CustomMessage fallback semantics.
std::string GetGameText(const std::string& locale, const std::string& key);
std::string GetEnglishText(const std::string& key);

// Numeric objects represent ordered locale-specific pools. If the requested
// pool is absent, use the complete English pool, never mix languages by index.
std::vector<std::string> GetGameTextPool(const std::string& locale, const std::string& key);

// Catalogs use [[byte:HH]] for legacy encoded bytes, avoiding binary controls
// in translator-edited JSON. Decode before the existing message formatter.
std::string DecodeGameText(const std::string& text);

// Shared by the archive loader and the English constructor path. Initialized
// from the same JSON at first use, including use before archive loading.
Lang::TranslationCatalog& GetGameTextCatalog();

// Item names also appear in UTF-8 UI notifications. Collect their glyphs without
// adding the entire native message catalog to the UI font atlas.
std::string GetGameItemUIFontText();

} // namespace CustomMessages
