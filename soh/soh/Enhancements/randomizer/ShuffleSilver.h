#pragma once

#include <stdint.h>
#include "randomizerEnums.h"
#include <stdint.h>
#include <libultraship/libultra/types.h>

bool IsSilverInPool(RandomizerGet rg);
RandomizerGet SilverFromSwitchFlag(s16 switchFlag);
bool IsSilverCleared(RandomizerGet rg);
bool IsSilver(RandomizerGet rg);
