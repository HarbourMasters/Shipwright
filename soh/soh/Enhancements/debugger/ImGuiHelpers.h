#pragma once
#include "../libultraship/Lib/ImGui/imgui.h"

#include <string>

void SetLastItemHoverText(const std::string& text);

void InsertHelpHoverText(const std::string& text);

void PaddedSeparator(bool padTop = true, bool padBottom = true, float extraVerticalPadding = 0);
