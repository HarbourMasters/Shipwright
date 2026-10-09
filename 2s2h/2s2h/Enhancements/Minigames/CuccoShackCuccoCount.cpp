#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Hs/z_en_hs.h"
#include "overlays/actors/ovl_En_Nwc/z_en_nwc.h"

void func_8095345C(EnHs* enHs, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Minigames.CuccoShackCuccoCount"
#define CVAR CVarGetInteger(CVAR_NAME, 10)

void RegisterCuccoShackCuccoCount() {
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_HS, CVAR != 10, [](Actor* actor, bool* should) {
        EnHs* enHs = (EnHs*)actor;

        if (enHs->actionFunc != func_8095345C) {
            return;
        }

        // As soon as enough have grown up, set the count to the max
        if (enHs->actor.home.rot.x >= (CVAR * 2) && enHs->actor.home.rot.x != 20) {
            enHs->actor.home.rot.x = 20;
        }

        // As soon as enough are following the player, set the count to the max
        if (enHs->actor.home.rot.z >= (CVAR * 2) && enHs->actor.home.rot.z != 20) {
            enHs->actor.home.rot.z = 20;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterCuccoShackCuccoCount, { CVAR_NAME });
