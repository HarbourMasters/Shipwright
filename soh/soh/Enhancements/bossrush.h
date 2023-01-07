#pragma once

#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

#ifdef __cplusplus
extern "C" {
#endif
void BossRush_SpawnBlueWarps(PlayState* play);
void BossRush_HandleBlueWarp(DoorWarp1* warp, PlayState* play);
void BossRush_InitSave();
void BossRush_SetEquipment(uint8_t linkAge);
#ifdef __cplusplus
};
#endif
