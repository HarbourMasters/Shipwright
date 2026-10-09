#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

#define CAN_TRAVERSE_WAIST_DEEP_WATER (CAN_USE_ABILITY(SWIM) || CAN_BE_DEKU || CAN_BE_ZORA || CAN_BE_GORON)

inline bool CanGetPastBigOctoWithoutBoat() {
    /*
     * Kill the Big Octo. This means the water is still in a poison state, so swimming is not logically considered due
     * to the damage boost method not being logically considered. This piece may need expanded as tricks and glitches
     * are logically introduced.
     */
    if (RANDO_EVENTS[RE_SOUTHERN_SWAMP_KILL_OCTOROK] && CAN_BE_DEKU) {
        return true;
    }
    /*
     * The water is clear in this state, so swimming comes into question. Either Link uses the swim ability, he hops
     * over the water as Deku, or he uses one of the forms tall enough to walk through the water. This may also be
     * expanded if FD is ever logically considered.
     */
    if (RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE] && CAN_TRAVERSE_WAIST_DEEP_WATER) {
        return true;
    }
    return false;
}

inline bool CanGetPastBigOcto() {
    // Boat ride. This takes Link to a landing in the next region and thus has no additional requirements.
    if (RANDO_EVENTS[RE_SOUTHERN_SWAMP_RIDE_BOAT]) {
        return true;
    }

    return CanGetPastBigOctoWithoutBoat();
}

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_DEKU_KINGS_CHAMBER_HOLDING_CELL] = RandoRegion{ .name = "Holding Cell", .sceneId = SCENE_DEKU_KING,
        .checks = {
            CHECK(RC_DEKU_KINGS_CHAMBER_MONKEY, CAN_BE_DEKU && HAS_ITEM(ITEM_OCARINA_OF_TIME)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(DEKU_PALACE, 3),                  ENTRANCE(DEKU_KINGS_CHAMBER, 1), true),
        },
    };
    Regions[RR_DEKU_KINGS_CHAMBER] = RandoRegion{ .sceneId = SCENE_DEKU_KING,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(DEKU_PALACE, 2),                  ENTRANCE(DEKU_KINGS_CHAMBER, 0), true),
        },
        .events = {
            EVENT(RE_ACCESS_PICTOGRAPH_DEKU_KING, HAS_ITEM(ITEM_PICTOGRAPH_BOX) && CAN_BE_DEKU),
            EVENT(RE_RETURN_DEKU_PRINCESS, HAS_BOTTLE && CAN_ACCESS(DEKU_PRINCESS)),
        }
    };
    Regions[RR_DEKU_PALACE_BEAN_SALESMAN_GROTTO] = RandoRegion{ .name = "Deku Palace Bean Salesman Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_DEKU_PALACE_GROTTO_CHEST, CAN_GROW_BEAN_PLANT || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_GRASS_01, true),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_GRASS_02, true),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_GRASS_03, true),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_GRASS_04, true),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_GRASS_05, true),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_GRASS_06, true),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_GRASS_07, true),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_GRASS_08, true),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_GRASS_09, true),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_GRASS_10, true),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_GRASS_11, true),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_GRASS_12, true),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_BUTTERFLY_01, HAS_ITEM(ITEM_DEKU_STICK)),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_BUTTERFLY_02, HAS_ITEM(ITEM_DEKU_STICK)),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_BUTTERFLY_03, HAS_ITEM(ITEM_DEKU_STICK)),
            CHECK(RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_BUTTERFLY_04, HAS_ITEM(ITEM_DEKU_STICK)),
            // TODO: Bean salesman check
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(DEKU_PALACE, 9),                  ENTRANCE(GROTTOS, 12), true),
        },
    };
    Regions[RR_DEKU_PALACE_INSIDE_LOWER] = RandoRegion{ .name = "Inside, Lower", .sceneId = SCENE_22DEKUCITY,
        .checks = {
            CHECK(RC_DEKU_PALACE_PIECE_OF_HEART,    true),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_01, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_02, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_03, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_04, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_05, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_06, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_07, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_08, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_09, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_10, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_11, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_12, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_13, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_14, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_15, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_16, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_17, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_18, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_19, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_20, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_21, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_22, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_23, CAN_BE_DEKU),
            CHECK(RC_DEKU_PALACE_FREESTANDING_RUPEE_24, CAN_BE_DEKU),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(DEKU_KINGS_CHAMBER, 0),           ENTRANCE(DEKU_PALACE, 2), true),
            EXIT(ENTRANCE(GROTTOS, 12),                     ENTRANCE(DEKU_PALACE, 9), true),
        },
        .connections = {
            CONNECTION(RR_DEKU_PALACE_OUTSIDE, true),
        },
    };
    Regions[RR_DEKU_PALACE_INSIDE_UPPER_CELL_SIDE_LEDGE] = RandoRegion{ .name = "Inside, Upper", .sceneId = SCENE_22DEKUCITY,
        // TODO: Region exists for entrance rando
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(DEKU_KINGS_CHAMBER, 1),           ENTRANCE(DEKU_PALACE, 3), true), // Cell
        },
        .connections = {
            CONNECTION(RR_DEKU_PALACE_INSIDE_UPPER_CELL_SIDE, CanKillEnemy(ACTOR_EN_DEKUNUTS) && CAN_BE_DEKU),
            CONNECTION(RR_DEKU_PALACE_INSIDE_LOWER, true),
        },
    };
    // TODO: There is a trick you can do to get up to the dekunut with a backflip onto the doorway and then an angled roll jump to grab the upper ledge
    Regions[RR_DEKU_PALACE_INSIDE_UPPER_CELL_SIDE] = RandoRegion{ .name = "Inside, Upper", .sceneId = SCENE_22DEKUCITY,
        .checks = {
            CHECK(RC_ENEMY_DROP_MAD_SCRUB, CanKillEnemy(ACTOR_EN_DEKUNUTS) && CAN_BE_DEKU),
        },
        .connections = {
            CONNECTION(RR_DEKU_PALACE_INSIDE_UPPER_CELL_SIDE_LEDGE, CanKillEnemy(ACTOR_EN_DEKUNUTS) && CAN_BE_DEKU),
            CONNECTION(RR_DEKU_PALACE_INSIDE_UPPER_MIDDLE, CanKillEnemy(ACTOR_EN_DEKUNUTS) && CAN_BE_DEKU),
            CONNECTION(RR_DEKU_PALACE_INSIDE_LOWER, true),
        },
    };
    Regions[RR_DEKU_PALACE_INSIDE_UPPER_MIDDLE] = RandoRegion { .name = "Inside, Upper", .sceneId = SCENE_22DEKUCITY,
        .checks = {
            CHECK(RC_DEKU_PALACE_POT_01, true),
            CHECK(RC_DEKU_PALACE_POT_02, true),
        },
        .connections = {
            CONNECTION(RR_DEKU_PALACE_INSIDE_UPPER_CELL_SIDE, CanKillEnemy(ACTOR_EN_DEKUNUTS) && CAN_BE_DEKU),
            CONNECTION(RR_DEKU_PALACE_INSIDE_UPPER_BEAN_SIDE, CanKillEnemy(ACTOR_EN_DEKUNUTS) && CAN_BE_DEKU),
            CONNECTION(RR_DEKU_PALACE_INSIDE_LOWER, true),
        }
    };
    Regions[RR_DEKU_PALACE_INSIDE_UPPER_BEAN_SIDE] = RandoRegion { .name = "Inside, Upper", .sceneId = SCENE_22DEKUCITY,
        .checks = {
            CHECK(RC_ENEMY_DROP_MAD_SCRUB, CanKillEnemy(ACTOR_EN_DEKUNUTS) && CAN_BE_DEKU),
        },
        .connections = {
            CONNECTION(RR_DEKU_PALACE_OUTSIDE, CanKillEnemy(ACTOR_EN_DEKUNUTS) && CAN_BE_DEKU),
            CONNECTION(RR_DEKU_PALACE_INSIDE_UPPER_MIDDLE, CanKillEnemy(ACTOR_EN_DEKUNUTS) && CAN_BE_DEKU),
            CONNECTION(RR_DEKU_PALACE_INSIDE_LOWER, true),
        }
    };
    Regions[RR_DEKU_PALACE_OUTSIDE] = RandoRegion{ .name = "Outside", .sceneId = SCENE_22DEKUCITY,
        .checks = {
            CHECK(RC_ENEMY_DROP_MINI_BABA, (CAN_BE_DEKU || (RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE] && CAN_TRAVERSE_WAIST_DEEP_WATER)) && CanKillEnemy(ACTOR_EN_KAREBABA)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 3),      ENTRANCE(DEKU_PALACE, 0), true),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 4),      ENTRANCE(DEKU_PALACE, 5), CAN_BE_DEKU), // Treetop
            EXIT(ENTRANCE(DEKU_SHRINE, 0),                  ENTRANCE(DEKU_PALACE, 4), RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE] && CAN_TRAVERSE_WAIST_DEEP_WATER),
        },
        .connections = {
            CONNECTION(RR_DEKU_PALACE_INSIDE_LOWER, CAN_BE_DEKU),
            CONNECTION(RR_DEKU_PALACE_INSIDE_UPPER_BEAN_SIDE, (CAN_BE_DEKU || (RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE] && CAN_TRAVERSE_WAIST_DEEP_WATER)) && CAN_USE_DAY2_RAIN_BEAN),
        },
    };
    Regions[RR_DEKU_SHRINE_ENTRANCE] = RandoRegion{ .name = "Entrance", .sceneId = SCENE_DANPEI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(DEKU_PALACE, 4),                  ENTRANCE(DEKU_SHRINE, 0), true),
        },
        .connections = {
            CONNECTION(RR_DEKU_SHRINE, RANDO_EVENTS[RE_RETURN_DEKU_PRINCESS]),
        },
    };
    Regions[RR_DEKU_SHRINE] = RandoRegion{ .sceneId = SCENE_DANPEI,
        .checks = {
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_01,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_02,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_03,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_04,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_05,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_06,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_07,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_08,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_09,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_10,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_11,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_12,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_13,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_14,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_15,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_16,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_17,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_18,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_19,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_20,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_21,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_22,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_23,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_24,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_25,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_26,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_27,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_28,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_29,             true),
            CHECK(RC_DEKU_SHRINE_FREESTANDING_RUPEE_30,             true),
            CHECK(RC_DEKU_SHRINE_POT_01,                            true),
            CHECK(RC_DEKU_SHRINE_POT_02,                            true),
            CHECK(RC_DEKU_SHRINE_MASK_OF_SCENTS,                    true),
        },
        .connections = {
            CONNECTION(RR_DEKU_SHRINE_ENTRANCE, RANDO_EVENTS[RE_RETURN_DEKU_PRINCESS]),
        },
    };
    Regions[RR_MAGIC_HAGS_POTION_SHOP] = RandoRegion{ .sceneId = SCENE_WITCH_SHOP,
        .checks = {
            CHECK(RC_HAGS_POTION_SHOP_FREESTANDING_RUPEE, true),
            // TODO: Add CAN_ACCESS(MUSHROOM) once that is shuffled.
            CHECK(RC_HAGS_POTION_SHOP_ITEM_01, (FIRST_DAY() || RANDO_EVENTS[RE_SAVED_KOUME]) && CAN_AFFORD(RC_HAGS_POTION_SHOP_ITEM_01) && HAS_ITEM(ITEM_MASK_SCENTS) && HAS_BOTTLE),
            CHECK(RC_HAGS_POTION_SHOP_ITEM_02, (FIRST_DAY() || RANDO_EVENTS[RE_SAVED_KOUME]) && CAN_AFFORD(RC_HAGS_POTION_SHOP_ITEM_02)),
            CHECK(RC_HAGS_POTION_SHOP_ITEM_03, (FIRST_DAY() || RANDO_EVENTS[RE_SAVED_KOUME]) && CAN_AFFORD(RC_HAGS_POTION_SHOP_ITEM_03)),
            CHECK(RC_HAGS_POTION_SHOP_KOTAKE, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 5),      ENTRANCE(MAGIC_HAGS_POTION_SHOP, 0), true),
        },
        .events = {
            EVENT(RE_ACCESS_RED_POTION_REFILL, true),
        },
    };
    Regions[RR_ROAD_TO_SOUTHERN_SWAMP_GROTTO] = RandoRegion{ .name = "Road to Southern Swamp Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_CHEST, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_GRASS_01, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_GRASS_02, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_GRASS_03, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_GRASS_04, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_GRASS_05, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_GRASS_06, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_GRASS_07, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_GRASS_08, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_GRASS_09, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_GRASS_10, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_GRASS_11, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_GRASS_12, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_GRASS_13, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_GRASS_14, true),
            CHECK(RC_ENEMY_DROP_MINI_BABA, CanKillEnemy(ACTOR_EN_KAREBABA)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 0),       ENTRANCE(GROTTOS, 20), true),
        },
    };
    Regions[RR_ROAD_TO_SOUTHERN_SWAMP] = RandoRegion{ .sceneId = SCENE_24KEMONOMITI,
        .checks = {
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_PIECE_OF_HEART, CanKillEnemy(ACTOR_EN_BAT) && (CAN_USE_PROJECTILE || HAS_ITEM(ITEM_BOMBCHU))),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_01, CAN_USE_PROJECTILE && CAN_AFFORD(RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_01)),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_02, CAN_USE_PROJECTILE && CAN_AFFORD(RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_02)),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_01, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_02, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_03, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_04, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_05, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_06, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_07, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_08, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_09, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_10, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_11, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_12, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_13, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_14, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_15, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_16, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_17, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_18, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_19, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_20, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_TREE_01, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_TREE_02, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_TREE_03, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_TREE_04, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_TREE_05, true),
            CHECK(RC_ROAD_TO_SOUTHERN_SWAMP_TREE_06, true),
            CHECK(RC_ENEMY_DROP_DEKU_BABA, CanKillEnemy(ACTOR_EN_DEKUBABA)),
            CHECK(RC_ENEMY_DROP_CHUCHU, CanKillEnemy(ACTOR_EN_SLIME) && IS_DAY()), // Day only
            CHECK(RC_ENEMY_DROP_WOLFOS, CanKillEnemy(ACTOR_EN_WF) && IS_NIGHT()), // Night only
            CHECK(RC_ENEMY_DROP_BAD_BAT, CanKillEnemy(ACTOR_EN_BAT)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 1),                ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 0), true),
            EXIT(ENTRANCE(GROTTOS, 20),                     ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 0), true),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 0),      ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 1), true),
            EXIT(ENTRANCE(SWAMP_SHOOTING_GALLERY, 0),       ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 2), BEFORE(TIME_NIGHT1_PM_10_00) || BETWEEN(TIME_DAY2_AM_06_00, TIME_NIGHT2_PM_10_00) || BETWEEN(TIME_DAY3_AM_06_00, TIME_NIGHT3_PM_10_00)),
        },
        .events = {
            EVENT(RE_ACCESS_PICTOGRAPH_TINGLE, HAS_ITEM(ITEM_PICTOGRAPH_BOX)),
            EVENT(RE_ACCESS_SPRING_WATER, true),
        },
    };
    Regions[RR_SOUTHERN_SWAMP_GROTTO] = RandoRegion{ .name = "Southern Swamp Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_CHEST, true),
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_GRASS_01, true),
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_GRASS_02, true),
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_GRASS_03, true),
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_GRASS_04, true),
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_GRASS_05, true),
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_GRASS_06, true),
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_GRASS_07, true),
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_GRASS_08, true),
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_GRASS_09, true),
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_GRASS_10, true),
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_GRASS_11, true),
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_GRASS_12, true),
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_GRASS_13, true),
            CHECK(RC_SOUTHERN_SWAMP_GROTTO_GRASS_14, true),
            CHECK(RC_ENEMY_DROP_MINI_BABA, CanKillEnemy(ACTOR_EN_KAREBABA)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 3),      ENTRANCE(GROTTOS, 21), true),
        },
    };
    Regions[RR_SOUTHERN_SWAMP_NORTH] = RandoRegion{ .name = "North Tourist Section", .sceneId = SCENE_20SICHITAI,
        .checks = {
            CHECK(RC_SOUTHERN_SWAMP_POISON_TOURIST_GRASS_01, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_TOURIST_GRASS_02, true),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_TOURIST_GRASS_01, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_TOURIST_GRASS_02, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_PIECE_OF_HEART, CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND)),
            CHECK(RC_SOUTHERN_SWAMP_SCRUB_DEED, Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND)),
            CHECK(RC_SOUTHERN_SWAMP_SCRUB_BEANS, CAN_BE_DEKU),
            CHECK(RC_SOUTHERN_SWAMP_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_01, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_02, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_03, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_04, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_05, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_06, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_07, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_08, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_09, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_10, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_11, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_12, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_01, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_02, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_03, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_04, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_05, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_06, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_07, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_08, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_09, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_10, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_11, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_12, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_BEEHIVE, CAN_USE_PROJECTILE && CAN_TRAVERSE_WAIST_DEEP_WATER),
            CHECK(RC_ENEMY_DROP_MINI_BABA, CanKillEnemy(ACTOR_EN_KAREBABA)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 1),       ENTRANCE(SOUTHERN_SWAMP_POISONED, 0), true),
            EXIT(ENTRANCE(TOURIST_INFORMATION, 0),          ENTRANCE(SOUTHERN_SWAMP_POISONED, 1), true),
        },
        .connections = {
            CONNECTION(RR_SOUTHERN_SWAMP_SOUTH, CanGetPastBigOcto()), // Via Octo near Tourist Center
    		CONNECTION(RR_SOUTHERN_SWAMP_NEAR_FLOWERS, CAN_TRAVERSE_WAIST_DEEP_WATER),
    		CONNECTION(RR_SOUTHERN_SWAMP_NEAR_WOODS, CAN_TRAVERSE_WAIST_DEEP_WATER),
        },
        .events = {
            EVENT(RE_ACCESS_SPRING_WATER, true),
            EVENT(RE_ACCESS_BEANS_REFILL, CAN_BE_DEKU && HAS_ITEM(ITEM_MAGIC_BEANS)),
            EVENT(RE_SOUTHERN_SWAMP_KILL_OCTOROK, (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA)),
            EVENT(RE_ACCESS_PICTOGRAPH_SWAMP_GENERIC, HAS_ITEM(ITEM_PICTOGRAPH_BOX)),
        },
        .oneWayEntrances = {
            ENTRANCE(SOUTHERN_SWAMP_POISONED, 10), // From Song of Soaring
        }
    };
    Regions[RR_SOUTHERN_SWAMP_NEAR_FLOWERS] = RandoRegion{ .name = "Flowers Section", .sceneId = SCENE_20SICHITAI,
        .checks = {
            CHECK(RC_SOUTHERN_SWAMP_FROG, HAS_ITEM(ITEM_MASK_DON_GERO)),
            CHECK(RC_SOUTHERN_SWAMP_FREESTANDING_RUPEE_01, CAN_BE_DEKU || CAN_BE_ZORA),
            CHECK(RC_SOUTHERN_SWAMP_FREESTANDING_RUPEE_02, CAN_BE_DEKU || CAN_BE_ZORA),
            CHECK(RC_ENEMY_DROP_OCTOROK, CanKillEnemy(ACTOR_EN_OKUTA) && (CAN_USE_ABILITY(SWIM) || CAN_BE_ZORA || CAN_BE_GORON)),
        },
        .connections = {
            CONNECTION(RR_SOUTHERN_SWAMP_SOUTH, CanGetPastBigOctoWithoutBoat()),
    		CONNECTION(RR_SOUTHERN_SWAMP_NORTH, CAN_TRAVERSE_WAIST_DEEP_WATER),
            CONNECTION(RR_SOUTHERN_SWAMP_NEAR_WOODS, CAN_TRAVERSE_WAIST_DEEP_WATER),
        },
        .events = {
            // Any form but Deku can bottle water here. Human can do it from a lilypad before it sinks.
            // If human is ever shuffled, revisit this.
            EVENT(RE_ACCESS_SPRING_WATER, true),
            EVENT(RE_SOUTHERN_SWAMP_KILL_OCTOROK, (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA)),
            EVENT(RE_ACCESS_PICTOGRAPH_SWAMP_GENERIC, HAS_ITEM(ITEM_PICTOGRAPH_BOX)),
        },
    };
    Regions[RR_SOUTHERN_SWAMP_NEAR_WOODS] = RandoRegion{ .name = "North Woods Section", .sceneId = SCENE_20SICHITAI,
        .checks = {
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_WOODS_GRASS_01, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_WOODS_GRASS_02, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_POISON_WOODS_GRASS_01, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_WOODS_GRASS_02, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_POT_01, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_POT_02, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_POT_03, true),
            CHECK(RC_SOUTHERN_SWAMP_CLEAR_POT_01, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEAR_POT_02, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEAR_POT_03, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_13, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_14, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_15, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_16, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_17, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_18, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_19, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_20, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_21, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_22, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_23, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_24, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_25, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_26, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_27, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_28, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_29, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_CLEARED_GRASS_30, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_13, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_14, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_15, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_16, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_17, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_18, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_19, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_20, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_21, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_22, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_23, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_24, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_25, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_26, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_27, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_28, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_29, true),
            CHECK(RC_SOUTHERN_SWAMP_POISON_GRASS_30, true),
            CHECK(RC_ENEMY_DROP_MINI_BABA, CanKillEnemy(ACTOR_EN_KAREBABA)),
            CHECK(RC_ENEMY_DROP_DEKU_BABA, CanKillEnemy(ACTOR_EN_DEKUBABA)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MAGIC_HAGS_POTION_SHOP, 0),       ENTRANCE(SOUTHERN_SWAMP_POISONED, 5), true),
            EXIT(ENTRANCE(WOODS_OF_MYSTERY, 0),             ENTRANCE(SOUTHERN_SWAMP_POISONED, 7), true),
        },
        .connections = {
            CONNECTION(RR_SOUTHERN_SWAMP_NEAR_FLOWERS, CAN_TRAVERSE_WAIST_DEEP_WATER),
    		CONNECTION(RR_SOUTHERN_SWAMP_NORTH, CAN_TRAVERSE_WAIST_DEEP_WATER),
        },
        .events = {
            EVENT(RE_ACCESS_SPRING_WATER, true),
            EVENT(RE_ACCESS_PICTOGRAPH_SWAMP_GENERIC, HAS_ITEM(ITEM_PICTOGRAPH_BOX)),
        },
        .oneWayEntrances = {
            ENTRANCE(SOUTHERN_SWAMP_POISONED, 9), // From river in Ikana
        }
    };
    Regions[RR_SOUTHERN_SWAMP_SOUTH] = RandoRegion{ .name = "South Section", .sceneId = SCENE_20SICHITAI,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GROTTOS, 21),                     ENTRANCE(SOUTHERN_SWAMP_POISONED, 3), CAN_BE_DEKU || (RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE] && CAN_TRAVERSE_WAIST_DEEP_WATER)),
            EXIT(ENTRANCE(DEKU_PALACE, 0),                  ENTRANCE(SOUTHERN_SWAMP_POISONED, 3), true),
            EXIT(ENTRANCE(SWAMP_SPIDER_HOUSE, 0),           ENTRANCE(SOUTHERN_SWAMP_POISONED, 8), CAN_LIGHT_TORCH_NEAR_ANOTHER && (CAN_BE_DEKU || (RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE] && (CAN_USE_ABILITY(SWIM) || CAN_BE_ZORA)))),
        },
        .connections = {
            CONNECTION(RR_SOUTHERN_SWAMP_NORTH, CanGetPastBigOcto()),
            CONNECTION(RR_SOUTHERN_SWAMP_NEAR_FLOWERS, CanGetPastBigOctoWithoutBoat()),
            CONNECTION(RR_SOUTHERN_SWAMP_SOUTH_UPPER, RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE] && CAN_TRAVERSE_WAIST_DEEP_WATER), // Climbable vines, only in cleared swamp
        },
        .events = {
            EVENT(RE_ACCESS_PICTOGRAPH_SWAMP_GENERIC, HAS_ITEM(ITEM_PICTOGRAPH_BOX))
        }
    };
    Regions[RR_SOUTHERN_SWAMP_SOUTH_UPPER] = RandoRegion{ .name = "Upper South Section", .sceneId = SCENE_20SICHITAI,
        .checks = {
            CHECK(RC_SOUTHERN_SWAMP_SONG_OF_SOARING, CAN_BE_DEKU),
            CHECK(RC_ENEMY_DROP_DRAGONFLY, CanKillEnemy(ACTOR_EN_GRASSHOPPER)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WOODFALL, 0),                     ENTRANCE(SOUTHERN_SWAMP_POISONED, 2), CAN_BE_DEKU),
            EXIT(ENTRANCE(DEKU_PALACE, 5),                  ENTRANCE(SOUTHERN_SWAMP_POISONED, 4), CAN_BE_DEKU), // Treetop
        },
        .connections = {
            CONNECTION(RR_SOUTHERN_SWAMP_SOUTH, true),
        },
        .events = {
            EVENT(RE_ACCESS_PICTOGRAPH_SWAMP_GENERIC, HAS_ITEM(ITEM_PICTOGRAPH_BOX)) //Adding here to future proof for later when map shuffle is in.
        }
    };
    Regions[RR_SWAMP_SHOOTING_GALLERY] = RandoRegion{ .sceneId = SCENE_SYATEKI_MORI,
        .checks = {
            CHECK(RC_SWAMP_SHOOTING_GALLERY_HIGH_SCORE, HAS_ITEM(ITEM_BOW)),
            CHECK(RC_SWAMP_SHOOTING_GALLERY_PERFECT_SCORE, HAS_ITEM(ITEM_BOW)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 2),       ENTRANCE(SWAMP_SHOOTING_GALLERY, 0), true),
        },
        .timeStayRestrictions = {
            STAY(TIME_NIGHT1_PM_10_00, false),
            STAY(TIME_NIGHT2_PM_10_00, false),
            STAY(TIME_NIGHT3_PM_10_00, false),
        },
    };
    Regions[RR_TOURIST_INFORMATION] = RandoRegion{ .sceneId = SCENE_MAP_SHOP,
        .checks = {
            // Also requires poison to not be cleared
            CHECK(RC_TOURIST_INFORMATION_ARCHERY, RANDO_EVENTS[RE_SAVED_KOUME] && RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
            CHECK(RC_TOURIST_INFORMATION_GOOD_PHOTO, CAN_ACCESS(PICTOGRAPH_TINGLE) || CAN_ACCESS(PICTOGRAPH_DEKU_KING)),
            CHECK(RC_TOURIST_INFORMATION_PICTOBOX, RANDO_EVENTS[RE_SAVED_KOUME]),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 1),      ENTRANCE(TOURIST_INFORMATION, 0), true),
        },
        .events = {
            EVENT(RE_SOUTHERN_SWAMP_RIDE_BOAT, RANDO_EVENTS[RE_SAVED_KOUME] || CAN_ACCESS(PICTOGRAPH_SWAMP_GENERIC)),
        },
    };
    Regions[RR_WOODFALL_GREAT_FAIRY_FOUNTAIN] = RandoRegion{ .name = "Woodfall", .sceneId = SCENE_YOUSEI_IZUMI,
        .checks = {
            CHECK(RC_WOODFALL_GREAT_FAIRY, HAS_ENOUGH_STRAY_FAIRIES(DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WOODFALL, 2),                     ENTRANCE(FAIRY_FOUNTAIN, 1), true),
        },
    };
    Regions[RR_WOODFALL] = RandoRegion{ .sceneId = SCENE_21MITURINMAE,
        .checks = {
            CHECK(RC_WOODFALL_ENTRANCE_CHEST, CAN_BE_DEKU || (RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE] && (CAN_BE_ZORA || CAN_BE_GORON || CAN_USE_ABILITY(SWIM)))),
            CHECK(RC_WOODFALL_PIECE_OF_HEART_CHEST, CAN_BE_DEKU && (CanKillEnemy(ACTOR_EN_DEKUNUTS) || HAS_ITEM(ITEM_HOOKSHOT))),
            CHECK(RC_WOODFALL_FREESTANDING_RUPEE, CAN_BE_DEKU && CanKillEnemy(ACTOR_EN_DEKUNUTS)),
            CHECK(RC_WOODFALL_GRASS_01, true),
            CHECK(RC_WOODFALL_GRASS_02, true),
            CHECK(RC_WOODFALL_GRASS_03, true),
            CHECK(RC_WOODFALL_GRASS_04, true),
            CHECK(RC_WOODFALL_GRASS_05, true),
            CHECK(RC_WOODFALL_GRASS_06, true),
            CHECK(RC_ENEMY_DROP_DRAGONFLY, CanKillEnemy(ACTOR_EN_GRASSHOPPER)),
            CHECK(RC_ENEMY_DROP_HIPLOOP, CanKillEnemy(ACTOR_EN_PP)),
            CHECK(RC_ENEMY_DROP_MAD_SCRUB, CanKillEnemy(ACTOR_EN_DEKUNUTS)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 2),      ENTRANCE(WOODFALL, 0), true),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 1),               ENTRANCE(WOODFALL, 2), CAN_BE_DEKU && CanKillEnemy(ACTOR_EN_DEKUNUTS)),
            EXIT(ENTRANCE(WOODFALL_TEMPLE, 2),              ENTRANCE(WOODFALL, 3), RANDO_EVENTS[RE_CLEARED_WOODFALL_TEMPLE]),
        },
        .connections = {
            CONNECTION(RR_WOODFALL_OWL_STATUE_PLATFORM, CAN_BE_DEKU && CanKillEnemy(ACTOR_EN_DEKUNUTS)),
        },
    };
    Regions[RR_WOODFALL_OWL_STATUE_PLATFORM] = RandoRegion{ .name = "Owl Statue Platform", .sceneId = SCENE_21MITURINMAE,
        .checks = {
            CHECK(RC_WOODFALL_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_WOODFALL_NEAR_OWL_CHEST, CAN_BE_DEKU),
            CHECK(RC_WOODFALL_POT_01, true),
            CHECK(RC_WOODFALL_POT_02, true),
            CHECK(RC_WOODFALL_POT_03, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WOODFALL_TEMPLE, 0),              ENTRANCE(WOODFALL, 1), CAN_BE_DEKU && CanAccessDungeon(DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE)),
        },
        .connections = {
            CONNECTION(RR_WOODFALL, CAN_BE_DEKU),
        },
        .oneWayEntrances = {
            ENTRANCE(WOODFALL, 4), // From Song of Soaring
        }
    };
    Regions[RR_WOODS_OF_MYSTERY_GROTTO] = RandoRegion{ .name = "Woods of Mystery Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_CHEST, true),
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_GRASS_01, true),
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_GRASS_02, true),
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_GRASS_03, true),
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_GRASS_04, true),
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_GRASS_05, true),
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_GRASS_06, true),
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_GRASS_07, true),
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_GRASS_08, true),
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_GRASS_09, true),
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_GRASS_10, true),
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_GRASS_11, true),
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_GRASS_12, true),
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_GRASS_13, true),
            CHECK(RC_WOODS_OF_MYSTERY_GROTTO_GRASS_14, true),
            CHECK(RC_ENEMY_DROP_MINI_BABA, CanKillEnemy(ACTOR_EN_KAREBABA)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WOODS_OF_MYSTERY, 0),             ENTRANCE(GROTTOS, 22), true),
        },
    };
    Regions[RR_WOODS_OF_MYSTERY] = RandoRegion{ .sceneId = SCENE_26SARUNOMORI,
        .checks = {
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_01, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_02, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_03, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_04, FIRST_DAY()),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_05, FIRST_DAY()),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_06, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_07, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_08, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_09, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_10, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_11, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_12, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_13, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_14, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_15, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_16, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_17, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_18, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_19, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_20, true),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_21, SECOND_DAY()),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_22, FINAL_DAY()),
            CHECK(RC_WOODS_OF_MYSTERY_GRASS_23, FINAL_DAY()),
            CHECK(RC_ENEMY_DROP_SNAPPER, CAN_BE_DEKU && CanKillEnemy(ACTOR_EN_KAME)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GROTTOS, 22),                     ENTRANCE(WOODS_OF_MYSTERY, 0), SECOND_DAY()),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 7),      ENTRANCE(WOODS_OF_MYSTERY, 0), true),
        },
        .events = {
            EVENT(RE_SAVED_KOUME, HAS_BOTTLE && (CAN_ACCESS(RED_POTION_REFILL) || CAN_ACCESS(BLUE_POTION_REFILL))),
        },
    };
}, {});
// clang-format on
