#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Masks.EquipWhileSwimming"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static void RegisterEquipWhileSwimming() {
    COND_VB_SHOULD(VB_USE_ITEM_CONSIDER_ITEM_ACTION, CVAR, {
        PlayerItemAction itemAction = *va_arg(args, PlayerItemAction*);
        if (itemAction >= PLAYER_IA_MASK_MIN && itemAction < PLAYER_IA_MASK_GIANT) {
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_DISABLE_ITEM_UNDERWATER, CVAR, {
        s32 item = va_arg(args, s32);
        if (GET_PLAYER_FORM == PLAYER_FORM_HUMAN && item > ITEM_MASK_FIERCE_DEITY && item < ITEM_MASK_GIANT &&
            Player_GetEnvironmentalHazard(gPlayState) > PLAYER_ENV_HAZARD_UNDERWATER_FLOOR) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterEquipWhileSwimming, { CVAR_NAME });
