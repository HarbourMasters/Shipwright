#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

#define BIGGORON_GORON_LULLABY_CSID 11

void RegisterSkipBiggoronSnowheadLullabyCutscene() {
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        Actor* actor = va_arg(args, Actor*);

        if (*csId != BIGGORON_GORON_LULLABY_CSID || actor == NULL || actor->id != ACTOR_EN_DAI ||
            gPlayState->sceneId != SCENE_12HAKUGINMAE) {
            return;
        }

        *should = false;

        // This register gets set at the end of the cutscene after Biggoron falls off of the
        // Snowhead Temple path. The actor can be killed since he doesn't do anything, and
        // in fact the actor is automatically killed when the scene is reloaded if this is set.
        SET_WEEKEVENTREG(WEEKEVENTREG_30_01);

        // Make the cutscene skip less jarring by playing the SFX that
        // plays at the end of the cutscene so that the player knows
        // the cutscene was skipped.
        Audio_PlaySfx(NA_SE_EV_ROLL_AND_FALL);
        Audio_PlaySequenceInCutscene(NA_BGM_DUNGEON_APPEAR);
        Actor_Kill(actor);
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipBiggoronSnowheadLullabyCutscene, { CVAR_NAME });
