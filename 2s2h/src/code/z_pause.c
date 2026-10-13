/*
 * File: z_pause.c
 * Description: Frame Advance debug feature
 *
 * This allows you to advance through the game one frame at a time on command.
 * To advance a frame, hold Z and press R on the specified controller (see z_play).
 * Holding Z and R will advance a frame every half second.
 *
 * Note: While the system is fully hooked up, there is no way to enable it in game
 * Instead one would have to add something like:
 *
 * if (CHECK_BTN_ALL(input->cur.button, BTN_R) && CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
 *     frameAdvCtx->enabled = !frameAdvCtx->enabled;
 * }
 *
 * to the start of FrameAdvance_Update, which would allow the system to be toggled on and off by holding R
 * and pressing Dpad Down on the specified controller.
 *
 * Note2: Controllers 2-4's inputs are normally zeroed out, so this would also need to be fixed to use frame advance
 */

#include "z64frameadvance.h"
#include "libc/stdbool.h"
#include "controller.h"
#include "padutils.h"
#include <libultraship/bridge/consolevariablebridge.h>

void FrameAdvance_Init(FrameAdvanceContext* frameAdvCtx) {
    frameAdvCtx->timer = 0;
    frameAdvCtx->enabled = false;
}

/*
 * Returns true when frame advance is not active (game will run normally)
 */
s32 FrameAdvance_Update(FrameAdvanceContext* frameAdvCtx, Input* input) {
    if (CVarGetInteger("gDeveloperTools.DebugEnabled", 0)) {
        if (CHECK_BTN_ALL(input->cur.button, BTN_R) && CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
            frameAdvCtx->enabled = !frameAdvCtx->enabled;
        }
    }

    if (!frameAdvCtx->enabled || CVarGetInteger("gDeveloperTools.FrameAdvanceTick", 0) ||
        (CHECK_BTN_ALL(input->cur.button, BTN_Z) &&
         (CHECK_BTN_ALL(input->press.button, BTN_R) ||
          (CHECK_BTN_ALL(input->cur.button, BTN_R) && (++frameAdvCtx->timer >= 9))))) {
        CVarClear("gDeveloperTools.FrameAdvanceTick");
        frameAdvCtx->timer = 0;
        return true;
    }
    return false;
}
