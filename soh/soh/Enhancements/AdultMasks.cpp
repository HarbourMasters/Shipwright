#include "soh/Enhancements/AdultMasks.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/Enhancements/randomizer/SeedContext.h"

extern "C" {
#include "z64.h"
#include "macros.h"
#include "variables.h"
}

// rando's Masks as Adult setting forces the enhancement on, as logic may require it
static bool AdultMasksEnabled() {
    return CVarGetInteger(CVAR_ADULT_MASKS_NAME, 0) || (IS_RANDO && RAND_GET_OPTION(RSK_MASKS_AS_ADULT));
}

bool Ship_MasksEquippableAsAdult() {
    return AdultMasksEnabled() || CVarGetInteger(CVAR_CHEAT("TimelessEquipment"), 0);
}

static bool IsMask(int item) {
    return item >= ITEM_MASK_KEATON && item <= ITEM_MASK_TRUTH;
}

static void RegisterAdultMasks() {
    bool adultMasks = AdultMasksEnabled();

    // Masks drop their child-only age requirement
    COND_VB_SHOULD(VB_ITEM_MEETS_AGE_REQ, adultMasks, {
        if (IsMask(va_arg(args, int))) {
            *should = true;
        }
    });

    // Same for the child trade slot, but only while it holds a mask
    COND_VB_SHOULD(VB_SLOT_MEETS_AGE_REQ, adultMasks, {
        if (va_arg(args, int) == SLOT_TRADE_CHILD && IsMask(INV_CONTENT(ITEM_TRADE_CHILD))) {
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterAdultMasks, { "IS_RANDO", CVAR_ADULT_MASKS_NAME });
