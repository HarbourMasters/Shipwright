#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "z64.h"
#include "functions.h"
#include "variables.h"
}

extern "C" {
extern void Title_Calc(TitleContext*);
extern void Title_Draw(TitleContext*);
}

extern "C" void CustomLogoTitle_Main(void* gameState) {
    TitleContext* titleContext = (TitleContext*)gameState;

    OPEN_DISPS(titleContext->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0, (uintptr_t)NULL);
    gSPSegment(POLY_OPA_DISP++, 1, (uintptr_t)titleContext->staticSegment);
    Gfx_SetupFrame(titleContext->state.gfxCtx, 0, 0, 0);
    Title_Calc(titleContext);
    Title_Draw(titleContext);

    if (titleContext->exit || CVarGetInteger(CVAR_DEVELOPER_TOOLS("SkipLogoTitle"), 0)) {
        gSaveContext.seqId = (u8)NA_BGM_DISABLED;
        gSaveContext.natureAmbienceId = 0xFF;
        gSaveContext.gameMode = 1;
        titleContext->state.running = false;

        if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("SkipLogoTitle"), 0))
            SET_NEXT_GAMESTATE(&titleContext->state, FileChoose_Init, FileChooseContext);
        else
            SET_NEXT_GAMESTATE(&titleContext->state, Opening_Init, OpeningContext);
    }

    GameInteractor_ExecuteOnZTitleUpdate(titleContext);

    CLOSE_DISPS(titleContext->state.gfxCtx);
}

// Allows pressing A to skip the boot logo and go to the next state (opening or file select)
void OnZTitleUpdateSkipLogoTitle(void* gameState) {
    TitleContext* titleContext = (TitleContext*)gameState;

    if (CHECK_BTN_ANY(titleContext->state.input->press.button, BTN_A | BTN_B | BTN_START)) {
        // Force the title state to start fading to black and to last roughly 5 frames based on current fade in/out
        titleContext->visibleDuration = 0;
        titleContext->addAlpha = (255 - titleContext->coverAlpha) / 5;
    }
}

void OnZTitleInitReplaceTitleMainWithCustom(void* gameState) {
    TitleContext* titleContext = (TitleContext*)gameState;

    titleContext->state.main = (GameStateFunc)CustomLogoTitle_Main;
}

void RegisterCustomLogoTitle() {
    COND_HOOK(OnZTitleInit, true, OnZTitleInitReplaceTitleMainWithCustom);
    COND_HOOK(OnZTitleUpdate, true, OnZTitleUpdateSkipLogoTitle);
}

static RegisterShipInitFunc initFunc(RegisterCustomLogoTitle);
