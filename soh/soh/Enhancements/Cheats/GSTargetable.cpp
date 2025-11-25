#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "src/overlays/actors/ovl_En_Sw/z_en_sw.h"

extern PlayState* gPlayState;
}

static constexpr int32_t CVAR_GSTARGETABLE_DEFAULT = 0;
#define CVAR_GSTARGETABLE_NAME CVAR_CHEAT("GSTargetable")
#define CVAR_GSTARGETABLE_VALUE CVarGetInteger(CVAR_GSTARGETABLE_NAME, CVAR_GSTARGETABLE_DEFAULT)

static void OnInitGSTargetable(void* refActor) {
    EnSw* enSw = reinterpret_cast<EnSw*>(refActor);

    if (enSw->actor.naviEnemyId == 0x20) {
        // Enable Targeting this Gold Skulltula
        enSw->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;

        // By default Gold Skulltulas are categorized as NPCs (blue cursor) which feels wrong.
        // Change the category to Misc (green cursor) instead.
        // It might be possible to change the category to Enemy but doing so will likely affect Clear Rooms.
        Actor_ChangeCategory(gPlayState, &gPlayState->actorCtx, &enSw->actor, ACTORCAT_MISC);
    }
}

static void OnEnemyDefeatGSTargetable(void* refActor) {
    EnSw* enSw = reinterpret_cast<EnSw*>(refActor);

    if (enSw->actor.naviEnemyId == 0x20) {
        // Disable Targeting immediately when the Gold Skulltula is defeated (like regular Skullwalltulas)
        enSw->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

static void RegisterGSTargetable() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_SW, CVAR_GSTARGETABLE_VALUE, OnInitGSTargetable);
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_SW, CVAR_GSTARGETABLE_VALUE, OnEnemyDefeatGSTargetable);
}

static RegisterShipInitFunc initFunc(RegisterGSTargetable, { CVAR_GSTARGETABLE_NAME });
