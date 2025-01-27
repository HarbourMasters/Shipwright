#include <libultraship/libultraship.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "overlays/misc/ovl_kaleido_scope/z_kaleido_scope.h"
}

#define CVAR_BUFFER_NAME CVAR_ENHANCEMENT("PauseBufferWindow")
#define CVAR_BUFFER_DEFAULT 0
#define CVAR_BUFFER_VALUE CVarGetInteger(CVAR_BUFFER_NAME, CVAR_BUFFER_DEFAULT)

#define CVAR_INCLUDE_NAME CVAR_ENHANCEMENT("IncludeHeldInputsBufferWindow")
#define CVAR_INCLUDE_DEFAULT 0
#define CVAR_INCLUDE_VALUE CVarGetInteger(CVAR_INCLUDE_NAME, CVAR_INCLUDE_DEFAULT)

#define CVAR_FRAME_ADVANCE_NAME CVAR_CHEAT("EasyFrameAdvance")
#define CVAR_FRAME_ADVANCE_DEFAULT 0
#define CVAR_FRAME_ADVANCE_VALUE CVarGetInteger(CVAR_FRAME_ADVANCE_NAME, CVAR_FRAME_ADVANCE_DEFAULT)

static u16 inputBufferTimer = 0;
static u16 prePauseInputs = 0;
static u16 pauseInputs = 0;
#define PAUSE_STATE_OFF 0
#define PAUSE_STATE_OPENING_1 2
#define PAUSE_STATE_UNPAUSE_SETUP 18

void RegisterPauseBufferInputs() {
    COND_VB_SHOULD(VB_KALEIDO_UNPAUSE_CLOSE, CVAR_BUFFER_VALUE || CVAR_INCLUDE_VALUE, {
        Input* input = &gPlayState->state.input[0];

        // Store all inputs that were pressed during the buffer window
        pauseInputs |= input->press.button;

        // If the user opts to include held inputs in the buffer window, store the held inputs, minus the held inputs when the pause menu was opened
        if (CVAR_INCLUDE_VALUE && inputBufferTimer == 0) {
            pauseInputs |= input->cur.button & ~prePauseInputs;
            prePauseInputs = 0;
        }

        // Wait a specified number of frames before continuing the unpause
        inputBufferTimer++;
        if (inputBufferTimer < CVAR_BUFFER_VALUE) {
            *should = false;
        }
    });

    COND_HOOK(OnGameStateMainStart, CVAR_BUFFER_VALUE || CVAR_INCLUDE_VALUE, []() {
        if (gPlayState == NULL) {
            return;
        }

        Input* input = &gPlayState->state.input[0];
        PauseContext* pauseCtx = &gPlayState->pauseCtx;

        // if the input buffer timer is not 0 and the pause state is off, then the player just unpaused
        if (inputBufferTimer != 0 && pauseCtx->state == PAUSE_STATE_OFF) {
            inputBufferTimer = 0;

            // If the user opts into easy frame advance, remove START input
            if (CVAR_FRAME_ADVANCE_VALUE) {
                pauseInputs &= ~BTN_START;
            }

            // So we need to re-apply the inputs that were pressed during the buffer window
            input->press.button |= pauseInputs;
        }

        // Reset the timer and stored inputs at the beginning of the unpause process
        if (pauseCtx->state == PAUSE_STATE_UNPAUSE_SETUP && pauseCtx->unk_1F4 != 160.0f) {
            inputBufferTimer = 0;
            pauseInputs = 0;
        }

        // If the user opts to include held inputs in the buffer window, store the held inputs at the beginning of the pause process, minus the START input
        if (pauseCtx->state == PAUSE_STATE_OPENING_1 && CVAR_INCLUDE_VALUE) {
            prePauseInputs = input->cur.button & ~BTN_START;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterPauseBufferInputs, { CVAR_BUFFER_NAME, CVAR_INCLUDE_NAME });
