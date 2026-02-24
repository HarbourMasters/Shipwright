#pragma once

#include <stdint.h>

struct PlayState;

typedef struct ExternalModsPauseCellInfo {
    int32_t occupied;
    int32_t absoluteIndex;
    const uint8_t* iconRgba32;
    const char* displayName;
    uint8_t assignableButtonsMask;
    uint8_t assignedButtonsMask;
} ExternalModsPauseCellInfo;

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

#ifdef __cplusplus
}
#endif
