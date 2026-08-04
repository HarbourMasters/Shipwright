#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/ShipInit.hpp"

extern "C" PlayState* gPlayState;

static constexpr ZFightingFixType CVAR_DIRT_PATH_DEFAULT = ZFIGHT_FIX_DISABLED;
#define CVAR_DIRT_PATH_NAME CVAR_ENHANCEMENT("SceneSpecificDirtPathFix")
#define CVAR_DIRT_PATH_VALUE CVarGetInteger(CVAR_DIRT_PATH_NAME, CVAR_DIRT_PATH_DEFAULT)

static void DirtPathFix_UpdateZFightingMode(int32_t sceneNum) {
    switch (sceneNum) {
        case SCENE_HYRULE_FIELD:
        case SCENE_KOKIRI_FOREST:
        case SCENE_HYRULE_CASTLE:
            CVarSetInteger(CVAR_Z_FIGHTING_MODE, CVAR_DIRT_PATH_VALUE);
            return;
        default:
            CVarClear(CVAR_Z_FIGHTING_MODE);
    }
}

static void RegisterDirtPathFix() {
    if (gPlayState != NULL) {
        DirtPathFix_UpdateZFightingMode(gPlayState->sceneNum);
    }

    COND_HOOK(OnTransitionEnd, CVAR_DIRT_PATH_VALUE, DirtPathFix_UpdateZFightingMode);
}

static RegisterShipInitFunc initFunc(RegisterDirtPathFix, { CVAR_DIRT_PATH_NAME });
