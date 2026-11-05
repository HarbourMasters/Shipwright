#include "soh/Enhancements/Lang/Lang.h"
#include "CustomMessageManager.h"
#include "soh/Enhancements/Lang/GameLanguage.h"
#include "soh/Enhancements/Lang/MessageLanguage.h"
#include "CustomMessageInterfaceAddon.h"
#include "UnicodeMessage.h"
#include <algorithm>
#include <stdint.h>
#include <cstring>
#include <map>
#include <spdlog/spdlog.h>
#include <variables.h>
#include <soh/Enhancements/gameconsole.h>
#include <soh/util.h>

using namespace std::literals::string_literals;

static const std::unordered_map<std::string, char> textBoxSpecialCharacters = {
    { "À", 0x80 }, { "î", 0x81 }, { "Â", 0x82 }, { "Ä", 0x83 }, { "Ç", 0x84 }, { "È", 0x85 }, { "É", 0x86 },
    { "Ê", 0x87 }, { "Ë", 0x88 }, { "Ï", 0x89 }, { "Ô", 0x8A }, { "Ö", 0x8B }, { "Ù", 0x8C }, { "Û", 0x8D },
    { "Ü", 0x8E }, { "ß", 0x8F }, { "à", 0x90 }, { "á", 0x91 }, { "â", 0x92 }, { "ä", 0x93 }, { "ç", 0x94 },
    { "è", 0x95 }, { "é", 0x96 }, { "ê", 0x97 }, { "ë", 0x98 }, { "ï", 0x99 }, { "ô", 0x9A }, { "ö", 0x9B },
    { "ù", 0x9C }, { "û", 0x9D }, { "ü", 0x9E }
};
static const std::unordered_map<std::string, std::string> percentColors = {
    { "w", QM_WHITE }, { "r", QM_RED },  { "g", QM_GREEN },  { "b", QM_BLUE },
    { "c", QM_LBLUE }, { "p", QM_PINK }, { "y", QM_YELLOW }, { "B", QM_BLACK },
};

static const std::unordered_map<std::string, std::string> colorToPercent = {
    { QM_WHITE, "%w" }, { QM_RED, "%r" },  { QM_GREEN, "%g" },  { QM_BLUE, "%b" },
    { QM_LBLUE, "%c" }, { QM_PINK, "%p" }, { QM_YELLOW, "%y" }, { QM_BLACK, "%B" },
};

static const std::unordered_map<std::string, ItemID> altarIcons = {
    { "0", ITEM_KOKIRI_EMERALD },  { "1", ITEM_GORON_RUBY },       { "2", ITEM_ZORA_SAPPHIRE },
    { "8", ITEM_MEDALLION_LIGHT }, { "3", ITEM_MEDALLION_FOREST }, { "4", ITEM_MEDALLION_FIRE },
    { "5", ITEM_MEDALLION_WATER }, { "6", ITEM_MEDALLION_SPIRIT }, { "7", ITEM_MEDALLION_SHADOW },
    { "l", ITEM_ARROW_LIGHT },     { "b", ITEM_KEY_BOSS },         { "o", ITEM_SWORD_MASTER },
    { "c", ITEM_OCARINA_FAIRY },   { "i", ITEM_OCARINA_TIME },     { "L", ITEM_BOW_ARROW_LIGHT },
    { "k", ITEM_TUNIC_KOKIRI },    { "m", ITEM_DUNGEON_MAP },      { "C", ITEM_COMPASS },
    { "s", ITEM_SKULL_TOKEN },     { "g", ITEM_MASK_GORON },       { "w", ITEM_CUSTOM },
};

