#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "src/overlays/actors/ovl_En_Horse_Game_Check/z_en_horse_game_check.h"
}

#define CVAR_INGO_RACE_ONCE_NAME CVAR_ENHANCEMENT("IngoRaceOnce")
#define CVAR_INGO_RACE_ONCE_VALUE CVarGetInteger(CVAR_INGO_RACE_ONCE_NAME, INGO_RACE_TWICE)

static void IngoRaceInstantWin(void* refActor) {
    EnHorseGameCheckBase* base = reinterpret_cast<EnHorseGameCheckBase*>(refActor);
    if (base->actor.params != HORSEGAME_INGO_RACE) {
        return;
    }

    EnHorseGameCheckIngoRace* ingoRace = reinterpret_cast<EnHorseGameCheckIngoRace*>(base);
    ingoRace->result = 1;
}

static void RegisterIngoRaceOnce() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_HORSE_GAME_CHECK, CVAR_INGO_RACE_ONCE_VALUE == INGO_RACE_NONE, IngoRaceInstantWin);

    COND_VB_SHOULD(VB_LINK_WIN_EPONA, CVAR_INGO_RACE_ONCE_VALUE != INGO_RACE_TWICE, { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterIngoRaceOnce, { CVAR_INGO_RACE_ONCE_NAME });
