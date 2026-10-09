#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "variables.h"

#include "overlays/actors/ovl_En_Sellnuts/z_en_sellnuts.h"
#include "overlays/actors/ovl_En_Akindonuts/z_en_akindonuts.h"
void func_80ADC118(EnSellnuts* enSellnuts, PlayState* play);
void func_80BEF83C(EnAkindonuts* enAkindonuts, PlayState* play);
void CutsceneManager_End();
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipDekuSalesman() {
    // prevents him from doing his "fly in" animation
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_SELLNUTS, CVAR, [](Actor* actor, bool* should) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_73_04)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_73_04);
        }
    });

    // Kills him when he's about to pop out of the ground for his exit animation
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_SELLNUTS, CVAR, [](Actor* actor) {
        EnSellnuts* enSellnuts = (EnSellnuts*)actor;

        if (enSellnuts->actionFunc == func_80ADC118 && enSellnuts->unk_34A < 40) {
            CutsceneManager_End();
            Actor_Kill(&enSellnuts->actor);
        }
    });

    // Kills him when he's about to pop out of the ground for his exit animation
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_AKINDONUTS, CVAR, [](Actor* actor) {
        EnAkindonuts* enAkindonuts = (EnAkindonuts*)actor;

        if (enAkindonuts->actionFunc == func_80BEF83C && enAkindonuts->unk_33A < 40) {
            CutsceneManager_End();
            Actor_Kill(&enAkindonuts->actor);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipDekuSalesman, { CVAR_NAME });
