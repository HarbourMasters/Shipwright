#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "functions.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipHealingMikau() {
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_30GYOSON) { // Great Bay Coast
            if (*csId == 13) {                       // Played Song of Healing for Mikau
                // Transition to Link bowing at Mikau's grave
                GameInteractor::Instance->events.emplace_back(GIEventTransition{
                    .entrance = ENTRANCE(GREAT_BAY_COAST, 10),
                    .cutsceneIndex = 0,
                    .transitionTrigger = TRANS_TRIGGER_START,
                    .transitionType = TRANS_TYPE_FADE_BLACK,
                });
                /*
                 * This scene entrance has no transition effect, so assign gSaveContext.nextTransitionType manually
                 * to prevent the snap from black to the scene.
                 */
                gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
                if (GameInteractor_Should(VB_GIVE_ITEM_FROM_DMCHAR05, true, ITEM_MASK_ZORA)) {
                    GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                        .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                        .param = GID_MASK_ZORA,
                        .giveItem =
                            [](Actor* actor, PlayState* play) {
                                if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                                    CustomMessage::SetActiveCustomMessage("You received the Zora Mask!",
                                                                          { .textboxType = 2 });
                                } else {
                                    CustomMessage::StartTextbox("You received the Zora Mask!\x1C\x02\x10",
                                                                { .textboxType = 2 });
                                }
                            },
                    });
                    // Give item immediately instead of queuing it so that the gravestone and Mikau spawns behave
                    Item_Give(gPlayState, ITEM_MASK_ZORA);
                }
                *should = false;
            } else if (*csId == 15) { // Link bows at Mikau's grave
                *should = false;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipHealingMikau, { CVAR_NAME });
