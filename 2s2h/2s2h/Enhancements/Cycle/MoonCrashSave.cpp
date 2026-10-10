#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Enhancements/Saving/SavingEnhancements.h"
#include "2s2h/Rando/Rando.h"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Cycle.SaveOnMoonCrash"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterMoonCrashSave() {
    COND_HOOK(BeforeMoonCrash, CVAR || IS_RANDO, []() {
        if (CVAR || (IS_RANDO && RANDO_SAVE_OPTIONS[RO_LOGIC] == RO_LOGIC_GLITCHLESS)) {
            SavingEnhancements_AdvancePlaytime();
            Sram_SaveEndOfCycle(gPlayState);
            func_8014546C(&gPlayState->sramCtx);
            if (gSaveContext.fileNum != 0xFF) {
                Sram_SetFlashPagesDefault(&gPlayState->sramCtx,
                                          gFlashSaveStartPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER],
                                          gFlashSpecialSaveNumPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER]);
                Sram_StartWriteToFlashDefault(&gPlayState->sramCtx);
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterMoonCrashSave, { CVAR_NAME, "IS_RANDO" });
