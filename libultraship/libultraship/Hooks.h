#pragma once

#ifdef __cplusplus

#include <functional>

#include "UltraController.h"
#include "Controller.h"

#define DEFINE_HOOK(name, type) struct name { typedef std::function<type> fn; }

namespace ModInternal {

    template <typename H>
    struct RegisteredHooks {
        inline static std::vector<typename H::fn> functions;
    };

    template <typename H>
    void RegisterHook(typename H::fn h) {
        RegisteredHooks<H>::functions.push_back(h);
    }

    template <typename H, typename... Args>
    void ExecuteHooks(Args&&... args) {
        for (auto& fn : RegisteredHooks<H>::functions) {
            fn(std::forward<Args>(args)...);
        }
    }

    DEFINE_HOOK(ControllerRead, void(OSContPad* cont_pad));
    DEFINE_HOOK(ControllerRawInput, void(Ship::Controller* backend, uint32_t raw));
    DEFINE_HOOK(AudioInit, void());
    DEFINE_HOOK(LoadTexture, void(const char* path, uint8_t** texture));
    DEFINE_HOOK(GfxInit, void());
    DEFINE_HOOK(ExitGame, void());

}

#endif

#ifdef __cplusplus
extern "C" {
#endif

void ModInternal_ExecuteAudioInitHooks();

#ifdef __cplusplus
}
#endif
