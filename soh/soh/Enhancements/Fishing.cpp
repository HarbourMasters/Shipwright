#include <cmath>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "src/overlays/actors/ovl_Fishing/z_fishing.h"

extern "C" {
#include <variables.h>
#include <functions.h>
#include <macros.h>
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

// RANDOTODO: update the enhancement sliders to not allow
// values above rando fish weight values when rando'd
static float Fishing_GetMinimumRequiredScore() {
    int weight = LINK_IS_CHILD ? CVarGetInteger(CVAR_ENHANCEMENT("MinimumFishWeightChild"), 10)
                               : CVarGetInteger(CVAR_ENHANCEMENT("MinimumFishWeightAdult"), 13);
    return sqrtf((weight - 0.5f) / 0.0036f);
}

void BuildFishingMessage(uint16_t* textId, bool* loadFromMessageTable) {
    if (gSaveContext.minigameScore == 0) {
        gSaveContext.minigameScore = static_cast<u16>(Fishing_GetMinimumRequiredScore());
    }
}

void RegisterFishingMessages() {
    COND_ID_HOOK(OnOpenText, 0x40AE, CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFishing"), 0), BuildFishingMessage);
    COND_ID_HOOK(OnOpenText, 0x4080, CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFishing"), 0), BuildFishingMessage);

    COND_VB_SHOULD(
        VB_SHOULD_GIVE_VANILLA_FISHING_PRIZE, (!IS_RANDO && CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFishing"), 0)), {
            VBFishingData* fishData = va_arg(args, VBFishingData*);
            *should = fishData->fishWeight >= Fishing_GetMinimumRequiredScore() &&
                      !(HIGH_SCORE(HS_FISHING) & (LINK_IS_CHILD ? HS_FISH_PRIZE_CHILD : HS_FISH_PRIZE_ADULT));
        });
}

void RegisterCustomizeFishing() {
    bool customized = CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFishing"), 0);

    COND_VB_SHOULD(VB_FISHING_USE_DEFAULT_RECORD_LENGTH, customized, {
        f32* recordLength = va_arg(args, f32*);
        *recordLength = Fishing_GetMinimumRequiredScore();
        *should = false;
    });

    COND_VB_SHOULD(VB_FISHING_SPAWN_LOACHES, (customized && CVarGetInteger(CVAR_ENHANCEMENT("LoachesAlwaysAppear"), 0)),
                   { *should = true; });

    COND_VB_SHOULD(VB_FISHING_FISH_IS_LOACH, (customized && CVarGetInteger(CVAR_ENHANCEMENT("AllHyruleLoaches"), 0)),
                   { *should = true; });

    COND_VB_SHOULD(VB_FISHING_FISH_BITE, (customized && CVarGetInteger(CVAR_ENHANCEMENT("GuaranteeFishingBite"), 0)),
                   { *should = true; });

    COND_VB_SHOULD(VB_FISHING_FISH_ESCAPE, (customized && CVarGetInteger(CVAR_ENHANCEMENT("FishNeverEscape"), 0)),
                   { *should = false; });

    COND_VB_SHOULD(VB_FISHING_CATCH_FISH, (customized && CVarGetInteger(CVAR_ENHANCEMENT("InstantFishing"), 0)),
                   { *should = true; });

    COND_VB_SHOULD(VB_FISHING_INSTANT_CATCH, (customized && CVarGetInteger(CVAR_ENHANCEMENT("InstantFishing"), 0)),
                   { *should = true; });

    COND_VB_SHOULD(VB_FISHING_CONFIRM_KEEPING_SMALLER_FISH,
                   (customized && CVarGetInteger(CVAR_ENHANCEMENT("SkipKeepConfirmation"), 0)), { *should = false; });
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

static RegisterShipInitFunc initFunc(RegisterFishingMessages, { CVAR_ENHANCEMENT("CustomizeFishing"), "IS_RANDO" });
static RegisterShipInitFunc
    initCustomizeFishing(RegisterCustomizeFishing,
                         { CVAR_ENHANCEMENT("CustomizeFishing"), CVAR_ENHANCEMENT("LoachesAlwaysAppear"),
                           CVAR_ENHANCEMENT("AllHyruleLoaches"), CVAR_ENHANCEMENT("GuaranteeFishingBite"),
                           CVAR_ENHANCEMENT("FishNeverEscape"), CVAR_ENHANCEMENT("InstantFishing"),
                           CVAR_ENHANCEMENT("SkipKeepConfirmation") });
static RegisterShipInitFunc initHoverFishing(RegisterHoverFishing, { CVAR_ENHANCEMENT("HoverFishing") });
static RegisterShipInitFunc initAllowFishingBlankB(RegisterAllowFishingBlankB,
                                                   { CVAR_ENHANCEMENT("FishingBlankB"), "IS_RANDO" });
