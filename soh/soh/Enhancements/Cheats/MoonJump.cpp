#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern PlayState* gPlayState;
#include "macros.h"
}

#define CVAR_MOON_JUMP_NAME CVAR_CHEAT("MoonJumpOnL")
#define CVAR_MOON_JUMP_DEFAULT 0
#define CVAR_MOON_JUMP_VALUE CVarGetInteger(CVAR_MOON_JUMP_NAME, CVAR_MOON_JUMP_DEFAULT)

void OnPlayerUpdateMoonJump() {
    Player* player = GET_PLAYER(gPlayState);

    if (player != nullptr && CHECK_BTN_ANY(gPlayState->state.input[0].cur.button, BTN_L)) {
        player->actor.velocity.y = 6.34375f;
    }
}

void RegisterMoonJump() {
    COND_HOOK(OnPlayerUpdate, CVAR_MOON_JUMP_VALUE, OnPlayerUpdateMoonJump);
}

static RegisterShipInitFunc initFunc(RegisterMoonJump, { CVAR_MOON_JUMP_NAME });
