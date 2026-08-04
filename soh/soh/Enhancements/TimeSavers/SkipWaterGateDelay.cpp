#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_Bg_Spot06_Objects/z_bg_spot06_objects.h"
extern SaveContext gSaveContext;
}

static void RegisterSpot06GateSkip() {
    COND_VB_SHOULD(VB_BG_SPOT06_OBJECTS_GATE_SKIP,
                   CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.OnePoint"), IS_RANDO), {
                       BgSpot06Objects* actor = va_arg(args, BgSpot06Objects*);
                       actor->timer = 0;
                       *should = false;
                   });
}

static RegisterShipInitFunc initFunc(RegisterSpot06GateSkip, { CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.OnePoint") });