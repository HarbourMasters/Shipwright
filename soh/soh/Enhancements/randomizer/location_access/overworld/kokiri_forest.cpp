#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_KokiriForest() {
    // clang-format off
    areaTable[RR_KOKIRI_FOREST] = Region("Kokiri Forest", SCENE_KOKIRI_FOREST, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS,                 logic->CallGossipFairyExceptSuns() || (logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_KOKIRI_FOREST_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS))),
        EVENT_ACCESS(LOGIC_SHOWED_MIDO_SWORD_AND_SHIELD, logic->IsChild && logic->HasItem(RG_SPEAK_KOKIRI) && logic->CanUse(RG_KOKIRI_SWORD) && logic->CanUse(RG_DEKU_SHIELD)),
    }, {
        //Locations
        LOCATION(RC_KF_GS_KNOW_IT_ALL_HOUSE,    logic->IsChild && logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_CLOSE) && logic->CanGetNightTimeGS()),
        LOCATION(RC_KF_GS_BEAN_PATCH,           logic->CanSpawnSoilSkull(RG_KOKIRI_FOREST_BEAN_SOUL) && logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_CLOSE)),
        LOCATION(RC_KF_GS_HOUSE_OF_TWINS,       logic->IsAdult && logic->CanGetNightTimeGS() && 
                                                (logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG) || 
                                                 (ctx->GetTrickOption(RT_KF_ADULT_GS) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_SHORT_JUMPSLASH)))),
        LOCATION(RC_KF_BEAN_SPROUT_FAIRY_1,     logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_KOKIRI_FOREST_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_KF_BEAN_SPROUT_FAIRY_2,     logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_KOKIRI_FOREST_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_KF_BEAN_SPROUT_FAIRY_3,     logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->HasItem(RG_KOKIRI_FOREST_BEAN_SOUL) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_KF_BRIDGE_RUPEE,            logic->IsChild),
        LOCATION(RC_KF_BEHIND_MIDOS_RUPEE,      logic->IsChild),
        LOCATION(RC_KF_SOUTH_GRASS_WEST_RUPEE,  logic->IsChild),
        LOCATION(RC_KF_SOUTH_GRASS_EAST_RUPEE,  logic->IsChild),
        LOCATION(RC_KF_NORTH_GRASS_WEST_RUPEE,  logic->IsChild),
        LOCATION(RC_KF_NORTH_GRASS_EAST_RUPEE,  logic->IsChild),
        LOCATION(RC_KF_BEAN_RUPEE_1,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST, RG_KOKIRI_FOREST_BEAN_SOUL) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_KF_BEAN_RUPEE_2,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST, RG_KOKIRI_FOREST_BEAN_SOUL) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_KF_BEAN_RUPEE_3,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST, RG_KOKIRI_FOREST_BEAN_SOUL) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_KF_BEAN_RUPEE_4,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST, RG_KOKIRI_FOREST_BEAN_SOUL) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_KF_BEAN_RUPEE_5,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST, RG_KOKIRI_FOREST_BEAN_SOUL) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_KF_BEAN_RUPEE_6,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST, RG_KOKIRI_FOREST_BEAN_SOUL) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_KF_BEAN_RED_RUPEE,          logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST, RG_KOKIRI_FOREST_BEAN_SOUL) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_KF_SARIAS_ROOF_WEST_HEART,  logic->IsChild),
        LOCATION(RC_KF_SARIAS_ROOF_EAST_HEART,  logic->IsChild),
        LOCATION(RC_KF_SARIAS_ROOF_NORTH_HEART, logic->IsChild),
        LOCATION(RC_KF_CHILD_GRASS_1,           logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_CHILD_GRASS_2,           logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_CHILD_GRASS_3,           logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_CHILD_GRASS_4,           logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_CHILD_GRASS_5,           logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_CHILD_GRASS_6,           logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_CHILD_GRASS_7,           logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_CHILD_GRASS_8,           logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_CHILD_GRASS_9,           logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_CHILD_GRASS_10,          logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_CHILD_GRASS_11,          logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_CHILD_GRASS_12,          logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_1,           logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_2,           logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_3,           logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_4,           logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_5,           logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_6,           logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_7,           logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_8,           logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_9,           logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_10,          logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_11,          logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_12,          logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_13,          logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_14,          logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_15,          logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_16,          logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_17,          logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_18,          logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_19,          logic->IsAdult && logic->CanCutShrubs()),
        LOCATION(RC_KF_ADULT_GRASS_20,          logic->IsAdult && logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_KF_BOULDER_LOOP,       logic->CanUse(RG_CRAWL)),
        ENTRANCE(RR_KF_LINKS_PORCH,        logic->IsChild ? logic->CanClimbLadder() : logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_KF_MIDOS_HOUSE,        true),
        ENTRANCE(RR_KF_SARIAS_HOUSE,       true),
        ENTRANCE(RR_KF_HOUSE_OF_TWINS,     true),
        ENTRANCE(RR_KF_KNOW_IT_ALL_HOUSE,  true),
        ENTRANCE(RR_KF_KOKIRI_SHOP,        true),
        ENTRANCE(RR_KF_OUTSIDE_DEKU_TREE,  (logic->IsAdult && (logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->Get(LOGIC_FOREST_TEMPLE_CLEAR))) || ctx->GetOption(RSK_FOREST).Is(RO_CLOSED_FOREST_OFF) || logic->Get(LOGIC_SHOWED_MIDO_SWORD_AND_SHIELD)),
        ENTRANCE(RR_KF_OUTSIDE_LOST_WOODS, logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST, RG_KOKIRI_FOREST_BEAN_SOUL) || ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS)))),
        ENTRANCE(RR_KF_RUPEE_ALCOVE,       logic->IsAdult && CanPlantBean(RR_KOKIRI_FOREST, RG_KOKIRI_FOREST_BEAN_SOUL)),
        ENTRANCE(RR_LW_BRIDGE_FROM_FOREST, logic->IsAdult || ctx->GetOption(RSK_FOREST).IsNot(RO_CLOSED_FOREST_ON) || logic->Get(LOGIC_DEKU_TREE_CLEAR)),
    });

    areaTable[RR_KF_BOULDER_LOOP] = Region("KF Boulder Loop", SCENE_KOKIRI_FOREST, {}, {
        //Locations
        LOCATION(RC_KF_KOKIRI_SWORD_CHEST, logic->IsChild && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_KF_BOULDER_RUPEE_1,    logic->IsChild),
        LOCATION(RC_KF_BOULDER_RUPEE_2,    logic->IsChild),
        LOCATION(RC_KF_CHILD_GRASS_MAZE_1, logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_CHILD_GRASS_MAZE_2, logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_CHILD_GRASS_MAZE_3, logic->IsChild && logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_KOKIRI_FOREST, logic->CanUse(RG_CRAWL)),
    });

    areaTable[RR_KF_OUTSIDE_DEKU_TREE] = Region("KF Outside Deku Tree", SCENE_KOKIRI_FOREST, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS,                 logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,                   logic->CanGetDekuBabaNuts()),
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS,                 logic->CallGossipFairyExceptSuns()),
        EVENT_ACCESS(LOGIC_SHOWED_MIDO_SWORD_AND_SHIELD, logic->IsChild && logic->HasItem(RG_SPEAK_KOKIRI) && logic->CanUse(RG_KOKIRI_SWORD) && logic->CanUse(RG_DEKU_SHIELD)),
    }, {
        //Locations
        LOCATION(RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE_FAIRY,      logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE_FAIRY_BIG,  logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE_FAIRY,     logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE,            true),
        LOCATION(RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE,           true),
    }, {
        //Exits
        ENTRANCE(RR_DEKU_TREE_ENTRYWAY, logic->IsChild || (ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF) && (ctx->GetOption(RSK_FOREST).Is(RO_CLOSED_FOREST_OFF) || logic->Get(LOGIC_SHOWED_MIDO_SWORD_AND_SHIELD)))),
        ENTRANCE(RR_KOKIRI_FOREST,      (logic->IsAdult && (logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->Get(LOGIC_DEKU_TREE_CLEAR))) || ctx->GetOption(RSK_FOREST).Is(RO_CLOSED_FOREST_OFF) || logic->Get(LOGIC_SHOWED_MIDO_SWORD_AND_SHIELD)),
    });

    areaTable[RR_KF_LINKS_PORCH] = Region("KF Link's Porch", SCENE_KOKIRI_FOREST, {}, {}, {
        ENTRANCE(RR_KOKIRI_FOREST,  true),
        ENTRANCE(RR_KF_LINKS_HOUSE, true),
    });

    areaTable[RR_KF_LINKS_HOUSE] = Region("KF Link's House", SCENE_LINKS_HOUSE, {}, {
        //Locations
        LOCATION(RC_KF_LINKS_HOUSE_COW, logic->IsAdult && logic->CanUse(RG_EPONAS_SONG) && logic->Get(LOGIC_LINKS_COW)),
        LOCATION(RC_KF_LINKS_HOUSE_POT, logic->HasItem(RG_POWER_BRACELET)), // TODO: CanBreakPots() restricted
    }, {
        //Exits
        ENTRANCE(RR_KF_LINKS_PORCH, true),
    });

    areaTable[RR_KF_MIDOS_HOUSE] = Region("KF Mido's House", SCENE_MIDOS_HOUSE, {}, {
        //Locations
        LOCATION(RC_KF_MIDOS_TOP_LEFT_CHEST,     logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_KF_MIDOS_TOP_RIGHT_CHEST,    logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_KF_MIDOS_BOTTOM_LEFT_CHEST,  logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_KF_MIDOS_BOTTOM_RIGHT_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_KOKIRI_FOREST, true),
    });

    areaTable[RR_KF_SARIAS_HOUSE] = Region("KF Saria's House", SCENE_SARIAS_HOUSE, {}, {
        //Locations
        LOCATION(RC_KF_SARIAS_TOP_LEFT_HEART,     true),
        LOCATION(RC_KF_SARIAS_TOP_RIGHT_HEART,    true),
        LOCATION(RC_KF_SARIAS_BOTTOM_LEFT_HEART,  true),
        LOCATION(RC_KF_SARIAS_BOTTOM_RIGHT_HEART, true),
    }, {
        //Exits
        ENTRANCE(RR_KOKIRI_FOREST, true),
    });

    areaTable[RR_KF_HOUSE_OF_TWINS] = Region("KF House of Twins", SCENE_TWINS_HOUSE, {}, {
        //Locations
        LOCATION(RC_KF_TWINS_HOUSE_POT_1, logic->HasItem(RG_POWER_BRACELET)), // TODO: CanBreakPots() restricted
        LOCATION(RC_KF_TWINS_HOUSE_POT_2, logic->HasItem(RG_POWER_BRACELET)), // TODO: CanBreakPots() restricted
    }, {
        //Exits
        ENTRANCE(RR_KOKIRI_FOREST, true),
    });

    areaTable[RR_KF_KNOW_IT_ALL_HOUSE] = Region("KF Know It All House", SCENE_KNOW_IT_ALL_BROS_HOUSE, {}, {
        // Locations
        LOCATION(RC_KF_BROTHERS_HOUSE_POT_1, logic->HasItem(RG_POWER_BRACELET)), // TODO: CanBreakPots() restricted
        LOCATION(RC_KF_BROTHERS_HOUSE_POT_2, logic->HasItem(RG_POWER_BRACELET)), // TODO: CanBreakPots() restricted
    }, {
        //Exits
        ENTRANCE(RR_KOKIRI_FOREST, true),
    });

    areaTable[RR_KF_KOKIRI_SHOP] = Region("KF Kokiri Shop", SCENE_KOKIRI_SHOP, {}, {
        //Locations
        LOCATION(RC_KF_SHOP_ITEM_1, logic->HasItem(RG_SPEAK_KOKIRI) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KF_SHOP_ITEM_2, logic->HasItem(RG_SPEAK_KOKIRI) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KF_SHOP_ITEM_3, logic->HasItem(RG_SPEAK_KOKIRI) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KF_SHOP_ITEM_4, logic->HasItem(RG_SPEAK_KOKIRI) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KF_SHOP_ITEM_5, logic->HasItem(RG_SPEAK_KOKIRI) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KF_SHOP_ITEM_6, logic->HasItem(RG_SPEAK_KOKIRI) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KF_SHOP_ITEM_7, logic->HasItem(RG_SPEAK_KOKIRI) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_KF_SHOP_ITEM_8, logic->HasItem(RG_SPEAK_KOKIRI) && GetCheckPrice() <= GetWalletCapacity()),
    }, {
        //Exits
        ENTRANCE(RR_KOKIRI_FOREST, true),
    });

    areaTable[RR_KF_OUTSIDE_LOST_WOODS] = Region("KF Outside Lost Woods", SCENE_KOKIRI_FOREST, {}, {
        //Locations
        LOCATION(RC_KF_BEAN_RUPEE_1,           logic->IsAdult && logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_KF_BEAN_RUPEE_2,           logic->IsAdult && logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_KF_BEAN_RUPEE_3,           logic->IsAdult && logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_KF_BEAN_RUPEE_4,           logic->IsAdult && logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_KF_BEAN_RUPEE_5,           logic->IsAdult && logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_KF_BEAN_RUPEE_6,           logic->IsAdult && logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_KF_BEAN_RED_RUPEE,         logic->IsAdult && logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_KF_GOSSIP_STONE_FAIRY,     logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_KF_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_KF_GOSSIP_STONE,           true),
    }, {
        //Exits
        ENTRANCE(RR_KOKIRI_FOREST,         true),
        ENTRANCE(RR_THE_LOST_WOODS,        true),
        ENTRANCE(RR_KF_RUPEE_ALCOVE,       logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST, RG_KOKIRI_FOREST_BEAN_SOUL) || logic->CanUse(RG_HOVER_BOOTS))),
        ENTRANCE(RR_KF_STORMS_GROTTO,      logic->CanOpenStormsGrotto()),
    });

    areaTable[RR_KF_RUPEE_ALCOVE] = Region("KF Alcove", SCENE_KOKIRI_FOREST, {}, {
        //Locations
        LOCATION(RC_KF_BEAN_RUPEE_1,   logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)),
        LOCATION(RC_KF_BEAN_RUPEE_2,   logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)),
        LOCATION(RC_KF_BEAN_RUPEE_3,   logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)),
        LOCATION(RC_KF_BEAN_RUPEE_4,   logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)),
        LOCATION(RC_KF_BEAN_RUPEE_5,   logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)),
        LOCATION(RC_KF_BEAN_RUPEE_6,   logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)),
        LOCATION(RC_KF_BEAN_RED_RUPEE, logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)),
    }, {
        ENTRANCE(RR_KOKIRI_FOREST, true),
    });

    areaTable[RR_KF_STORMS_GROTTO] = Region("KF Storms Grotto", SCENE_GROTTOS, grottoEvents, {
        //Locations
        LOCATION(RC_KF_STORMS_GROTTO_CHEST,                  logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_KF_STORMS_GROTTO_FISH,                   logic->HasBottle()),
        LOCATION(RC_KF_STORMS_GROTTO_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_KF_STORMS_GROTTO_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_KF_STORMS_GROTTO_GOSSIP_STONE,           true),
        LOCATION(RC_KF_STORMS_GROTTO_BEEHIVE_LEFT,           logic->CanBreakLowerBeehives()),
        LOCATION(RC_KF_STORMS_GROTTO_BEEHIVE_RIGHT,          logic->CanBreakLowerBeehives()),
        LOCATION(RC_KF_STORMS_GROTTO_GRASS_1,                logic->CanCutShrubs()),
        LOCATION(RC_KF_STORMS_GROTTO_GRASS_2,                logic->CanCutShrubs()),
        LOCATION(RC_KF_STORMS_GROTTO_GRASS_3,                logic->CanCutShrubs()),
        LOCATION(RC_KF_STORMS_GROTTO_GRASS_4,                logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_KF_OUTSIDE_LOST_WOODS, true),
    });

    // clang-format on
}
