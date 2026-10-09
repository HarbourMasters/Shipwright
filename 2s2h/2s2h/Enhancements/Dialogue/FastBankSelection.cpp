#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Ginko_Man/z_en_ginko_man.h"
}

#define CVAR_NAME "gEnhancements.Dialogue.FastBankSelection"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static const char zeroRupees[3] = { '0', '0', '0' };

void FastBankSelection_UpdateMessage(const char rupeeValue[3]) {
    for (int i = 0; i <= 2; i++) {
        gPlayState->msgCtx.decodedBuffer.schar[gPlayState->msgCtx.unk120C0 + i] = rupeeValue[i];
        Font_LoadCharNES(gPlayState, gPlayState->msgCtx.decodedBuffer.schar[gPlayState->msgCtx.unk120C0 + i],
                         gPlayState->msgCtx.unk120C4 + (i << 7));
    }
    Audio_PlaySfx(NA_SE_SY_RUPY_COUNT);
}

void OnEnGinkoManUpdate(Actor* actor) {
    EnGinkoMan* enGinko = (EnGinkoMan*)actor;
    if (gPlayState->msgCtx.currentTextId == 0x450) {
        uint32_t depositAmount = MIN(gSaveContext.save.saveInfo.playerData.rupees, 999u);
        if (CHECK_BTN_ALL(gPlayState->state.input[0].cur.button, BTN_Z) &&
            gPlayState->msgCtx.rupeesSelected != (s32)depositAmount) {
            char firstChar = (depositAmount / 100) + '0';
            char secondChar = ((depositAmount / 10) % 10) + '0';
            char thirdChar = (depositAmount % 10) + '0';
            const char rupeeChar[3] = { firstChar, secondChar, thirdChar };

            FastBankSelection_UpdateMessage(rupeeChar);
        }
    } else if (gPlayState->msgCtx.currentTextId == 0x46E) {
        uint32_t currentRupees = gSaveContext.save.saveInfo.playerData.rupees;
        uint32_t maxWallet;
        if (CHECK_BTN_ALL(gPlayState->state.input[0].cur.button, BTN_Z)) {
            uint32_t walletCapacity = CUR_CAPACITY(UPG_WALLET);
            maxWallet = (walletCapacity > currentRupees) ? (walletCapacity - currentRupees) : 0;
            maxWallet = MIN(maxWallet, MIN(HS_GET_BANK_RUPEES(), 999u));

            if (maxWallet != 0 && gPlayState->msgCtx.rupeesSelected != maxWallet) {
                char firstChar = (maxWallet / 100) + '0';
                char secondChar = ((maxWallet / 10) % 10) + '0';
                char thirdChar = (maxWallet % 10) + '0';
                const char rupeeChar[3] = { firstChar, secondChar, thirdChar };

                FastBankSelection_UpdateMessage(rupeeChar);
            }
        }
    }
    if (gPlayState->msgCtx.currentTextId == 0x450 || gPlayState->msgCtx.currentTextId == 0x46E) {
        if (CHECK_BTN_ALL(gPlayState->state.input[0].cur.button, BTN_R) && gPlayState->msgCtx.rupeesSelected != 0) {
            FastBankSelection_UpdateMessage(zeroRupees);
        }
    }
}

void RegisterFastBankSelection() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_GINKO_MAN, CVAR, OnEnGinkoManUpdate);
}

static RegisterShipInitFunc initFunc(RegisterFastBankSelection, { CVAR_NAME });
