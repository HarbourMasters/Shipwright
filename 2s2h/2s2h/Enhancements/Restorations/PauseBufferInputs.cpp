#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
}

#define CVAR_NAME "gEnhancements.Restorations.PauseBufferWindow"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static u16 inputBufferTimer = 0;
static u16 pauseInputs = 0;

void RegisterPauseBufferInputs() {
    COND_VB_SHOULD(VB_KALEIDO_UNPAUSE_CLOSE, CVAR, {
        Input* input = CONTROLLER1(&gPlayState->state);

        // Store all inputs that were pressed during the buffer window
        pauseInputs |= input->press.button;

        // Wait a specified number of frames before continuing the unpause
        inputBufferTimer++;
        if (inputBufferTimer < CVAR) {
            *should = false;
        }
    });

    COND_HOOK(OnGameStateMainStart, CVAR, []() {
        if (gPlayState == NULL) {
            return;
        }

        Input* input = CONTROLLER1(&gPlayState->state);
        PauseContext* pauseCtx = &gPlayState->pauseCtx;

        // if the input buffer timer is not 0 and the pause state is off, then the player just unpaused
        if (inputBufferTimer != 0 && pauseCtx->state == PAUSE_STATE_OFF) {
            // So we need to re-apply the inputs that were pressed during the buffer window
            input->press.button |= pauseInputs;

            inputBufferTimer = 0;
            pauseInputs = 0;
        } else if (pauseCtx->state != PAUSE_STATE_UNPAUSE_CLOSE) {
            // Reset the timer and stored inputs at the beginning of the unpause process
            inputBufferTimer = 0;
            pauseInputs = 0;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterPauseBufferInputs, { CVAR_NAME });
