//
//  GameInteractor.h
//  soh
//
//  Created by David Chavez on 07.12.22.
//

#ifndef GameInteractor_h
#define GameInteractor_h

#include <stdio.h>

enum GameInteractionEffectQueryResult {
    Possible        = 0x00,
    NotPossible     = 0x01,
    NotApplicable   = 0xFF
};

class GameInteractionEffect {
    virtual GameInteractionEffectQueryResult CanBeApplied();
};

class SetLinkOnFire: GameInteractionEffect {
    GameInteractionEffectQueryResult CanBeApplied() override {

    }
};

class GameInteractor {
public:
    static GameInteractionEffectQueryResult CanApplyEffect(GameInteractionEffect effect);
    static void ApplyEffect(GameInteractionEffect effect);
    static void RemoveEffect();
    static bool CanSpawnEnemy();
    static void SpawnEnemy();

    // Game hooks?
    void RegisterItemWillBeGivenHook() {}
private:
};

#endif /* GameInteractor_h */
