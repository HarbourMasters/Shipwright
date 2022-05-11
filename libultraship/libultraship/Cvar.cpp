#include "Cvar.h"
#include <map>
#include <string>
#include <functional>
#include <memory>
#include <utility>
#include <PR/ultra64/gbi.h>

std::map<std::string, std::unique_ptr<CVar>, std::less<>> cvars;

extern "C" CVar* CVar_Get(const char* name) {
    auto it = cvars.find(name);
    return (it != cvars.end()) ? it->second.get() : nullptr;
}

extern "C" s32 CVar_GetS32(const char* name, s32 defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar) {
        if (cvar->type == CVAR_TYPE_S32)
            return cvar->value.valueS32;
    }

    return defaultValue;
}

extern "C" float CVar_GetFloat(const char* name, float defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar) {
        if (cvar->type == CVAR_TYPE_FLOAT)
            return cvar->value.valueFloat;
    }

    return defaultValue;
}

extern "C" const char* CVar_GetString(const char* name, const char* defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar) {
        if (cvar->type == CVAR_TYPE_STRING)
            return cvar->value.valueStr;
    }

    return defaultValue;
}

extern "C" void CVar_SetS32(const char* name, s32 value) {
    auto& cvar = cvars[name];
    if (!cvar) {
        cvar = std::make_unique<CVar>();
    }
    cvar->type = CVAR_TYPE_S32;
    cvar->value.valueS32 = value;
}

void CVar_SetFloat(const char* name, float value) {
    auto& cvar = cvars[name];
    if (!cvar) {
        cvar = std::make_unique<CVar>();
    }
    cvar->type = CVAR_TYPE_FLOAT;
    cvar->value.valueFloat = value;
}

void CVar_SetString(const char* name, const char* value) {
    auto& cvar = cvars[name];
    if (!cvar) {
        cvar = std::make_unique<CVar>();
    }
    cvar->type = CVAR_TYPE_STRING;
    cvar->value.valueStr = value;
}

extern "C" void CVar_RegisterS32(const char* name, s32 defaultValue) {
    if (!CVar_Get(name))
        CVar_SetS32(name, defaultValue);
}

extern "C" void CVar_RegisterFloat(const char* name, float defaultValue) {
    if (!CVar_Get(name))
        CVar_SetFloat(name, defaultValue);
}

extern "C" void CVar_RegisterString(const char* name, const char* defaultValue) {
    if (!CVar_Get(name))
        CVar_SetString(name, defaultValue);
}
