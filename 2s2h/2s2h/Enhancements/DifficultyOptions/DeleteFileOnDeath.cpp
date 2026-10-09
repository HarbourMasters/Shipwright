#include <libultraship/bridge/consolevariablebridge.h>
#include <ship/window/gui/ConsoleWindow.h>
#include <ship/Context.h>
#include <ship/window/Window.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.DifficultyOptions.DeleteFileOnDeath"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void SaveManager_DeleteSaveFile(const std::filesystem::path& fileName);
std::string SaveManager_GetFileName(int fileNum, bool isBackup);

void RegisterDeleteFileOnDeath() {
    COND_HOOK(OnGameStateUpdate, CVAR, []() {
        if (!gPlayState) {
            return;
        }

        static bool fileDeleted = false;

        if (gPlayState->gameOverCtx.state == GAMEOVER_DEATH_WAIT_GROUND && !fileDeleted) {
            fileDeleted = true;
            if (gSaveContext.fileNum >= 0 && gSaveContext.fileNum <= 2) {
                std::string fileName = SaveManager_GetFileName(gSaveContext.fileNum + 1, false);
                std::string backupFileName = SaveManager_GetFileName(gSaveContext.fileNum + 1, true);
                SaveManager_DeleteSaveFile(fileName);
                SaveManager_DeleteSaveFile(backupFileName);
            }
        }

        static int resetTimer = 0;

        if (gPlayState->gameOverCtx.state == 4) { // After GAMEOVER_DEATH_FADE_OUT, no enum
            resetTimer++;

            if (resetTimer >= 20) { // Completely black
                std::reinterpret_pointer_cast<Ship::ConsoleWindow>(
                    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
                    ->Dispatch("reset");
            }
        }

        if (gPlayState->gameOverCtx.state == GAMEOVER_INACTIVE) {
            fileDeleted = false;
            resetTimer = 0;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterDeleteFileOnDeath, { CVAR_NAME });