static std::map<std::string, int> pixelWidthTable = {
    { " ", 6 },  { "!", 6 },  { "\"", 5 },     { "#", 7 },  { "$", 7 },  { "%", 11 }, { "&", 9 },  { "\'", 3 },
    { "(", 6 },  { ")", 6 },  { "*", 6 },      { "+", 7 },  { ",", 3 },  { "-", 5 },  { ".", 3 },  { "/", 7 },
    { "0", 8 },  { "1", 4 },  { "2", 7 },      { "3", 7 },  { "4", 8 },  { "5", 7 },  { "6", 7 },  { "7", 7 },
    { "8", 7 },  { "9", 7 },  { ":", 5 },      { ";", 5 },  { "<", 7 },  { "=", 9 },  { ">", 7 },  { "?", 9 },
    { "@", 10 }, { "A", 9 },  { "B", 7 },      { "C", 9 },  { "D", 9 },  { "E", 6 },  { "F", 6 },  { "G", 9 },
    { "H", 8 },  { "I", 3 },  { "J", 6 },      { "K", 8 },  { "L", 6 },  { "M", 10 }, { "N", 9 },  { "O", 10 },
    { "P", 7 },  { "Q", 10 }, { "R", 8 },      { "S", 8 },  { "T", 7 },  { "U", 8 },  { "V", 9 },  { "W", 12 },
    { "X", 9 },  { "Y", 8 },  { "Z", 8 },      { "[", 6 },  { "\\", 8 }, { "]", 6 },  { "^", 8 },  { "_", 7 },
    { "`", 4 },  { "a", 6 },  { "b", 7 },      { "c", 6 },  { "d", 7 },  { "e", 7 },  { "f", 5 },  { "g", 7 },
    { "h", 6 },  { "i", 3 },  { "j", 5 },      { "k", 6 },  { "l", 3 },  { "m", 9 },  { "n", 7 },  { "o", 7 },
    { "p", 7 },  { "q", 7 },  { "r", 6 },      { "s", 6 },  { "t", 6 },  { "u", 6 },  { "v", 7 },  { "w", 9 },
    { "x", 6 },  { "y", 7 },  { "z", 6 },      { "{", 6 },  { "¦", 4 },  { "}", 6 },  { "¡", 5 },  { "¢", 7 },
    { "£", 8 },  { "¤", 7 },  { "¥", 8 },      { "|", 4 },  { "§", 12 }, { "¨", 12 }, { "©", 10 }, { "ª", 5 },
    { "«", 8 },  { "¬", 7 },  { "\u00AD", 6 }, { "®", 10 }, { "¯", 8 },  { "°", 12 }, { "±", 12 }, { "²", 5 },
    { "³", 5 },  { "µ", 6 },  { "¶", 8 },      { "·", 4 },  { "¹", 4 },  { "º", 5 },  { "»", 9 },  { "¼", 9 },
    { "½", 9 },  { "¾", 10 }, { "¿", 7 },      { "À", 11 }, { "Á", 9 },  { "Â", 9 },  { "Ã", 9 },  { "Ä", 9 },
    { "Å", 9 },  { "Æ", 12 }, { "Ç", 9 },      { "È", 6 },  { "É", 6 },  { "Ê", 6 },  { "Ë", 6 },  { "Ì", 5 },
    { "Í", 5 },  { "Î", 5 },  { "Ï", 5 },      { "Ð", 10 }, { "Ñ", 9 },  { "Ò", 10 }, { "Ó", 10 }, { "Ô", 10 },
    { "Õ", 10 }, { "Ö", 10 }, { "×", 9 },      { "Ø", 10 }, { "Ù", 8 },  { "Ú", 8 },  { "Û", 8 },  { "Ü", 8 },
    { "Ý", 10 }, { "Þ", 8 },  { "ß", 7 },      { "à", 6 },  { "á", 6 },  { "â", 6 },  { "ã", 6 },  { "ä", 6 },
    { "å", 6 },  { "æ", 11 }, { "ç", 6 },      { "è", 7 },  { "é", 7 },  { "ê", 7 },  { "ë", 7 },  { "ì", 5 },
    { "í", 5 },  { "î", 5 },  { "ï", 5 },      { "ð", 7 },  { "ñ", 7 },  { "ò", 7 },  { "ó", 7 },  { "ô", 7 },
    { "õ", 7 },  { "ö", 7 },  { "÷", 11 },     { "ø", 9 },  { "ù", 7 },  { "ú", 7 },  { "û", 7 },  { "ü", 7 },
    { "ý", 8 },  { "þ", 8 },  { "ÿ", 8 },      { "Œ", 11 }, { "œ", 11 }, { "„", 5 },  { "”", 5 },  { "€", 10 },
    { "Ÿ", 10 }, { "~", 8 },
};

CustomMessage::CustomMessage(std::string english_, std::string german_, std::string french_, TextBoxType type_,
                             TextBoxPosition position_)
    : type(type_), position(position_) {
    messages[LANGUAGE_ENG] = std::move(english_);
    messages[LANGUAGE_GER] = std::move(german_);
    messages[LANGUAGE_FRA] = std::move(french_);
}

CustomMessage::CustomMessage(std::string english_, std::string german_, std::string french_,
                             std::vector<std::string> colors_, std::vector<bool> capital_, TextBoxType type_,
                             TextBoxPosition position_) {
    messages[LANGUAGE_ENG] = std::move(english_);
    messages[LANGUAGE_GER] = std::move(german_);
    messages[LANGUAGE_FRA] = std::move(french_);
    colors = std::move(colors_);
    capital = std::move(capital_);
    type = type_;
    position = position_;
}

CustomMessage::CustomMessage(std::string english_, TextBoxType type_, TextBoxPosition position_)
    : type(type_), position(position_) {
    messages[LANGUAGE_ENG] = std::move(english_);
}

CustomMessage::CustomMessage(std::string english_, std::vector<std::string> colors_, std::vector<bool> capital_,
                             TextBoxType type_, TextBoxPosition position_) {
    messages[LANGUAGE_ENG] = std::move(english_);
    colors = std::move(colors_);
    capital = std::move(capital_);
    type = type_;
    position = position_;
}

CustomMessage::CustomMessage(Text text, TextBoxType type_, TextBoxPosition position_)
    : type(type_), position(position_) {
    messages[LANGUAGE_ENG] = text.GetEnglish();
    messages[LANGUAGE_GER] = text.GetGerman();
    messages[LANGUAGE_FRA] = text.GetFrench();
    locales = text.GetLocales();
}

typedef struct {
    u16 textId;
    u8 typePos;
    const char* segment;
    u32 msgSize;
} MessageTableEntry;

extern "C" MessageTableEntry* sNesMessageEntryTablePtr;
extern "C" MessageTableEntry* sGerMessageEntryTablePtr;
extern "C" MessageTableEntry* sFraMessageEntryTablePtr;

CustomMessage CustomMessage::LoadVanillaMessageTableEntry(uint16_t textId) {
    MessageTableEntry* msgEntry = sNesMessageEntryTablePtr;
    u16 bufferId = textId;
    CustomMessage msg;
    if (GameLanguage_GetLegacyText() == LANGUAGE_GER) {
        msgEntry = sGerMessageEntryTablePtr;
    } else if (GameLanguage_GetLegacyText() == LANGUAGE_FRA) {
        msgEntry = sFraMessageEntryTablePtr;
    }
    while (msgEntry->textId != 0xFFFF) {
        if (msgEntry->textId == bufferId) {
            TextBoxPosition position = static_cast<TextBoxPosition>(msgEntry->typePos & 0xF);
            TextBoxType type = static_cast<TextBoxType>(msgEntry->typePos >> 4);
            // uint8_t icon = msgEntry->segment[1];
            std::string message = std::string(msgEntry->segment, msgEntry->msgSize);
            msg = CustomMessage(message, type, position);
            // msg.Format(static_cast<ItemID>(icon));
            return msg;
        }
        msgEntry++;
    }
    return CustomMessage();
}

