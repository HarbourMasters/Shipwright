#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
void Player_Action_93(Player* player, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Player.FastFlowerLaunch"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterFastFlowerLaunch() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, CVAR, [](Actor* actor) {
        Player* player = GET_PLAYER(gPlayState);
        if (player->actionFunc != Player_Action_93) {
            return;
        }

        DynaPolyActor* dyna;
        Input* input = &gPlayState->state.input[0];

        if (player->av1.actionVar1 != 0 && !(player->av1.actionVar1 == 1 && player->unk_B48 > -170.0f) &&
            player->av2.actionVar2 != 10 && !CHECK_BTN_ALL(input->cur.button, BTN_A)) {
            player->unk_ABC = -3900.0f;
            player->unk_B48 = -170.0f;
            player->av1.actionVar1 = 2;
            player->av2.actionVar2 = 10;
            player->actor.scale.y = 0.01f;
            dyna = DynaPoly_GetActor(&gPlayState->colCtx, player->actor.floorBgId);

            if (dyna != NULL) {
                player->actor.world.pos.x = dyna->actor.world.pos.x;
                player->actor.world.pos.z = dyna->actor.world.pos.z;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFastFlowerLaunch, { CVAR_NAME });
