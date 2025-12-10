#pragma once

#include <cstddef>
#include <vector>
#include <stdint.h>
#include "../randomizerTypes.h"

class ItemLocation;

void AddItemToPool(std::vector<RandomizerGet>& pool, const RandomizerGet item, size_t count = 1);
RandomizerGet GetJunkItem();
void GenerateItemPool();

extern std::vector<RandomizerGet> ItemPool;
