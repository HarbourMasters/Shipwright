#pragma once

#include <nlohmann/json_fwd.hpp>

namespace Lang {

// Generated from ui-lang/en_US.json at configure time; no duplicated translations.
nlohmann::json GetBuiltinUICatalog();

} // namespace Lang
