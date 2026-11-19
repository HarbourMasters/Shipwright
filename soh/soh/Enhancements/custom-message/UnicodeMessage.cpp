#include "UnicodeMessage.h"
#include "JapaneseMessageLayout.h"
#include <algorithm>
#include <vector>

namespace UnicodeMessage {
std::size_t ControlSize(unsigned char control) {
    switch (control) {
        case 0x05:
        case 0x06:
        case 0x0C:
        case 0x0E:
        case 0x13:
        case 0x14:
        case 0x1E:
            return 2;
        case 0x07:
        case 0x11:
        case 0x12:
            return 3;
        case 0x15:
            return 4;
        default:
            return 1;
    }
}

namespace {
struct Token {
    std::string text;
    unsigned int code;
    int width;
    bool breakBefore = true;
    int groupWidth = 0;
    int groupUnits = 0;
};
int DecodedUnits(const Token& token) {
    return token.code == 0x0F || (token.code >= 0x16 && token.code <= 0x1F) ? 8
           : token.code < 0x20                                              ? static_cast<int>(token.text.size())
                                                                            : 1;
}
int VariableWidth(unsigned int code, bool japaneseFont) {
    if (!japaneseFont)
        return code == 0x0F ? 96 : 0;
    switch (code) {
        case 0x0F:
            return 8 * JapaneseMessageLayout::MaxNameGlyphAdvance; // Longest player name.
        case 0x16:
        case 0x17:
        case 0x1F:
            return 6 * 14;
        case 0x18:
        case 0x1E:
            return 5 * 14;
        case 0x19:
            return 3 * 14;
        default:
            return 0;
    }
}
std::vector<Token> Tokens(const std::string& text, bool japaneseFont = false) {
    std::vector<Token> tokens;
    for (std::size_t cursor = 0; cursor < text.size();) {
        const auto byte = static_cast<unsigned char>(text[cursor]);
        if (byte < 0x20) {
            const auto size = ControlSize(byte);
            if (cursor + size > text.size())
                break;
            const int width = japaneseFont && byte == 0x06 ? static_cast<unsigned char>(text[cursor + 1])
                                                           : VariableWidth(byte, japaneseFont);
            tokens.push_back({ text.substr(cursor, size), byte, width });
            cursor += size;
        } else if (byte == '&' || byte == '^' || byte == '@') {
            const unsigned int code = byte == '^' ? 4 : byte == '@' ? 15 : 1;
            tokens.push_back({ std::string(1, static_cast<char>(code)), code, VariableWidth(code, japaneseFont) });
            ++cursor;
        } else if (byte >= 0x80 && byte <= 0xAB) {
            // Explicit legacy glyph bytes (buttons/diacritics) remain valid.
            tokens.push_back({ text.substr(cursor++, 1), byte, japaneseFont ? 14 : 12 });
        } else {
            uint32_t scalar;
            const auto size = ReadScalar(text, cursor, scalar);
            const bool space = scalar == '_' || (japaneseFont && scalar == U'　');
            const std::string normalized = scalar == 0xFFFD ? "�" : space ? " " : text.substr(cursor, size);
            const int width = japaneseFont                     ? JapaneseMessageLayout::Advance(space ? ' ' : scalar)
                              : scalar >= 0x80                 ? 16
                              : scalar == ' ' || scalar == '_' ? 6
                              : std::string("il.,'!:;").find(static_cast<char>(scalar)) != std::string::npos ? 5
                                                                                                             : 10;
            tokens.push_back({ normalized, space ? ' ' : scalar, width });
            cursor += size;
        }
    }
    return tokens;
}

bool IsBoundary(unsigned int code) {
    return code == 1 || code == 2 || code == 4 || code == 12 || code == 0x1B || code == 0x1C;
}

void GroupJapanesePunctuation(std::vector<Token>& tokens) {
    uint32_t previous = 0;
    for (auto& token : tokens) {
        if (IsBoundary(token.code)) {
            previous = 0;
        } else if (token.width > 0) {
            token.breakBefore =
                !previous ||
                (!(JapaneseMessageLayout::IsAsciiWord(previous) && JapaneseMessageLayout::IsAsciiWord(token.code)) &&
                 !JapaneseMessageLayout::CannotEndLine(previous) &&
                 !JapaneseMessageLayout::CannotStartLine(token.code));
            previous = token.code;
        }
    }
    int following = 0;
    int followingUnits = 0;
    for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
        if (IsBoundary(it->code)) {
            following = 0;
            followingUnits = 0;
        } else {
            it->groupWidth = it->width + following;
            it->groupUnits = DecodedUnits(*it) + followingUnits;
            if (it->width > 0) {
                following = it->breakBefore ? 0 : it->groupWidth;
                followingUnits = it->breakBefore ? 0 : it->groupUnits;
            } else if (following > 0) {
                followingUnits += DecodedUnits(*it);
            }
        }
    }
}
} // namespace

