#ifdef ENABLE_REMOTE_CONTROL

#ifdef __cplusplus
#include <SDL2/SDL_net.h>
#include <cstdint>
#include <thread>
#include <memory>
#include <map>
#include <vector>
#include <iostream>
#include <chrono>
#include <future>

#include "./GameInteractor.h"

class GameInteractorSail {
    private:
        bool isEnabled;

        void HandleRemoteJson(nlohmann::json payload);
        GameInteractionEffectBase* EffectFromJson(nlohmann::json payload);
        void RegisterHooks();
    public:
        static GameInteractorSail* Instance;
        void Enable();
        void Disable();
};
#endif
#endif
