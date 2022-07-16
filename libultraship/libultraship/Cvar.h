#ifndef _CVAR_H
#define _CVAR_H

typedef enum CVarType { CVAR_TYPE_S32, CVAR_TYPE_FLOAT, CVAR_TYPE_STRING } CVarType;

typedef struct CVar {
    const char* name;
    CVarType type;

    union {
        int valueS32;
        float valueFloat;
        const char* valueStr;
    } value;
} CVar;

#ifdef __cplusplus
extern "C"
{
#endif
//#include <ultra64.h>

CVar* CVar_Get(const char* name);
int CVar_GetS32(const char* name, int defaultValue);
float CVar_GetFloat(const char* name, float defaultValue);
const char* CVar_GetString(const char* name, const char* defaultValue);
void CVar_SetS32(const char* name, int value);
void CVar_SetString(const char* name, const char* value);

void CVar_RegisterS32(const char* name, int defaultValue);
void CVar_RegisterFloat(const char* name, float defaultValue);
void CVar_RegisterString(const char* name, const char* defaultValue);

#ifdef __cplusplus
};
#endif

#ifdef __cplusplus
#include <map>
#include <string>
#include <functional>
#include <memory>

extern std::map<std::string, std::unique_ptr<CVar>, std::less<>> cvars;
void CVar_SetFloat(const char* name, float value);
#endif
#endif