const std::string CustomMessage::GetEnglish(MessageFormat format) const {
    return GetForLanguage(LANGUAGE_ENG, format);
}

const std::string CustomMessage::GetGerman(MessageFormat format) const {
    return GetForLanguage(LANGUAGE_GER, format);
}

const std::string CustomMessage::GetFrench(MessageFormat format) const {
    return GetForLanguage(LANGUAGE_FRA, format);
}

const std::string CustomMessage::GetForCurrentLanguage(MessageFormat format) const {
    return GetForLocale(GameLanguage_GetMessageLocale(), format);
}

const std::string CustomMessage::GetForUILanguage(MessageFormat format) const {
    return GetForLocale(Lang::GetUILanguage(), format);
}

const std::string CustomMessage::GetForLanguage(uint8_t language, MessageFormat format) const {
    if (language == LANGUAGE_JPN)
        return GetForLocale("ja_JP", format);
    const uint8_t slot = LanguagePolicy_LegacyText(language);
    std::string output = !messages[slot].starts_with(TODO_TRANSLATE) ? messages[slot] : messages[LANGUAGE_ENG];
    ProcessMessageFormat(output, format);
    return output;
}

void CustomMessage::SetForLocale(const std::string& locale, std::string text) {
    if (locale == "en_US")
        messages[LANGUAGE_ENG] = std::move(text);
    else if (locale == "de_DE")
        messages[LANGUAGE_GER] = std::move(text);
    else if (locale == "fr_FR")
        messages[LANGUAGE_FRA] = std::move(text);
    else
        locales[locale] = std::move(text);
}
const std::map<std::string, std::string>& CustomMessage::GetLocales() const {
    return locales;
}
void CustomMessage::SetAlignment(const std::string& locale, MessageAlignment alignment) {
    if (alignment == MESSAGE_ALIGNMENT_AUTO)
        alignments.erase(locale);
    else
        alignments[locale] = alignment;
}
MessageAlignment CustomMessage::GetAlignment(const std::string& locale) const {
    const auto found = alignments.find(locale);
    return found == alignments.end() ? MESSAGE_ALIGNMENT_AUTO : found->second;
}
bool CustomMessage::HasLocale(const std::string& locale) const {
    if (locale == "en_US")
        return !messages[LANGUAGE_ENG].starts_with(TODO_TRANSLATE);
    if (locale == "de_DE")
        return !messages[LANGUAGE_GER].starts_with(TODO_TRANSLATE);
    if (locale == "fr_FR")
        return !messages[LANGUAGE_FRA].starts_with(TODO_TRANSLATE);
    const auto found = locales.find(locale);
    return found != locales.end() && !found->second.starts_with(TODO_TRANSLATE);
}
std::string CustomMessage::GetForLocale(const std::string& locale, MessageFormat format) const {
    if (locale == "en_US")
        return GetEnglish(format);
    if (locale == "de_DE")
        return GetGerman(format);
    if (locale == "fr_FR")
        return GetFrench(format);
    std::string result = HasLocale(locale) ? locales.at(locale) : messages[LANGUAGE_ENG];
    if (format == MF_CLEAN)
        CleanString(result);
    else if (format == MF_ENCODE)
        EncodeColors(result);
    else if (format != MF_RAW)
        FormatUnicode(result, locale);
    return result;
}
std::vector<std::reference_wrapper<std::string>> CustomMessage::AllMessages() {
    std::vector<std::reference_wrapper<std::string>> result;
    for (auto& text : messages)
        result.emplace_back(text);
    for (auto& [locale, text] : locales)
        result.emplace_back(text);
    return result;
}
void CustomMessage::IncludeLocales(const CustomMessage& other) {
    for (const auto& [locale, text] : other.locales)
        if (!locales.contains(locale))
            locales[locale] = messages[LANGUAGE_ENG];
}
void CustomMessage::FormatUnicode(std::string& text, const std::string& locale, std::optional<uint8_t> itemIcon) const {
    ReplaceColors(text);
    ReplaceAltarIcons(text);
    text = UnicodeMessage::Format(text, locale == "ja_JP", itemIcon);
}

const std::vector<std::string> CustomMessage::GetAllMessages(MessageFormat format) const {
    std::vector<std::string> output = messages;
    for (auto& str : output) {
        ProcessMessageFormat(str, format);
    }
    return output;
}

void CustomMessage::ProcessMessageFormat(std::string& str, MessageFormat format) const {
    if (format == MF_FORMATTED) {
        FormatString(str);
    } else if (format == MF_CLEAN) {
        CleanString(str);
    } else if (format == MF_AUTO_FORMAT) {
        AutoFormatString(str);
    } else if (format == MF_ENCODE) {
        EncodeColors(str);
    }
}

const std::vector<bool>& CustomMessage::GetCapital() const {
    return capital;
}

void CustomMessage::SetCapital(std::vector<bool> capital_) {
    capital = std::move(capital_);
}
const std::vector<std::string>& CustomMessage::GetColors() const {
    return colors;
}

