#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/OTRGlobals.h"

#include <spdlog/spdlog.h>

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"

extern PlayState* gPlayState;
}

#define CVAR_FRAMEADVANCEALTSCHEME_NAME CVAR_DEVELOPER_TOOLS("FrameAdvanceAltScheme")
#define CVAR_FRAMEADVANCEALTSCHEME_DEFAULT 0
#define CVAR_FRAMEADVANCEALTSCHEME_VALUE \
    CVarGetInteger(CVAR_FRAMEADVANCEALTSCHEME_NAME, CVAR_FRAMEADVANCEALTSCHEME_DEFAULT)

void RegisterFrameAdvanceAltScheme() {
    COND_VB_SHOULD(VB_FRAME_ADVANCE_BE_VANILLA, CVAR_FRAMEADVANCEALTSCHEME_VALUE, {
        // Do not run vanilla Frame Advance codes.
        *should = false;
    });

    COND_VB_SHOULD(VB_FRAME_ADVANCE_FREEZE_FRAME, CVAR_FRAMEADVANCEALTSCHEME_VALUE, {
        FrameAdvanceContext* frameAdvCtx = va_arg(args, FrameAdvanceContext*);
        Input* input = gPlayState->state.input;
        bool runFrame = false;

        // Push START to toggle the frame advance mode.
        if (CHECK_BTN_ALL(input[3].press.button, BTN_START)) {
            frameAdvCtx->enabled = !frameAdvCtx->enabled;
            if (frameAdvCtx->enabled) {
                SPDLOG_DEBUG("Frame Advance is now enabled");
            } else {
                SPDLOG_DEBUG("Frame Advance is now disabled");
            }
        }

        // Push A to advance one frame.
        // Hold L to run normally until L is released.
        // Hold R to advance a frame every half second.
        if (!frameAdvCtx->enabled || CVarGetInteger(CVAR_DEVELOPER_TOOLS("FrameAdvanceTick"), 0) ||
            CHECK_BTN_ALL(input[3].press.button, BTN_A) || CHECK_BTN_ALL(input[3].cur.button, BTN_L) ||
            CHECK_BTN_ALL(input[3].press.button, BTN_R) ||
            (CHECK_BTN_ALL(input[3].cur.button, BTN_R) && (++frameAdvCtx->timer >= 9))) {
            CVarClear(CVAR_DEVELOPER_TOOLS("FrameAdvanceTick"));
            frameAdvCtx->timer = 0;
            runFrame = true;
        }

        *should = !runFrame;
    });
}

static RegisterShipInitFunc initFunc_FrameAdvanceAltScheme(RegisterFrameAdvanceAltScheme,
                                                           { CVAR_FRAMEADVANCEALTSCHEME_NAME });
