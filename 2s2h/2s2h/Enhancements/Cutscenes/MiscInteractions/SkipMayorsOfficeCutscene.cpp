#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Dt/z_en_dt.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// These skips still allow the first textboxes to display, but they do ignore most of the scenes
static bool ShouldStartMayorsOfficeCutscene(s16 csId) {
    /*
     * The Mayor starts these cutscenes but passes the target actor, which always seems to be Viscen for the start.
     * The Mayor also handles the logic for progressing and ending these cutscenes. We need to alter Dotour's state
     * but cannot rely on the cutscene start actor for that. Because of this, we use Actor_FindNearby to find the
     * EnDt in this scene.
     */
    EnDt* enDt =
        (EnDt*)Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor, ACTOR_EN_DT, ACTORCAT_NPC, 99999.9f);
    if (enDt != nullptr) {
        if (csId == 17) { // Argument scenes without Couples Mask
            enDt->csIdIndex = 26;
            enDt->cutsceneState = 2; // EN_DT_CS_STATE_PLAYING
            enDt->textIdIndex = 8;
            Message_BombersNotebookQueueEvent(gPlayState, BOMBERS_NOTEBOOK_PERSON_MAYOR_DOTOUR);
            return false;
        } else if (csId == 21) { // Couples Mask scene
            // Set flags to trigger scene transition and reward
            enDt->timer = 0;
            enDt->textIdIndex = 20;
            enDt->cutsceneState = 0; // EN_DT_CS_STATE_NONE
            return false;
        }
    }

    return true;
}

static void RegisterSkipMayorsOfficeCutscene() {
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        if (gPlayState->sceneId != SCENE_SONCHONOIE) {
            return;
        }

        s16* csId = va_arg(args, s16*);
        *should = ShouldStartMayorsOfficeCutscene(*csId);
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipMayorsOfficeCutscene, { CVAR_NAME });
