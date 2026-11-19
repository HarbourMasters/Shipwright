#pragma once

#include <libultraship/libultra/types.h>

#ifdef __cplusplus
extern "C" {
#endif
struct GraphicsContext;
const char* SohFileSelect_GetSettingText(u8 optionIndex);
void SohFileSelect_DrawSettingText(struct GraphicsContext* gfx, u8 optionIndex, s16 x, s16 y, u8 red, u8 green, u8 blue,
                                   u8 alpha, float scale, bool shadow);
void SohFileSelect_ShowPresetModal();
bool SohFileSelect_IsQuestHidden(u8 quest);
u8 SohFileSelect_CountVisibleQuests();
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
