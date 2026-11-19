#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Message-local state, not a saved setting. Initialize before OnOpenText hooks,
// which can then declare the actual encoding of the buffer they supply.
typedef enum {
    MESSAGE_ALIGNMENT_AUTO, // Center ordinary added text; left-align choices.
    MESSAGE_ALIGNMENT_LEFT,
    MESSAGE_ALIGNMENT_CENTER,
    MESSAGE_ALIGNMENT_RIGHT,
} MessageAlignment;

void MessageLanguage_Begin(int32_t requestedLanguage);
void MessageLanguage_Set(int32_t actualLanguage);
void MessageLanguage_SetUnicode(int32_t actualLanguage);
bool MessageLanguage_UsesUnicode(void);
// Reset by Begin/Set/SetUnicode. Never stored in gSaveContext or applied to ROM text.
void MessageLanguage_SetAlignment(MessageAlignment alignment);
MessageAlignment MessageLanguage_GetAlignment(void);
void MessageLanguage_UseLegacyFallback(void);
uint8_t MessageLanguage_Get(void);
bool MessageLanguage_IsJapanese(void);
// Japanese geometry applies to both native ROM text and custom UTF-8 text.
bool MessageLanguage_UsesJapaneseLayout(void);
bool MessageLanguage_IsEnglishFallback(void);

#ifdef __cplusplus
}
#endif
