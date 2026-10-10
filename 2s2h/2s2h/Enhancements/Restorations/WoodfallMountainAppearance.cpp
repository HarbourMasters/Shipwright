#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "z64save.h"
#include "overlays/actors/ovl_Bg_Breakwall/z_bg_breakwall.h"
}

#define CVAR_NAME "gEnhancements.Restorations.WoodfallMountainAppearance"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

typedef enum {
    /*  2 */ BGBREAKWALL_F_2 = 2, // Poisoned Woodfall Mountain
    /*  3 */ BGBREAKWALL_F_3 = 3, // Spring Woodfall Mountain
} BgBreakwallParamEx;

void RegisterWoodfallMountainAppearance() {
    COND_ID_HOOK(OnActorInit, ACTOR_BG_BREAKWALL, CVAR, [](Actor* actor) {
        if (BGBREAKWALL_GET_F(actor) == BGBREAKWALL_F_2 && CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)) {
            actor->params = (actor->params & 0xFFF0) | BGBREAKWALL_F_3;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterWoodfallMountainAppearance, { CVAR_NAME });
