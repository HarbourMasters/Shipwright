#pragma once

#include "hints.hpp"
#include <vector>

extern std::array<HintText, RHT_MAX> hintTable;

void HintTable_Init();
const HintText& GetHintText(const RandomizerHintTextKey hintKey);

void HintTable_Init_Item();
void HintTable_Init_Exclude_Overworld();
void HintTable_Init_Exclude_Dungeon();