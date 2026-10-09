#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipLearningSongOfTime() {
    COND_VB_SHOULD(VB_PLAY_SONG_OF_TIME_CS, CVAR, {
        if (!*should) {
            return;
        }

        *should = false;
        // This typically gets set in the cutscene
        gSaveContext.save.playerForm = PLAYER_FORM_DEKU;
        GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
            .showGetItemCutscene = true,
            .param = GID_MASK_DEKU,
            .giveItem =
                [](Actor* actor, PlayState* play) {
                    if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                        CustomMessage::SetActiveCustomMessage("You received the Song of Time!", { .textboxType = 2 });
                    } else {
                        CustomMessage::StartTextbox("You received the Song of Time!\x1C\x02\x10", { .textboxType = 2 });
                    }
                    Item_Give(gPlayState, ITEM_SONG_TIME);
                },
            .drawItem =
                [](Actor* actor, PlayState* play) {
                    Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                    Rando::DrawItem(RI_SONG_TIME);
                } });
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipLearningSongOfTime, { CVAR_NAME });
