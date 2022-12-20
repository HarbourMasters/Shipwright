//
//  GameInteractor.cpp
//  soh
//
//  Created by David Chavez on 07.12.22.
//

#include "GameInteractor.h"

GameInteractionEffectQueryResult GameInteractor::CanApplyEffect(GameInteractionEffectBase effect) {
    return effect.CanBeApplied();
}

void GameInteractor::ApplyEffect(GameInteractionEffectBase effect) {
    return effect.Apply();
}
