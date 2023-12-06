#pragma once

#include "hostapi.h"
#include <string>
#include <unordered_map>

struct GameBinding;

class LuaHost : public HostAPI {
    std::unordered_map<std::string, GameBinding> mBindings;
    void CreateLuaState();
    void BindRequireOverride();
public:
    bool Initialize() override;
    void Bind(std::string name, GameBinding binding) override;
    void Call(uintptr_t context, uintptr_t function, const std::vector<std::any>& arguments) override;
    std::any GetArgument(int index, uintptr_t context) override;
    size_t GetArgumentCount(uintptr_t context) override;
    uint16_t Execute(const std::string& script) override;
    void Kill(uint16_t pid) override;

    // LuaHost specific methods
    static void PushIntoLua(uintptr_t state, const std::any& value);
};