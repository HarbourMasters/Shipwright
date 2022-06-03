#pragma once

#include "hints.hpp"
#include "keys.hpp"

#include <vector>

void HintTable_Init();
const HintText& Hint(uint32_t hintKey);
std::vector<HintText> GetHintCategory(HintCategory category);
