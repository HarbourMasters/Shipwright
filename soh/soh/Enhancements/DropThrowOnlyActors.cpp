#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern PlayState* gPlayState;
}

static void AllowThrowOnlyDrop(Actor* actor) {
    switch (actor->id) {
        case ACTOR_EN_ISHI:
        case ACTOR_EN_KUSA:
        case ACTOR_EN_NIW:
        case ACTOR_OBJ_TSUBO:
            break;
        default:
            return;
    }

    if (CVarGetInteger(CVAR_ENHANCEMENT("DropThrowOnlyObjects"), 0)) {
        actor->flags &= ~ACTOR_FLAG_THROW_ONLY;
    } else {
        actor->flags |= ACTOR_FLAG_THROW_ONLY;
    }
}

static void OnThrowOnlyActorInit(void* actorPtr) {
    if (gPlayState == nullptr) {
        return;
    }

    AllowThrowOnlyDrop((Actor*)actorPtr);
}

static int sDropThrowOnlyLastValue = -1;

static void DropThrowOnlyCVarWatcher(void*) {
    if (gPlayState == nullptr) {
        return;
    }

    int currentValue = CVarGetInteger(CVAR_ENHANCEMENT("DropThrowOnlyObjects"), 0);
    if (currentValue == sDropThrowOnlyLastValue) {
        return;
    }
    sDropThrowOnlyLastValue = currentValue;

    for (int category = 0; category < ACTORCAT_MAX; category++) {
        Actor* actor = gPlayState->actorCtx.actorLists[category].head;
        while (actor != nullptr) {
            AllowThrowOnlyDrop(actor);
            actor = actor->next;
        }
    }
}

void RegisterAllowThrowOnlyDrop() {
    COND_HOOK(OnActorInit, true, OnThrowOnlyActorInit);
    COND_HOOK(OnActorUpdate, CVarGetInteger(CVAR_ENHANCEMENT("DropThrowOnlyObjects"), 0) != sDropThrowOnlyLastValue,
              DropThrowOnlyCVarWatcher);
}

static RegisterShipInitFunc initFunc(RegisterAllowThrowOnlyDrop, { CVAR_ENHANCEMENT("DropThrowOnlyObjects") });
