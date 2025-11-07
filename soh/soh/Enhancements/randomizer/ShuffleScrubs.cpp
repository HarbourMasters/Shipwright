#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/context.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/OTRGlobals.h"
#include "soh/ShipInit.hpp"

//RANDOTODO: Convert the reset of Scrub Shuffle to be in this file and
//use hooks appropriately

void RegisterShuffleScrubs() {
    SHOULD_SHUFFLE_LOCATION({
        if (location->GetRCType() == RCTYPE_SCRUB) {
            switch (RAND_GET_OPTION(RSK_SHUFFLE_SCRUBS)) {
                case RO_SCRUBS_OFF:
                    *should = false;
                    break;
                case RO_SCRUBS_ONE_TIME_ONLY:
                    switch (location->GetRandomizerCheck()) {
                        case RC_LW_DEKU_SCRUB_GROTTO_FRONT:
                        case RC_LW_DEKU_SCRUB_NEAR_BRIDGE:
                        case RC_HF_DEKU_SCRUB_GROTTO:
                            break;
                        default:
                            *should = false;
                            break;
                    }
            }
        }
    });
}

static RegisterShipInitFunc registerShuffleScrubs(RegisterShuffleScrubs, { "IS_RANDO" });