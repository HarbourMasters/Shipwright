//
//  GameInteractionEffect.h
//  soh
//
//  Created by David Chavez on 15.12.22.
//

#ifndef GameInteractionEffect_h
#define GameInteractionEffect_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
extern uint32_t GameInteractor_NoUI;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
enum GameInteractionEffectQueryResult {
    Possible                = 0x00,
    TemporarilyNotPossible  = 0x01,
    NotPossibe              = 0xFF
};

class GameInteractionEffect {
public:
    virtual GameInteractionEffectQueryResult CanBeApplied() = 0;
    virtual void Apply() = 0;
};

class AddHeartContainer: GameInteractionEffect {
    GameInteractionEffectQueryResult CanBeApplied() override;
    void Apply() override;
};

class RemoveHeartContainer: GameInteractionEffect {
    GameInteractionEffectQueryResult CanBeApplied() override;
    void Apply() override;
};

class GiveRupees: GameInteractionEffect {
public:
    uint32_t amount;

    GameInteractionEffectQueryResult CanBeApplied() override;
    void Apply() override;
};

class NoUIEffect: GameInteractionEffect {
    GameInteractionEffectQueryResult CanBeApplied() override;
    void Apply() override;
};

#endif /* __cplusplus */
#endif /* GameInteractionEffect_h */
