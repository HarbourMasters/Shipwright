#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include <variables.h>
#include <functions.h>
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

void RegisterHoverFishing() {
    COND_VB_SHOULD(VB_NOT_CAST_FISHING, (CVarGetInteger(CVAR_ENHANCEMENT("HoverFishing"), false)), {
        Vec3f* rodCheckPos = va_arg(args, Vec3f*);
        *should = false;
        // Run only original NTSC 1.0 check before cast
        if (BgCheck_SphVsFirstPoly(&gPlayState->colCtx, rodCheckPos, 20.0f)) {
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_FISHING_ZERO_XZ, (CVarGetInteger(CVAR_ENHANCEMENT("HoverFishing"), false)), {
        *should = false; // NTSC 1.0
    });
}

// Vanilla bug: Not possible to fish with blank B because blank B item value 0xFF is saved
// as temp B = disabled B -> fishing pole is unequipped.
// Fix: If fishing, disregard disabled B and on B press set used item to fishing pole.
void RegisterAllowFishingBlankB() {
    COND_VB_SHOULD(VB_PUTAWAY_BECAUSE_DISABLED_ITEM_BUTTONS,
                   (IS_RANDO || CVarGetInteger(CVAR_ENHANCEMENT("FishingBlankB"), IS_RANDO)), {
                       if (gPlayState->interfaceCtx.unk_260 != 0 &&
                           gSaveContext.equips.buttonItems[0] == ITEM_FISHING_POLE) {
                           *should = false;
                       }
                   });

    COND_VB_SHOULD(
        VB_OVERRIDE_BUTTON_ITEM_USED, (IS_RANDO || CVarGetInteger(CVAR_ENHANCEMENT("FishingBlankB"), IS_RANDO)), {
            s32* i = va_arg(args, s32*);
            Player* player = va_arg(args, Player*);
            s32* item = va_arg(args, s32*);
            if (gPlayState->interfaceCtx.unk_260 != 0 && *i == 0 && player->itemAction == PLAYER_IA_FISHING_POLE) {
                *item = ITEM_FISHING_POLE;
            }
        });
}

static RegisterShipInitFunc initFunc(RegisterFishingMessages, { CVAR_ENHANCEMENT("CustomizeFishing") });
static RegisterShipInitFunc initHoverFishing(RegisterHoverFishing, { CVAR_ENHANCEMENT("HoverFishing") });
static RegisterShipInitFunc initAllowFishingBlankB(RegisterAllowFishingBlankB,
                                                   { CVAR_ENHANCEMENT("FishingBlankB"), "IS_RANDO" });
