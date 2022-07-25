#pragma once

#include "hints.hpp"
#include "keys.hpp"

#include <vector>

extern std::array<HintText, KEY_ENUM_MAX> hintTable;

void HintTable_Init();
const HintText& Hint(uint32_t hintKey);
std::vector<HintText> GetHintCategory(HintCategory category);

void HintTable_Init_Item();
void HintTable_Init_Exclude_Overworld();
void HintTable_Init_Exclude_Dungeon();