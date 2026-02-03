/**
 * extended_inventory.c - Extended inventory system implementation
 *
 * Manages custom items in a second inventory page (slots 24-47).
 * Handles icon lookups, age requirements, and page switching.
 */

#include "extended_inventory.h"
#include "z64.h"
#include <string.h>
#include "items/custom_icons.c"
extern void* gItemIcons[];
extern uint8_t gItemSlots[];
extern const unsigned char gItemIconRocsFeatherTex[];
extern const unsigned char gItemIconRocsCapeTex[];
extern const unsigned char gItemIconDesireSensorTex[];
extern const unsigned char gItemIconHyliaGraceTex[];
extern const unsigned char gItemIconZonaiPermafrostTex[];
extern const unsigned char gItemIconDemiseDestructionTex[];
extern const unsigned char gItemIconDekuLeafTex[];
extern const unsigned char gItemIconSwitchHookTex[];
extern const unsigned char gItemIconMogmaMittsTex[];
extern const unsigned char gItemIconGustJarTex[];
extern const unsigned char gItemIconBallAndChainTex[];
extern const unsigned char gItemIconWhipTex[];
extern const unsigned char gItemIconSpinnerTex[];
extern const unsigned char gItemIconCaneOfSomariaTex[];
extern const unsigned char gItemIconDominionRodTex[];
extern const unsigned char gItemIconTimeGateTex[];
extern const unsigned char gItemIconBombArrowsTex[];
extern const unsigned char gItemIconFireRodTex[];
extern const unsigned char gItemIconIceRodTex[];
extern const unsigned char gItemIconLightRodTex[];
extern const unsigned char gItemIconBeetleTex[];
extern const unsigned char gItemIconShovelTex[];
extern const unsigned char gItemIconPending1Tex[];
extern const unsigned char gItemIconPending2Tex[];
extern const unsigned char gItemIconPending3Tex[];
static ExtendedInventoryState sExtInvState = { .currentPage = 0, .pageSwitchTimer = 0 };
// Page 2 item layout (slots 24-47)
// Note: ITEM_ROCS_FEATHER at slot 24 is progressive - becomes ITEM_ROCS_CAPE when upgraded (shares slot)
// Slot 15 (actual slot 39) now has ITEM_DESIRE_SENSOR instead of ITEM_ROCS_CAPE
const uint8_t gPage2Items[24] = { ITEM_ROCS_FEATHER,   ITEM_WHIP,          ITEM_SPINNER,        ITEM_BOMB_ARROWS,    ITEM_ROD_FIRE,      ITEM_DEMISE_DESTRUCTION,
                                  ITEM_DEKU_LEAF,      ITEM_TIME_GATE,     ITEM_BEETLE,         ITEM_SWITCH_HOOK, ITEM_ROD_ICE,       ITEM_ZONAI_PERMAFROST,
                                  ITEM_MOGMA_MITTS,    ITEM_GUST_JAR,      ITEM_BALL_AND_CHAIN, ITEM_DESIRE_SENSOR,  ITEM_ROD_LIGHT,     ITEM_HYLIAS_GRACE,
                                  ITEM_PENDING_2,      ITEM_PENDING_1,     ITEM_PENDING_3,      ITEM_CANE_OF_SOMARIA, ITEM_SHOVEL,        ITEM_DOMINION_ROD };

// Age requirements for page 2 items
// Roc's items (slot 0/24) = AGE_REQ_NONE (both adult and child can use Feather AND Cape)
// Desire Sensor (slot 15/39) = AGE_REQ_NONE (both adult and child can use)
const uint8_t gPage2ItemAgeReqs[24] = { AGE_REQ_NONE,  AGE_REQ_ADULT, AGE_REQ_ADULT, AGE_REQ_ADULT, AGE_REQ_NONE,
                                        AGE_REQ_NONE,  AGE_REQ_CHILD, AGE_REQ_NONE,  AGE_REQ_ADULT, AGE_REQ_CHILD,
                                        AGE_REQ_NONE,  AGE_REQ_NONE,  AGE_REQ_ADULT, AGE_REQ_CHILD, AGE_REQ_ADULT,
                                        AGE_REQ_NONE,  AGE_REQ_NONE,  AGE_REQ_NONE,  AGE_REQ_ADULT, AGE_REQ_ADULT,
                                        AGE_REQ_ADULT, AGE_REQ_ADULT, AGE_REQ_NONE,  AGE_REQ_NONE };
