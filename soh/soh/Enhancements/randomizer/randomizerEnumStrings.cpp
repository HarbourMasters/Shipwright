#include "randomizerEnumStrings.h"

#include "randomizerEnums.h"

// Redefine enum macros to generate string->enum maps for every enum.
#define RANDO_ENUM_BEGIN(EnumName)                                                                     \
    template <> const std::unordered_map<std::string_view, EnumName>& GetStringToEnumMap<EnumName>() { \
        static const std::unordered_map<std::string_view, EnumName> map = [] {         \
            std::unordered_map<std::string_view, EnumName> m;

#define RANDO_ENUM_ITEM(name, ...) m.emplace(#name, name);

#define RANDO_ENUM_END(EnumName) \
    return m;                    \
    }                            \
    ();                          \
    return map;                  \
    }

#include "randomizerEnums.h"

// Redefine enum macros to generate enum->string maps for every enum.
#define RANDO_ENUM_BEGIN(EnumName)                                                                     \
    template <> const std::unordered_map<EnumName, std::string_view>& GetEnumToStringMap<EnumName>() { \
        static const std::unordered_map<EnumName, std::string_view> map = [] {         \
            std::unordered_map<EnumName, std::string_view> m;

#define RANDO_ENUM_ITEM(name, ...) m.emplace(name, #name);

#define RANDO_ENUM_END(EnumName) \
    return m;                    \
    }                            \
    ();                          \
    return map;                  \
    }

#include "randomizerEnums.h"