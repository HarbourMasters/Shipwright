//
//  GameInteractor.cpp
//  soh
//
//  Created by David Chavez on 07.12.22.
//

#include "GameInteractor.h"

GameInteractionEffectQueryResult GameInteractor::CanApplyEffect(GameInteractionEffect effect) {
    return effect.CanBeApplied();
}

void GameInteractor::ApplyEffect(GameInteractionEffect effect) {
    return effect.Apply();
}
