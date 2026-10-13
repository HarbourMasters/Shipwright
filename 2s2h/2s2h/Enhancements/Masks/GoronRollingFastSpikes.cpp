#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Masks.GoronRollingFastSpikes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

extern "C" {
#include "macros.h"
extern PlayState* gPlayState;
}

void RegisterGoronRollingFastSpikes() {
    COND_VB_SHOULD(VB_GORON_ROLL_INCREASE_SPIKE_LEVEL, CVAR, {
        Player* player = GET_PLAYER(gPlayState);

        // This is what the game keeps track of the state of how far you're off to spikes.
        // The vanilla if statement this SHOULD comes from will set it back to 4 if the player
        // doesn't have magic, so it doesn't conflict with that or "Goron Rolling Ignores Magic".
        //
        // Vanilla checks for number 54 for spike mode, but still having a slight wind-up looks
        // and feels much better.
        player->av1.actionVar1 += 8;
    });
}

static RegisterShipInitFunc initFunc(RegisterGoronRollingFastSpikes, { CVAR_NAME });
