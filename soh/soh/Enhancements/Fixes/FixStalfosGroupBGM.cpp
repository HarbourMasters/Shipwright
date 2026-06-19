#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Test/z_en_test.h"
#include "functions.h"
extern PlayState* gPlayState;
}

static constexpr int32_t CVAR_STALFOS_GROUP_BGM_DEFAULT = 0;
#define CVAR_STALFOS_GROUP_BGM_NAME CVAR_ENHANCEMENT("FixStalfosGroupBGM")
#define CVAR_STALFOS_GROUP_BGM_VALUE CVarGetInteger(CVAR_STALFOS_GROUP_BGM_NAME, CVAR_STALFOS_GROUP_BGM_DEFAULT)

// Vanilla EnTest_Destroy restores the field/combat BGM only when no other
// En_Test (Stalfos) remains nearby -- but it checks with Actor_FindNearby,
// which also matches actors that were already killed this frame (update == NULL)
// and are still in the actor list. When the last Stalfos of a group dies in the
// same frame as its siblings, those siblings are still listed, so vanilla
// believes an enemy remains and the combat BGM never restores (most visible in
// the Master Quest Water Temple Stalfos group). This hook re-runs the restore
// check counting only LIVING En_Test actors, so the BGM restores correctly.
static bool EnTest_HasLivingNearby(Actor* refActor) {
    Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_ENEMY].head;
    while (actor != NULL) {
        if (actor != refActor && actor->id == ACTOR_EN_TEST && actor->update != NULL &&
            Actor_WorldDistXYZToActor(refActor, actor) <= 8000.0f) {
            return true;
        }
        actor = actor->next;
    }
    return false;
}

static void OnEnTestDestroy(void* refActor) {
    Actor* actor = reinterpret_cast<Actor*>(refActor);
    if (actor->params != STALFOS_TYPE_2 && !EnTest_HasLivingNearby(actor)) {
        func_800F5B58();
    }
}

static void RegisterFixStalfosGroupBGM() {
    COND_ID_HOOK(OnActorDestroy, ACTOR_EN_TEST, CVAR_STALFOS_GROUP_BGM_VALUE, OnEnTestDestroy);
}

static RegisterShipInitFunc initFunc(RegisterFixStalfosGroupBGM, { CVAR_STALFOS_GROUP_BGM_NAME });
