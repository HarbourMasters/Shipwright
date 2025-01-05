#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_ZorasFountain() {
    areaTable[RR_ZORAS_FOUNTAIN] = Region("Zoras Fountain", "Zoras Fountain", {RA_ZORAS_FOUNTAIN}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->GossipStoneFairy, []{return logic->CallGossipFairyExceptSuns();}),
        EventAccess(&logic->ButterflyFairy,   []{return logic->ButterflyFairy   || (logic->CanUse(RG_STICKS) && logic->AtDay);}),
    }, {
        //Locations
        LOCATION(RC_ZF_ICEBERG_FREESTANDING_POH,      logic->IsAdult),
        LOCATION(RC_ZF_BOTTOM_FREESTANDING_POH,       logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_GS_TREE,                       logic->IsChild),
        LOCATION(RC_ZF_GS_ABOVE_THE_LOG,              logic->IsChild && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
        LOCATION(RC_ZF_GS_HIDDEN_CAVE,                logic->CanUse(RG_SILVER_GAUNTLETS) && logic->BlastOrSmash() && logic->HookshotOrBoomerang() && logic->IsAdult && logic->CanGetNightTimeGS()),
        LOCATION(RC_ZF_HIDDEN_CAVE_POT_1,             logic->CanUse(RG_SILVER_GAUNTLETS) && logic->IsAdult && logic->BlastOrSmash() && logic->CanBreakPots()),
        LOCATION(RC_ZF_HIDDEN_CAVE_POT_2,             logic->CanUse(RG_SILVER_GAUNTLETS) && logic->IsAdult && logic->BlastOrSmash() && logic->CanBreakPots()),
        LOCATION(RC_ZF_HIDDEN_CAVE_POT_3,             logic->CanUse(RG_SILVER_GAUNTLETS) && logic->IsAdult && logic->BlastOrSmash() && logic->CanBreakPots()),
        LOCATION(RC_ZF_FAIRY_GOSSIP_STONE_FAIRY,      logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_ZF_FAIRY_GOSSIP_STONE_FAIRY_BIG,  logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_ZF_JABU_GOSSIP_STONE_FAIRY,       logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_ZF_JABU_GOSSIP_STONE_FAIRY_BIG,   logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_ZF_BOTTOM_NORTH_INNER_RUPEE,      logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_NORTHEAST_INNER_RUPEE,  logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_SOUTHEAST_INNER_RUPEE,  logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_SOUTH_INNER_RUPEE,      logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_SOUTHWEST_INNER_RUPEE,  logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_NORTHWEST_INNER_RUPEE,  logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_NORTH_MIDDLE_RUPEE,     logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_NORTHEAST_MIDDLE_RUPEE, logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_SOUTHEAST_MIDDLE_RUPEE, logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_SOUTH_MIDDLE_RUPEE,     logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_SOUTHWEST_MIDDLE_RUPEE, logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_NORTHWEST_MIDDLE_RUPEE, logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_NORTH_OUTER_RUPEE,      logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_NORTHEAST_OUTER_RUPEE,  logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_SOUTHEAST_OUTER_RUPEE,  logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_SOUTH_OUTER_RUPEE,      logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_SOUTHWEST_OUTER_RUPEE,  logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_BOTTOM_NORTHWEST_OUTER_RUPEE,  logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
        LOCATION(RC_ZF_FAIRY_GOSSIP_STONE,            true),
        LOCATION(RC_ZF_JABU_GOSSIP_STONE,             true),
        LOCATION(RC_ZF_NEAR_JABU_POT_1,               logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_ZF_NEAR_JABU_POT_2,               logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_ZF_NEAR_JABU_POT_3,               logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_ZF_NEAR_JABU_POT_4,               logic->IsChild && logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_ZD_BEHIND_KING_ZORA,       []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_ENTRYWAY, []{return (logic->IsChild && logic->CanUse(RG_BOTTLE_WITH_FISH));}),
        Entrance(RR_ICE_CAVERN_ENTRYWAY,       []{return logic->IsAdult;}),
        Entrance(RR_ZF_GREAT_FAIRY_FOUNTAIN,   []{return logic->HasExplosives() || (ctx->GetTrickOption(RT_ZF_GREAT_FAIRY_WITHOUT_EXPLOSIVES) && logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_SILVER_GAUNTLETS));}),
    });

    areaTable[RR_ZF_GREAT_FAIRY_FOUNTAIN] = Region("ZF Great Fairy Fountain", "ZF Great Fairy Fountain", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_ZF_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Exits
        Entrance(RR_ZORAS_FOUNTAIN, []{return true;}),
    });
}