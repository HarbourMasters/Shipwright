#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_PIRATES_FORTRESS_CAPTAIN_ROOM_UPPER] = RandoRegion{ .name = "Captain Room Upper", .sceneId = SCENE_PIRATE,
        // TODO : If NTSC JP 1.0 support is added we should add a connection here to RR_PIRATES_FORTRESS_CAPTAIN_ROOM and a unique flag to check for it...or ignore it and let the player go the long way around
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_BEEHIVE_POT_01, true),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_BEEHIVE_POT_02, true),
            CHECK(RC_PIRATE_FORTRESS_CAPTAIN_ROOM_UPPER_BEEHIVE, CAN_USE_PROJECTILE),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 2),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 1), true),
        },
        .events = {
            EVENT(RE_PIRATE_FORTRESS_BEEHIVE_HIT, (HAS_ITEM(ITEM_BOW) || (CAN_BE_DEKU && HAS_MAGIC))),
        },
    };
    Regions[RR_PIRATES_FORTRESS_CAPTAIN_ROOM] = RandoRegion{ .name = "Captain Room", .sceneId = SCENE_PIRATE,
        .checks = {
            // TODO: Zora Egg Here
            CHECK(RC_PIRATE_FORTRESS_CAPTAIN_ROOM_BARREL_01, RANDO_EVENTS[RE_PIRATE_FORTRESS_BEEHIVE_HIT]),
            CHECK(RC_PIRATE_FORTRESS_CAPTAIN_ROOM_BARREL_02, RANDO_EVENTS[RE_PIRATE_FORTRESS_BEEHIVE_HIT]),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_HOOKSHOT_CHEST, RANDO_EVENTS[RE_PIRATE_FORTRESS_BEEHIVE_HIT]),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_WONDER_ITEM_01, RANDO_EVENTS[RE_PIRATE_FORTRESS_BEEHIVE_HIT] && CAN_USE_PROJECTILE),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_WONDER_ITEM_02, RANDO_EVENTS[RE_PIRATE_FORTRESS_BEEHIVE_HIT] && CAN_USE_PROJECTILE),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_WONDER_ITEM_03, RANDO_EVENTS[RE_PIRATE_FORTRESS_BEEHIVE_HIT] && CAN_USE_PROJECTILE),
            CHECK(RC_ENEMY_DROP_SHELLBLADE, CanKillEnemy(ACTOR_EN_SB)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 1),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 0), true),
        },
        .events = {
            EVENT(RE_ACCESS_ZORA_EGG, HAS_ITEM(ITEM_HOOKSHOT) && HAS_BOTTLE && CAN_BE_ZORA && CAN_USE_ABILITY(SWIM)),
        },
    };
    Regions[RR_PIRATES_FORTRESS_INSIDE_3_GUARD_ROOM] = RandoRegion{ .name = "3 Guard Room", .sceneId = SCENE_PIRATE,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_SILVER_RUPEE_CHEST, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 3),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 2), true),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_PURPLE_GUARD, true),
        },
        .events = {
            EVENT(RE_ACCESS_PIRATE_PICTURE, true),
        },
    };
    Regions[RR_PIRATES_FORTRESS_INSIDE_CHEST_EGG_ROOM] = RandoRegion{ .name = "Chest Egg Room", .sceneId = SCENE_PIRATE,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_AQUARIUM_CHEST, HAS_ITEM(ITEM_HOOKSHOT) && CAN_BE_ZORA && CAN_USE_ABILITY(SWIM)),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_CHEST_AQUARIUM_POT_01, true),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_CHEST_AQUARIUM_POT_02, true),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_CHEST_AQUARIUM_POT_03, true),
            CHECK(RC_ENEMY_DROP_DESBREKO, CanKillEnemy(ACTOR_EN_PR)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 8),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 7), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_ORANGE_GUARD, true),
        },
        .events = {
            EVENT(RE_ACCESS_ZORA_EGG, HAS_ITEM(ITEM_HOOKSHOT) && HAS_BOTTLE && CAN_BE_ZORA && CAN_USE_ABILITY(SWIM)),
        },
    };
    Regions[RR_PIRATES_FORTRESS_INSIDE_GREEN_GUARD] = RandoRegion{ .name = "Green Guard Room", .sceneId = SCENE_PIRATE,
        .checks = {
            CHECK(RC_ENEMY_DROP_PIRATE, CanKillEnemy(ACTOR_EN_KAIZOKU)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 5),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 4), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_MAZE_GUARD, CanKillEnemy(ACTOR_EN_KAIZOKU)),
            CONNECTION(RR_PIRATES_FORTRESS_RIGHT_CLAM_EGG_ROOM, CanKillEnemy(ACTOR_EN_KAIZOKU)),
        }
    };
    Regions[RR_PIRATES_FORTRESS_INSIDE_LINE_GUARD] = RandoRegion{ .name = "Line Guard Room", .sceneId = SCENE_PIRATE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 7),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 7), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_ORANGE_GUARD, true),
        },
        .events = {
            EVENT(RE_ACCESS_PIRATE_PICTURE, true),
        },
    };
    Regions[RR_PIRATES_FORTRESS_INSIDE_MAZE_GUARD] = RandoRegion{ .name = "Maze Room", .sceneId = SCENE_PIRATE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 5),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 4), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_GREEN_GUARD, true),
        },
        .events = {
            EVENT(RE_ACCESS_PIRATE_PICTURE, true),
        },
    };
    Regions[RR_PIRATES_FORTRESS_INSIDE_ORANGE_GUARD] = RandoRegion{ .name = "Orange Guard Room", .sceneId = SCENE_PIRATE,
        .checks = {
            CHECK(RC_ENEMY_DROP_PIRATE, CanKillEnemy(ACTOR_EN_KAIZOKU)),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_LINE_GUARD, CanKillEnemy(ACTOR_EN_KAIZOKU)),
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_CHEST_EGG_ROOM, CanKillEnemy(ACTOR_EN_KAIZOKU)),
        }
    };
    Regions[RR_PIRATES_FORTRESS_INSIDE_PURPLE_GUARD] = RandoRegion{ .name = "Purple Guard Room", .sceneId = SCENE_PIRATE,
        .checks = {
            CHECK(RC_ENEMY_DROP_PIRATE, CanKillEnemy(ACTOR_EN_KAIZOKU)),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_3_GUARD_ROOM, CanKillEnemy(ACTOR_EN_KAIZOKU)),
            CONNECTION(RR_PIRATES_FORTRESS_LEFT_CLAM_EGG_ROOM, CanKillEnemy(ACTOR_EN_KAIZOKU)),
        }
    };
    Regions[RR_PIRATES_FORTRESS_LEFT_CLAM_EGG_ROOM] = RandoRegion{ .name = "Left Clam Room", .sceneId = SCENE_PIRATE,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_GUARDED_BARREL, true),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_GUARDED_POT_01, true),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_GUARDED_POT_02, true),
            CHECK(RC_ENEMY_DROP_SHELLBLADE, CanKillEnemy(ACTOR_EN_SB)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 4),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 3), true),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_LEFT_CLAM_EGG_ROOM, true),
        },
        .events = {
            EVENT(RE_ACCESS_ZORA_EGG, HAS_ITEM(ITEM_HOOKSHOT) && HAS_BOTTLE && CAN_BE_ZORA && CAN_USE_ABILITY(SWIM)),
        },
    };
    Regions[RR_PIRATES_FORTRESS_LEFT_PLATFORM] = RandoRegion{ .name = "Left Platform", .sceneId = SCENE_KAIZOKU,
        // The drop down from the LEFT_CLAM_EGG_ROOM
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 3),    ENTRANCE(PIRATES_FORTRESS, 4), true),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PLAZA, true),
        }
    };
    Regions[RR_PIRATES_FORTRESS_MOAT_HIGHER] = RandoRegion{ .name = "Higher", .sceneId = SCENE_TORIDE,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 0),             ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 1), CAN_USE_ABILITY(SWIM)),
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 10),   ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 6), CAN_USE_ABILITY(SWIM)),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_MOAT_LOWER, CAN_USE_ABILITY(SWIM)),
        },
        .oneWayEntrances = {
            ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 4), // From being captured in the inner part of Pirate Fortress
        },
    };
    Regions[RR_PIRATES_FORTRESS_MOAT_LOWER] = RandoRegion{ .name = "Lower", .sceneId = SCENE_TORIDE,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_ENTRANCE_CHEST_01, CAN_BE_ZORA && CAN_USE_ABILITY(SWIM)),
            CHECK(RC_PIRATE_FORTRESS_ENTRANCE_CHEST_02, CAN_BE_ZORA && CAN_USE_ABILITY(SWIM)),
            CHECK(RC_PIRATE_FORTRESS_ENTRANCE_CHEST_03, CAN_BE_ZORA && CAN_USE_ABILITY(SWIM)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(GREAT_BAY_COAST, 5),              ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 0), CAN_BE_ZORA && CAN_USE_ABILITY(SWIM)),
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 9),    ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 2), CAN_BE_ZORA && CAN_USE_ABILITY(SWIM) && CAN_BE_GORON),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_MOAT_HIGHER, HAS_ITEM(ITEM_HOOKSHOT) && CAN_USE_ABILITY(SWIM)),
        },
        .events = {
            EVENT(RE_ACCESS_PIRATE_PICTURE, true),
        },
        .oneWayEntrances = {
            ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 3), // Two steams in "RR_PIRATES_FORTRESS_SEWERS_PREGATE" and "RR_PIRATES_FORTRESS_SEWERS_POSTGATE"
        }
    };
    Regions[RR_PIRATES_FORTRESS_MOAT_PLATFORM] = RandoRegion{ .name = "Platform", .sceneId = SCENE_TORIDE,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_ENTRANCE_BARREL, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 12),            ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 5), true),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_MOAT_LOWER, CAN_USE_ABILITY(SWIM)),
        },
    };
    Regions[RR_PIRATES_FORTRESS_PLAZA_LEFT_EXIT] = RandoRegion{ .name = "Left Side Exit", .sceneId = SCENE_KAIZOKU,
        // The doorway when exiting the CHEST_EGG_ROOM, one way jump down to PLAZA
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 7),    ENTRANCE(PIRATES_FORTRESS, 8), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PLAZA, true),
        }
    };
    Regions[RR_PIRATES_FORTRESS_PLAZA_LEFT_LOWER] = RandoRegion{ .name = "Plaza Left Lower", .sceneId = SCENE_KAIZOKU,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 7),    ENTRANCE(PIRATES_FORTRESS, 7), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PLAZA, true),
            CONNECTION(RR_PIRATES_FORTRESS_PLAZA_LEFT_UPPER, HAS_ITEM(ITEM_HOOKSHOT))
        }
    };
    Regions[RR_PIRATES_FORTRESS_PLAZA_LEFT_UPPER] = RandoRegion{ .name = "Plaza Left Upper", .sceneId = SCENE_KAIZOKU,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_PLAZA_BARREL,  true),
            CHECK(RC_PIRATE_FORTRESS_PLAZA_FREESTANDING_HEART_01, true),
            CHECK(RC_PIRATE_FORTRESS_PLAZA_FREESTANDING_HEART_02, true),
            CHECK(RC_PIRATE_FORTRESS_PLAZA_FREESTANDING_HEART_03, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 2),    ENTRANCE(PIRATES_FORTRESS, 3), true),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PLAZA, true),
            CONNECTION(RR_PIRATES_FORTRESS_PLAZA_LEFT_LOWER, HAS_ITEM(ITEM_HOOKSHOT))
        },
        .events = {
            EVENT(RE_ACCESS_PIRATE_PICTURE, true),
        },
    };
    Regions[RR_PIRATES_FORTRESS_PLAZA_RIGHT_EXIT] = RandoRegion{ .name = "Right Side Exit", .sceneId = SCENE_KAIZOKU,
        // The doorway when exiting the RIGHT_CLAM_EGG_ROOM, one way jump down to PLAZA
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 5),    ENTRANCE(PIRATES_FORTRESS, 6), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PLAZA, true),
        }
    };
    Regions[RR_PIRATES_FORTRESS_PLAZA_RIGHT] = RandoRegion{ .name = "Right Side", .sceneId = SCENE_KAIZOKU,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_PLAZA_UPPER_CHEST, HAS_ITEM(ITEM_HOOKSHOT)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 5),    ENTRANCE(PIRATES_FORTRESS, 12), true),
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 4),    ENTRANCE(PIRATES_FORTRESS, 5), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PLAZA, true),
        }
    };
    Regions[RR_PIRATES_FORTRESS_PLAZA_TOWER] = RandoRegion{ .name = "Plaza Tower", .sceneId = SCENE_KAIZOKU,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_PLAZA_LARGE_CRATE_02, true),
            CHECK(RC_PIRATE_FORTRESS_PLAZA_LARGE_CRATE_03, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 1),    ENTRANCE(PIRATES_FORTRESS, 2), true),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PLAZA, true),
        },
        .events = {
            EVENT(RE_ACCESS_PIRATE_PICTURE, true),
        },
    };
    Regions[RR_PIRATES_FORTRESS_PLAZA] = RandoRegion{ .name = "Plaza", .sceneId = SCENE_KAIZOKU,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_PLAZA_LARGE_CRATE_01,  true),
            CHECK(RC_PIRATE_FORTRESS_PLAZA_LOWER_CHEST,     true),
            CHECK(RC_PIRATE_FORTRESS_PLAZA_WONDER_ITEM_01,  CAN_USE_PROJECTILE),
            CHECK(RC_PIRATE_FORTRESS_PLAZA_WONDER_ITEM_02,  CAN_USE_PROJECTILE),
            CHECK(RC_PIRATE_FORTRESS_PLAZA_WONDER_ITEM_03,  CAN_USE_PROJECTILE),
            CHECK(RC_PIRATE_FORTRESS_PLAZA_WONDER_ITEM_04,  CAN_USE_PROJECTILE),
            CHECK(RC_PIRATE_FORTRESS_PLAZA_WONDER_ITEM_05,  CAN_USE_PROJECTILE),
            CHECK(RC_PIRATE_FORTRESS_PLAZA_WONDER_ITEM_06,  CAN_USE_PROJECTILE),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_INTERIOR, 0),    ENTRANCE(PIRATES_FORTRESS, 1), true),
            EXIT(ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 1),    ENTRANCE(PIRATES_FORTRESS, 0), true),
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_PLAZA_LEFT_LOWER, HAS_ITEM(ITEM_HOOKSHOT)),
            CONNECTION(RR_PIRATES_FORTRESS_PLAZA_RIGHT, HAS_ITEM(ITEM_HOOKSHOT)),
            // Outside of using Stones Mask you have to deal with this guard in some way.
            CONNECTION(RR_PIRATES_FORTRESS_PLAZA_TOWER, (
                (HAS_ITEM(ITEM_DEKU_NUT) || HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || HAS_ITEM(ITEM_MASK_STONE)) ||
                (CAN_BE_DEKU && HAS_MAGIC) || CAN_BE_ZORA
            )),
        },
        .events = {
            EVENT(RE_ACCESS_PIRATE_PICTURE, true),
        },
    };
    Regions[RR_PIRATES_FORTRESS_RIGHT_CLAM_EGG_ROOM] = RandoRegion{ .name = "Right Clam Room", .sceneId = SCENE_PIRATE,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_BARREL_MAZE_POT_01, true),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_BARREL_MAZE_POT_02, true),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_BARREL_MAZE_POT_03, true),
            CHECK(RC_ENEMY_DROP_SHELLBLADE, CanKillEnemy(ACTOR_EN_SB)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS, 6),             ENTRANCE(PIRATES_FORTRESS_INTERIOR, 5), true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_INSIDE_MAZE_GUARD, true),
        },
        .events = {
            EVENT(RE_ACCESS_ZORA_EGG, HAS_ITEM(ITEM_HOOKSHOT) && HAS_BOTTLE && CAN_BE_ZORA && CAN_USE_ABILITY(SWIM)),
        },
    };
    Regions[RR_PIRATES_FORTRESS_SEWERS_POSTGATE] = RandoRegion{ .name = "Sewers Postgate", .sceneId = SCENE_PIRATE,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_SEWERS_WATERWAY_POT_01, CAN_USE_ABILITY(SWIM)),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_WATERWAY_POT_02, CAN_USE_ABILITY(SWIM)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 3),             ONE_WAY_EXIT, CAN_USE_ABILITY(SWIM))
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_TELESCOPE_ROOM, (HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA) && CAN_USE_ABILITY(SWIM))
        }
    };
    Regions[RR_PIRATES_FORTRESS_SEWERS_PREGATE] = RandoRegion{ .name = "Sewers Pregate", .sceneId = SCENE_PIRATE,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_SEWERS_CHEST_01,                       CAN_BE_ZORA && CAN_USE_ABILITY(SWIM)),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_SEWERS_CHEST_02,                       CAN_BE_ZORA && CAN_USE_ABILITY(SWIM)),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_SEWERS_CHEST_03,                       CAN_BE_ZORA && CAN_USE_ABILITY(SWIM)),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_POT_01,                 true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_POT_02,                 true),
            CHECK(RC_PIRATE_FORTRESS_INTERIOR_SEWERS_PIECE_OF_HEART,                true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_01,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_02,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_03,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_04,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_05,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_06,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_07,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_08,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_09,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_10,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_11,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_12,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_13,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_14,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_15,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_16,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_FREESTANDING_RUPEE_01, true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_FREESTANDING_RUPEE_02, true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_FREESTANDING_RUPEE_03, true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_FREESTANDING_RUPEE_04, true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_FREESTANDING_RUPEE_05, true),

        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 2),    ENTRANCE(PIRATES_FORTRESS_INTERIOR, 9), CAN_BE_ZORA),
            EXIT(ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 3),             ONE_WAY_EXIT, true)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_SEWERS_POSTGATE, HAS_ITEM(ITEM_BOW) || HAS_ITEM(ITEM_HOOKSHOT) || CAN_BE_ZORA)
        }
    };
    Regions[RR_PIRATES_FORTRESS_TELESCOPE_ROOM] = RandoRegion{ .name = "Telescope Room", .sceneId = SCENE_PIRATE,
        .checks = {
            CHECK(RC_PIRATE_FORTRESS_SEWERS_END_BARREL_01,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_END_BARREL_02,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_END_BARREL_03,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_END_BARREL_04,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_END_BARREL_05,             true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_END_FREESTANDING_RUPEE_01, true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_END_FREESTANDING_RUPEE_02, true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_END_FREESTANDING_RUPEE_03, true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_END_POT_01,                 true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_END_POT_02,                 true),
            CHECK(RC_PIRATE_FORTRESS_SEWERS_END_POT_03,                 true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(PIRATES_FORTRESS_EXTERIOR, 6),    ENTRANCE(PIRATES_FORTRESS_INTERIOR, 10), CAN_USE_PROJECTILE)
        },
        .connections = {
            CONNECTION(RR_PIRATES_FORTRESS_SEWERS_POSTGATE, CAN_USE_ABILITY(SWIM))
        }
    };
}, {});
// clang-format on
