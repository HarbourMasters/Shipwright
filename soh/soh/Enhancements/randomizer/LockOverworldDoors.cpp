#include <libultraship/libultraship.h>
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern PlayState* gPlayState;
#include "macros.h"
#include "src/overlays/actors/ovl_En_Door/z_en_door.h"
}

#define RAND_GET_OPTION(option) Rando::Context::GetInstance()->GetOption(option).Get()

using SceneDoorParamsPair = std::pair<int, int>;
std::map<SceneDoorParamsPair, RandomizerInf> lookupTable = {
    // clang-format off
    {{ SCENE_MARKET_ENTRANCE_DAY, 447 },         RAND_INF_GUARD_HOUSE_UNLOCKED },
    {{ SCENE_MARKET_ENTRANCE_NIGHT, 447 },       RAND_INF_GUARD_HOUSE_UNLOCKED },
    {{ SCENE_MARKET_ENTRANCE_RUINS, 447 },       RAND_INF_GUARD_HOUSE_UNLOCKED },
    {{ SCENE_MARKET_DAY, 4543 },                 RAND_INF_MARKET_BAZAAR_UNLOCKED },
    {{ SCENE_MARKET_NIGHT, 4753 },               RAND_INF_MARKET_BAZAAR_UNLOCKED },
    {{ SCENE_MARKET_DAY, 1471 },                 RAND_INF_MARKET_POTION_SHOP_UNLOCKED },
    {{ SCENE_MARKET_NIGHT, 1678 },               RAND_INF_MARKET_POTION_SHOP_UNLOCKED },
    {{ SCENE_MARKET_DAY, 3519 },                 RAND_INF_MASK_SHOP_UNLOCKED },
    {{ SCENE_MARKET_NIGHT, 3728 },               RAND_INF_MASK_SHOP_UNLOCKED },
    {{ SCENE_MARKET_DAY, 2495 },                 RAND_INF_MARKET_SHOOTING_GALLERY_UNLOCKED },
    {{ SCENE_MARKET_NIGHT, 2703 },               RAND_INF_MARKET_SHOOTING_GALLERY_UNLOCKED },
    {{ SCENE_MARKET_DAY, 5567 },                 RAND_INF_BOMBCHU_BOWLING_UNLOCKED },
    {{ SCENE_MARKET_NIGHT, 5567 },               RAND_INF_BOMBCHU_BOWLING_UNLOCKED },
    {{ SCENE_MARKET_DAY, 653 },                  RAND_INF_TREASURE_CHEST_GAME_BUILDING_UNLOCKED },
    {{ SCENE_MARKET_NIGHT, 447 },                RAND_INF_TREASURE_CHEST_GAME_BUILDING_UNLOCKED },
    {{ SCENE_BACK_ALLEY_DAY, 2689 },             RAND_INF_BOMBCHU_SHOP_UNLOCKED },
    {{ SCENE_BACK_ALLEY_NIGHT, 2495 },           RAND_INF_BOMBCHU_SHOP_UNLOCKED },
    {{ SCENE_BACK_ALLEY_DAY, 447 },              RAND_INF_RICHARDS_HOUSE_UNLOCKED },
    {{ SCENE_BACK_ALLEY_NIGHT, 447 },            RAND_INF_RICHARDS_HOUSE_UNLOCKED },
    {{ SCENE_BACK_ALLEY_DAY, 1665 },             RAND_INF_ALLEY_HOUSE_UNLOCKED },
    {{ SCENE_BACK_ALLEY_NIGHT, 1471 },           RAND_INF_ALLEY_HOUSE_UNLOCKED },
    {{ SCENE_KAKARIKO_VILLAGE, 6801 },           RAND_INF_KAK_BAZAAR_UNLOCKED }, // Adult Night
    {{ SCENE_KAKARIKO_VILLAGE, 6591 },           RAND_INF_KAK_BAZAAR_UNLOCKED }, // Adult Day
    {{ SCENE_KAKARIKO_VILLAGE, 6813 },           RAND_INF_KAK_BAZAAR_UNLOCKED }, // Child Day
    {{ SCENE_KAKARIKO_VILLAGE, 6814 },           RAND_INF_KAK_BAZAAR_UNLOCKED }, // Child Night
    {{ SCENE_KAKARIKO_VILLAGE, 8871 },           RAND_INF_KAK_POTION_SHOP_UNLOCKED }, // Child Day/Night Rear
    {{ SCENE_KAKARIKO_VILLAGE, 8846 },           RAND_INF_KAK_POTION_SHOP_UNLOCKED }, // Adult Night Rear
    {{ SCENE_KAKARIKO_VILLAGE, 8639 },           RAND_INF_KAK_POTION_SHOP_UNLOCKED }, // Adult Day Rear
    {{ SCENE_KAKARIKO_VILLAGE, 7822 },           RAND_INF_KAK_POTION_SHOP_UNLOCKED }, // Adult Night
    {{ SCENE_KAKARIKO_VILLAGE, 7615 },           RAND_INF_KAK_POTION_SHOP_UNLOCKED }, // Child Day/Night and Adult Day
    {{ SCENE_KAKARIKO_VILLAGE, 2495 },           RAND_INF_BOSS_HOUSE_UNLOCKED },
    {{ SCENE_KAKARIKO_VILLAGE, 3750 },           RAND_INF_GRANNYS_POTION_SHOP_UNLOCKED }, // Child
    {{ SCENE_KAKARIKO_VILLAGE, 3519 },           RAND_INF_GRANNYS_POTION_SHOP_UNLOCKED }, // Adult
    {{ SCENE_KAKARIKO_VILLAGE, 5567 },           RAND_INF_SKULLTULA_HOUSE_UNLOCKED },
    {{ SCENE_KAKARIKO_VILLAGE, 1471 },           RAND_INF_IMPAS_HOUSE_UNLOCKED },
    {{ SCENE_KAKARIKO_VILLAGE, 447 },            RAND_INF_WINDMILL_UNLOCKED },
    {{ SCENE_KAKARIKO_VILLAGE, 4543 },           RAND_INF_KAK_SHOOTING_GALLERY_UNLOCKED }, // Day
    {{ SCENE_KAKARIKO_VILLAGE, 4751 },           RAND_INF_KAK_SHOOTING_GALLERY_UNLOCKED }, // Night
    {{ SCENE_GRAVEYARD, 645 },                   RAND_INF_DAMPES_HUT_UNLOCKED }, // Child Day
    {{ SCENE_GRAVEYARD, 447 },                   RAND_INF_DAMPES_HUT_UNLOCKED }, // Child Evening & Adult
    {{ SCENE_GRAVEYARD, 774 },                   RAND_INF_DAMPES_HUT_UNLOCKED }, // Child Night (After Dampes Tour)
    {{ SCENE_LON_LON_RANCH, 2495 },              RAND_INF_TALONS_HOUSE_UNLOCKED },
    {{ SCENE_LON_LON_RANCH, 2473 },              RAND_INF_TALONS_HOUSE_UNLOCKED },
    {{ SCENE_LON_LON_RANCH, 2729 },              RAND_INF_TALONS_HOUSE_UNLOCKED },
    {{ SCENE_LON_LON_RANCH, 1471 },              RAND_INF_STABLES_UNLOCKED },
    {{ SCENE_LON_LON_RANCH, 447 },               RAND_INF_BACK_TOWER_UNLOCKED },
    {{ SCENE_LAKE_HYLIA, 447 },                  RAND_INF_HYLIA_LAB_UNLOCKED },
    {{ SCENE_LAKE_HYLIA, 1471 },                 RAND_INF_FISHING_HOLE_UNLOCKED },

    // clang-format on
};

