#include "GameLanguage.h"
#include "TranslationCatalog.h"
#include "soh/Enhancements/custom-message/GameText.h"

#include <libultraship/bridge/consolevariablebridge.h>
#include "soh/cvar_prefixes.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

extern "C" {
#include "variables.h"
#include "message_data_static.h"
extern MessageTableEntry* sNesMessageEntryTablePtr;
extern MessageTableEntry* sGerMessageEntryTablePtr;
extern MessageTableEntry* sFraMessageEntryTablePtr;
extern MessageTableEntry* sJpnMessageEntryTablePtr;
}

static_assert(LANGUAGE_ENG == 0 && LANGUAGE_GER == 1 && LANGUAGE_FRA == 2 && LANGUAGE_JPN == 3 && LANGUAGE_MAX == 4,
              "Update the language policy when the original resource/save slots change");

uint8_t GameLanguage_Get(void) {
    return LanguagePolicy_Normalize(gSaveContext.language);
}

uint8_t GameLanguage_GetConfigured(void) {
    return LanguagePolicy_Normalize(CVarGetInteger(CVAR_SETTING("Languages"), LANGUAGE_ENG));
}

uint8_t GameLanguage_GetLegacyText(void) {
    return LanguagePolicy_LegacyText(GameLanguage_Get());
}

const char* GameLanguage_GetMessageLocale(void) {
    const char* locale = CVarGetString(CVAR_SETTING("MessageLanguage"), "");
    return CustomMessages::GetGameTextCatalog().HasLanguage(locale) ? locale
                                                                    : LanguagePolicy_Locale(GameLanguage_Get());
}

bool GameLanguage_SetMessageLocale(const char* locale) {
    if (locale == nullptr || (*locale && !CustomMessages::GetGameTextCatalog().HasLanguage(locale)))
        return false;
    CVarSetString(CVAR_SETTING("MessageLanguage"), locale);
    CVarSave();
    return true;
}

bool GameLanguage_IsAvailable(int32_t language) {
    switch (language) {
        case LANGUAGE_ENG:
            return sNesMessageEntryTablePtr != nullptr;
        case LANGUAGE_GER:
            return sGerMessageEntryTablePtr != nullptr;
        case LANGUAGE_FRA:
            return sFraMessageEntryTablePtr != nullptr;
        case LANGUAGE_JPN:
            return sJpnMessageEntryTablePtr != nullptr;
        default:
            return false;
    }
}

void GameLanguage_Set(int32_t language) {
    const uint8_t normalized = LanguagePolicy_Normalize(language);
    const bool changed = gSaveContext.language != normalized;
    gSaveContext.language = normalized;
    CVarSetInteger(CVAR_SETTING("Languages"), normalized);
    CVarSetString(CVAR_SETTING("MessageLanguage"), "");
    CVarSave();
    if (changed) {
        GameInteractor_ExecuteOnSetGameLanguage();
    }
}
