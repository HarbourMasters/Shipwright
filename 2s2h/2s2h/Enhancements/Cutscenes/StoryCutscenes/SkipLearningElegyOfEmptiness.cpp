#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// Forced on in rando for now
void RegisterSkipLearningElegyOfEmptiness() {
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR || IS_RANDO, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_IKNINSIDE && *csId == 10) { // Defeated Igos, learn Elegy of Emptiness
            if (GameInteractor_Should(VB_GIVE_ITEM_FROM_KNIGHT, true)) {
                GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                    .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                    .giveItem =
                        [](Actor* actor, PlayState* play) {
                            if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                                CustomMessage::SetActiveCustomMessage("You learned the Elegy of Emptiness!",
                                                                      { .textboxType = 2 });
                            } else {
                                CustomMessage::StartTextbox("You learned the Elegy of Emptiness!\x1C\x02\x10",
                                                            { .textboxType = 2 });
                            }
                            Item_Give(gPlayState, ITEM_SONG_ELEGY);
                        },
                    .drawItem =
                        [](Actor* actor, PlayState* play) {
                            Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                            Rando::DrawItem(RI_SONG_ELEGY);
                        } });
            }
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipLearningElegyOfEmptiness, { CVAR_NAME, "IS_RANDO" });
