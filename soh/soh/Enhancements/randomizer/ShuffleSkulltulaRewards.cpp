#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/context.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/OTRGlobals.h"
#include "soh/ShipInit.hpp"

// RANDOTODO: Convert the rest of the skulltula reward handling
// to use hooks in this file.

void RegisterShuffleSkulltulaRewards() {
    SHOULD_SHUFFLE_LOCATION({
        if (location->GetRandomizerCheck() == RC_KAK_100_GOLD_SKULLTULA_REWARD) {
            if (RAND_GET_OPTION(RSK_SHUFFLE_100_GS_REWARD) == RO_GENERIC_OFF) {
                *should = false;
            }
        }
    });
}

static RegisterShipInitFunc registerShuffleSkulltulaRewards(RegisterShuffleSkulltulaRewards, { "IS_RANDO" });