#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenGui/CosmeticEditor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "functions.h"
extern PlayState* gPlayState;
}

#define CVAR_NAME CVAR_COSMETIC("Silly.RainEverywhere")
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterRainEverywhere() {
    COND_HOOK(OnPlayDrawWorldStart, CVAR, []() {
        gPlayState->envCtx.precipitation[PRECIP_SOS_MAX] = 60;
        gPlayState->envCtx.precipitation[PRECIP_SNOW_MAX] = 0;
        gPlayState->envCtx.precipitation[PRECIP_SNOW_CUR] = 0;
    });

    COND_HOOK(OnSceneInit, CVAR, [](s8 sceneId, s8 spawnNum) { Environment_PlayStormNatureAmbience(gPlayState); });

    if (gPlayState == NULL) {
        return;
    }

    if (CVAR) {
        Environment_PlayStormNatureAmbience(gPlayState);
    } else {
        gPlayState->envCtx.precipitation[PRECIP_SOS_MAX] = 0;
        Environment_StopStormNatureAmbience(gPlayState);
    }
}

static RegisterShipInitFunc initFunc(RegisterRainEverywhere, { CVAR_NAME });
