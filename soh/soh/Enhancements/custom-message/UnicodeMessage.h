#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <optional>
#include "soh/Enhancements/Lang/Utf8.h"

namespace UnicodeMessage {
// UTF-8 is text; byte controls retain the established Western command ABI.
// These helpers never interpret a control operand as a character or separator.
using Lang::Utf8::ReadScalar;
std::size_t ControlSize(unsigned char control);
bool RequiresUnicode(const std::string& text);
// An explicit item icon belongs on every authored/automatic page, exactly once.
// Without this argument, authored pages may intentionally omit/change an icon.
std::string Format(const std::string& text, bool japaneseFont = false, std::optional<uint8_t> itemIcon = std::nullopt);
std::string FitToBuffer(const std::string& text, std::size_t capacity);
} // namespace UnicodeMessage
