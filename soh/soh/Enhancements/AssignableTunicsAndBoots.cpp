#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "variables.h"

extern s32 Player_GetItemOnButton(PlayState*, s32);
extern void Inventory_ChangeEquipment(s16, u16);
extern void Player_SetEquipmentData(PlayState*, Player*);
extern void func_808328EC(Player*, u16);
extern PlayState* gPlayState;
}

static u16 sItemButtons[] = { BTN_B, BTN_CLEFT, BTN_CDOWN, BTN_CRIGHT, BTN_DUP, BTN_DDOWN, BTN_DLEFT, BTN_DRIGHT };

void UseTunicBoots(Player* player, PlayState* play, Input* input) {
    // Boots and tunics equip despite state
    if (
        player->stateFlags1 & (PLAYER_STATE1_INPUT_DISABLED | PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD) ||
        player->stateFlags2 & PLAYER_STATE2_OCARINA_PLAYING
    ) {
        return;
    }

    s32 item = ITEM_NONE;
    for (s32 i = 0; i < ARRAY_COUNT(sItemButtons); i++) {
        if (CHECK_BTN_ALL(input->press.button, sItemButtons[i])) {
            item = Player_GetItemOnButton(play, i);
            break;
        }
    }

    if (item >= ITEM_TUNIC_KOKIRI && item <= ITEM_BOOTS_HOVER) {
        if (item >= ITEM_BOOTS_KOKIRI) {
            u16 bootsValue = item - ITEM_BOOTS_KOKIRI + 1;
            if (CUR_EQUIP_VALUE(EQUIP_TYPE_BOOTS) == bootsValue) {
                Inventory_ChangeEquipment(EQUIP_TYPE_BOOTS, EQUIP_VALUE_BOOTS_KOKIRI);
            } else {
                Inventory_ChangeEquipment(EQUIP_TYPE_BOOTS, bootsValue);
            }
            Player_SetEquipmentData(play, player);
            func_808328EC(player, CUR_EQUIP_VALUE(EQUIP_TYPE_BOOTS) == EQUIP_VALUE_BOOTS_IRON ? NA_SE_PL_WALK_HEAVYBOOTS : NA_SE_PL_CHANGE_ARMS);
        } else {
            u16 tunicValue = item - ITEM_TUNIC_KOKIRI + 1;
            if (CUR_EQUIP_VALUE(EQUIP_TYPE_TUNIC) == tunicValue) {
                Inventory_ChangeEquipment(EQUIP_TYPE_TUNIC, EQUIP_VALUE_TUNIC_KOKIRI);
            } else {
                Inventory_ChangeEquipment(EQUIP_TYPE_TUNIC, tunicValue);
            }
            Player_SetEquipmentData(play, player);
            func_808328EC(player, NA_SE_PL_CHANGE_ARMS);
        }
    }
}

void ClearAssignedTunicsBoots(int32_t unused = 0) {
    for (int32_t buttonIndex = 0; buttonIndex < 8; buttonIndex++) {
        int32_t item = gSaveContext.equips.buttonItems[buttonIndex];

        if (item >= ITEM_TUNIC_KOKIRI && item <= ITEM_BOOTS_HOVER) {
            gSaveContext.equips.buttonItems[buttonIndex] = ITEM_NONE;
        }
    }
}

#define CVAR_TUNICBOOTS_NAME CVAR_ENHANCEMENT("AssignableTunicsAndBoots")
#define CVAR_TUNICBOOTS_DEFAULT 0
#define CVAR_TUNICBOOTS_VALUE CVarGetInteger(CVAR_TUNICBOOTS_NAME, CVAR_TUNICBOOTS_DEFAULT)

void RegisterAssignableTunicsBoots() {
    // make sure we don't change our held/equipped item when changing tunics/boots
    COND_VB_SHOULD(VB_CHANGE_HELD_ITEM_AND_USE_ITEM, CVAR_TUNICBOOTS_VALUE != CVAR_TUNICBOOTS_DEFAULT, {
        int32_t item = va_arg(args, int32_t);

        if (item >= ITEM_TUNIC_KOKIRI && item <= ITEM_BOOTS_HOVER) {
            *should = false;
        }
    });

    // make sure we don't crash because tunics/boots don't have assoicated item actions
    COND_VB_SHOULD(VB_ITEM_ACTION_BE_NONE, CVAR_TUNICBOOTS_VALUE != CVAR_TUNICBOOTS_DEFAULT, {
        int32_t item = va_arg(args, int32_t);

        if (item >= ITEM_TUNIC_KOKIRI && item <= ITEM_BOOTS_HOVER) {
            *should = true;
        }
    });

    // do something when the player presses a button to use the tunics/boots
    COND_VB_SHOULD(VB_EXECUTE_PLAYER_ACTION_FUNC, CVAR_TUNICBOOTS_VALUE != CVAR_TUNICBOOTS_DEFAULT, {
        // if the vanilla condition doesn't want us to run the actionFunc, don't do any of this
        if (!*should) {
            return;
        }

        Player* player = va_arg(args, Player*);

        // if we're not dealing with the actual player, we're probably
        // dealing with dark link, don't do any tunics/boots stuff, just
        // run the vanilla action func
        if (player->actor.id != ACTOR_PLAYER) {
            return;
        }

        *should = false;

        Input* input = va_arg(args, Input*);

        player->actionFunc(player, gPlayState);
        UseTunicBoots(player, gPlayState, input);
    });

    // clear out assigned tunics/boots when the enhancement is toggled off
    if (GameInteractor::IsSaveLoaded(true) && CVAR_TUNICBOOTS_VALUE == CVAR_TUNICBOOTS_DEFAULT) {
        ClearAssignedTunicsBoots();
    }

    // clear out assigned tunics/boots when loading a save with enhancement turned off
    COND_HOOK(OnLoadGame, CVAR_TUNICBOOTS_VALUE == CVAR_TUNICBOOTS_DEFAULT, ClearAssignedTunicsBoots);
}

static RegisterShipInitFunc initFunc(RegisterAssignableTunicsBoots, { CVAR_TUNICBOOTS_NAME });
