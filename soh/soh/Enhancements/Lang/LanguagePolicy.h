#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// The original save/resource ABI has four slots. New locale catalogs must not
// be used as unchecked indices into those tables.
uint8_t LanguagePolicy_Normalize(int32_t language);
uint8_t LanguagePolicy_LegacyText(int32_t language);
const char* LanguagePolicy_Locale(int32_t language);
uint8_t LanguagePolicy_FromLocale(const char* locale);

#ifdef __cplusplus
}
#endif
