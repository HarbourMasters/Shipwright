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

void RegisterSkipLearningSonataOfAwakening() {
    /*
     * Forced on for rando for now. If this ever changes, look at the Item_Give calls in
     * Message_DrawMain. The player actually "learns" the song upon hitting the final correct note in
     * the tutorial prompt.
     */
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR || IS_RANDO, {
        s16* csId = va_arg(args, s16*);
        // Cutscenes 11 and 12 in the Deku Palace King's chamber play when Link pulls out Deku Pipes for the monkey.
        if (gPlayState->sceneId == SCENE_DEKU_KING) {
            if (*csId == 11) {
                *should = false;
            } else if (*csId == 12) {
                if (GameInteractor_Should(VB_GIVE_ITEM_FROM_MNK, true)) {
                    GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                        .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                        .giveItem =
                            [](Actor* actor, PlayState* play) {
                                if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                                    CustomMessage::SetActiveCustomMessage("You learned the Sonata of Awakening!",
                                                                          { .textboxType = 2 });
                                } else {
                                    CustomMessage::StartTextbox("You learned the Sonata of Awakening!\x1C\x02\x10",
                                                                { .textboxType = 2 });
                                }
                                Item_Give(gPlayState, ITEM_SONG_SONATA);
                            },
                        .drawItem =
                            [](Actor* actor, PlayState* play) {
                                Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                                Rando::DrawItem(RI_SONG_SONATA);
                            } });
                }
                gPlayState->nextEntrance = ENTRANCE(DEKU_PALACE, 1);
                gPlayState->transitionType = TRANS_TYPE_64;
                gSaveContext.nextTransitionType = TRANS_TYPE_64;
                gPlayState->transitionTrigger = TRANS_TRIGGER_START;
                SET_WEEKEVENTREG(WEEKEVENTREG_09_80); // Boil the monkey
                CLEAR_EVENTINF(EVENTINF_24);          // Stop forcing instrument to be Deku Pipes
                *should = false;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipLearningSonataOfAwakening, { CVAR_NAME, "IS_RANDO" });
