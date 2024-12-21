#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_SacredForestMeadow() {
    areaTable[RR_SFM_ENTRYWAY] = Region("SFM Entryway", "Sacred Forest Meadow", {RA_SACRED_FOREST_MEADOW}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_LW_BEYOND_MIDO,       {[]{return true;}}),
        Entrance(RR_SACRED_FOREST_MEADOW, {[]{return logic->CanJumpslash() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE);}}),
        Entrance(RR_SFM_WOLFOS_GROTTO,    {[]{return logic->CanOpenBombGrotto();}}),
    });

    areaTable[RR_SACRED_FOREST_MEADOW] = Region("Sacred Forest Meadow", "Sacred Forest Meadow", {RA_SACRED_FOREST_MEADOW}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->GossipStoneFairy, {[]{return logic->CallGossipFairyExceptSuns();}}),
    }, {
        //Locations
        LOCATION(RC_SONG_FROM_SARIA,             logic->IsChild && logic->HasItem(RG_ZELDAS_LETTER)),
        LOCATION(RC_SHEIK_IN_FOREST,             logic->IsAdult),
        LOCATION(RC_SFM_GS,                      logic->IsAdult && logic->HookshotOrBoomerang() && logic->AtNight && logic->CanGetNightTimeGS()),
        LOCATION(RC_SFM_MAZE_LOWER_GOSSIP_STONE, true),
        LOCATION(RC_SFM_MAZE_UPPER_GOSSIP_STONE, true),
        LOCATION(RC_SFM_SARIA_GOSSIP_STONE,      true),
    }, {
        //Exits
        Entrance(RR_SFM_ENTRYWAY,           {[]{return true;}}),
        Entrance(RR_FOREST_TEMPLE_ENTRYWAY, {[]{return logic->CanUse(RG_HOOKSHOT);}}),
        Entrance(RR_SFM_FAIRY_GROTTO,       {[]{return true;}}),
        Entrance(RR_SFM_STORMS_GROTTO,      {[]{return logic->CanOpenStormsGrotto();}}),
    });

    areaTable[RR_SFM_FAIRY_GROTTO] = Region("SFM Fairy Grotto", "SFM Fairy Grotto", {}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->FreeFairies, {[]{return true;}}),
    }, {}, {
        //Exits
        Entrance(RR_SACRED_FOREST_MEADOW, {[]{return true;}}),
    });

    areaTable[RR_SFM_WOLFOS_GROTTO] = Region("SFM Wolfos Grotto", "SFM Wolfos Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SFM_WOLFOS_GROTTO_CHEST, logic->IsAdult || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_STICKS) || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_DINS_FIRE) || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)), //RANDOTODO is this meant to auto-pass as adult?
    }, {
        //Exits
        Entrance(RR_SFM_ENTRYWAY, {[]{return true;}}),
    });

    areaTable[RR_SFM_STORMS_GROTTO] = Region("SFM Storms Grotto", "SFM Storms Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SFM_DEKU_SCRUB_GROTTO_REAR,  logic->CanStunDeku()),
        LOCATION(RC_SFM_DEKU_SCRUB_GROTTO_FRONT, logic->CanStunDeku()),
        LOCATION(RC_SFM_STORMS_GROTTO_BEEHIVE,   logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        Entrance(RR_SACRED_FOREST_MEADOW, {[]{return true;}}),
    });
}