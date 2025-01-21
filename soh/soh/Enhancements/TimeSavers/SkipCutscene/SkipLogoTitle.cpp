#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

extern "C" {
#include "macros.h"
#include "z64.h"
}

void SkipLogoTitle_Register() {
    // Allows pressing A to skip the boot logo and go to the next state (opening or file select)
    COND_HOOK(OnZTitleUpdate, true, [](void* gameState) {
        TitleContext* titleContext = (TitleContext*)gameState;

        if (CHECK_BTN_ANY(titleContext->state.input->press.button, BTN_A | BTN_B | BTN_START)) {
            // Force the title state to start fading to black and to last roughly 5 frames based on current fade in/out
            titleContext->visibleDuration = 0;
            titleContext->addAlpha = (255 - titleContext->coverAlpha) / 5;
        }
    });
}
