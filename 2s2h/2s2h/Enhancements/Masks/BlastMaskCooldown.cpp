#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Masks.BlastMaskCooldown"
#define CVAR CVarGetFloat(CVAR_NAME, 15.5f)

void RegisterBlastMaskCooldown() {
    COND_VB_SHOULD(VB_SET_BLAST_MASK_COOLDOWN_TIMER, CVAR < 15.5f, {
        *should = false;

        Player* player = GET_PLAYER(gPlayState);
        player->blastMaskTimer = CVAR * 20; // Convert seconds to frames
    });
}

static RegisterShipInitFunc initFunc(RegisterBlastMaskCooldown, { CVAR_NAME });
