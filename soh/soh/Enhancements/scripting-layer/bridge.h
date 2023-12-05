#pragma once

#include "./hosts/hostapi.h"
#include "types/methodcall.h"
#include <unordered_map>
#include <string>
#include <memory>

#define BIND_FUNCTION(name, ptr) NamedEntry { name, (FunctionPtr)ptr }
#define BIND_FIELD(name, field) NamedEntry { name, field }
#define AUTOBIND_FIELD(field) NamedEntry { #field, field }

struct NamedEntry {
    const std::string& name;
    const std::variant<FunctionPtr, std::any> link;
};

class GameBridge {
private:
    std::unordered_map<std::string, std::shared_ptr<HostAPI>> hosts;
    std::unordered_map<uint16_t, std::shared_ptr<HostAPI>> pids;
    void RegisterHost(const std::string& name, std::shared_ptr<HostAPI> host);
public:
    GameBridge() {
        this->Initialize();
    }
    static GameBridge* Instance;
    void Initialize();
    uint16_t Execute(const std::string& script, const std::string& hostname);

    // Static Methods
    void Register(std::vector<NamedEntry> entries, const std::variant<std::string, std::monostate>& mod_name = std::monostate());
    void BindField(const std::string& name, const std::any& field, const std::variant<std::string, std::monostate>& mod_name = std::monostate());
    void BindFunction(const std::string& name, FunctionPtr function, const std::variant<std::string, std::monostate>& mod_name = std::monostate());
    void Kill(uint16_t pid);
};