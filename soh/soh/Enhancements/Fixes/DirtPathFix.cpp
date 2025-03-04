#include <libultraship/bridge.h>
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" PlayState* gPlayState;

#define CVAR_DIRT_PATH_FIX_NAME CVAR_ENHANCEMENT("SceneSpecificDirtPathFix")
#define CVAR_DIRT_PATH_FIX_DEFAULT ZFIGHT_FIX_DISABLED
#define CVAR_DIRT_PATH_FIX_VALUE CVarGetInteger(CVAR_DIRT_PATH_FIX_NAME, CVAR_DIRT_PATH_FIX_DEFAULT)

void OnTransitionEndDirtPathFix(s32 sceneNum) {
    switch (sceneNum) {
        case SCENE_HYRULE_FIELD:
        case SCENE_KOKIRI_FOREST:
        case SCENE_HYRULE_CASTLE:
            CVarSetInteger(CVAR_Z_FIGHTING_MODE, CVAR_DIRT_PATH_FIX_VALUE);
            return;
        default:
            CVarClear(CVAR_Z_FIGHTING_MODE);
    }
}

void RegisterDirtPathFix() {
    COND_HOOK(OnTransitionEnd, CVAR_DIRT_PATH_FIX_VALUE, OnTransitionEndDirtPathFix);
    if (gPlayState != NULL) {
        OnTransitionEndDirtPathFix(gPlayState->sceneNum);
    }
}

static RegisterShipInitFunc initFunc(RegisterDirtPathFix, { CVAR_DIRT_PATH_FIX_NAME });
