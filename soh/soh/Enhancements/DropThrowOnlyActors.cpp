#include <soh/Enhancements/game-interactor/GameInteractor.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

void RegisterAllowThrowOnlyDrop() {
    COND_VB_SHOULD(VB_ON_ACTOR_THROW_ONLY_CHECK, true, {
        Actor* actor = va_arg(args, Actor*);

        switch (actor->id) {
            case ACTOR_EN_ISHI:
            case ACTOR_EN_KUSA:
            case ACTOR_EN_NIW:
            case ACTOR_OBJ_TSUBO:
                break;
            default:
                return;
        }

        // Set correct flag for held actor + condition for the cvar
        if (CVarGetInteger(CVAR_ENHANCEMENT("DropThrowOnlyObjects"), false)) {
            actor->flags &= ~ACTOR_FLAG_THROW_ONLY;
            *should = true;
        } else {
            actor->flags |= ACTOR_FLAG_THROW_ONLY;
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterAllowThrowOnlyDrop, { CVAR_ENHANCEMENT("DropThrowOnlyObjects") });
