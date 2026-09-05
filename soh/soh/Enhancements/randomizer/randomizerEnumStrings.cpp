#include "randomizerEnumStrings.h"

static std::string NameOrValue(std::string_view name, std::string_view enumName, long long value) {
    if (!name.empty()) {
        return std::string(name);
    }
    return std::string(enumName) + '(' + std::to_string(value) + ')';
}

#define RANDO_ENUM_BEGIN(EnumName)                  \
    std::string_view EnumToString(EnumName value) { \
        switch (value) {

#define RANDO_ENUM_ITEM(name, ...) \
    case name:                     \
        return #name;

#define RANDO_ENUM_END(EnumName)                                              \
    }                                                                         \
    return {};                                                                \
    }                                                                         \
    std::string format_as(EnumName value) {                                   \
        return NameOrValue(EnumToString(value), #EnumName, (long long)value); \
    }

#include "randomizerEnumList.h"