bool RequiresUnicode(const std::string& text) {
    const auto tokens = Tokens(text);
    return std::any_of(tokens.begin(), tokens.end(), [](const auto& token) { return token.code > 0xFF; });
}

std::string Format(const std::string& text, bool japaneseFont, std::optional<uint8_t> itemIcon) {
    const std::string requestedIcon = itemIcon ? std::string{ '\x13', static_cast<char>(*itemIcon) } : "";
    auto tokens = Tokens(itemIcon ? requestedIcon + text : text, japaneseFont);
    if (japaneseFont)
        GroupJapanesePunctuation(tokens);
    const bool hasIcon =
        std::any_of(tokens.begin(), tokens.end(), [](const auto& token) { return token.code == 0x13; });
    const int maxWidth = japaneseFont ? JapaneseMessageLayout::LineWidth(hasIcon) : hasIcon ? 200 : 240;
    const int maxLines = japaneseFont ? JapaneseMessageLayout::LinesPerPage : 4;
    std::string result;
    std::string color;
    std::string icon;
    int line = 1;
    int width = 0;
    int pageUnits = 0;
    const auto page = [&] {
        result += '\x04';
        if (japaneseFont || itemIcon)
            result += icon;
        result += color;
        line = 1;
        width = 0;
        pageUnits = japaneseFont || itemIcon ? static_cast<int>(icon.size() + color.size()) : 0;
    };
    const auto newline = [&](int upcomingUnits = 0) {
        if (line == maxLines || pageUnits + 1 + upcomingUnits > 100)
            page();
        else {
            result += '\x01';
            ++line;
            ++pageUnits;
        }
        width = 0;
    };
    for (const auto& token : tokens) {
        const int units = DecodedUnits(token);
        // Reserve the possible continuation icon and color (four bytes).
        const int requiredUnits =
            japaneseFont && token.breakBefore && token.groupUnits <= 96 ? std::max(units, token.groupUnits) : units;
        if (token.code == 0x02)
            break;
        if (token.code == 0x04 || token.code == 0x0C) {
            result += token.text;
            color.clear();
            icon = requestedIcon;
            result += icon;
            line = 1;
            width = 0;
            pageUnits = static_cast<int>(icon.size());
        } else if (token.code == 0x01) {
            newline();
        } else {
            if (pageUnits + requiredUnits > 100)
                page();
            if (token.code == 0x1B || token.code == 0x1C) {
                const int startLine = maxLines - (token.code == 0x1B ? 1 : 2);
                if (line > startLine || (japaneseFont && line == startLine && width > 0)) {
                    page();
                }
                while (line < startLine)
                    newline(units);
                result += token.text;
                pageUnits += units;
            } else {
                // Move the preceding character with closing punctuation; opening
                // brackets move with the following character. Style/control bytes
                // stay ordered inside the group. Pathological groups wider than
                // an entire line fall back to bounded character wrapping.
                const int required =
                    japaneseFont && token.breakBefore && token.groupWidth <= maxWidth ? token.groupWidth : token.width;
                if (token.width && width + required > maxWidth)
                    newline(requiredUnits);
                if (!(width == 0 && token.code == ' ')) {
                    result += token.text;
                    width += token.width;
                    pageUnits += units;
                }
                if (token.code == 0x05)
                    color = token.text;
                if (token.code == 0x13)
                    icon = token.text;
            }
        }
    }
    result += '\x02';
    return result;
}

std::string FitToBuffer(const std::string& text, std::size_t capacity) {
    if (capacity == 0)
        return {};
    if (text.size() <= capacity)
        return text;
    std::string result;
    for (const auto& token : Tokens(text)) {
        if (token.code == 0x02 || result.size() + token.text.size() + 1 > capacity)
            break;
        result += token.text;
    }
    result += '\x02';
    return result;
}
} // namespace UnicodeMessage
