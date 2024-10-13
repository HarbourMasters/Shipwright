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
}

void RegisterLightSensitivityBehavior() {
    static uint32_t shouldHookId1 = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnVanillaBehavior>(shouldHookId1);
    shouldHookId1 = 0;

	if (CVarGetInteger(CVAR_SETTING("Lightsensitivity"), 0)) {
        return;
    }

	shouldHookId1 = REGISTER_VB_SHOULD(VB_ALLOW_FLASHING_LIGHTS, {
        *should = false;
        return;
    });
}
