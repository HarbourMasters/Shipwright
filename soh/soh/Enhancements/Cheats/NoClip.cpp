#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

static void RegisterNoClip() {
    COND_VB_SHOULD(VB_PERFORM_WALL_COLLISION_CHECK, CVarGetInteger(CVAR_CHEAT("NoClip"), 0), {
        Actor* actor = va_arg(args, Actor*);
        if (actor != NULL && actor->id == ACTOR_PLAYER) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterNoClip, { CVAR_CHEAT("NoClip") });
