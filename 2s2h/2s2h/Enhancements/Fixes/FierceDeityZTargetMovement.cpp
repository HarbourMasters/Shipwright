#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Fixes.FierceDeityZTargetMovement"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterFierceDeityZTargetMovement() {
    COND_VB_SHOULD(VB_ZTARGET_SPEED_CHECK, CVAR, {
        Player* player = GET_PLAYER(gPlayState);
        float* speedArg = va_arg(args, float*);

        // If the player is Fierce Deity and targeting,
        if (player->focusActor != NULL && player->transformation == PLAYER_FORM_FIERCE_DEITY) {
            // 6.0f is the maximum speed of Zora/Goron/Deku link, whereas FD can be up to 10
            // Clamping to 6.0 keeps z target movement similar to other transformations
            *speedArg = CLAMP_MAX(*speedArg, 6.0f);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFierceDeityZTargetMovement, { CVAR_NAME });
