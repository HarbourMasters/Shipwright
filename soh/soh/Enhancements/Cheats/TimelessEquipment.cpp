#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

#define CVAR_TIMELESS_EQUIPMENT_NAME CVAR_CHEAT("TimelessEquipment")

static void RegisterTimelessEquipment() {
    bool timelessEquipment = CVarGetInteger(CVAR_TIMELESS_EQUIPMENT_NAME, 0);

    COND_VB_SHOULD(VB_PLAYER_MEETS_AGE_REQ, timelessEquipment, { *should = true; });
    COND_VB_SHOULD(VB_ITEM_MEETS_AGE_REQ, timelessEquipment, { *should = true; });
    COND_VB_SHOULD(VB_SLOT_MEETS_AGE_REQ, timelessEquipment, { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterTimelessEquipment, { CVAR_TIMELESS_EQUIPMENT_NAME });
