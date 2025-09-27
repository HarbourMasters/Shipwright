#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_Bg_Bdan_Objects/z_bg_bdan_objects.h"
}

void BdanInit(void* actorRef) {
    Player* player = GET_PLAYER(gPlayState);
    BgBdanObjects* bgBdanObjects = static_cast<BgBdanObjects*>(actorRef);

    if (bgBdanObjects->dyna.actor.params == 1) {
        if (player->actor.world.pos.y < -500.0f) {
            bgBdanObjects->timer = 220;
        }
    }
}

/**
 * Adjusts the behavior of the elevator to start near the bottom if you are entering the room from the bottom
 */
void RegisterMoveJabuJabuElevator() {
    COND_ID_HOOK(OnActorInit, ACTOR_BG_BDAN_OBJECTS,
                 CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO), BdanInit);
}

static RegisterShipInitFunc initFunc(RegisterMoveJabuJabuElevator,
                                     { CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), "IS_RANDO" });