void CustomMessage::SetColors(std::vector<std::string> colors_) {
    colors = std::move(colors_);
}

const TextBoxType& CustomMessage::GetTextBoxType() const {
    return type;
}

void CustomMessage::SetTextBoxType(TextBoxType boxType) {
    type = boxType;
}

const TextBoxPosition& CustomMessage::GetTextBoxPosition() const {
    return position;
}

void CustomMessage::SetTextBoxPosition(TextBoxPosition boxPos) {
    position = boxPos;
}

CustomMessage CustomMessage::operator+(const CustomMessage& right) const {
    CustomMessage result = *this;
    result += right;
    return result;
}

CustomMessage CustomMessage::operator+(const std::string& right) const {
    CustomMessage result = *this;
    result += right;
    return result;
}

void CustomMessage::operator+=(const CustomMessage& right) {
    IncludeLocales(right);
    for (auto& [locale, text] : locales)
        text += right.GetForLocale(locale, MF_RAW);
    messages[LANGUAGE_ENG] += right.GetEnglish(MF_RAW);
    messages[LANGUAGE_GER] += right.GetGerman(MF_RAW);
    messages[LANGUAGE_FRA] += right.GetFrench(MF_RAW);
    SohUtils::AppendVector(colors, right.GetColors());
    SohUtils::AppendVector(capital, right.GetCapital());
}

void CustomMessage::operator+=(const std::string& right) {
    for (auto& [locale, text] : locales)
        text += right;
    messages[LANGUAGE_ENG] += right;
    messages[LANGUAGE_GER] += right;
    messages[LANGUAGE_FRA] += right;
}

bool CustomMessage::operator==(const CustomMessage& operand) const {
    return messages[LANGUAGE_ENG] == operand.messages[LANGUAGE_ENG];
}

bool CustomMessage::operator==(const std::string& operand) const {
    for (const auto& str : messages) {
        if (str == operand) {
            return true;
        }
    }
    return false;
}

bool CustomMessage::operator!=(const CustomMessage& operand) const {
    return !operator==(operand);
}

void CustomMessage::LoadIntoFont() {
    LoadIntoFont(GameLanguage_GetMessageLocale());
}

void CustomMessage::LoadIntoFont(const std::string& locale) {
    MessageContext* msgCtx = &gPlayState->msgCtx;
    Font* font = &msgCtx->font;
    char* buffer = font->msgBuf;
    const size_t maxBufferSize = sizeof(font->msgBuf);
    font->charTexBuf[0] = (type << 4) | position;
    bool unicode = (locale != "en_US" && locale != "de_DE" && locale != "fr_FR" && HasLocale(locale)) ||
                   UnicodeMessage::RequiresUnicode(GetForLocale(locale, MF_RAW));
    uint8_t language = unicode ? LanguagePolicy_FromLocale(locale.c_str())
                               : LanguagePolicy_LegacyText(LanguagePolicy_FromLocale(locale.c_str()));
    std::string text = unicode ? GetForLocale(locale, MF_FORMATTED) : GetForLanguage(language, MF_RAW);
    if (unicode && text.size() > maxBufferSize) {
        // Never cut off choice labels or half of a control code in an oversized translation.
        SPDLOG_WARN("Custom message ({}) exceeds native buffer; using the complete legacy message", locale);
        unicode = false;
        language = LanguagePolicy_LegacyText(LanguagePolicy_FromLocale(locale.c_str()));
        text = GetForLanguage(language, MF_RAW);
    }
    if (unicode) {
        MessageLanguage_SetUnicode(language);
        auto alignment = GetAlignment(locale);
        if (alignment == MESSAGE_ALIGNMENT_AUTO) {
            alignment = MESSAGE_ALIGNMENT_CENTER;
            // Inspect the complete formatted message once, skipping operands.
            // Choice prompts stay left-aligned even on preceding pages.
            for (size_t cursor = 0; cursor < text.size();) {
                const auto code = static_cast<unsigned char>(text[cursor]);
                if (code == 0x1B || code == 0x1C) {
                    alignment = MESSAGE_ALIGNMENT_LEFT;
                    break;
                }
                cursor += code < 0x20 ? UnicodeMessage::ControlSize(code) : 1;
            }
        }
        MessageLanguage_SetAlignment(alignment);
    } else {
        MessageLanguage_Set(language);
    }
    const size_t length = SohUtils::CopyStringToCharBuffer(buffer, text, maxBufferSize);
    font->msgLength = static_cast<u32>(length);
    msgCtx->msgLength = static_cast<s32>(length);
}

void CustomMessage::Replace(std::string&& oldStr, std::string&& newStr) {
    for (std::string& str : AllMessages()) {
        size_t position = str.find(oldStr);
        while (position != std::string::npos) {
            str.replace(position, oldStr.length(), newStr);
            position = str.find(oldStr, position + 1);
        }
    }
}

void CustomMessage::Replace(std::string&& oldStr, CustomMessage newMessage) {
    IncludeLocales(newMessage);
    for (uint8_t language = 0; language < LANGUAGE_MAX - 1; language++) {
        size_t position = messages[language].find(oldStr);
        std::string newMsg = newMessage.messages[language];
        if (language != LANGUAGE_ENG && newMsg == TODO_TRANSLATE) {
            newMsg = newMessage.messages[LANGUAGE_ENG];
        }
        while (position != std::string::npos) {
            messages[language].replace(position, oldStr.length(), newMsg);
            position = messages[language].find(oldStr);
        }
    }
    for (auto& [locale, text] : locales) {
        const std::string replacement = newMessage.GetForLocale(locale, MF_RAW);
        size_t cursor = 0;
        while ((cursor = text.find(oldStr, cursor)) != std::string::npos) {
            text.replace(cursor, oldStr.size(), replacement);
            cursor += replacement.size();
        }
    }
}

