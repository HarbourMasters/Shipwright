#pragma once

#include "BossRushTypes.h"
#include "variables.h"

#ifdef __cplusplus
extern "C" {
#endif
void BossRush_SpawnBlueWarps(PlayState* play);
void BossRush_HandleBlueWarp(PlayState* play, f32 warpPosX, f32 warpPosZ);
void BossRush_HandleBlueWarpHeal(PlayState* play);
void BossRush_InitSave();
void BossRush_SetEquipment(uint8_t linkAge);
void BossRush_HandleCompleteBoss(PlayState* play);
const char* BossRush_GetSettingName(uint8_t optionIndex, uint8_t language);
const char* BossRush_GetSettingChoiceName(uint8_t optionIndex, uint8_t choiceIndex, uint8_t language);
uint8_t BossRush_GetSettingOptionsAmount(uint8_t optionIndex);
#ifdef __cplusplus
};
#endif
