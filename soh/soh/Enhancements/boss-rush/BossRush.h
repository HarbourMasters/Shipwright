#pragma once

#include "z64.h"

#ifdef __cplusplus
extern "C" {
#endif
    void BossRush_HandleBlueWarpHeal(PlayState* play);
    void BossRush_InitSave();
    const char* BossRush_GetSettingName(u8 optionIndex, u8 language);
    const char* BossRush_GetSettingChoiceName(u8 optionIndex, u8 choiceIndex, u8 language);
    u8 BossRush_GetSettingOptionsAmount(u8 optionIndex);
    void BossRush_RegisterHooks();
#ifdef __cplusplus
};
#endif
