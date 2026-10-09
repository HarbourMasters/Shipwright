#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
extern Input* sPlayerControlInput;
}

#define CVAR_NAME "gEnhancements.Equipment.GreatFairySwordBButton"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterGreatFairySwordBButton() {
    COND_VB_SHOULD(VB_GET_ITEM_ON_BUTTON, CVAR, {
        Player* player = GET_PLAYER(gPlayState);
        EquipSlot slot = (EquipSlot)va_arg(args, int);
        ItemId* item = va_arg(args, ItemId*);

        if (slot == EQUIP_SLOT_B && player->transformation == PLAYER_FORM_HUMAN &&
            player->heldItemId == ITEM_SWORD_GREAT_FAIRY) {
            *item = ITEM_SWORD_GREAT_FAIRY;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterGreatFairySwordBButton, { CVAR_NAME });
