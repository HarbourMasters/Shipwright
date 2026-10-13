#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
void Player_Action_Idle(Player* player, PlayState* play);
void Player_Action_13(Player* player, PlayState* play);
void Player_Action_26(Player* player, PlayState* play);
}

typedef enum {
    DASH_AFTER_ROLL_OFF,
    DASH_AFTER_ROLL_ON,
    DASH_AFTER_ROLL_STACK,
} DashAfterRollMode;

#define CVAR_NAME "gEnhancements.Player.DashAfterRoll"
#define CVAR CVarGetInteger(CVAR_NAME, DASH_AFTER_ROLL_OFF)

static bool isDashing = false;
static PlayerActionFunc prevActionFunc = NULL;

static RegisterShipInitFunc initFunc(
    []() {
        isDashing = false;

        COND_HOOK(OnPassPlayerInputs, CVAR, [](Input* input) {
            Player* player = GET_PLAYER(gPlayState);
            // For some reason you go back into "idle" for one frame after a roll even if you keep running. Ignore that
            // frame for the dash
            bool rollJustEnded = prevActionFunc == Player_Action_26 && player->actionFunc == Player_Action_Idle;

            if (!CHECK_BTN_ALL(input->cur.button, BTN_A)) {
                isDashing = false;
            } else if (player->actionFunc == Player_Action_26) {
                // Player_Action_26 is roll but if you bonk it sets actionVar2, only start dash if it's 0
                isDashing = player->av2.actionVar2 == 0;
            } else if (player->actionFunc != Player_Action_13 && !rollJustEnded) {
                isDashing = false;
            }
            prevActionFunc = player->actionFunc;
        });

        COND_VB_SHOULD(VB_CONSIDER_BUNNY_HOOD_EQUIPPED, CVAR == DASH_AFTER_ROLL_ON, {
            if (isDashing) {
                *should = true;
            }
        });

        COND_VB_SHOULD(VB_SPEED_MODIFIER_WALK, CVAR == DASH_AFTER_ROLL_STACK, {
            f32* speedTarget = va_arg(args, f32*);

            if (isDashing) {
                *speedTarget *= 1.5f;
            }
        });
    },
    { CVAR_NAME });
