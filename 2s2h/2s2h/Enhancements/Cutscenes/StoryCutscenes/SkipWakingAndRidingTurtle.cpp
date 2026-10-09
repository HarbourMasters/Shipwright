#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_Dm_Char08/z_dm_char08.h"

void DmChar08_Init(Actor* thisx, PlayState* play2);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipWakingAndRidingTurtle() {
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_31MISAKI) {
            // 12 is first time waking turtle, 20 is subsequent times
            if (*csId == 12 || *csId == 20) {
                DmChar08* dmChar08 = (DmChar08*)Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor,
                                                                 ACTOR_DM_CHAR08, ACTORCAT_BG, 99999.9f);
                if (!dmChar08) {
                    return;
                }

                *should = false;

                // This gets set soon after this hook executes, but needs to be set before reinitializing turtle
                SET_WEEKEVENTREG(WEEKEVENTREG_53_20);

                Actor_Kill(dmChar08->palmTree1);
                Actor_Kill(dmChar08->palmTree2);
                dmChar08->dyna.actor.init = DmChar08_Init;

                Audio_PlayFanfare(NA_BGM_DUNGEON_APPEAR);
            }
            // 13 is turtle leaving zora cape first time, 15 is subsequent times
            if (*csId == 13 || *csId == 15) {
                *should = false;
                GameInteractor::Instance->events.emplace_back(GIEventTransition{
                    .entrance = ENTRANCE(GREAT_BAY_TEMPLE, 0),
                    .cutsceneIndex = 0,
                    .transitionTrigger = TRANS_TRIGGER_START,
                    .transitionType = TRANS_TYPE_FADE_BLACK_FAST,
                });
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipWakingAndRidingTurtle, { CVAR_NAME });
