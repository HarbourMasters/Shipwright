#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/OTRGlobals.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Door/z_en_door.h"
extern PlayState* gPlayState;
}

static constexpr int32_t CVAR_OPEN_ALL_HOURS_DEFAULT = 0;
#define CVAR_OPEN_ALL_HOURS_NAME CVAR_ENHANCEMENT("OpenAllHours")
#define CVAR_OPEN_ALL_HOURS_VALUE CVarGetInteger(CVAR_OPEN_ALL_HOURS_NAME, CVAR_OPEN_ALL_HOURS_DEFAULT)

static constexpr int32_t DOOR_DAY_CHEST_GAME = 653;
static constexpr int32_t DOOR_DAY_BOMBCHU_SHOP = 2689;

static constexpr int32_t DOOR_NIGHT_POTION_SHOP = 1678;
static constexpr int32_t DOOR_NIGHT_SLINGSHOT_GAME = 2703;
static constexpr int32_t DOOR_NIGHT_MASK_SHOP = 3728;
static constexpr int32_t DOOR_NIGHT_MARKET_BAZAAR = 4753;
static constexpr int32_t DOOR_NIGHT_KAK_ARCHERY_GAME = 4751;
static constexpr int32_t DOOR_NIGHT_KAK_BAZAAR = 6801;
static constexpr int32_t DOOR_NIGHT_KAK_POTION_SHOP = 7822;
static constexpr int32_t DOOR_NIGHT_KAK_POTION_SHOP_BACK = 8846;

static void OpenAllHours(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);
    if (actor->id != ACTOR_EN_DOOR) {
        return;
    }

    switch (actor->params) {
        case DOOR_DAY_CHEST_GAME:
        case DOOR_DAY_BOMBCHU_SHOP:
        case DOOR_NIGHT_POTION_SHOP:
        case DOOR_NIGHT_SLINGSHOT_GAME:
        case DOOR_NIGHT_MASK_SHOP:
        case DOOR_NIGHT_MARKET_BAZAAR:
        case DOOR_NIGHT_KAK_ARCHERY_GAME:
        case DOOR_NIGHT_KAK_BAZAAR:
        case DOOR_NIGHT_KAK_POTION_SHOP:
        case DOOR_NIGHT_KAK_POTION_SHOP_BACK: {
            actor->params = (actor->params & 0xFC00) | (DOOR_SCENEEXIT << 7) | 0x3F;
            EnDoor* enDoor = static_cast<EnDoor*>(refActor);
            EnDoor_SetupType(enDoor, gPlayState);
            break;
        }
        default:
            break;
    }
}

static void RegisterOpenAllHours() {
    bool overworldDoorsOpen =
        !IS_RANDO || !OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_LOCK_OVERWORLD_DOORS);

    COND_HOOK(OnActorInit, CVAR_OPEN_ALL_HOURS_VALUE && overworldDoorsOpen, OpenAllHours);
}

static RegisterShipInitFunc initFunc(RegisterOpenAllHours, { CVAR_OPEN_ALL_HOURS_NAME, "IS_RANDO" });
