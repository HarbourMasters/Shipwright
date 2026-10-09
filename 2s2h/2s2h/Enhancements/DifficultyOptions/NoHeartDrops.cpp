#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.DifficultyOptions.NoHeartDrops"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterNoHeartDrops() {
    // Disable spawning of hearts and fairies
    COND_VB_SHOULD(VB_DROP_COLLECTIBLE, CVAR, {
        Vec3f unused = va_arg(args, Vec3f);
        u32 item = va_arg(args, u32) & 0xFF;
        if (item == ITEM00_RECOVERY_HEART || item == ITEM00_3_HEARTS || item == ITEM00_FLEXIBLE) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterNoHeartDrops, { CVAR_NAME });
