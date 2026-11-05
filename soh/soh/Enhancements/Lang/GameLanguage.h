#pragma once

#include "LanguagePolicy.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// The port's boundary to the original save-language byte. Rendering and UI
// callers should not read or write gSaveContext.language directly.
uint8_t GameLanguage_Get(void);
uint8_t GameLanguage_GetConfigured(void);
uint8_t GameLanguage_GetLegacyText(void);
// Optional port-only locale override; absent selection follows Language.
const char* GameLanguage_GetMessageLocale(void);
bool GameLanguage_SetMessageLocale(const char* locale);
bool GameLanguage_IsAvailable(int32_t language);
void GameLanguage_Set(int32_t language);

#ifdef __cplusplus
}
#endif
