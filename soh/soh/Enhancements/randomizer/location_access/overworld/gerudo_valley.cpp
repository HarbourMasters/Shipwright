#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_GerudoValley() {
    // clang-format off
    areaTable[RR_GERUDO_VALLEY] = Region("Gerudo Valley", "Gerudo Valley", {RA_GERUDO_VALLEY}, DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->BugRock, []{return logic->IsChild;}),
    }, {
        //Locations
        LOCATION(RC_GV_GS_SMALL_BRIDGE, logic->IsChild && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        Entrance(RR_HYRULE_FIELD,     []{return true;}),
        Entrance(RR_GV_UPPER_STREAM,  []{return logic->IsChild || logic->HasItem(RG_BRONZE_SCALE) || logic->TakeDamage();}),
        Entrance(RR_GV_CRATE_LEDGE,   []{return logic->IsChild || logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_GV_GROTTO_LEDGE,  []{return true;}),
        Entrance(RR_GV_FORTRESS_SIDE, []{return (logic->IsAdult && (logic->CanUse(RG_EPONA) || logic->CanUse(RG_LONGSHOT) || ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE) || logic->CarpenterRescue)) || (logic->IsChild && logic->CanUse(RG_HOOKSHOT));}),
        Entrance(RR_GV_LOWER_STREAM,  []{return logic->IsChild;}), //can use cucco as child
    });

    areaTable[RR_GV_UPPER_STREAM] = Region("GV Upper Stream", "Gerudo Valley", {RA_GERUDO_VALLEY}, DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->GossipStoneFairy, []{return logic->CallGossipFairy();}),
        EventAccess(&logic->BeanPlantFairy,   []{return logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS);}),
    }, {
        //Locations
        LOCATION(RC_GV_WATERFALL_FREESTANDING_POH, logic->IsChild || logic->HasItem(RG_BRONZE_SCALE)),//can use cucco as child
        LOCATION(RC_GV_GS_BEAN_PATCH,              logic->CanSpawnSoilSkull() && logic->CanAttack()),
        LOCATION(RC_GV_COW,                        logic->IsChild && logic->CanUse(RG_EPONAS_SONG)),
        LOCATION(RC_GV_BEAN_SPROUT_FAIRY_1,        logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GV_BEAN_SPROUT_FAIRY_2,        logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GV_BEAN_SPROUT_FAIRY_3,        logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GV_GOSSIP_STONE_FAIRY,         logic->CallGossipFairy()),
        LOCATION(RC_GV_GOSSIP_STONE_FAIRY_BIG,     logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GV_GOSSIP_STONE,               true),
        LOCATION(RC_GV_NEAR_COW_CRATE,             logic->IsChild && logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_GV_LOWER_STREAM, []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS);}),
    });

    // scale/boots logic is outside lower stream, as lower stream combines access to lake hylia for entrance randomizer's sake
    areaTable[RR_GV_LOWER_STREAM] = Region("GV Lower Stream", "Gerudo Valley", {RA_GERUDO_VALLEY}, DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_LAKE_HYLIA, []{return true;}),
    });

    areaTable[RR_GV_GROTTO_LEDGE] = Region("GV Grotto Ledge", "Gerudo Valley", {RA_GERUDO_VALLEY}, DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_GV_UPPER_STREAM,   []{return ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives();}),
        Entrance(RR_GV_LOWER_STREAM,   []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS);}),
        Entrance(RR_GV_OCTOROK_GROTTO, []{return logic->CanUse(RG_SILVER_GAUNTLETS);}),
        Entrance(RR_GV_CRATE_LEDGE,    []{return logic->CanUse(RG_LONGSHOT);}),
    });

    areaTable[RR_GV_CRATE_LEDGE] = Region("GV Crate Ledge", "Gerudo Valley", {RA_GERUDO_VALLEY}, DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GV_CRATE_FREESTANDING_POH, logic->CanBreakCrates()), 
        LOCATION(RC_GV_FREESTANDING_POH_CRATE, logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_GV_UPPER_STREAM, []{return ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives();}),
        Entrance(RR_GV_LOWER_STREAM, []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS);}),
    });

    areaTable[RR_GV_FORTRESS_SIDE] = Region("GV Fortress Side", "Gerudo Valley", {RA_GERUDO_VALLEY}, DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GV_CHEST,          logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER)),
        LOCATION(RC_GV_TRADE_SAW,      logic->IsAdult && logic->CanUse(RG_POACHERS_SAW)),
        LOCATION(RC_GV_GS_BEHIND_TENT, logic->IsAdult && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
        LOCATION(RC_GV_GS_PILLAR,      logic->IsAdult && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
        LOCATION(RC_GV_CRATE_BRIDGE_1, logic->IsChild && logic->CanBreakCrates()),
        LOCATION(RC_GV_CRATE_BRIDGE_2, logic->IsChild && logic->CanBreakCrates()),
        LOCATION(RC_GV_CRATE_BRIDGE_3, logic->IsChild && logic->CanBreakCrates()),
        LOCATION(RC_GV_CRATE_BRIDGE_4, logic->IsChild && logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_GERUDO_FORTRESS,   []{return true;}),
        Entrance(RR_GV_UPPER_STREAM,   []{return true;}),
        Entrance(RR_GERUDO_VALLEY,     []{return logic->IsChild || logic->CanUse(RG_EPONA) || logic->CanUse(RG_LONGSHOT) || ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE) || logic->CarpenterRescue;}),
        Entrance(RR_GV_CARPENTER_TENT, []{return logic->IsAdult;}),
        Entrance(RR_GV_STORMS_GROTTO,  []{return logic->IsAdult && logic->CanOpenStormsGrotto();}),
        Entrance(RR_GV_CRATE_LEDGE,    []{return ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives();}),
    });

    areaTable[RR_GV_CARPENTER_TENT] = Region("GV Carpenter Tent", "GV Carpenter Tent", {}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_GV_FORTRESS_SIDE, []{return true;}),
    });

    areaTable[RR_GV_OCTOROK_GROTTO] = Region("GV Octorok Grotto", "GV Octorok Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GV_OCTOROK_GROTTO_FRONT_LEFT_BLUE_RUPEE,   logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GV_OCTOROK_GROTTO_FRONT_RIGHT_BLUE_RUPEE,  logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GV_OCTOROK_GROTTO_BACK_BLUE_RUPEE,         logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GV_OCTOROK_GROTTO_FRONT_LEFT_GREEN_RUPEE,  logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GV_OCTOROK_GROTTO_FRONT_RIGHT_GREEN_RUPEE, logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GV_OCTOROK_GROTTO_BACK_LEFT_GREEN_RUPEE,   logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GV_OCTOROK_GROTTO_BACK_RIGHT_GREEN_RUPEE,  logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GV_OCTOROK_GROTTO_RED_RUPEE,               logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        Entrance(RR_GV_GROTTO_LEDGE, []{return true;}),
    });

    areaTable[RR_GV_STORMS_GROTTO] = Region("GV Storms Grotto", "GV Storms Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GV_DEKU_SCRUB_GROTTO_REAR,    logic->CanStunDeku()),
        LOCATION(RC_GV_DEKU_SCRUB_GROTTO_FRONT,   logic->CanStunDeku()),
        LOCATION(RC_GV_DEKU_SCRUB_GROTTO_BEEHIVE, logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        Entrance(RR_GV_FORTRESS_SIDE, []{return true;}),
    });

    // clang-format on
}
