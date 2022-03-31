#include "SohHooks.h"
#include <vector>
#include <cstdarg>
#include <iostream>

std::unordered_map<HookTable, std::vector<void (*)(HookEvent)>> listeners;
std::unordered_map<unsigned long, void*> hookArgs;
HookTable hookEvent = NULL_HOOK;

unsigned long hash(const char* str) {
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void* HookCall::getArgument(const char* key) {
    return this->args[hash(key)];
}

/*
#############################
   Module: Hook C++ Handle
#############################
*/

namespace ModInternal {

    void registerHookListener(const HookListener& listener) {
        listeners[listener.event].push_back(listener.callback);
    }

    bool handleHook(const std::shared_ptr<HookCall>& call) {
	    const HookTable event = call->event;
        for (auto& l : listeners[event]) {
	        l(call);
        }
        return call->cancelled;
    }

    void bindHook(HookTable event) {
        hookEvent = event;
    }

    bool triggerHook(int length, ...) {
        if (length > 0) {
            va_list args;
            va_start(args, length);
            for (int i = 0; i < length; i++) {
	            const HookParameter currentParam = va_arg(args, struct HookParameter);
                hookArgs[hash(currentParam.name)] = currentParam.parameter;
            }
            va_end(args);
        }

        HookCall call = {
            .event = hookEvent,
            .args = hookArgs
		};
        const bool cancelled = handleHook(std::make_shared<HookCall>(call));

        hookEvent = NULL_HOOK;
        hookArgs.clear();

        return cancelled;
    }
}

/*
#############################
    Module: Hook C Handle
#############################
*/

extern "C" {

    void bind_hook(HookTable event) {
        hookEvent = event;
    }

    bool trigger_hook(int length, ...) {
        if (length > 0) {
            va_list args;
            va_start(args, length);
            for (int i = 0; i < length; i++) {
	            const HookParameter currentParam = va_arg(args, struct HookParameter);
                hookArgs[hash(currentParam.name)] = currentParam.parameter;
            }
            va_end(args);
        }

        HookCall call = {
            .event = hookEvent,
            .args = hookArgs
        };

        const bool cancelled = ModInternal::handleHook(std::make_shared<HookCall>(call));

        hookEvent = NULL_HOOK;
        hookArgs.clear();

        return cancelled;
    }
}