#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_Bg_Ingate/z_bg_ingate.h"
}

#define CVAR_NAME "gEnhancements.Timesavers.SwampBoatSpeed"
#define CVAR CVarGetInteger(CVAR_NAME, 0)
#define SCORE_CVAR_NAME "gEnhancements.Minigames.BoatArcheryScore"
#define SCORE_CVAR CVarGetInteger(SCORE_CVAR_NAME, 20)

void RegisterSwampBoatSpeed() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_BG_INGATE, CVAR, [](Actor* actor) {
        BgIngate* boat = (BgIngate*)actor;
        if (DynaPolyActor_IsPlayerOnTop(&boat->dyna)) {
            if (!CHECK_EVENTINF(EVENTINF_35)) {
                // Pictograph Tour
                if (CHECK_BTN_ALL(CONTROLLER1(&gPlayState->state)->cur.button, BTN_Z)) {
                    boat->timePathTimeSpeed = (s16)(4 * 5);
                } else {
                    boat->timePathTimeSpeed = 4; // Default speed
                }
            } else if (CHECK_EVENTINF(EVENTINF_35) && HS_GET_BOAT_ARCHERY_HIGH_SCORE() >= SCORE_CVAR) {
                // Archery Minigame
                if (CHECK_BTN_ALL(CONTROLLER1(&gPlayState->state)->cur.button, BTN_Z)) {
                    boat->timePathTimeSpeed = (s16)(1 * 5);
                } else {
                    boat->timePathTimeSpeed = 1; // Default speed
                }
            } else if (CHECK_EVENTINF(EVENTINF_35) && gSaveContext.minigameScore >= SCORE_CVAR) { // Current score
                // Update boat archery high score early
                // Leaving minigame early prevents the score from being updated
                if (gSaveContext.minigameScore > HS_GET_BOAT_ARCHERY_HIGH_SCORE()) {
                    HS_SET_BOAT_ARCHERY_HIGH_SCORE(gSaveContext.minigameScore);
                    SET_EVENTINF(EVENTINF_37);
                }

                // Leave the minigame
                gPlayState->nextEntrance = ENTRANCE(TOURIST_INFORMATION, 1); // 1 = Koume
                gSaveContext.nextCutsceneIndex = 0;
                gPlayState->transitionTrigger = TRANS_TRIGGER_START;
                gPlayState->transitionType = TRANS_TYPE_FADE_WHITE;
                gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                return;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSwampBoatSpeed, { CVAR_NAME });