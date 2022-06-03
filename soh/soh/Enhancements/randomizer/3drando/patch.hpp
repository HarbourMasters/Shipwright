#pragma once
#include <set>
#include "playthrough.hpp"
#include "settings.hpp"

#define V_TO_P(addr) (addr - 0x100000)
#define P_TO_V(offset) (offset + 0x100000)
#define PATCH_CONSOLE 0
#define PATCH_CITRA 1
#define PATCH_SIZE_MAX 65535

bool WriteAllPatches();
