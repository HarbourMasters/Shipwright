#include "LanguagePolicy.h"

#include <string_view>

uint8_t LanguagePolicy_Normalize(int32_t language) {
    return language >= 0 && language < 4 ? static_cast<uint8_t>(language) : 0;
}

uint8_t LanguagePolicy_LegacyText(int32_t language) {
    const uint8_t normalized = LanguagePolicy_Normalize(language);
    return normalized == 3 ? 0 : normalized;
}

const char* LanguagePolicy_Locale(int32_t language) {
    static const char* const locales[] = { "en_US", "de_DE", "fr_FR", "ja_JP" };
    return locales[LanguagePolicy_Normalize(language)];
}

uint8_t LanguagePolicy_FromLocale(const char* locale) {
    const std::string_view name = locale ? locale : "";
    for (uint8_t language = 0; language < 4; ++language) {
        if (name == LanguagePolicy_Locale(language)) {
            return language;
        }
    }
    return 0;
}
