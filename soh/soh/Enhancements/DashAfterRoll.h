#pragma once

#include "soh/Enhancements/enhancementTypes.h"
#include "soh/cvar_prefixes.h"

typedef struct Player Player;

#define CVAR_DASH_AFTER_ROLL_NAME CVAR_ENHANCEMENT("DashAfterRoll")

// Run speed multiplier the dash applies, 1.0f when it doesn't.
float Ship_GetDashRunFactor(Player* player);

// Extra per-frame acceleration toward speedTarget while dashing, 0.0f when not. Vanilla steps
// by a fixed amount, so without this the dash takes longer to reach speed the faster it is.
float Ship_GetDashAccel(Player* player, float speedTarget);
