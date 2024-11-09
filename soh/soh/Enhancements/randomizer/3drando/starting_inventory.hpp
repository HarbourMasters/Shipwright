#pragma once

#include "../randomizerTypes.h"

#include <vector>
#include <stdint.h>

extern std::vector<RandomizerGet> StartingInventory;
extern uint8_t AdditionalHeartContainers;

void GenerateStartingInventory();
bool StartingInventoryHasBottle();
void ApplyStartingInventory();
