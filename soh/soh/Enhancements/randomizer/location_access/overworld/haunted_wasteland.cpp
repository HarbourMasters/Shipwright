#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_HauntedWasteland() {
    areaTable[RR_WASTELAND_NEAR_FORTRESS] = Region("Wasteland Near Fortress", "Haunted Wasteland", {RA_HAUNTED_WASTELAND}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_GF_OUTSIDE_GATE,   []{return true;}),
        Entrance(RR_HAUNTED_WASTELAND, []{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT) || ctx->GetTrickOption(RT_HW_CROSSING);}),
    });

    areaTable[RR_HAUNTED_WASTELAND] = Region("Haunted Wasteland", "Haunted Wasteland", {RA_HAUNTED_WASTELAND}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->FairyPot,       []{return true;}),
        EventAccess(&logic->NutPot,         []{return true;}),
        EventAccess(&logic->CarpetMerchant, []{return logic->HasItem(RG_ADULT_WALLET) && CanBuyAnother(RC_WASTELAND_BOMBCHU_SALESMAN) && (logic->CanJumpslashExceptHammer() || logic->CanUse(RG_HOVER_BOOTS));}),
    }, {
        //Locations
        LOCATION(RC_WASTELAND_CHEST,            logic->HasFireSource()),
        LOCATION(RC_WASTELAND_BOMBCHU_SALESMAN, logic->CanJumpslashExceptHammer() || logic->CanUse(RG_HOVER_BOOTS)),
        LOCATION(RC_WASTELAND_GS,               logic->HookshotOrBoomerang()),
        LOCATION(RC_WASTELAND_NEAR_GS_POT_1,    logic->CanBreakPots()),
        LOCATION(RC_WASTELAND_NEAR_GS_POT_2,    logic->CanBreakPots()),
        LOCATION(RC_WASTELAND_NEAR_GS_POT_3,    logic->CanBreakPots()),
        LOCATION(RC_WASTELAND_NEAR_GS_POT_4,    logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_WASTELAND_NEAR_COLOSSUS, []{return ctx->GetTrickOption(RT_LENS_HW) || logic->CanUse(RG_LENS_OF_TRUTH);}),
        Entrance(RR_WASTELAND_NEAR_FORTRESS, []{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT) || ctx->GetTrickOption(RT_HW_CROSSING);}),
    });

    areaTable[RR_WASTELAND_NEAR_COLOSSUS] = Region("Wasteland Near Colossus", "Haunted Wasteland", {RA_HAUNTED_WASTELAND}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_DESERT_COLOSSUS,   []{return true;}),
        Entrance(RR_HAUNTED_WASTELAND, []{return ctx->GetTrickOption(RT_HW_REVERSE) || false;}),
    });
}