#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_TempleOfTime() {
    // clang-format off
    areaTable[RR_TOT_ENTRANCE] = Region("ToT Entrance", SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY, {
        //Events
        EventAccess(&logic->GossipStoneFairy, []{return logic->CallGossipFairyExceptSuns();}),
    }, {
        //Locations
        LOCATION(RC_TOT_LEFTMOST_GOSSIP_STONE_FAIRY,         logic->CallGossipFairyExceptSuns() || (logic->CanUse(RG_SUNS_SONG) && logic->IsAdult)),
        LOCATION(RC_TOT_LEFTMOST_GOSSIP_STONE_FAIRY_BIG,     logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_TOT_LEFT_CENTER_GOSSIP_STONE_FAIRY,      logic->CallGossipFairyExceptSuns() || (logic->CanUse(RG_SUNS_SONG) && logic->IsAdult)),
        LOCATION(RC_TOT_LEFT_CENTER_GOSSIP_STONE_FAIRY_BIG,  logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_TOT_RIGHT_CENTER_GOSSIP_STONE_FAIRY,     logic->CallGossipFairyExceptSuns() || (logic->CanUse(RG_SUNS_SONG) && logic->IsAdult)),
        LOCATION(RC_TOT_RIGHT_CENTER_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_TOT_RIGHTMOST_GOSSIP_STONE_FAIRY,        logic->CallGossipFairyExceptSuns() || (logic->CanUse(RG_SUNS_SONG) && logic->IsAdult)),
        LOCATION(RC_TOT_RIGHTMOST_GOSSIP_STONE_FAIRY_BIG,    logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_TOT_LEFTMOST_GOSSIP_STONE,               true),
        LOCATION(RC_TOT_LEFT_CENTER_GOSSIP_STONE,            true),
        LOCATION(RC_TOT_RIGHT_CENTER_GOSSIP_STONE,           true),
        LOCATION(RC_TOT_RIGHTMOST_GOSSIP_STONE,              true),
    }, {
        //Exits
        Entrance(RR_THE_MARKET,     []{return true;}),
        Entrance(RR_TEMPLE_OF_TIME, []{return true;}),
    });

    areaTable[RR_TEMPLE_OF_TIME] = Region("Temple of Time", SCENE_TEMPLE_OF_TIME, {}, {
        //Locations
        LOCATION(RC_TOT_LIGHT_ARROWS_CUTSCENE, logic->IsAdult && logic->CanTriggerLACS()),
        LOCATION(RC_ALTAR_HINT_CHILD,          logic->IsChild),
        LOCATION(RC_ALTAR_HINT_ADULT,          logic->IsAdult),
        LOCATION(RC_TOT_SHEIK_HINT,            logic->IsAdult),
    }, {
        //Exits
        Entrance(RR_TOT_ENTRANCE,            []{return true;}),
        Entrance(RR_TOT_BEYOND_DOOR_OF_TIME, []{return ctx->GetOption(RSK_DOOR_OF_TIME).Is(RO_DOOROFTIME_OPEN) || (logic->CanUse(RG_SONG_OF_TIME) && (ctx->GetOption(RSK_DOOR_OF_TIME).Is(RO_DOOROFTIME_SONGONLY) || (logic->StoneCount() == 3 && logic->HasItem(RG_OCARINA_OF_TIME))));}),
    });

    areaTable[RR_TOT_BEYOND_DOOR_OF_TIME] = Region("Beyond Door of Time", SCENE_TEMPLE_OF_TIME, {
        //Events
        //EventAccess(&logic->TimeTravel, []{return true;}),
    }, {
        //Locations
        LOCATION(RC_TOT_MASTER_SWORD, logic->IsAdult),
        LOCATION(RC_GIFT_FROM_RAURU,  logic->IsAdult),
        LOCATION(RC_SHEIK_AT_TEMPLE,  logic->HasItem(RG_FOREST_MEDALLION) && logic->IsAdult),
    }, {
        //Exits
        Entrance(RR_TEMPLE_OF_TIME, []{return true;}),
    });

    // clang-format on
}
