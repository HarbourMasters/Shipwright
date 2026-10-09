#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.AutoAdvanceEndingText"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static bool sAdvanceMessages = false;
static u32 sTextboxTimer = 0;

static bool IsWaitingOnPlayer(MessageContext* msgCtx) {
    return (msgCtx->msgLength != 0) &&
           ((msgCtx->msgMode == MSGMODE_TEXT_AWAIT_INPUT) || (msgCtx->msgMode == MSGMODE_TEXT_AWAIT_NEXT) ||
            (msgCtx->msgMode == MSGMODE_TEXT_DONE));
}

static RegisterShipInitFunc initFunc(
    []() {
        // Unconditionally registering these in case the user forgets to turn this on prior to finishing
        COND_HOOK(OnGameCompletion, true, []() { sAdvanceMessages = true; });
        COND_HOOK(OnSaveLoad, true, [](s16 fileNum) { sAdvanceMessages = false; });

        COND_HOOK(OnGameStateUpdate, CVAR, []() {
            if (sAdvanceMessages && (gPlayState != NULL) && IsWaitingOnPlayer(&gPlayState->msgCtx)) {
                sTextboxTimer++;
            } else {
                sTextboxTimer = 0;
            }
        });

        COND_VB_SHOULD(VB_MSG_ADVANCE, CVAR, {
            if (sAdvanceMessages && (gPlayState != NULL) && (sTextboxTimer >= 20)) {
                sTextboxTimer = 0;
                *should = true;
            }
        });
    },
    { CVAR_NAME });
