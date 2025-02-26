#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_ThievesHideout() {
    areaTable[RR_TH_NORTH_F1_CARPENTER_AREA] = Region("Thieves Hideout North F1 Carpenter Area", "Thieves Hideout", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->CarpenterRescue, []{return logic->CanFinishGerudoFortress();}),
    }, {
        //Locations
        LOCATION(RC_GF_NORTH_F1_CARPENTER,       logic->CanKillEnemy(RE_GERUDO_WARRIOR)),
        LOCATION(RC_GF_NORTH_F1_CARPENTER_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GF_NORTH_F1_CARPENTER_POT_2, logic->CanBreakPots()),
        LOCATION(RC_GF_NORTH_F1_CARPENTER_POT_3, logic->CanBreakPots()),
        LOCATION(RC_GF_GERUDO_MEMBERSHIP_CARD,   logic->CanFinishGerudoFortress()),
    }, {
        //Exits
        Entrance(RR_GF_GROUND_RED,    []{return true;}),
        Entrance(RR_GF_GROUND_ORANGE, []{return true;}),
    });

    areaTable[RR_TH_SOUTH_F1_CARPENTER_AREA] = Region("Thieves Hideout South F1 Carpenter Area", "Thieves Hideout", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->CarpenterRescue, []{return logic->CanFinishGerudoFortress();}),
    }, {
        //Locations
        LOCATION(RC_GF_SOUTH_F1_CARPENTER,            logic->CanKillEnemy(RE_GERUDO_WARRIOR)),
        LOCATION(RC_GF_SOUTH_F1_CARPENTER_POT_1,      logic->CanBreakPots()),
        LOCATION(RC_GF_SOUTH_F1_CARPENTER_POT_2,      logic->CanBreakPots()),
        LOCATION(RC_GF_SOUTH_F1_CARPENTER_POT_3,      logic->CanBreakPots()),
        LOCATION(RC_GF_SOUTH_F1_CARPENTER_CELL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GF_SOUTH_F1_CARPENTER_CELL_POT_2, logic->CanBreakPots()),
        LOCATION(RC_GF_SOUTH_F1_CARPENTER_CELL_POT_3, logic->CanBreakPots()),
        LOCATION(RC_GF_SOUTH_F1_CARPENTER_CELL_POT_4, logic->CanBreakPots()),
        LOCATION(RC_GF_GERUDO_MEMBERSHIP_CARD,        logic->CanFinishGerudoFortress()),
    }, {
        //Exits
        Entrance(RR_GF_GROUND_RED,    []{return true;}),
        Entrance(RR_GF_GROUND_ORANGE, []{return true;}),
    });

    areaTable[RR_TH_NORTH_F2_CARPENTER_AREA] = Region("Thieves Hideout North F2 Carpenter Area", "Thieves Hideout", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->CarpenterRescue, []{return logic->CanFinishGerudoFortress();}),
    }, {
        //Locations
        LOCATION(RC_GF_NORTH_F2_CARPENTER,       logic->CanKillEnemy(RE_GERUDO_WARRIOR)),
        LOCATION(RC_GF_NORTH_F2_CARPENTER_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GF_NORTH_F2_CARPENTER_POT_2, logic->CanBreakPots()),
        LOCATION(RC_GF_GERUDO_MEMBERSHIP_CARD,   logic->CanFinishGerudoFortress()),
    }, {
        //Exits
        Entrance(RR_GF_ROOFTOP_MAGENTA, []{return true;}),
    });

    areaTable[RR_TH_SOUTH_F2_CARPENTER_AREA] = Region("Thieves Hideout South F2 Carpenter Area", "Thieves Hideout", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->CarpenterRescue, []{return logic->CanFinishGerudoFortress();}),
    }, {
        //Locations
        LOCATION(RC_GF_SOUTH_F2_CARPENTER,     logic->CanKillEnemy(RE_GERUDO_WARRIOR)),
        LOCATION(RC_GF_GERUDO_MEMBERSHIP_CARD, logic->CanFinishGerudoFortress()),
    }, {
        //Exits
        Entrance(RR_GF_ROOFTOP_WHITE, []{return true;}),
        Entrance(RR_GF_ROOFTOP_CYAN,  []{return true;}),
    });

    areaTable[RR_TH_KITCHEN_BOTTOM] = Region("Thieves Hideout Kitchen Bottom", "Thieves Hideout", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_GF_GROUND_ORANGE, []{return true;}),
        Entrance(RR_GF_ROOFTOP_WHITE, []{return true;}),
        Entrance(RR_TH_KITCHEN_MAIN,  []{return logic->CanPassEnemy(RE_GERUDO_GUARD) || ctx->GetTrickOption(RT_GF_KITCHEN);}),
    });

    areaTable[RR_TH_KITCHEN_MAIN] = Region("Thieves Hideout Kitchen Bottom", "Thieves Hideout", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GF_KITCHEN_POT_1,     logic->CanBreakPots() && logic->CanPassEnemy(RE_GERUDO_GUARD)),
        LOCATION(RC_GF_KITCHEN_POT_2,     logic->CanBreakPots() && logic->CanPassEnemy(RE_GERUDO_GUARD)),
        LOCATION(RC_GF_KITCHEN_SUN_FAIRY, logic->CanPassEnemy(RE_GERUDO_GUARD) && logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        Entrance(RR_TH_KITCHEN_BOTTOM, []{return logic->CanPassEnemy(RE_GERUDO_GUARD);}),
        Entrance(RR_TH_KITCHEN_TOP,    []{return logic->CanPassEnemy(RE_GERUDO_GUARD) || ctx->GetTrickOption(RT_GF_KITCHEN);}),
    });

    areaTable[RR_TH_KITCHEN_TOP] = Region("Thieves Hideout Kitchen Top", "Thieves Hideout", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_TH_KITCHEN_MAIN,  []{return true;}),
        Entrance(RR_GF_ROOFTOP_GREEN, []{return logic->CanPassEnemy(RE_GERUDO_GUARD) || logic->CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_GF_KITCHEN);}),
        Entrance(RR_GF_ROOFTOP_CYAN,  []{return logic->CanPassEnemy(RE_GERUDO_GUARD) || logic->CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_GF_KITCHEN);}),
    });

    areaTable[RR_TH_BREAK_ROOM] = Region("Thieves Hideout Break Room", "Thieves Hideout", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GF_BREAK_ROOM_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GF_BREAK_ROOM_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GF_ROOFTOP_YELLOW,  []{return true;}),
        /*
        Entrance(RR_GF_TOP_OF_FORTRESS, []{return true;}),
        */
    });
}