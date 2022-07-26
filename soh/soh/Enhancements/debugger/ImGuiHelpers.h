#pragma once
#include "../libultraship/Lib/ImGui/imgui.h"

#include <string>

std::string BreakTooltip(const char* text);

void SetLastItemHoverText(const std::string& text);

void InsertHelpHoverText(const std::string& text);
