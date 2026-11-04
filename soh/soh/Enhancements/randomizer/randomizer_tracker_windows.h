#pragma once

#include <string>

#include <imgui.h>
#include <libultraship/color.h>

#include "randomizerEnums.h"

namespace Trackers {
bool IsDockedToMainDeck(const std::string& UniqueName);
bool BeginFloatWindows(const std::string& UniqueName, bool* open, const Color_RGBA8& bgCol,
                       TrackerWindowType windowType, bool draggable, ImGuiWindowFlags flags = 0);
void EndFloatWindows();
} // namespace Trackers
