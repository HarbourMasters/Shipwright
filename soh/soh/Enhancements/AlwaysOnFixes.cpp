#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

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

// `Player_Action_DismountLadder`: Dismounting a ladder in a cutscene or when using
// a cutscene item (possible using restricted items glitch), i.e. `player->unk_6AD` is 3 or 4,
// causes the animation in `Player_Action_DismountLadder` to never finish because
// `Player_TryActionInterrupt` always returns true and player cannot exit the action.
// Fix: Disregard result of `Player_TryActionInterrupt` if `unk_6AD` >= 3.
void RegisterFixLadderCutsceneSoftlock() {
    COND_VB_SHOULD(VB_LADDER_CUTSCENE_FLAG, true, {
        u8* unk_6AD = va_arg(args, u8*);
        if (*unk_6AD >= 3) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFuncFixOutsideTotCrash(RegisterFixOutsideTotCrash, { "" });
static RegisterShipInitFunc initFuncFixLadderCutsceneSoftlock(RegisterFixLadderCutsceneSoftlock, { "" });
