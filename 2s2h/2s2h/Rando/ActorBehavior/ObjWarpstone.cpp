#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
}

RandoCheckId Identify_Statue(u8 refId) {
    switch (refId) {
        case 0:
            return RC_GREAT_BAY_COAST_OWL_STATUE;
        case 1:
            return RC_ZORA_CAPE_OWL_STATUE;
        case 2:
            return RC_SNOWHEAD_OWL_STATUE;
        case 3:
            return RC_MOUNTAIN_VILLAGE_OWL_STATUE;
        case 4:
            return RC_CLOCK_TOWN_SOUTH_OWL_STATUE;
        case 5:
            return RC_MILK_ROAD_OWL_STATUE;
        case 6:
            return RC_WOODFALL_OWL_STATUE;
        case 7:
            return RC_SOUTHERN_SWAMP_OWL_STATUE;
        case 8:
            return RC_IKANA_CANYON_OWL_STATUE;
        case 9:
            return RC_STONE_TOWER_OWL_STATUE;
        default:
            break;
    }

    return RC_UNKNOWN;
}

void Rando::ActorBehavior::InitObjWarpstoneBehavior() {
    COND_VB_SHOULD(VB_OWL_STATUE_ACTIVATE, IS_RANDO, {
        u8 warpstoneId = (u8)va_arg(args, u32);
        RandoCheckId randoCheckId = Identify_Statue(warpstoneId);

        if (randoCheckId != RC_UNKNOWN && RANDO_SAVE_CHECKS[randoCheckId].shuffled) {
            if (!RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
                RANDO_SAVE_CHECKS[randoCheckId].eligible = true;
            }
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_OWL_STATUE_BE_ACTIVE, IS_RANDO, {
        u8 warpstoneId = (u8)va_arg(args, u32);
        RandoCheckId randoCheckId = Identify_Statue(warpstoneId);

        if (randoCheckId != RC_UNKNOWN && RANDO_SAVE_CHECKS[randoCheckId].shuffled) {
            *should = RANDO_SAVE_CHECKS[randoCheckId].cycleObtained;
        }
    });
}
