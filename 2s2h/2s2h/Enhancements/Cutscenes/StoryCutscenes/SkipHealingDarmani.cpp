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

void RegisterSkipHealingDarmani() {
    /*
     * Use the cutscene queue hook here. This allows us to kill the actor while it is still invisible. Using the
     * cutscene start hook would result in 1 frame where the actor becomes visible before disappearing again.
     */
    COND_VB_SHOULD(VB_QUEUE_CUTSCENE, CVAR, {
        s16* csId = va_arg(args, s16*);
        // Played Song of Healing for Darmani in Goron Graveyard
        if (gPlayState->sceneId == SCENE_GORON_HAKA && *csId == 9) {
            if (GameInteractor_Should(VB_GIVE_ITEM_FROM_DMCHAR05, true, ITEM_MASK_GORON)) {
                GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                    .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                    .param = GID_MASK_GORON,
                    .giveItem =
                        [](Actor* actor, PlayState* play) {
                            if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                                CustomMessage::SetActiveCustomMessage("You received the Goron Mask!",
                                                                      { .textboxType = 2 });
                            } else {
                                CustomMessage::StartTextbox("You received the Goron Mask!\x1C\x02\x10",
                                                            { .textboxType = 2 });
                            }
                            Item_Give(gPlayState, ITEM_MASK_GORON);
                        },
                });
            }
            /*
             * Darmani's ghost normally goes away after a scene reload, but we're skipping that transition, so we
             * find the actor and kill it manually.
             */
            Actor* actor =
                Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor, ACTOR_EN_GG, ACTORCAT_NPC, 99999.9f);
            Actor_Kill(actor);
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipHealingDarmani, { CVAR_NAME });
