#pragma once

#include <string>
#include <variant>

namespace Lang {

enum Error {
    LanguageDataNotInitialized,
    LanguageDataNotFound,
    PathDataNotFound,
    PathInvalidValue,
};

// TODO: Upgrade to std::expected when we update to C++ 23
std::variant<std::reference_wrapper<const std::string>, Lang::Error> TryTranslate(const char* path);
const std::string& Translate(const char* path);
void LoadLangs();
} // namespace Lang