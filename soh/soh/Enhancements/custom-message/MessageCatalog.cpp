#include "MessageCatalog.h"
#include "GameText.h"

#include "soh/Enhancements/Lang/Lang.h"
#include "soh/Enhancements/Lang/TranslationCatalog.h"
#include "spdlog/spdlog.h"
#include <optional>

namespace {

std::string ResolveMessage(const std::string& locale, const std::string& key, const std::string& fallbackKey) {
    const std::string path = "messages." + key;
    const std::string fallbackPath = fallbackKey.empty() ? "" : "messages." + fallbackKey;
    if (const auto* text = CustomMessages::GetGameTextCatalog().Find(locale, path, fallbackPath)) {
        return CustomMessages::DecodeGameText(*text);
    }
    SPDLOG_ERROR("Missing game message ({}) for locale ({}); rebuild soh.o2r after updating catalogs", key, locale);
    return "[Missing game message: " + key + "]";
}

std::string ResolveOverride(const std::string& locale, const std::string& key, const std::string& defaultText) {
    if (const auto* text = CustomMessages::GetGameTextCatalog().FindExact(locale, "messages." + key)) {
        return CustomMessages::DecodeGameText(*text);
    }
    return defaultText;
}

std::optional<MessageAlignment> ReadAlignment(const std::string& locale, const std::string& path) {
    const auto* value = CustomMessages::GetGameTextCatalog().FindExact(locale, path);
    if (value == nullptr)
        return std::nullopt;
    if (*value == "auto")
        return MESSAGE_ALIGNMENT_AUTO;
    if (*value == "left")
        return MESSAGE_ALIGNMENT_LEFT;
    if (*value == "center")
        return MESSAGE_ALIGNMENT_CENTER;
    if (*value == "right")
        return MESSAGE_ALIGNMENT_RIGHT;
    SPDLOG_WARN("Invalid game message alignment ({}) at ({}:{}); using auto", *value, locale, path);
    return MESSAGE_ALIGNMENT_AUTO;
}

std::optional<MessageAlignment> FindAlignment(const std::string& locale, std::string key) {
    while (!key.empty()) {
        if (auto alignment = ReadAlignment(locale, "message_layout.messages." + key + ".alignment"))
            return alignment;
        const auto separator = key.rfind('.');
        if (separator == std::string::npos)
            break;
        key.resize(separator);
    }
    return std::nullopt;
}

CustomMessage ApplyAlignment(CustomMessage result, const std::string& key, const std::string& fallbackKey = "") {
    for (const auto& locale : CustomMessages::GetGameTextCatalog().GetLanguages()) {
        auto alignment = FindAlignment(locale, key);
        if (!alignment && !fallbackKey.empty())
            alignment = FindAlignment(locale, fallbackKey);
        if (!alignment && result.GetAlignment(locale) == MESSAGE_ALIGNMENT_AUTO)
            alignment = ReadAlignment(locale, "message_layout.alignment");
        if (alignment)
            result.SetAlignment(locale, *alignment);
    }
    return result;
}

template <typename Message>
Message AddLocales(Message result, const std::string& key, const std::string& fallbackKey = "") {
    for (const auto& locale : CustomMessages::GetGameTextCatalog().GetLanguages()) {
        if (locale != "en_US" && locale != "de_DE" && locale != "fr_FR") {
            result.SetForLocale(locale, ResolveMessage(locale, key, fallbackKey));
        }
    }
    return result;
}

} // namespace

namespace CustomMessages {

void LoadCatalog() {
    Lang::LoadCatalogs(GetGameTextCatalog(), "game-lang");
}

CustomMessage CreateMessage(const std::string& key, TextBoxType type, TextBoxPosition position) {
    auto result = AddLocales(CustomMessage(GetEnglishText(key), ResolveOverride("de_DE", key, TODO_TRANSLATE),
                                           ResolveOverride("fr_FR", key, TODO_TRANSLATE), type, position),
                             key);
    return ApplyAlignment(std::move(result), key);
}

CustomMessage CreateMessage(const std::string& key, std::vector<std::string> colors, std::vector<bool> capital,
                            TextBoxType type, TextBoxPosition position) {
    auto result = AddLocales(CustomMessage(GetEnglishText(key), ResolveOverride("de_DE", key, TODO_TRANSLATE),
                                           ResolveOverride("fr_FR", key, TODO_TRANSLATE), std::move(colors),
                                           std::move(capital), type, position),
                             key);
    return ApplyAlignment(std::move(result), key);
}

Text CreateText(const std::string& key) {
    const auto english = GetEnglishText(key);
    return AddLocales(Text(english, ResolveOverride("fr_FR", key, english), ResolveOverride("de_DE", key, english)),
                      key);
}

std::string GetTextForUI(const std::string& key, const std::string& fallback) {
    if (const auto* text = CustomMessages::GetGameTextCatalog().Find(Lang::GetUILanguage(), "messages." + key)) {
        return text->starts_with(TODO_TRANSLATE) ? fallback : *text;
    }
    return fallback;
}

CustomMessage GetMessage(const std::string& key, TextBoxType type, TextBoxPosition position) {
    return GetMessage(key, "", type, position);
}

CustomMessage GetMessage(const std::string& key, const std::string& fallbackKey, TextBoxType type,
                         TextBoxPosition position) {
    const std::string english = ResolveMessage("en_US", key, fallbackKey);
    const std::string german = ResolveMessage("de_DE", key, fallbackKey);
    const std::string french = ResolveMessage("fr_FR", key, fallbackKey);
    return ApplyAlignment(AddLocales(CustomMessage(english, german, french, type, position), key, fallbackKey), key,
                          fallbackKey);
}

CustomMessage GetMessageOrDefault(const std::string& key, const CustomMessage& defaultMessage) {
    const std::string english = ResolveOverride("en_US", key, defaultMessage.GetEnglish(MF_RAW));
    const std::string german = ResolveOverride("de_DE", key, defaultMessage.GetGerman(MF_RAW));
    const std::string french = ResolveOverride("fr_FR", key, defaultMessage.GetFrench(MF_RAW));
    CustomMessage result = defaultMessage;
    result.SetForLocale("en_US", english);
    result.SetForLocale("de_DE", german);
    result.SetForLocale("fr_FR", french);
    for (const auto& locale : GetGameTextCatalog().GetLanguages()) {
        if (locale != "en_US" && locale != "de_DE" && locale != "fr_FR")
            result.SetForLocale(locale, ResolveOverride(locale, key, defaultMessage.GetForLocale(locale, MF_RAW)));
    }
    return ApplyAlignment(std::move(result), key);
}

} // namespace CustomMessages
