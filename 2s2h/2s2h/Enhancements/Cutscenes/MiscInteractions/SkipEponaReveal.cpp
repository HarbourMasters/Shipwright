#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_Bg_Umajump/z_bg_umajump.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipEponaReveal() {
    COND_VB_SHOULD(VB_QUEUE_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);

        if (gPlayState->sceneId != SCENE_F01) { // Romani Ranch
            return;
        }

        BgUmajump* bgUmajump = (BgUmajump*)Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor,
                                                            ACTOR_BG_UMAJUMP, ACTORCAT_PROP, 99999.9f);
        if (!bgUmajump) {
            return;
        }

        if (*csId != 20) { // Epona Reveal
            return;
        }

        *should = false;
        SET_WEEKEVENTREG(WEEKEVENTREG_89_20);
        bgUmajump->dyna.actor.update = Actor_Noop;
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipEponaReveal, { CVAR_NAME });
