#pragma once

#include "./hosts/hostapi.h"
#include "types/methodcall.h"
#include <unordered_map>
#include <string>

#define BIND_FUNCTION(name, ptr) NamedEntry { name, (FunctionPtr)ptr }
#define BIND_FIELD(name, field) NamedEntry { name, field }
#define AUTOBIND_FIELD(field) NamedEntry { #field, field }

struct NamedEntry {
    const std::string& name;
    const std::variant<FunctionPtr, std::any> link;
};

class GameBridge {
    std::unordered_map<std::string, std::shared_ptr<HostAPI>> hosts;
    std::unordered_map<uint16_t, std::shared_ptr<HostAPI>> pids;
    void RegisterHost(const std::string& name, std::shared_ptr<HostAPI> host);
    void BindField(const std::string& name, const std::any& field, const std::string& mod_name = ROOT_MODULE) const;
    void BindFunction(const std::string& name, FunctionPtr function, const std::string& mod_name = ROOT_MODULE) const;
public:
    static GameBridge* Instance;
    void Initialize();
    uint16_t Execute(const std::string& script, const std::string& hostname);

    // Static Methods
    void Register(const std::vector<NamedEntry>& entries, const std::string& mod_name = ROOT_MODULE) const;
    void Kill(uint16_t pid) const;
};