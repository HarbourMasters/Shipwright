#pragma once

#include <stdbool.h>

#include "soh/cvar_prefixes.h"

#define CVAR_ADULT_MASKS_NAME CVAR_ENHANCEMENT("AdultMasks")

#ifdef __cplusplus
extern "C" {
#endif
// Whether adult Link may equip the child masks: the enhancement, the cheat, or whatever rando forces on top of them.
bool Ship_MasksEquippableAsAdult();
#ifdef __cplusplus
};
#endif
