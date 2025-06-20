#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_SacredForestMeadow() {
    // clang-format off
    areaTable[RR_SFM_ENTRYWAY] = Region("SFM Entryway", SCENE_SACRED_FOREST_MEADOW, {}, {}, {
        //Exits
        Entrance(RR_LW_BEYOND_MIDO,       []{return true;}),
        Entrance(RR_SACRED_FOREST_MEADOW, []{return logic->IsAdult || logic->CanKillEnemy(RE_WOLFOS);}),
        Entrance(RR_SFM_WOLFOS_GROTTO,    []{return logic->CanOpenBombGrotto();}),
    });

    areaTable[RR_SACRED_FOREST_MEADOW] = Region("Sacred Forest Meadow", SCENE_SACRED_FOREST_MEADOW, {
        //Events
        EventAccess(&logic->GossipStoneFairy, []{return logic->CallGossipFairyExceptSuns();}),
    }, {
        //Locations
        LOCATION(RC_SONG_FROM_SARIA,                       logic->IsChild && logic->HasItem(RG_ZELDAS_LETTER)),
        LOCATION(RC_SHEIK_IN_FOREST,                       logic->IsAdult),
        LOCATION(RC_SFM_GS,                                logic->IsAdult && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
        LOCATION(RC_SFM_MAZE_LOWER_GOSSIP_STONE_FAIRY,     logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_SFM_MAZE_LOWER_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_SFM_MAZE_UPPER_GOSSIP_STONE_FAIRY,     logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_SFM_MAZE_UPPER_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_SFM_SARIA_GOSSIP_STONE_FAIRY,          logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_SFM_SARIA_GOSSIP_STONE_FAIRY_BIG,      logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_SFM_MAZE_LOWER_GOSSIP_STONE,           true),
        LOCATION(RC_SFM_MAZE_UPPER_GOSSIP_STONE,           true),
        LOCATION(RC_SFM_SARIA_GOSSIP_STONE,                true),
    }, {
        //Exits
        Entrance(RR_SFM_ENTRYWAY,           []{return true;}),
        Entrance(RR_FOREST_TEMPLE_ENTRYWAY, []{return logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_SFM_FAIRY_GROTTO,       []{return true;}),
        Entrance(RR_SFM_STORMS_GROTTO,      []{return logic->CanOpenStormsGrotto();}),
    });

    areaTable[RR_SFM_FAIRY_GROTTO] = Region("SFM Fairy Grotto", SCENE_GROTTOS, {
        //Events
        EventAccess(&logic->FreeFairies, []{return true;}),
    }, {
        //Locations
        LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_1, true),
        LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_2, true),
        LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_3, true),
        LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_4, true),
        LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_5, true),
        LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_6, true),
        LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_7, true),
        LOCATION(RC_SFM_FAIRY_GROTTO_FAIRY_8, true),
    }, {
        //Exits
        Entrance(RR_SACRED_FOREST_MEADOW, []{return true;}),
    });

    areaTable[RR_SFM_WOLFOS_GROTTO] = Region("SFM Wolfos Grotto", SCENE_GROTTOS, {}, {
        //Locations
        LOCATION(RC_SFM_WOLFOS_GROTTO_CHEST, logic->CanKillEnemy(RE_WOLFOS, ED_CLOSE, true, 2)),
    }, {
        //Exits
        Entrance(RR_SFM_ENTRYWAY, []{return true;}),
    });

    areaTable[RR_SFM_STORMS_GROTTO] = Region("SFM Storms Grotto", SCENE_GROTTOS, {}, {
        //Locations
        LOCATION(RC_SFM_DEKU_SCRUB_GROTTO_REAR,  logic->CanStunDeku()),
        LOCATION(RC_SFM_DEKU_SCRUB_GROTTO_FRONT, logic->CanStunDeku()),
        LOCATION(RC_SFM_STORMS_GROTTO_BEEHIVE,   logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        Entrance(RR_SACRED_FOREST_MEADOW, []{return true;}),
    });

    // clang-format on
}
