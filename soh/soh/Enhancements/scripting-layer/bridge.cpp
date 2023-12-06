#include "bridge.h"
#include "hosts/lua.h"
#include <Console.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "soh/Enhancements/scripting-layer/exceptions/bridgeexception.h"
#include "soh/Enhancements/scripting-layer/types/hostfunction.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>

#define CMD_REGISTER LUS::Context::GetInstance()->GetConsole()->AddCommand
#define ERROR_MESSAGE std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->SendErrorMessage
#define INFO_MESSAGE std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->SendInfoMessage

static bool RunScript(const std::shared_ptr<LUS::Console>&, const std::vector<std::string>& args, std::string*) {
    if (args.size() > 1) {
        auto path = std::filesystem::path("scripts") / args[1];
        auto ext = path.extension().string().substr(1);

        if(!std::filesystem::exists(path)) {
            ERROR_MESSAGE("Script not found: %s", args[1].c_str());
            return true;
        }

        if(ext == "moon"){
            ext = "lua";
        }

        std::ifstream file(path);
        std::string content((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());

        if(content.empty()) {
            ERROR_MESSAGE("Script is empty: %s", args[1].c_str());
            return true;
        }

        try {
            uint16_t pid = GameBridge::Instance->Execute(content, ext);
            INFO_MESSAGE("Script %s started with pid %d", args[1].c_str(), pid);
        } catch (GameBridgeException& e) {
            ERROR_MESSAGE("Error while executing script: %s", e.what());
            return true;
        } catch (HostAPIException& e) {
            ERROR_MESSAGE("Error while executing script: %s", e.what());
            return true;
        }
    } else {
        ERROR_MESSAGE("Usage: run <path>");
        return true;
    }
    return false;
}

static bool KillScript(const std::shared_ptr<LUS::Console>&, const std::vector<std::string>& args, std::string*) {
    if (args.size() > 1) {
        try {
            const uint16_t pid = std::stoi(args[1]);
            GameBridge::Instance->Kill(pid);
            INFO_MESSAGE("Script with pid %d killed", pid);
        } catch (std::invalid_argument&) {
            ERROR_MESSAGE("Invalid pid: %s", args[1].c_str());
            return true;
        }
    } else {
        ERROR_MESSAGE("Usage: kill <pid>");
        return true;
    }
    return false;
}

void GameBridge::Initialize() {
    this->RegisterHost("lua", std::make_shared<LuaHost>());
    this->BindFunction("print", [](uintptr_t, MethodCall* method) {
        const size_t count = method->ArgumentCount();
        std::stringstream message;
        for(size_t i = 0; i < count; i++) {
            std::any value = method->RawArgument(static_cast<int>(i));
            if (IS_TYPE(std::string, value)) {
                message << std::any_cast<std::string>(value);
            } else if (IS_TYPE(bool, value)) {
                message << (std::any_cast<bool>(value) ? "true" : "false");
            } else if (IS_TYPE(int, value)) {
                message << std::any_cast<int>(value);
            } else if (IS_TYPE(double, value)) {
                message << std::any_cast<double>(value);
            } else if (IS_TYPE(std::monostate, value) || IS_TYPE(nullptr, value)) {
                message << "null";
            } else {
                message << "[" << value.type().name() << "]";
            }
            if(i < count - 1){
                message << " ";
            }
        }
        INFO_MESSAGE(message.str().c_str());
        method->success();
    });
    // TODO: Find a way to automatically bind all hooks
    this->BindFunction("hook", [](uintptr_t, MethodCall* method) {
        const auto mode = method->GetArgument<std::string>(0);
        const auto hook = method->GetArgument<std::string>(1);
        if(mode == "add"){
            auto function = method->GetArgument<HostFunction*>(2);
            if(hook == "update"){
                const size_t idx = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([function]() {
                    try {
                        function->execute();
                    } catch (HostAPIException& e) {
                        ERROR_MESSAGE("Error while executing script: %s", e.what());
                    }
                });
                method->success(static_cast<int>(idx));
                return;
            }
            method->error("Unknown hook: " + hook);
            return;
        }
        if(mode == "remove"){
            const auto idx = method->GetArgument<int>(2);
            if(hook == "update"){
                GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnGameFrameUpdate>(idx);
                method->success();
                return;
            }
            method->error("Unknown hook: " + hook);
            return;
        }
        method->error("Unknown mode: " + mode);
    }, "Game");
    CMD_REGISTER("run", { RunScript, "Tries to execute an script", {
        { "path", LUS::ArgumentType::TEXT, true }
    }});
    CMD_REGISTER("stop", { KillScript, "Tries to kill a program with its pid", {
        { "pid", LUS::ArgumentType::NUMBER, true }
    }});
}

uint16_t GameBridge::Execute(const std::string& script, const std::string& hostname){
    const auto host = this->hosts.find(hostname);
    if(host == this->hosts.end()){
        throw GameBridgeException("Host api not found: " + hostname);
    }

    const uint16_t pid = host->second->Execute(script);
    this->pids[pid] = host->second;

    return pid;
}

void GameBridge::Kill(const uint16_t pid) const {
    if(!this->pids.contains(pid)){
        return;
    }

    const auto host = this->pids.find(pid);
    return host->second->Kill(pid);
}

void GameBridge::RegisterHost(const std::string& name, std::shared_ptr<HostAPI> host){
    if(!host->Initialize()){
        throw GameBridgeException("Failed to initialize host api: " + name);
    }
    this->hosts[name] = std::move(host);
}

void GameBridge::Register(const std::vector<NamedEntry>& entries, const std::string& mod_name) const {
    for (const auto& [fst, snd] : this->hosts) {
        for (auto& [name, link] : entries) {
            snd->Bind(name, { static_cast<BindingType>(link.index()), link, mod_name });
        }
    }
}

void GameBridge::BindField(const std::string& name, const std::any& field, const std::string& mod_name) const {
    for(const auto& [fst, snd] : this->hosts){
        snd->Bind(name, { BindingType::KField, field, mod_name });
    }
}

void GameBridge::BindFunction(const std::string& name, FunctionPtr function, const std::string& mod_name) const {
    for(const auto& [fst, snd] : this->hosts){
        snd->Bind(name, { BindingType::KFunction, function, mod_name });
    }
}
