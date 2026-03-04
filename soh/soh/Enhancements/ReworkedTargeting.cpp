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

#define CVAR_REWORKED_TARGETING_NAME CVAR_ENHANCEMENT("ReworkedTargeting")
#define CVAR_REWORKED_TARGETING_VALUE CVarGetInteger(CVAR_REWORKED_TARGETING_NAME, 0)

#define RIGHT_STICK_THRESHOLD 20

static bool sTriggeredByRightStick = false;

void RegisterReworkedTargeting() {

    COND_VB_SHOULD(VB_TOGGLE_Z_TARGET_SWITCH_TARGETS, CVAR_REWORKED_TARGETING_VALUE, {
        Player* player = GET_PLAYER(gPlayState);
        if (player->focusActor != NULL && !sTriggeredByRightStick) {
            *should = false;
        }
        sTriggeredByRightStick = false;
    });

    COND_VB_SHOULD(VB_TOGGLE_Z_TARGET_SWITCH_DIRECTION, CVAR_REWORKED_TARGETING_VALUE, {
        if (*should)
            return;

        Player* player = GET_PLAYER(gPlayState);

        if (player->focusActor != NULL) {
            Input* input = &gPlayState->state.input[0];

            static bool wasRightStickActive = false;

            s8 rightStickX = input->cur.right_stick_x;
            s8 rightStickY = input->cur.right_stick_y;

            bool isRightStickActive = (rightStickX > RIGHT_STICK_THRESHOLD || rightStickX < -RIGHT_STICK_THRESHOLD ||
                                       rightStickY > RIGHT_STICK_THRESHOLD || rightStickY < -RIGHT_STICK_THRESHOLD);

            if (isRightStickActive && !wasRightStickActive) {
                Actor* nextTarget = gPlayState->actorCtx.targetCtx.unk_94;
                if (nextTarget != NULL) {
                    sTriggeredByRightStick = true;
                    *should = true;
                } else {
                    *should = false;
                }
            }

            wasRightStickActive = isRightStickActive;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterReworkedTargeting, { CVAR_REWORKED_TARGETING_NAME });
