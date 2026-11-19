#include <map>
#include <mutex>
#include <set>
#include <tuple>
#include <cstring>
#include <cctype>
#include <nlohmann/json.hpp>

#include <libultraship/bridge/consolevariablebridge.h>

#include "Lang.h"
#include "BuiltinUICatalog.h"
#include "TranslationCatalog.h"
#include "LanguagePolicy.h"
#include "soh/cvar_prefixes.h"
#include "spdlog/spdlog.h"

namespace {

bool initialized = false;
bool selectionInitialized = false;
bool usingBuiltinCatalog = false;
std::mutex diagnosticsMutex;
std::set<std::pair<std::string, std::string>> reportedMissing;
Lang::TranslationCatalog catalog;
std::map<std::string, std::string> languageNames;

constexpr const char* LANGUAGE_CVAR = CVAR_SETTING("UILanguage");
constexpr const char* LEGACY_LANGUAGE_CVAR = CVAR_SETTING("Language");
constexpr const char* DEFAULT_LANGUAGE = "en_US";
constexpr const char* UI_CATALOG_FOLDER = "ui-lang";
const std::string errorMessage =
    "[ERROR] Couldn't retrieve language data for this item, check the log for more information.";

} // namespace

std::variant<std::reference_wrapper<const std::string>, Lang::Error> Lang::TryTranslate(const char* path) {
    return TryTranslateForLanguage(path, GetUILanguage());
}

std::variant<std::reference_wrapper<const std::string>, Lang::Error>
Lang::TryTranslateForLanguage(const char* path, const char* language) {
    if (!initialized) {
        return Lang::Error::LanguageDataNotInitialized;
    }
    if (!catalog.HasLanguage(DEFAULT_LANGUAGE)) {
        return Lang::Error::LanguageDataNotFound;
    }
    if (path == nullptr || language == nullptr) {
        return Lang::Error::PathInvalidValue;
    }
    if (const auto* text = catalog.Find(language, path)) {
        return std::cref(*text);
    }
    return Lang::Error::PathDataNotFound;
}

const std::string& Lang::Translate(const char* path) {
    return TranslateForLanguage(path, GetUILanguage());
}

const std::string& Lang::TranslateForLanguage(const char* path, const char* language) {
    auto value = TryTranslateForLanguage(path, language);
    if (std::holds_alternative<std::reference_wrapper<const std::string>>(value)) {
        return std::get<std::reference_wrapper<const std::string>>(value);
    }
    // A missing translation may be requested once per frame. Report each pair
    // once, without making optional TryTranslate lookups produce any I/O.
    const std::lock_guard<std::mutex> lock(diagnosticsMutex);
    if (reportedMissing.emplace(language ? language : "<null>", path ? path : "<null>").second) {
        SPDLOG_WARN("Couldn't retrieve language data for locale ({}) at path ({})", language ? language : "<null>",
                    path ? path : "<null>");
    }
    return errorMessage;
}

void Lang::PrepareUICatalogs() {
    if (initialized) {
        return;
    }
    catalog.AddLanguage(DEFAULT_LANGUAGE, GetBuiltinUICatalog());
    const auto result = LoadCatalogs(catalog, UI_CATALOG_FOLDER);
    usingBuiltinCatalog = !result.hasEnglish;
    for (const auto& language : catalog.GetLanguages()) {
        if (const auto* name = catalog.FindExact(language, "language_name")) {
            languageNames.emplace(language, *name);
        }
    }
    initialized = true;
}

bool Lang::IsUsingBuiltinUICatalog() {
    return usingBuiltinCatalog;
}

void Lang::LoadLangs() {
    PrepareUICatalogs();
    if (selectionInitialized) {
        return;
    }
    selectionInitialized = true;
    // Initialize() has installed the application's file logger by this point.
    if (usingBuiltinCatalog) {
        SPDLOG_WARN("No valid ui-lang/en_US.json in mounted archives; using built-in English UI. "
                    "Rebuild GenerateSohOtr and deploy the matching soh.o2r beside the executable.");
    }
    const std::string configured = CVarGetString(LANGUAGE_CVAR, "");
    const std::string validLanguage =
        catalog.ResolveLanguage(configured, CVarGetString(LEGACY_LANGUAGE_CVAR, DEFAULT_LANGUAGE));
    if (configured != validLanguage && !usingBuiltinCatalog) {
        CVarSetString(LANGUAGE_CVAR, validLanguage.c_str());
        CVarSave();
    }
}

std::string Lang::GetUIFontText() {
    PrepareUICatalogs();
    return catalog.GetFontText();
}

