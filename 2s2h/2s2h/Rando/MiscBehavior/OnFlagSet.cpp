#include "MiscBehavior.h"

extern "C" {
#include "variables.h"
}

void Rando::MiscBehavior::OnFlagSet(FlagType flagType, u32 flag) {
    auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(flagType, flag);
    if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
        return;
    }

    auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];
    if (randoSaveCheck.shuffled) {
        randoSaveCheck.eligible = true;
    }
}

void Rando::MiscBehavior::OnSceneFlagSet(s16 sceneId, FlagType flagType, u32 flag) {
    auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(flagType, flag, sceneId);
    if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
        return;
    }

    // Pots handle their own items, ignore them
    if (randoStaticCheck.randoCheckType == RCTYPE_POT) {
        return;
    }

    auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];
    if (randoSaveCheck.shuffled) {
        randoSaveCheck.eligible = true;
    }
}
