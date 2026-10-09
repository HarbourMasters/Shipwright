#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Yb/z_en_yb.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipKamaroTeachDance() {
    /*
     * The game strangely forces you to watch Kamaro dance for 10 seconds before continuing his dialog, so we just set
     * that timer to 0.
     */
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        // Cutscene 41 in Termina Field is the Kamaro dance scene.
        if (gPlayState->sceneId == SCENE_00KEIKOKU) {
            s16* csId = va_arg(args, s16*);
            if (*csId == 41) {
                EnYb* enYb = va_arg(args, EnYb*);
                enYb->teachingCutsceneTimer = 0;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipKamaroTeachDance, { CVAR_NAME });
