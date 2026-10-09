#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

// TODO: This is a temporary fix to ensure cows can only be accessed if they can reach the ranch
// on day 1 or beat the aliens. This will not work for entrance rando when that comes.
#define BREAK_BOULDER_BEFORE_OR_BEAT_ALIENS_DAY                                                          \
    ((BETWEEN(TIME_DAY1_AM_06_00, TIME_NIGHT1_PM_06_00) && CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG)) || \
     RANDO_EVENTS[RE_COWS_FROM_ALIENS])
#define BREAK_BOULDER_BEFORE_OR_BEAT_ALIENS_NIGHT                                                          \
    ((BETWEEN(TIME_NIGHT1_PM_06_00, TIME_NIGHT1_AM_02_30) && CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG)) || \
     RANDO_EVENTS[RE_COWS_FROM_ALIENS])

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_CUCCO_SHACK] = RandoRegion{ .sceneId = SCENE_F01C,
        .checks = {
            CHECK(RC_ROMANI_RANCH_GROG, HAS_ITEM(ITEM_MASK_BREMEN)),
            CHECK(RC_CUCCO_SHACK_LARGE_CRATE_01, true),
            CHECK(RC_CUCCO_SHACK_LARGE_CRATE_02, true),
            CHECK(RC_CUCCO_SHACK_LARGE_CRATE_03, true),
            CHECK(RC_CUCCO_SHACK_TREE, true),
            CHECK(RC_CUCCO_SHACK_WONDER_ITEM_01, CAN_USE_SWORD || CAN_BE_GORON || CAN_USE_PROJECTILE || CAN_USE_EXPLOSIVE || HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_DEKU_NUT)),
            CHECK(RC_CUCCO_SHACK_WONDER_ITEM_02, CAN_USE_SWORD || CAN_BE_GORON || CAN_USE_PROJECTILE || CAN_USE_EXPLOSIVE || HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_DEKU_NUT)),
            CHECK(RC_CUCCO_SHACK_WONDER_ITEM_03, CAN_USE_SWORD || CAN_BE_GORON || CAN_USE_PROJECTILE || CAN_USE_EXPLOSIVE || HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_DEKU_NUT)),
            CHECK(RC_CUCCO_SHACK_WONDER_ITEM_04, CAN_USE_SWORD || CAN_BE_GORON || CAN_USE_PROJECTILE || CAN_USE_EXPLOSIVE || HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_DEKU_NUT)),
            CHECK(RC_CUCCO_SHACK_WONDER_ITEM_05, CAN_USE_SWORD || CAN_BE_GORON || CAN_USE_PROJECTILE || CAN_USE_EXPLOSIVE || HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_DEKU_NUT)),
            CHECK(RC_CUCCO_SHACK_WONDER_ITEM_06, CAN_USE_SWORD || CAN_BE_GORON || CAN_USE_PROJECTILE || CAN_USE_EXPLOSIVE || HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_DEKU_NUT)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 4),                 ENTRANCE(CUCCO_SHACK, 0), true),
        },
        .timeStayRestrictions = {
            STAY(TIME_NIGHT1_PM_08_00, false),
            STAY(TIME_NIGHT2_PM_08_00, false),
            STAY(TIME_NIGHT3_PM_08_00, false),
        },
    };
    Regions[RR_DOGGY_RACETRACK] = RandoRegion{ .sceneId = SCENE_F01_B,
        .checks = {
            // TODO: Trick: Jumpslash to clip through (similar to Clock Town Straw).
            // Zora can just climb up, adding it to logic for now but if someone wants to make it a trick later feel free.
            CHECK(RC_DOGGY_RACETRACK_CHEST, HAS_ITEM(ITEM_HOOKSHOT) || CAN_USE_DAY2_RAIN_BEAN || CAN_BE_ZORA),
            CHECK(RC_DOGGY_RACETRACK_PIECE_OF_HEART,    HAS_ITEM(ITEM_MASK_TRUTH)),
            CHECK(RC_DOGGY_RACETRACK_POT_01, true),
            CHECK(RC_DOGGY_RACETRACK_POT_02, true),
            CHECK(RC_DOGGY_RACETRACK_POT_03, true),
            CHECK(RC_DOGGY_RACETRACK_POT_04, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 5),                 ENTRANCE(DOGGY_RACETRACK, 0), true),
        },
        .timeStayRestrictions = {
            STAY(TIME_NIGHT1_PM_08_00, false),
            STAY(TIME_NIGHT2_PM_08_00, false),
            STAY(TIME_NIGHT3_PM_08_00, false),
        },
    };
    Regions[RR_GORMAN_TRACK_FRONT] = RandoRegion{ .sceneId = SCENE_KOEPONARACE,
        .checks = {
            CHECK(RC_GORMAN_MILK_PURCHASE, CAN_AFFORD(RC_GORMAN_MILK_PURCHASE) && IS_DAY()),
            CHECK(RC_GORMAN_TRACK_GARO_MASK, CAN_PLAY_SONG(EPONA) && IS_DAY()),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MILK_ROAD, 3),                    ENTRANCE(GORMAN_TRACK, 0), true),
        },
        .connections = {
            // TODO: Also apparently can be reached using a trick with Goron mask and Bombs. Add trick later here
            CONNECTION(RR_GORMAN_TRACK, RANDO_EVENTS[RE_COWS_FROM_ALIENS] && IS_NIGHT2()),
        },
    };
    Regions[RR_GORMAN_TRACK] = RandoRegion{ .sceneId = SCENE_KOEPONARACE,
        .checks = {
            // The grass is technically reachable while racing on Epona, but successfully picking up the drops can be
            // dubious. We can make this a trick in the future. For now, gate the entire region behind saving the ranch
            // from aliens.
            CHECK(RC_GORMAN_TRACK_LARGE_CRATE, true), 
            CHECK(RC_GORMAN_TRACK_GRASS_01, true),
            CHECK(RC_GORMAN_TRACK_GRASS_02, true),
            CHECK(RC_GORMAN_TRACK_GRASS_03, true),
            CHECK(RC_GORMAN_TRACK_GRASS_04, true),
            CHECK(RC_GORMAN_TRACK_GRASS_05, true),
            CHECK(RC_GORMAN_TRACK_GRASS_06, true),
            CHECK(RC_GORMAN_TRACK_GRASS_07, true),
            CHECK(RC_GORMAN_TRACK_GRASS_08, true),
            CHECK(RC_GORMAN_TRACK_GRASS_09, true),
            CHECK(RC_GORMAN_TRACK_GRASS_10, true),
            CHECK(RC_GORMAN_TRACK_GRASS_11, true),
            CHECK(RC_GORMAN_TRACK_GRASS_12, true),
            CHECK(RC_GORMAN_TRACK_GRASS_13, true),
            CHECK(RC_GORMAN_TRACK_GRASS_14, true),
            CHECK(RC_GORMAN_TRACK_GRASS_15, true),
            CHECK(RC_GORMAN_TRACK_GRASS_16, true),
            CHECK(RC_GORMAN_TRACK_GRASS_17, true),
            CHECK(RC_GORMAN_TRACK_GRASS_18, true),
            CHECK(RC_GORMAN_TRACK_GRASS_19, true),
            CHECK(RC_GORMAN_TRACK_GRASS_20, true),
            CHECK(RC_GORMAN_TRACK_GRASS_21, true),
            CHECK(RC_GORMAN_TRACK_GRASS_22, true),
            CHECK(RC_GORMAN_TRACK_GRASS_23, true),
            CHECK(RC_GORMAN_TRACK_GRASS_24, true),
        },
        .connections = {
            CONNECTION(RR_GORMAN_TRACK_FRONT, CAN_PLAY_SONG(EPONA) || (RANDO_EVENTS[RE_COWS_FROM_ALIENS] && IS_NIGHT2())),
            CONNECTION(RR_GORMAN_TRACK_BACK, CAN_PLAY_SONG(EPONA) || (RANDO_EVENTS[RE_COWS_FROM_ALIENS] && IS_NIGHT2())),
        },
    };
    Regions[RR_GORMAN_TRACK_BACK] = RandoRegion{ .sceneId = SCENE_KOEPONARACE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MILK_ROAD, 2),                    ENTRANCE(GORMAN_TRACK, 3), true),
        },
        .connections = {
            CONNECTION(RR_GORMAN_TRACK, RANDO_EVENTS[RE_COWS_FROM_ALIENS] && IS_NIGHT2()),
        },
    };
    Regions[RR_MILK_ROAD] = RandoRegion{ .sceneId = SCENE_ROMANYMAE,
        .checks = {
            CHECK(RC_MILK_ROAD_GRASS_01, true),
            CHECK(RC_MILK_ROAD_GRASS_02, true),
            CHECK(RC_MILK_ROAD_GRASS_03, true),
            CHECK(RC_MILK_ROAD_KEATON_GRASS_01, true),
            CHECK(RC_MILK_ROAD_KEATON_GRASS_02, true),
            CHECK(RC_MILK_ROAD_KEATON_GRASS_03, true),
            CHECK(RC_MILK_ROAD_KEATON_GRASS_04, true),
            CHECK(RC_MILK_ROAD_KEATON_GRASS_05, true),
            CHECK(RC_MILK_ROAD_KEATON_GRASS_06, true),
            CHECK(RC_MILK_ROAD_KEATON_GRASS_07, true),
            CHECK(RC_MILK_ROAD_KEATON_GRASS_08, true),
            CHECK(RC_MILK_ROAD_KEATON_GRASS_09, true),
            CHECK(RC_MILK_ROAD_KEATON_GRASS_WONDER_ITEM, CAN_FULLY_CUT_KEATON_GRASS),
            CHECK(RC_KEATON_QUIZ, HAS_ITEM(ITEM_MASK_KEATON)),
            CHECK(RC_MILK_ROAD_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_MILK_ROAD_TINGLE_MAP_01, CAN_USE_PROJECTILE && CAN_AFFORD(RC_MILK_ROAD_TINGLE_MAP_01)),
            CHECK(RC_MILK_ROAD_TINGLE_MAP_02, CAN_USE_PROJECTILE && CAN_AFFORD(RC_MILK_ROAD_TINGLE_MAP_02)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 5),                ENTRANCE(MILK_ROAD, 0), true),
            EXIT(ENTRANCE(ROMANI_RANCH, 0),                 ENTRANCE(MILK_ROAD, 1), AFTER(TIME_DAY3_AM_06_00) || RANDO_EVENTS[RE_DESTROY_MILK_ROAD_BOULDER]),
            EXIT(ENTRANCE(GORMAN_TRACK, 0),                 ENTRANCE(MILK_ROAD, 3), true),
        },
        .connections = {
            // TODO: Trick to Goron bomb jump over the fence
            CONNECTION(RR_MILK_ROAD_BEHIND_FENCE, (RANDO_EVENTS[RE_COWS_FROM_ALIENS] && IS_NIGHT2()) || FINAL_DAY()),
        },
        .events = {
            EVENT(RE_ACCESS_PICTOGRAPH_TINGLE, HAS_ITEM(ITEM_PICTOGRAPH_BOX)),
            EVENT(RE_DESTROY_MILK_ROAD_BOULDER, CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG)),
        },
        .oneWayEntrances = {
            ENTRANCE(MILK_ROAD, 4), // From Song of Soaring
        }
    };
    Regions[RR_MILK_ROAD_BEHIND_FENCE] = RandoRegion{ .sceneId = SCENE_ROMANYMAE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GORMAN_TRACK, 3),                 ENTRANCE(MILK_ROAD, 2), true),
        },
        .connections = {
            // TODO: Trick to Goron bomb jump over the fence
            CONNECTION(RR_MILK_ROAD, (RANDO_EVENTS[RE_COWS_FROM_ALIENS] && IS_NIGHT2()) || FINAL_DAY()),
        },
    };
    Regions[RR_RANCH_BARN] = RandoRegion{ .sceneId = SCENE_OMOYA,
        .checks = {
            CHECK(RC_ROMANI_RANCH_BARN_COW_LEFT, CAN_PLAY_SONG(EPONA) && BREAK_BOULDER_BEFORE_OR_BEAT_ALIENS_NIGHT),
            CHECK(RC_ROMANI_RANCH_BARN_COW_MIDDLE, CAN_PLAY_SONG(EPONA) && BREAK_BOULDER_BEFORE_OR_BEAT_ALIENS_NIGHT),
            CHECK(RC_ROMANI_RANCH_BARN_COW_RIGHT, CAN_PLAY_SONG(EPONA) && BREAK_BOULDER_BEFORE_OR_BEAT_ALIENS_NIGHT),
            CHECK(RC_ROMANI_RANCH_BARN_WONDER_ITEM_01, true),
            CHECK(RC_ROMANI_RANCH_BARN_WONDER_ITEM_02, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 2),                 ENTRANCE(RANCH_HOUSE, 0), true),
        },
        .timeStayRestrictions = {
            STAY(TIME_NIGHT1_AM_02_30, false),
            STAY(TIME_NIGHT3_PM_08_00, false),
        },
    };
    Regions[RR_RANCH_HOUSE] = RandoRegion{ .sceneId = SCENE_OMOYA,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROMANI_RANCH, 3),                 ENTRANCE(RANCH_HOUSE, 1), true),
        },
        .timeStayRestrictions = {
            STAY(TIME_NIGHT1_PM_08_00, false),
            STAY(TIME_NIGHT2_PM_08_00, false),
            STAY(TIME_NIGHT3_PM_08_00, false),
        },
    };
    Regions[RR_ROMANI_RANCH] = RandoRegion{ .sceneId = SCENE_F01,
        .checks = {
            // TODO: Time logic is wrong for entrance rando
            CHECK(RC_ROMANI_RANCH_ALIENS, CanKillEnemy(ACTOR_EN_INVADEPOH) && IS_NIGHT1() && CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG)),
            // TODO: Time logic is wrong for entrance rando
            CHECK(RC_ROMANI_RANCH_EPONAS_SONG, BEFORE(TIME_NIGHT1_PM_06_00) && CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG)),
            CHECK(RC_ROMANI_RANCH_FIELD_COW_ENTRANCE, CAN_PLAY_SONG(EPONA) && BREAK_BOULDER_BEFORE_OR_BEAT_ALIENS_DAY),
            CHECK(RC_ROMANI_RANCH_FIELD_COW_NEAR_HOUSE_BACK, CAN_PLAY_SONG(EPONA) && BREAK_BOULDER_BEFORE_OR_BEAT_ALIENS_DAY),
            CHECK(RC_ROMANI_RANCH_FIELD_COW_NEAR_HOUSE_FRONT, CAN_PLAY_SONG(EPONA) && BREAK_BOULDER_BEFORE_OR_BEAT_ALIENS_DAY),
            CHECK(RC_ROMANI_RANCH_FIELD_LARGE_CRATE, true),
            CHECK(RC_CREMIA_ESCORT, RANDO_EVENTS[RE_COWS_FROM_ALIENS] && AT(TIME_NIGHT2_PM_06_00)),
            CHECK(RC_ROMANI_RANCH_GRASS_01, true),
            CHECK(RC_ROMANI_RANCH_GRASS_02, true),
            CHECK(RC_ROMANI_RANCH_GRASS_03, true),
            CHECK(RC_ROMANI_RANCH_GRASS_04, true),
            CHECK(RC_ROMANI_RANCH_GRASS_05, true),
            CHECK(RC_ROMANI_RANCH_GRASS_06, true),
            CHECK(RC_ROMANI_RANCH_GRASS_07, true),
            CHECK(RC_ROMANI_RANCH_GRASS_08, true),
            CHECK(RC_ROMANI_RANCH_GRASS_09, true),
            CHECK(RC_ROMANI_RANCH_GRASS_10, true),
            CHECK(RC_ROMANI_RANCH_GRASS_11, true),
            CHECK(RC_ROMANI_RANCH_GRASS_12, true),
            CHECK(RC_ROMANI_RANCH_GRASS_13, true),
            CHECK(RC_ROMANI_RANCH_GRASS_14, true),
            CHECK(RC_ROMANI_RANCH_GRASS_15, true),
            CHECK(RC_ROMANI_RANCH_GRASS_16, true),
            CHECK(RC_ROMANI_RANCH_GRASS_17, true),
            CHECK(RC_ROMANI_RANCH_GRASS_18, true),
            CHECK(RC_ROMANI_RANCH_GRASS_19, true),
            CHECK(RC_ROMANI_RANCH_GRASS_20, true),
            CHECK(RC_ROMANI_RANCH_GRASS_21, true),
            CHECK(RC_ROMANI_RANCH_GRASS_22, true),
            CHECK(RC_ROMANI_RANCH_GRASS_23, true),
            CHECK(RC_ROMANI_RANCH_GRASS_24, true),
            CHECK(RC_ROMANI_RANCH_GRASS_25, true),
            CHECK(RC_ROMANI_RANCH_GRASS_26, true),
            CHECK(RC_ROMANI_RANCH_GRASS_27, true),
            CHECK(RC_ROMANI_RANCH_GRASS_28, true),
            CHECK(RC_ROMANI_RANCH_GRASS_29, true),
            CHECK(RC_ROMANI_RANCH_GRASS_30, true),
            CHECK(RC_ROMANI_RANCH_GRASS_31, true),
            CHECK(RC_ROMANI_RANCH_GRASS_32, true),
            CHECK(RC_ROMANI_RANCH_GRASS_33, true),
            CHECK(RC_ROMANI_RANCH_GRASS_34, true),
            CHECK(RC_ROMANI_RANCH_GRASS_35, true),
            CHECK(RC_ROMANI_RANCH_GRASS_36, true),
            CHECK(RC_ROMANI_RANCH_GRASS_37, true),
            CHECK(RC_ROMANI_RANCH_GRASS_38, true),
            CHECK(RC_ROMANI_RANCH_GRASS_39, true),
            CHECK(RC_ROMANI_RANCH_GRASS_40, true),
            CHECK(RC_ROMANI_RANCH_GRASS_41, true),
            CHECK(RC_ROMANI_RANCH_GRASS_42, true),
            CHECK(RC_ROMANI_RANCH_GRASS_43, true),
            CHECK(RC_ROMANI_RANCH_GRASS_44, true),
            CHECK(RC_ROMANI_RANCH_GRASS_45, true),
            CHECK(RC_ROMANI_RANCH_GRASS_46, true),
            CHECK(RC_ROMANI_RANCH_GRASS_47, true),
            CHECK(RC_ROMANI_RANCH_GRASS_48, true),
            CHECK(RC_ROMANI_RANCH_GRASS_49, true),
            CHECK(RC_ROMANI_RANCH_GRASS_50, true),
            CHECK(RC_ROMANI_RANCH_GRASS_51, true),
            CHECK(RC_ROMANI_RANCH_GRASS_52, true),
            CHECK(RC_ROMANI_RANCH_GRASS_53, true),
            CHECK(RC_ROMANI_RANCH_GRASS_54, true),
            CHECK(RC_ROMANI_RANCH_GRASS_55, true),
            CHECK(RC_ROMANI_RANCH_GRASS_56, true),
            CHECK(RC_ROMANI_RANCH_GRASS_57, true),
            CHECK(RC_ROMANI_RANCH_TREE_01, true),
            CHECK(RC_ROMANI_RANCH_TREE_02, true),
            CHECK(RC_ROMANI_RANCH_TREE_03, true),
            CHECK(RC_ROMANI_RANCH_TREE_04, true),
            CHECK(RC_ROMANI_RANCH_TREE_05, true),
            CHECK(RC_ROMANI_RANCH_TREE_06, true),
            CHECK(RC_ROMANI_RANCH_TREE_07, true),
            CHECK(RC_ROMANI_RANCH_WONDER_ITEM_01, true),
            CHECK(RC_ROMANI_RANCH_WONDER_ITEM_02, true),
            CHECK(RC_ROMANI_RANCH_WONDER_ITEM_03, true),
            CHECK(RC_ROMANI_RANCH_WONDER_ITEM_04, true),
            CHECK(RC_ROMANI_RANCH_WONDER_ITEM_05, true),
            CHECK(RC_ROMANI_RANCH_WONDER_ITEM_06, true),
            // TODO: Time logic is wrong for entrance rando
            CHECK(RC_ENEMY_DROP_ALIEN, CanKillEnemy(ACTOR_EN_INVADEPOH) && IS_NIGHT1() && CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG)), // Night 1 only
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MILK_ROAD, 1),                    ENTRANCE(ROMANI_RANCH, 0), true),
            EXIT(ENTRANCE(RANCH_HOUSE, 0),                  ENTRANCE(ROMANI_RANCH, 2), BETWEEN(TIME_DAY1_AM_06_00, TIME_NIGHT1_AM_02_30) || SECOND_DAY() || BETWEEN(TIME_DAY3_AM_06_00, TIME_NIGHT3_PM_08_00)), // Barn
            EXIT(ENTRANCE(RANCH_HOUSE, 1),                  ENTRANCE(ROMANI_RANCH, 3), BETWEEN(TIME_DAY1_AM_06_00, TIME_NIGHT1_PM_08_00) || BETWEEN(TIME_DAY2_AM_06_00, TIME_NIGHT2_PM_08_00) || BETWEEN(TIME_DAY3_AM_06_00, TIME_NIGHT3_PM_08_00)), // House
            EXIT(ENTRANCE(CUCCO_SHACK, 0),                  ENTRANCE(ROMANI_RANCH, 4), BETWEEN(TIME_DAY1_AM_06_00, TIME_NIGHT1_PM_08_00) || BETWEEN(TIME_DAY2_AM_06_00, TIME_NIGHT2_PM_08_00) || BETWEEN(TIME_DAY3_AM_06_00, TIME_NIGHT3_PM_08_00)),
            EXIT(ENTRANCE(DOGGY_RACETRACK, 0),              ENTRANCE(ROMANI_RANCH, 5), BETWEEN(TIME_DAY1_AM_06_00, TIME_NIGHT1_PM_08_00) || BETWEEN(TIME_DAY2_AM_06_00, TIME_NIGHT2_PM_08_00) || BETWEEN(TIME_DAY3_AM_06_00, TIME_NIGHT3_PM_08_00)),
        },
        .events = {
            EVENT(RE_COWS_FROM_ALIENS, CanKillEnemy(ACTOR_EN_INVADEPOH) && IS_NIGHT1() && CAN_BE_GORON && HAS_ITEM(ITEM_POWDER_KEG)),
        },
    };
}, {});
// clang-format on
