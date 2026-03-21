#pragma once

// Precompiled header for the soh target.
// Only include stable, expensive headers that appear across many translation units.
// Do NOT include frequently-changing project headers here.

// ============================================================
// Standard Library
// ============================================================
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

// ============================================================
// Third-party: nlohmann/json (header-only, very expensive — 29 TUs)
// ============================================================
#include <nlohmann/json.hpp>

// ============================================================
// Third-party: spdlog (63 TUs include spdlog/spdlog.h)
// ============================================================
#include <spdlog/spdlog.h>

// ============================================================
// Third-party: Dear ImGui (7 TUs, but imgui_internal.h is expensive)
// ============================================================
#include <imgui.h>
#include <imgui_internal.h>

// ============================================================
// libultraship (67 soh TUs include this)
// ============================================================
#include <libultraship/libultraship.h>
