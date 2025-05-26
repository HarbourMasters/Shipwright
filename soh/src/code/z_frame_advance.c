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
    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("FrameAdvanceAltScheme"), 0) != 0) {
        // Frame Advance Alternative Control Scheme

        // Push START to toggle the frame advance mode.
        if (CHECK_BTN_ALL(input->press.button, BTN_START)) {
            frameAdvCtx->enabled = !frameAdvCtx->enabled;
        }

        // Push A to advance one frame.
        // Hold L to run normally until L is released.
        // Hold R to advance a frame every half second.
        if (!frameAdvCtx->enabled || CVarGetInteger(CVAR_DEVELOPER_TOOLS("FrameAdvanceTick"), 0) ||
            CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->cur.button, BTN_L) ||
            CHECK_BTN_ALL(input->press.button, BTN_R) ||
            (CHECK_BTN_ALL(input->cur.button, BTN_R) && (++frameAdvCtx->timer >= 9))) {
            CVarClear(CVAR_DEVELOPER_TOOLS("FrameAdvanceTick"));
            frameAdvCtx->timer = 0;
            return true;
        }

        return false;
    } else {
        // Frame Advance Original Control Scheme

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
}
