#include "GameText.h"

#include "soh/Enhancements/Lang/TranslationCatalog.h"
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace CustomMessages {
nlohmann::json GetBuiltinGameCatalogs();

Lang::TranslationCatalog& GetGameTextCatalog() {
    static Lang::TranslationCatalog catalog = [] {
        Lang::TranslationCatalog result;
        const auto builtins = GetBuiltinGameCatalogs();
        for (const auto& [locale, document] : builtins.items()) {
            result.AddLanguage(locale, document);
        }
        return result;
    }();
    return catalog;
}

std::string GetGameItemUIFontText() {
    const auto& catalog = GetGameTextCatalog();
    return catalog.GetFontText("messages.vanilla.item_names.") + catalog.GetFontText("messages.randomizer.item_names.");
}

std::string DecodeGameText(const std::string& text) {
    const auto hex = [](char c) -> int {
        if (c >= '0' && c <= '9')
            return c - '0';
        if (c >= 'A' && c <= 'F')
            return c - 'A' + 10;
        if (c >= 'a' && c <= 'f')
            return c - 'a' + 10;
        return -1;
    };
    std::string result;
    size_t cursor = 0;
    while (cursor < text.size()) {
        const auto start = text.find("[[byte:", cursor);
        if (start == std::string::npos) {
            result.append(text, cursor, std::string::npos);
            break;
        }
        result.append(text, cursor, start - cursor);
        if (start + 11 > text.size() || text.compare(start + 9, 2, "]]") != 0 || hex(text[start + 7]) < 0 ||
            hex(text[start + 8]) < 0) {
            throw std::invalid_argument("Invalid [[byte:HH]] in game text");
        }
        result += static_cast<char>((hex(text[start + 7]) << 4) | hex(text[start + 8]));
        cursor = start + 11;
    }
    return result;
}

std::string GetGameText(const std::string& locale, const std::string& key) {
    const auto* text = GetGameTextCatalog().Find(locale, "messages." + key);
    if (text == nullptr) {
        throw std::invalid_argument("Missing game text: " + locale + ":" + key);
    }
    return DecodeGameText(*text);
}

std::string GetEnglishText(const std::string& key) {
    return GetGameText("en_US", key);
}

std::vector<std::string> GetGameTextPool(const std::string& locale, const std::string& key) {
    const auto read = [&](const std::string& language) {
        std::vector<std::string> result;
        for (size_t index = 0;; ++index) {
            const auto* text =
                GetGameTextCatalog().FindExact(language, "messages." + key + "." + std::to_string(index));
            if (text == nullptr)
                break;
            result.push_back(DecodeGameText(*text));
        }
        return result;
    };
    auto result = read(locale);
    if (result.empty() && locale != "en_US")
        result = read("en_US");
    if (result.empty())
        throw std::invalid_argument("Missing game text pool: " + locale + ":" + key);
    return result;
}
} // namespace CustomMessages
