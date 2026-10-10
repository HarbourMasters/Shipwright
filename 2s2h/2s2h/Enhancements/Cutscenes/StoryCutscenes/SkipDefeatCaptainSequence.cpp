#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void SkipDefeatCaptainTextbox() {
    // from func_80C0D9B4
    gPlayState->nextEntrance = Entrance_CreateFromSpawn(5);
    gSaveContext.nextCutsceneIndex = 0;
    gPlayState->transitionTrigger = 0x14;
    gPlayState->transitionType = 2;
    gSaveContext.nextTransitionType = 3;
}

void SkipDefeatCaptainCutscene() {
    Player* player = GET_PLAYER(gPlayState);

    // the cutscene sets link's position/rotation to
    //
    // player->actor.world = { { -100.0f, 474.0f, -2330.0f }, { 0, -16384, 0 } };
    //
    // but link walks forward when not told not to by the cutscene, so we're
    // setting link's position back a little bit so he doesn't walk off the ledge
    player->actor.world = { { 64.0f, 488.0f, -2332.0f }, { 0, -16384, 0 } };

    // make sure captain's gone
    SET_WEEKEVENTREG(WEEKEVENTREG_23_04);

    // get rid of the fire from the chest
    Flags_SetSwitch(gPlayState, 0x0B);
}

void RegisterSkipDefeatCaptainSequence() {
    COND_VB_SHOULD(VB_PLAY_DEFEAT_CAPTAIN_SEQUENCE, CVAR, {
        *should = false;
        SkipDefeatCaptainTextbox();
    });

    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_BOTI && *csId == 20) {
            *should = false;
            SkipDefeatCaptainCutscene();
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipDefeatCaptainSequence, { CVAR_NAME, "IS_RANDO" });
