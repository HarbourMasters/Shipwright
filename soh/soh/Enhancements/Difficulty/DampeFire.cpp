#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/Enhancements/enhancementTypes.h"

extern "C" {
extern PlayState* gPlayState;
#include "src/overlays/actors/ovl_En_Po_Relay/z_en_po_relay.h"
}

static constexpr DampeDropRate CVAR_DAMPE_DROP_RATE_DEFAULT = DAMPE_NORMAL;
#define CVAR_DAMPE_DROP_RATE_NAME CVAR_ENHANCEMENT("DampeDropRate")
#define CVAR_DAMPE_DROP_RATE_VALUE CVarGetInteger(CVAR_DAMPE_DROP_RATE_NAME, CVAR_DAMPE_DROP_RATE_DEFAULT)
#define CVAR_DAMPE_DROP_RATE_SET (CVAR_DAMPE_DROP_RATE_VALUE != CVAR_DAMPE_DROP_RATE_DEFAULT)

static void RegisterDampeFire() {
    COND_VB_SHOULD(VB_DAMPE_DROP_FLAME, CVAR_DAMPE_DROP_RATE_SET, {
        double chance;
        int cooldown = 9;
        switch (CVAR_DAMPE_DROP_RATE_VALUE) {
            case DAMPE_NONE:
                *should = false;
                return;
            default:
            case DAMPE_NORMAL:
                return;
            case DAMPE_JALAPENO:
                chance = 0.03;
                break;
            case DAMPE_CHIPOTLE:
                chance = 0.1;
                break;
            case DAMPE_SCOTCH_BONNET:
                chance = 0.2;
                break;
            case DAMPE_GHOST_PEPPER:
                chance = 0.5;
                cooldown = 4;
                break;
            case DAMPE_INFERNO:
                *should = true;
                return;
        }

        EnPoRelay* actor = va_arg(args, EnPoRelay*);
        if (actor->actionTimer > cooldown) {
            actor->actionTimer = cooldown;
        }
        *should = actor->actionTimer == 0 && Rand_ZeroOne() < chance;
    });
}

static RegisterShipInitFunc initFunc(RegisterDampeFire, { CVAR_DAMPE_DROP_RATE_NAME });
