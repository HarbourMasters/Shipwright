#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_LakeHylia() {
    // clang-format off
    areaTable[RR_LAKE_HYLIA] = Region("Lake Hylia", "Lake Hylia", {RA_LAKE_HYLIA}, DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->GossipStoneFairy, []{return logic->CallGossipFairy();}),
        EventAccess(&logic->BeanPlantFairy,   []{return logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS);}),
        EventAccess(&logic->ButterflyFairy,   []{return logic->ButterflyFairy   || logic->CanUse(RG_STICKS);}),
        EventAccess(&logic->BugShrub,         []{return logic->BugShrub         || (logic->IsChild && logic->CanCutShrubs());}),
        EventAccess(&logic->ChildScarecrow,   []{return logic->ChildScarecrow   || (logic->IsChild && logic->HasItem(RG_FAIRY_OCARINA) && logic->OcarinaButtons() >= 2);}),
        EventAccess(&logic->AdultScarecrow,   []{return logic->AdultScarecrow   || (logic->IsAdult && logic->HasItem(RG_FAIRY_OCARINA) && logic->OcarinaButtons() >= 2);}),
    }, {
        //Locations
        LOCATION(RC_LH_UNDERWATER_ITEM,                  logic->IsChild && logic->HasItem(RG_SILVER_SCALE)),
        LOCATION(RC_LH_SUN,                              logic->IsAdult && ((logic->WaterTempleClear && logic->HasItem(RG_BRONZE_SCALE)) || logic->CanUse(RG_DISTANT_SCARECROW)) && logic->CanUse(RG_FAIRY_BOW)),
        LOCATION(RC_LH_FREESTANDING_POH,                 logic->IsAdult && (logic->CanUse(RG_SCARECROW) || CanPlantBean(RR_LAKE_HYLIA))),
        LOCATION(RC_LH_GS_BEAN_PATCH,                    logic->CanSpawnSoilSkull() && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
        LOCATION(RC_LH_GS_LAB_WALL,                      logic->IsChild && (logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG) || (ctx->GetTrickOption(RT_LH_LAB_WALL_GS) && logic->CanJumpslashExceptHammer())) && logic->CanGetNightTimeGS()),
        LOCATION(RC_LH_GS_SMALL_ISLAND,                  logic->IsChild && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA) && logic->CanGetNightTimeGS() && logic->HasItem(RG_BRONZE_SCALE)),
        LOCATION(RC_LH_GS_TREE,                          logic->IsAdult && logic->CanUse(RG_LONGSHOT) && logic->CanGetNightTimeGS()),
        LOCATION(RC_LH_FRONT_RUPEE,                      logic->IsChild && logic->HasItem(RG_BRONZE_SCALE)),
        LOCATION(RC_LH_MIDDLE_RUPEE,                     logic->IsChild && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS))),
        LOCATION(RC_LH_BACK_RUPEE,                       logic->IsChild && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS))),
        LOCATION(RC_LH_BEAN_SPROUT_FAIRY_1,              logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_LH_BEAN_SPROUT_FAIRY_2,              logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_LH_BEAN_SPROUT_FAIRY_3,              logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_LH_LAB_GOSSIP_STONE_FAIRY,           logic->CallGossipFairy()),
        LOCATION(RC_LH_LAB_GOSSIP_STONE_FAIRY_BIG,       logic->CanUse(RG_SONG_OF_STORMS)),
        //You can walk along the edge of the lake to get these without swimming, the fairy is created going backwards, which is convenient here
        LOCATION(RC_LH_SOUTHEAST_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_LH_SOUTHEAST_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_LH_SOUTHWEST_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_LH_SOUTHWEST_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_LH_ISLAND_SUN_FAIRY,                 logic->CanUse(RG_SUNS_SONG) && ((logic->HasItem(RG_BRONZE_SCALE) && (logic->IsChild || logic->WaterTempleClear)) || logic->CanUse(RG_DISTANT_SCARECROW))),
        LOCATION(RC_LH_LAB_GOSSIP_STONE,                 true),
        LOCATION(RC_LH_SOUTHEAST_GOSSIP_STONE,           true),
        LOCATION(RC_LH_SOUTHWEST_GOSSIP_STONE,           true),
    }, {
        //Exits
        Entrance(RR_HYRULE_FIELD,          []{return true;}),
        Entrance(RR_ZORAS_DOMAIN,          []{return logic->IsChild && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS));}),
        Entrance(RR_LH_OWL_FLIGHT,         []{return logic->IsChild;}),
        Entrance(RR_LH_FISHING_ISLAND,     []{return ((logic->IsChild || logic->WaterTempleClear) && logic->HasItem(RG_BRONZE_SCALE)) || (logic->IsAdult && (logic->CanUse(RG_SCARECROW) || CanPlantBean(RR_LAKE_HYLIA)));}),
        Entrance(RR_LH_LAB,                []{return logic->CanOpenOverworldDoor(RG_HYLIA_LAB_KEY);}),
        Entrance(RR_WATER_TEMPLE_ENTRYWAY, []{return logic->CanUse(RG_HOOKSHOT) && ((logic->CanUse(RG_IRON_BOOTS) || (ctx->GetTrickOption(RT_LH_WATER_HOOKSHOT) && logic->HasItem(RG_GOLDEN_SCALE))) || (logic->IsAdult && logic->CanUse(RG_LONGSHOT) && logic->HasItem(RG_GOLDEN_SCALE)));}),
        Entrance(RR_LH_GROTTO,             []{return true;}),
    });

    areaTable[RR_LH_FISHING_ISLAND] = Region("LH Fishing Island", "Lake Hylia", {RA_LAKE_HYLIA}, DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_LAKE_HYLIA,      []{return logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_LH_FISHING_POND, []{return logic->CanOpenOverworldDoor(RG_FISHING_HOLE_KEY);}),
    });

    areaTable[RR_LH_OWL_FLIGHT] = Region("LH Owl Flight", "Lake Hylia", {RA_LAKE_HYLIA}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_HYRULE_FIELD, []{return true;}, false),
    });

    areaTable[RR_LH_LAB] = Region("LH Lab", "LH Lab", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_LH_LAB_DIVE,        logic->HasItem(RG_GOLDEN_SCALE) || (ctx->GetTrickOption(RT_LH_LAB_DIVING) && logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT))),
        LOCATION(RC_LH_TRADE_FROG,      logic->IsAdult && logic->CanUse(RG_EYEBALL_FROG)),
        LOCATION(RC_LH_GS_LAB_CRATE,    logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT)),
        LOCATION(RC_LH_LAB_FRONT_RUPEE, logic->CanUse(RG_IRON_BOOTS) || logic->HasItem(RG_GOLDEN_SCALE)),
        LOCATION(RC_LH_LAB_LEFT_RUPEE,  logic->CanUse(RG_IRON_BOOTS) || logic->HasItem(RG_GOLDEN_SCALE)),
        LOCATION(RC_LH_LAB_RIGHT_RUPEE, logic->CanUse(RG_IRON_BOOTS) || logic->HasItem(RG_GOLDEN_SCALE)),
    }, {
        //Exits
        Entrance(RR_LAKE_HYLIA, []{return true;}),
    });

    // TODO: should some of these helpers be done via events instead?
    areaTable[RR_LH_FISHING_POND] = Region("LH Fishing Hole", "LH Fishing Hole", {}, DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_LH_CHILD_FISHING,  logic->CanUse(RG_FISHING_POLE) && logic->IsChild),
        LOCATION(RC_LH_CHILD_FISH_1,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_FISH_2,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_FISH_3,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_FISH_4,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_FISH_5,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_FISH_6,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_FISH_7,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_FISH_8,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_FISH_9,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_FISH_10,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_FISH_11,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_FISH_12,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_FISH_13,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_FISH_14,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_FISH_15,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_LOACH_1,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_CHILD_LOACH_2,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || !ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
        LOCATION(RC_LH_ADULT_FISHING,  logic->CanUse(RG_FISHING_POLE) && logic->IsAdult),
        LOCATION(RC_LH_ADULT_FISH_1,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_ADULT_FISH_2,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_ADULT_FISH_3,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_ADULT_FISH_4,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_ADULT_FISH_5,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_ADULT_FISH_6,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_ADULT_FISH_7,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_ADULT_FISH_8,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_ADULT_FISH_9,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_ADULT_FISH_10,  logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_ADULT_FISH_11,  logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_ADULT_FISH_12,  logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_ADULT_FISH_13,  logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_ADULT_FISH_14,  logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_ADULT_FISH_15,  logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_ADULT_LOACH,    logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
        LOCATION(RC_LH_HYRULE_LOACH,   logic->CanUse(RG_FISHING_POLE)),
        LOCATION(RC_FISHING_POLE_HINT, true),
    }, {
        //Exits
        Entrance(RR_LH_FISHING_ISLAND, []{return true;}),
    });

    areaTable[RR_LH_GROTTO] = Region("LH Grotto", "LH Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_LH_DEKU_SCRUB_GROTTO_LEFT,   logic->CanStunDeku()),
        LOCATION(RC_LH_DEKU_SCRUB_GROTTO_RIGHT,  logic->CanStunDeku()),
        LOCATION(RC_LH_DEKU_SCRUB_GROTTO_CENTER, logic->CanStunDeku()),
        LOCATION(RC_LH_GROTTO_BEEHIVE,           logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        Entrance(RR_LAKE_HYLIA, []{return true;}),
    });

    // clang-format on
}
