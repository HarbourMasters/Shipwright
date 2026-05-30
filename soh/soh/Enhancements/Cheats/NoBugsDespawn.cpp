#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "src/overlays/actors/ovl_En_Insect/z_en_insect.h"

extern s16 D_80A7DEB8;
}

static void OnActorInitNoBugsDespawn(IEvent* event) {
    OnActorInit* ev = reinterpret_cast<OnActorInit*>(event);

    EnInsect* insect = reinterpret_cast<EnInsect*>(ev->actor);

    if ((insect->actor.params & 2) && insect->soilActor == NULL) {
        insect->insectFlags &= ~4;
        D_80A7DEB8--;
    }
}

static void RegisterNoBugsDespawn() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_INSECT, CVarGetInteger(CVAR_CHEAT("NoBugsDespawn"), 0),
                 OnActorInitNoBugsDespawn);
}

static RegisterShipInitFunc initFunc(RegisterNoBugsDespawn, { CVAR_CHEAT("NoBugsDespawn") });
