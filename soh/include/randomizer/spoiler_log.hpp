#pragma once

#include <array>
#include <string>
#include <string_view>
#include "randomizerTypes.h"

using RandomizerHash = std::array<std::string, 5>;

void GenerateHash();
const RandomizerHash& GetRandomizerHash();

void WriteIngameSpoilerLog();

bool SpoilerLog_Write();
const SpoilerData& GetSpoilerData();