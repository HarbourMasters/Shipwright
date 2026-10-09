#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Dialogue.AutoBombersCode"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterAutoBombersCode() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_BOMBERS2, CVAR, [](Actor* actor) {
        if (gPlayState->msgCtx.currentTextId == 0x0729 && CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
            for (int i = 0; i < 5; i++) {
                u8 digit = gSaveContext.save.saveInfo.bomberCode[i];
                gPlayState->msgCtx.unk12054[i] = digit;
                Font_LoadCharNES(gPlayState, '0' + digit, gPlayState->msgCtx.unk120C4 + (i << 7));
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterAutoBombersCode, { CVAR_NAME });
