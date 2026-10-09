#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_MOON_DEKU_TRIAL] = RandoRegion{ .sceneId = SCENE_LAST_DEKU,
        .checks = {
            CHECK(RC_MOON_TRIAL_DEKU_PIECE_OF_HEART, CAN_BE_DEKU),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(THE_MOON, 0),                     ENTRANCE(MOON_DEKU_TRIAL, 0), true),
            EXIT(ENTRANCE(THE_MOON, 0),                              ONE_WAY_EXIT, CAN_BE_DEKU), // Exit from Mask NPC
        },
    };
    Regions[RR_MOON_GORON_TRIAL] = RandoRegion{ .sceneId = SCENE_LAST_GORON,
        .checks = {
            CHECK(RC_MOON_TRIAL_GORON_EARLY_POT_01, true),
            CHECK(RC_MOON_TRIAL_GORON_EARLY_POT_02, true),
            CHECK(RC_MOON_TRIAL_GORON_EARLY_POT_03, true),
            CHECK(RC_MOON_TRIAL_GORON_EARLY_POT_04, true),
            CHECK(RC_MOON_TRIAL_GORON_POT_01, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_02, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_03, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_04, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_05, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_06, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_07, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_08, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_09, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_10, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_POT_11, CAN_BE_GORON && HAS_MAGIC),
            CHECK(RC_MOON_TRIAL_GORON_PIECE_OF_HEART, CAN_BE_GORON && HAS_MAGIC),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(THE_MOON, 0),                     ENTRANCE(MOON_GORON_TRIAL, 0), true),
            EXIT(ENTRANCE(THE_MOON, 0),                              ONE_WAY_EXIT, CAN_BE_GORON && HAS_MAGIC), // Exit from Mask NPC
        },
    };
    Regions[RR_MOON_LINK_TRIAL_ENTRANCE] = RandoRegion{ .sceneId = SCENE_LAST_LINK,
        .checks = {
            CHECK(RC_MOON_TRIAL_LINK_POT_01, true),
            CHECK(RC_MOON_TRIAL_LINK_POT_02, true),
            CHECK(RC_MOON_TRIAL_LINK_POT_03, true),
            CHECK(RC_MOON_TRIAL_LINK_POT_04, true),
            CHECK(RC_MOON_TRIAL_LINK_POT_05, true),
            CHECK(RC_MOON_TRIAL_LINK_POT_06, true),
            CHECK(RC_MOON_TRIAL_LINK_POT_07, true),
            CHECK(RC_MOON_TRIAL_LINK_POT_08, true),
            CHECK(RC_ENEMY_DROP_DINOLFOS, CanKillEnemy(ACTOR_EN_DINOFOS)),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(THE_MOON, 0),                     ENTRANCE(MOON_LINK_TRIAL, 0), true),
        },
        .connections = {
            CONNECTION(RR_MOON_LINK_TRIAL_GARO_ROOM, CanKillEnemy(ACTOR_EN_DINOFOS)),
        },
    };
    Regions[RR_MOON_LINK_TRIAL_GARO_ROOM] = RandoRegion{ .sceneId = SCENE_LAST_LINK,
        .checks = {
            CHECK(RC_ENEMY_DROP_GARO_MASTER, CanKillEnemy(ACTOR_EN_JSO2)),
            CHECK(RC_MOON_TRIAL_LINK_GARO_MASTER_CHEST, HAS_ITEM(ITEM_HOOKSHOT)),
        },
        .connections = {
            CONNECTION(RR_MOON_LINK_TRIAL_ENTRANCE, CanKillEnemy(ACTOR_EN_DINOFOS)),
            CONNECTION(RR_MOON_LINK_TRIAL_KNUCKLE_ROOM, CanKillEnemy(ACTOR_EN_JSO2) && HAS_ITEM(ITEM_HOOKSHOT)),
        },
    };
    Regions[RR_MOON_LINK_TRIAL_KNUCKLE_ROOM] = RandoRegion{ .sceneId = SCENE_LAST_LINK,
        .checks = {
            CHECK(RC_MOON_TRIAL_LINK_IRON_KNUCKLE_CHEST, CanKillEnemy(ACTOR_EN_IK)),
            CHECK(RC_ENEMY_DROP_IRON_KNUCKLE, CanKillEnemy(ACTOR_EN_IK)),
        },
        .connections = {
            CONNECTION(RR_MOON_LINK_TRIAL_FINAL_ROOM, CanKillEnemy(ACTOR_EN_IK) && HAS_ITEM(ITEM_BOMBCHU) && HAS_ITEM(ITEM_BOW)),
            CONNECTION(RR_MOON_LINK_TRIAL_GARO_ROOM, CanKillEnemy(ACTOR_EN_IK)),
        },
    };
    Regions[RR_MOON_LINK_TRIAL_FINAL_ROOM] = RandoRegion{ .sceneId = SCENE_LAST_LINK,
        .checks = {
            CHECK(RC_MOON_TRIAL_LINK_PIECE_OF_HEART, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(THE_MOON, 0),                              ONE_WAY_EXIT, HAS_ITEM(ITEM_BOMBCHU) && CAN_USE_MAGIC_ARROW(FIRE)), // Exit from Mask NPC
        },
        .connections = {
            CONNECTION(RR_MOON_LINK_TRIAL_GARO_ROOM, true),
        },
    };
    Regions[RR_MOON_MAJORAS_LAIR] = RandoRegion{ .sceneId = SCENE_LAST_BS,
        .checks = {
            // TODO: 1) Add a check for Game Completion?
            //       2) Determine if it's ok for these pots to be shuffled since we cannot return from here.
            CHECK(RC_MOON_MAJORA_POT_01, true),
            CHECK(RC_MOON_MAJORA_POT_02, true),
        },
        .oneWayEntrances = {
            ENTRANCE(MAJORAS_LAIR, 0), // From moon
        },
    };
    Regions[RR_MOON_ZORA_TRIAL] = RandoRegion{ .sceneId = SCENE_LAST_ZORA,
        .checks = {
            CHECK(RC_MOON_TRIAL_ZORA_PIECE_OF_HEART, true),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(THE_MOON, 0),                     ENTRANCE(MOON_ZORA_TRIAL, 0), true),
            EXIT(ENTRANCE(THE_MOON, 0),                              ONE_WAY_EXIT, true), // Exit from Mask NPC
        },
    };
    Regions[RR_MOON] = RandoRegion{ .sceneId = SCENE_SOUGEN,
        .checks = {
            CHECK(RC_MOON_FIERCE_DEITY_MASK, MoonMaskCount() >= 20),
        },
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(MOON_DEKU_TRIAL, 0),              ENTRANCE(THE_MOON, 0), 
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_20_MASKS && MoonMaskCount() >= 2) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_REMAINS && CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA)) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_FORMS && CAN_BE_DEKU) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_OPEN) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_VANILLA && MoonMaskCount() >= 1)
            ),
            EXIT(ENTRANCE(MOON_GORON_TRIAL, 0),             ENTRANCE(THE_MOON, 0), 
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_20_MASKS && MoonMaskCount() >= 6) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_REMAINS && CHECK_QUEST_ITEM(QUEST_REMAINS_GOHT)) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_FORMS && CAN_BE_GORON) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_OPEN) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_VANILLA && MoonMaskCount() >= 2)
            ),
            EXIT(ENTRANCE(MOON_ZORA_TRIAL, 0),              ENTRANCE(THE_MOON, 0),
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_20_MASKS && MoonMaskCount() >= 12) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_REMAINS && CHECK_QUEST_ITEM(QUEST_REMAINS_GYORG)) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_FORMS && CAN_BE_ZORA) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_OPEN) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_VANILLA && MoonMaskCount() >= 3)
            ),
            EXIT(ENTRANCE(MOON_LINK_TRIAL, 0),              ENTRANCE(THE_MOON, 0),
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_20_MASKS && MoonMaskCount() >= 20) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_REMAINS && CHECK_QUEST_ITEM(QUEST_REMAINS_TWINMOLD)) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_FORMS) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_OPEN) ||
                (RANDO_SAVE_OPTIONS[RO_ACCESS_TRIALS] == RO_ACCESS_TRIALS_VANILLA && MoonMaskCount() >= 4)
            ),
            EXIT(ENTRANCE(MAJORAS_LAIR, 0),                          ONE_WAY_EXIT, (RemainsCount() >= RANDO_SAVE_OPTIONS[RO_ACCESS_MAJORA_REMAINS_COUNT]) && (MoonMaskCount() >= RANDO_SAVE_OPTIONS[RO_ACCESS_MAJORA_MASKS_COUNT])
            ),
        },
        .oneWayEntrances = {
            ENTRANCE(THE_MOON, 0), // From rooftop and trials
        },
    };
}, {});
// clang-format on
