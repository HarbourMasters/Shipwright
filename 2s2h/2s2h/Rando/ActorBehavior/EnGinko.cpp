#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Ginko_Man/z_en_ginko_man.h"

void EnGinkoMan_SetupIdle(EnGinkoMan* enGinkoMan);
}

void Rando::ActorBehavior::InitEnGinkoBehavior() {
    COND_VB_SHOULD(VB_BANKER_GIVE_REWARD, IS_RANDO, {
        EnGinkoMan* enGinkoMan = va_arg(args, EnGinkoMan*);

        EnGinkoMan_SetupIdle(enGinkoMan);

        if (GameInteractor_Should(VB_PASS_FIRST_BANK_THRESHOLD,
                                  (HS_GET_BANK_RUPEES() >= 200) && (enGinkoMan->previousBankValue < 200), enGinkoMan)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_WALLET_UPGRADE);
        }

        if (GameInteractor_Should(VB_PASS_INTEREST_BANK_THRESHOLD,
                                  (HS_GET_BANK_RUPEES() >= 1000) && (enGinkoMan->previousBankValue < 1000),
                                  enGinkoMan) &&
            !RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_BANK_INTEREST].cycleObtained) {
            RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_WEST_BANK_INTEREST].eligible = true;
        }

        if (GameInteractor_Should(VB_PASS_SECOND_BANK_THRESHOLD, HS_GET_BANK_RUPEES() >= 5000, enGinkoMan)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_HEART_PIECE);
        }

        *should = false;
    });
}
