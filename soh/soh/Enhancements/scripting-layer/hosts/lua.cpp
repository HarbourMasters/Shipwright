#include "lua.h"
#include "soh/Enhancements/scripting-layer/exceptions/hostapiexception.h"
#include "soh/Enhancements/scripting-layer/types/methodcall.h"
#include "soh/Enhancements/scripting-layer/types/hostfunction.h"
#include <cstring>

#include <filesystem>
#include <unordered_map>
#include <fstream>

extern "C" {
// Fixed dumb error because switch seems to not have limits.h
#if defined(__SWITCH__) || defined(__WIIU__)
#define LLONG_MAX 9223372036854775807LL
#define LLONG_MIN  (-LLONG_MAX-1LL)
#define ULLONG_MAX (LLONG_MAX*2ULL+1ULL)
#endif

#include <lib/lua/lua.h>
#include <lib/lua/lualib.h>
#include <lib/lua/lauxlib.h>
}

typedef int(*LuaFunction)(lua_State*);
typedef std::variant<LuaFunction, std::any> LuaBinding;
std::unordered_map<std::string, std::unordered_map<std::string, LuaBinding>> mLuaBindings;
std::vector<lua_State*> mLuaStates;

const char* tryCatchImpl = R"(
function try(f, catch_f)
    local status, exception = pcall(f)
    if not status then
        catch_f(exception)
    end
end
)";

static const luaL_Reg mLuaLibs[] = {
    { "", luaopen_base },
    { LUA_TABLIBNAME, luaopen_table },
    { LUA_STRLIBNAME, luaopen_string },
    { LUA_MATHLIBNAME, luaopen_math },
    { LUA_DBLIBNAME, luaopen_debug },
    { nullptr, nullptr }
};

struct EmptyTable {};

bool LuaHost::Initialize() {
    this->BindRequireOverride();
    return true;
}

