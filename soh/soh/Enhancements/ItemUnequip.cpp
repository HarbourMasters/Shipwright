#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64.h"
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "overlays/misc/ovl_kaleido_scope/z_kaleido_scope.h"
extern SaveContext gSaveContext;
}

#define CVAR_ITEM_UNEQUIP_NAME CVAR_ENHANCEMENT("ItemUnequip")
#define CVAR_ITEM_UNEQUIP_DEFAULT 0
#define CVAR_ITEM_UNEQUIP_VALUE CVarGetInteger(CVAR_ITEM_UNEQUIP_NAME, CVAR_ITEM_UNEQUIP_DEFAULT)

void RegisterItemUnequip() {
    COND_VB_SHOULD(VB_EQUIP_ITEM_TO_C_BUTTON, CVAR_ITEM_UNEQUIP_VALUE, {
        PlayState* play = va_arg(args, PlayState*);
        u16 cursorSlot = va_arg(args, int);
        u16 cursorItem = va_arg(args, int);

        Input* input = &play->state.input[0];

        int targetButton = -1;

        if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
            targetButton = 1;
        } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
            targetButton = 2;
        } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
            targetButton = 3;
        } else if (CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0)) {
            if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
                targetButton = 4;
            } else if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
                targetButton = 5;
            } else if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT)) {
                targetButton = 6;
            } else if (CHECK_BTN_ALL(input->press.button, BTN_DRIGHT)) {
                targetButton = 7;
            }
        }

        if (targetButton == -1) {
            return;
        }

        u8 equippedItem = gSaveContext.equips.buttonItems[targetButton];
        u8 equippedSlot = gSaveContext.equips.cButtonSlots[targetButton - 1];
        bool shouldUnequip = false;

        if (equippedItem == cursorItem) {
            if (cursorItem >= ITEM_BOTTLE && cursorItem <= ITEM_POE) {
                if (equippedSlot == cursorSlot) {
                    shouldUnequip = true;
                }
            } else {
                shouldUnequip = true;
            }
        } else if (cursorItem == ITEM_ARROW_FIRE && equippedItem == ITEM_BOW_ARROW_FIRE) {
            shouldUnequip = true;
        } else if (cursorItem == ITEM_ARROW_ICE && equippedItem == ITEM_BOW_ARROW_ICE) {
            shouldUnequip = true;
        } else if (cursorItem == ITEM_ARROW_LIGHT && equippedItem == ITEM_BOW_ARROW_LIGHT) {
            shouldUnequip = true;
        }

        if (shouldUnequip) {
            gSaveContext.equips.buttonItems[targetButton] = ITEM_NONE;
            gSaveContext.equips.cButtonSlots[targetButton - 1] = SLOT_NONE;
            Interface_LoadItemIcon1(play, targetButton);

            Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterItemUnequip, { CVAR_ITEM_UNEQUIP_NAME });
