#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

#define CVAR_INGO_RACE_ONCE_NAME CVAR_ENHANCEMENT("IngoRaceOnce")
#define CVAR_INGO_RACE_ONCE_VALUE CVarGetInteger(CVAR_INGO_RACE_ONCE_NAME, INGO_RACE_TWICE)

static constexpr u16 INGO_RACE_EVENT_FLAGS = 0x8046;

static void RegisterIngoRaceOnce() {
    COND_VB_SHOULD(VB_RACE_INGO, CVAR_INGO_RACE_ONCE_VALUE == INGO_RACE_NONE, {
        s32 entranceIndex = va_arg(args, s32);
        if (entranceIndex == 2 && (gSaveContext.eventInf[0] & 0x12) == 2) {
            gPlayState->nextEntranceIndex = ENTR_LON_LON_RANCH_7;
            gSaveContext.eventInf[0] = (gSaveContext.eventInf[0] & ~0xF) | INGO_RACE_EVENT_FLAGS;
            gPlayState->transitionType = TRANS_TYPE_FADE_WHITE;
            gPlayState->transitionTrigger = TRANS_TRIGGER_START;
            gSaveContext.timerState = TIMER_STATE_OFF;
            Environment_ForcePlaySequence(NA_BGM_INGO);
            *should = false;
        }
    })

    COND_VB_SHOULD(VB_LINK_WIN_EPONA, CVAR_INGO_RACE_ONCE_VALUE != INGO_RACE_TWICE, { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterIngoRaceOnce, { CVAR_INGO_RACE_ONCE_NAME });
