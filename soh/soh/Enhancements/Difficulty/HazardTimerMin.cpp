#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_HAZARD_TIMER_MIN_NAME CVAR_ENHANCEMENT("HazardTimerMin")
#define CVAR_HAZARD_TIMER_MIN_VALUE CVarGetInteger(CVAR_HAZARD_TIMER_MIN_NAME, 0)

static void RegisterHazardTimerMin() {
    COND_HOOK(OnGameFrameUpdate, CVAR_HAZARD_TIMER_MIN_VALUE, []() {
        if (GameInteractor::IsSaveLoaded() && gSaveContext.timerState == TIMER_STATE_ENV_HAZARD_PREVIEW) {
            gSaveContext.timerSeconds = std::max<int16_t>(gSaveContext.timerSeconds, CVAR_HAZARD_TIMER_MIN_VALUE);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterHazardTimerMin, { CVAR_HAZARD_TIMER_MIN_NAME });
