#pragma once

#include "keys.hpp"

#include <cstddef>
#include <vector>
#include <stdint.h>

class ItemLocation;

void AddItemToPool(std::vector<uint32_t>& pool, const uint32_t item, size_t count = 1);
uint32_t GetJunkItem();
void PlaceJunkInExcludedLocation(const uint32_t il);
void GenerateItemPool();
void AddJunk();

extern std::vector<uint32_t> ItemPool;
extern std::vector<uint8_t> IceTrapModels;
