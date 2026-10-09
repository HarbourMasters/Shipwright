#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"

#include "overlays/actors/ovl_En_Sellnuts/z_en_sellnuts.h"

void func_80ADBBEC(EnSellnuts* enSellnuts, PlayState* play);
void func_80ADB0D8(EnSellnuts* enSellnuts, PlayState* play);
}

u16 D_80ADD930_copy[] = { 0x0619, 0x0613, 0x0613 };

void Rando::ActorBehavior::InitEnSellnutsBehavior() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_SELLNUTS, IS_RANDO, [](Actor* actor) {
        EnSellnuts* enSellnuts = (EnSellnuts*)actor;

        if (enSellnuts->actionFunc == func_80ADBBEC) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_LAND_TITLE_DEED);
            enSellnuts->actor.parent = NULL;
            enSellnuts->actionFunc = func_80ADB0D8;
            enSellnuts->unk_340 = D_80ADD930_copy[enSellnuts->unk_33A];
            Message_StartTextbox(gPlayState, enSellnuts->unk_340, &enSellnuts->actor);
        }
    });
}