ExtendedInventoryState* ExtInv_GetState(void) {
    return &sExtInvState;
}
void ExtInv_Reset(void) {
    sExtInvState.currentPage = 0;
    sExtInvState.pageSwitchTimer = 0;
}
void ExtInv_Update(void) {
    if (sExtInvState.pageSwitchTimer > 0) {
        sExtInvState.pageSwitchTimer--;
    }
}
bool ExtInv_CanSwitchPage(void) {
    return sExtInvState.pageSwitchTimer == 0;
}
void ExtInv_SwitchPage(void) {
    sExtInvState.currentPage ^= 1;
    sExtInvState.pageSwitchTimer = 15;
}
int ExtInv_GetCurrentPage(void) {
    return sExtInvState.currentPage;
}
int ExtInv_GetInventorySlot(int visualSlot) {
    return visualSlot + (sExtInvState.currentPage * 24);
}
bool ExtInv_IsSlotOnCurrentPage(uint8_t slot) {
    int pageStart = sExtInvState.currentPage * 24;
    int pageEnd = pageStart + 23;
    return (slot >= pageStart && slot <= pageEnd);
}
int ExtInv_GetPageForSlot(uint8_t slot) {
    return (slot >= 24) ? 1 : 0;
}
uint8_t ExtInv_GetItemAgeReq(uint16_t itemId) {
    for (int i = 0; i < 24; i++) {
        if (gPage2Items[i] == itemId) {
            return gPage2ItemAgeReqs[i];
        }
    }
    return 9;
}

// External vanilla array (trimmed to 24 entries)
extern uint8_t gSlotAgeReqs[];

