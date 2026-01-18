#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern PlayState* gPlayState;
}

static int sDropThrowOnlyLastValue = -1;

static void AllowThrowOnlyDrop(Actor* actor, int cvarValue) {
    if (actor == nullptr) {
        return;
    }

    switch (actor->id) {
        case ACTOR_EN_ISHI:
        case ACTOR_EN_KUSA:
        case ACTOR_EN_NIW:
        case ACTOR_OBJ_TSUBO:
            break;
        default:
            return;
    }

    if (cvarValue) {
        actor->flags &= ~ACTOR_FLAG_THROW_ONLY;
    } else {
        actor->flags |= ACTOR_FLAG_THROW_ONLY;
    }
}

static void OnThrowOnlyActorInit(void* actorPtr) {
    if (gPlayState == nullptr || actorPtr == nullptr) {
        return;
    }

    // Use current cvar state for newly spawned actors too
    const int currentValue = CVarGetInteger(CVAR_ENHANCEMENT("DropThrowOnlyObjects"), 0);
    AllowThrowOnlyDrop((Actor*)actorPtr, currentValue);
}

static void DropThrowOnlyCVarWatcher() {
    if (gPlayState == nullptr) {
        return;
    }

    const int currentValue = CVarGetInteger(CVAR_ENHANCEMENT("DropThrowOnlyObjects"), 0);

    sDropThrowOnlyLastValue = currentValue;

    for (int category = 0; category < ACTORCAT_MAX; category++) {
        for (Actor* actor = gPlayState->actorCtx.actorLists[category].head; actor != nullptr; actor = actor->next) {
            AllowThrowOnlyDrop(actor, currentValue);
        }
    }
}

void RegisterAllowThrowOnlyDrop() {
    // Always enabled so newly spawned actors match the current cvar state.
    COND_HOOK(OnActorInit, true, OnThrowOnlyActorInit);

    COND_HOOK(OnGameFrameUpdate, CVarGetInteger(CVAR_ENHANCEMENT("DropThrowOnlyObjects"), 0) != sDropThrowOnlyLastValue,
              DropThrowOnlyCVarWatcher);
}

static RegisterShipInitFunc initFunc(RegisterAllowThrowOnlyDrop, { CVAR_ENHANCEMENT("DropThrowOnlyObjects") });
