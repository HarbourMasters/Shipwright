#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// This is a song tutorial, so the skip is forced on in rando for now
void RegisterSkipLearningSongOfStorms() {
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR || IS_RANDO, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_HAKASHITA && *csId == 13) { // Z-Target Flat's tombstone Beneath the Graveyard
            if (IS_RANDO) {
                RANDO_SAVE_CHECKS[RC_BENEATH_THE_GRAVEYARD_SONG_OF_STORMS].eligible = true;
            } else {
                GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                    .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                    .giveItem =
                        [](Actor* actor, PlayState* play) {
                            if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                                CustomMessage::SetActiveCustomMessage("You learned the Song of Storms!",
                                                                      { .textboxType = 2 });
                            } else {
                                CustomMessage::StartTextbox("You learned the Song of Storms!\x1C\x02\x10",
                                                            { .textboxType = 2 });
                            }
                            Item_Give(gPlayState, ITEM_SONG_STORMS);
                        },
                    .drawItem =
                        [](Actor* actor, PlayState* play) {
                            Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                            Rando::DrawItem(RI_SONG_STORMS);
                        } });
            }
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipLearningSongOfStorms, { CVAR_NAME, "IS_RANDO" });
