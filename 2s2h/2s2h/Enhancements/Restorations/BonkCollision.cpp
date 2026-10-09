#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Restorations.BonkCollision"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterBonkCollision() {
    COND_VB_SHOULD(VB_SET_PLAYER_CYLINDER_OC_FLAGS, CVAR, {
        Player* player = va_arg(args, Player*);
        u32 dmgFlags = va_arg(args, u32);
        if (dmgFlags == DMG_NORMAL_ROLL) {
            // OR the new flags instead of directly assigning them
            player->cylinder.base.ocFlags1 |= OC1_ON | OC1_TYPE_ALL;
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterBonkCollision, { CVAR_NAME });
