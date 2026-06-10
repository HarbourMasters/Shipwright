#ifndef FILE_SELECT_ENHANCEMENTS_H
#define FILE_SELECT_ENHANCEMENTS_H

#include "z64.h"

#ifdef __cplusplus
extern "C" {
#endif
const char* SohFileSelect_GetRandomizerSettingText(u8 optionIndex, u8 language);
const char* SohFileSelect_GetArchipelagoSettingText(u8 optionIndex, u8 language);
const char* SohFileSelect_GetSettingText(u8 optionIndex, u8 language);
void SohFileSelect_ShowPresetModal();
#ifdef __cplusplus
};
#endif

typedef enum {
    RSM_START_RANDOMIZER,
    RSM_GENERATE_RANDOMIZER,
    RSM_OPEN_RANDOMIZER_SETTINGS,
    RSM_GENERATING,
    RSM_NO_RANDOMIZER_GENERATED,
    RSM_MAX,
} RandomizerSettingsMenuEnums;

typedef enum {
    ASM_START_ARCHIPELAGO,
    ASM_CHANGE_CONNECTION_INFO,
    ASM_SERVER_ADDRESS,
    ASM_SLOT_NAME,
    ASM_NOT_CONNECTED,
    ASM_CONNECTING,
    ASM_CONNECTED,
    ASM_LOADING_DATA,
    ASM_DATA_LOADED,
    ASM_STATUS,
    ASM_CHAR_START_TO_CONNECT,
    ASM_CHAR_SELECT_CONNECTED_TO_OTHER_SLOT,
    ASM_CONNECT_AND_START_ARCHIPELAGO,
    ASM_MAX
} ArchipelagoSettingsMenuEnums;

#endif
