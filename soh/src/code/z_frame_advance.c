#include "global.h"

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
    if (CHECK_BTN_ALL(input->cur.button, BTN_R) && CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
        frameAdvCtx->enabled = !frameAdvCtx->enabled;
    }

    if (!frameAdvCtx->enabled || (CHECK_BTN_ALL(input->cur.button, BTN_Z) &&
                                  (CHECK_BTN_ALL(input->press.button, BTN_R) ||
                                   (CHECK_BTN_ALL(input->cur.button, BTN_R) && (++frameAdvCtx->timer >= 9))))) {
        frameAdvCtx->timer = 0;
        return true;
    }

    return false;
}