void CustomMessage::Format(ItemID iid) {
    for (std::string& str : messages) {
        str.insert(0, ITEM_OBTAINED(iid));
        size_t start_pos = 0;
        std::replace(str.begin(), str.end(), '&', NEWLINE()[0]);
        while ((start_pos = str.find('^', start_pos)) != std::string::npos) {
            str.replace(start_pos, 1, WAIT_FOR_INPUT() + ITEM_OBTAINED(iid));
            start_pos += 3;
        }
        std::replace(str.begin(), str.end(), '@', PLAYER_NAME()[0]);
        ReplaceSpecialCharacters(str);
        ReplaceColors(str);
        ReplaceAltarIcons(str);
    }
    for (auto& [locale, text] : locales) {
        FormatUnicode(text, locale, static_cast<uint8_t>(iid));
    }
    for (auto& text : messages)
        text += MESSAGE_END();
}

void CustomMessage::Format() {
    for (std::string& str : messages) {
        FormatString(str);
    }
    for (auto& [locale, text] : locales)
        FormatUnicode(text, locale);
}

void CustomMessage::AutoFormat() {
    for (std::string& str : messages) {
        AutoFormatString(str);
    }
    for (auto& [locale, text] : locales)
        FormatUnicode(text, locale);
}

void CustomMessage::AutoFormat(ItemID iid) {
    const auto icon = ITEM_OBTAINED(iid);
    for (std::string& str : messages) {
        str.insert(0, icon);
        AutoFormatString(str);
        // Legacy pages still get their icon here, but a control operand whose
        // value is 0x04/0x0C is not a page break (including the icon ID itself).
        for (size_t cursor = 0; cursor < str.size();) {
            const auto code = static_cast<unsigned char>(str[cursor]);
            cursor += code < 0x20 ? UnicodeMessage::ControlSize(code) : 1;
            if (code == 0x04 || code == 0x0C) {
                str.insert(cursor, icon);
                cursor += icon.size();
            }
        }
    }
    // The Unicode formatter owns both authored and automatic continuations.
    // Do not insert icons again after it has already repeated them.
    for (auto& [locale, text] : locales)
        FormatUnicode(text, locale, static_cast<uint8_t>(iid));
}

void CustomMessage::Clean() {
    for (std::string& str : AllMessages()) {
        CleanString(str);
    }
}

void CustomMessage::Encode() {
    for (std::string& str : AllMessages()) {
        EncodeColors(str);
    }
}

void CustomMessage::FormatString(std::string& str) const {
    std::replace(str.begin(), str.end(), '&', NEWLINE()[0]);
    std::replace(str.begin(), str.end(), '^', WAIT_FOR_INPUT()[0]);
    std::replace(str.begin(), str.end(), '@', PLAYER_NAME()[0]);
    ReplaceSpecialCharacters(str);
    ReplaceColors(str);
    ReplaceAltarIcons(str);
    str += MESSAGE_END();
}

void DeleteControlCode(std::string& str, std::string code) {
    size_t start_pos = 0;
    while ((start_pos = str.find(code, start_pos)) != std::string::npos) {
        str.replace(start_pos, code.length(), "");
        start_pos += code.length();
    }
}

void CustomMessage::CleanString(std::string& str) const {
    std::replace(str.begin(), str.end(), '&', "\n"[0]);
    std::replace(str.begin(), str.end(), '^', " "[0]);
    for (const auto& colorPair : percentColors) {
        DeleteControlCode(str, "%" + colorPair.first);
    }
    std::erase(str, '#');
    for (const auto& iconPair : altarIcons) {
        DeleteControlCode(str, "$" + iconPair.first);
    }
}

static size_t NextLineLength(const std::string* textStr, const size_t lastNewline, bool hasIcon = false) {
    const size_t maxLinePixelWidth = hasIcon ? 200 : 216;

    size_t totalPixelWidth = 0;
    size_t currentPos = lastNewline;

    // Looping through the string from the lastNewline until the total
    // width of counted characters exceeds the maximum pixels in a line.
    size_t nextPosJump = 0;
    while (totalPixelWidth < maxLinePixelWidth && currentPos < textStr->length()) {
        // Skip over control codes
        if (textStr->at(currentPos) == '%') {
            nextPosJump = 2;
        } else if (textStr->at(currentPos) == '\x13') {
            nextPosJump = 2;
        } else if (textStr->at(currentPos) == '@') {
            nextPosJump = 1;
            // Assume worst case for player name 12 * 8 (widest character * longest name length)
            totalPixelWidth += 96;
        } else if (textStr->at(currentPos) == '\x05') {
            // Skip colour control characters.
            nextPosJump = 2;
        } else if (textStr->at(currentPos) == '\x1E') {
            // For the high score char, we have to take the next Char, then use that to get a worst case scenario.
            if (textStr->at(currentPos + 1) == '\x01') {
                totalPixelWidth += 28;
            }
            nextPosJump = 2;
        } else {
            // Some characters only one byte while others are two bytes
            // So check both possibilities when checking for a character
            if (pixelWidthTable.count(textStr->substr(currentPos, 1))) {
                totalPixelWidth += static_cast<size_t>(pixelWidthTable[textStr->substr(currentPos, 1)]);
                nextPosJump = 1;
            } else if (pixelWidthTable.count(textStr->substr(currentPos, 2))) {
                totalPixelWidth += static_cast<size_t>(pixelWidthTable[textStr->substr(currentPos, 2)]);
                nextPosJump = 2;
            } else {
                SPDLOG_DEBUG("Table does not contain " + textStr->substr(currentPos, 1) + "/" +
                             textStr->substr(currentPos, 2));
                SPDLOG_DEBUG("Full string: " + *textStr);
                nextPosJump = 1;
            }
        }
        currentPos += nextPosJump;
    }
    // return the total number of characters we looped through
    if (totalPixelWidth > maxLinePixelWidth && textStr->at(currentPos - nextPosJump) != ' ') {
        return currentPos - lastNewline - nextPosJump;
    } else {
        return currentPos - lastNewline;
    }
}

