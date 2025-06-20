#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_GoronCity() {
    // clang-format off
    areaTable[RR_GORON_CITY] = Region("Goron City", SCENE_GORON_CITY, {
        //Events
        EventAccess(&logic->GossipStoneFairy,          []{return logic->CallGossipFairyExceptSuns();}),
        EventAccess(&logic->StickPot,                  []{return logic->IsChild;}),
        EventAccess(&logic->BugRock,                   []{return logic->BlastOrSmash() || logic->CanUse(RG_SILVER_GAUNTLETS);}),
        EventAccess(&logic->GoronCityChildFire,        []{return logic->IsChild && logic->CanUse(RG_DINS_FIRE);}),
        EventAccess(&logic->GCWoodsWarpOpen,           []{return logic->CanDetonateUprightBombFlower() || logic->CanUse(RG_MEGATON_HAMMER) || logic->GoronCityChildFire;}),
        EventAccess(&logic->GCDaruniasDoorOpenChild,   []{return logic->IsChild && logic->CanUse(RG_ZELDAS_LULLABY);}),
        // bottle animation causes similar complications as stopping goron with Din's Fire, only put in logic when both din's & blue fire tricks enabled
        EventAccess(&logic->StopGCRollingGoronAsAdult, []{return logic->IsAdult && (logic->HasItem(RG_GORONS_BRACELET) || logic->HasExplosives() || logic->CanUse(RG_FAIRY_BOW) ||
                                                                                   (ctx->GetTrickOption(RT_GC_LINK_GORON_DINS) && (logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_BLUE_FIRE_MUD_WALLS) && logic->CanUse(RG_BOTTLE_WITH_BLUE_FIRE)))));}),
    }, {
        //Locations
        LOCATION(RC_GC_MAZE_LEFT_CHEST,             logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_SILVER_GAUNTLETS) || (ctx->GetTrickOption(RT_GC_LEFTMOST) && logic->HasExplosives() && logic->CanUse(RG_HOVER_BOOTS))),
        LOCATION(RC_GC_MAZE_CENTER_CHEST,           logic->BlastOrSmash()  || logic->CanUse(RG_SILVER_GAUNTLETS)),
        LOCATION(RC_GC_MAZE_RIGHT_CHEST,            logic->BlastOrSmash()  || logic->CanUse(RG_SILVER_GAUNTLETS)),
        LOCATION(RC_GC_POT_FREESTANDING_POH,        logic->IsChild && logic->GoronCityChildFire && (logic->CanUse(RG_BOMB_BAG) || (logic->HasItem(RG_GORONS_BRACELET) && ctx->GetTrickOption(RT_GC_POT_STRENGTH)) || (logic->CanUse(RG_BOMBCHU_5) && ctx->GetTrickOption(RT_GC_POT)))),
        LOCATION(RC_GC_ROLLING_GORON_AS_CHILD,      logic->IsChild && (logic->HasExplosives() || (logic->HasItem(RG_GORONS_BRACELET) && ctx->GetTrickOption(RT_GC_ROLLING_STRENGTH)))),
        LOCATION(RC_GC_ROLLING_GORON_AS_ADULT,      logic->StopGCRollingGoronAsAdult),
        LOCATION(RC_GC_GS_BOULDER_MAZE,             logic->IsChild && logic->BlastOrSmash()),
        LOCATION(RC_GC_GS_CENTER_PLATFORM,          logic->IsAdult && logic->CanAttack()),
        LOCATION(RC_GC_MEDIGORON,                   logic->IsAdult && (logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET))),
        LOCATION(RC_GC_MAZE_GOSSIP_STONE_FAIRY,     (logic->BlastOrSmash() || logic->CanUse(RG_SILVER_GAUNTLETS)) && logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_GC_MAZE_GOSSIP_STONE_FAIRY_BIG, (logic->BlastOrSmash() || logic->CanUse(RG_SILVER_GAUNTLETS)) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GC_MAZE_GOSSIP_STONE,           logic->BlastOrSmash() || logic->CanUse(RG_SILVER_GAUNTLETS)),
        LOCATION(RC_GC_LOWER_STAIRCASE_POT_1,       logic->CanBreakPots()),
        LOCATION(RC_GC_LOWER_STAIRCASE_POT_2,       logic->CanBreakPots()),
        LOCATION(RC_GC_UPPER_STAIRCASE_POT_1,       logic->CanBreakPots()),
        LOCATION(RC_GC_UPPER_STAIRCASE_POT_2,       logic->CanBreakPots()),
        LOCATION(RC_GC_UPPER_STAIRCASE_POT_3,       logic->CanBreakPots()),
        LOCATION(RC_GC_MAZE_CRATE,                  logic->BlastOrSmash()  || (logic->CanUse(RG_SILVER_GAUNTLETS) && logic->CanBreakCrates())),

    }, {
        //Exits
        Entrance(RR_DEATH_MOUNTAIN_TRAIL, []{return true;}),
        Entrance(RR_GC_MEDIGORON,         []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);}),
        Entrance(RR_GC_WOODS_WARP,        []{return logic->GCWoodsWarpOpen;}),
        Entrance(RR_GC_SHOP,              []{return (logic->IsAdult && logic->StopGCRollingGoronAsAdult) || (logic->IsChild && (logic->BlastOrSmash() || logic->HasItem(RG_GORONS_BRACELET) || logic->GoronCityChildFire || logic->CanUse(RG_FAIRY_BOW)));}),
        Entrance(RR_GC_DARUNIAS_CHAMBER,  []{return (logic->IsAdult && logic->StopGCRollingGoronAsAdult) || (logic->IsChild && logic->GCDaruniasDoorOpenChild);}),
        Entrance(RR_GC_GROTTO_PLATFORM,   []{return logic->IsAdult && ((logic->CanUse(RG_SONG_OF_TIME) && ((logic->EffectiveHealth() > 2) || logic->CanUse(RG_GORON_TUNIC) || logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_NAYRUS_LOVE))) || (logic->EffectiveHealth() > 1 && logic->CanUse(RG_GORON_TUNIC) && logic->CanUse(RG_HOOKSHOT)) || (logic->CanUse(RG_NAYRUS_LOVE) && logic->CanUse(RG_HOOKSHOT)) || (logic->EffectiveHealth() > 2 && logic->CanUse(RG_HOOKSHOT) && ctx->GetTrickOption(RT_GC_GROTTO)));}),
    });

    areaTable[RR_GC_MEDIGORON] = Region("GC Medigoron", SCENE_GORON_CITY, {}, {
        //Locations
        LOCATION(RC_GC_MEDIGORON_GOSSIP_STONE_FAIRY,     logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_GC_MEDIGORON_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GC_MEDIGORON_GOSSIP_STONE,           true),
        LOCATION(RC_GC_MEDIGORON_POT_1,                  logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GORON_CITY, []{return true;}),
    });

    areaTable[RR_GC_WOODS_WARP] = Region("GC Woods Warp", SCENE_GORON_CITY, {
        //Events
        EventAccess(&logic->GCWoodsWarpOpen, []{return logic->BlastOrSmash() || logic->CanUse(RG_DINS_FIRE);}),
    }, {}, {
        //Exits
        Entrance(RR_GORON_CITY,     []{return logic->CanLeaveForest() && logic->GCWoodsWarpOpen;}),
        Entrance(RR_THE_LOST_WOODS, []{return true;}),
    });

    areaTable[RR_GC_DARUNIAS_CHAMBER] = Region("GC Darunias Chamber", SCENE_GORON_CITY, {
        //Events
        EventAccess(&logic->GoronCityChildFire, []{return logic->IsChild && logic->CanUse(RG_STICKS);}),
    }, {
        //Locations
        LOCATION(RC_GC_DARUNIAS_JOY,  logic->IsChild && logic->CanUse(RG_SARIAS_SONG)),
        LOCATION(RC_GC_DARUNIA_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GC_DARUNIA_POT_2, logic->CanBreakPots()),
        LOCATION(RC_GC_DARUNIA_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GORON_CITY,      []{return true;}),
        Entrance(RR_DMC_LOWER_LOCAL, []{return logic->IsAdult;}),
    });

    areaTable[RR_GC_GROTTO_PLATFORM] = Region("GC Grotto Platform", SCENE_GORON_CITY, {}, {}, {
        //Exits
        Entrance(RR_GC_GROTTO,  []{return true;}),
        Entrance(RR_GORON_CITY, []{return logic->EffectiveHealth() > 2 || logic->CanUse(RG_GORON_TUNIC) || logic->CanUse(RG_NAYRUS_LOVE) || ((logic->IsChild || logic->CanUse(RG_SONG_OF_TIME)) && logic->CanUse(RG_LONGSHOT));}),
    });

    areaTable[RR_GC_SHOP] = Region("GC Shop", SCENE_GORON_SHOP, {}, {
        //Locations
        LOCATION(RC_GC_SHOP_ITEM_1, true),
        LOCATION(RC_GC_SHOP_ITEM_2, true),
        LOCATION(RC_GC_SHOP_ITEM_3, true),
        LOCATION(RC_GC_SHOP_ITEM_4, true),
        LOCATION(RC_GC_SHOP_ITEM_5, true),
        LOCATION(RC_GC_SHOP_ITEM_6, true),
        LOCATION(RC_GC_SHOP_ITEM_7, true),
        LOCATION(RC_GC_SHOP_ITEM_8, true),
    }, {
        //Exits
        Entrance(RR_GORON_CITY, []{return true;}),
    });

    areaTable[RR_GC_GROTTO] = Region("GC Grotto", SCENE_GROTTOS, {}, {
        //Locations
        LOCATION(RC_GC_DEKU_SCRUB_GROTTO_LEFT,   logic->CanStunDeku()),
        LOCATION(RC_GC_DEKU_SCRUB_GROTTO_RIGHT,  logic->CanStunDeku()),
        LOCATION(RC_GC_DEKU_SCRUB_GROTTO_CENTER, logic->CanStunDeku()),
        LOCATION(RC_GC_GROTTO_BEEHIVE,           logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        Entrance(RR_GC_GROTTO_PLATFORM, []{return true;}),
    });

    // clang-format on
}
