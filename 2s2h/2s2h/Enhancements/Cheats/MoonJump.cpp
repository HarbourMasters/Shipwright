#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gCheats.MoonJumpOnL"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterMoonJump() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, CVAR, [](Actor* actor) {
        Player* player = GET_PLAYER(gPlayState);

        if (player != nullptr && CHECK_BTN_ANY(gPlayState->state.input[0].cur.button, BTN_L)) {
            player->actor.velocity.y = 6.34375f;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterMoonJump, { CVAR_NAME });
