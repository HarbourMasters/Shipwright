//
//  GameInteractionEffect.cpp
//  soh
//
//  Created by David Chavez on 15.12.22.
//

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

GameInteractionEffectQueryResult AddHeartContainer::CanBeApplied() {
    return gSaveContext.healthCapacity >= 0x140
        ? GameInteractionEffectQueryResult::NotPossibe
        : GameInteractionEffectQueryResult::Possible;
}

void AddHeartContainer::Apply() {
    Health_GiveHearts(1);
}

// RemoveHeartContainer

GameInteractionEffectQueryResult RemoveHeartContainer::CanBeApplied() {
    return ((gSaveContext.healthCapacity - 0x10) <= 0)
        ? GameInteractionEffectQueryResult::NotPossibe
        : GameInteractionEffectQueryResult::Possible;
}

void RemoveHeartContainer::Apply() {
    Health_RemoveHearts(1);
}

// GiveRupees

GameInteractionEffectQueryResult GiveRupees::CanBeApplied() {
    return GameInteractionEffectQueryResult::Possible;
}

void GiveRupees::Apply() {
    Rupees_ChangeBy(amount);
}


// NoUIEffect

GameInteractionEffectQueryResult NoUIEffect::CanBeApplied() {
    return GameInteractionEffectQueryResult::Possible;
}

void NoUIEffect::Apply() {
    GameInteractor_NoUI = 1;
}
