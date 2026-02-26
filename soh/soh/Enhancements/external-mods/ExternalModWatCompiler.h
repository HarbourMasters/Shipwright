#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace SOH {

bool CompileWatToWasm(const std::string& sourceText, const std::string& sourceName, std::vector<uint8_t>& outWasm,
                      int32_t& outCompileMs, std::string& outDiagnostics);

} // namespace SOH