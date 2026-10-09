#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static void RegisterSkipHideoutCutscenes() {
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_IKANA) {
            if (*csId == 14 || *csId == 34) {
                *should = false;
            }
        } else if (gPlayState->sceneId == SCENE_SECOM) {
            if (*csId == 21) {
                *should = false;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipHideoutCutscenes, { CVAR_NAME });
