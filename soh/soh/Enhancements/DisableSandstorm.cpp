#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" PlayState* gPlayState;

void DisableSandstormAfterTransition(int16_t sceneNum) {
    if (sceneNum == SCENE_HAUNTED_WASTELAND) {
        gPlayState->envCtx.sandstormState = SANDSTORM_OFF;
    }
}

void RegisterDisableSandstorm() {
    COND_HOOK(OnTransitionEnd, CVarGetInteger(CVAR_CHEAT("DisableSandstorm"), 0), DisableSandstormAfterTransition);
}

static RegisterShipInitFunc initFunc(RegisterDisableSandstorm, { CVAR_CHEAT("DisableSandstorm") });
