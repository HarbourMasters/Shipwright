#ifndef _CVAR_H
#define _CVAR_H

#include <PR/ultra64/gbi.h>

typedef enum CVarType { CVAR_TYPE_S32, CVAR_TYPE_FLOAT, CVAR_TYPE_STRING } CVarType;

typedef struct CVar {
    char* name;
    CVarType type;

    union {
        s32 valueS32;
        float valueFloat;
        char* valueStr;
    } value;
} CVar;

#ifdef __cplusplus
extern "C"
{
#endif
//#include <ultra64.h>


CVar* CVar_Get(const char* name);
s32 CVar_GetS32(const char* name, s32 defaultValue);
float CVar_GetFloat(const char* name, float defaultValue);
char* CVar_GetString(const char* name, char* defaultValue);
void CVar_SetS32(const char* name, s32 value);

void CVar_RegisterS32(const char* name, s32 defaultValue);
void CVar_RegisterFloat(const char* name, float defaultValue);
void CVar_RegisterString(const char* name, char* defaultValue);

#ifdef __cplusplus
};
#endif

#ifdef __cplusplus
#include <map>
#include <string>

extern std::map<std::string, CVar*> cvars;
CVar* CVar_GetVar(const char* name);
void CVar_SetFloat(const char* name, float value);
void CVar_SetString(const char* name, char* value);
#endif
#endif
