#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipRaisingWoodfall() {
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_21MITURINMAE) {
            if (*csId == 11) {
                *should = false;

                // Need to reload the scene after WEEKEVENTREG_12_20 gets set for the temple to be up.
                // Based on WarpPoint.cpp
                Player* player = GET_PLAYER(gPlayState);

                gPlayState->nextEntrance = ENTRANCE(WOODFALL, 0);
                gPlayState->transitionTrigger = TRANS_TRIGGER_START;
                gPlayState->transitionType = TRANS_TYPE_INSTANT;

                gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance = ENTRANCE(WOODFALL, 0);
                gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex = 0;
                gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.x = player->actor.world.pos.x;
                gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.y = player->actor.world.pos.y;
                gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.z = player->actor.world.pos.z;
                gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = player->actor.shape.rot.y;
                gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = PLAYER_PARAMS(0xFF, PLAYER_START_MODE_D);
                gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK_FAST;
                gSaveContext.respawnFlag = -8;

                Audio_PlaySequenceInCutscene(NA_BGM_DUNGEON_APPEAR);
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipRaisingWoodfall, { CVAR_NAME });
