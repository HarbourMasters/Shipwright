#pragma once
#include "../libultraship/Lib/ImGui/imgui.h"

#include <string>

void SetLastItemHoverText(const std::string& text);

void InsertHelpHoverText(const std::string& text);

void PaddedSeparator(bool topPadding = true, bool bottomPadding = true, float x = 0.0f, float y = 0.0f);