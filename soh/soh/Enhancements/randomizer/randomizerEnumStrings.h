#pragma once

#include <string>
#include <string_view>

#include "randomizerEnums.h"

// EnumToString gives an item's name, or an empty view for a value that isn't one of them. A
// name is a view over a string literal, so data() is NUL terminated and safe for %s.
//
// format_as is the hook fmt looks for: SPDLOG_ERROR("{}", RC_LINKS_POCKET) logs
// "RC_LINKS_POCKET" rather than a number, and a value with no name logs as
// "RandomizerCheck(...)". fmt finds it by argument dependent lookup, so including this
// header is all a caller has to do.
#define RANDO_ENUM_BEGIN(EnumName)                 \
    std::string_view EnumToString(EnumName value); \
    std::string format_as(EnumName value);
#define RANDO_ENUM_ITEM(...)
#define RANDO_ENUM_END(...)
#include "randomizerEnumList.h"
#undef RANDO_ENUM_BEGIN
#undef RANDO_ENUM_ITEM
#undef RANDO_ENUM_END
