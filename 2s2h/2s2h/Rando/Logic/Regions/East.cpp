#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_BENEATH_THE_GRAVEYARD_DAMPE] = RandoRegion{ .sceneId = SCENE_DANPEI2TEST,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_DAMPE_CHEST, CanKillEnemy(ACTOR_EN_BIGPO)),
            CHECK(RC_BENEATH_THE_GRAVEYARD_DAMPE_POT_01, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_DAMPE_POT_02, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_DAMPE_POT_03, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_DAMPE_POT_04, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_DAMPE_POT_05, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_DAMPE_POT_06, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_DAMPE_POT_07, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_DAMPE_POT_08, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_DAMPE_POT_09, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_DAMPE_POT_10, true),
            CHECK(RC_ENEMY_DROP_WALLMASTER, CanKillEnemy(ACTOR_EN_WALLMAS)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 4),                       ONE_WAY_EXIT, true),
        },
        .oneWayEntrances = {
            ENTRANCE(DAMPES_HOUSE, 0), // Day 3 grave
        }
    };
    Regions[RR_BENEATH_THE_GRAVEYARD_NIGHT_1_BOSS] = RandoRegion{ .name = "Night 1 Boss", .sceneId = SCENE_HAKASHITA,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_SONG_OF_STORMS, CanKillEnemy(ACTOR_EN_IK)),
            CHECK(RC_ENEMY_DROP_IRON_KNUCKLE, CanKillEnemy(ACTOR_EN_IK)),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_1_GRAVE, CanKillEnemy(ACTOR_EN_IK)),
        },
    };
    Regions[RR_BENEATH_THE_GRAVEYARD_NIGHT_1_GRAVE] = RandoRegion{ .name = "Night 1 Grave", .sceneId = SCENE_HAKASHITA,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_1_EARLY_POT_01, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_1_EARLY_POT_02, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_CHEST, CanKillEnemy(ACTOR_EN_BAT)),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_1_BATS_POT_01, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_1_BATS_POT_02, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_1_BATS_POT_03, true),
            CHECK(RC_ENEMY_DROP_BAD_BAT, CanKillEnemy(ACTOR_EN_BAT)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 3),              ENTRANCE(BENEATH_THE_GRAVERYARD, 1), true),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_1_BOSS, CAN_LIGHT_TORCH_NEAR_ANOTHER),
        },
    };
    Regions[RR_BENEATH_THE_GRAVEYARD_NIGHT_2_BOSS] = RandoRegion{ .name = "Night 2 Boss", .sceneId = SCENE_HAKASHITA,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_PIECE_OF_HEART, CanKillEnemy(ACTOR_EN_IK)),
            CHECK(RC_ENEMY_DROP_IRON_KNUCKLE, CanKillEnemy(ACTOR_EN_IK)),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_2_GRAVE_AFTER_PIT, true),
        },
    };
    Regions[RR_BENEATH_THE_GRAVEYARD_NIGHT_2_GRAVE_AFTER_PIT] = RandoRegion{ .name = "Night 2 Grave After Pit", .sceneId = SCENE_HAKASHITA,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_AFTER_PIT_POT_01, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_AFTER_PIT_POT_02, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_AFTER_PIT_POT_03, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_AFTER_PIT_POT_04, true),
            CHECK(RC_ENEMY_DROP_SKULLTULA, CanKillEnemy(ACTOR_EN_ST)),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_2_BOSS, CAN_USE_EXPLOSIVE),
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_2_GRAVE_BEFORE_PIT, HAS_MAGIC && HAS_ITEM(ITEM_LENS_OF_TRUTH)),
        },
    };
    Regions[RR_BENEATH_THE_GRAVEYARD_NIGHT_2_GRAVE_BEFORE_PIT] = RandoRegion{ .name = "Night 2 Grave Before Pit", .sceneId = SCENE_HAKASHITA,
        .checks = {
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_01, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_02, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_03, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_04, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_05, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_06, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_07, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_EARLY_POT, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_BEFORE_PIT_POT_01, true),
            CHECK(RC_BENEATH_THE_GRAVEYARD_NIGHT_2_BEFORE_PIT_POT_02, true),
            CHECK(RC_ENEMY_DROP_KEESE, CanKillEnemy(ACTOR_EN_FIREFLY)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 2),              ENTRANCE(BENEATH_THE_GRAVERYARD, 0), true),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_GRAVEYARD_NIGHT_2_GRAVE_AFTER_PIT, HAS_MAGIC && HAS_ITEM(ITEM_LENS_OF_TRUTH)),
        },
    };
    Regions[RR_GHOST_HUT] = RandoRegion{ .sceneId = SCENE_TOUGITES,
        .checks = {
            CHECK(RC_IKANA_CANYON_GHOST_HUT_PIECE_OF_HEART, CHECK_MAX_HP(4) && CanKillEnemy(ACTOR_EN_PO_SISTERS)),
            CHECK(RC_ENEMY_DROP_POE_SISTER, CHECK_MAX_HP(4) && CanKillEnemy(ACTOR_EN_PO_SISTERS)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 1),                 ENTRANCE(GHOST_HUT, 0), true),
        },
    };
    Regions[RR_IKANA_CANYON_CAVE] = RandoRegion{ .name = "Cave", .sceneId = SCENE_IKANA,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 13),                ENTRANCE(IKANA_CANYON, 14), true),
        },
        .events = {
            EVENT(RE_BREAK_SHARP_CURSE, CAN_PLAY_SONG(STORMS)),
        },
    };
    Regions[RR_IKANA_CANYON_GROTTO] = RandoRegion{ .name = "Ikana Canyon Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_IKANA_CANYON_GROTTO_CHEST, true),
            CHECK(RC_IKANA_CANYON_GROTTO_GRASS_01, true),
            CHECK(RC_IKANA_CANYON_GROTTO_GRASS_02, true),
            CHECK(RC_IKANA_CANYON_GROTTO_GRASS_03, true),
            CHECK(RC_IKANA_CANYON_GROTTO_GRASS_04, true),
            CHECK(RC_IKANA_CANYON_GROTTO_GRASS_05, true),
            CHECK(RC_IKANA_CANYON_GROTTO_GRASS_06, true),
            CHECK(RC_IKANA_CANYON_GROTTO_GRASS_07, true),
            CHECK(RC_IKANA_CANYON_GROTTO_GRASS_08, true),
            CHECK(RC_IKANA_CANYON_GROTTO_GRASS_09, true),
            CHECK(RC_IKANA_CANYON_GROTTO_GRASS_10, true),
            CHECK(RC_IKANA_CANYON_GROTTO_GRASS_11, true),
            CHECK(RC_IKANA_CANYON_GROTTO_GRASS_12, true),
            CHECK(RC_IKANA_CANYON_GROTTO_GRASS_13, true),
            CHECK(RC_IKANA_CANYON_GROTTO_GRASS_14, true),
            CHECK(RC_ENEMY_DROP_MINI_BABA, CanKillEnemy(ACTOR_EN_KAREBABA)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 0),                 ENTRANCE(GROTTOS, 29), true),
        },
    };
    Regions[RR_IKANA_CANYON_LOWER] = RandoRegion{ .name = "Lower", .sceneId = SCENE_IKANA,
        .checks = {
            CHECK(RC_IKANA_CANYON_SCRUB_PIECE_OF_HEART, Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN) && CAN_BE_ZORA && CAN_BE_DEKU),
            CHECK(RC_IKANA_CANYON_SCRUB_HUGE_RUPEE, Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN) && CAN_BE_ZORA),
            CHECK(RC_IKANA_CANYON_SCRUB_POTION_REFILL, CUR_UPG_VALUE(UPG_WALLET) >= 1),
            CHECK(RC_ENEMY_DROP_OCTOROK, CanKillEnemy(ACTOR_EN_OKUTA) && CAN_USE_ABILITY(SWIM)),
            CHECK(RC_ENEMY_DROP_GARO, CanKillEnemy(ACTOR_EN_JSO)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GROTTOS, 29),                     ENTRANCE(IKANA_CANYON, 0), CAN_USE_ABILITY(SWIM)),
            EXIT(ENTRANCE(ROAD_TO_IKANA, 1),                ENTRANCE(IKANA_CANYON, 0), true), 
            /*              
                TODO : Sakon's Hideout is heavily flag based so we should check for those. Consider what I have in here now to be loose placeholders.
                Must NOT have helped the Bomb Shop old lady on this cycle(Kafei does not show up if you do, as Sakon never visits the shop to be followed.)
                Must have delivered the Letter to Kafei and met Kafei.(Sakon just does not show up otherwise, as odd as that may sound.)
            */
            EXIT(ENTRANCE(SAKONS_HIDEOUT, 0),               ENTRANCE(IKANA_CANYON, 6), RANDO_EVENTS[RE_MEET_KAFEI] && AT(TIME_NIGHT3_PM_06_00)),
            EXIT(ENTRANCE(SECRET_SHRINE, 0),                ENTRANCE(IKANA_CANYON, 12), CAN_USE_ABILITY(SWIM)),
            EXIT(ENTRANCE(SOUTHERN_SWAMP_POISONED, 9),               ONE_WAY_EXIT, CAN_USE_ABILITY(SWIM)),
        },
        .connections = {
            // Octorok soul not needed; the player can also create ice platforms on the water itself.
            CONNECTION(RR_IKANA_CANYON_UPPER, HAS_ITEM(ITEM_HOOKSHOT) && CAN_USE_MAGIC_ARROW(ICE)),
        },
        .events = {
            EVENT(RE_ACCESS_BLUE_POTION_REFILL, CUR_UPG_VALUE(UPG_WALLET) >= 1),
        },
    };
    Regions[RR_IKANA_CANYON_UPPER] = RandoRegion{ .name = "Upper", .sceneId = SCENE_IKANA,
        .checks = {
            CHECK(RC_IKANA_CANYON_GRASS_01, true),
            CHECK(RC_IKANA_CANYON_GRASS_02, true),
            CHECK(RC_IKANA_CANYON_GRASS_03, true),
            CHECK(RC_IKANA_CANYON_GRASS_04, true),
            CHECK(RC_IKANA_CANYON_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_IKANA_CANYON_TINGLE_MAP_01, CAN_USE_PROJECTILE && CAN_AFFORD(RC_IKANA_CANYON_TINGLE_MAP_01)),
            CHECK(RC_IKANA_CANYON_TINGLE_MAP_02, CAN_USE_PROJECTILE && CAN_AFFORD(RC_IKANA_CANYON_TINGLE_MAP_02)),
            CHECK(RC_ENEMY_DROP_GUAY, CanKillEnemy(ACTOR_EN_CROW) && IS_DAY()), // Day only
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GHOST_HUT, 0),                    ENTRANCE(IKANA_CANYON, 1), true),
            EXIT(ENTRANCE(MUSIC_BOX_HOUSE, 0),              ENTRANCE(IKANA_CANYON, 2), RANDO_EVENTS[RE_BREAK_SHARP_CURSE]),
            EXIT(ENTRANCE(STONE_TOWER, 0),                  ENTRANCE(IKANA_CANYON, 3), true),
            EXIT(ENTRANCE(BENEATH_THE_WELL, 0),             ENTRANCE(IKANA_CANYON, 5), true),
            EXIT(ENTRANCE(IKANA_CASTLE, 1),                 ENTRANCE(IKANA_CANYON, 8), true),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 4),               ENTRANCE(IKANA_CANYON, 11), true),
            EXIT(ENTRANCE(IKANA_CANYON, 14),                ENTRANCE(IKANA_CANYON, 13), true), // Cave
        },
        .connections = {
            // May consider cutting Deku and Goron from this since getting down as them may be seen as a trick. But its possible and is pretty easy to do.
            CONNECTION(RR_IKANA_CANYON_LOWER, true),
        },
        .events = {
            EVENT(RE_ACCESS_PICTOGRAPH_TINGLE, HAS_ITEM(ITEM_PICTOGRAPH_BOX)),
        },
        .oneWayEntrances = {
            ENTRANCE(IKANA_CANYON, 4), // From Song of Soaring
            ENTRANCE(IKANA_CANYON, 15), // From Stone Tower Temple Blue Warp
        }
    };
    Regions[RR_IKANA_GRAVEYARD_GROTTO] = RandoRegion{ .name = "Ikana Graveyard Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_IKANA_GRAVEYARD_GROTTO, true),
            CHECK(RC_IKANA_GRAVEYARD_GROTTO_GRASS_01, true),
            CHECK(RC_IKANA_GRAVEYARD_GROTTO_GRASS_02, true),
            CHECK(RC_IKANA_GRAVEYARD_GROTTO_GRASS_03, true),
            CHECK(RC_IKANA_GRAVEYARD_GROTTO_GRASS_04, true),
            CHECK(RC_IKANA_GRAVEYARD_GROTTO_GRASS_05, true),
            CHECK(RC_IKANA_GRAVEYARD_GROTTO_GRASS_06, true),
            CHECK(RC_IKANA_GRAVEYARD_GROTTO_GRASS_07, true),
            CHECK(RC_IKANA_GRAVEYARD_GROTTO_GRASS_08, true),
            CHECK(RC_IKANA_GRAVEYARD_GROTTO_GRASS_09, true),
            CHECK(RC_IKANA_GRAVEYARD_GROTTO_GRASS_10, true),
            CHECK(RC_IKANA_GRAVEYARD_GROTTO_GRASS_11, true),
            CHECK(RC_IKANA_GRAVEYARD_GROTTO_GRASS_12, true),
            CHECK(RC_IKANA_GRAVEYARD_GROTTO_GRASS_13, true),
            CHECK(RC_IKANA_GRAVEYARD_GROTTO_GRASS_14, true),
            CHECK(RC_ENEMY_DROP_MINI_BABA, CanKillEnemy(ACTOR_EN_KAREBABA)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 0),              ENTRANCE(GROTTOS, 30), true),
        },
    };
    Regions[RR_IKANA_GRAVEYARD_LOWER] = RandoRegion{ .name = "Lower", .sceneId = SCENE_BOTI,
        .checks = {
            CHECK(RC_IKANA_GRAVEYARD_LOWER_GRASS_01, true),
            CHECK(RC_IKANA_GRAVEYARD_LOWER_GRASS_02, true),
            CHECK(RC_IKANA_GRAVEYARD_LOWER_GRASS_03, true),
            CHECK(RC_IKANA_GRAVEYARD_LOWER_GRASS_04, true),
            CHECK(RC_IKANA_GRAVEYARD_LOWER_GRASS_05, true),
            CHECK(RC_IKANA_GRAVEYARD_GRASS_01, true),
            CHECK(RC_IKANA_GRAVEYARD_GRASS_02, true),
            CHECK(RC_IKANA_GRAVEYARD_GRASS_03, true),
            CHECK(RC_IKANA_GRAVEYARD_GRASS_04, true),
            CHECK(RC_IKANA_GRAVEYARD_GRASS_05, true),
            CHECK(RC_IKANA_GRAVEYARD_GRASS_06, true),
            CHECK(RC_IKANA_GRAVEYARD_GRASS_07, true),
            CHECK(RC_IKANA_GRAVEYARD_GRASS_08, true),
            CHECK(RC_IKANA_GRAVEYARD_GRASS_09, true),
            // Rocks can be picked up and thrown at these:
            CHECK(RC_IKANA_GRAVEYARD_WONDER_ITEM_01, true),
            CHECK(RC_IKANA_GRAVEYARD_WONDER_ITEM_02, true),
            CHECK(RC_IKANA_GRAVEYARD_WONDER_ITEM_03, true),
            CHECK(RC_IKANA_GRAVEYARD_WONDER_ITEM_04, true),
            CHECK(RC_IKANA_GRAVEYARD_WONDER_ITEM_05, true),
            CHECK(RC_IKANA_GRAVEYARD_WONDER_ITEM_06, true),
            CHECK(RC_IKANA_GRAVEYARD_WONDER_ITEM_07, true),
            CHECK(RC_IKANA_GRAVEYARD_WONDER_ITEM_08, true),
            CHECK(RC_IKANA_GRAVEYARD_WONDER_ITEM_09, true),
            CHECK(RC_IKANA_GRAVEYARD_WONDER_ITEM_10, true),
            CHECK(RC_IKANA_GRAVEYARD_WONDER_ITEM_11, true),
            CHECK(RC_IKANA_GRAVEYARD_WONDER_ITEM_12, true),
            CHECK(RC_ENEMY_DROP_STALCHILD, CanKillEnemy(ACTOR_EN_SKB) && IS_NIGHT()), // Night only
            CHECK(RC_ENEMY_DROP_BAD_BAT, CanKillEnemy(ACTOR_EN_BAT) && IS_DAY()), // Day only
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GROTTOS, 30),                     ENTRANCE(IKANA_GRAVEYARD, 0), CAN_USE_EXPLOSIVE),
            EXIT(ENTRANCE(ROAD_TO_IKANA, 2),                ENTRANCE(IKANA_GRAVEYARD, 0), true),
            EXIT(ENTRANCE(DAMPES_HOUSE, 0),                          ONE_WAY_EXIT, HAS_ITEM(ITEM_MASK_CAPTAIN) && IS_NIGHT3()), // Day 3 hole
            EXIT(ENTRANCE(BENEATH_THE_GRAVERYARD, 0),       ENTRANCE(IKANA_GRAVEYARD, 2), HAS_ITEM(ITEM_MASK_CAPTAIN) && IS_NIGHT2()), // Day 2 hole
            EXIT(ENTRANCE(BENEATH_THE_GRAVERYARD, 1),       ENTRANCE(IKANA_GRAVEYARD, 3), HAS_ITEM(ITEM_MASK_CAPTAIN) && IS_NIGHT1()), // Day 1 hole
        },
        .connections = {
            CONNECTION(RR_IKANA_GRAVEYARD_UPPER, CAN_PLAY_SONG(SONATA)),
        },
        .oneWayEntrances = {
            ENTRANCE(IKANA_GRAVEYARD, 4), // Exiting Dampe's house
        }
    };
    Regions[RR_IKANA_GRAVEYARD_UPPER] = RandoRegion{ .name = "Upper", .sceneId = SCENE_BOTI,
        .checks = {
            CHECK(RC_IKANA_GRAVEYARD_CAPTAIN_MASK, CanKillEnemy(ACTOR_EN_SKB) && CanKillEnemy(ACTOR_EN_BSB)),
            CHECK(RC_ENEMY_DROP_STALCHILD, CanKillEnemy(ACTOR_EN_SKB)),
            CHECK(RC_ENEMY_DROP_CAPTAIN_KEETA, CanKillEnemy(ACTOR_EN_SKB) && CanKillEnemy(ACTOR_EN_BSB)),
        },
        .connections = {
            CONNECTION(RR_IKANA_GRAVEYARD_LOWER, true)
        },
    };
    Regions[RR_IKANA_GREAT_FAIRY_FOUNTAIN] = RandoRegion{ .name = "Ikana", .sceneId = SCENE_YOUSEI_IZUMI,
        .checks = {
            CHECK(RC_IKANA_GREAT_FAIRY, HAS_ENOUGH_STRAY_FAIRIES(DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 11),                ENTRANCE(FAIRY_FOUNTAIN, 4), true),
        },
    };
    Regions[RR_MUSIC_BOX_HOUSE] = RandoRegion{ .sceneId = SCENE_MUSICHOUSE,
        .checks = {
            CHECK(RC_MUSIC_BOX_HOUSE_FATHER, CAN_PLAY_SONG(HEALING)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 2),                 ENTRANCE(MUSIC_BOX_HOUSE, 0), true),
        },
    };
    Regions[RR_ROAD_TO_IKANA_ABOVE_LEDGE] = RandoRegion{ .name = "Above Ledge", .sceneId = SCENE_IKANAMAE,
        .checks = {
            CHECK(RC_ENEMY_DROP_NEJIRON, CanKillEnemy(ACTOR_EN_BAGUO) && IS_DAY()), // Day only
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 0),                 ENTRANCE(ROAD_TO_IKANA, 1), true),
        },
        .connections = {
            CONNECTION(RR_ROAD_TO_IKANA_BELOW_LEDGE, true),
        },
    };
    Regions[RR_ROAD_TO_IKANA_BELOW_LEDGE] = RandoRegion{ .name = "Below Ledge", .sceneId = SCENE_IKANAMAE,
        .checks = {
            CHECK(RC_ROAD_TO_IKANA_POT, CAN_HOOK_SCARECROW),
            CHECK(RC_ROAD_TO_IKANA_STONE_MASK, HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_MAGIC && HAS_BOTTLE && (CAN_ACCESS(RED_POTION_REFILL) || CAN_ACCESS(BLUE_POTION_REFILL))),
            CHECK(RC_ENEMY_DROP_BLUE_BUBBLE, CanKillEnemy(ACTOR_EN_BB) && IS_NIGHT()), // Night only
            CHECK(RC_ENEMY_DROP_REAL_BOMBCHU, CanKillEnemy(ACTOR_EN_RAT) && IS_DAY()), // Day only
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_GRAVEYARD, 0),              ENTRANCE(ROAD_TO_IKANA, 2), true)
        },
        .connections = {
            CONNECTION(RR_ROAD_TO_IKANA_FIELD_SIDE, CAN_RIDE_EPONA),
            CONNECTION(RR_ROAD_TO_IKANA_ABOVE_LEDGE, HAS_ITEM(ITEM_HOOKSHOT) && (HAS_ITEM(ITEM_MASK_GARO) || HAS_ITEM(ITEM_MASK_GIBDO))),
        },
    };
    Regions[RR_ROAD_TO_IKANA_FIELD_SIDE] = RandoRegion{ .name = "Field Side", .sceneId = SCENE_IKANAMAE,
        .checks = {
            CHECK(RC_ROAD_TO_IKANA_CHEST, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_ENEMY_DROP_BLUE_BUBBLE, CanKillEnemy(ACTOR_EN_BB) && IS_NIGHT()), // Night only
            CHECK(RC_ENEMY_DROP_REAL_BOMBCHU, CanKillEnemy(ACTOR_EN_RAT) && IS_DAY()), // Day only
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(TERMINA_FIELD, 4),                ENTRANCE(ROAD_TO_IKANA, 0), true),
            EXIT(ENTRANCE(GROTTOS, 31),                     ENTRANCE(ROAD_TO_IKANA, 0), CAN_BE_GORON),
        },
        .connections = {
            CONNECTION(RR_ROAD_TO_IKANA_BELOW_LEDGE, CAN_RIDE_EPONA),
        },
    };
    Regions[RR_ROAD_TO_IKANA_GROTTO] = RandoRegion{ .name = "Road to Ikana Grotto", .sceneId = SCENE_KAKUSIANA,
        .checks = {
            CHECK(RC_ROAD_TO_IKANA_GROTTO_CHEST, true),
            CHECK(RC_ROAD_TO_IKANA_GROTTO_GRASS_01, true),
            CHECK(RC_ROAD_TO_IKANA_GROTTO_GRASS_02, true),
            CHECK(RC_ROAD_TO_IKANA_GROTTO_GRASS_03, true),
            CHECK(RC_ROAD_TO_IKANA_GROTTO_GRASS_04, true),
            CHECK(RC_ROAD_TO_IKANA_GROTTO_GRASS_05, true),
            CHECK(RC_ROAD_TO_IKANA_GROTTO_GRASS_06, true),
            CHECK(RC_ROAD_TO_IKANA_GROTTO_GRASS_07, true),
            CHECK(RC_ROAD_TO_IKANA_GROTTO_GRASS_08, true),
            CHECK(RC_ROAD_TO_IKANA_GROTTO_GRASS_09, true),
            CHECK(RC_ROAD_TO_IKANA_GROTTO_GRASS_10, true),
            CHECK(RC_ROAD_TO_IKANA_GROTTO_GRASS_11, true),
            CHECK(RC_ROAD_TO_IKANA_GROTTO_GRASS_12, true),
            CHECK(RC_ROAD_TO_IKANA_GROTTO_GRASS_13, true),
            CHECK(RC_ROAD_TO_IKANA_GROTTO_GRASS_14, true),
            CHECK(RC_ENEMY_DROP_MINI_BABA, CanKillEnemy(ACTOR_EN_KAREBABA)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(ROAD_TO_IKANA, 0),                ENTRANCE(GROTTOS, 31), true),
        },
    };
    Regions[RR_SAKON_HIDEOUT] = RandoRegion{ .sceneId = SCENE_SECOM,
        .checks = {
            CHECK(RC_SAKON_HIDEOUT_FIRST_ROOM_POT_01,    true),
            CHECK(RC_SAKON_HIDEOUT_FIRST_ROOM_POT_02,    true),
            CHECK(RC_SAKON_HIDEOUT_SECOND_ROOM_POT_01,   true),
            CHECK(RC_SAKON_HIDEOUT_SECOND_ROOM_POT_02,   true),
            CHECK(RC_SAKON_HIDEOUT_THIRD_ROOM_POT,       true),
            CHECK(RC_ENEMY_DROP_DEKU_BABA,               CanKillEnemy(ACTOR_EN_DEKUBABA)),
            CHECK(RC_ENEMY_DROP_WOLFOS,                  CanKillEnemy(ACTOR_EN_WF)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 6),                 ENTRANCE(SAKONS_HIDEOUT, 0), true),
        },
        .events = {
            EVENT(RE_RETRIEVE_SUN_MASK, CanKillEnemy(ACTOR_EN_WF)),
        },
    };
    Regions[RR_SECRET_SHRINE_ENTRANCE] = RandoRegion{ .name = "Entrance", .sceneId = SCENE_RANDOM,
        .checks = {
            CHECK(RC_SECRET_SHRINE_ENTRANCE_GRASS_01, true),
            CHECK(RC_SECRET_SHRINE_ENTRANCE_GRASS_02, true),
            CHECK(RC_SECRET_SHRINE_ENTRANCE_GRASS_03, true),
            CHECK(RC_SECRET_SHRINE_ENTRANCE_GRASS_04, true),
            CHECK(RC_SECRET_SHRINE_ENTRANCE_GRASS_05, true),
            CHECK(RC_SECRET_SHRINE_ENTRANCE_GRASS_06, true),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_01, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_02, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_03, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_04, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_05, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_06, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_07, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_08, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_09, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_10, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_11, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_12, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_13, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_14, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_15, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_16, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_FREESTANDING_RUPEE_17, CAN_GROW_BEAN_PLANT || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_01, true),
            CHECK(RC_SECRET_SHRINE_POT_02, true),
            CHECK(RC_SECRET_SHRINE_POT_03, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 12),                ENTRANCE(SECRET_SHRINE, 0), true),
        },
        .connections = {
            CONNECTION(RR_SECRET_SHRINE, HAS_MAGIC && HAS_ITEM(ITEM_ARROW_LIGHT) && HAS_ITEM(ITEM_BOW))
        }
    };
    Regions[RR_SECRET_SHRINE] = RandoRegion{ .sceneId = SCENE_RANDOM,
        .checks = {
            CHECK(RC_SECRET_SHRINE_DINOLFOS_GRASS_01, true),
            CHECK(RC_SECRET_SHRINE_DINOLFOS_GRASS_02, true),
            CHECK(RC_SECRET_SHRINE_DINOLFOS_GRASS_03, true),
            CHECK(RC_SECRET_SHRINE_DINOLFOS_GRASS_04, true),
            CHECK(RC_SECRET_SHRINE_WIZZROBE_GRASS_01, true),
            CHECK(RC_SECRET_SHRINE_WIZZROBE_GRASS_02, true),
            CHECK(RC_SECRET_SHRINE_WIZZROBE_GRASS_03, true),
            CHECK(RC_SECRET_SHRINE_WIZZROBE_GRASS_04, true),
            CHECK(RC_SECRET_SHRINE_WIZZROBE_GRASS_05, true),
            CHECK(RC_SECRET_SHRINE_WART_GRASS_01, true),
            CHECK(RC_SECRET_SHRINE_WART_GRASS_02, true),
            CHECK(RC_SECRET_SHRINE_WART_GRASS_03, true),
            CHECK(RC_SECRET_SHRINE_WART_GRASS_04, true),
            CHECK(RC_SECRET_SHRINE_WART_GRASS_05, true),
            CHECK(RC_SECRET_SHRINE_WART_GRASS_06, true),
            CHECK(RC_SECRET_SHRINE_WART_GRASS_07, true),
            CHECK(RC_SECRET_SHRINE_WART_GRASS_08, true),
            CHECK(RC_SECRET_SHRINE_GARO_MASTER_GRASS_01, true),
            CHECK(RC_SECRET_SHRINE_GARO_MASTER_GRASS_02, true),
            CHECK(RC_SECRET_SHRINE_GARO_MASTER_GRASS_03, true),
            CHECK(RC_SECRET_SHRINE_GARO_MASTER_GRASS_04, true),
            CHECK(RC_SECRET_SHRINE_GARO_MASTER_GRASS_05, true),
            CHECK(RC_SECRET_SHRINE_GARO_MASTER_GRASS_06, true),
            CHECK(RC_SECRET_SHRINE_DINOLFOS_CHEST, RANDO_EVENTS[RE_SECRET_SHRINE_DINOLFOS]),
            CHECK(RC_SECRET_SHRINE_WIZZROBE_CHEST, RANDO_EVENTS[RE_SECRET_SHRINE_WIZZROBE]),
            CHECK(RC_SECRET_SHRINE_WART_CHEST, RANDO_EVENTS[RE_SECRET_SHRINE_WART]),
            CHECK(RC_SECRET_SHRINE_GARO_MASTER_CHEST, RANDO_EVENTS[RE_SECRET_SHRINE_GARO_MASTER]),
            CHECK(RC_SECRET_SHRINE_PIECE_OF_HEART_CHEST, RANDO_EVENTS[RE_SECRET_SHRINE_DINOLFOS] && RANDO_EVENTS[RE_SECRET_SHRINE_WIZZROBE] && RANDO_EVENTS[RE_SECRET_SHRINE_WART] && RANDO_EVENTS[RE_SECRET_SHRINE_GARO_MASTER]),
            CHECK(RC_SECRET_SHRINE_POT_04, (CAN_USE_PROJECTILE && CAN_USE_ABILITY(SWIM)) || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_05, (CAN_USE_PROJECTILE && CAN_USE_ABILITY(SWIM)) || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_06, (CAN_USE_PROJECTILE && CAN_USE_ABILITY(SWIM)) || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_07, (CAN_USE_PROJECTILE && CAN_USE_ABILITY(SWIM)) || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_08, (CAN_USE_PROJECTILE && CAN_USE_ABILITY(SWIM)) || CAN_BE_ZORA),
            CHECK(RC_SECRET_SHRINE_POT_09, (CAN_USE_PROJECTILE && CAN_USE_ABILITY(SWIM)) || CAN_BE_ZORA),
            CHECK(RC_ENEMY_DROP_GARO_MASTER, CanKillEnemy(ACTOR_EN_JSO2)),
            CHECK(RC_ENEMY_DROP_WIZROBE, CanKillEnemy(ACTOR_EN_WIZ)),
            CHECK(RC_ENEMY_DROP_WART, CanKillEnemy(ACTOR_BOSS_04)),
            CHECK(RC_ENEMY_DROP_DINOLFOS, CanKillEnemy(ACTOR_EN_DINOFOS)),
        },
        .connections = {
            CONNECTION(RR_SECRET_SHRINE_ENTRANCE, true),
        },
        .events = {
            // TODO: Allow opting in to health checks
            EVENT(RE_SECRET_SHRINE_DINOLFOS, /* CHECK_MAX_HP(4) && */ CanKillEnemy(ACTOR_EN_DINOFOS)),
            EVENT(RE_SECRET_SHRINE_WIZZROBE, /* CHECK_MAX_HP(8) && */ CanKillEnemy(ACTOR_EN_WIZ)),
            EVENT(RE_SECRET_SHRINE_WART, /* CHECK_MAX_HP(12) && */ CanKillEnemy(ACTOR_BOSS_04)),
            EVENT(RE_SECRET_SHRINE_GARO_MASTER, /* CHECK_MAX_HP(16) && */ CanKillEnemy(ACTOR_EN_JSO2)),
        },
    };
    Regions[RR_STONE_TOWER_BOTTOM] = RandoRegion{ .name = "Bottom", .sceneId = SCENE_F40,
        .checks = {
            CHECK(RC_STONE_TOWER_CLIMB_POT_01, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_STONE_TOWER_CLIMB_POT_02, HAS_ITEM(ITEM_HOOKSHOT)),
            CHECK(RC_ENEMY_DROP_BEAMOS, CanKillEnemy(ACTOR_EN_VM)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 3),                 ENTRANCE(STONE_TOWER, 0), true)
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_MIDDLE, HAS_ITEM(ITEM_HOOKSHOT) && CAN_PLAY_SONG(ELEGY) && CAN_BE_GORON && CAN_BE_ZORA),
        },
    };
    Regions[RR_STONE_TOWER_INVERTED_LOWER] = RandoRegion{ .sceneId = SCENE_F41,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(STONE_TOWER, 1),                  ENTRANCE(STONE_TOWER_INVERTED, 0), HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_LIGHT) && HAS_MAGIC),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_INVERTED_UPPER, CAN_GROW_BEAN_PLANT),
            CONNECTION(RR_STONE_TOWER_INVERTED_NEAR_TEMPLE, true),
        },
    };
    Regions[RR_STONE_TOWER_INVERTED_NEAR_TEMPLE] = RandoRegion{ .sceneId = SCENE_F41,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(STONE_TOWER_TEMPLE_INVERTED, 0),  ENTRANCE(STONE_TOWER_INVERTED, 1), true),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_INVERTED_LOWER, true),
        },
    };
    Regions[RR_STONE_TOWER_INVERTED_UPPER] = RandoRegion{ .sceneId = SCENE_F41,
        .checks = {
            CHECK(RC_STONE_TOWER_INVERTED_CHEST_01, true),
            CHECK(RC_STONE_TOWER_INVERTED_CHEST_02, true),
            CHECK(RC_STONE_TOWER_INVERTED_CHEST_03, true),
            // These pots don't seem to work, no clue why.
            CHECK(RC_STONE_TOWER_INVERTED_POT_01, true),
            CHECK(RC_STONE_TOWER_INVERTED_POT_02, true),
            CHECK(RC_STONE_TOWER_INVERTED_POT_03, true),
            CHECK(RC_STONE_TOWER_INVERTED_POT_04, true),
            CHECK(RC_STONE_TOWER_INVERTED_POT_05, true),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_INVERTED_LOWER, true),
        }
    };
    Regions[RR_STONE_TOWER_MIDDLE] = RandoRegion{ .name = "Middle", .sceneId = SCENE_F40,
        .checks = {
            CHECK(RC_STONE_TOWER_LOWER_SCARECROW_POT_01, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_LOWER_SCARECROW_POT_02, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_LOWER_SCARECROW_POT_03, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_LOWER_SCARECROW_POT_04, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_LOWER_SCARECROW_POT_05, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_LOWER_SCARECROW_POT_06, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_LOWER_SCARECROW_POT_07, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_LOWER_SCARECROW_POT_08, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_LOWER_SCARECROW_POT_09, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_LOWER_SCARECROW_POT_10, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_LOWER_SCARECROW_POT_11, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_LOWER_SCARECROW_POT_12, CAN_HOOK_SCARECROW),
            CHECK(RC_ENEMY_DROP_BEAMOS, CanKillEnemy(ACTOR_EN_VM)),
            CHECK(RC_ENEMY_DROP_KEESE, CanKillEnemy(ACTOR_EN_FIREFLY)),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_BOTTOM, HAS_ITEM(ITEM_HOOKSHOT) && CAN_PLAY_SONG(ELEGY) && CAN_BE_GORON && CAN_BE_ZORA),
            CONNECTION(RR_STONE_TOWER_UPPER, HAS_ITEM(ITEM_HOOKSHOT)),
        },
    };
    // TODO: Probably need to split this up to account for entrance rando later
    Regions[RR_STONE_TOWER_TOP] = RandoRegion{ .name = "Top", .sceneId = SCENE_F40,
        .checks = {
            CHECK(RC_STONE_TOWER_OWL_STATUE, CAN_USE_SWORD),
            CHECK(RC_STONE_TOWER_OWL_STATUE_POT_01, true),
            CHECK(RC_STONE_TOWER_OWL_STATUE_POT_02, true),
            CHECK(RC_STONE_TOWER_OWL_STATUE_POT_03, true),
            CHECK(RC_STONE_TOWER_OWL_STATUE_POT_04, true),
            CHECK(RC_ENEMY_DROP_KEESE, CanKillEnemy(ACTOR_EN_FIREFLY)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(STONE_TOWER_INVERTED, 0),         ENTRANCE(STONE_TOWER, 1), CAN_PLAY_SONG(ELEGY) && HAS_ITEM(ITEM_BOW) && HAS_ITEM(ITEM_ARROW_LIGHT) && HAS_MAGIC),
            EXIT(ENTRANCE(STONE_TOWER_TEMPLE, 0),           ENTRANCE(STONE_TOWER, 2), CAN_BE_ZORA && CAN_BE_GORON && CAN_PLAY_SONG(ELEGY))
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_UPPER, HAS_ITEM(ITEM_HOOKSHOT)),
        },
        .oneWayEntrances = {
            ENTRANCE(STONE_TOWER, 3), // From Song of Soaring
        }
    };
    Regions[RR_STONE_TOWER_UPPER] = RandoRegion{ .name = "Upper", .sceneId = SCENE_F40,
        .checks = {
            CHECK(RC_STONE_TOWER_HIGHER_SCARECROW_POT_01, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_HIGHER_SCARECROW_POT_02, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_HIGHER_SCARECROW_POT_03, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_HIGHER_SCARECROW_POT_04, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_HIGHER_SCARECROW_POT_05, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_HIGHER_SCARECROW_POT_06, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_HIGHER_SCARECROW_POT_07, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_HIGHER_SCARECROW_POT_08, CAN_HOOK_SCARECROW),
            CHECK(RC_STONE_TOWER_HIGHER_SCARECROW_POT_09, CAN_HOOK_SCARECROW),
            CHECK(RC_ENEMY_DROP_KEESE, CanKillEnemy(ACTOR_EN_FIREFLY)),
            CHECK(RC_ENEMY_DROP_REDEAD, CanKillEnemy(ACTOR_EN_RD)),
        },
        .connections = {
            CONNECTION(RR_STONE_TOWER_MIDDLE, HAS_ITEM(ITEM_HOOKSHOT)),
            CONNECTION(RR_STONE_TOWER_TOP, HAS_ITEM(ITEM_HOOKSHOT))
        },
    };
}, {});
// clang-format on
