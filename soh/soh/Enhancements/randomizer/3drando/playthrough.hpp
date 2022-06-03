#pragma once
#include <string>
#include <set>
#include "item_location.hpp"

namespace Playthrough {
    int Playthrough_Init(uint32_t seed);
    int Playthrough_Repeat(int count = 1);
}
