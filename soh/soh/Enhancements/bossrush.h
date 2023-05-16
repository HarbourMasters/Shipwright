#pragma once

#include "variables.h"

#define BOSSRUSH_OPTIONS_AMOUNT 8
#define BOSSRUSH_MAX_OPTIONS_ON_SCREEN 6

#ifdef __cplusplus
extern "C" {
#endif
void BossRush_SpawnBlueWarps(PlayState* play);
void BossRush_HandleBlueWarp(PlayState* play, f32 warpPosX, f32 warpPosZ);
void BossRush_InitSave();
void BossRush_SetEquipment(uint8_t linkAge);
void BossRush_HandleCompleteBoss(PlayState* play);
const char* BossRush_GetSettingName(uint8_t optionIndex, uint8_t language);
const char* BossRush_GetSettingChoiceName(uint8_t optionIndex, uint8_t choiceIndex, uint8_t language);
uint8_t BossRush_GetSettingOptionsAmount(uint8_t optionIndex);
#ifdef __cplusplus
};
#endif
