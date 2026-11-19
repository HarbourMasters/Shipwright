#include "text.h"
#include "soh/Enhancements/Lang/LanguagePolicy.h"
#include "soh/ShipUtils.h"
#include <functional>
#include <nlohmann/json.hpp>
#include "z64.h"

Text::Text() = default;

void to_json(nlohmann::json& json, const Text& text) {
    json = { { "english", text.GetEnglish() },
             { "french", text.GetFrench() },
             { "german", text.GetGerman() },
             { "spanish", text.GetSpanish() },
             { "locales", text.GetLocales() } };
}

void from_json(const nlohmann::json& json, Text& text) {
    Text result;
    if (json.is_string()) {
        result = Text(json.get<std::string>());
    } else if (json.is_object()) {
        result = Text(json.value("english", ""), json.value("french", ""), json.value("german", ""),
                      json.value("spanish", ""));
        if (json.contains("locales")) {
            for (const auto& [locale, value] : json.at("locales").items())
                result.SetForLocale(locale, value.get<std::string>());
        }
    }
    text = std::move(result);
}

Text::Text(std::string english_, std::string french_, std::string german_)
    : english(std::move(english_)), french(std::move(french_)), german(std::move(german_)), spanish("") {
    spanish = english;
}

Text::Text(std::string english_, std::string french_, std::string german_, std::string spanish_)
    : english(std::move(english_)), french(std::move(french_)), german(std::move(german_)),
      spanish(std::move(spanish_)) {
}

Text::Text(std::string english_) : english(std::move(english_)), french(""), german(""), spanish("") {
    french = spanish = german = english;
}

static bool IsMissing(const std::string& translation) {
    return translation.empty() || translation == TODO_TRANSLATE;
}

// Use English where a translation is missing, so text never shows "TranslateThis"
void Text::FillMissingTranslations() {
    for (std::string* translation : { &french, &german, &spanish }) {
        if (IsMissing(*translation)) {
            *translation = english;
        }
    }
}

const std::string& Text::GetEnglish() const {
    return english;
}

const std::string& Text::GetFrench() const {
    return IsMissing(french) ? english : french;
}

const std::string& Text::GetGerman() const {
    return IsMissing(german) ? english : german;
}

const std::string& Text::GetSpanish() const {
    return IsMissing(spanish) ? english : spanish;
}

const std::string& Text::GetForLanguage(uint8_t language) const {
    if (language == 3)
        return GetForLocale("ja_JP");
    switch (LanguagePolicy_LegacyText(language)) {
        case 0:
            return GetEnglish();
        case 2:
            return GetFrench();
        case 1:
            return GetGerman();
        default:
            return GetEnglish();
    }
}

const std::string& Text::GetForLocale(const std::string& locale) const {
    if (locale == "en_US")
        return GetEnglish();
    if (locale == "de_DE")
        return GetGerman();
    if (locale == "fr_FR")
        return GetFrench();
    const auto found = locales.find(locale);
    return found == locales.end() || found->second == TODO_TRANSLATE ? english : found->second;
}

void Text::SetForLocale(const std::string& locale, std::string text) {
    if (locale == "en_US")
        english = std::move(text);
    else if (locale == "de_DE")
        german = std::move(text);
    else if (locale == "fr_FR")
        french = std::move(text);
    else
        locales[locale] = std::move(text);
}

const std::map<std::string, std::string>& Text::GetLocales() const {
    return locales;
}

std::vector<std::reference_wrapper<std::string>> Text::AllTexts() {
    std::vector<std::reference_wrapper<std::string>> result = { english, french, german, spanish };
    for (auto& [locale, text] : locales)
        result.emplace_back(text);
    return result;
}

Text Text::operator+(const Text& right) const {
    Text result{ english + right.GetEnglish(), french + right.GetFrench(), german + right.GetGerman(),
                 spanish + right.GetSpanish() };
    for (const auto& [locale, text] : locales)
        result.SetForLocale(locale, text + right.GetForLocale(locale));
    for (const auto& [locale, text] : right.locales)
        if (!locales.contains(locale))
            result.SetForLocale(locale, GetForLocale(locale) + text);
    return result;
}

Text Text::operator+(const std::string& right) const {
    Text result{ english + right, french + right, german + right, spanish + right };
    for (const auto& [locale, text] : locales)
        result.SetForLocale(locale, text + right);
    return result;
}

bool Text::operator==(const Text& right) const {
    return english == right.english;
}

bool Text::operator==(const std::string& right) const {
    return english == right || french == right || german == right || spanish == right;
}

bool Text::operator!=(const Text& right) const {
    return !operator==(right);
}

static void replaceAll(std::string& target, const std::string& oldStr, const std::string& replacement) {
    size_t position = target.find(oldStr);
    while (position != std::string::npos) {
        target.replace(position, oldStr.length(), replacement);
        position = target.find(oldStr);
    }
}

void Text::Replace(const std::string& oldStr, const std::string& newStr) {
    for (std::string& str : AllTexts()) {
        replaceAll(str, oldStr, newStr);
    }
}

void Text::Replace(const std::string& oldStr, const Text& newText) {
    for (const auto& [locale, text] : newText.locales)
        if (!locales.contains(locale))
            locales[locale] = english;
    replaceAll(english, oldStr, newText.GetEnglish());
    replaceAll(french, oldStr, newText.GetFrench());
    replaceAll(german, oldStr, newText.GetGerman());
    replaceAll(spanish, oldStr, newText.GetSpanish());
    for (auto& [locale, text] : locales)
        replaceAll(text, oldStr, newText.GetForLocale(locale));
}

static bool isAsciiVowel(char c) {
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'A' || c == 'E' || c == 'I' || c == 'O' ||
           c == 'U';
}

static bool isAsciiLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Uniformly picks one position where pred holds, in a single pass. npos if nothing matches.
static size_t randomMatch(const std::string& target, bool (*pred)(char), uint64_t* randState) {
    uint32_t seen = 0;
    size_t pos = std::string::npos;

    for (size_t i = 0; i < target.size(); ++i) {
        if (pred(target[i]) && ShipUtils::Random(0, ++seen, randState) == 0) {
            pos = i;
        }
    }

    return pos;
}

static void replaceRandomVowel(std::string& target, uint64_t* randState) {
    size_t pos = randomMatch(target, isAsciiVowel, randState);

    if (pos == std::string::npos) {
        return;
    }

    static constexpr char vowels[] = { 'a', 'e', 'i', 'o', 'u' };
    bool upper = target[pos] >= 'A' && target[pos] <= 'Z';
    char oldVowel = upper ? target[pos] + ('a' - 'A') : target[pos];

    // don't replace vowel with itself
    uint32_t idx = ShipUtils::Random(0, 4, randState);
    idx += vowels[idx] >= oldVowel;

    target[pos] = upper ? vowels[idx] - ('a' - 'A') : vowels[idx];
}

void Text::ReplaceRandomVowel(uint64_t* randState) {
    for (std::string& str : AllTexts()) {
        replaceRandomVowel(str, randState);
    }
}

static void duplicateRandomLetter(std::string& target, uint64_t* randState) {
    size_t pos = randomMatch(target, isAsciiLetter, randState);

    if (pos != std::string::npos) {
        target.insert(pos + 1, 1, target[pos]);
    }
}

void Text::DuplicateRandomLetter(uint64_t* randState) {
    for (std::string& str : AllTexts()) {
        duplicateRandomLetter(str, randState);
    }
}
