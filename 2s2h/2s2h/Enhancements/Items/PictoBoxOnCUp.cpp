#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "z64player.h"
extern Input* sPlayerControlInput;
s32 func_80831814(Player* player, PlayState* play, PlayerUnkAA5 arg2);
}

#define CVAR_NAME "gEnhancements.Items.PictoBoxOnCUp"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

/**
 * Enhancement: Opens the Pictograph Box with C-Up once acquired
 *
 * This enhancement makes C-Up open the Pictograph Box instead of entering
 * normal first-person camera mode when the player has acquired the Picto Box
 */

void RegisterPictoBoxOnCUp() {
    COND_VB_SHOULD(VB_FIRST_PERSON_CAMERA, CVAR, {
        *should = false;

        Player* player = GET_PLAYER(gPlayState);
        PlayerUnkAA5 firstPersonMode = PLAYER_UNKAA5_1;

        if (gSaveContext.save.saveInfo.inventory.items[SLOT_PICTOGRAPH_BOX] == ITEM_PICTOGRAPH_BOX &&
            !CHECK_QUEST_ITEM(QUEST_PICTOGRAPH)) {
            firstPersonMode = PLAYER_UNKAA5_2;
        }

        if (player->tatlTextId == 0 && !Player_CheckHostileLockOn(player) &&
            CHECK_BTN_ALL(sPlayerControlInput->press.button, BTN_CUP) &&
            !func_80831814(player, gPlayState, firstPersonMode)) {
            Audio_PlaySfx(NA_SE_SY_ERROR);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterPictoBoxOnCUp, { CVAR_NAME });
