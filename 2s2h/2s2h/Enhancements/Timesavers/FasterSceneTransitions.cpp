#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "z64.h"
#include "z64transition.h"

void Play_SetupTransition(PlayState* playState, s32 transitionType);
};

#define CVAR_NAME "gEnhancements.Timesavers.FasterSceneTransitions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static void SetupFasterSceneTransition() {
    auto transitionType = gPlayState->transitionType;

    switch (transitionType) {
        case TRANS_TYPE_FADE_BLACK:
            transitionType = TRANS_TYPE_FADE_BLACK_FAST;
            break;

        case TRANS_TYPE_FADE_WHITE:
            transitionType = TRANS_TYPE_FADE_WHITE_FAST;
            break;

        case TRANS_TYPE_WIPE:
            transitionType = TRANS_TYPE_WIPE_FAST;
            break;

        case TRANS_TYPE_FILL_WHITE:
            transitionType = TRANS_TYPE_FILL_WHITE_FAST;
            break;

        case TRANS_TYPE_FADE_DYNAMIC:
            transitionType = gSaveContext.save.isNight ? TRANS_TYPE_FADE_BLACK_FAST : TRANS_TYPE_FADE_WHITE_FAST;
            break;
    }

    gPlayState->transitionType = transitionType;
}

void RegisterFasterSceneTransitions() {
    COND_VB_SHOULD(VB_SETUP_TRANSITION, CVAR, {
        switch (gPlayState->sceneId) {
            case SCENE_SPOT00:    // Prevent glitch going into the save file select
            case SCENE_KAKUSIANA: // Falling into grottos causes fall damage with fast transitions
            case SCENE_HAKASHITA: // Similar but falling into a grave
                break;

            default:
                SetupFasterSceneTransition();
                Play_SetupTransition(gPlayState, gPlayState->transitionType);
                *should = false;
                break;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFasterSceneTransitions, { CVAR_NAME });
