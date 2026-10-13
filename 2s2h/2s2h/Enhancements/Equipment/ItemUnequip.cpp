#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h_assets.h"

extern "C" {
#include "z64.h"
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
}

#define CVAR_DPAD_NAME "gEnhancements.Dpad.DpadEquips"
#define CVAR_DPAD CVarGetInteger(CVAR_DPAD_NAME, 0)

extern bool IsBombArrowButton(s32 slot, bool isDpad);
extern void SetBombArrowButton(s32 slot, bool state, bool isDpad);
extern bool sBombSlotIsBombArrowMode;

#define CVAR_NAME "gEnhancements.Equipment.ItemUnequip"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterDpadPageSwitchPrevention() {
    COND_VB_SHOULD(VB_KALEIDO_SWITCH_PAGE_WITH_DPAD, CVAR_DPAD, {
        u16 button = va_arg(args, int);
        Input* input = &gPlayState->state.input[0];

        if (CHECK_BTN_ALL(input->cur.button, button)) {
            PauseContext* pauseCtx = &gPlayState->pauseCtx;

            // Prevent page switching with D-pad when on item or mask page
            if ((pauseCtx->pageIndex == PAUSE_ITEM || pauseCtx->pageIndex == PAUSE_MASK) &&
                pauseCtx->mainState <= PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG) {
                *should = false;
            }
        }
    });
}

void RegisterItemUnequip() {
    COND_VB_SHOULD(VB_KALEIDO_EQUIP_ITEM_TO_BUTTON, CVAR, {
        u16 cursorSlot = va_arg(args, int);
        u16 cursorItem = va_arg(args, int);

        PauseContext* pauseCtx = &gPlayState->pauseCtx;
        s32 targetSlot = -1;
        bool isDpad = false;

        // Determine which button was pressed based on equipTargetCBtn
        switch (pauseCtx->equipTargetCBtn) {
            case PAUSE_EQUIP_C_LEFT:
                targetSlot = EQUIP_SLOT_C_LEFT;
                break;
            case PAUSE_EQUIP_C_DOWN:
                targetSlot = EQUIP_SLOT_C_DOWN;
                break;
            case PAUSE_EQUIP_C_RIGHT:
                targetSlot = EQUIP_SLOT_C_RIGHT;
                break;
            case PAUSE_EQUIP_D_RIGHT:
                targetSlot = EQUIP_SLOT_D_RIGHT;
                isDpad = true;
                break;
            case PAUSE_EQUIP_D_LEFT:
                targetSlot = EQUIP_SLOT_D_LEFT;
                isDpad = true;
                break;
            case PAUSE_EQUIP_D_DOWN:
                targetSlot = EQUIP_SLOT_D_DOWN;
                isDpad = true;
                break;
            case PAUSE_EQUIP_D_UP:
                targetSlot = EQUIP_SLOT_D_UP;
                isDpad = true;
                break;
            default:
                return;
        }

        u8 equippedItem;
        u8 equippedSlot;
        bool shouldUnequip = false;
        bool isMask = cursorSlot >= ITEM_NUM_SLOTS;

        // C-buttons vs D-pad
        if (!isDpad) {
            equippedItem = BUTTON_ITEM_EQUIP(0, targetSlot);
            equippedSlot = C_SLOT_EQUIP(0, targetSlot);
        } else {
            equippedItem = DPAD_BUTTON_ITEM_EQUIP(0, targetSlot);
            equippedSlot = DPAD_SLOT_EQUIP(0, targetSlot);
        }

        bool isBombArrow = IsBombArrowButton(targetSlot, isDpad);

        if (equippedItem == cursorItem || (equippedItem == ITEM_BOW && cursorItem == ITEM_BOMB && isBombArrow)) {
            if (isBombArrow) {
                if (cursorItem == ITEM_BOMB && sBombSlotIsBombArrowMode) {
                    shouldUnequip = true;
                }
            } else if (cursorItem != ITEM_BOMB || !sBombSlotIsBombArrowMode) {
                shouldUnequip = true;
            }

            if (shouldUnequip) {
                if (isMask && equippedSlot != cursorSlot) {
                    shouldUnequip = false;
                } else if (cursorItem >= ITEM_BOTTLE && cursorItem <= ITEM_OBABA_DRINK && equippedSlot != cursorSlot) {
                    shouldUnequip = false;
                }
            }
        } else if (cursorItem == ITEM_BOMB && sBombSlotIsBombArrowMode && isBombArrow) {
            shouldUnequip = true;
        }
        // Handle magic arrows (bow variants)
        else if (cursorItem == ITEM_ARROW_FIRE && equippedItem == ITEM_BOW_FIRE) {
            shouldUnequip = true;
        } else if (cursorItem == ITEM_ARROW_ICE && equippedItem == ITEM_BOW_ICE) {
            shouldUnequip = true;
        } else if (cursorItem == ITEM_ARROW_LIGHT && equippedItem == ITEM_BOW_LIGHT) {
            shouldUnequip = true;
        } else if (cursorItem == ITEM_BOW &&
                   (equippedItem == ITEM_BOW_FIRE || equippedItem == ITEM_BOW_ICE || equippedItem == ITEM_BOW_LIGHT)) {
            shouldUnequip = true;
        }

        if (shouldUnequip) {
            if (!isDpad) {
                // C-buttons
                BUTTON_ITEM_EQUIP(0, targetSlot) = ITEM_NONE;
                C_SLOT_EQUIP(0, targetSlot) = SLOT_NONE;
                Interface_LoadItemIconImpl(gPlayState, targetSlot);
            } else {
                // D-pad
                DPAD_BUTTON_ITEM_EQUIP(0, targetSlot) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, targetSlot) = SLOT_NONE;
                // Manually clear D-pad icon
                gPlayState->interfaceCtx.iconItemSegment[DPAD_BUTTON(targetSlot) + EQUIP_SLOT_MAX] =
                    (char*)gEmptyTexture;
            }

            SetBombArrowButton(targetSlot, false, isDpad);

            Audio_PlaySfx(NA_SE_SY_DECIDE);
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterItemUnequip, { CVAR_NAME });
static RegisterShipInitFunc initDpadPageSwitch(RegisterDpadPageSwitchPrevention, { CVAR_DPAD_NAME });