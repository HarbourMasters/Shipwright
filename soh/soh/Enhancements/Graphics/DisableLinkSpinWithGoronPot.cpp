#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "src/overlays/actors/ovl_Bg_Spot18_Basket/z_bg_spot18_basket.h"
extern PlayState* gPlayState;
}

static constexpr int32_t CVAR_LINK_SPIN_DISABLE_DEFAULT = 0;
#define CVAR_LINK_SPIN_DISABLE_NAME CVAR_ENHANCEMENT("DisableLinkSpinWithGoronPot")
#define CVAR_LINK_SPIN_DISABLE_VALUE CVarGetInteger(CVAR_LINK_SPIN_DISABLE_NAME, CVAR_LINK_SPIN_DISABLE_DEFAULT)

static void MakeLinkFocusOnPot() {
    BgSpot18Basket* bgSpot18 =
        (BgSpot18Basket*)Actor_Find(&gPlayState->actorCtx, ACTOR_BG_SPOT18_BASKET, ACTORCAT_PROP);
    if (bgSpot18 != NULL) {
        func_8002DF38(gPlayState, &bgSpot18->dyna.actor, 1);
    }
}

static void RegisterDisableLinkSpinWithGoronPot() {
    COND_VB_SHOULD(VB_LINK_SPIN_WITH_GORON_POT, CVAR_LINK_SPIN_DISABLE_VALUE, {
        MakeLinkFocusOnPot();
        *should = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterDisableLinkSpinWithGoronPot, { CVAR_LINK_SPIN_DISABLE_NAME });
