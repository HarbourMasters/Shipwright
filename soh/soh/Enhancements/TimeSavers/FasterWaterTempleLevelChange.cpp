#include <libultraship/bridge/consolevariablebridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

extern "C" {
#include "z64save.h"
extern SaveContext gSaveContext;
}

static void RegisterFasterWaterTempleLevelChange() {
    COND_VB_SHOULD(VB_MODIFY_WATER_TEMPLE_WATER_LEVEL_SPEED,
                   IS_RANDO || CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.OnePoint"), 0), {
                       f32* speed = va_arg(args, f32*);
                       *speed = 20.0f;
                   });
}

static RegisterShipInitFunc initFunc(RegisterFasterWaterTempleLevelChange,
                                     { CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.OnePoint"), "IS_RANDO" });
