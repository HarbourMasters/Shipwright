#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "z64save.h"
}

static constexpr int32_t CVAR_GC_DOOR_FIX_DEFAULT = 0;
#define CVAR_GC_DOOR_FIX_NAME CVAR_ENHANCEMENT("GCDoorsAfterFireFix")
#define CVAR_GC_DOOR_FIX_VALUE CVarGetInteger(CVAR_GC_DOOR_FIX_NAME, CVAR_GC_DOOR_FIX_DEFAULT)

static void OnInitGCDoor(void* refActor) {
    // In the off chance that Fire Temple is completed before stopping Goron Link and getting
    // Goron Tunic / opening the city doors, open them the next time Link goes to Goron City.
    if (GameInteractor_Should(VB_GORONS_CONSIDER_FIRE_TEMPLE_FINISHED, CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) &&
        !Flags_GetInfTable(INFTABLE_GORON_CITY_DOORS_UNLOCKED)) {
        Flags_SetInfTable(INFTABLE_GORON_CITY_DOORS_UNLOCKED);
    }
}

void RegisterGoronTunicFix() {
    COND_ID_HOOK(OnActorInit, ACTOR_BG_SPOT18_SHUTTER, !IS_RANDO && CVAR_GC_DOOR_FIX_VALUE, OnInitGCDoor);
}

static RegisterShipInitFunc initFunc(RegisterGoronTunicFix, { CVAR_GC_DOOR_FIX_NAME, "IS_RANDO" });