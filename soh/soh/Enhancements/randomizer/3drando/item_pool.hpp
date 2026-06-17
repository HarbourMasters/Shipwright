#pragma once

#include <cstddef>
#include <vector>
#include "soh/Enhancements/randomizer/randomizerEnums.h"

class ItemLocation;

void AddItemToPool(std::vector<RandomizerGet>& pool, const RandomizerGet item, size_t count = 1);
RandomizerGet GetJunkItem();
bool GenerateItemPool();

extern std::vector<RandomizerGet> itemPool;
