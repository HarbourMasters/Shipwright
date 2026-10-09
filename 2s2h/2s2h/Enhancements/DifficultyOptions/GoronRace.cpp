#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Enhancements/Enhancements.h"

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.DifficultyOptions.GoronRace"
#define CVAR CVarGetInteger(CVAR_NAME, GORON_RACE_DIFFICULTY_VANILLA)

void RegisterGoronRaceDifficulty() {
    COND_VB_SHOULD(VB_GORON_RACE_RUBBERBANDING, CVAR == GORON_RACE_DIFFICULTY_BALANCED, {
        f32* phi_f0 = va_arg(args, f32*);
        f32 phi_f2 = *va_arg(args, f32*);
        if (*phi_f0 > phi_f2) { // Goron is speeding up
            *phi_f0 = phi_f2;
        }
    });

    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVAR == GORON_RACE_DIFFICULTY_SKIP, {
        if (gSaveContext.save.entrance == ENTRANCE(GORON_RACETRACK, 1)) { // Starting race
            SET_EVENTINF(EVENTINF_11);                                    // Won race
            gSaveContext.save.entrance = ENTRANCE(GORON_RACETRACK, 2);    // Talk to baby Goron after race
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterGoronRaceDifficulty, { CVAR_NAME });
