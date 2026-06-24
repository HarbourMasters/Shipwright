#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
}

// Dying or using Din's Fire in the Outside Temple of Time area crashes the game.
// In vanilla this can never happen, but with CrowdControl, Sail, Unrestricted Items
// and others this *can* happen. Because it checks for a camId of -1, this code path
// shouldn't ever influence vanilla play regardless.
void RegisterFixOutsideTotCrash() {
    COND_VB_SHOULD(VB_SHOULD_LOAD_BG_IMAGE, true, {
        int32_t* camId = va_arg(args, int*);
        if (*camId == -1) {
            *should = false;
        }
    });
}

// Vanilla bug: `Actor_Item_Shield` (dropped Deku Shield when burning) assumes that segment 12
// contains Link display list `gCullBackDList`. If an actor is drawn between player and shield
// that uses segment 12 (such as Jabu-Jabu tentacles), the game will crash on Deku Shield drop.
// Fix: Re-set segment 12 to the required display list.
void RegisterFixDekuShieldDropCrash() {
    COND_VB_SHOULD(VB_ITEMSHIELD_DRAW, true, {
        GraphicsContext* __gfxCtx = gPlayState->state.gfxCtx;
        gSPSegment(POLY_OPA_DISP++, 0x0C, (uintptr_t)SEGMENTED_TO_VIRTUAL(gCullBackDList));
    });
}

static RegisterShipInitFunc initFuncFixOutsideTotCrash(RegisterFixOutsideTotCrash, { "" });
static RegisterShipInitFunc initFunxFixDekuShieldDropCrash(RegisterFixDekuShieldDropCrash, { "" });
