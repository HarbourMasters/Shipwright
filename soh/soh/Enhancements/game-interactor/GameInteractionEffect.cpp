#include "GameInteractionEffect.h"
#include "GameInteractor.h"

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
}

// Effect State

uint32_t GameInteractor_NoUI;

// AddHeartContainer

namespace GameInteractionEffect {

    // Add Heart Container
    GameInteractionEffectQueryResult AddHeartContainer::CanBeApplied() {
        if (gSaveContext.healthCapacity >= 0x140) {
            return GameInteractionEffectQueryResult::NotPossible;
        } else if (!GameInteractor::IsSaveLoaded()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void AddHeartContainer::Apply() {
        Health_GiveHearts(1);
    }
    void AddHeartContainer::Remove() {}
    // End Add Heart Container

    // No UI
    GameInteractionEffectQueryResult NoUI::CanBeApplied() {
        if (!GameInteractor::IsSaveLoaded() || GameInteractor::IsGameplayPaused()) {
            return GameInteractionEffectQueryResult::TemporarilyNotPossible;
        } else {
            return GameInteractionEffectQueryResult::Possible;
        }
    }
    void NoUI::Apply() {
        GameInteractor_NoUI = 1;
    }

    void NoUI::Remove() {
        GameInteractor_NoUI = 0;
    }
    // End No UI

    // RemoveHeartContainer
    GameInteractionEffectQueryResult RemoveHeartContainer::CanBeApplied() {
        return ((gSaveContext.healthCapacity - 0x10) <= 0)
        ? GameInteractionEffectQueryResult::NotPossible
        : GameInteractionEffectQueryResult::Possible;
    }

    void RemoveHeartContainer::Apply() {
        Health_RemoveHearts(1);
    }
    void RemoveHeartContainer::Remove() {}

    // GiveRupees
    GameInteractionEffectQueryResult GiveRupees::CanBeApplied() {
        return GameInteractionEffectQueryResult::Possible;
    }

    void GiveRupees::Apply() {
        Rupees_ChangeBy(amount);
    }
    void GiveRupees::Remove() {}
}
