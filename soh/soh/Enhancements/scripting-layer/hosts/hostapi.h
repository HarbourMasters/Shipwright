#pragma once

#include <any>
#include <vector>

struct GameBinding;

#define IS_TYPE(t, value) value.type() == typeid(t)
#define ROOT_MODULE "root"

class HostAPI {
public:
    virtual bool Initialize() = 0;
    virtual void Bind(std::string name, GameBinding binding) = 0;
    virtual void Call(uintptr_t context, uintptr_t function, const std::vector<std::any>& arguments) = 0;
    virtual std::any GetArgument(int index, uintptr_t context) = 0;
    virtual size_t GetArgumentCount(uintptr_t context) = 0;
    virtual uint16_t Execute(const std::string& script) = 0;
    virtual void Kill(uint16_t pid) = 0;
};