#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/OTRGlobals.h"

extern "C" {
#include "src/overlays/actors/ovl_Bg_Bdan_Objects/z_bg_bdan_objects.h"
}

/**
 * Adjusts the behavior of the elevator to start near the bottom if you are entering the room from the bottom
 */
void MoveJabuJabuElevator_Register() {
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorInit>(
        ACTOR_BG_BDAN_OBJECTS, [](void* actorRef) {
            Player* player = GET_PLAYER(gPlayState);
            BgBdanObjects* bgBdanObjects = static_cast<BgBdanObjects*>(actorRef);

            if (!CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO)) {
                return;
            }

            if (bgBdanObjects->dyna.actor.params == 1) {
                if (player->actor.world.pos.y < -500.0f) {
                    bgBdanObjects->timer = 220;
                }
            }
        });
}
