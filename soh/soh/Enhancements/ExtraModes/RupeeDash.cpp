#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
}

static constexpr int32_t CVAR_RUPEE_DASH_DEFAULT = 0;
#define CVAR_RUPEE_DASH_NAME CVAR_ENHANCEMENT("RupeeDash")
#define CVAR_RUPEE_DASH_VALUE CVarGetInteger(CVAR_RUPEE_DASH_NAME, CVAR_RUPEE_DASH_DEFAULT)

static constexpr int32_t CVAR_RUPEE_DASH_INTERVAL_DEFAULT = 5;
#define CVAR_RUPEE_DASH_INTERVAL_NAME CVAR_ENHANCEMENT("RupeeDashInterval")
#define CVAR_RUPEE_DASH_INTERVAL_TIME \
    CVarGetInteger(CVAR_RUPEE_DASH_INTERVAL_NAME, CVAR_RUPEE_DASH_INTERVAL_DEFAULT) * 20

void UpdateRupeeDash() {
    // Initialize Timer
    static uint16_t rupeeDashTimer = 0;

    // Did time change by DashInterval?
    if (rupeeDashTimer < CVAR_RUPEE_DASH_INTERVAL_TIME) {
        rupeeDashTimer++;
        return;
    }

    rupeeDashTimer = 0;
    if (gSaveContext.rupees > 0) {
        uint16_t walletSize = (CUR_UPG_VALUE(UPG_WALLET) + 1) * -1;
        Rupees_ChangeBy(walletSize);
    } else {
        Health_ChangeBy(gPlayState, -16);
    }
}

void RegisterRupeeDash() {
    COND_HOOK(OnPlayerUpdate, CVAR_RUPEE_DASH_VALUE, UpdateRupeeDash);
}

static RegisterShipInitFunc initFunc_RupeeDash(RegisterRupeeDash, { CVAR_RUPEE_DASH_NAME });
