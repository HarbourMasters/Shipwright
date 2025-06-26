#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "variables.h"
extern PlayState* gPlayState;
extern void func_808237B4(PlayState* play, Input* input);
}

#define CVAR_FASTER_PAUSE_MENU_NAME CVAR_ENHANCEMENT("FasterPauseMenu")
#define CVAR_FASTER_PAUSE_MENU_VALUE CVarGetInteger(CVAR_FASTER_PAUSE_MENU_NAME, 0)

void OnKaleidoUpdateFaster() {
    ZREG(46) = 2; // pauseCtx->eye and pauseCtx->unk_1EA multiply by this for animation. Double the default value.
    WREG(6) = 4;  // Numerous kaleido animations divide by this for movement and alpha. Half the default value.

    // Page turn animation is governed by func_808237B4. Those values don't use registers to modify the speed, so we
    // just directly call it twice to effectively double the speed.
    if (gPlayState->pauseCtx.state == 6 && gPlayState->pauseCtx.unk_1E4 == 1) { // In page turning mode
        func_808237B4(gPlayState, gPlayState->state.input);
    }
}

void InitFasterPauseMenu() {
    COND_HOOK(GameInteractor::OnKaleidoUpdate, CVAR_FASTER_PAUSE_MENU_VALUE, OnKaleidoUpdateFaster);

    // Reset register values on close. These values are only used by z_kaleido_scope_PAL.c
    COND_VB_SHOULD(VB_KALEIDO_UNPAUSE_CLOSE, CVAR_FASTER_PAUSE_MENU_VALUE, {
        // Default values, as defined in Regs_InitDataImpl
        ZREG(46) = 1;
        WREG(6) = 8;
    });
}

static RegisterShipInitFunc initFunc(InitFasterPauseMenu, { CVAR_FASTER_PAUSE_MENU_NAME });