size_t CustomMessage::FindNEWLINE(std::string& str, size_t lastNewline) const {
    size_t newLine = str.find(NEWLINE()[0], lastNewline);
    bool done;

    // Bail out early
    if (newLine == std::string::npos) {
        return newLine;
    }

    do {
        done = true;
        if (newLine != 0) {
            switch (str[newLine - 1]) {
                case '\x05': // COLOR
                case '\x06': // SHIFT
                case '\x07': // TEXTID
                case '\x0C': // BOX_BREAK_DELAYED
                case '\x0E': // FADE
                case '\x11': // FADE2
                case '\x12': // SFX
                case '\x13': // ITEM_ICON
                case '\x14': // TEXT_SPEED
                case '\x15': // BACKGROUND
                case '\x1E': // POINTS/HIGH_SCORE
                    done = false;
                    break;
                default:
                    break;
            }
            if (newLine > 1) {
                switch (str[newLine - 2]) {
                    case '\x07': // TEXTID
                    case '\x11': // FADE2
                    case '\x12': // SFX
                    case '\x15': // BACKGROUND
                        done = false;
                        break;
                    default:
                        break;
                }
                if (newLine > 2) {
                    if (str[newLine - 3] == '\x15') { // BACKGROUND
                        done = false;
                    }
                }
            }
        }
        if (!done) {
            newLine = str.find(NEWLINE()[0], newLine + 1);
            if (newLine == std::string::npos) {
                // if we reach the end of the string, quit now to save a loop
                done = true;
            }
        }
    } while (!done);

    return newLine;
}

bool CustomMessage::AddBreakString(std::string& str, size_t pos, std::string breakString) const {
    if (str[pos] == ' ' || str[pos] == '&') {
        // don't add a break next to an existing newline
        if (str[pos] != ' ' || (str[pos + 1] != '&' && str[pos + 1] != NEWLINE()[0])) {
            str.replace(pos, 1, breakString);
        }
        return false;
    } else {
        if (pos <= str.size() - 1) {
            // If the next char is a new textbox, it has priority, ignore whatever we are replacing it with
            if (str[pos + 1] == '^') {
                return false;
                // otherwise, if it is a line break or space, replace it
            } else if (str[pos + 1] == ' ' || str[pos + 1] == '&') {
                // don't add a break next to an existing newline
                if (str[pos + 1] != ' ' || (str[pos + 2] != '&' && str[pos + 2] != NEWLINE()[0])) {
                    str.replace(pos + 1, 1, breakString);
                }
                return false;
            }
        }
        // otherwise insert after it
        str.insert(pos + 1, breakString);
        return true;
    }
}