void LuaHost::BindRequireOverride() {
    FunctionPtr call = [](uintptr_t ctx, MethodCall* method) {
        auto _module = method->GetArgument<std::string>(0);

        // This is a proof of concept, the script external loader should be on the game bridge
        // and not on the host.
        if(std::filesystem::exists(_module) && std::filesystem::is_regular_file(_module) && _module.ends_with(".lua")){
            auto path = std::filesystem::path("scripts") / _module;
            auto ext = path.extension().string().substr(1);

            if(!exists(path)) {
                method->error("File not found");
                return;
            }

            std::ifstream file(path);
            std::string content((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());
            luaL_loadstring(reinterpret_cast<lua_State*>(ctx), content.c_str());
            method->success();
            return;
        }

        auto state = reinterpret_cast<lua_State*>(ctx);

        if (!mLuaBindings.contains(_module)){
            method->error("Module not found");
            return;
        }

        auto& functions = mLuaBindings[_module];

        luaL_checkversion(state);
        lua_createtable(state, 0, static_cast<int>(functions.size() - 1));

        for (auto& func : functions) {

            LuaBinding& binding = func.second;

            if(binding.index() == 0){
                lua_pushlightuserdata(state, method->GetHost());
                lua_pushlightuserdata(state, &dynamic_cast<LuaHost *>(method->GetHost())->mBindings[func.first]);
                lua_pushcclosure(state, std::get<LuaFunction>(binding), 2);
            } else {
                auto value = std::get<std::any>(binding);
                PushIntoLua(reinterpret_cast<uintptr_t>(state), value);
            }

            lua_setfield(state, -2, func.first.c_str());
        }

        lua_setglobal(state, _module.c_str());

        method->success();
    };
    // TODO: Find a better way to avoid repeating this.
    this->Bind("require", { BindingType::KFunction, call });
}

void LuaHost::PushIntoLua(const uintptr_t context, const std::any& value) {
    auto *state = reinterpret_cast<lua_State *>(context);

    if (IS_TYPE(std::string, value)) {
        lua_pushstring(state, std::any_cast<std::string>(value).c_str());
    } else if (IS_TYPE(bool, value)) {
        lua_pushboolean(state, std::any_cast<bool>(value));
    } else if (IS_TYPE(int, value)) {
        lua_pushinteger(state, std::any_cast<int>(value));
    } else if (IS_TYPE(double, value)) {
        lua_pushnumber(state, std::any_cast<double>(value));
    } else if (IS_TYPE(float, value)) {
        lua_pushboolean(state, std::any_cast<int>(value));
    } else if (IS_TYPE(std::vector<std::any>, value)) {
        lua_newtable(state);
        int index = 1;
        for (auto &item: std::any_cast<std::vector<std::any>>(value)) {
            printf("Pushing into lua: %s\n", item.type().name());
            PushIntoLua(context, item);
            lua_rawseti(state, -2, index++);
        }
    } else if (IS_TYPE(std::monostate, value) || IS_TYPE(nullptr, value)) {
        lua_pushnil(state);
    } else if (IS_TYPE(EmptyTable, value)) {
        // TODO: Remove this, it's a workaround for the import function
    } else {
        throw HostAPIException("Unknown type" + std::string(value.type().name()));
    }
}

void LuaHost::Bind(std::string name, GameBinding binding) {
    mBindings[name] = binding;
    if (binding.type == BindingType::KFunction) {
        LuaFunction func = [](lua_State* state) -> int {
            auto* api = static_cast<LuaHost*>(const_cast<void*>(lua_topointer(state, lua_upvalueindex(1))));
            const auto* bind = static_cast<const GameBinding *>(lua_topointer(state, lua_upvalueindex(2)));

            auto* result = new MethodCall(api, reinterpret_cast<uintptr_t>(state));
            const auto execute = std::get<FunctionPtr>(bind->binding);
            execute(reinterpret_cast<uintptr_t>(state), result);

            const std::vector<std::any> results = result->result();

            if(results.empty()){
                throw HostAPIException("No results returned from function");
            }

            if (result->succeed()) {
                for (auto& value : results) {
                    PushIntoLua(reinterpret_cast<uintptr_t>(state), value);
                }
            } else {
                luaL_error(state, std::any_cast<std::string>(results[0]).c_str());
            }
            return static_cast<int>(results.size());
        };

        mLuaBindings[binding.module].insert({ name, func });
        return;
    }

    if (binding.type == BindingType::KField) {
        mLuaBindings[binding.module].insert({ name, std::get<std::any>(binding.binding) });
    }
}

void LuaHost::Call(const uintptr_t context, const uintptr_t function, const std::vector<std::any> &arguments) {

    auto* state = reinterpret_cast<lua_State *>(context);
    const auto ref = static_cast<int>(function);

    if (state == nullptr) {
        throw HostAPIException("Invalid host state, probably closed");
    }

    lua_rawgeti( state, LUA_REGISTRYINDEX, ref );

    for (auto& argument : arguments) {
        PushIntoLua(reinterpret_cast<uintptr_t>(state), argument);
    }

    if (lua_pcall(state, static_cast<int>(arguments.size()), 0, 0) != 0) {
        throw HostAPIException("Error while calling function with ref " + std::to_string(ref));
    }
}

std::any LuaHost::GetArgument(int index, const uintptr_t context) {
    auto* state = reinterpret_cast<lua_State *>(context);
    index += 1;

    if (lua_isinteger(state, index)) {
        return static_cast<int>(luaL_checkinteger(state, index));
    }

    if (lua_isnumber(state, index)) {
        return luaL_checknumber(state, index);
    }

    if (lua_isboolean(state, index)) {
        return luaL_checkinteger(state, index);
    }

    if (lua_isstring(state, index)) {
        return std::string(luaL_checkstring(state, index));
    }

    if (lua_isfunction(state, index)) {
        const int ref = luaL_ref(state, LUA_REGISTRYINDEX);
        return new HostFunction(this, reinterpret_cast<uintptr_t>(state), ref);
    }

    if (lua_isnoneornil(state, index)) {
        return nullptr;
    }

    throw HostAPIException("Unknown argument type: " + std::string(lua_typename(state, lua_type(state, index))));
}

size_t LuaHost::GetArgumentCount(const uintptr_t context) {
    auto* state = reinterpret_cast<lua_State *>(context);
    return lua_gettop(state);
}

uint16_t LuaHost::Execute(const std::string& script) {
    lua_State* state = luaL_newstate();

    const luaL_Reg *lib = mLuaLibs;
    for (; lib->func; lib++) {
        lua_pushcfunction(state, lib->func);
        lua_pushstring(state, lib->name);
        lua_call(state, 1, 0);
    }

    for (auto& [fst, snd] : mLuaBindings[ROOT_MODULE]) {

        LuaBinding binding = snd;

        if(binding.index() == 0){
            lua_pushlightuserdata(state, this);
            lua_pushlightuserdata(state, &this->mBindings[fst]);
            lua_pushcclosure(state, std::get<LuaFunction>(binding), 2);
        } else {
            auto value = std::get<std::any>(binding);
            PushIntoLua(reinterpret_cast<uintptr_t>(state), value);
        }

        lua_setglobal(state, fst.c_str());
    }

    luaL_dostring(state, tryCatchImpl);

    

    const int ret = luaL_dostring(state, script.c_str());
    if (ret != LUA_OK) {
        const std::string error(lua_tostring(state, -1));
        lua_close(state);
        throw HostAPIException(error);
    }

    mLuaStates.push_back(state);
    return static_cast<uint16_t>(mLuaStates.size()) - 1;
}

void LuaHost::Kill(const uint16_t pid) {
    if (pid > mLuaStates.size()) {
        return;
    }

    lua_State* state = mLuaStates[pid];

    lua_getglobal(state, "onExit");
    if(lua_isfunction(state, -1)){
        lua_pcall(state, 0, 0, 0);
    }

    lua_close(state);
    mLuaStates.erase(mLuaStates.begin() + pid);
}