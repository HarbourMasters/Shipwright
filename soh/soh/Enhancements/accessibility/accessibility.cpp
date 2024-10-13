#include "accessibility.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/OTRGlobals.h"

extern "C" {
#include <z64.h>
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
#include "src/overlays/actors/ovl_Boss_Goma/z_boss_goma.h"
}

void RegisterLightSensitivityBehavior() {
    static uint32_t shouldHookId1 = 0;
    static uint32_t ActorUpdateHookId = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnVanillaBehavior>(shouldHookId1);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(ActorUpdateHookId);
    shouldHookId1 = 0;
    ActorUpdateHookId = 0;

	if (!CVarGetInteger(CVAR_SETTING("Lightsensitivity"), 0)) {
        return;
    }

	shouldHookId1 = REGISTER_VB_SHOULD(VB_ALLOW_FLASHING_LIGHTS, {
        *should = false;
        return;
    });

    ActorUpdateHookId = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(ACTOR_BOSS_GOMA, [](void* refActor) {
        BossGoma* actor = (BossGoma*)refActor;
        //if (actor->visualState >= VISUALSTATE_STUNNED) {
            actor->visualState = VISUALSTATE_RED;
        //};
    });
}
