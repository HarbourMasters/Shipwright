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

class GameInteractionEffectBase {
public:
    virtual GameInteractionEffectQueryResult CanBeApplied() = 0;
    virtual void Apply() = 0;
};

namespace GameInteractionEffect {
    class AddHeartContainer: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void Apply() override;
    };

    class RemoveHeartContainer: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void Apply() override;
    };

    class AddOrRemoveHeartContainer: public GameInteractionEffectBase {
        int32_t amount;

        GameInteractionEffectQueryResult CanBeApplied() override;
        void Apply() override;
    };

    class GiveRupees: public GameInteractionEffectBase {
    public:
        uint32_t amount;

        GameInteractionEffectQueryResult CanBeApplied() override;
        void Apply() override;
    };

    class NoUI: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void Apply() override;
    };
}

#endif /* __cplusplus */
#endif /* GameInteractionEffect_h */
