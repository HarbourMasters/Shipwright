#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "functions.h"
extern PlayState* gPlayState;
}

#define CVAR_BOUNCE_OFF_WALLS_NAME CVAR_ENHANCEMENT("BounceOffWalls")
#define CVAR_BOUNCE_OFF_WALLS_VALUE CVarGetInteger(CVAR_BOUNCE_OFF_WALLS_NAME, 0)

static RegisterShipInitFunc initFunc(
    []() {
        COND_HOOK(OnPlayerUpdate, CVAR_BOUNCE_OFF_WALLS_VALUE, []() {
            Player* player = GET_PLAYER(gPlayState);
            if (player->actor.bgCheckFlags & 0x08 && ABS(player->linearVelocity) > 15.0f) {
                player->yaw = ((player->actor.wallYaw - player->yaw) + player->actor.wallYaw) - 0x8000;
                Player_PlaySfx(&player->actor, NA_SE_PL_BODY_HIT);
            }
        });
    },
    { CVAR_BOUNCE_OFF_WALLS_NAME });
