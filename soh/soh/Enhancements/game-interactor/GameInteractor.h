//
//  GameInteractor.h
//  soh
//
//  Created by David Chavez on 07.12.22.
//

#ifndef GameInteractor_h
#define GameInteractor_h

#include "GameInteractionEffect.h"

#ifdef __cplusplus
class GameInteractor {
public:
    static GameInteractor* Instance;
    static GameInteractionEffectQueryResult CanApplyEffect(GameInteractionEffect effect);
    static void ApplyEffect(GameInteractionEffect effect);
    static void RemoveEffect();
    static bool CanSpawnEnemy();
    static void SpawnEnemy();
};

#endif
#endif /* GameInteractor_h */
