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
    AllowThrowOnlyDrop((Actor*)actorPtr);
}

static void DropThrowOnlyCVarWatcher(void*) {
    static int lastValue = -1;

    if (gPlayState == nullptr) {
        return;
    }

    int currentValue = CVarGetInteger(CVAR_ENHANCEMENT("DropThrowOnlyObjects"), 0);
    if (currentValue == lastValue) {
        return;
    }
    lastValue = currentValue;

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
    COND_HOOK(OnActorUpdate, true, DropThrowOnlyCVarWatcher);
}

static RegisterShipInitFunc initFunc(RegisterAllowThrowOnlyDrop, { CVAR_ENHANCEMENT("DropThrowOnlyObjects") });
