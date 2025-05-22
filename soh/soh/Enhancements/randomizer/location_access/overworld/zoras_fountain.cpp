#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_ZorasFountain() {
    // clang-format off
    areaTable[RR_ZORAS_FOUNTAIN] = Region("Zoras Fountain", "Zoras Fountain", {RA_ZORAS_FOUNTAIN}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->GossipStoneFairy, []{return logic->CallGossipFairyExceptSuns();}),
        EventAccess(&logic->ButterflyFairy,   []{return logic->CanUse(RG_STICKS) && logic->AtDay;}),
    }, {
        //Locations
        LOCATION(RC_ZF_GS_TREE,                      logic->IsChild),
        LOCATION(RC_ZF_GS_ABOVE_THE_LOG,             logic->IsChild && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
        LOCATION(RC_ZF_FAIRY_GOSSIP_STONE_FAIRY,     logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_ZF_FAIRY_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_ZF_JABU_GOSSIP_STONE_FAIRY,      logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_ZF_JABU_GOSSIP_STONE_FAIRY_BIG,  logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_ZF_FAIRY_GOSSIP_STONE,           true),
        LOCATION(RC_ZF_JABU_GOSSIP_STONE,            true),
        LOCATION(RC_ZF_NEAR_JABU_POT_1,              logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_ZF_NEAR_JABU_POT_2,              logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_ZF_NEAR_JABU_POT_3,              logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_ZF_NEAR_JABU_POT_4,              logic->IsChild && logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_ZD_BEHIND_KING_ZORA,       []{return true;}),
        Entrance(RR_ZF_ICEBERGS,               []{return logic->IsAdult;}),
        Entrance(RR_ZF_LAKEBED,                []{return logic->CanUse(RG_IRON_BOOTS);}),
        //child can break the brown rock without lifting the silver rock and it stays gone for adult, but it's not intuitive and there's no reasonable case where it matters.
        Entrance(RR_ZF_HIDDEN_CAVE,            []{return logic->CanUse(RG_SILVER_GAUNTLETS) && logic->BlastOrSmash();}),
        Entrance(RR_ZF_ROCK,                   []{return logic->IsAdult && logic->CanUse(RG_SCARECROW);}),
        Entrance(RR_JABU_JABUS_BELLY_ENTRYWAY, []{return (logic->IsChild && logic->CanUse(RG_BOTTLE_WITH_FISH));}),
        Entrance(RR_ZF_GREAT_FAIRY_FOUNTAIN,   []{return logic->HasExplosives() || (ctx->GetTrickOption(RT_ZF_GREAT_FAIRY_WITHOUT_EXPLOSIVES) && logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_SILVER_GAUNTLETS));}),
    });

    areaTable[RR_ZF_ICEBERGS] = Region("ZF Icebergs", "Zoras Fountain", {RA_ZORAS_FOUNTAIN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_ZF_ICEBERG_FREESTANDING_POH, logic->IsAdult),
    }, {
        //Exits
        //This hover is pretty tight, come at it with momentum and aim for the small corner polygon of the big iceburg while spamming roll
        Entrance(RR_ZORAS_FOUNTAIN, []{return logic->HasItem(RG_BRONZE_SCALE) || logic->HasItem(RG_HOVER_BOOTS);}),
        Entrance(RR_ZF_LAKEBED,     []{return logic->CanUse(RG_IRON_BOOTS);}),
        Entrance(RR_ZF_LEDGE,       []{return true;}),
    });

    areaTable[RR_ZF_LAKEBED] = Region("ZF Lakebed", "Zoras Fountain", {RA_ZORAS_FOUNTAIN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_ZF_BOTTOM_FREESTANDING_POH,       logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16),
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
    }, {
        //Exits
        Entrance(RR_ZORAS_FOUNTAIN, []{return logic->HasItem(RG_BRONZE_SCALE);}),
    });

    areaTable[RR_ZF_LEDGE] = Region("ZF Ledge", "Zoras Fountain", {RA_ZORAS_FOUNTAIN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_ZORAS_FOUNTAIN,      []{return logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_ZF_ICEBERGS,         []{return logic->IsAdult;}),
        Entrance(RR_ZF_LAKEBED,          []{return logic->CanUse(RG_IRON_BOOTS);}),
        Entrance(RR_ICE_CAVERN_ENTRYWAY, []{return true;}),
    });

    areaTable[RR_ZF_HIDDEN_CAVE] = Region("ZF Hidden Cave", "Zoras Fountain", {RA_ZORAS_FOUNTAIN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_ZF_HIDDEN_CAVE_POT_1, logic->IsAdult && logic->CanBreakPots()),
        LOCATION(RC_ZF_HIDDEN_CAVE_POT_2, logic->IsAdult && logic->CanBreakPots()),
        LOCATION(RC_ZF_HIDDEN_CAVE_POT_3, logic->IsAdult && logic->CanBreakPots()),
    }, {
        //Exits
        //There are invisible big skultullas here as adult but they do not block the path and can be "seen" with Z-target
        //Lens is not currently needed for this either, implying they are not considered blocking, but it's open for discussion long-term
        Entrance(RR_ZF_HIDDEN_LEDGE, []{return true;}),
    });

    areaTable[RR_ZF_HIDDEN_LEDGE] = Region("ZF Hidden Ledge", "Zoras Fountain", {RA_ZORAS_FOUNTAIN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_ZF_GS_HIDDEN_CAVE, logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOMB_THROW) && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        //It is possible to avoid fall damage by jumping towards the right and landing in deeper water, but this is basically never relevent
        Entrance(RR_ZORAS_FOUNTAIN, []{return logic->HasItem(RG_BRONZE_SCALE) || logic->TakeDamage();}),
        Entrance(RR_ZF_HIDDEN_CAVE, []{return true;}),
    });

    areaTable[RR_ZF_ROCK] = Region("ZF Rock", "Zoras Fountain", {RA_ZORAS_FOUNTAIN}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        //Has a wonder item
    }, {
        //Exits
        Entrance(RR_ZORAS_FOUNTAIN, []{return true;}),
    });

    areaTable[RR_ZF_GREAT_FAIRY_FOUNTAIN] = Region("ZF Great Fairy Fountain", "ZF Great Fairy Fountain", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_ZF_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Exits
        Entrance(RR_ZORAS_FOUNTAIN, []{return true;}),
    });

    // clang-format on
}
