#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
void Player_Action_57(Player* player, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Restorations.OoTFasterSwim"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterOoTFasterSwim() {
    COND_VB_SHOULD(VB_CLAMP_ANIMATION_SPEED, CVAR, {
        Player* player = GET_PLAYER(gPlayState);
        f32* animationSpeed = va_arg(args, f32*);

        if (player->actionFunc == Player_Action_57 && player->transformation == PLAYER_FORM_HUMAN) {
            *should = false;
            if (*animationSpeed < 1.0f) {
                *animationSpeed = 1.0f;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterOoTFasterSwim, { CVAR_NAME });
