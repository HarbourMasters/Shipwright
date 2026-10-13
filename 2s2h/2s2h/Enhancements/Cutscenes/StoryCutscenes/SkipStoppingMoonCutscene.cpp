#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipStoppingMoon() {
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_OKUJOU) {
            if (*csId == 12) {
                if (GameInteractor_Should(VB_MEET_MOON_REQUIREMENTS, CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA) &&
                                                                         CHECK_QUEST_ITEM(QUEST_REMAINS_GOHT) &&
                                                                         CHECK_QUEST_ITEM(QUEST_REMAINS_GYORG) &&
                                                                         CHECK_QUEST_ITEM(QUEST_REMAINS_TWINMOLD))) {
                    *should = false;
                    GameInteractor::Instance->events.emplace_back(GIEventTransition{
                        .entrance = ENTRANCE(THE_MOON, 0),
                        .cutsceneIndex = 0,
                        .transitionTrigger = TRANS_TRIGGER_START,
                        .transitionType = TRANS_TYPE_INSTANT,
                    });
                    SET_WEEKEVENTREG(WEEKEVENTREG_25_02); // giants called to tower
                    SET_WEEKEVENTREG(WEEKEVENTREG_93_04); // watched giants stopping moon cs (persistant)
                }
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipStoppingMoon, { CVAR_NAME });