static void OnDoorInit(void* actorRef) {
    EnDoor* enDoor = static_cast<EnDoor*>(actorRef);
    enDoor->randomizerInf = RAND_INF_MAX;

    auto it = lookupTable.find({ gPlayState->sceneNum, enDoor->actor.params });
    if (it != lookupTable.end()) {
        enDoor->randomizerInf = it->second;
        if (!Flags_GetRandomizerInf(enDoor->randomizerInf)) {
            // We don't want to override checkable doors, we still want those to not be openable even if they have a key
            if (((enDoor->actor.params >> 7) & 7) != DOOR_CHECKABLE) {
                enDoor->actor.params = (enDoor->actor.params & ~0x380) | (DOOR_LOCKED << 7);
                enDoor->actionFunc = EnDoor_SetupType;
            } else {
                enDoor->lockTimer = 10;
            }
        }
    }
}

void RegisterLockOverworldDoors() {
    bool shouldRegister = IS_RANDO && RAND_GET_OPTION(RSK_LOCK_OVERWORLD_DOORS);

    COND_ID_HOOK(OnActorInit, ACTOR_EN_DOOR, shouldRegister, OnDoorInit);

    COND_VB_SHOULD(VB_CONSUME_SMALL_KEY, shouldRegister, {
        EnDoor* enDoor = va_arg(args, EnDoor*);

        if (enDoor->randomizerInf >= RAND_INF_GUARD_HOUSE_UNLOCKED &&
            enDoor->randomizerInf <= RAND_INF_FISHING_HOLE_KEY_OBTAINED) {
            Flags_SetRandomizerInf(enDoor->randomizerInf);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_NOT_HAVE_SMALL_KEY, shouldRegister, {
        EnDoor* enDoor = va_arg(args, EnDoor*);

        if (enDoor->randomizerInf >= RAND_INF_GUARD_HOUSE_UNLOCKED &&
            enDoor->randomizerInf <= RAND_INF_FISHING_HOLE_KEY_OBTAINED) {
            *should = !Flags_GetRandomizerInf((RandomizerInf)(enDoor->randomizerInf + 1));
        }
    });

    COND_VB_SHOULD(VB_DOOR_BE_LOCKED, shouldRegister, {
        EnDoor* enDoor = va_arg(args, EnDoor*);

        if (enDoor->randomizerInf >= RAND_INF_GUARD_HOUSE_UNLOCKED &&
            enDoor->randomizerInf <= RAND_INF_FISHING_HOLE_KEY_OBTAINED) {
            *should = !Flags_GetRandomizerInf(enDoor->randomizerInf);
        }
    });

    // The door actor uses the same param to indicate if a door should be locked or be a scene transition, so it cannot
    // be both. Here we're overriding the check for scene transition to also check if the door is being unlocked and
    // should be a scene transition.
    COND_VB_SHOULD(VB_DOOR_PLAY_SCENE_TRANSITION, shouldRegister, {
        EnDoor* enDoor = va_arg(args, EnDoor*);

        if (!*should && (enDoor->actor.id == ACTOR_EN_DOOR && ((enDoor->actor.params >> 7) & 7) == 1 &&
                         enDoor->randomizerInf != RAND_INF_MAX)) {
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterLockOverworldDoors, { "IS_RANDO" });
