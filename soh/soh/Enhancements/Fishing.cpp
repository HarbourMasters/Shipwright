#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include <variables.h>

f32 Fishing_GetMinimumRequiredScore();
}

void BuildFishingMessage(uint16_t* textId, bool* loadFromMessageTable) {
    if (gSaveContext.minigameScore == 0) {
        gSaveContext.minigameScore = Fishing_GetMinimumRequiredScore();
    }
}

void RegisterFishingMessages() {
    COND_ID_HOOK(OnOpenText, 0x40AE, CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFishing"), 0), BuildFishingMessage);
    COND_ID_HOOK(OnOpenText, 0x4080, CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFishing"), 0), BuildFishingMessage);
}

static RegisterShipInitFunc initFunc(RegisterFishingMessages, { CVAR_ENHANCEMENT("CustomizeFishing") });
