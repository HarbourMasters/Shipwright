#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_Graveyard() {
    // clang-format off
    areaTable[RR_THE_GRAVEYARD] = Region("The Graveyard", "The Graveyard", {RA_THE_GRAVEYARD}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->ButterflyFairy,  []{return logic->ButterflyFairy || (logic->CanUse(RG_STICKS) && logic->AtDay);}),
        EventAccess(&logic->BeanPlantFairy,  []{return logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS);}),
        EventAccess(&logic->BugRock,         []{return true;}),
        EventAccess(&logic->BorrowBunnyHood, []{return logic->IsChild && logic->AtDay && logic->BorrowSpookyMask && logic->HasItem(RG_CHILD_WALLET);}),
    }, {
        //Locations
        LOCATION(RC_GRAVEYARD_FREESTANDING_POH,        (((logic->IsAdult && CanPlantBean(RR_THE_GRAVEYARD)) || logic->CanUse(RG_LONGSHOT)) && logic->CanBreakCrates()) || (ctx->GetTrickOption(RT_GY_POH) && logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR, logic->HasItem(RG_CHILD_WALLET) && logic->IsChild && logic->AtNight), //TODO: This needs to change
        LOCATION(RC_GRAVEYARD_GS_WALL,                 logic->IsChild && logic->HookshotOrBoomerang() && logic->AtNight && logic->CanGetNightTimeGS()),
        LOCATION(RC_GRAVEYARD_GS_BEAN_PATCH,           logic->CanSpawnSoilSkull() && logic->CanAttack()),
        LOCATION(RC_GRAVEYARD_BEAN_SPROUT_FAIRY_1,     logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GRAVEYARD_BEAN_SPROUT_FAIRY_2,     logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GRAVEYARD_BEAN_SPROUT_FAIRY_3,     logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GY_GRASS_1,                        logic->CanCutShrubs()),
        LOCATION(RC_GY_GRASS_2,                        logic->CanCutShrubs()),
        LOCATION(RC_GY_GRASS_3,                        logic->CanCutShrubs()),
        LOCATION(RC_GY_GRASS_4,                        logic->CanCutShrubs()),
        LOCATION(RC_GY_GRASS_5,                        logic->CanCutShrubs()),
        LOCATION(RC_GY_GRASS_6,                        logic->CanCutShrubs()),
        LOCATION(RC_GY_GRASS_7,                        logic->CanCutShrubs()),
        LOCATION(RC_GY_GRASS_8,                        logic->CanCutShrubs()),
        LOCATION(RC_GY_GRASS_9,                        logic->CanCutShrubs()),
        LOCATION(RC_GY_GRASS_10,                       logic->CanCutShrubs()),
        LOCATION(RC_GY_GRASS_11,                       logic->CanCutShrubs()),
        LOCATION(RC_GY_GRASS_12,                       logic->CanCutShrubs()),
        LOCATION(RC_GRAVEYARD_CRATE,                   ((logic->IsAdult && CanPlantBean(RR_THE_GRAVEYARD)) || logic->CanUse(RG_LONGSHOT)) && logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_GRAVEYARD_SHIELD_GRAVE,       []{return logic->IsAdult || logic->AtNight;}),
        Entrance(RR_GRAVEYARD_COMPOSERS_GRAVE,    []{return logic->CanUse(RG_ZELDAS_LULLABY);}),
        Entrance(RR_GRAVEYARD_HEART_PIECE_GRAVE,  []{return logic->IsAdult || logic->AtNight;}),
        Entrance(RR_GRAVEYARD_DAMPES_GRAVE,       []{return logic->IsAdult;}),
        Entrance(RR_GRAVEYARD_DAMPES_HOUSE,       []{return logic->IsAdult && logic->CanOpenOverworldDoor(RG_DAMPES_HUT_KEY) /*|| logic->AtDampeTime*/;}), //TODO: This needs to be handled in ToD rework
        Entrance(RR_KAKARIKO_VILLAGE,             []{return true;}),
        Entrance(RR_GRAVEYARD_WARP_PAD_REGION,    []{return false;}),
    });

    areaTable[RR_GRAVEYARD_SHIELD_GRAVE] = Region("Graveyard Shield Grave", "Graveyard Shield Grave", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GRAVEYARD_SHIELD_GRAVE_CHEST, true),
    }, {
        //Exits
        Entrance(RR_THE_GRAVEYARD,               []{return true;}),
        Entrance(RR_GRAVEYARD_SHIELD_GRAVE_BACK, []{return Here(RR_GRAVEYARD_SHIELD_GRAVE, []{return logic->CanBreakMudWalls();});}),
    });

    areaTable[RR_GRAVEYARD_SHIELD_GRAVE_BACK] = Region("Graveyard Shield Grave Back", "Graveyard Shield Grave", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GRAVEYARD_SHIELD_GRAVE_FAIRY_1, true),
        LOCATION(RC_GRAVEYARD_SHIELD_GRAVE_FAIRY_2, true),
        LOCATION(RC_GRAVEYARD_SHIELD_GRAVE_FAIRY_3, true),
        LOCATION(RC_GRAVEYARD_SHIELD_GRAVE_FAIRY_4, true),
        LOCATION(RC_GRAVEYARD_SHIELD_GRAVE_FAIRY_5, true),
        LOCATION(RC_GRAVEYARD_SHIELD_GRAVE_FAIRY_6, true),
        LOCATION(RC_GRAVEYARD_SHIELD_GRAVE_FAIRY_7, true),
        LOCATION(RC_GRAVEYARD_SHIELD_GRAVE_FAIRY_8, true),
    }, {
        //Exits
        Entrance(RR_GRAVEYARD_SHIELD_GRAVE, []{return true;}),
    });

    areaTable[RR_GRAVEYARD_HEART_PIECE_GRAVE] = Region("Graveyard Heart Piece Grave", "Graveyard Heart Piece Grave", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST, logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        Entrance(RR_THE_GRAVEYARD, []{return true;}),
    });

    areaTable[RR_GRAVEYARD_COMPOSERS_GRAVE] = Region("Graveyard Composers Grave", "Graveyard Composers Grave", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST,     logic->HasFireSource()),
        LOCATION(RC_SONG_FROM_ROYAL_FAMILYS_TOMB,           logic->CanUseProjectile() || logic->CanJumpslash()), 
        LOCATION(RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_SUN_FAIRY, logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        Entrance(RR_THE_GRAVEYARD, []{return true;}),
    });

    areaTable[RR_GRAVEYARD_DAMPES_GRAVE] = Region("Graveyard Dampes Grave", "Windmill and Dampes Grave", {}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->NutPot,               []{return true;}),
        EventAccess(&logic->DampesWindmillAccess, []{return logic->DampesWindmillAccess || (logic->IsAdult && logic->CanUse(RG_SONG_OF_TIME));}),
    }, {
        //Locations
        LOCATION(RC_GRAVEYARD_HOOKSHOT_CHEST,              true),
        LOCATION(RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH, logic->IsAdult || ctx->GetTrickOption(RT_GY_CHILD_DAMPE_RACE_POH)),
        LOCATION(RC_GY_DAMPES_GRAVE_POT_1,                 logic->CanBreakPots()),
        LOCATION(RC_GY_DAMPES_GRAVE_POT_2,                 logic->CanBreakPots()),
        LOCATION(RC_GY_DAMPES_GRAVE_POT_3,                 logic->CanBreakPots()),
        LOCATION(RC_GY_DAMPES_GRAVE_POT_4,                 logic->CanBreakPots()),
        LOCATION(RC_GY_DAMPES_GRAVE_POT_5,                 logic->CanBreakPots()),
        LOCATION(RC_GY_DAMPES_GRAVE_POT_6,                 logic->CanBreakPots()),
        LOCATION(RC_GRAVEYARD_DAMPE_RACE_RUPEE_1,          true),
        LOCATION(RC_GRAVEYARD_DAMPE_RACE_RUPEE_2,          true),
        LOCATION(RC_GRAVEYARD_DAMPE_RACE_RUPEE_3,          true),
        LOCATION(RC_GRAVEYARD_DAMPE_RACE_RUPEE_4,          true),
        LOCATION(RC_GRAVEYARD_DAMPE_RACE_RUPEE_5,          true),
        LOCATION(RC_GRAVEYARD_DAMPE_RACE_RUPEE_6,          true),
        LOCATION(RC_GRAVEYARD_DAMPE_RACE_RUPEE_7,          true),
        LOCATION(RC_GRAVEYARD_DAMPE_RACE_RUPEE_8,          true),
    }, {
        //Exits
        Entrance(RR_THE_GRAVEYARD, []{return true;}),
        Entrance(RR_KAK_WINDMILL,  []{return logic->IsAdult && logic->CanUse(RG_SONG_OF_TIME);}, false),
    });

    areaTable[RR_GRAVEYARD_DAMPES_HOUSE] = Region("Graveyard Dampes House", "Graveyard Dampes House", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_DAMPE_HINT, logic->IsAdult),
    }, {
        //Exits
        Entrance(RR_THE_GRAVEYARD, []{return true;}),
    });

    areaTable[RR_GRAVEYARD_WARP_PAD_REGION] = Region("Graveyard Warp Pad Region", "Graveyard", {RA_THE_GRAVEYARD}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->GossipStoneFairy, []{return logic->CallGossipFairyExceptSuns();}),
    }, {
        //Locations
        LOCATION(RC_GRAVEYARD_GOSSIP_STONE_FAIRY,     logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_GRAVEYARD_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_GRAVEYARD_GOSSIP_STONE,           true),
    }, {
        //Exits
        Entrance(RR_THE_GRAVEYARD,          []{return true;}),
        Entrance(RR_SHADOW_TEMPLE_ENTRYWAY, []{return logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_GY_SHADOW_FIRE_ARROWS) && logic->IsAdult && logic->CanUse(RG_FIRE_ARROWS));}),
    });

    // clang-format on
}
