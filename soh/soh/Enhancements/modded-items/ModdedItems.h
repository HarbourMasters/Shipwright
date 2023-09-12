#pragma once
#include "z64.h"

#ifdef __cplusplus
extern "C" {
#endif

void ModdedItems_ExecuteModdedItemAction(PlayState* play, Player* player, s32 modId, s32 itemId);
void* ModdedItems_GetModdedItemIcon(s32 modId, s32 itemId);
void* ModdedItems_GetModdedItemNameTexture(s32 modId, s32 itemId);

#ifdef __cplusplus
}
#endif