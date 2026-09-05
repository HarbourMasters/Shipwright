#include "global.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

void FrameAdvance_Init(FrameAdvanceContext* frameAdvCtx) {
    frameAdvCtx->timer = 0;
    frameAdvCtx->enabled = false;
}

/**
 * Frame advance allows you to advance through the game one frame at a time on command.
 * To enable, hold R and press Dpad Down on the specified controller.
 * To advance a frame, hold Z and press R.
 * Holding Z and R will advance a frame every half second.
 *
 * This function returns true when frame advance is not active (game will run normally)
 */
s32 FrameAdvance_Update(FrameAdvanceContext* frameAdvCtx, Input* input) {
    if (GameInteractor_Should(VB_FRAME_ADVANCE_BE_VANILLA, true, frameAdvCtx)) {
        // Vanilla Frame Advance
        if (CHECK_BTN_ALL(input->cur.button, BTN_R) && CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
            frameAdvCtx->enabled = !frameAdvCtx->enabled;
        }

        if (!frameAdvCtx->enabled || CVarGetInteger(CVAR_DEVELOPER_TOOLS("FrameAdvanceTick"), 0) ||
            (CHECK_BTN_ALL(input->cur.button, BTN_Z) &&
             (CHECK_BTN_ALL(input->press.button, BTN_R) ||
              (CHECK_BTN_ALL(input->cur.button, BTN_R) && (++frameAdvCtx->timer >= 9))))) {
            CVarClear(CVAR_DEVELOPER_TOOLS("FrameAdvanceTick"));
            frameAdvCtx->timer = 0;
            return true;
        }

        return false;
    }

    // Call hooks and ask if we should freeze the frame
    if (GameInteractor_Should(VB_FRAME_ADVANCE_FREEZE_FRAME, false, frameAdvCtx)) {
        return false;
    }
    // No hooks said we should freeze the frame, so run the game normally
    return true;
}
