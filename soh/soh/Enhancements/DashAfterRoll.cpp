#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/Enhancements/BunnyHood.h"
#include "soh/Enhancements/DashAfterRoll.h"

extern "C" {
#include "z64.h"
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
void Player_Action_Roll(Player* player, PlayState* play);
void Player_Action_Idle(Player* player, PlayState* play);
void Player_Action_80842180(Player* player, PlayState* play);
}

static bool sIsDashing = false;
static PlayerActionFunc sPrevActionFunc = NULL;

static DashAfterRollMode Ship_GetDashAfterRollMode() {
    return (DashAfterRollMode)CVarGetInteger(CVAR_DASH_AFTER_ROLL_NAME, DASH_AFTER_ROLL_OFF);
}

float Ship_GetDashRunFactor(Player* player) {
    DashAfterRollMode mode = Ship_GetDashAfterRollMode();

    if (!sIsDashing || mode == DASH_AFTER_ROLL_OFF) {
        return 1.0f;
    }

    // "On" tops the dash out at Bunny Hood speed, so the hood adds nothing while dashing
    if (mode == DASH_AFTER_ROLL_ON && Ship_GetBunnyHoodRunFactor(player) != 1.0f) {
        return 1.0f;
    }

    return 1.5f;
}

// Frames the dash should take to reach its target speed, whichever mode set it.
#define DASH_RAMP_FRAMES 5.0f

float Ship_GetDashAccel(Player* player, float speedTarget) {
    if (Ship_GetDashRunFactor(player) == 1.0f) {
        return 0.0f;
    }

    float remaining = speedTarget - player->linearVelocity;

    return remaining > 0.0f ? remaining / DASH_RAMP_FRAMES : 0.0f;
}

// Runs at the end of Player_UpdateCommon, so the flag set here reaches the speed
// pipeline on the following frame.
static void Ship_UpdateDashState() {
    Player* player = GET_PLAYER(gPlayState);

    // A roll exits through func_80839FFC, which parks the player in idle for a frame
    // before the run action picks the stick back up.
    bool rollJustEnded = sPrevActionFunc == Player_Action_Roll && player->actionFunc == Player_Action_Idle;

    if (!CHECK_BTN_ALL(gPlayState->state.input[0].cur.button, BTN_A)) {
        sIsDashing = false;
    } else if (player->actionFunc == Player_Action_Roll) {
        // Bonking ends the roll early and drops the momentum the dash carries
        sIsDashing = player->av2.bonked == 0;
    } else if (player->actionFunc != Player_Action_80842180 && !rollJustEnded) {
        sIsDashing = false;
    }

    sPrevActionFunc = player->actionFunc;
}

static void RegisterDashAfterRoll() {
    sIsDashing = false;
    sPrevActionFunc = NULL;

    COND_HOOK(OnPlayerUpdate, Ship_GetDashAfterRollMode() != DASH_AFTER_ROLL_OFF, []() { Ship_UpdateDashState(); });
}

static RegisterShipInitFunc initFunc(RegisterDashAfterRoll, { CVAR_DASH_AFTER_ROLL_NAME });
