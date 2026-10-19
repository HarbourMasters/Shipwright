#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "variables.h"
}

#define CVAR_NAME CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story")
#define CVAR_VALUE CVarGetInteger(CVAR_NAME, IS_RANDO)

void RegisterSkipGanonsTowerCollapse() {
    // Zelda coming down in her crystal after Ganondorf dies
    COND_VB_SHOULD(VB_PLAY_ZELDA_CRYSTAL_CS, CVAR_VALUE || IS_RANDO || IS_BOSS_RUSH, { *should = false; });

    // The tower falling after the escape, same as what Skip Tower Escape does on arrival
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVAR_VALUE, {
        if (gEntranceTable[gSaveContext.entranceIndex].scene == SCENE_GANON_BOSS &&
            !Flags_GetEventChkInf(EVENTCHKINF_WATCHED_GANONS_CASTLE_COLLAPSE_CAUGHT_BY_GERUDO)) {
            Flags_SetEventChkInf(EVENTCHKINF_WATCHED_GANONS_CASTLE_COLLAPSE_CAUGHT_BY_GERUDO);
            gSaveContext.entranceIndex = ENTR_GANON_BOSS_0;
            *should = false;
        }
    });

    // Zelda talking once the tower has fallen
    COND_VB_SHOULD(VB_PLAY_ESCAPED_TOWER_CS, CVAR_VALUE || IS_RANDO || IS_BOSS_RUSH, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterSkipGanonsTowerCollapse, { CVAR_NAME, "IS_RANDO", "IS_BOSS_RUSH" });
