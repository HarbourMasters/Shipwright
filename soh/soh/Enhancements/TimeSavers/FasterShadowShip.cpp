#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "src/overlays/actors/ovl_Bg_Haka_Ship/z_bg_haka_ship.h"
}

void RegisterFasterShadowShip() {
    COND_VB_SHOULD(VB_SHADOW_SHIP_SET_SPEED, CVarGetInteger(CVAR_ENHANCEMENT("FasterShadowShip"), 0), {
        BgHakaShip* ship = va_arg(args, BgHakaShip*);
        PlayState* play = va_arg(args, PlayState*);
        if (!Play_InCsMode(play)) {
            if (Actor_FindNearby(play, &ship->dyna.actor, ACTOR_EN_TEST, ACTORCAT_ENEMY, 800.0) == NULL &&
                Actor_FindNearby(play, &ship->dyna.actor, ACTOR_PLAYER, ACTORCAT_PLAYER, 450.0) != NULL) {
                *should = false;
                ship->dyna.actor.speedXZ = 25.0f;
            } else if (ship->dyna.actor.speedXZ > 4.0f) {
                *should = false;
                ship->dyna.actor.speedXZ = 4.0f;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFasterShadowShip, { CVAR_ENHANCEMENT("FasterShadowShip") });
