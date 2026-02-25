#pragma once

#include <stdint.h>

struct PlayState;
struct Player;

typedef struct ExternalModsPauseCellInfo {
    int32_t occupied;
    int32_t absoluteIndex;
    const uint8_t* iconRgba32;
    const char* displayName;
    uint8_t assignableButtonsMask;
    uint8_t assignedButtonsMask;
} ExternalModsPauseCellInfo;

typedef enum ExternalModsAimCameraContext {
    EXTERNAL_MODS_AIM_CONTEXT_CUP = 0,
    EXTERNAL_MODS_AIM_CONTEXT_BOW = 1,
    EXTERNAL_MODS_AIM_CONTEXT_HOOKSHOT = 2,
    EXTERNAL_MODS_AIM_CONTEXT_SLINGSHOT = 3,
    EXTERNAL_MODS_AIM_CONTEXT_BOOMERANG = 4,
} ExternalModsAimCameraContext;

typedef enum ExternalModsAimSelectSlotPressResult {
    EXTERNAL_MODS_AIM_SELECT_SLOT_NONE = 0,
    EXTERNAL_MODS_AIM_SELECT_SLOT_ACTIVATED = 1,
    EXTERNAL_MODS_AIM_SELECT_SLOT_DEACTIVATED_CONSUMED = 2,
} ExternalModsAimSelectSlotPressResult;

#ifdef __cplusplus
extern "C" {
#endif

int32_t ExternalMods_GetVirtualInventoryPageCount(void);
int32_t ExternalMods_GetVirtualInventoryCellCount(void);
int32_t ExternalMods_GetVirtualInventoryPageCell(int32_t pageIndex, int32_t cellIndex,
                                                 ExternalModsPauseCellInfo* outInfo);
int32_t ExternalMods_MoveVirtualInventoryCell(int32_t fromAbsoluteIndex, int32_t toAbsoluteIndex);
int32_t ExternalMods_AssignVirtualInventoryCellToButton(int32_t absoluteIndex, int32_t buttonIndex);
void ExternalMods_OnVanillaButtonEquipped(int32_t buttonIndex);
int32_t ExternalMods_TryDrawButtonOverrideIcon(struct PlayState* play, int32_t buttonIndex, int32_t alpha);
int32_t ExternalMods_DrawSurfBoardIfActive(struct PlayState* play, struct Player* player);
int32_t ExternalMods_OnHammerGroundImpact(struct PlayState* play, struct Player* player, float impactX, float impactY,
                                          float impactZ);
int32_t ExternalMods_DrawAimReticleIfActive(struct PlayState* play, struct Player* player, int32_t context);
int32_t ExternalMods_ResolveAimCameraMode(struct PlayState* play, struct Player* player, int32_t defaultMode,
                                          int32_t context);
int32_t ExternalMods_IsAimMouseFireHeld(struct PlayState* play, struct Player* player, int32_t heldItemAction);
int32_t ExternalMods_HandleAimSelectSlotPress(struct PlayState* play, struct Player* player, int32_t buttonIndex,
                                              int32_t itemId);
int32_t ExternalMods_IsAimAttackButtonFireEnabled(struct PlayState* play, struct Player* player);
int32_t ExternalMods_IsAimOverShoulderEnabled(void);
int32_t ExternalMods_HasCustomEquippedSlingshotModel(void);
int32_t ExternalMods_DrawCustomEquippedSlingshotModel(struct PlayState* play);
int32_t ExternalMods_IsPlayerFreezeNoDamageActive(struct Player* player);

#ifdef __cplusplus
}
#endif
