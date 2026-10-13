#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.DifficultyOptions.HyperEnemies"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterHyperEnemies() {
    COND_HOOK(OnActorUpdate, CVAR, [](Actor* actor) {
        if (actor->category != ACTORCAT_ENEMY || actor->id == ACTOR_EN_DG) {
            return;
        }

        if (Player_InBlockingCsMode(gPlayState, GET_PLAYER(gPlayState))) {
            return;
        }

        if (gPlayState->sceneId == SCENE_SYATEKI_MIZU || // Town Shooting Gallery
            gPlayState->sceneId == SCENE_SYATEKI_MORI) { // Swamp Shooting Gallery
            return;
        }

        if (actor->update != NULL) {
            // Everywhere we need to disable the collision checks already checks if frameAdvance is enabled, so we abuse
            // that temporarily. This doesn't have any _known_ side effects :)
            bool prevFrameAdv = gPlayState->frameAdvCtx.enabled;
            gPlayState->frameAdvCtx.enabled = true;
            actor->update(actor, gPlayState);
            gPlayState->frameAdvCtx.enabled = prevFrameAdv;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterHyperEnemies, { CVAR_NAME });
