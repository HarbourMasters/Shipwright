#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "functions.h"
}

#define FIDGET_SWORD_SWING 9
#define FIDGET_ADJUST_SHIELD 12

static constexpr int32_t CVAR_FIX_TWO_HANDED_IDLE_DEFAULT = 0;
#define CVAR_FIX_TWO_HANDED_IDLE_NAME CVAR_ENHANCEMENT("TwoHandedIdle")
#define CVAR_FIX_TWO_HANDED_IDLE_VALUE CVarGetInteger(CVAR_FIX_TWO_HANDED_IDLE_NAME, CVAR_FIX_TWO_HANDED_IDLE_DEFAULT)

// clang-format off
static RegisterShipInitFunc initFunc([]() {
    COND_VB_SHOULD(VB_SET_IDLE_ANIM, CVAR_FIX_TWO_HANDED_IDLE_VALUE, {
        Player* player = va_arg(args, Player*);
        s32 commonType = va_arg(args, s32);

        // Fixes a bug here where the condition for reaching two-handed idle animation was impossible. Original condition:
        /*
        (
            (
                (commonType + FIDGET_SWORD_SWING != FIDGET_SWORD_SWING) &&
                (commonType + FIDGET_SWORD_SWING != FIDGET_ADJUST_SHIELD)
            ) ||
            (
                (player->rightHandType == PLAYER_MODELTYPE_RH_SHIELD) &&
                (
                    (commonType + FIDGET_SWORD_SWING == FIDGET_ADJUST_SHIELD) ||
                    // This should not have been grouped here, because two handed melee weapons do not have shield.
                    (Player_GetMeleeWeaponHeld2(player) != 0)
                )
            )
        )
        */

        *should = (
            // Animation is not FIDGET_SWORD_SWING and FIDGET_ADJUST_SHIELD (So it's either FIDGET_ADJUST_TUNIC or FIDGET_TAP_FEET)
            (
                (commonType + FIDGET_SWORD_SWING != FIDGET_SWORD_SWING) &&
                (commonType + FIDGET_SWORD_SWING != FIDGET_ADJUST_SHIELD)
            ) ||
            // Animation is FIDGET_ADJUST_SHIELD and player is holding a shield in right hand
            (
                (player->rightHandType == PLAYER_MODELTYPE_RH_SHIELD) &&
                (commonType + FIDGET_SWORD_SWING == FIDGET_ADJUST_SHIELD)
            ) ||
            // Animation is FIDGET_SWORD_SWING and player is holding a melee weapon
            (
                (commonType + FIDGET_SWORD_SWING == FIDGET_SWORD_SWING) &&
                (Player_GetMeleeWeaponHeld(player) != 0)
            )
        );
    });
}, { CVAR_FIX_TWO_HANDED_IDLE_NAME });
// clang-format on