const char* Lang::GetUILanguage() {
    if (!initialized) {
        return DEFAULT_LANGUAGE;
    }
    const char* language = CVarGetString(LANGUAGE_CVAR, DEFAULT_LANGUAGE);
    return catalog.HasLanguage(language) ? language : DEFAULT_LANGUAGE;
}

uint64_t Lang::GetUICatalogRevision() {
    return catalog.GetRevision();
}

const std::string* Lang::FindExactUITranslation(std::string_view path) {
    return initialized ? catalog.FindExact(GetUILanguage(), path) : nullptr;
}

bool Lang::SetUILanguage(const std::string& language) {
    if (!initialized || !catalog.HasLanguage(language)) {
        return false;
    }
    if (language != GetUILanguage()) {
        CVarSetString(LANGUAGE_CVAR, language.c_str());
        CVarSave();
    }
    return true;
}

const std::map<std::string, std::string>& Lang::GetAvailableUILanguages() {
    return languageNames;
}

uint8_t Lang::GetUIResourceLanguage() {
    return LanguagePolicy_FromLocale(GetUILanguage());
}

uint8_t Lang::GetUILegacyTextLanguage() {
    return LanguagePolicy_LegacyText(GetUIResourceLanguage());
}

std::string Lang::TranslateOrFallback(const std::string& path, const std::string& fallback) {
    const auto result = TryTranslate(path.c_str());
    if (const auto* text = std::get_if<std::reference_wrapper<const std::string>>(&result)) {
        return text->get();
    }
    return fallback;
}

namespace {
enum class UIStringKind { Text, Label, Format, BraceFormat };
struct UIStringKeyLess {
    using is_transparent = void;
    template <typename A, typename B> bool operator()(const A& a, const B& b) const {
        return std::tuple{ std::get<0>(a), std::string_view(std::get<1>(a)), std::string_view(std::get<2>(a)) } <
               std::tuple{ std::get<0>(b), std::string_view(std::get<1>(b)), std::string_view(std::get<2>(b)) };
    }
};
struct VariantKeyLess {
    using is_transparent = void;
    template <typename A, typename B> bool operator()(const A& a, const B& b) const {
        return std::pair{ std::string_view(a.first), std::string_view(a.second) } <
               std::pair{ std::string_view(b.first), std::string_view(b.second) };
    }
};

bool PrintfSignature(std::string_view format, std::string& signature) {
    for (size_t i = 0; i < format.size(); ++i) {
        if (format[i] != '%')
            continue;
        if (++i == format.size())
            return false;
        if (format[i] == '%')
            continue;
        while (i < format.size() && std::strchr("-+ #0", format[i]))
            ++i;
        if (i < format.size() && format[i] == '*') {
            signature += "*;";
            ++i;
        }
        while (i < format.size() && std::isdigit(static_cast<unsigned char>(format[i])))
            ++i;
        if (i < format.size() && format[i] == '.') {
            ++i;
            if (i < format.size() && format[i] == '*') {
                signature += "*;";
                ++i;
            }
            while (i < format.size() && std::isdigit(static_cast<unsigned char>(format[i])))
                ++i;
        }
        if (i < format.size() && std::strchr("hljztL", format[i])) {
            const char length = format[i++];
            signature += length;
            if (i < format.size() && (length == 'h' || length == 'l') && format[i] == length)
                signature += format[i++];
        }
        if (i == format.size() || !std::strchr("diuoxXfFeEgGaAcsp", format[i]))
            return false;
        signature += format[i];
        signature += ';';
    }
    return true;
}

bool BraceSignature(std::string_view text, std::string& signature) {
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '{' || text[i] == '}') {
            if (i + 1 < text.size() && text[i + 1] == text[i]) {
                ++i;
                continue;
            }
            if (text[i] == '}')
                return false;
            const auto end = text.find('}', i + 1);
            if (end == std::string_view::npos || text.substr(i + 1, end - i - 1).find('{') != std::string_view::npos)
                return false;
            signature.append(text.substr(i, end - i + 1));
            i = end;
        }
    }
    return true;
}

