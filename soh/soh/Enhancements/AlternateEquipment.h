#pragma once

#include <global.h>
#include <objects/object_link_boy/object_link_boy.h>
#include <objects/object_link_child/object_link_child.h>
#include <soh_assets.h>
#include "soh/frame_interpolation.h"

#ifdef __cplusplus
extern "C" {
#endif

// Alternate Equipment "CanUse" Functions
uint8_t AlternateEquipment_CanUseNewLoadingMethodLeftHand(Player* player);
uint8_t AlternateEquipment_CanUseNewLoadingMethodRightHand(Player* player);
uint8_t AlternateEquipment_CanUseNewLoadingMethodLeftHandPause(Player* player);
uint8_t AlternateEquipment_CanUseNewLoadingMethodRightHandPause(Player* player);
uint8_t AlternateEquipment_CanUseNewLoadingMethodFirstPerson(Player* player);
uint8_t AlternateEquipment_CanUseNewLoadingMethodSheathSword(Player* player);
uint8_t AlternateEquipment_CanUseNewLoadingMethodSheathShield(Player* player);

// Alternate Equipment Draw Functions
void AlternateEquipment_DrawChildItem(PlayState* play, Gfx* dlist);
void AlternateEquipment_DrawAdultItem(PlayState* play, Gfx* dlist);
void AlternateEquipment_DrawRightHandItem(PlayState* play, Gfx* dlist);
void AlternateEquipment_DrawOcarinaItem(PlayState* play, Gfx* dlist);

#ifdef __cplusplus
}
#endif