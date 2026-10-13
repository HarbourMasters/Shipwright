#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
Actor* Actor_SpawnEntry(ActorContext* actorCtx, ActorEntry* actorEntry, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Cycle.TingleAlwaysInClockTown"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static ActorEntry originalTingle;

static ActorEntry* GetTingleActorEntry() {
    ActorEntry* entry = gPlayState->setupActorList;
    for (size_t i = 0; i < gPlayState->numSetupActors; i++, entry++) {
        if (entry->id == ACTOR_EN_BAL) {
            return entry;
        }
    }

    return NULL;
}

// This hook modifies Tingle's actor entry so he'll always appear in Clock Town, regardless of the time.
// This modification is permanent, which is why we also need the other hook to revert it.
static void SetTingleAlwaysInClockTown(s16 sceneId, s8 roomNum) {
    if (sceneId != SCENE_BACKTOWN) {
        return;
    }

    ActorEntry* tingleEntry = GetTingleActorEntry();
    if (tingleEntry == NULL) {
        return;
    }

    // Copy original entry so we can revert it later
    originalTingle = *tingleEntry;

    // Set half-day flags all to on so he'll spawn regardless of the time of day
    tingleEntry->rot.x |= 0x7;
    tingleEntry->rot.z |= 0x7F;
}

static void ResetTingleActorEntry(Actor* actor) {
    if (gPlayState->sceneId != SCENE_BACKTOWN) {
        return;
    }

    if (originalTingle.id != ACTOR_EN_BAL) {
        // ActorEntry data hasn't been copied, so there's nothing to revert
        return;
    }

    ActorEntry* tingleEntry = GetTingleActorEntry();
    if (tingleEntry == NULL) {
        return;
    }

    *tingleEntry = originalTingle;
}

static void SpawnTingle() {
    Actor* tingle = SubS_FindActor(gPlayState, NULL, ACTORCAT_NPC, ACTOR_EN_BAL);
    if (tingle != NULL) {
        // Tingle already spawned, do not spawn a second one
        return;
    }

    ActorEntry* tingleEntry = GetTingleActorEntry();
    if (tingleEntry == NULL) {
        return;
    }

    Actor_SpawnEntry(&gPlayState->actorCtx, tingleEntry, gPlayState);
}

static void DestroyTingle() {
    Actor* tingle = SubS_FindActor(gPlayState, NULL, ACTORCAT_NPC, ACTOR_EN_BAL);
    if (tingle == NULL) {
        // Tingle not present, nothing to destroy
        return;
    }

    Actor_Kill(tingle);
}

static void RegisterTingleAlwaysInClockTown() {
    if (gPlayState != NULL && gPlayState->sceneId == SCENE_BACKTOWN &&
        (HALFDAYBIT_NIGHTS & gPlayState->actorCtx.halfDaysBit)) {
        if (CVAR) {
            SpawnTingle();
        } else {
            DestroyTingle();
        }
    }

    COND_HOOK(AfterRoomSceneCommands, CVAR, SetTingleAlwaysInClockTown);
    COND_ID_HOOK(OnActorInit, ACTOR_EN_BAL, CVAR, ResetTingleActorEntry);
}

static RegisterShipInitFunc initFunc(RegisterTingleAlwaysInClockTown, { CVAR_NAME });
