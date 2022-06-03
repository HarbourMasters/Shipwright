#pragma once

#include "keys.hpp"

#include <vector>
#include <stdint.h>

extern std::vector<uint32_t> StartingInventory;
extern uint8_t AdditionalHeartContainers;

void GenerateStartingInventory();
bool StartingInventoryHasBottle();
void ApplyStartingInventory();
