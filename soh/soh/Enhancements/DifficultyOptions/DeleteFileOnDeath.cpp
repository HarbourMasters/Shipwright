#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/SaveManager.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
#include "macros.h"
}

#define CVAR_DELETE_FILE_ON_DEATH_NAME CVAR_ENHANCEMENT("DeleteFileOnDeath")
#define CVAR_DELETE_FILE_ON_DEATH_DEFAULT 0
#define CVAR_DELETE_FILE_ON_DEATH_VALUE CVarGetInteger(CVAR_DELETE_FILE_ON_DEATH_NAME, CVAR_DELETE_FILE_ON_DEATH_DEFAULT)

void OnGameFrameUpdateDeleteFileOnDeath() {
    if (
        !GameInteractor::IsSaveLoaded() ||
        gPlayState == NULL ||
        gPlayState->gameOverCtx.state != GAMEOVER_DEATH_MENU ||
        gPlayState->pauseCtx.state != 9
    ) {
        return;
    }

    SaveManager::Instance->DeleteZeldaFile(gSaveContext.fileNum);
    std::reinterpret_pointer_cast<Ship::ConsoleWindow>(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch("reset");
}

void RegisterDeleteFileOnDeath() {
    COND_HOOK(OnGameFrameUpdate, CVAR_DELETE_FILE_ON_DEATH_VALUE, OnGameFrameUpdateDeleteFileOnDeath);
}

static RegisterShipInitFunc initFunc(RegisterDeleteFileOnDeath, { CVAR_DELETE_FILE_ON_DEATH_NAME });