void CustomMessage::AutoFormatString(std::string& str) const {
    ReplaceAltarIcons(str);
    ReplaceColors(str);
    // insert newlines either manually or when encountering a '&'
    size_t lastNewline = 0;
    const bool hasIcon = str.find('\x13') != std::string::npos;
    size_t lineLength = NextLineLength(&str, lastNewline, hasIcon);
    size_t lineCount = 1;
    size_t yesNo = str.find('\x1B', lastNewline);
    while (lastNewline + lineLength < str.length() || yesNo != std::string::npos) {
        const size_t carrot = str.find('^', lastNewline);
        const size_t ampersand = str.find('&', lastNewline);
        size_t waitForInput = str.find(WAIT_FOR_INPUT()[0], lastNewline);
        size_t newLine = FindNEWLINE(str, lastNewline);
        if (carrot < waitForInput) {
            waitForInput = carrot;
        }
        if (ampersand < newLine) {
            newLine = ampersand;
        }
        if (lineCount != 3 && yesNo < lastNewline + lineLength && yesNo < waitForInput && yesNo < newLine) {
            if (lineCount >= 4) {
                str.replace(yesNo, 1, "^&&\x1B");
                lineCount = 3;
                lastNewline = yesNo + 3;
            } else {
                while (lineCount < 3) {
                    str.replace(yesNo, 1, "&\x1B");
                    yesNo++;
                    lineCount++;
                }
                lastNewline = yesNo;
            }
        } else {
            if (lineCount < 4) {
                // replace '&' first if it's within the newline range
                if (newLine < lastNewline + lineLength) {
                    lastNewline = newLine + 1;
                    // or move the lastNewline cursor to the next line if a '^' is encountered
                } else if (waitForInput < lastNewline + lineLength) {
                    lastNewline = waitForInput + 1;
                    lineCount = 0;
                    // some lines need to be split but don't have spaces, look for periods instead
                } else {
                    const size_t lastBreak = str.find_last_of(".,!?- ", lastNewline + lineLength);
                    // if none exist or we go backwards, we look forward for a something and allow the overflow
                    if (lastBreak == std::string::npos || lastBreak < lastNewline) {
                        const size_t nextBreak = str.find_first_of(".,!?- &^", lastNewline);
                        if (str[nextBreak] == '^') {
                            lastNewline = nextBreak + 1;
                            lineCount = 0; // increments to 1 at the end
                        } else if (str[nextBreak] == '&') {
                            lastNewline = nextBreak + 1;
                        } else {
                            bool isAdded = AddBreakString(str, nextBreak, "&");
                            lastNewline = nextBreak + 1 + isAdded;
                        }
                    } else {
                        bool isAdded = AddBreakString(str, lastBreak, "&");
                        lastNewline = lastBreak + 1 + isAdded;
                    }
                }
                lineCount += 1;
            } else {
                const size_t lastColor = str.rfind("\x05"s, lastNewline + lineLength);
                std::string colorText = "";
                // check if we are on a non default colour, as ^ resets it, and readd if needed
                if (lastColor != std::string::npos && str[lastColor + 1] != 0) {
                    colorText = "\x05"s + str[lastColor + 1];
                }
                // replace '&' first if it's within the newline range
                if (ampersand < lastNewline + lineLength) {
                    str.replace(ampersand, 1, "^" + colorText);
                    lastNewline = ampersand + 1;
                    // or move the lastNewline cursor to the next line if a '^' is encountered.
                } else if (carrot < lastNewline + lineLength) {
                    lastNewline = carrot + 1;
                    // some lines need to be split but don't have spaces, look for punctuation instead
                } else {
                    const size_t lastBreak = str.find_last_of(".,!?- &", lastNewline + lineLength);
                    // if none exist or we go backwards, we look forward for a something and allow the overflow
                    if (lastBreak == std::string::npos || lastBreak < lastNewline) {
                        const size_t nextBreak = str.find_first_of(".,!?- &^", lastNewline);
                        if (str[nextBreak] == '^') {
                            lastNewline = nextBreak + 1;
                        } else {
                            bool isAdded = AddBreakString(str, nextBreak, "^" + colorText);
                            lastNewline = nextBreak + 1 + isAdded;
                        }
                    } else {
                        bool isAdded = AddBreakString(str, lastBreak, "^" + colorText);
                        lastNewline = lastBreak + 1 + isAdded;
                    }
                }
                lineCount = 1;
            }
            lineLength = NextLineLength(&str, lastNewline, hasIcon);
        }
        yesNo = str.find('\x1B', lastNewline);
    }
    ReplaceSpecialCharacters(str);
    ReplaceAltarIcons(str);
    std::replace(str.begin(), str.end(), '&', NEWLINE()[0]);
    std::replace(str.begin(), str.end(), '^', WAIT_FOR_INPUT()[0]);
    std::replace(str.begin(), str.end(), '@', PLAYER_NAME()[0]);
    std::replace(str.begin(), str.end(), '_', ' ');
    str += MESSAGE_END();
}

void CustomMessage::InsertNumber(uint8_t num) {
    for (std::string& str : AllMessages()) {
        size_t firstBar = str.find('|');
        if (firstBar != std::string::npos) {
            size_t secondBar = str.find('|', firstBar + 1);
            if (secondBar != std::string::npos) {
                size_t thirdBar = str.find('|', secondBar + 1);
                if (thirdBar != std::string::npos) {
                    if (num == 1) {
                        str.erase(secondBar, thirdBar - secondBar);
                    } else {
                        str.erase(firstBar, secondBar - firstBar);
                    }
                }
            }
        }
    }
    // remove the remaining bar
    Replace("|", "");
    Replace("[[d]]", std::to_string(num));
}

void CustomMessage::SetSingularPlural() {
    for (std::string& str : AllMessages()) {
        size_t firstBar = str.find('|');
        if (firstBar != std::string::npos) {
            size_t euroSign = str.find("€");
            size_t secondBar = str.find('|', firstBar + 1);
            if (secondBar != std::string::npos) {
                size_t thirdBar = str.find('|', secondBar + 1);
                if (thirdBar != std::string::npos) {
                    if (euroSign == std::string::npos) {
                        str.erase(secondBar, thirdBar - secondBar);
                    } else {
                        str.erase(firstBar, secondBar - firstBar);
                    }
                }
            }
        }
    }
    // remove the remaining bar
    Replace("|", "");
    Replace("€", "");
}

void CustomMessage::Capitalize() {
    for (std::string& str : AllMessages()) {
        if (!str.empty() && static_cast<unsigned char>(str[0]) < 0x80) {
            str[0] = std::toupper(str[0]);
        }
    }
}

void CustomMessage::ReplaceSpecialCharacters(std::string& str) const {
    // add special characters
    for (const auto& specialCharacterPair : textBoxSpecialCharacters) {
        size_t start_pos = 0;
        std::string textBoxSpecialCharacterString = ""s;
        textBoxSpecialCharacterString += specialCharacterPair.second;
        while ((start_pos = str.find(specialCharacterPair.first, start_pos)) != std::string::npos) {
            str.replace(start_pos, specialCharacterPair.first.length(), textBoxSpecialCharacterString);
            start_pos += textBoxSpecialCharacterString.length();
        }
    }
}

