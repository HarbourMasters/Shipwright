#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Bomjimb/z_en_bomjimb.h"
void func_80C02CA4(EnBomjimb* thisx, PlayState* play);
void func_80C02A14(EnBomjimb* thisx, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Minigames.BombersHideAndSeek"
#define CVAR CVarGetInteger(CVAR_NAME, 5)

void RegisterBombersHideAndSeek() {
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_BOMJIMB, CVAR < 5, [](Actor* actor, bool* should) {
        EnBomjimb* bomberKids = (EnBomjimb*)actor;

        if (bomberKids->actionFunc != func_80C02A14) {
            return;
        }

        if (gSaveContext.save.saveInfo.bombersCaughtNum >= CVAR) {
            SET_WEEKEVENTREG(WEEKEVENTREG_76_01);
            SET_WEEKEVENTREG(WEEKEVENTREG_76_02);
            SET_WEEKEVENTREG(WEEKEVENTREG_76_04);
            SET_WEEKEVENTREG(WEEKEVENTREG_76_08);
            SET_WEEKEVENTREG(WEEKEVENTREG_76_10);
            func_80C02CA4(bomberKids, gPlayState);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterBombersHideAndSeek, { CVAR_NAME });
