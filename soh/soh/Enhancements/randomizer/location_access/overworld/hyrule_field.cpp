#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_HyruleField() {
    // clang-format off
    areaTable[RR_HYRULE_FIELD] = Region("Hyrule Field", SCENE_HYRULE_FIELD, {
        //Events
        EVENT_ACCESS(LOGIC_BIG_POE_KILL,       logic->HasBottle() && logic->CanUse(RG_FAIRY_BOW) && (logic->SummonEpona() || ctx->GetTrickOption(RT_HF_BIG_POE_WITHOUT_EPONA))),
        EVENT_ACCESS(LOGIC_BORROW_RIGHT_MASKS, logic->IsChild && logic->Get(LOGIC_BORROW_BUNNY_HOOD) && logic->HasItem(RG_KOKIRI_EMERALD) && logic->HasItem(RG_GORON_RUBY) && logic->HasItem(RG_ZORA_SAPPHIRE) && logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_SPEAK_HYLIAN)),
    }, {
        //Locations
        LOCATION(RC_HF_OCARINA_OF_TIME_ITEM,   logic->IsChild && logic->StoneCount() == 3 && logic->HasItem(RG_BRONZE_SCALE)),
        LOCATION(RC_SONG_FROM_OCARINA_OF_TIME, logic->IsChild && logic->StoneCount() == 3 && logic->HasItem(RG_BRONZE_SCALE)),
        LOCATION(RC_HF_POND_STORMS_FAIRY,      logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_HF_CENTRAL_GRASS_1,        logic->CanCutShrubs()),
        LOCATION(RC_HF_CENTRAL_GRASS_2,        logic->CanCutShrubs()),
        LOCATION(RC_HF_CENTRAL_GRASS_3,        logic->CanCutShrubs()),
        LOCATION(RC_HF_CENTRAL_GRASS_4,        logic->CanCutShrubs()),
        LOCATION(RC_HF_CENTRAL_GRASS_5,        logic->CanCutShrubs()),
        LOCATION(RC_HF_CENTRAL_GRASS_6,        logic->CanCutShrubs()),
        LOCATION(RC_HF_CENTRAL_GRASS_7,        logic->CanCutShrubs()),
        LOCATION(RC_HF_CENTRAL_GRASS_8,        logic->CanCutShrubs()),
        LOCATION(RC_HF_CENTRAL_GRASS_9,        logic->CanCutShrubs()),
        LOCATION(RC_HF_CENTRAL_GRASS_10,       logic->CanCutShrubs()),
        LOCATION(RC_HF_CENTRAL_GRASS_11,       logic->CanCutShrubs()),
        LOCATION(RC_HF_CENTRAL_GRASS_12,       logic->CanCutShrubs()),
        LOCATION(RC_HF_SOUTH_GRASS_1,          logic->CanCutShrubs()),
        LOCATION(RC_HF_SOUTH_GRASS_2,          logic->CanCutShrubs()),
        LOCATION(RC_HF_SOUTH_GRASS_3,          logic->CanCutShrubs()),
        LOCATION(RC_HF_SOUTH_GRASS_4,          logic->CanCutShrubs()),
        LOCATION(RC_HF_SOUTH_GRASS_5,          logic->CanCutShrubs()),
        LOCATION(RC_HF_SOUTH_GRASS_6,          logic->CanCutShrubs()),
        LOCATION(RC_HF_SOUTH_GRASS_7,          logic->CanCutShrubs()),
        LOCATION(RC_HF_SOUTH_GRASS_8,          logic->CanCutShrubs()),
        LOCATION(RC_HF_SOUTH_GRASS_9,          logic->CanCutShrubs()),
        LOCATION(RC_HF_SOUTH_GRASS_10,         logic->CanCutShrubs()),
        LOCATION(RC_HF_SOUTH_GRASS_11,         logic->CanCutShrubs()),
        LOCATION(RC_HF_SOUTH_GRASS_12,         logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_MARKET_GRASS_1,    logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_MARKET_GRASS_2,    logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_MARKET_GRASS_3,    logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_MARKET_GRASS_4,    logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_MARKET_GRASS_5,    logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_MARKET_GRASS_6,    logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_MARKET_GRASS_7,    logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_MARKET_GRASS_8,    logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_MARKET_GRASS_9,    logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_MARKET_GRASS_10,   logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_MARKET_GRASS_11,   logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_MARKET_GRASS_12,   logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_KF_GRASS_1,        logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_KF_GRASS_2,        logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_KF_GRASS_3,        logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_KF_GRASS_4,        logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_KF_GRASS_5,        logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_KF_GRASS_6,        logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_KF_GRASS_7,        logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_KF_GRASS_8,        logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_KF_GRASS_9,        logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_KF_GRASS_10,       logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_KF_GRASS_11,       logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_KF_GRASS_12,       logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_LLR_TREE,          logic->CanBonkTrees()),
        LOCATION(RC_HF_NEAR_LH_TREE,           logic->CanBonkTrees()),
        LOCATION(RC_HF_CHILD_NEAR_GV_TREE,     logic->IsChild && logic->CanBonkTrees()),
        LOCATION(RC_HF_ADULT_NEAR_GV_TREE,     logic->IsAdult && logic->CanBonkTrees()),
        LOCATION(RC_HF_NEAR_ZR_TREE,           logic->CanBonkTrees()),
        LOCATION(RC_HF_NEAR_KAK_TREE,          logic->CanBonkTrees()),
        LOCATION(RC_HF_NEAR_KAK_SMALL_TREE,    logic->CanBonkTrees()),
        LOCATION(RC_HF_NEAR_MARKET_TREE_1,     logic->CanBonkTrees()),
        LOCATION(RC_HF_NEAR_MARKET_TREE_2,     logic->CanBonkTrees()),
        LOCATION(RC_HF_NEAR_MARKET_TREE_3,     logic->CanBonkTrees()),
        LOCATION(RC_HF_NORTHWEST_TREE_1,       logic->CanBonkTrees()),
        LOCATION(RC_HF_NORTHWEST_TREE_2,       logic->CanBonkTrees()),
        LOCATION(RC_HF_NORTHWEST_TREE_3,       logic->CanBonkTrees()),
        LOCATION(RC_HF_NORTHWEST_TREE_4,       logic->CanBonkTrees()),
        LOCATION(RC_HF_NORTHWEST_TREE_5,       logic->CanBonkTrees()),
        LOCATION(RC_HF_NORTHWEST_TREE_6,       logic->CanBonkTrees()),
        LOCATION(RC_HF_EAST_TREE_1,            logic->CanBonkTrees()),
        LOCATION(RC_HF_EAST_TREE_2,            logic->CanBonkTrees()),
        LOCATION(RC_HF_EAST_TREE_3,            logic->CanBonkTrees()),
        LOCATION(RC_HF_EAST_TREE_4,            logic->CanBonkTrees()),
        LOCATION(RC_HF_EAST_TREE_5,            logic->CanBonkTrees()),
        LOCATION(RC_HF_EAST_TREE_6,            logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_1,       logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_2,       logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_3,       logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_4,       logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_5,       logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_6,       logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_7,       logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_8,       logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_9,       logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_10,      logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_11,      logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_12,      logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_13,      logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_14,      logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_15,      logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_16,      logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_17,      logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_18,      logic->CanBonkTrees()),
        LOCATION(RC_HF_SOUTHEAST_TREE_19,      logic->CanBonkTrees()),
        LOCATION(RC_HF_CHILD_SOUTHEAST_TREE_1, logic->IsChild && logic->CanBonkTrees()),
        LOCATION(RC_HF_CHILD_SOUTHEAST_TREE_2, logic->IsChild && logic->CanBonkTrees()),
        LOCATION(RC_HF_CHILD_SOUTHEAST_TREE_3, logic->IsChild && logic->CanBonkTrees()),
        LOCATION(RC_HF_CHILD_SOUTHEAST_TREE_4, logic->IsChild && logic->CanBonkTrees()),
        LOCATION(RC_HF_CHILD_SOUTHEAST_TREE_5, logic->IsChild && logic->CanBonkTrees()),
        LOCATION(RC_HF_CHILD_SOUTHEAST_TREE_6, logic->IsChild && logic->CanBonkTrees()),
        LOCATION(RC_HF_TEKTITE_GROTTO_TREE,    logic->CanBonkTrees()),
        LOCATION(RC_HF_BUSH_NEAR_LAKE_1,       true),
        LOCATION(RC_HF_BUSH_NEAR_LAKE_2,       true),
        LOCATION(RC_HF_BUSH_NEAR_LAKE_3,       true),
        LOCATION(RC_HF_BUSH_NEAR_LAKE_4,       true),
        LOCATION(RC_HF_BUSH_NEAR_LAKE_5,       true),
        LOCATION(RC_HF_BUSH_NEAR_LAKE_6,       true),
        LOCATION(RC_HF_BUSH_NEAR_LAKE_7,       true),
        LOCATION(RC_HF_BUSH_NEAR_LAKE_8,       true),
        LOCATION(RC_HF_BUSH_NEAR_LAKE_9,       true),
        LOCATION(RC_HF_BUSH_NEAR_LAKE_10,      true),
        LOCATION(RC_HF_BUSH_NEAR_LAKE_11,      true),
        LOCATION(RC_HF_NORTHERN_BUSH_1,        true),
        LOCATION(RC_HF_NORTHERN_BUSH_2,        true),
        LOCATION(RC_HF_NORTHERN_BUSH_3,        true),
        LOCATION(RC_HF_NORTHERN_BUSH_4,        true),
        LOCATION(RC_HF_NORTHERN_BUSH_5,        true),
        LOCATION(RC_HF_NORTHERN_BUSH_6,        true),
        LOCATION(RC_HF_CHILD_NORTHERN_BUSH_1,  logic->IsChild),
        LOCATION(RC_HF_CHILD_NORTHERN_BUSH_2,  logic->IsChild),
        LOCATION(RC_HF_CHILD_NORTHERN_BUSH_3,  logic->IsChild),
        LOCATION(RC_HF_CHILD_NORTHERN_BUSH_4,  logic->IsChild),
        LOCATION(RC_HF_CHILD_NORTHERN_BUSH_5,  logic->IsChild),
        LOCATION(RC_HF_CHILD_NORTHERN_BUSH_6,  logic->IsChild),
        LOCATION(RC_HF_CHILD_NORTHERN_BUSH_7,  logic->IsChild),
        LOCATION(RC_HF_CHILD_NORTHERN_BUSH_8,  logic->IsChild),
        LOCATION(RC_HF_CHILD_NORTHERN_BUSH_9,  logic->IsChild),
        LOCATION(RC_HF_CHILD_NORTHERN_BUSH_10, logic->IsChild),
        LOCATION(RC_HF_CHILD_NORTHERN_BUSH_11, logic->IsChild),
        LOCATION(RC_HF_BUSH_BY_ROCKY_PATH_1,   true),
        LOCATION(RC_HF_BUSH_BY_ROCKY_PATH_2,   true),
        LOCATION(RC_HF_BUSH_BY_ROCKY_PATH_3,   true),
        LOCATION(RC_HF_BUSH_BY_ROCKY_PATH_4,   true),
        LOCATION(RC_HF_BUSH_BY_ROCKY_PATH_5,   true),
        LOCATION(RC_HF_BUSH_BY_ROCKY_PATH_6,   true),
        LOCATION(RC_HF_SOUTHERN_BUSH_1,        true),
        LOCATION(RC_HF_SOUTHERN_BUSH_2,        true),
        LOCATION(RC_HF_SOUTHERN_BUSH_3,        true),
        LOCATION(RC_HF_SOUTHERN_BUSH_4,        true),
        LOCATION(RC_HF_SOUTHERN_BUSH_5,        true),
        LOCATION(RC_HF_SOUTHERN_BUSH_6,        true),
        LOCATION(RC_HF_SOUTHERN_BUSH_7,        true),
        LOCATION(RC_HF_SOUTHERN_BUSH_8,        true),
        LOCATION(RC_HF_SOUTHERN_BUSH_9,        true),
        LOCATION(RC_HF_SOUTHERN_BUSH_10,       true),
        LOCATION(RC_HF_SOUTHERN_BUSH_11,       true),
        LOCATION(RC_HF_SOUTHERN_BUSH_12,       true),
        LOCATION(RC_HF_CHILD_SOUTHERN_BUSH_1,  logic->IsChild),
        LOCATION(RC_HF_CHILD_SOUTHERN_BUSH_2,  logic->IsChild),
        LOCATION(RC_HF_CHILD_SOUTHERN_BUSH_3,  logic->IsChild),
        LOCATION(RC_HF_CHILD_SOUTHERN_BUSH_4,  logic->IsChild),
        LOCATION(RC_HF_CHILD_SOUTHERN_BUSH_5,  logic->IsChild),
        LOCATION(RC_HF_CHILD_SOUTHERN_BUSH_6,  logic->IsChild),
        LOCATION(RC_HF_CHILD_SOUTHERN_BUSH_7,  logic->IsChild),
        LOCATION(RC_HF_CHILD_SOUTHERN_BUSH_8,  logic->IsChild),
        LOCATION(RC_HF_CHILD_SOUTHERN_BUSH_9,  logic->IsChild),
        LOCATION(RC_HF_CHILD_SOUTHERN_BUSH_10, logic->IsChild),
        LOCATION(RC_HF_CHILD_SOUTHERN_BUSH_11, logic->IsChild),
        LOCATION(RC_HF_CHILD_SOUTHERN_BUSH_12, logic->IsChild),
    }, {
        //Exits
        ENTRANCE(RR_LW_BRIDGE,              true),
        ENTRANCE(RR_GERUDO_VALLEY,          true),
        ENTRANCE(RR_MARKET_ENTRANCE,        true),
        ENTRANCE(RR_KAKARIKO_VILLAGE,       true),
        ENTRANCE(RR_ZR_FRONT,               true),
        ENTRANCE(RR_LON_LON_RANCH,          true),
        ENTRANCE(RR_HF_SOUTHEAST_GROTTO,    AnyAgeTime([]{return logic->BlastOrSmash();})),
        ENTRANCE(RR_HF_TO_LAKE_HYLIA,       logic->CanClimbLadder() || logic->SummonEpona()),
        ENTRANCE(RR_HF_OPEN_GROTTO,         true),
        ENTRANCE(RR_HF_INSIDE_FENCE_GROTTO, logic->CanOpenBombGrotto()),
        ENTRANCE(RR_HF_COW_GROTTO,          (logic->CanUse(RG_MEGATON_HAMMER) || logic->IsChild) && logic->CanOpenBombGrotto()),
        ENTRANCE(RR_HF_NEAR_MARKET_GROTTO,  AnyAgeTime([]{return logic->BlastOrSmash();})),
        ENTRANCE(RR_HF_FAIRY_GROTTO,        AnyAgeTime([]{return logic->BlastOrSmash();})),
        ENTRANCE(RR_HF_NEAR_KAK_GROTTO,     logic->CanOpenBombGrotto()),
        ENTRANCE(RR_HF_TEKTITE_GROTTO,      logic->CanOpenBombGrotto()),
    });

    areaTable[RR_HF_TO_LAKE_HYLIA] = Region("HF to Lake Hylia", SCENE_HYRULE_FIELD, {}, {}, {
        //Exits
        ENTRANCE(RR_LAKE_HYLIA,   true),
        ENTRANCE(RR_HYRULE_FIELD, logic->CanClimbLadder() || logic->SummonEpona()),
    });

    areaTable[RR_HF_SOUTHEAST_GROTTO] = Region("HF Southeast Grotto", SCENE_GROTTOS, grottoEvents, {
        //Locations
        LOCATION(RC_HF_SOUTHEAST_GROTTO_CHEST,                  logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_HF_SOUTHEAST_GROTTO_FISH,                   logic->HasBottle()),
        LOCATION(RC_HF_SOUTHEAST_GROTTO_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_HF_SOUTHEAST_GROTTO_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_HF_SOUTHEAST_GROTTO_GOSSIP_STONE,           true),
        LOCATION(RC_HF_SOUTHEAST_GROTTO_BEEHIVE_LEFT,           logic->CanBreakLowerBeehives()),
        LOCATION(RC_HF_SOUTHEAST_GROTTO_BEEHIVE_RIGHT,          logic->CanBreakLowerBeehives()),
        LOCATION(RC_HF_SOUTHEAST_GROTTO_GRASS_1,                logic->CanCutShrubs()),
        LOCATION(RC_HF_SOUTHEAST_GROTTO_GRASS_2,                logic->CanCutShrubs()),
        LOCATION(RC_HF_SOUTHEAST_GROTTO_GRASS_3,                logic->CanCutShrubs()),
        LOCATION(RC_HF_SOUTHEAST_GROTTO_GRASS_4,                logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_HYRULE_FIELD, true),
    });

    areaTable[RR_HF_OPEN_GROTTO] = Region("HF Open Grotto", SCENE_GROTTOS, grottoEvents, {
        //Locations
        LOCATION(RC_HF_OPEN_GROTTO_CHEST,                   logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_HF_OPEN_GROTTO_FISH,                    logic->HasBottle()),
        LOCATION(RC_HF_OPEN_GROTTO_GOSSIP_STONE_FAIRY,      logic->CallGossipFairy()),
        LOCATION(RC_HF_OPEN_GROTTO_GOSSIP_STONE_FAIRY_BIG,  logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_HF_OPEN_GROTTO_GOSSIP_STONE,            true),
        LOCATION(RC_HF_OPEN_GROTTO_BEEHIVE_LEFT,            logic->CanBreakLowerBeehives()),
        LOCATION(RC_HF_OPEN_GROTTO_BEEHIVE_RIGHT,           logic->CanBreakLowerBeehives()),
        LOCATION(RC_HF_OPEN_GROTTO_GRASS_1,                 logic->CanCutShrubs()),
        LOCATION(RC_HF_OPEN_GROTTO_GRASS_2,                 logic->CanCutShrubs()),
        LOCATION(RC_HF_OPEN_GROTTO_GRASS_3,                 logic->CanCutShrubs()),
        LOCATION(RC_HF_OPEN_GROTTO_GRASS_4,                 logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_HYRULE_FIELD, true),
    });

    areaTable[RR_HF_INSIDE_FENCE_GROTTO] = Region("HF Inside Fence Grotto", SCENE_GROTTOS, {}, {
        //Locations
        LOCATION(RC_HF_DEKU_SCRUB_GROTTO,           logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_HF_INSIDE_FENCE_GROTTO_BEEHIVE, logic->CanBreakLowerBeehives()),
        LOCATION(RC_HF_FENCE_GROTTO_STORMS_FAIRY,   logic->CanUse(RG_SONG_OF_STORMS)),
    }, {
        //Exits
        ENTRANCE(RR_HYRULE_FIELD, true),
    });

    areaTable[RR_HF_COW_GROTTO] = Region("HF Cow Grotto", SCENE_GROTTOS, {}, {}, {
        //Exits
        ENTRANCE(RR_HYRULE_FIELD,              true),
        ENTRANCE(RR_HF_COW_GROTTO_BEHIND_WEBS, logic->HasFireSource()),
    });

    areaTable[RR_HF_COW_GROTTO_BEHIND_WEBS] = Region("HF Cow Grotto Behind Webs", SCENE_GROTTOS, {
        //Events
        EVENT_ACCESS(LOGIC_BUG_ACCESS,   logic->CanCutShrubs()),
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CallGossipFairy()),
    }, {
        //Locations
        LOCATION(RC_HF_GS_COW_GROTTO,                     logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
        LOCATION(RC_HF_COW_GROTTO_COW,                    logic->CanUse(RG_EPONAS_SONG)),
        LOCATION(RC_HF_COW_GROTTO_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_HF_COW_GROTTO_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_HF_COW_GROTTO_GOSSIP_STONE,           true),
        LOCATION(RC_HF_COW_GROTTO_POT_1,                  logic->CanBreakPots()),
        LOCATION(RC_HF_COW_GROTTO_POT_2,                  logic->CanBreakPots()),
        LOCATION(RC_HF_COW_GROTTO_GRASS_1,                logic->CanCutShrubs()),
        LOCATION(RC_HF_COW_GROTTO_GRASS_2,                logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_HF_COW_GROTTO, true),
    });

    areaTable[RR_HF_NEAR_MARKET_GROTTO] = Region("HF Near Market Grotto", SCENE_GROTTOS, grottoEvents, {
        //Locations
        LOCATION(RC_HF_NEAR_MARKET_GROTTO_CHEST,                  logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_HF_NEAR_MARKET_GROTTO_FISH,                   logic->HasBottle()),
        LOCATION(RC_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE,           true),
        LOCATION(RC_HF_NEAR_MARKET_GROTTO_BEEHIVE_LEFT,           logic->CanBreakLowerBeehives()),
        LOCATION(RC_HF_NEAR_MARKET_GROTTO_BEEHIVE_RIGHT,          logic->CanBreakLowerBeehives()),
        LOCATION(RC_HF_NEAR_MARKET_GROTTO_GRASS_1,                logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_MARKET_GROTTO_GRASS_2,                logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_MARKET_GROTTO_GRASS_3,                logic->CanCutShrubs()),
        LOCATION(RC_HF_NEAR_MARKET_GROTTO_GRASS_4,                logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_HYRULE_FIELD, true),
    });

    areaTable[RR_HF_FAIRY_GROTTO] = Region("HF Fairy Grotto", SCENE_GROTTOS, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, true),
    }, {
        //Locations
        LOCATION(RC_HF_FAIRY_GROTTO_FAIRY_1, true),
        LOCATION(RC_HF_FAIRY_GROTTO_FAIRY_2, true),
        LOCATION(RC_HF_FAIRY_GROTTO_FAIRY_3, true),
        LOCATION(RC_HF_FAIRY_GROTTO_FAIRY_4, true),
        LOCATION(RC_HF_FAIRY_GROTTO_FAIRY_5, true),
        LOCATION(RC_HF_FAIRY_GROTTO_FAIRY_6, true),
        LOCATION(RC_HF_FAIRY_GROTTO_FAIRY_7, true),
        LOCATION(RC_HF_FAIRY_GROTTO_FAIRY_8, true),
    }, {
        //Exits
        ENTRANCE(RR_HYRULE_FIELD, true),
    });

    areaTable[RR_HF_NEAR_KAK_GROTTO] = Region("HF Near Kak Grotto", SCENE_GROTTOS, {}, {
        //Locations
        LOCATION(RC_HF_GS_NEAR_KAK_GROTTO, logic->HookshotOrBoomerang()),
    }, {
        //Exits
        ENTRANCE(RR_HYRULE_FIELD, true),
    });

    areaTable[RR_HF_TEKTITE_GROTTO] = Region("HF Tektite Grotto", SCENE_GROTTOS, {}, {
        //Locations
        LOCATION(RC_HF_TEKTITE_GROTTO_FREESTANDING_POH, logic->HasItem(RG_GOLDEN_SCALE) || logic->CanUse(RG_IRON_BOOTS)),
    }, {
        //Exits
        ENTRANCE(RR_HYRULE_FIELD, true),
    });

    // clang-format on
}
