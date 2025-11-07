#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/OTRGlobals.h"
#include "soh/ShipInit.hpp"
void RegisterShuffleFrogs() {
    SHOULD_SHUFFLE_LOCATION({
        if (location->GetRCType() == RCTYPE_FROG_SONG) {
            if (RAND_GET_OPTION(RSK_SHUFFLE_FROG_SONG_RUPEES) == RO_GENERIC_OFF) {
                *should = false;
            }
        }
    });
}

static RegisterShipInitFunc registerShuffleFrogs(RegisterShuffleFrogs, { "IS_RANDO" });