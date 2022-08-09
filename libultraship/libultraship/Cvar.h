#ifndef _CVAR_H
#define _CVAR_H

#include "color.h"
#include <stdint.h>

#ifdef __cplusplus
typedef enum class CVarType 
{ 
    S32, 
    Float, 
    String,
    RGBA
} CVarType;

typedef struct CVar {
    const char* name;
    CVarType type;

    union {
        int32_t valueS32;
        float valueFloat;
        const char* valueStr;
        Color_RGBA8 valueRGBA;
    } value;
} CVar;

extern "C" CVar * CVar_Get(const char* name);
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    float CVar_GetFloat(const char* name, float defaultValue);
    void CVar_SetFloat(const char* name, float value);
	int32_t CVar_GetS32(const char* name, int32_t defaultValue);
	void CVar_SetS32(const char* name, int32_t value);
	const char* CVar_GetString(const char* name, const char* defaultValue);
	void CVar_SetString(const char* name, const char* value);
	Color_RGB8 CVar_GetRGB(const char* name, Color_RGB8 defaultValue);
	Color_RGBA8 CVar_GetRGBA(const char* name, Color_RGBA8 defaultValue);
	void CVar_SetRGBA(const char* name, Color_RGBA8 value);

	void CVar_RegisterS32(const char* name, int32_t defaultValue);
	void CVar_RegisterFloat(const char* name, float defaultValue);
	void CVar_RegisterString(const char* name, const char* defaultValue);
	void CVar_RegisterRGBA(const char* name, Color_RGBA8 defaultValue);

	void CVar_Load();
	void CVar_Save();

#ifdef __cplusplus
};
#endif

#ifdef __cplusplus
#include <map>
#include <string>
#include <memory>

extern std::map<std::string, std::unique_ptr<CVar>, std::less<>> cvars;
#endif
#endif
