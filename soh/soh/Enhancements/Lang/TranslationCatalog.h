#pragma once

#include <map>
#include <string>
#include <string_view>
#include <cstdint>
#include <vector>
#include <nlohmann/json_fwd.hpp>

namespace Lang {

// Populate during startup, then treat as immutable. Lookups do not populate a
// cache, so UI and seed-generation readers can share the loaded catalog.
class TranslationCatalog {
  public:
    explicit TranslationCatalog(std::string fallbackLanguage = "en_US");

    // A language is replaced only after the complete document has been validated.
    // Throws std::invalid_argument for invalid paths or non-text leaf values.
    void AddLanguage(const std::string& language, const nlohmann::json& document);
    // Startup-only overlay: unspecified entries retain their built-in defaults.
    // Validation completes before any existing entry is changed.
    void MergeLanguage(const std::string& language, const nlohmann::json& document);
    bool HasLanguage(std::string_view language) const;
    uint64_t GetRevision() const noexcept {
        return revision;
    }
    std::vector<std::string> GetLanguages() const;
    // All loaded text for one-time font glyph collection, optionally limited to a
    // path prefix; independent of the selected locale.
    std::string GetFontText(const std::string& prefix = "") const;
    // An explicit selection takes precedence over a legacy setting. Only an
    // absent selection migrates legacySelection; unknown locales use fallback.
    std::string ResolveLanguage(const std::string& selection, const std::string& legacySelection = "") const;

    const std::string* FindExact(std::string_view language, std::string_view path) const;
    // Bind canonical built-in display-table values within one explicit scope.
    // Callers cache the result by catalog revision; never use this for user data.
    std::string FindEnglishKey(std::string_view prefix, std::string_view text) const;

    // Order: selected path, selected fallbackPath, English path, English fallbackPath.
    // Empty strings are intentional translations; only missing keys fall back.
    const std::string* Find(std::string_view language, std::string_view path, std::string_view fallbackPath = "") const;

  private:
    using Entries = std::map<std::string, std::string, std::less<>>;
    std::string fallbackLanguage;
    std::map<std::string, Entries, std::less<>> languages;
    uint64_t revision = 0;
};

} // namespace Lang
