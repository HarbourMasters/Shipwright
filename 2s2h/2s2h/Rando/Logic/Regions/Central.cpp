#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_ASTRAL_OBSERVATORY_OUTSIDE] = RandoRegion{ .name = "Outside Astral Observatory", .sceneId = SCENE_00KEIKOKU,
        .checks = {
            CHECK(RC_ASTRAL_OBSERVATORY_MOON_TEAR, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ASTRAL_OBSERVATORY, 1),           ENTRANCE(TERMINA_FIELD, 9), true),
        },
        .connections = {
            CONNECTION(RR_TERMINA_FIELD, CAN_GROW_BEAN_PLANT)
        },
    };
    Regions[RR_ASTRAL_OBSERVATORY_PASSAGE] = RandoRegion{ .name = "Passage", .sceneId = SCENE_TENMON_DAI,
        .checks = {
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_CHEST, CAN_USE_EXPLOSIVE && (CAN_USE_ABILITY(SWIM) || CAN_BE_ZORA)),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_01, true),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_02, true),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_03, true),
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_POT_04, true),
            CHECK(RC_ENEMY_DROP_SKULLTULA, CanKillEnemy(ACTOR_EN_ST)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 2),              ENTRANCE(ASTRAL_OBSERVATORY, 0), true),
        },
        .connections = {
            CONNECTION(RR_ASTRAL_OBSERVATORY, true),
        },
    };
    Regions[RR_ASTRAL_OBSERVATORY] = RandoRegion{ .name = "Inside Astral Observatory", .sceneId = SCENE_TENMON_DAI,
        .checks = {
            CHECK(RC_ASTRAL_OBSERVATORY_POT_01, true),
            CHECK(RC_ASTRAL_OBSERVATORY_POT_02, true),
            CHECK(RC_ASTRAL_OBSERVATORY_POT_03, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 9),                ENTRANCE(ASTRAL_OBSERVATORY, 1), true),
        },
        .connections = {
            CONNECTION(RR_ASTRAL_OBSERVATORY_PASSAGE, true),
        },
        .events = {
            EVENT(RE_TERMINA_FIELD_SCRUB_ENTERED_GROTTO, true),
        },
    };
    Regions[RR_BOMB_SHOP] = RandoRegion{ .sceneId = SCENE_BOMYA,
        .checks = {
            CHECK(RC_BOMB_SHOP_ITEM_01, CAN_AFFORD(RC_BOMB_SHOP_ITEM_01)),
            CHECK(RC_BOMB_SHOP_ITEM_02, CAN_AFFORD(RC_BOMB_SHOP_ITEM_02)),
            // Upon saving the Bomb Shop lady, one item in the shop gets replaced with the other for the remainder of the cycle.
            CHECK(RC_BOMB_SHOP_ITEM_03, CAN_AFFORD(RC_BOMB_SHOP_ITEM_03)),
            CHECK(RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM, CAN_AFFORD(RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM) && RANDO_EVENTS[RE_SAVE_BOMB_SHOP_LADY]),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 6),              ENTRANCE(BOMB_SHOP, 0), true),
        },
    };
    Regions[RR_CLOCK_TOWER_INTERIOR] = RandoRegion{ .sceneId = SCENE_INSIDETOWER,
        .checks = {
            // There are no checks here, the 2 that the mask salesman would give you are in RR_MAX
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 0),             ENTRANCE(CLOCK_TOWER_INTERIOR, 1), true),
        },
    };
    Regions[RR_CLOCK_TOWER_ROOF] = RandoRegion{ .sceneId = SCENE_OKUJOU,
        .checks = {
            CHECK(RC_CLOCK_TOWER_ROOF_SONG_OF_TIME, CAN_USE_PROJECTILE),
            CHECK(RC_CLOCK_TOWER_ROOF_OCARINA, CAN_USE_PROJECTILE),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_01, true),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_02, true),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_03, true),
            CHECK(RC_CLOCK_TOWER_ROOF_POT_04, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(THE_MOON, 0),                              ONE_WAY_EXIT, CAN_PLAY_SONG(OATH) && MeetsMoonRequirements()),
        },
        .oneWayEntrances = {
            ENTRANCE(CLOCK_TOWER_ROOFTOP, 0), // From clock tower platform
        },
    };
    Regions[RR_CLOCK_TOWN_EAST] = RandoRegion{ .sceneId = SCENE_TOWN,
        .checks = {
            CHECK(RC_CLOCK_TOWN_EAST_SMALL_CRATE_01, true),
            CHECK(RC_CLOCK_TOWN_EAST_SMALL_CRATE_02, true),
            CHECK(RC_CLOCK_TOWN_EAST_POSTMAN_HAT, RANDO_EVENTS[RE_POSTMAN_FREEDOM] && BETWEEN(TIME_NIGHT3_PM_06_00, TIME_NIGHT3_AM_05_00)),
            CHECK(RC_CLOCK_TOWN_STRAY_FAIRY,         CAN_BE_DEKU && IS_NIGHT()),
            CHECK(RC_CLOCK_TOWN_EAST_UPPER_CHEST,    true),
            CHECK(RC_CLOCK_TOWN_BOMBERS_NOTEBOOK,    RANDO_EVENTS[RE_BOMBER_CODE]),
            CHECK(RC_CLOCK_TOWN_POSTBOX,             HAS_ITEM(ITEM_MASK_POSTMAN)),
            CHECK(RC_CLOCK_TOWN_EAST_WONDER_ITEM_01, CAN_USE_SWORD || CAN_USE_PROJECTILE || HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_DEKU_NUT)),
            CHECK(RC_CLOCK_TOWN_EAST_WONDER_ITEM_02, CAN_USE_SWORD || CAN_USE_PROJECTILE || HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_DEKU_NUT)),
            CHECK(RC_CLOCK_TOWN_EAST_WONDER_ITEM_03, CAN_USE_SWORD || CAN_USE_PROJECTILE || HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_DEKU_NUT)),
            CHECK(RC_CLOCK_TOWN_EAST_WONDER_ITEM_04, CAN_USE_SWORD || CAN_USE_PROJECTILE || HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_DEKU_NUT)),
            CHECK(RC_CLOCK_TOWN_EAST_WONDER_ITEM_05, CAN_USE_SWORD || CAN_USE_PROJECTILE || HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_DEKU_NUT)),
            CHECK(RC_CLOCK_TOWN_EAST_WONDER_ITEM_06, CAN_USE_SWORD || CAN_USE_PROJECTILE || HAS_ITEM(ITEM_DEKU_STICK) || HAS_ITEM(ITEM_DEKU_NUT)),
            CHECK(RC_CLOCK_TOWN_EAST_WONDER_ITEM_07, true),
            CHECK(RC_CLOCK_TOWN_EAST_WONDER_ITEM_08, true),
            CHECK(RC_CLOCK_TOWN_EAST_WONDER_ITEM_09, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 7),                ENTRANCE(EAST_CLOCK_TOWN, 0), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 7),             ENTRANCE(EAST_CLOCK_TOWN, 1), true), // To lower
            EXIT(ENTRANCE(ASTRAL_OBSERVATORY, 0),           ENTRANCE(EAST_CLOCK_TOWN, 2), CAN_USE_PROJECTILE),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 2),             ENTRANCE(EAST_CLOCK_TOWN, 3), true), // To upper
            EXIT(ENTRANCE(TREASURE_CHEST_SHOP, 0),          ENTRANCE(EAST_CLOCK_TOWN, 4), BEFORE(TIME_NIGHT1_PM_10_00) || BETWEEN(TIME_DAY2_AM_06_00, TIME_NIGHT2_PM_10_00) || BETWEEN(TIME_DAY3_AM_06_00, TIME_NIGHT3_PM_10_00)),
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 1),             ENTRANCE(EAST_CLOCK_TOWN, 5), true),
            EXIT(ENTRANCE(HONEY_AND_DARLINGS_SHOP, 0),      ENTRANCE(EAST_CLOCK_TOWN, 6), BEFORE(TIME_NIGHT1_PM_10_00) || BETWEEN(TIME_DAY2_AM_06_00, TIME_NIGHT2_PM_10_00) || BETWEEN(TIME_DAY3_AM_06_00, TIME_NIGHT3_PM_10_00)),
            EXIT(ENTRANCE(MAYORS_RESIDENCE, 0),             ENTRANCE(EAST_CLOCK_TOWN, 7), BETWEEN(TIME_DAY1_AM_10_00, TIME_NIGHT1_PM_08_00) || BETWEEN(TIME_DAY2_AM_10_00, TIME_NIGHT2_PM_08_00) || AFTER(TIME_DAY3_AM_10_00)),
            EXIT(ENTRANCE(TOWN_SHOOTING_GALLERY, 0),        ENTRANCE(EAST_CLOCK_TOWN, 8), BEFORE(TIME_NIGHT1_PM_10_00) || BETWEEN(TIME_DAY2_AM_06_00, TIME_NIGHT2_PM_10_00) || BETWEEN(TIME_DAY3_AM_06_00, TIME_NIGHT3_PM_10_00)),
            EXIT(ENTRANCE(STOCK_POT_INN, 0),                ENTRANCE(EAST_CLOCK_TOWN, 9), HAS_ITEM(ITEM_ROOM_KEY) || BETWEEN(TIME_DAY1_AM_08_00, TIME_NIGHT1_PM_08_00) || BETWEEN(TIME_DAY2_AM_08_00, TIME_NIGHT2_PM_08_00) || AFTER(TIME_DAY3_AM_08_00)),
            EXIT(ENTRANCE(STOCK_POT_INN, 1),                ENTRANCE(EAST_CLOCK_TOWN, 10), CAN_BE_DEKU), // To upstairs
            EXIT(ENTRANCE(MILK_BAR, 0),                     ENTRANCE(EAST_CLOCK_TOWN, 11), (BETWEEN(TIME_DAY1_AM_10_00, TIME_NIGHT1_PM_09_00) || 
                                                                                            BETWEEN(TIME_DAY2_AM_10_00, TIME_NIGHT2_PM_09_00) || 
                                                                                            BETWEEN(TIME_DAY3_AM_10_00, TIME_NIGHT3_PM_09_00)) || 
                                                                                            (HAS_ITEM(ITEM_MASK_ROMANI) && (BETWEEN(TIME_NIGHT1_PM_10_00, TIME_NIGHT1_AM_05_00) || 
                                                                                            BETWEEN(TIME_NIGHT2_PM_10_00, TIME_NIGHT2_AM_05_00) ||
                                                                                            AFTER(TIME_NIGHT3_PM_10_00)))),
        },
        .events = {
            EVENT(RE_MAIL_LETTER_TO_KAFEI, Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI) && BEFORE(TIME_DAY2_AM_11_30)),
        },
    };
    Regions[RR_CLOCK_TOWN_GREAT_FAIRY_FOUNTAIN] = RandoRegion{ .name = "Clock Town", .sceneId = SCENE_YOUSEI_IZUMI,
        .checks = {
            CHECK(RC_CLOCK_TOWN_GREAT_FAIRY, CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80)),
            CHECK(RC_CLOCK_TOWN_GREAT_FAIRY_ALT, CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80) && (CAN_BE_DEKU || CAN_BE_GORON || CAN_BE_ZORA)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 3),             ENTRANCE(FAIRY_FOUNTAIN, 0), true),
        },
    };
    Regions[RR_CLOCK_TOWN_LAUNDRY] = RandoRegion{ .sceneId = SCENE_ALLEY,
        .checks = {
            CHECK(RC_CLOCK_TOWN_STRAY_FAIRY,                    IS_DAY()),
            CHECK(RC_CLOCK_TOWN_LAUNDRY_FREESTANDING_RUPEE_01,  (CAN_USE_ABILITY(SWIM) || CAN_BE_ZORA) && IS_NIGHT2()),
            CHECK(RC_CLOCK_TOWN_LAUNDRY_FREESTANDING_RUPEE_02,  (CAN_USE_ABILITY(SWIM) || CAN_BE_ZORA) && IS_NIGHT2()),
            CHECK(RC_CLOCK_TOWN_LAUNDRY_FREESTANDING_RUPEE_03,  (CAN_USE_ABILITY(SWIM) || CAN_BE_ZORA) && IS_NIGHT2()),
            CHECK(RC_CLOCK_TOWN_LAUNDRY_FROG,                   HAS_ITEM(ITEM_MASK_DON_GERO)),
            CHECK(RC_CLOCK_TOWN_LAUNDRY_GURU_GURU,              IS_NIGHT1() || IS_NIGHT2()),
            CHECK(RC_CLOCK_TOWN_LAUNDRY_SMALL_CRATE,            true),
            CHECK(RC_CLOCK_TOWN_LAUNDRY_POOL_GRASS_01, true),
            CHECK(RC_CLOCK_TOWN_LAUNDRY_POOL_GRASS_02, true),
            CHECK(RC_CLOCK_TOWN_LAUNDRY_POOL_GRASS_03, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 6),             ENTRANCE(LAUNDRY_POOL, 0), true),
            EXIT(ENTRANCE(CURIOSITY_SHOP, 1),               ENTRANCE(LAUNDRY_POOL, 1), (RANDO_EVENTS[RE_MAIL_LETTER_TO_KAFEI] && BETWEEN(TIME_DAY2_PM_04_00, TIME_NIGHT2_PM_10_00)) || (RANDO_EVENTS[RE_MEET_KAFEI] && BETWEEN(TIME_DAY3_PM_01_00, TIME_NIGHT3_PM_10_00))),
        },
    };
    Regions[RR_CLOCK_TOWN_NORTH] = RandoRegion{ .sceneId = SCENE_BACKTOWN,
        .checks = {
            CHECK(RC_CLOCK_TOWN_NORTH_KEATON_GRASS_01, true),
            CHECK(RC_CLOCK_TOWN_NORTH_KEATON_GRASS_02, true),
            CHECK(RC_CLOCK_TOWN_NORTH_KEATON_GRASS_03, true),
            CHECK(RC_CLOCK_TOWN_NORTH_KEATON_GRASS_04, true),
            CHECK(RC_CLOCK_TOWN_NORTH_KEATON_GRASS_05, true),
            CHECK(RC_CLOCK_TOWN_NORTH_KEATON_GRASS_06, true),
            CHECK(RC_CLOCK_TOWN_NORTH_KEATON_GRASS_07, true),
            CHECK(RC_CLOCK_TOWN_NORTH_KEATON_GRASS_08, true),
            CHECK(RC_CLOCK_TOWN_NORTH_KEATON_GRASS_09, true),
            CHECK(RC_CLOCK_TOWN_NORTH_KEATON_GRASS_WONDER_ITEM, CAN_FULLY_CUT_KEATON_GRASS),
            CHECK(RC_CLOCK_TOWN_NORTH_TINGLE_MAP_01, CAN_USE_PROJECTILE && CAN_AFFORD(RC_CLOCK_TOWN_NORTH_TINGLE_MAP_01) && IS_DAY()),
            CHECK(RC_CLOCK_TOWN_NORTH_TINGLE_MAP_02, CAN_USE_PROJECTILE && CAN_AFFORD(RC_CLOCK_TOWN_NORTH_TINGLE_MAP_02) && IS_DAY()),
            CHECK(RC_CLOCK_TOWN_NORTH_TREE_PIECE_OF_HEART, true),
            CHECK(RC_CLOCK_TOWN_NORTH_TREE_01, true),
            CHECK(RC_CLOCK_TOWN_NORTH_BOMB_LADY, RANDO_EVENTS[RE_SAVE_BOMB_SHOP_LADY]),
            CHECK(RC_CLOCK_TOWN_BOMBERS_NOTEBOOK, RANDO_EVENTS[RE_BOMBER_CODE]),
            CHECK(RC_CLOCK_TOWN_POSTBOX, HAS_ITEM(ITEM_MASK_POSTMAN)),
            CHECK(RC_KEATON_QUIZ, HAS_ITEM(ITEM_MASK_KEATON)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 8),                ENTRANCE(NORTH_CLOCK_TOWN, 0), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 5),              ENTRANCE(NORTH_CLOCK_TOWN, 1), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 4),             ENTRANCE(NORTH_CLOCK_TOWN, 2), true),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 0),               ENTRANCE(NORTH_CLOCK_TOWN, 3), true),
            EXIT(ENTRANCE(DEKU_SCRUB_PLAYGROUND, 0),        ENTRANCE(NORTH_CLOCK_TOWN, 4), CAN_BE_DEKU),
        },
        .events = {
            EVENT(RE_MAIL_LETTER_TO_KAFEI, Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI) && BEFORE(TIME_DAY2_AM_11_30)),
            EVENT(RE_ACCESS_PICTOGRAPH_TINGLE, HAS_ITEM(ITEM_PICTOGRAPH_BOX) && IS_DAY()),
            // Refer to z_en_suttari's damage table for more info. Damage effect 0xF stops him nonlethally, while 0xE kills.
            // FD sword beams can also kill him, but currently FD is not logically considered.
            EVENT(RE_SAVE_BOMB_SHOP_LADY, (CAN_USE_SWORD || CAN_BE_ZORA || CAN_BE_GORON) && AT(TIME_NIGHT1_AM_12_00)),
            EVENT(RE_KILL_SAKON, (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA || CAN_USE_EXPLOSIVE) && AT(TIME_NIGHT1_AM_12_00)),
            // Hide and seek events
            EVENT(RE_HIDE_SEEK_DAY1, CAN_USE_PROJECTILE && FIRST_DAY()),
            EVENT(RE_HIDE_SEEK_DAY2, CAN_USE_PROJECTILE && SECOND_DAY()),
            EVENT(RE_HIDE_SEEK_DAY3, CAN_USE_PROJECTILE && FINAL_DAY()),
            // North bomber events
            EVENT(RE_BOMBERS_NORTH_DAY1, RANDO_EVENTS[RE_HIDE_SEEK_DAY1]),
            EVENT(RE_BOMBERS_NORTH_DAY2, RANDO_EVENTS[RE_HIDE_SEEK_DAY2]),
            EVENT(RE_BOMBERS_NORTH_DAY3, RANDO_EVENTS[RE_HIDE_SEEK_DAY3]),
            // East bomber events
            EVENT(RE_BOMBERS_EAST_DAY1, RANDO_EVENTS[RE_HIDE_SEEK_DAY1]),
            EVENT(RE_BOMBERS_EAST_DAY2, RANDO_EVENTS[RE_HIDE_SEEK_DAY2]),
            EVENT(RE_BOMBERS_EAST_DAY3, RANDO_EVENTS[RE_HIDE_SEEK_DAY3]),
            // West bomber events
            EVENT(RE_BOMBERS_WEST_DAY1, RANDO_EVENTS[RE_HIDE_SEEK_DAY1]),
            EVENT(RE_BOMBERS_WEST_DAY2, RANDO_EVENTS[RE_HIDE_SEEK_DAY2]),
            EVENT(RE_BOMBERS_WEST_DAY3, RANDO_EVENTS[RE_HIDE_SEEK_DAY3]),
            // Bomber code event
            EVENT(RE_BOMBER_CODE, 
                (RANDO_EVENTS[RE_BOMBERS_NORTH_DAY1] && RANDO_EVENTS[RE_BOMBERS_WEST_DAY1] && RANDO_EVENTS[RE_BOMBERS_EAST_DAY1]) ||
                (RANDO_EVENTS[RE_BOMBERS_NORTH_DAY2] && RANDO_EVENTS[RE_BOMBERS_WEST_DAY2] && RANDO_EVENTS[RE_BOMBERS_EAST_DAY2]) ||
                (RANDO_EVENTS[RE_BOMBERS_NORTH_DAY3] && RANDO_EVENTS[RE_BOMBERS_WEST_DAY3] && RANDO_EVENTS[RE_BOMBERS_EAST_DAY3])),
        },
    };
    Regions[RR_CLOCK_TOWN_SOUTH] = RandoRegion{ .sceneId = SCENE_CLOCKTOWER,
        .checks = {
            CHECK(RC_CLOCK_TOWN_POSTBOX, HAS_ITEM(ITEM_MASK_POSTMAN)),
            CHECK(RC_CLOCK_TOWN_SOUTH_PLATFORM_PIECE_OF_HEART, true),
            CHECK(RC_CLOCK_TOWN_SCRUB_DEED, Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)),
            CHECK(RC_CLOCK_TOWN_SOUTH_CHEST_UPPER, ((CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)) || HAS_ITEM(ITEM_HOOKSHOT)) && FINAL_DAY()),
            CHECK(RC_CLOCK_TOWN_SOUTH_CHEST_LOWER, (CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)) || HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_CLOCK_TOWN_SOUTH_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_CLOCK_TOWN_SOUTH_WONDER_ITEM_01, CAN_USE_PROJECTILE),
            CHECK(RC_CLOCK_TOWN_SOUTH_WONDER_ITEM_02, CAN_USE_PROJECTILE),
            CHECK(RC_CLOCK_TOWN_SOUTH_WONDER_ITEM_03, CAN_USE_PROJECTILE),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(CLOCK_TOWER_INTERIOR, 1),         ENTRANCE(SOUTH_CLOCK_TOWN, 0), true),
            EXIT(ENTRANCE(TERMINA_FIELD, 6),                ENTRANCE(SOUTH_CLOCK_TOWN, 1), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 3),              ENTRANCE(SOUTH_CLOCK_TOWN, 2), true), // To upper
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 2),              ENTRANCE(SOUTH_CLOCK_TOWN, 3), true), // To upper
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 2),             ENTRANCE(SOUTH_CLOCK_TOWN, 4), true),
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 1),              ENTRANCE(SOUTH_CLOCK_TOWN, 5), true), // To lower
            EXIT(ENTRANCE(LAUNDRY_POOL, 0),                 ENTRANCE(SOUTH_CLOCK_TOWN, 6), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 1),              ENTRANCE(SOUTH_CLOCK_TOWN, 7), true), // To lower
            EXIT(ENTRANCE(CLOCK_TOWER_ROOFTOP, 0),                   ONE_WAY_EXIT, AFTER(TIME_NIGHT3_AM_12_00)), // Clock Tower Platform accessible only after midnight Night 3
        },
        .connections = {
            CONNECTION(RR_MAX, true),
        },
        .events = {
            EVENT(RE_MAIL_LETTER_TO_KAFEI, Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI) && BEFORE(TIME_DAY2_AM_11_30)),
        },
        .oneWayEntrances = {
            ENTRANCE(SOUTH_CLOCK_TOWN, 9), // From Song of Soaring
        }
    };
    Regions[RR_CLOCK_TOWN_WEST] = RandoRegion{ .sceneId = SCENE_ICHIBA,
        .checks = {
            CHECK(RC_CLOCK_TOWN_WEST_BANK_ADULTS_WALLET, true),
            CHECK(RC_CLOCK_TOWN_WEST_BANK_PIECE_OF_HEART, CUR_UPG_VALUE(UPG_WALLET) >= 1),
            CHECK(RC_CLOCK_TOWN_WEST_BANK_INTEREST, CUR_UPG_VALUE(UPG_WALLET) >= 1),
            CHECK(RC_CLOCK_TOWN_WEST_SISTERS_PIECE_OF_HEART, HAS_ITEM(ITEM_MASK_KAMARO) && (IS_NIGHT1() || IS_NIGHT2())),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 0),                ENTRANCE(WEST_CLOCK_TOWN, 0), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 5),             ENTRANCE(WEST_CLOCK_TOWN, 1), true), // To lower
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 3),             ENTRANCE(WEST_CLOCK_TOWN, 2), true), // To upper
            EXIT(ENTRANCE(SWORDMANS_SCHOOL, 0),             ENTRANCE(WEST_CLOCK_TOWN, 3), FIRST_DAY() || SECOND_DAY() || BETWEEN(TIME_DAY3_AM_06_00, TIME_NIGHT3_PM_11_00) || AFTER(TIME_NIGHT3_AM_12_00)),
            EXIT(ENTRANCE(CURIOSITY_SHOP, 0),               ENTRANCE(WEST_CLOCK_TOWN, 4), BETWEEN(TIME_NIGHT1_PM_10_00, TIME_DAY2_AM_06_00) || BETWEEN(TIME_NIGHT2_PM_10_00, TIME_DAY3_AM_06_00) || AFTER(TIME_NIGHT3_PM_10_00)),
            EXIT(ENTRANCE(TRADING_POST, 0),                 ENTRANCE(WEST_CLOCK_TOWN, 5), true),
            EXIT(ENTRANCE(BOMB_SHOP, 0),                    ENTRANCE(WEST_CLOCK_TOWN, 6), true),
            EXIT(ENTRANCE(POST_OFFICE, 0),                  ENTRANCE(WEST_CLOCK_TOWN, 7), BETWEEN(TIME_DAY1_PM_03_00, TIME_NIGHT1_AM_12_00) || (RANDO_EVENTS[RE_MAIL_LETTER_TO_KAFEI] && BETWEEN(TIME_NIGHT2_PM_06_00, TIME_NIGHT2_AM_12_00)) || IS_NIGHT3()),
            EXIT(ENTRANCE(LOTTERY_SHOP, 0),                 ENTRANCE(WEST_CLOCK_TOWN, 8), IS_DAY() || (BEFORE(TIME_NIGHT1_PM_11_00) || BETWEEN(TIME_NIGHT2_PM_06_00, TIME_NIGHT2_PM_11_00) || BETWEEN(TIME_NIGHT3_PM_06_00, TIME_NIGHT3_PM_11_00))),
        },
    };
    Regions[RR_CURIOSITY_SHOP_BACK] = RandoRegion{ .name = "Back", .sceneId = SCENE_AYASHIISHOP,
        .checks = {
            CHECK(RC_KAFEIS_HIDEOUT_KEATON_MASK, BETWEEN(TIME_DAY3_PM_01_00, TIME_NIGHT3_PM_10_00)),
            CHECK(RC_KAFEIS_HIDEOUT_LETTER_TO_MAMA, BETWEEN(TIME_DAY3_PM_01_00, TIME_NIGHT3_PM_10_00)),
            CHECK(RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES, RANDO_EVENTS[RE_MAIL_LETTER_TO_KAFEI] && BETWEEN(TIME_DAY2_PM_04_00, TIME_NIGHT2_PM_10_00)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(LAUNDRY_POOL, 1),                 ENTRANCE(CURIOSITY_SHOP, 1), true)
        },
        .events = {
            EVENT(RE_MEET_KAFEI, RANDO_EVENTS[RE_MAIL_LETTER_TO_KAFEI] && BETWEEN(TIME_DAY2_PM_04_00, TIME_NIGHT2_PM_10_00)),
        },
        .timeStayRestrictions = {
            STAY(TIME_NIGHT2_PM_10_00, false),
            STAY(TIME_NIGHT3_PM_10_00, false),
            STAY(TIME_DAY2_AM_06_00, false), // Kick at start of day
            STAY(TIME_DAY3_AM_06_00, false),
        },
    };
    Regions[RR_CURIOSITY_SHOP_FRONT] = RandoRegion{ .name = "Front", .sceneId = SCENE_AYASHIISHOP,
        .checks = {
            CHECK(RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM, CAN_AFFORD(RC_BOMB_SHOP_ITEM_04_OR_CURIOSITY_SHOP_ITEM) && IS_NIGHT3()),
            CHECK(RC_CURIOSITY_SHOP_SPECIAL_ITEM, CAN_AFFORD(RC_CURIOSITY_SHOP_SPECIAL_ITEM) && (RANDO_EVENTS[RE_SAVE_BOMB_SHOP_LADY] || RANDO_EVENTS[RE_KILL_SAKON]) && IS_NIGHT3()),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 4),              ENTRANCE(CURIOSITY_SHOP, 0), true),
        },
    };
    Regions[RR_HONEY_AND_DARLING] = RandoRegion{ .sceneId = SCENE_BOWLING,
        .checks = {
            CHECK(RC_CLOCK_TOWN_EAST_HONEY_DARLING_ALL_DAYS, RANDO_EVENTS[RE_HONEY_DARLING_REWARD_DAY1] && RANDO_EVENTS[RE_HONEY_DARLING_REWARD_DAY2] && RANDO_EVENTS[RE_HONEY_DARLING_REWARD_DAY3]),
            CHECK(RC_CLOCK_TOWN_EAST_HONEY_DARLING_ANY_DAY, (RANDO_EVENTS[RE_HONEY_DARLING_REWARD_DAY1] || RANDO_EVENTS[RE_HONEY_DARLING_REWARD_DAY2] || RANDO_EVENTS[RE_HONEY_DARLING_REWARD_DAY3])),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 6),              ENTRANCE(HONEY_AND_DARLINGS_SHOP, 0), true),
        },
        .events = {
            EVENT(RE_HONEY_DARLING_REWARD_DAY1, (HAS_ITEM(ITEM_BOMB) || HAS_ITEM(ITEM_BOMBCHU)) && BEFORE(TIME_NIGHT1_PM_10_00)),
            EVENT(RE_HONEY_DARLING_REWARD_DAY2, HAS_ITEM(ITEM_BOMB) && BETWEEN(TIME_DAY2_AM_06_00, TIME_NIGHT2_PM_10_00)),
            EVENT(RE_HONEY_DARLING_REWARD_DAY3, HAS_ITEM(ITEM_BOW) && IS_DAY3()),
        },
    };
    Regions[RR_INN] = RandoRegion{ .sceneId = SCENE_YADOYA,
        .checks = {
            CHECK(RC_STOCK_POT_INN_COUPLES_MASK, HAS_ITEM(ITEM_MASK_KAFEIS_MASK) && RANDO_EVENTS[RE_DELIVER_PENDANT] && RANDO_EVENTS[RE_RETRIEVE_SUN_MASK] && AFTER(TIME_NIGHT3_AM_04_00)),
            CHECK(RC_STOCK_POT_INN_GRANDMA_LONG_STORY, HAS_ITEM(ITEM_MASK_ALL_NIGHT) && 
                ((BEFORE(TIME_DAY1_PM_04_00) && CLOCK_NIGHT1()) || BETWEEN(TIME_DAY2_AM_06_00, TIME_DAY2_PM_04_00) ||
                 (IS_DAY1() && (CLOCK_NIGHT1() || CLOCK_DAY2() || CLOCK_NIGHT2() || CLOCK_DAY3() || CLOCK_NIGHT3())) ||
                 (IS_DAY2() && (CLOCK_NIGHT2() || CLOCK_DAY3() || CLOCK_NIGHT3())))),
            CHECK(RC_STOCK_POT_INN_GRANDMA_SHORT_STORY, HAS_ITEM(ITEM_MASK_ALL_NIGHT) && 
                ((IS_DAY1() && (CLOCK_DAY2() || CLOCK_NIGHT2() || CLOCK_DAY3() || CLOCK_NIGHT3())) ||
                 (IS_DAY2() && (CLOCK_DAY3() || CLOCK_NIGHT3())))),
            CHECK(RC_STOCK_POT_INN_GUEST_ROOM_CHEST, Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY)),
            CHECK(RC_STOCK_POT_INN_LETTER_TO_KAFEI, HAS_ITEM(ITEM_MASK_KAFEIS_MASK) && RANDO_EVENTS[RE_ANJU_MIDNIGHT_MEETING]),
            CHECK(RC_STOCK_POT_INN_ROOM_KEY, BETWEEN(TIME_DAY1_PM_01_45, TIME_DAY1_PM_04_00)),
            CHECK(RC_STOCK_POT_INN_STAFF_ROOM_CHEST, IS_NIGHT3()),
            CHECK(RC_STOCK_POT_INN_TOILET_HAND, 
                (Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND) || Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP) ||
                Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN) || Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN) ||
                Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA) || Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI)) && 
                MIDNIGHT()
            ),
            CHECK(RC_STOCK_POT_INN_WONDER_ITEM_01, CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_STOCK_POT_INN_WONDER_ITEM_02, CAN_BE_ZORA && HAS_MAGIC),
            CHECK(RC_STOCK_POT_INN_WONDER_ITEM_03, CAN_BE_ZORA && HAS_MAGIC),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 9),              ENTRANCE(STOCK_POT_INN, 0), true), // From ground floor
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 10),             ENTRANCE(STOCK_POT_INN, 1), true), // From upstairs
        },
        .events = {
            EVENT(RE_ACCESS_FISH, true),
            EVENT(RE_ACCESS_BUGS, true),
            EVENT(RE_SETUP_MEET_ANJU, HAS_ITEM(ITEM_MASK_KAFEIS_MASK) && BETWEEN(TIME_DAY1_PM_01_45, TIME_NIGHT1_PM_09_00)),
            EVENT(RE_ANJU_MIDNIGHT_MEETING, RANDO_EVENTS[RE_SETUP_MEET_ANJU] && BETWEEN(TIME_NIGHT1_AM_12_00, TIME_DAY2_AM_06_00) && (Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY) || CAN_BE_DEKU)),
            EVENT(RE_DELIVER_PENDANT, RANDO_EVENTS[RE_ANJU_MIDNIGHT_MEETING] && Flags_GetRandoInf(RANDO_INF_OBTAINED_PENDANT_OF_MEMORIES) && (BETWEEN(TIME_DAY2_AM_06_00, TIME_NIGHT2_PM_09_00) || BETWEEN(TIME_DAY3_AM_06_00, TIME_DAY3_AM_11_30))),
        },
        .timeStayRestrictions = {
            STAY(TIME_NIGHT1_PM_08_00, Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY)),
            STAY(TIME_DAY2_AM_06_00, Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY)),
            STAY(TIME_NIGHT2_PM_08_00, Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY)),
            STAY(TIME_DAY3_AM_06_00, Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY)),
        },
    };
    Regions[RR_LOTTERY_SHOP] = RandoRegion{ .sceneId = SCENE_TAKARAKUJI,
        .checks = {
            CHECK(RC_CLOCK_TOWN_WEST_LOTTERY, 
                  (IS_DAY1() && CLOCK_NIGHT1()) || 
                  (IS_DAY2() && CLOCK_NIGHT2()) || 
                  (IS_DAY3() && CLOCK_NIGHT3())),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 8),              ENTRANCE(LOTTERY_SHOP, 0), true),
        },
    };
    Regions[RR_MAYOR_RESIDENCE] = RandoRegion{ .sceneId = SCENE_SONCHONOIE,
        .checks = {
            CHECK(RC_MAYORS_OFFICE_PIECE_OF_HEART, HAS_ITEM(ITEM_MASK_COUPLE) && (BETWEEN(TIME_DAY1_AM_10_00, TIME_NIGHT1_PM_08_00) || BETWEEN(TIME_DAY2_AM_10_00, TIME_NIGHT2_PM_08_00) || BETWEEN(TIME_DAY3_AM_10_00, TIME_NIGHT3_PM_06_00))),
            CHECK(RC_MAYORS_OFFICE_KAFEIS_MASK, BETWEEN(TIME_DAY1_AM_10_00, TIME_NIGHT1_PM_08_00) || BETWEEN(TIME_DAY2_AM_10_00, TIME_NIGHT2_PM_08_00))
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 7),              ENTRANCE(MAYORS_RESIDENCE, 0), true),
        },
    };
    Regions[RR_MILK_BAR] = RandoRegion{ .sceneId = SCENE_MILK_BAR,
        .checks = {
            CHECK(RC_MILK_BAR_CIRCUS_LEADER_MASK, HAS_ITEM(ITEM_OCARINA_OF_TIME) && HAS_ITEM(ITEM_MASK_ROMANI) &&
                                                                                    (BETWEEN(TIME_NIGHT1_PM_10_00, TIME_NIGHT1_AM_05_00) ||
                                                                                    BETWEEN(TIME_NIGHT2_PM_10_00, TIME_NIGHT2_AM_05_00))
                                                                                 && (canPlaySong(OCARINA_SONG_WIND_FISH_HUMAN) &&
                                                                                    (CAN_BE_DEKU && canPlaySong(OCARINA_SONG_WIND_FISH_DEKU)) &&
                                                                                    (CAN_BE_GORON && canPlaySong(OCARINA_SONG_WIND_FISH_GORON)) &&
                                                                                    (CAN_BE_ZORA && canPlaySong(OCARINA_SONG_WIND_FISH_ZORA)))),
            CHECK(RC_MILK_BAR_MADAME_AROMA, HAS_ITEM(ITEM_MASK_KAFEIS_MASK) && Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA) && (BETWEEN(TIME_NIGHT3_PM_06_00, TIME_NIGHT3_PM_09_00) || AFTER(TIME_NIGHT3_PM_10_00))),
            CHECK(RC_MILK_BAR_PURCHASE_CHATEAU, CAN_AFFORD(RC_MILK_BAR_PURCHASE_CHATEAU) && HAS_ITEM(ITEM_MASK_ROMANI) && (BETWEEN(TIME_NIGHT1_PM_10_00, TIME_DAY2_AM_06_00) || BETWEEN(TIME_NIGHT2_PM_10_00, TIME_DAY3_AM_06_00) || BETWEEN(TIME_NIGHT3_PM_06_00, TIME_NIGHT3_PM_09_00) || AFTER(TIME_NIGHT3_PM_10_00))),
            CHECK(RC_MILK_BAR_PURCHASE_MILK, CAN_AFFORD(RC_MILK_BAR_PURCHASE_MILK) && HAS_ITEM(ITEM_MASK_ROMANI) && (BETWEEN(TIME_NIGHT1_PM_10_00, TIME_DAY2_AM_06_00) || BETWEEN(TIME_NIGHT2_PM_10_00, TIME_DAY3_AM_06_00) || BETWEEN(TIME_NIGHT3_PM_06_00, TIME_NIGHT3_PM_09_00) || AFTER(TIME_NIGHT3_PM_10_00))),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 11),             ENTRANCE(MILK_BAR, 0), true),
        },
        .timeStayRestrictions = {
            STAY(TIME_NIGHT1_PM_10_00, false),
            STAY(TIME_NIGHT1_AM_05_00, false),
            STAY(TIME_NIGHT2_PM_10_00, false),
            STAY(TIME_NIGHT2_AM_05_00, false),
            STAY(TIME_NIGHT3_PM_10_00, false),
            STAY(TIME_NIGHT3_AM_05_00, false),
        },
    };
    Regions[RR_POST_OFFICE] = RandoRegion{ .sceneId = SCENE_POSTHOUSE,
        .checks = {
            // TODO: Trick for doing without the Bunny Hood
            CHECK(RC_CLOCK_TOWN_WEST_POSTMAN_MINIGAME, HAS_ITEM(ITEM_MASK_BUNNY) && (BETWEEN(TIME_DAY1_PM_03_00, TIME_NIGHT1_AM_12_00) || (RANDO_EVENTS[RE_MAIL_LETTER_TO_KAFEI] && BETWEEN(TIME_NIGHT2_PM_06_00, TIME_NIGHT2_AM_12_00)))),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 7),              ENTRANCE(POST_OFFICE, 0), true),
        },
        .events = {
            EVENT(RE_POSTMAN_FREEDOM, Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA) && IS_NIGHT3()),
        },
    };
    Regions[RR_SWORDSMAN_SCHOOL] = RandoRegion{ .sceneId = SCENE_DOUJOU,
        .checks = {
            CHECK(RC_SWORDSMAN_SCHOOL_PIECE_OF_HEART, CAN_USE_HUMAN_SWORD && BEFORE(TIME_NIGHT3_PM_11_00)),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_01, CAN_USE_HUMAN_SWORD && AFTER(TIME_NIGHT3_AM_12_00)),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_02, CAN_USE_HUMAN_SWORD && AFTER(TIME_NIGHT3_AM_12_00)),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_03, CAN_USE_HUMAN_SWORD && AFTER(TIME_NIGHT3_AM_12_00)),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_04, CAN_USE_HUMAN_SWORD && AFTER(TIME_NIGHT3_AM_12_00)),
            CHECK(RC_SWORDSMAN_SCHOOL_POT_05, CAN_USE_HUMAN_SWORD && AFTER(TIME_NIGHT3_AM_12_00)),
            CHECK(RC_SWORDSMAN_SCHOOL_WONDER_ITEM, (CAN_BE_GORON || CAN_BE_ZORA || CAN_BE_DEKU || CAN_USE_HUMAN_SWORD || CAN_BE_DEITY) &&
                                                    BEFORE(TIME_NIGHT3_PM_11_00)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 3),              ENTRANCE(SWORDMANS_SCHOOL, 0), true),
        },
        .timeStayRestrictions = {
            STAY(TIME_NIGHT3_PM_11_00, false),
        },
    };
    Regions[RR_TOWN_DEKU_PLAYGROUND] = RandoRegion{ .sceneId = SCENE_DEKUTES,
        .checks = {
            CHECK(RC_DEKU_PLAYGROUND_ALL_DAYS, CAN_BE_DEKU && RANDO_EVENTS[RE_DEKU_PLAYGROUND_1] && RANDO_EVENTS[RE_DEKU_PLAYGROUND_2] && RANDO_EVENTS[RE_DEKU_PLAYGROUND_3]),
            CHECK(RC_DEKU_PLAYGROUND_ANY_DAY, CAN_BE_DEKU),
            CHECK(RC_DEKU_PLAYGROUND_DAY_1_RUPEE_01, RANDO_EVENTS[RE_DEKU_PLAYGROUND_1]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_1_RUPEE_02, RANDO_EVENTS[RE_DEKU_PLAYGROUND_1]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_1_RUPEE_03, RANDO_EVENTS[RE_DEKU_PLAYGROUND_1]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_1_RUPEE_04, RANDO_EVENTS[RE_DEKU_PLAYGROUND_1]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_1_RUPEE_05, RANDO_EVENTS[RE_DEKU_PLAYGROUND_1]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_1_RUPEE_06, RANDO_EVENTS[RE_DEKU_PLAYGROUND_1]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_2_RUPEE_01, RANDO_EVENTS[RE_DEKU_PLAYGROUND_2]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_2_RUPEE_02, RANDO_EVENTS[RE_DEKU_PLAYGROUND_2]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_2_RUPEE_03, RANDO_EVENTS[RE_DEKU_PLAYGROUND_2]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_2_RUPEE_04, RANDO_EVENTS[RE_DEKU_PLAYGROUND_2]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_2_RUPEE_05, RANDO_EVENTS[RE_DEKU_PLAYGROUND_2]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_2_RUPEE_06, RANDO_EVENTS[RE_DEKU_PLAYGROUND_2]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_3_RUPEE_01, RANDO_EVENTS[RE_DEKU_PLAYGROUND_3]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_3_RUPEE_02, RANDO_EVENTS[RE_DEKU_PLAYGROUND_3]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_3_RUPEE_03, RANDO_EVENTS[RE_DEKU_PLAYGROUND_3]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_3_RUPEE_04, RANDO_EVENTS[RE_DEKU_PLAYGROUND_3]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_3_RUPEE_05, RANDO_EVENTS[RE_DEKU_PLAYGROUND_3]),
            CHECK(RC_DEKU_PLAYGROUND_DAY_3_RUPEE_06, RANDO_EVENTS[RE_DEKU_PLAYGROUND_3]),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 4),             ENTRANCE(DEKU_SCRUB_PLAYGROUND, 0), true),
        },
        .events = {
            EVENT(RE_DEKU_PLAYGROUND_1, CAN_BE_DEKU && FIRST_DAY()),
            EVENT(RE_DEKU_PLAYGROUND_2, CAN_BE_DEKU && SECOND_DAY()),
            EVENT(RE_DEKU_PLAYGROUND_3, CAN_BE_DEKU && FINAL_DAY()),
        },
    };
    Regions[RR_TOWN_SHOOTING_GALLERY] = RandoRegion{ .sceneId = SCENE_SYATEKI_MIZU,
        .checks = {
            CHECK(RC_CLOCK_TOWN_EAST_SHOOTING_GALLERY_HIGH_SCORE, HAS_ITEM(ITEM_BOW)),
            CHECK(RC_CLOCK_TOWN_EAST_SHOOTING_GALLERY_PERFECT_SCORE, HAS_ITEM(ITEM_BOW)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 8),              ENTRANCE(TOWN_SHOOTING_GALLERY, 0), true),
        },
        .timeStayRestrictions = {
            STAY(TIME_NIGHT1_PM_10_00, false),
            STAY(TIME_NIGHT2_PM_10_00, false),
            STAY(TIME_NIGHT3_PM_10_00, false),
        },
    };
    Regions[RR_TRADING_POST] = RandoRegion{ .sceneId = SCENE_8ITEMSHOP,
        .checks = {
            CHECK(RC_CLOCK_TOWN_WEST_TRADING_POST_POT, true), // Note : Goron has to sidehop to get up.
            CHECK(RC_TRADING_POST_SHOP_ITEM_01, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_01)),
            CHECK(RC_TRADING_POST_SHOP_ITEM_02, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_02)),
            CHECK(RC_TRADING_POST_SHOP_ITEM_03, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_03)),
            CHECK(RC_TRADING_POST_SHOP_ITEM_04, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_04)),
            CHECK(RC_TRADING_POST_SHOP_ITEM_05, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_05)),
            CHECK(RC_TRADING_POST_SHOP_ITEM_06, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_06)),
            CHECK(RC_TRADING_POST_SHOP_ITEM_07, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_07)),
            CHECK(RC_TRADING_POST_SHOP_ITEM_08, CAN_AFFORD(RC_TRADING_POST_SHOP_ITEM_08)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 5),              ENTRANCE(TRADING_POST, 0), true),
        
        },
        .timeStayRestrictions = {
            // Logic break at night
            STAY(TIME_NIGHT1_PM_09_00, false),
            STAY(TIME_NIGHT1_PM_10_00, false),
            STAY(TIME_NIGHT2_PM_09_00, false),
            STAY(TIME_NIGHT2_PM_10_00, false),
            STAY(TIME_NIGHT3_PM_09_00, false),
        },
    };
    Regions[RR_TREASURE_SHOP] = RandoRegion{ .sceneId = SCENE_TAKARAYA,
        .checks = {
            CHECK(RC_CLOCK_TOWN_EAST_TREASURE_CHEST_GAME_DEKU,  CAN_BE_DEKU && BEFORE(TIME_NIGHT3_PM_06_00)),
            CHECK(RC_CLOCK_TOWN_EAST_TREASURE_CHEST_GAME_GORON, CAN_BE_GORON && BEFORE(TIME_NIGHT3_PM_06_00)),
            CHECK(RC_CLOCK_TOWN_EAST_TREASURE_CHEST_GAME_HUMAN, BEFORE(TIME_NIGHT3_PM_06_00)), // can be human
            CHECK(RC_CLOCK_TOWN_EAST_TREASURE_CHEST_GAME_ZORA,  CAN_BE_ZORA && BEFORE(TIME_NIGHT3_PM_06_00)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 4),              ENTRANCE(TREASURE_CHEST_SHOP, 0), true),
        },
    };
}, {});
// clang-format on
