#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_En_Boom/z_en_boom.h"
}

#define CVAR_NAME "gEnhancements.PlayerActions.InstantRecall"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void ReturnBoomerang(Actor* actor) {
    EnBoom* boomerang = (EnBoom*)actor;

    // Kill the boomerang as long as it is not carrying an actor
    if (boomerang->unk_1C8 == NULL) {
        Actor_Kill(&boomerang->actor);
    }
}

void RegisterInstantRecall() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_BOOM, CVAR, [](Actor* actor) {
        if (CHECK_BTN_ALL(gPlayState->state.input->press.button, BTN_B)) {
            ReturnBoomerang(actor);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterInstantRecall, { CVAR_NAME });
