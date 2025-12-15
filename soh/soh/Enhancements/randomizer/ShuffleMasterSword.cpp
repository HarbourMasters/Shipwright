#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/context.h"
#include "soh/OTRGlobals.h"
#include "soh/ShipInit.hpp"

// RANDOTODO: Convert remaining Shuffle Master Sword Changes to use
// hooks in this file.

void RegisterShuffleMasterSword() {
    SHOULD_SHUFFLE_LOCATION({
        if (location->GetRandomizerCheck() == RC_TOT_MASTER_SWORD) {
            if (RAND_GET_OPTION(RSK_SHUFFLE_MASTER_SWORD) == RO_GENERIC_OFF) {
                *should = false;
            }
        }
    });
}

static RegisterShipInitFunc registerShuffleMasterSword(RegisterShuffleMasterSword, { "IS_RANDO" });