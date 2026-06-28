#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include <variables.h>
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
f32 Fishing_GetMinimumRequiredScore();
}

void BuildFishingMessage(uint16_t* textId, bool* loadFromMessageTable) {
    if (gSaveContext.minigameScore == 0) {
        gSaveContext.minigameScore = static_cast<u16>(Fishing_GetMinimumRequiredScore());
    }
}

void RegisterFishingMessages() {
    COND_ID_HOOK(OnOpenText, 0x40AE, CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFishing"), 0), BuildFishingMessage);
    COND_ID_HOOK(OnOpenText, 0x4080, CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFishing"), 0), BuildFishingMessage);
}

// Vanilla bug: Not possible to fish with blank B because the blank B item value 0xFF gets saved
// as temp B, which also makes B button disabled - fishing pole gets unequipped.
// Fix: Disregard disabled B when fishing, and set used item to fishing pole on B press.
void RegisterAllowFishingBlankB() {
    COND_VB_SHOULD(VB_ALLOW_BLANK_B_FISHING_EQUIP, (IS_RANDO || CVarGetInteger(CVAR_ENHANCEMENT("FishingBlankB"), 0)), {
        if (gPlayState->interfaceCtx.unk_260 != 0 && gSaveContext.equips.buttonItems[0] == ITEM_FISHING_POLE) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_ALLOW_BLANK_B_FISHING_ITEM, (IS_RANDO || CVarGetInteger(CVAR_ENHANCEMENT("FishingBlankB"), 0)), {
        s32* i = va_arg(args, s32*);
        Player* player = va_arg(args, Player*);
        if (gPlayState->interfaceCtx.unk_260 != 0 && i == 0 && player->itemAction == PLAYER_IA_FISHING_POLE) {
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFishingMessages, { CVAR_ENHANCEMENT("CustomizeFishing") });
static RegisterShipInitFunc initAllowFishingBlankB(RegisterAllowFishingBlankB, { CVAR_ENHANCEMENT("FishingBlankB"), "IS_RANDO" });
