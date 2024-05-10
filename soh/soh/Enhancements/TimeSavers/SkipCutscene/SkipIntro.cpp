#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/OTRGlobals.h"

extern "C" {
    #include "z64save.h"
    #include "functions.h"
    extern PlayState* gPlayState;
    extern SaveContext gSaveContext;
}

void SkipIntro_Register() {
    REGISTER_VB_SHOULD(VB_PLAY_TRANSITION_CS, {
        if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Intro"), IS_RANDO)) {
            if (gSaveContext.entranceIndex == ENTR_LINKS_HOUSE_0 && gSaveContext.cutsceneIndex == 0xFFF1) {
                gSaveContext.cutsceneIndex = 0;
                *should = false;
            }
        }
    });
}