const char* Interface_ReplaceSpecialCharacters(char text[]) {
    std::string textString(text);

    for (const auto& specialCharacterPair : textBoxSpecialCharacters) {
        size_t start_pos = 0;
        std::string textBoxSpecialCharacterString = ""s;
        textBoxSpecialCharacterString += specialCharacterPair.second;
        while ((start_pos = textString.find(specialCharacterPair.first, start_pos)) != std::string::npos) {
            textString.replace(start_pos, specialCharacterPair.first.length(), textBoxSpecialCharacterString);
            start_pos += textBoxSpecialCharacterString.length();
        }
    }

    char* textChar = new char[textString.length() + 1];
    strcpy(textChar, textString.c_str());
    return textChar;
}

void CustomMessage::EncodeColors(std::string& str) const {
    for (const std::string& color : colors) {
        if (const size_t firstHashtag = str.find('#'); firstHashtag != std::string::npos) {
            str.replace(firstHashtag, 1, colorToPercent.at(color));
            if (const size_t secondHashtag = str.find('#', firstHashtag + 1); secondHashtag != std::string::npos) {
                str.replace(secondHashtag, 1, "%w");
            } else {
                SPDLOG_DEBUG("non-matching hashtags in string: \"{}\"", str);
            }
        }
    }
    // Remove any remaining '#' characters.
    std::erase(str, '#');
}

void CustomMessage::ReplaceColors(std::string& str) const {
    EncodeColors(str);
    for (const auto& colorPair : percentColors) {
        std::string textToReplace = "%";
        textToReplace += colorPair.first;
        size_t start_pos = 0;
        while ((start_pos = str.find(textToReplace, start_pos)) != std::string::npos) {
            str.replace(start_pos, textToReplace.length(), COLOR(colorPair.second));
            start_pos += textToReplace.length();
        }
    }
}

void CustomMessage::ReplaceAltarIcons(std::string& str) const {
    for (const auto& iconPair : altarIcons) {
        std::string textToReplace = "$";
        textToReplace += iconPair.first;
        size_t start_pos = 0;
        while ((start_pos = str.find(textToReplace, start_pos)) != std::string::npos) {
            str.replace(start_pos, textToReplace.length(), ITEM_OBTAINED(iconPair.second));
            start_pos += textToReplace.length();
        }
    }
}

void CustomMessage::InsertNames(std::vector<CustomMessage> toInsert) {
    for (uint8_t a = 0; a < toInsert.size(); a++) {
        CustomMessage temp = toInsert[a];
        if (capital.size() > a && capital[a]) {
            temp.Capitalize();
        }
        Replace("[[" + std::to_string(a + 1) + "]]", temp);
    }
}

std::string CustomMessage::MESSAGE_END() {
    return "\x02"s;
}

std::string CustomMessage::ITEM_OBTAINED(uint8_t x) {
    return "\x13"s + char(x);
}

std::string CustomMessage::NEWLINE() {
    return "\x01"s;
}

std::string CustomMessage::COLOR(std::string x) {
    return "\x05"s + x;
}

std::string CustomMessage::POINTS(std::string x) {
    return "\x1E"s + x;
}

std::string CustomMessage::WAIT_FOR_INPUT() {
    return "\x04"s;
}

std::string CustomMessage::PLAYER_NAME() {
    return "\x0F"s;
}

std::string CustomMessage::TWO_WAY_CHOICE() {
    return "\x1B"s;
}

std::string CustomMessage::THREE_WAY_CHOICE() {
    return "\x1C"s;
}

bool CustomMessageManager::InsertCustomMessage(const std::string& tableID, uint16_t textID, CustomMessage messages) {
    auto foundMessageTable = messageTables.find(tableID);
    if (foundMessageTable == messageTables.end()) {
        return false;
    }
    auto& messageTable = foundMessageTable->second;
    auto messageInsertResult = messageTable.emplace(textID, std::move(messages));
    return messageInsertResult.second;
}

bool CustomMessageManager::CreateGetItemMessage(const std::string& tableID, uint16_t giid, ItemID iid,
                                                CustomMessage messageEntry) {
    messageEntry.Format(iid);
    const uint16_t textID = giid;
    return InsertCustomMessage(tableID, textID, std::move(messageEntry));
}

bool CustomMessageManager::CreateMessage(const std::string& tableID, uint16_t textID, CustomMessage messageEntry) {
    return InsertCustomMessage(tableID, textID, std::move(messageEntry));
}

CustomMessage CustomMessageManager::RetrieveMessage(const std::string& tableID, uint16_t textID, MessageFormat format) {
    std::unordered_map<std::string, CustomMessageTable>::const_iterator foundMessageTable = messageTables.find(tableID);
    if (foundMessageTable == messageTables.end()) {
        throw(MessageNotFoundException(tableID, textID));
    }
    const CustomMessageTable& messageTable = foundMessageTable->second;
    std::unordered_map<uint16_t, CustomMessage>::const_iterator foundMessage = messageTable.find(textID);
    if (foundMessage == messageTable.end()) {
        throw(MessageNotFoundException(tableID, textID));
    }
    CustomMessage message = foundMessage->second;

    if (format == MF_FORMATTED) {
        message.Format();
    } else if (format == MF_AUTO_FORMAT) {
        message.AutoFormat();
    } else if (format == MF_CLEAN) {
        message.Clean();
    } else if (format == MF_ENCODE) {
        message.Encode();
    }

    return message;
}

bool CustomMessageManager::ClearMessageTable(const std::string& tableID) {
    auto foundMessageTable = messageTables.find(tableID);
    if (foundMessageTable == messageTables.end()) {
        return false;
    }
    auto& messageTable = foundMessageTable->second;
    messageTable.clear();
    return true;
}

bool CustomMessageManager::AddCustomMessageTable(const std::string& tableID) {
    return messageTables.try_emplace(tableID).second;
}
