#include <libultraship/libultraship.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "overlays/misc/ovl_kaleido_scope/z_kaleido_scope.h"
}

#define CVAR_FRAME_ADVANCE_NAME CVAR_CHEAT("EasyFrameAdvance")
#define CVAR_FRAME_ADVANCE_DEFAULT 0
#define CVAR_FRAME_ADVANCE_VALUE CVarGetInteger(CVAR_FRAME_ADVANCE_NAME, CVAR_FRAME_ADVANCE_DEFAULT)

static int frameAdvanceTimer = 0;
#define PAUSE_STATE_OFF 0
#define PAUSE_STATE_UNPAUSE_CLOSE 19

void RegisterEasyFrameAdvance() {
    COND_HOOK(OnGameStateMainStart, CVAR_FRAME_ADVANCE_VALUE, []() {
        if (gPlayState == NULL) {
            return;
        }

        Input* input = &gPlayState->state.input[0];
        PauseContext* pauseCtx = &gPlayState->pauseCtx;

        if (frameAdvanceTimer > 0 && pauseCtx->state == PAUSE_STATE_OFF) {
            frameAdvanceTimer--;
            if (frameAdvanceTimer == 0 && CHECK_BTN_ALL(input->cur.button, BTN_START)) {
                input->press.button |= BTN_START;
            }
        }

        if (pauseCtx->state == PAUSE_STATE_UNPAUSE_CLOSE) {
            frameAdvanceTimer = 2;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterEasyFrameAdvance, { CVAR_FRAME_ADVANCE_NAME });
