#pragma once

#include <cstddef>
#include <vector>
#include <stdint.h>
#include "../randomizerTypes.h"

class ItemLocation;

void AddItemToPool(std::vector<RandomizerGet>& pool, const RandomizerGet item, size_t count = 1);
RandomizerGet GetJunkItem();
void PlaceJunkInExcludedLocation(const RandomizerCheck il);
void GenerateItemPool();
void AddJunk();

extern std::vector<RandomizerGet> ItemPool;
