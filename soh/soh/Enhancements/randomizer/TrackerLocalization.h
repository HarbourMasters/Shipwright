#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace TrackerLocalization {

// UI-thread only. Display text only. Never use these strings for saves, seed identities or widget IDs.
// References remain valid until the UI language or catalog revision changes.
const std::string& Name(std::string_view section, std::string_view id, std::string_view fallback,
                        std::string_view field = {});

// Changes with either the selected UI language or the loaded catalog data.
uint64_t Revision();

} // namespace TrackerLocalization
