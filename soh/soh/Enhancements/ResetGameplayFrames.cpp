#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "variables.h"
}

void ResetGameplayFramesOnSoftReset() {
    PlayState* play = (PlayState*)gGameState;

    if (gGameState->init == TitleSetup_Init) {
        play->gameplayFrames = 0;
    }
}

void ResetGameplayFramesOnTitleScreenExit() {
    PlayState* play = (PlayState*)gGameState;

    if (gSaveContext.fileNum == 0xFF) {
        play->gameplayFrames = 0;
    }
}

void RegisterResetGameplayFrames() {
    COND_HOOK(OnPlayDestroy, true, ResetGameplayFramesOnSoftReset);
    COND_HOOK(OnPlayDestroy, true, ResetGameplayFramesOnTitleScreenExit);
}

static RegisterShipInitFunc initFunc(RegisterResetGameplayFrames);
