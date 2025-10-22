#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/mods.h"
#include "soh/ShipInit.hpp"

static constexpr ZFightingFixType CVAR_DIRT_PATH_DEFAULT = ZFIGHT_FIX_DISABLED;
#define CVAR_DIRT_PATH_NAME CVAR_ENHANCEMENT("SceneSpecificDirtPathFix")
#define CVAR_DIRT_PATH_VALUE CVarGetInteger(CVAR_DIRT_PATH_NAME, CVAR_DIRT_PATH_DEFAULT)

void UpdateDirtPathFixState(int32_t sceneNum) {
    switch (sceneNum) {
        case SCENE_HYRULE_FIELD:
        case SCENE_KOKIRI_FOREST:
        case SCENE_HYRULE_CASTLE:
            CVarSetInteger(CVAR_Z_FIGHTING_MODE,
                           CVarGetInteger(CVAR_ENHANCEMENT("SceneSpecificDirtPathFix"), ZFIGHT_FIX_DISABLED));
            return;
        default:
            CVarClear(CVAR_Z_FIGHTING_MODE);
    }
}

static void RegisterDirtPathFix() {
    COND_HOOK(OnTransitionEnd, true, UpdateDirtPathFixState);
}

static RegisterShipInitFunc initFunc(RegisterDirtPathFix, { CVAR_DIRT_PATH_NAME });
