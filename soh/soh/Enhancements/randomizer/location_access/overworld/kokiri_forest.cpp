#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_KokiriForest() {
    // clang-format off
    areaTable[RR_KOKIRI_FOREST] = Region("Kokiri Forest", "Kokiri Forest", {RA_KOKIRI_FOREST}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->BeanPlantFairy,           []{return logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS);}),
        EventAccess(&logic->GossipStoneFairy,         []{return logic->CallGossipFairyExceptSuns();}),
        EventAccess(&logic->ShowedMidoSwordAndShield, []{return logic->ShowedMidoSwordAndShield || (logic->IsChild && logic->CanUse(RG_KOKIRI_SWORD) && logic->CanUse(RG_DEKU_SHIELD));}),
    }, {
        //Locations
        LOCATION(RC_KF_KOKIRI_SWORD_CHEST,      logic->IsChild),
        LOCATION(RC_KF_GS_KNOW_IT_ALL_HOUSE,    logic->IsChild && logic->CanAttack() && (/*TODO: HasNightStart ||*/ logic->CanLeaveForest() || logic->CanUse(RG_SUNS_SONG)) && logic->CanGetNightTimeGS()),
        LOCATION(RC_KF_GS_BEAN_PATCH,           logic->CanSpawnSoilSkull() && logic->CanAttack()),
        LOCATION(RC_KF_GS_HOUSE_OF_TWINS,       logic->IsAdult && (logic->HookshotOrBoomerang() || (ctx->GetTrickOption(RT_KF_ADULT_GS) && logic->CanUse(RG_HOVER_BOOTS))) && logic->CanGetNightTimeGS()),
        LOCATION(RC_KF_BEAN_SPROUT_FAIRY_1,     logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_KF_BEAN_SPROUT_FAIRY_2,     logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_KF_BEAN_SPROUT_FAIRY_3,     logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_KF_GOSSIP_STONE_FAIRY,      logic->CallGossipFairyExceptSuns()),
        LOCATION(RC_KF_GOSSIP_STONE_FAIRY_BIG,  logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_KF_BRIDGE_RUPEE,            logic->IsChild),
        LOCATION(RC_KF_BEHIND_MIDOS_RUPEE,      logic->IsChild),
        LOCATION(RC_KF_SOUTH_GRASS_WEST_RUPEE,  logic->IsChild),
        LOCATION(RC_KF_SOUTH_GRASS_EAST_RUPEE,  logic->IsChild),
        LOCATION(RC_KF_NORTH_GRASS_WEST_RUPEE,  logic->IsChild),
        LOCATION(RC_KF_NORTH_GRASS_EAST_RUPEE,  logic->IsChild),
        LOCATION(RC_KF_BOULDER_RUPEE_1,         logic->IsChild),
        LOCATION(RC_KF_BOULDER_RUPEE_2,         logic->IsChild),
        LOCATION(RC_KF_BEAN_RUPEE_1,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_KF_BEAN_RUPEE_2,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_KF_BEAN_RUPEE_3,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_KF_BEAN_RUPEE_4,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_KF_BEAN_RUPEE_5,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_KF_BEAN_RUPEE_6,            logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_KF_BEAN_RED_RUPEE,          logic->IsAdult && (CanPlantBean(RR_KOKIRI_FOREST) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_KF_SARIAS_ROOF_WEST_HEART,  logic->IsChild),
        LOCATION(RC_KF_SARIAS_ROOF_EAST_HEART,  logic->IsChild),
        LOCATION(RC_KF_SARIAS_ROOF_NORTH_HEART, logic->IsChild),
        LOCATION(RC_KF_GOSSIP_STONE,            true),
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
        LOCATION(RC_KF_CHILD_GRASS_MAZE_1,      logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_CHILD_GRASS_MAZE_2,      logic->IsChild && logic->CanCutShrubs()),
        LOCATION(RC_KF_CHILD_GRASS_MAZE_3,      logic->IsChild && logic->CanCutShrubs()),
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
        Entrance(RR_KF_LINKS_HOUSE,        []{return true;}),
        Entrance(RR_KF_MIDOS_HOUSE,        []{return true;}),
        Entrance(RR_KF_SARIAS_HOUSE,       []{return true;}),
        Entrance(RR_KF_HOUSE_OF_TWINS,     []{return true;}),
        Entrance(RR_KF_KNOW_IT_ALL_HOUSE,  []{return true;}),
        Entrance(RR_KF_KOKIRI_SHOP,        []{return true;}),
        Entrance(RR_KF_OUTSIDE_DEKU_TREE,  []{return (logic->IsAdult && (logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->ForestTempleClear)) || ctx->GetOption(RSK_FOREST).Is(RO_CLOSED_FOREST_OFF) || logic->ShowedMidoSwordAndShield;}),
        Entrance(RR_THE_LOST_WOODS,        []{return true;}),
        Entrance(RR_LW_BRIDGE_FROM_FOREST, []{return logic->IsAdult || ctx->GetOption(RSK_FOREST).IsNot(RO_CLOSED_FOREST_ON) || logic->DekuTreeClear;}),
        Entrance(RR_KF_STORMS_GROTTO,      []{return logic->CanOpenStormsGrotto();}),
    });

    areaTable[RR_KF_OUTSIDE_DEKU_TREE] = Region("KF Outside Deku Tree", "Kokiri Forest", {RA_KOKIRI_FOREST}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->DekuBabaSticks,           []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(&logic->DekuBabaNuts,             []{return logic->CanGetDekuBabaNuts();}),
        EventAccess(&logic->ShowedMidoSwordAndShield, []{return logic->ShowedMidoSwordAndShield || (logic->IsChild && logic->CanUse(RG_KOKIRI_SWORD) && logic->CanUse(RG_DEKU_SHIELD));}),
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
        Entrance(RR_DEKU_TREE_ENTRYWAY, []{return logic->IsChild || (ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF) && (ctx->GetOption(RSK_FOREST).Is(RO_CLOSED_FOREST_OFF) || logic->ShowedMidoSwordAndShield));}),
        Entrance(RR_KOKIRI_FOREST,      []{return (logic->IsAdult && (logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->ForestTempleClear)) || ctx->GetOption(RSK_FOREST).Is(RO_CLOSED_FOREST_OFF) || logic->ShowedMidoSwordAndShield;}),
    });

    areaTable[RR_KF_LINKS_HOUSE] = Region("KF Link's House", "KF Link's House", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_KF_LINKS_HOUSE_COW, logic->IsAdult && logic->CanUse(RG_EPONAS_SONG) && logic->LinksCow),
        LOCATION(RC_KF_LINKS_HOUSE_POT, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_KOKIRI_FOREST, []{return true;})
    });

    areaTable[RR_KF_MIDOS_HOUSE] = Region("KF Mido's House", "KF Mido's House", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_KF_MIDOS_TOP_LEFT_CHEST,     true),
        LOCATION(RC_KF_MIDOS_TOP_RIGHT_CHEST,    true),
        LOCATION(RC_KF_MIDOS_BOTTOM_LEFT_CHEST,  true),
        LOCATION(RC_KF_MIDOS_BOTTOM_RIGHT_CHEST, true),
    }, {
        //Exits
        Entrance(RR_KOKIRI_FOREST, []{return true;}),
    });

    areaTable[RR_KF_SARIAS_HOUSE] = Region("KF Saria's House", "KF Saria's House", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_KF_SARIAS_TOP_LEFT_HEART,     true),
        LOCATION(RC_KF_SARIAS_TOP_RIGHT_HEART,    true),
        LOCATION(RC_KF_SARIAS_BOTTOM_LEFT_HEART,  true),
        LOCATION(RC_KF_SARIAS_BOTTOM_RIGHT_HEART, true),
    }, {
        //Exits
        Entrance(RR_KOKIRI_FOREST, []{return true;}),
    });

    areaTable[RR_KF_HOUSE_OF_TWINS] = Region("KF House of Twins", "KF House of Twins", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_KF_TWINS_HOUSE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_KF_TWINS_HOUSE_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_KOKIRI_FOREST, []{return true;}),
    });

    areaTable[RR_KF_KNOW_IT_ALL_HOUSE] = Region("KF Know It All House", "KF Know It All House", {}, NO_DAY_NIGHT_CYCLE, {}, {
        // Locations
        LOCATION(RC_KF_BROTHERS_HOUSE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_KF_BROTHERS_HOUSE_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_KOKIRI_FOREST, []{return true;}),
    });

    areaTable[RR_KF_KOKIRI_SHOP] = Region("KF Kokiri Shop", "KF Kokiri Shop", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_KF_SHOP_ITEM_1, true),
        LOCATION(RC_KF_SHOP_ITEM_2, true),
        LOCATION(RC_KF_SHOP_ITEM_3, true),
        LOCATION(RC_KF_SHOP_ITEM_4, true),
        LOCATION(RC_KF_SHOP_ITEM_5, true),
        LOCATION(RC_KF_SHOP_ITEM_6, true),
        LOCATION(RC_KF_SHOP_ITEM_7, true),
        LOCATION(RC_KF_SHOP_ITEM_8, true),
    }, {
        //Exits
        Entrance(RR_KOKIRI_FOREST, []{return true;}),
    });

    areaTable[RR_KF_STORMS_GROTTO] = Region("KF Storms Grotto", "KF Storms Grotto", {}, NO_DAY_NIGHT_CYCLE, grottoEvents, {
        //Locations
        LOCATION(RC_KF_STORMS_GROTTO_CHEST,                  true),
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
        Entrance(RR_KOKIRI_FOREST, []{return true;})
    });

    // clang-format on
}
