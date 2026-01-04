#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "global.h"
#include "functions.h"
}

enum FidgetTypeOverride {
    FIDGET_LOOK_AROUND = 0,
    FIDGET_COLD,
    FIDGET_WARM,
    FIDGET_HOT,
    FIDGET_STRETCH_1,
    FIDGET_STRETCH_2,
    FIDGET_STRETCH_3,
    FIDGET_CRIT_HEALTH_START,
    FIDGET_CRIT_HEALTH_LOOP,
    FIDGET_SWORD_SWING,
    FIDGET_ADJUST_TUNIC,
    FIDGET_TAP_FEET,
    FIDGET_ADJUST_SHIELD,
    FIDGET_SWORD_SWING_TWO_HAND,
};

extern "C" PlayState* gPlayState;

static void RegisterFidgetIdleOverrides() {
    REGISTER_VB_SHOULD(VB_PLAYER_PICK_FIDGET, {
        Player* player = va_arg(args, Player*);
        s32* fidgetType = va_arg(args, s32*);

        if ((player == NULL) || (fidgetType == NULL)) {
            return;
        }

        if (!CVarGetInteger(CVAR_ENHANCEMENT("TwoHandedIdle"), 0)) {
            return;
        }

        PlayState* play = gPlayState;

        if (*fidgetType == FIDGET_SWORD_SWING) {
            if (Player_HoldsTwoHandedWeapon(player)) {
                *fidgetType = FIDGET_SWORD_SWING_TWO_HAND;
            }
        } else if (*fidgetType == FIDGET_ADJUST_SHIELD) {
            if ((player->currentShield == PLAYER_SHIELD_NONE) || Player_HoldsTwoHandedWeapon(player)) {
                if (play != NULL) {
                    *fidgetType = play->roomCtx.curRoom.behaviorType2;
                } else {
                    *fidgetType = FIDGET_LOOK_AROUND;
                }
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFidgetIdleOverrides, { CVAR_ENHANCEMENT("TwoHandedIdle") });
