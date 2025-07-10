#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_LonLonRanch() {
    // clang-format off
    areaTable[RR_LON_LON_RANCH] = Region("Lon Lon Ranch", "Lon Lon Ranch", {RA_LON_LON_RANCH}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->FreedEpona, []{return logic->HasItem(RG_CHILD_WALLET) && logic->CanUse(RG_EPONAS_SONG) && logic->IsAdult && logic->AtDay;}),
        EventAccess(&logic->LinksCow,   []{return logic->HasItem(RG_CHILD_WALLET) && logic->CanUse(RG_EPONA) && logic->IsAdult && logic->AtDay;}),
    }, {
        //Locations
        LOCATION(RC_SONG_FROM_MALON,     logic->IsChild && logic->HasItem(RG_ZELDAS_LETTER) && logic->HasItem(RG_FAIRY_OCARINA) && logic->AtDay),
        LOCATION(RC_LLR_GS_TREE,         logic->IsChild),
        LOCATION(RC_LLR_GS_RAIN_SHED,    logic->IsChild && logic->CanGetNightTimeGS()),
        LOCATION(RC_LLR_GS_HOUSE_WINDOW, logic->IsChild && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
        LOCATION(RC_LLR_GS_BACK_WALL,    logic->IsChild && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
        LOCATION(RC_LLR_FRONT_POT_1,     logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_LLR_FRONT_POT_2,     logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_LLR_FRONT_POT_3,     logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_LLR_FRONT_POT_4,     logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_LLR_RAIN_SHED_POT_1, logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_LLR_RAIN_SHED_POT_2, logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_LLR_RAIN_SHED_POT_3, logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_LLR_NEAR_TREE_CRATE, logic->IsChild && logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_HYRULE_FIELD,     []{return true;}),
        Entrance(RR_LLR_TALONS_HOUSE, []{return logic->CanOpenOverworldDoor(RG_TALONS_HOUSE_KEY);}),
        Entrance(RR_LLR_STABLES,      []{return logic->CanOpenOverworldDoor(RG_STABLES_KEY);}),
        Entrance(RR_LLR_TOWER,        []{return logic->CanOpenOverworldDoor(RG_BACK_TOWER_KEY);}),
        Entrance(RR_LLR_GROTTO,       []{return logic->IsChild;}),
    });

    areaTable[RR_LLR_TALONS_HOUSE] = Region("LLR Talons House", "LLR Talons House", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_LLR_TALONS_CHICKENS,    logic->HasItem(RG_CHILD_WALLET) && logic->IsChild && logic->AtDay && logic->HasItem(RG_ZELDAS_LETTER)),
        LOCATION(RC_LLR_TALONS_HOUSE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_LLR_TALONS_HOUSE_POT_2, logic->CanBreakPots()),
        LOCATION(RC_LLR_TALONS_HOUSE_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_LON_LON_RANCH, []{return true;}),
    });

    areaTable[RR_LLR_STABLES] = Region("LLR Stables", "LLR Stables", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_LLR_STABLES_LEFT_COW,  logic->CanUse(RG_EPONAS_SONG)),
        LOCATION(RC_LLR_STABLES_RIGHT_COW, logic->CanUse(RG_EPONAS_SONG)),
    }, {
        //Exits
        Entrance(RR_LON_LON_RANCH, []{return true;}),
    });

    areaTable[RR_LLR_TOWER] = Region("LLR Tower", "LLR Tower", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_LLR_FREESTANDING_POH, logic->IsChild),
        LOCATION(RC_LLR_TOWER_LEFT_COW,   logic->CanUse(RG_EPONAS_SONG)),
        LOCATION(RC_LLR_TOWER_RIGHT_COW,  logic->CanUse(RG_EPONAS_SONG)),
    }, {
        //Exits
        Entrance(RR_LON_LON_RANCH, []{return true;}),
    });

    areaTable[RR_LLR_GROTTO] = Region("LLR Grotto", "LLR Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_LLR_DEKU_SCRUB_GROTTO_LEFT,   logic->CanStunDeku()),
        LOCATION(RC_LLR_DEKU_SCRUB_GROTTO_RIGHT,  logic->CanStunDeku()),
        LOCATION(RC_LLR_DEKU_SCRUB_GROTTO_CENTER, logic->CanStunDeku()),
        LOCATION(RC_LLR_GROTTO_BEEHIVE,           logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        Entrance(RR_LON_LON_RANCH, []{return true;}),
    });

    // clang-format on
}
