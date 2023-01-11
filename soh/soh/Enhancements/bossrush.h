#pragma once

#include "variables.h"

#ifdef __cplusplus
extern "C" {
#endif
void BossRush_SpawnBlueWarps(PlayState* play);
void BossRush_HandleBlueWarp(PlayState* play, f32 warpPosX, f32 warpPosZ);
void BossRush_InitSave();
void BossRush_SetEquipment(uint8_t linkAge);
void BossRush_HandleCompleteBoss(PlayState* play);
#ifdef __cplusplus
};
#endif