uint8_t ExtInv_GetSlotAgeReq(uint8_t slot) {
    // Vanilla slots (0-23) use the original array
    if (slot < 24) {
        return gSlotAgeReqs[slot];
    }
    // Custom slots (24-47) use gPage2ItemAgeReqs
    if (slot < 48) {
        return gPage2ItemAgeReqs[slot - 24];
    }
    return 9; // AGE_REQ_NONE for out-of-range
}
void* ExtInv_GetCustomItemNameTex(uint16_t itemId, uint8_t language) {
    extern const unsigned char gRocsFeatherNameTex[];
    extern const unsigned char gRocsCapeNameTex[];
    extern const unsigned char gDesireSensorNameTex[];
    extern const unsigned char gDekuLeafNameTex[];
    extern const unsigned char gSwitchHookNameTex[];
    extern const unsigned char gMogmaMittsNameTex[];
    extern const unsigned char gGustJarNameTex[];
    extern const unsigned char gBallAndChainNameTex[];
    extern const unsigned char gWhipNameTex[];
    extern const unsigned char gSpinnerNameTex[];
    extern const unsigned char gCaneOfSomariaNameTex[];
    extern const unsigned char gDominionRodNameTex[];
    extern const unsigned char gTimeGateNameTex[];
    extern const unsigned char gBombArrowsNameTex[];
    extern const unsigned char gFireRodNameTex[];
    extern const unsigned char gIceRodNameTex[];
    extern const unsigned char gLightRodNameTex[];
    extern const unsigned char gBeetleNameTex[];
    extern const unsigned char gShovelNameTex[];
    extern const unsigned char gHyliaGraceNameTex[];
    extern const unsigned char gZonaiPermafrostNameTex[];
    extern const unsigned char gDemiseDestructionNameTex[];
    extern const unsigned char gPending1NameTex[];
    extern const unsigned char gPending2NameTex[];
    extern const unsigned char gPending3NameTex[];
    switch (itemId) {
        case 0x9C:
            return (void*)gRocsFeatherNameTex;
        case 0x9D:
            return (void*)gRocsCapeNameTex;
        case 0x9E:
            return (void*)gDesireSensorNameTex;
        case 0x9F:
            return (void*)gHyliaGraceNameTex;
        case 0xA0:
            return (void*)gZonaiPermafrostNameTex;
        case 0xA1:
            return (void*)gDemiseDestructionNameTex;
        case 0xA2:
            return (void*)gDekuLeafNameTex;
        case 0xA3:
            return (void*)gSwitchHookNameTex;
        case 0xA4:
            return (void*)gMogmaMittsNameTex;
        case 0xA5:
            return (void*)gGustJarNameTex;
        case 0xA6:
            return (void*)gBallAndChainNameTex;
        case 0xA7:
            return (void*)gWhipNameTex;
        case 0xA8:
            return (void*)gSpinnerNameTex;
        case 0xA9:
            return (void*)gCaneOfSomariaNameTex;
        case 0xAA:
            return (void*)gDominionRodNameTex;
        case 0xAB:
            return (void*)gTimeGateNameTex;
        case 0xAC:
            return (void*)gBombArrowsNameTex;
        case 0xAD:
            return (void*)gFireRodNameTex;
        case 0xAE:
            return (void*)gIceRodNameTex;
        case 0xAF:
            return (void*)gLightRodNameTex;
        case 0xB0:
            return (void*)gBeetleNameTex;
        case 0xB1:
            return (void*)gShovelNameTex;
        case 0xB2:
            return (void*)gPending1NameTex;
        case 0xB3:
            return (void*)gPending2NameTex;
        case 0xB5:
            return (void*)gPending3NameTex;
        default:
            return NULL;
    }
}
void* ExtInv_GetItemIcon(uint16_t itemId) {
    if (itemId < 156) {
        return gItemIcons[itemId];
    }
    switch (itemId) {
        case 0x9C:
            return (void*)gItemIconRocsFeatherTex;
        case 0x9D:
            return (void*)gItemIconRocsCapeTex;
        case 0x9E:
            return (void*)gItemIconDesireSensorTex;
        case 0x9F:
            return (void*)gItemIconHyliaGraceTex;
        case 0xA0:
            return (void*)gItemIconZonaiPermafrostTex;
        case 0xA1:
            return (void*)gItemIconDemiseDestructionTex;
        case 0xA2:
            return (void*)gItemIconDekuLeafTex;
        case 0xA3:
            return (void*)gItemIconSwitchHookTex;
        case 0xA4:
            return (void*)gItemIconMogmaMittsTex;
        case 0xA5:
            return (void*)gItemIconGustJarTex;
        case 0xA6:
            return (void*)gItemIconBallAndChainTex;
        case 0xA7:
            return (void*)gItemIconWhipTex;
        case 0xA8:
            return (void*)gItemIconSpinnerTex;
        case 0xA9:
            return (void*)gItemIconCaneOfSomariaTex;
        case 0xAA:
            return (void*)gItemIconDominionRodTex;
        case 0xAB:
            return (void*)gItemIconTimeGateTex;
        case 0xAC:
            return (void*)gItemIconBombArrowsTex;
        case 0xAD:
            return (void*)gItemIconFireRodTex;
        case 0xAE:
            return (void*)gItemIconIceRodTex;
        case 0xAF:
            return (void*)gItemIconLightRodTex;
        case 0xB0:
            return (void*)gItemIconBeetleTex;
        case 0xB1:
            return (void*)gItemIconShovelTex;
        case 0xB2:
            return (void*)gItemIconPending1Tex;
        case 0xB3:
            return (void*)gItemIconPending2Tex;
        case 0xB5:
            return (void*)gItemIconPending3Tex;
        default:
            return gItemIcons[0];
    }
}
uint8_t ExtInv_GetItemSlot(uint16_t itemId) {
    if (itemId < 52) {
        return gItemSlots[itemId];
    }
    // Special case: ITEM_ROCS_CAPE shares slot with ITEM_ROCS_FEATHER (progressive upgrade)
    if (itemId == ITEM_ROCS_CAPE) {
        return SLOT_ROCS;  // Same as SLOT_ROCS_FEATHER (24)
    }
    for (int i = 0; i < 24; i++) {
        if (gPage2Items[i] == itemId) {
            return 24 + i;
        }
    }
    return 0xFF;
}
