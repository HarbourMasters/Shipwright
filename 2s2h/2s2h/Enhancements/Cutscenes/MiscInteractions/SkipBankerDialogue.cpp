#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Ginko_Man/z_en_ginko_man.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

int16_t choiceIndex = 0;

void RegisterSkipBankerDialogue() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_GINKO_MAN, CVAR, [](Actor* actor) {
        EnGinkoMan* refActor = (EnGinkoMan*)actor;

        // Set New Account
        refActor->isNewAccount = false;
        if (HS_GET_BANK_RUPEES() == 0) {
            HS_SET_BANK_RUPEES(1);
        }
    });

    COND_VB_SHOULD(VB_CONTINUE_BANKER_DIALOGUE, CVAR, {
        EnGinkoMan* enGinkoMan = va_arg(args, EnGinkoMan*);

        // Initial Banter
        if (enGinkoMan->curTextId == 0 || enGinkoMan->curTextId == 0x44c || enGinkoMan->curTextId == 0x457) {
            Message_StartTextbox(gPlayState, 0x466, &enGinkoMan->actor);
            enGinkoMan->curTextId = 0x466;
            *should = false;
            return;
        }

        if (enGinkoMan->curTextId == 0x468) {
            choiceIndex = gPlayState->msgCtx.choiceIndex;
            if (choiceIndex == GINKOMAN_CHOICE_DEPOSIT) {
                Message_StartTextbox(gPlayState, 0x450, &enGinkoMan->actor);
                enGinkoMan->curTextId = 0x450;
                *should = false;
                return;
            } else if (choiceIndex == GINKOMAN_CHOICE_WITHDRAWL) {
                Message_StartTextbox(gPlayState, 0x46e, &enGinkoMan->actor);
                enGinkoMan->curTextId = 0x46e;
                *should = false;
                return;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipBankerDialogue, { CVAR_NAME });
