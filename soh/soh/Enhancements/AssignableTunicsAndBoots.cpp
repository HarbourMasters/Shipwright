#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

// Most of the assignable tunics and boots logic has not been moved
// over to use hooks yet. It currently lives directly in z_player.c
// in Player_UseTunicBoots (where the actual logic lives), which is
// called by Player_UpdateCommon

#define CVAR_TUNICBOOTS_NAME CVAR_ENHANCEMENT("AssignableTunicsAndBoots")
#define CVAR_TUNICBOOTS_DEFAULT 0
#define CVAR_TUNICBOOTS_VALUE CVarGetInteger(CVAR_TUNICBOOTS_NAME, CVAR_TUNICBOOTS_DEFAULT)

void RegisterTunicBootsChangeUseHeldItem() {
    COND_VB_SHOULD(VB_CHANGE_HELD_ITEM_AND_USE_ITEM, CVAR_TUNICBOOTS_VALUE != CVAR_TUNICBOOTS_DEFAULT, {
        int32_t item = va_arg(args, int32_t);

        if (item >= ITEM_TUNIC_KOKIRI && item <= ITEM_BOOTS_HOVER) {
            *should = false;
            return;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterTunicBootsChangeUseHeldItem, { CVAR_TUNICBOOTS_NAME });
