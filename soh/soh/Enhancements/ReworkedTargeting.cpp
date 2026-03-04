#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/OTRGlobals.h"

extern "C" {
#include "z64.h"
#include "macros.h"
#include "functions.h"
extern PlayState* gPlayState;
}

#define CVAR_REWORKED_TARGETING_NAME CVAR_ENHANCEMENT("ReworkedTargeting.Enabled")
#define CVAR_REWORKED_TARGETING_VALUE CVarGetInteger(CVAR_REWORKED_TARGETING_NAME, 0)

static bool sTriggeredByButtonCombo = false;

void RegisterReworkedTargeting() {

    COND_VB_SHOULD(VB_TOGGLE_Z_TARGET_SWITCH_TARGETS, CVAR_REWORKED_TARGETING_VALUE, {
        Player* player = GET_PLAYER(gPlayState);
        if (player->focusActor != NULL && !sTriggeredByButtonCombo) {
            *should = false;
        }
        sTriggeredByButtonCombo = false;
    });

    COND_VB_SHOULD(VB_TOGGLE_Z_TARGET_SWITCH_DIRECTION, CVAR_REWORKED_TARGETING_VALUE, {
        if (*should)
            return;

        Player* player = GET_PLAYER(gPlayState);

        if (player->focusActor != NULL) {
            Input* input = &gPlayState->state.input[0];

            static bool wasButtonComboActive = false;

            const s32 targetSwitchMask = CVarGetInteger(CVAR_ENHANCEMENT("ReworkedTargeting.Btn"), 0);
            bool isButtonComboActive = CHECK_BTN_ANY(input->press.button, targetSwitchMask);

            if (isButtonComboActive && !wasButtonComboActive) {
                Actor* nextTarget = gPlayState->actorCtx.targetCtx.unk_94;
                if (nextTarget != NULL) {
                    sTriggeredByButtonCombo = true;
                    *should = true;
                } else {
                    *should = false;
                }
            }

            wasButtonComboActive = isButtonComboActive;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterReworkedTargeting, { CVAR_REWORKED_TARGETING_NAME });
