#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Time_Tag/z_en_time_tag.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)
#define ENGRAVING_TEXT_ID 0xC02

void RegisterSkipLearningSongOfSoaring() {
    /*
     * Whether the song prompt cutscene is triggered or not depends on the actor's textId, which itself is initially
     * determined by whether the player has obtained the Song of Soaring or not. We bypass the cutscene by always
     * setting this textId.
     */
    COND_ID_HOOK(OnActorInit, ACTOR_EN_TIME_TAG, CVAR || IS_RANDO, [](Actor* actor) {
        if (TIMETAG_GET_TYPE(actor) == TIMETAG_SOARING_ENGRAVING) {
            actor->textId = ENGRAVING_TEXT_ID;
        }
    });

    // Then, once this textId is opened for the first time, give the player the reward. (unless we're in rando)
    COND_ID_HOOK(OnOpenText, ENGRAVING_TEXT_ID, CVAR && !IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        if (!CHECK_QUEST_ITEM(QUEST_SONG_SOARING)) {
            GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                .giveItem =
                    [](Actor* actor, PlayState* play) {
                        if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                            CustomMessage::SetActiveCustomMessage("You learned the Song of Soaring!",
                                                                  { .textboxType = 2 });
                        } else {
                            CustomMessage::StartTextbox("You learned the Song of Soaring!\x1C\x02\x10",
                                                        { .textboxType = 2 });
                        }
                        Item_Give(gPlayState, ITEM_SONG_SOARING);
                    },
                .drawItem =
                    [](Actor* actor, PlayState* play) {
                        Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                        Rando::DrawItem(RI_SONG_SOARING);
                    } });
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipLearningSongOfSoaring, { CVAR_NAME, "IS_RANDO" });
