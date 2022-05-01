#include "Cvar.h"
#include <map>
#include <string>
#include <PR/ultra64/gbi.h>

std::map<std::string, CVar*> cvars;

CVar* CVar_GetVar(const char* name) {
    std::string key(name);
    return cvars.contains(key) ? cvars[key] : nullptr;
}

extern "C" CVar* CVar_Get(const char* name) {
    return CVar_GetVar(name);
}

extern "C" s32 CVar_GetS32(const char* name, s32 defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar != nullptr) {
        if (cvar->type == CVAR_TYPE_S32)
            return cvar->value.valueS32;
    }

    return defaultValue;
}

extern "C" float CVar_GetFloat(const char* name, float defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar != nullptr) {
        if (cvar->type == CVAR_TYPE_FLOAT)
            return cvar->value.valueFloat;
    }

    return defaultValue;
}

extern "C" char* CVar_GetString(const char* name, char* defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar != nullptr) {
        if (cvar->type == CVAR_TYPE_STRING)
            return cvar->value.valueStr;
    }

    return defaultValue;
}

extern "C" void CVar_SetS32(const char* name, s32 value) {
    CVar* cvar = CVar_Get(name);
    if (!cvar) {
        cvar = new CVar;
        cvars[std::string(name)] = cvar;
    }
    cvar->type = CVAR_TYPE_S32;
    cvar->value.valueS32 = value;
}

void CVar_SetFloat(const char* name, float value) {
    CVar* cvar = CVar_Get(name);
    if (!cvar) {
        cvar = new CVar;
        cvars[std::string(name)] = cvar;
    }
    cvar->type = CVAR_TYPE_FLOAT;
    cvar->value.valueFloat = value;
}

void CVar_SetString(const char* name, char* value) {
    CVar* cvar = CVar_Get(name);
    if (!cvar) {
        cvar = new CVar;
        cvars[std::string(name)] = cvar;
    }
    cvar->type = CVAR_TYPE_STRING;
    cvar->value.valueStr = value;
}


extern "C" void CVar_RegisterS32(const char* name, s32 defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar == nullptr)
        CVar_SetS32(name, defaultValue);
}

extern "C" void CVar_RegisterFloat(const char* name, float defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar == nullptr)
        CVar_SetFloat(name, defaultValue);
}

extern "C" void CVar_RegisterString(const char* name, char* defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar == nullptr)
        CVar_SetString(name, defaultValue);
}