const char* DisplayString(UIStringKind kind, const char* path, const char* fallback) {
    struct Cache {
        uint64_t revision = 0;
        std::string locale;
        std::map<std::tuple<UIStringKind, std::string, std::string>, std::string, UIStringKeyLess> strings;
    };
    static Cache cache;
    const std::string_view locale = Lang::GetUILanguage();
    const auto revision = Lang::GetUICatalogRevision();
    if (cache.locale != locale || cache.revision != revision) {
        cache.strings.clear();
        cache.locale = locale;
        cache.revision = revision;
    }
    const std::string_view original = fallback ? fallback : "";
    const auto key = std::make_tuple(kind, std::string_view(path ? path : ""), original);
    auto found = cache.strings.find(key);
    if (found != cache.strings.end())
        return found->second.c_str();
    const auto separator = original.find("##");
    const std::string visible(kind == UIStringKind::Label ? original.substr(0, separator) : original);
    std::string translated = Lang::TranslateOrFallback(path ? path : "", visible);
    // Catalog entries omit Font Awesome icons; retain an authored icon prefix.
    size_t prefix = 0;
    while (prefix + 2 < visible.size()) {
        const auto* p = reinterpret_cast<const unsigned char*>(visible.data() + prefix);
        const uint32_t scalar = ((p[0] & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F);
        if (p[0] < 0xE0 || p[0] > 0xEF || scalar < 0xE000 || scalar > 0xF8FF)
            break;
        prefix += 3;
        while (prefix < visible.size() && visible[prefix] == ' ')
            ++prefix;
    }
    if (prefix && translated != visible && !translated.starts_with(visible.substr(0, prefix)))
        translated.insert(0, visible.substr(0, prefix));
    if (kind == UIStringKind::Format || kind == UIStringKind::Label) {
        std::string sourceSignature, translatedSignature;
        const bool sourceValid = PrintfSignature(visible, sourceSignature);
        if ((kind == UIStringKind::Format || (sourceValid && !sourceSignature.empty())) &&
            (!sourceValid || !PrintfSignature(translated, translatedSignature) ||
             sourceSignature != translatedSignature))
            translated = visible;
        if (kind == UIStringKind::Format && !sourceValid) {
            // Constant option captions such as "50%" are display data. Escape
            // percent signs rather than passing a malformed format to ImGui.
            translated.clear();
            for (const char c : visible) {
                translated += c;
                if (c == '%')
                    translated += '%';
            }
        }
    }
    if (kind == UIStringKind::BraceFormat) {
        std::string sourceSignature, translatedSignature;
        if (!BraceSignature(visible, sourceSignature) || !BraceSignature(translated, translatedSignature) ||
            sourceSignature != translatedSignature)
            translated = visible;
    }
    if (kind == UIStringKind::Label && !visible.empty()) {
        // ### anchors the ID to canonical text, including any ## suffix.
        // An existing ### suffix already defines the canonical identifier.
        const auto stable = original.find("###");
        translated += "###";
        translated.append(stable == std::string::npos ? original : original.substr(stable + 3));
    } else if (kind == UIStringKind::Label) {
        translated = original; // Hidden IDs have no translatable caption.
    }
    return cache.strings
        .emplace(std::make_tuple(kind, std::string(path ? path : ""), std::string(original)), std::move(translated))
        .first->second.c_str();
}
} // namespace

const char* Lang::UIString(const char* path, const char* fallback) {
    return DisplayString(UIStringKind::Text, path, fallback);
}

const char* Lang::UILabel(const char* path, const char* fallback) {
    return DisplayString(UIStringKind::Label, path, fallback);
}

const char* Lang::UIFormat(const char* path, const char* fallback) {
    return DisplayString(UIStringKind::Format, path, fallback);
}

const char* Lang::UIBraceFormat(const char* path, const char* fallback) {
    return DisplayString(UIStringKind::BraceFormat, path, fallback);
}

namespace {
const std::string& VariantKey(std::string_view prefix, const char* fallback, bool label = false) {
    static uint64_t revision = 0;
    static std::map<std::pair<std::string, std::string>, std::string, VariantKeyLess> keys;
    if (revision != Lang::GetUICatalogRevision()) {
        keys.clear();
        revision = Lang::GetUICatalogRevision();
    }
    const std::string_view text = fallback ? fallback : "";
    const auto visible = label ? text.substr(0, text.find("##")) : text;
    const auto key = std::make_pair(std::string_view(prefix), visible);
    auto found = keys.find(key);
    if (found == keys.end())
        found = keys.emplace(std::make_pair(std::string(prefix), std::string(visible)),
                             catalog.FindEnglishKey(prefix, visible))
                    .first;
    return found->second;
}
} // namespace
const char* Lang::UIVariant(std::string_view prefix, const char* fallback) {
    return UIString(VariantKey(prefix, fallback).c_str(), fallback);
}
const char* Lang::UILabelVariant(std::string_view prefix, const char* fallback) {
    return UILabel(VariantKey(prefix, fallback, true).c_str(), fallback);
}
const char* Lang::UIFormatVariant(std::string_view prefix, const char* fallback) {
    return UIFormat(VariantKey(prefix, fallback).c_str(), fallback);
}
