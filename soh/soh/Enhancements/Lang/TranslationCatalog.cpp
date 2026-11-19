#include "TranslationCatalog.h"

#include <stdexcept>
#include <utility>
#include <nlohmann/json.hpp>

namespace {

void CollectEntries(const nlohmann::json& document, const std::string& prefix,
                    std::map<std::string, std::string, std::less<>>& entries) {
    if (document.is_object()) {
        for (const auto& [key, value] : document.items()) {
            if (key.empty() || key.find('.') != std::string::npos) {
                throw std::invalid_argument("Translation keys must be non-empty and cannot contain '.': " + prefix);
            }
            CollectEntries(value, prefix.empty() ? key : prefix + "." + key, entries);
        }
        return;
    }

    if (document.is_string()) {
        entries.emplace(prefix, document.get<std::string>());
        return;
    }

    if (document.is_array()) {
        std::string text;
        for (const auto& part : document) {
            if (!part.is_string()) {
                throw std::invalid_argument("Translation arrays must contain only strings: " + prefix);
            }
            text += part.get_ref<const std::string&>();
        }
        entries.emplace(prefix, std::move(text));
        return;
    }

    throw std::invalid_argument("Translation values must be strings, string arrays, or objects: " + prefix);
}

} // namespace

namespace Lang {

TranslationCatalog::TranslationCatalog(std::string fallbackLanguage) : fallbackLanguage(std::move(fallbackLanguage)) {
}

void TranslationCatalog::AddLanguage(const std::string& language, const nlohmann::json& document) {
    if (language.empty() || !document.is_object()) {
        throw std::invalid_argument("A translation catalog requires a language ID and a JSON object");
    }
    Entries entries;
    CollectEntries(document, "", entries);
    languages.insert_or_assign(language, std::move(entries));
    ++revision;
}

bool TranslationCatalog::HasLanguage(std::string_view language) const {
    return languages.contains(language);
}

void TranslationCatalog::MergeLanguage(const std::string& language, const nlohmann::json& document) {
    if (language.empty() || !document.is_object()) {
        throw std::invalid_argument("A translation catalog requires a language ID and a JSON object");
    }
    Entries entries;
    CollectEntries(document, "", entries);
    auto& target = languages[language];
    for (auto& [path, text] : entries) {
        target.insert_or_assign(path, std::move(text));
    }
    ++revision;
}

std::string TranslationCatalog::ResolveLanguage(const std::string& selection,
                                                const std::string& legacySelection) const {
    const std::string& candidate = selection.empty() ? legacySelection : selection;
    return HasLanguage(candidate) ? candidate : fallbackLanguage;
}

std::vector<std::string> TranslationCatalog::GetLanguages() const {
    std::vector<std::string> result;
    result.reserve(languages.size());
    for (const auto& [language, entries] : languages) {
        result.push_back(language);
    }
    return result;
}

const std::string* TranslationCatalog::FindExact(std::string_view language, std::string_view path) const {
    const auto catalog = languages.find(language);
    if (catalog == languages.end()) {
        return nullptr;
    }
    const auto entry = catalog->second.find(path);
    return entry == catalog->second.end() ? nullptr : &entry->second;
}

std::string TranslationCatalog::GetFontText(const std::string& prefix) const {
    std::string text;
    for (const auto& [language, entries] : languages) {
        for (const auto& [path, value] : entries) {
            if (path.compare(0, prefix.size(), prefix) != 0) {
                continue;
            }
            // Combo labels use embedded NUL separators. ImGui's glyph builder
            // reads a C string, so only the font-text copy must replace them.
            for (const char character : value) {
                text += character == '\0' ? '\n' : character;
            }
            text += '\n';
        }
    }
    return text;
}

const std::string* TranslationCatalog::Find(std::string_view language, std::string_view path,
                                            std::string_view fallbackPath) const {
    for (const std::string_view locale : { language, std::string_view(fallbackLanguage) }) {
        if (const auto* text = FindExact(locale, path)) {
            return text;
        }
        if (!fallbackPath.empty()) {
            if (const auto* text = FindExact(locale, fallbackPath)) {
                return text;
            }
        }
    }
    return nullptr;
}

} // namespace Lang

std::string Lang::TranslationCatalog::FindEnglishKey(std::string_view prefix, std::string_view text) const {
    const auto language = languages.find(fallbackLanguage);
    if (language == languages.end())
        return {};
    const std::string start = std::string(prefix) + ".";
    for (auto entry = language->second.lower_bound(start);
         entry != language->second.end() && entry->first.starts_with(start); ++entry) {
        if (entry->second == text)
            return entry->first;
    }
    return {};
}
