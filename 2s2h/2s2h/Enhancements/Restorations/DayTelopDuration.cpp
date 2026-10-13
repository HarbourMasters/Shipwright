#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "overlays/gamestates/ovl_daytelop/z_daytelop.h"
}

#define CVAR_NAME "gEnhancements.Restorations.DayTelopDuration"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterDayTelopDurationRestoration() {
    COND_HOOK(OnGameStateMainStart, CVAR, []() {
        if (gGameState == nullptr || gGameState->destroy != DayTelop_Destroy) {
            return;
        }

        // Only add frames the first time the DayTelop state is detected
        if (gGameState->frames != 1) {
            return;
        }

        DayTelopState* dayTelop = reinterpret_cast<DayTelopState*>(gGameState);
        dayTelop->transitionCountdown += 120;
    });
}

static RegisterShipInitFunc initFunc(RegisterDayTelopDurationRestoration, { CVAR_NAME });
