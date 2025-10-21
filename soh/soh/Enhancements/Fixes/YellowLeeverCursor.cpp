#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/OTRGlobals.h"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"

extern PlayState* gPlayState;
}

#define CVAR_YELLOWLEEVERCURSOR_NAME CVAR_ENHANCEMENT("YellowLeeverCursor")
#define CVAR_YELLOWLEEVERCURSOR_DEFAULT 0
#define CVAR_YELLOWLEEVERCURSOR_VALUE CVarGetInteger(CVAR_YELLOWLEEVERCURSOR_NAME, CVAR_YELLOWLEEVERCURSOR_DEFAULT)

void OnActorInitYellowLeeverCursor(void* refActor) {
    Actor* actor = (Actor*)refActor;
    Actor_ChangeCategory(gPlayState, &gPlayState->actorCtx, actor, ACTORCAT_ENEMY);
}

void RegisterYellowLeeverCursor() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_REEBA, CVAR_YELLOWLEEVERCURSOR_VALUE, OnActorInitYellowLeeverCursor);
}

static RegisterShipInitFunc initFunc(RegisterYellowLeeverCursor, { CVAR_YELLOWLEEVERCURSOR_NAME });
