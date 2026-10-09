#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/ShipUtils.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Test7/z_en_test7.h"
}

#define CVAR_NAME "gEnhancements.Songs.SkipSoaringCutscene"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static void SkipSoaringCutscene(Actor* actor, bool* should) {
    s16 ocarinaMode = OWL_WARP_CS_GET_OCARINA_MODE(actor);
    if (ocarinaMode == ENTEST7_ARRIVE) {
        return;
    }

    *should = false;

    if (gPlayState->sceneId == SCENE_SECOM) {
        gPlayState->nextEntrance = ENTRANCE(IKANA_CANYON, 6);
    } else if (ocarinaMode == OCARINA_MODE_WARP_TO_ENTRANCE) {
        func_80169F78(gPlayState);
        gSaveContext.respawn[RESPAWN_MODE_TOP].playerParams =
            PLAYER_PARAMS(gSaveContext.respawn[RESPAWN_MODE_TOP].playerParams, PLAYER_START_MODE_OWL);
        gSaveContext.respawnFlag = -6;
    } else {
        gPlayState->nextEntrance = sOwlWarpEntrancesForMods[ocarinaMode - OCARINA_MODE_WARP_TO_GREAT_BAY_COAST];
    }

    gPlayState->transitionTrigger = TRANS_TRIGGER_START;
    gPlayState->transitionType = TRANS_TYPE_FADE_BLACK;
}

static void RegisterSkipSoaringCutscene() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_TEST7, CVAR, SkipSoaringCutscene);
}

static RegisterShipInitFunc initFunc(RegisterSkipSoaringCutscene, { CVAR_NAME });
