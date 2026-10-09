#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include "2s2h/Rando/Logic/Logic.h"

using namespace Rando::Logic;

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    Regions[RR_BENEATH_THE_WELL_BABA_AND_POTS_ROOM] = RandoRegion{ .name = "Babas and Pots Room", .sceneId = SCENE_REDEAD,
        .checks = {
            CHECK(RC_BENEATH_THE_WELL_MIDDLE_POT_01, true),
            CHECK(RC_BENEATH_THE_WELL_MIDDLE_POT_02, true),
            CHECK(RC_BENEATH_THE_WELL_MIDDLE_POT_03, true),
            CHECK(RC_BENEATH_THE_WELL_MIDDLE_POT_04, true),
            CHECK(RC_BENEATH_THE_WELL_MIDDLE_POT_05, true),
            CHECK(RC_BENEATH_THE_WELL_MIDDLE_POT_06, true),
            CHECK(RC_BENEATH_THE_WELL_MIDDLE_POT_07, true),
            CHECK(RC_BENEATH_THE_WELL_MIDDLE_POT_08, true),
            CHECK(RC_BENEATH_THE_WELL_MIDDLE_POT_09, true),
            CHECK(RC_BENEATH_THE_WELL_MIDDLE_POT_10, true),
            CHECK(RC_ENEMY_DROP_MINI_BABA, CanKillEnemy(ACTOR_EN_KAREBABA)),
            CHECK(RC_ENEMY_DROP_DEKU_BABA, CanKillEnemy(ACTOR_EN_DEKUBABA)),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_WELL_FREEZARD_ROOM, true),
            CONNECTION(RR_BENEATH_THE_WELL_SKULLTULA_ROOM, HAS_BOTTLE && CAN_ACCESS(BUGS) && HAS_ITEM(ITEM_MASK_GIBDO)),
            CONNECTION(RR_BENEATH_THE_WELL_FOUR_SPIKED_BARS, HAS_BOTTLE && CAN_ACCESS(BIG_POE) && HAS_ITEM(ITEM_MASK_GIBDO))
        },
    };
    Regions[RR_BENEATH_THE_WELL_BACK_EXIT] = RandoRegion{ .name = "Back Exit", .sceneId = SCENE_REDEAD,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CASTLE, 0),                 ENTRANCE(BENEATH_THE_WELL, 1), true),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_WELL_MIRROR_SHIELD_ROOM, CAN_USE_MAGIC_ARROW(LIGHT)),
        },
    };
    Regions[RR_BENEATH_THE_WELL_BIG_POE_ROOM] = RandoRegion{ .name = "Big Poe Room", .sceneId = SCENE_REDEAD,
        .checks = {
            CHECK(RC_BENEATH_THE_WELL_BIG_POE_POT_01, true),
            CHECK(RC_BENEATH_THE_WELL_BIG_POE_POT_02, true),
            CHECK(RC_BENEATH_THE_WELL_BIG_POE_POT_03, true),
            CHECK(RC_BENEATH_THE_WELL_BIG_POE_POT_04, true),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_WELL_RIGHT_FIRE_KEESE, true),
        },
        .events = {
            EVENT(RE_ACCESS_BIG_POE, CanKillEnemy(ACTOR_EN_BIGPO)),
        }
    };
    Regions[RR_BENEATH_THE_WELL_COW_ROOM] = RandoRegion{ .name = "Cow Room", .sceneId = SCENE_REDEAD,
        .checks = {
            CHECK(RC_BENEATH_THE_WELL_COW_ROOM_GRASS_01, true),
            CHECK(RC_BENEATH_THE_WELL_COW_ROOM_GRASS_02, true),
            CHECK(RC_BENEATH_THE_WELL_COW_ROOM_GRASS_03, true),
            CHECK(RC_BENEATH_THE_WELL_COW, CAN_PLAY_SONG(EPONA)),
            CHECK(RC_BENEATH_THE_WELL_TREE, true),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_WELL_RIGHT_FIRE_KEESE, true),
        },
        .events = {
            EVENT(RE_ACCESS_MILK_REFILL, CAN_PLAY_SONG(EPONA)),
        }
    };
    Regions[RR_BENEATH_THE_WELL_DEXIHAND_ROOM] = RandoRegion{ .name = "Dexihand Room", .sceneId = SCENE_REDEAD,
        .checks = {
            CHECK(RC_ENEMY_DROP_DEXIHAND, CanKillEnemy(ACTOR_EN_WDHAND)),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_WELL_THREE_SPIKED_BARS, true),
        },
        .events = {
            EVENT(RE_ACCESS_SPRING_WATER, true),
            EVENT(RE_ACCESS_FISH, true),
            EVENT(RE_ACCESS_HOT_SPRING_WATER, HAS_ITEM(ITEM_BOW))
        },
    };
    Regions[RR_BENEATH_THE_WELL_ENTRANCE] = RandoRegion{ .name = "Entrance", .sceneId = SCENE_REDEAD,
        .exits = { //     TO                                         FROM
            EXIT(ENTRANCE(IKANA_CANYON, 5),                 ENTRANCE(BENEATH_THE_WELL, 0), true),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_WELL_THREE_SPIKED_BARS, HAS_BOTTLE && CAN_ACCESS(BLUE_POTION_REFILL) && HAS_ITEM(ITEM_MASK_GIBDO)),
            CONNECTION(RR_BENEATH_THE_WELL_FREEZARD_ROOM, RANDO_EVENTS[RE_ACCESS_BEANS_REFILL] && HAS_ITEM(ITEM_MASK_GIBDO))
        },
    };
    Regions[RR_BENEATH_THE_WELL_FAIRY_FOUNTAIN] = RandoRegion{ .name = "Fairy Fountain", .sceneId = SCENE_REDEAD,
        .connections = {
            CONNECTION(RR_BENEATH_THE_WELL_TWO_SPIKED_BARS, true),
        },
    };
    Regions[RR_BENEATH_THE_WELL_FOUR_SPIKED_BARS] = RandoRegion{ .name = "Four Spikes Room", .sceneId = SCENE_REDEAD,
        .checks = {
            CHECK(RC_BENEATH_THE_WELL_FOUR_SPIKED_BARS_GRASS_01, CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CHECK(RC_BENEATH_THE_WELL_FOUR_SPIKED_BARS_GRASS_02, CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CHECK(RC_BENEATH_THE_WELL_FOUR_SPIKED_BARS_GRASS_03, CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CHECK(RC_BENEATH_THE_WELL_FOUR_SPIKED_BARS_GRASS_04, CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CHECK(RC_BENEATH_THE_WELL_FOUR_SPIKED_BARS_GRASS_05, CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CHECK(RC_ENEMY_DROP_SKULLTULA, CanKillEnemy(ACTOR_EN_ST)),
            CHECK(RC_ENEMY_DROP_WALLMASTER, CanKillEnemy(ACTOR_EN_WALLMAS)),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_WELL_BABA_AND_POTS_ROOM, true),
            CONNECTION(RR_BENEATH_THE_WELL_MIRROR_SHIELD_ROOM, HAS_BOTTLE && CAN_ACCESS(MILK_REFILL) && HAS_ITEM(ITEM_MASK_GIBDO))
        },
        .events = {
            EVENT(RE_ACCESS_BUGS, CAN_LIGHT_TORCH_NEAR_ANOTHER && HAS_BOTTLE),
        }
    };
    Regions[RR_BENEATH_THE_WELL_FREEZARD_ROOM] = RandoRegion{ .name = "Freezard Room", .sceneId = SCENE_REDEAD,
        .checks = {
            CHECK(RC_ENEMY_DROP_KEESE, CanKillEnemy(ACTOR_EN_FIREFLY)),
            CHECK(RC_ENEMY_DROP_WALLMASTER, CanKillEnemy(ACTOR_EN_WALLMAS)),
            CHECK(RC_ENEMY_DROP_FREEZARD, CanKillEnemy(ACTOR_EN_FZ)),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_WELL_ENTRANCE, true),
            CONNECTION(RR_BENEATH_THE_WELL_RIGHT_FIRE_KEESE, HAS_ITEM(ITEM_MASK_GIBDO)),
            CONNECTION(RR_BENEATH_THE_WELL_BABA_AND_POTS_ROOM, HAS_BOTTLE && CAN_ACCESS(FISH) && HAS_ITEM(ITEM_MASK_GIBDO)),
        },
        .events = {
            EVENT(RE_ACCESS_SPRING_WATER, true),
        }
    };
    Regions[RR_BENEATH_THE_WELL_LEFT_FIRE_KEESE] = RandoRegion{ .name = "Left Fire Keese Room", .sceneId = SCENE_REDEAD,
        .checks = {
            CHECK(RC_BENEATH_THE_WELL_KEESE_CHEST, HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_MAGIC),
            CHECK(RC_ENEMY_DROP_KEESE, CanKillEnemy(ACTOR_EN_FIREFLY)),
            CHECK(RC_ENEMY_DROP_WALLMASTER, CanKillEnemy(ACTOR_EN_WALLMAS)),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_WELL_TWO_SPIKED_BARS, true),
        },
    };
    Regions[RR_BENEATH_THE_WELL_MIRROR_SHIELD_ROOM] = RandoRegion{ .name = "Mirror Shield Room", .sceneId = SCENE_REDEAD,
        .checks = {
            // In cases where the player comes from the exit. The flame is accessed two rooms back(RR_BENEATH_THE_WELL_BABA_AND_POTS_ROOM) and the door locks on the way out, so we also need to factor the requirements to get back into this room.
            CHECK(RC_BENEATH_THE_WELL_MIRROR_SHIELD, (HAS_BOTTLE && CAN_ACCESS(BIG_POE) && HAS_ITEM(ITEM_MASK_GIBDO) && CAN_ACCESS(MILK_REFILL) && HAS_ITEM(ITEM_DEKU_STICK)) || CAN_USE_MAGIC_ARROW(FIRE))
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_WELL_FOUR_SPIKED_BARS, true),
            CONNECTION(RR_BENEATH_THE_WELL_BACK_EXIT, CAN_USE_MAGIC_ARROW(LIGHT) || (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) >= EQUIP_VALUE_SHIELD_MIRROR))
        }
    };
    Regions[RR_BENEATH_THE_WELL_RIGHT_FIRE_KEESE] = RandoRegion{ .name = "Right Fire Keese Room", .sceneId = SCENE_REDEAD,
        .checks = {
            CHECK(RC_BENEATH_THE_WELL_RIGHT_FIRE_KEESE_GRASS_01, true),
            CHECK(RC_BENEATH_THE_WELL_RIGHT_FIRE_KEESE_GRASS_02, true),
            CHECK(RC_BENEATH_THE_WELL_RIGHT_FIRE_KEESE_GRASS_03, true),
            CHECK(RC_BENEATH_THE_WELL_RIGHT_FIRE_KEESE_GRASS_04, true),
            CHECK(RC_ENEMY_DROP_KEESE, CanKillEnemy(ACTOR_EN_FIREFLY)),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_WELL_FREEZARD_ROOM, true),
            CONNECTION(RR_BENEATH_THE_WELL_BIG_POE_ROOM, HAS_ITEM(ITEM_BOMB) && HAS_ITEM(ITEM_MASK_GIBDO)),
            CONNECTION(RR_BENEATH_THE_WELL_COW_ROOM, HAS_BOTTLE && CAN_ACCESS(HOT_SPRING_WATER) && HAS_ITEM(ITEM_MASK_GIBDO))
        },
        .events = {
            EVENT(RE_ACCESS_BUGS, HAS_ITEM(ITEM_BOW)),
        }
    };
    Regions[RR_BENEATH_THE_WELL_SKULLTULA_ROOM] = RandoRegion{ .name = "Skulltula Room", .sceneId = SCENE_REDEAD,
        .checks = {
            CHECK(RC_BENEATH_THE_WELL_SKULLTULLA_CHEST, CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CHECK(RC_ENEMY_DROP_SKULLTULA, CanKillEnemy(ACTOR_EN_ST)),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_WELL_BABA_AND_POTS_ROOM, true), 
        }
    };
    Regions[RR_BENEATH_THE_WELL_THREE_SPIKED_BARS] = RandoRegion{ .name = "Three Spikes Room", .sceneId = SCENE_REDEAD,
        .connections = {
            CONNECTION(RR_BENEATH_THE_WELL_ENTRANCE, true),
            CONNECTION(RR_BENEATH_THE_WELL_TWO_SPIKED_BARS, HAS_BOTTLE && (CAN_ACCESS(SPRING_WATER) || CAN_ACCESS(HOT_SPRING_WATER)) && HAS_ITEM(ITEM_MASK_GIBDO)),
            CONNECTION(RR_BENEATH_THE_WELL_DEXIHAND_ROOM, HAS_BOTTLE && CAN_ACCESS(FISH) && HAS_ITEM(ITEM_MASK_GIBDO))
        },
        .events = {
            EVENT(RE_ACCESS_SPRING_WATER, true),
            EVENT(RE_ACCESS_FISH, true)
        },
    };
    Regions[RR_BENEATH_THE_WELL_TWO_SPIKED_BARS] = RandoRegion{ .name = "Two Spikes Room", .sceneId = SCENE_REDEAD,
        .checks = {
            CHECK(RC_BENEATH_THE_WELL_TWO_SPIKED_BARS_GRASS_01, CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CHECK(RC_BENEATH_THE_WELL_TWO_SPIKED_BARS_GRASS_02, CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CHECK(RC_BENEATH_THE_WELL_LEFT_SIDE_POT_01, CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CHECK(RC_BENEATH_THE_WELL_LEFT_SIDE_POT_02, CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CHECK(RC_BENEATH_THE_WELL_LEFT_SIDE_POT_03, CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CHECK(RC_BENEATH_THE_WELL_LEFT_SIDE_POT_04, CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CHECK(RC_BENEATH_THE_WELL_LEFT_SIDE_POT_05, CAN_LIGHT_TORCH_NEAR_ANOTHER),
            CHECK(RC_ENEMY_DROP_WALLMASTER, CanKillEnemy(ACTOR_EN_WALLMAS)),
        },
        .connections = {
            CONNECTION(RR_BENEATH_THE_WELL_THREE_SPIKED_BARS, true),
            CONNECTION(RR_BENEATH_THE_WELL_FAIRY_FOUNTAIN, HAS_BOTTLE && CAN_ACCESS(BUGS) && HAS_ITEM(ITEM_MASK_GIBDO)),
            CONNECTION(RR_BENEATH_THE_WELL_LEFT_FIRE_KEESE, HAS_BOTTLE && CAN_ACCESS(BUGS) && HAS_ITEM(ITEM_MASK_GIBDO)),
        },
        .events = {
            EVENT(RE_ACCESS_BUGS, CAN_LIGHT_TORCH_NEAR_ANOTHER),
        },
    };
}, {});
// clang-format on
