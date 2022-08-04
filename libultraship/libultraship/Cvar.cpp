#include "Cvar.h"
#include <map>
#include <string.h>
#include <functional>
#include <memory>
#include <utility>

std::map<std::string, std::unique_ptr<CVar>, std::less<>> cvars;

CVar* CVar_Get(const char* name) {
    auto it = cvars.find(name);
    return (it != cvars.end()) ? it->second.get() : nullptr;
}

extern "C" int32_t CVar_GetS32(const char* name, int32_t defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar) {
        if (cvar->type == CVarType::S32)
            return cvar->value.valueS32;
    }

    return defaultValue;
}

extern "C" float CVar_GetFloat(const char* name, float defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar) {
        if (cvar->type == CVarType::Float)
            return cvar->value.valueFloat;
    }

    return defaultValue;
}

extern "C" const char* CVar_GetString(const char* name, const char* defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar) {
        if (cvar->type == CVarType::String)
            return cvar->value.valueStr;
    }

    return defaultValue;
}

extern "C" Color_RGB8 CVar_GetRGB(const char* name, Color_RGB8 defaultValue)
{
    Color_RGBA8 defaultValueRGBA;
    defaultValueRGBA.r = defaultValue.r;
    defaultValueRGBA.g = defaultValue.g;
    defaultValueRGBA.b = defaultValue.b;
    defaultValueRGBA.a = 255;

    Color_RGBA8 cvarGet = CVar_GetRGBA(name, defaultValueRGBA);
    Color_RGB8 result;

    result.r = cvarGet.r;
    result.g = cvarGet.g;
    result.b = cvarGet.b;

    return result;
}

extern "C" Color_RGBA8 CVar_GetRGBA(const char* name, Color_RGBA8 defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar != nullptr) {
        if (cvar->type == CVarType::RGBA)
            return cvar->value.valueRGBA;
    }

    return defaultValue;
}

extern "C" void CVar_SetRGBA(const char* name, Color_RGBA8 value)
{
    auto& cvar = cvars[name];
    if (!cvar) {
        cvar = std::make_unique<CVar>();
    }

    cvar->type = CVarType::RGBA;
    cvar->value.valueRGBA = value;
}

extern "C" void CVar_SetS32(const char* name, int32_t value) {
    auto& cvar = cvars[name];
    if (!cvar) {
        cvar = std::make_unique<CVar>();
    }
    cvar->type = CVarType::S32;
    cvar->value.valueS32 = value;
}

extern "C" void CVar_SetFloat(const char* name, float value) {
    auto& cvar = cvars[name];
    if (!cvar) {
        cvar = std::make_unique<CVar>();
    }
    cvar->type = CVarType::Float;
    cvar->value.valueFloat = value;
}

extern "C" void CVar_SetString(const char* name, const char* value) {
    auto& cvar = cvars[name];
    if (!cvar) {
        cvar = std::make_unique<CVar>();
    }
    cvar->type = CVarType::String;
#ifdef _MSC_VER
    cvar->value.valueStr = _strdup(value);
#else
    cvar->value.valueStr = strdup(value);
#endif
}

extern "C" void CVar_RegisterRGBA(const char* name, Color_RGBA8 defaultValue) {
    if (!CVar_Get(name))
        CVar_SetRGBA(name, defaultValue);
}

extern "C" void CVar_RegisterS32(const char* name, int32_t defaultValue) {
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
