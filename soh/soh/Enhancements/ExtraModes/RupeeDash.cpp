#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
#include "functions.h"
#include "macros.h"
#include "variables.h"
}

#define CVAR_RUPEE_DASH_NAME CVAR_ENHANCEMENT("RupeeDash")
#define CVAR_RUPEE_DASH_DEFAULT 0
#define CVAR_RUPEE_DASH_VALUE CVarGetInteger(CVAR_RUPEE_DASH_NAME, CVAR_RUPEE_DASH_DEFAULT)

#define CVAR_RUPEE_DASH_INTERVAL_NAME CVAR_ENHANCEMENT("RupeeDashInterval")
#define CVAR_RUPEE_DASH_INTERVAL_DEFAULT 5
#define CVAR_RUPEE_DASH_INTERVAL_VALUE CVarGetInteger(CVAR_RUPEE_DASH_INTERVAL_NAME, CVAR_RUPEE_DASH_INTERVAL_DEFAULT)

void OnPlayerUpdateRupeeDash() {
    // Initialize Timer
    static u16 rupeeDashTimer = 0;

    // Did time change by DashInterval?
    if (rupeeDashTimer >= CVAR_RUPEE_DASH_INTERVAL_VALUE * 20) {
        rupeeDashTimer = 0;
        if (gSaveContext.rupees > 0) {
            u16 walletSize = (CUR_UPG_VALUE(UPG_WALLET) + 1) * -1;
            Rupees_ChangeBy(walletSize);
        } else {
            Health_ChangeBy(gPlayState, -16);
        }
    } else {
        rupeeDashTimer++;
    }
}

void RegisterRupeeDash() {
    COND_HOOK(OnPlayerUpdate, CVAR_RUPEE_DASH_VALUE, OnPlayerUpdateRupeeDash);
}

static RegisterShipInitFunc initFunc(RegisterRupeeDash, { CVAR_RUPEE_DASH_NAME });
