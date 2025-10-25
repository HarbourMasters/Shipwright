#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

void OnAllowThrowOnlyDrop(void* actor) {
    Actor* thisx = (Actor*)actor;
    if (CVarGetInteger(CVAR_ENHANCEMENT("DropThrowOnlyObjects"), 0)) {
        thisx->flags &= ~ACTOR_FLAG_THROW_ONLY;
    } else {
        thisx->flags |= ACTOR_FLAG_THROW_ONLY;
    }
}

void RegisterAllowThrowOnlyDrop() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>(OnAllowThrowOnlyDrop);
}

static RegisterShipInitFunc initFunc(RegisterAllowThrowOnlyDrop, { CVAR_ENHANCEMENT("DropThrowOnlyObjects") });
