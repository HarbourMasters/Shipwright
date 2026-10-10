#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/ShipInit.hpp"
#include "SavingEnhancements.h"

extern "C" PlayState* gPlayState;

#define CVAR_NAME "gEnhancements.Saving.PauseSave"
#define CVAR CVarGetInteger(CVAR_NAME, false)

void RegisterPauseSave() {
    COND_VB_SHOULD(VB_SAVE_ON_B_BUTTON_IN_PAUSE_MENU, CVAR, {
        Input* input = CONTROLLER1(&gPlayState->state);
        *should = SavingEnhancements_CanSave() && CHECK_BTN_ALL(input->press.button, BTN_B);
    });
}

static RegisterShipInitFunc initFunc(RegisterPauseSave, { CVAR_NAME });
