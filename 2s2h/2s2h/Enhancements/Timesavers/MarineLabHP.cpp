#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Fish2/z_en_fish2.h"
}

#define CVAR_NAME "gEnhancements.Timesavers.MarineLabHP"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static f32 D_80B2B370[] = { 0.01f, 0.012f, 0.014f, 0.017f, 0.019f, 0.033f };

void RegisterMarineLabHP() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_FISH2, CVAR, [](Actor* actor) {
        EnFish2* marineFish = (EnFish2*)actor;

        // Setup Marine Fish to spawn Piece of Heart on next feeding.
        marineFish->unk_2C0 = 4;
        marineFish->unk_330 = D_80B2B370[marineFish->unk_2C0];
    });
}

static RegisterShipInitFunc initFunc(RegisterMarineLabHP, { CVAR_NAME });