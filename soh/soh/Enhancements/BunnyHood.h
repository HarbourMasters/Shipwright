#pragma once

#include "soh/Enhancements/enhancementTypes.h"
#include "soh/cvar_prefixes.h"

typedef struct Player Player;

#define CVAR_BUNNY_HOOD_NAME CVAR_ENHANCEMENT("MMBunnyHood")

BunnyHoodMode Ship_GetBunnyHoodMode();
// Speed multipliers the effect applies to the player, 1.0f when it doesn't.
float Ship_GetBunnyHoodRunFactor(Player* player);
float Ship_GetBunnyHoodJumpFactor(Player* player);
