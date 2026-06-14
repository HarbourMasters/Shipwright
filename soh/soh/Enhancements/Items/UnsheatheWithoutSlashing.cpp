#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

#define CVAR_UNSHEATHE_NAME CVAR_ENHANCEMENT("UnsheatheWithoutSlashing")
#define CVAR_UNSHEATHE_VALUE CVarGetInteger(CVAR_UNSHEATHE_NAME, 0)

void RegisterUnsheatheWithoutSlashing() {
    COND_VB_SHOULD(VB_USE_HELD_ITEM_AFTER_CHANGE, CVAR_UNSHEATHE_VALUE, {
        Player* player = va_arg(args, Player*);
        ItemID heldItemId = static_cast<ItemID>(player->heldItemId);
        if ((heldItemId == ITEM_SWORD_KOKIRI) || (heldItemId == ITEM_SWORD_MASTER)) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterUnsheatheWithoutSlashing, { CVAR_UNSHEATHE_NAME });
