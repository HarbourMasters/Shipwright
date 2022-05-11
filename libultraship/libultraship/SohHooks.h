#pragma once

struct HookParameter {
    const char* name;
    void* parameter;
};

#define LOOKUP_TEXTURE  "F3D::LookupCacheTexture"
#define GRAYOUT_TEXTURE "Kaleido::GrayOutTexture"
#define INVALIDATE_TEXTURE "GBI::gSPInvalidateTexCache"
#define CONTROLLER_READ "N64::ControllerRead"

#define AUDIO_INIT      "AudioMgr::Init"

#define LOAD_TEXTURE    "ResourceMgr::LoadTexByName"

#define UPDATE_VOLUME   "AudioVolume::Bind"

#define IMGUI_API_INIT "ImGuiApiInit"
#define IMGUI_API_DRAW "ImGuiApiDraw"

#define WINDOW_API_INIT  "WApiInit"
#define WINDOW_API_HANDLE_EVENTS  "WApiHandleEvents"
#define WINDOW_API_START_FRAME  "WApiStartFrame"

// Graphics API Hooks
#define GFX_PRE_START_FRAME  "GFXApiPreStartFrame"
#define GFX_POST_START_FRAME "GFXApiPostStartFrame"

#define GFX_PRE_END_FRAME  "GFXApiPreEndFrame"
#define GFX_POST_END_FRAME "GFXApiPostEndFrame"

#define GFX_ON_REZISE "GFXApiOnResize"
#define GFX_INIT      "GFXApiInit"
#define GFX_SHUTDOWN  "GFXApiShutdown"

// End

#ifdef __cplusplus

#define HOOK_PARAMETER(name, ptr) HookParameter({ name, static_cast<void*>(ptr) })
#define BIND_HOOK(name, func) ModInternal::registerHookListener({ name, [this](HookEvent call) { func(call); }})
#define BIND_PTR(name, type) static_cast<type>(call->baseArgs[name])
#define BIND_VAR(name, type) *BIND_PTR(name, type)


#include <functional>
#include <string>
#include <map>
#include <memory>

struct HookCall {
    std::string name;
    std::map<std::string, void*> baseArgs;
    std::map<std::string, void*> hookedArgs;
    bool cancelled = false;
};

typedef std::shared_ptr<HookCall> HookEvent;
typedef std::function<void(HookEvent)> HookFunc;
struct HookListener {
    std::string hookName;
    HookFunc callback;
    int priority = 0;
};

namespace ModInternal {
    void registerHookListener(HookListener listener);
    void bindHook(std::string name);
    void initBindHook(int length, ...);
    bool callBindHook(int length, ...);
}

#else

void bind_hook(char* name);
void init_hook(int length, ...);
bool call_hook(int length, ...);

#endif