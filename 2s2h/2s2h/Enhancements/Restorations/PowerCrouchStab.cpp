#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Restorations.PowerCrouchStab"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterPowerCrouchStab() {
    COND_VB_SHOULD(VB_PATCH_POWER_CROUCH_STAB, CVAR, { *should = false; });

    COND_ID_HOOK(OnActorInit, ACTOR_PLAYER, CVAR == 2, [](Actor* actor) {
        Player* player = (Player*)actor;
        player->meleeWeaponQuads[0].elem.atDmgInfo.dmgFlags = 512; // Kokiri Sword
        player->meleeWeaponQuads[1].elem.atDmgInfo.dmgFlags = 512; // Kokiri Sword
    });
}

static RegisterShipInitFunc initFunc(RegisterPowerCrouchStab, { CVAR_NAME });
