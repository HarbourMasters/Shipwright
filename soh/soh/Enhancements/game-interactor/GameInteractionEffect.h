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
    NotPossible             = 0xFF
};

class GameInteractionEffectBase {
public:
    virtual GameInteractionEffectQueryResult CanBeApplied() = 0;
    virtual void Apply() = 0;
    virtual void Remove() = 0;
};

namespace GameInteractionEffect {
    class AddHeartContainer: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void Apply() override;
        void Remove() override;
    };

    class RemoveHeartContainer: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void Apply() override;
        void Remove() override;
    };

    class GiveRupees: public GameInteractionEffectBase {
    public:
        uint32_t amount;

        GameInteractionEffectQueryResult CanBeApplied() override;
        void Apply() override;
        void Remove() override;
    };

    class NoUI: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void Apply() override;
        void Remove() override;
    };
}

#endif /* __cplusplus */
#endif /* GameInteractionEffect_h */
