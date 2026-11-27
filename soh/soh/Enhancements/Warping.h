#include <nlohmann/json.hpp>

void ApplyWarpPointPreset(const nlohmann::json& preset, bool merge);
void SaveWarpPointPreset(nlohmann::json& preset);