#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include <functions.h>
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// Forced on in rando for now
void RegisterSkipLearningNewWaveBossaNova() {
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR || IS_RANDO, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_LABO && *csId == 11) {
            if (GameInteractor_Should(VB_GIVE_NEW_WAVE_BOSSA_NOVA, true)) {
                GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                    .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                    .giveItem =
                        [](Actor* actor, PlayState* play) {
                            if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                                CustomMessage::SetActiveCustomMessage("You learned the New Wave Bossa Nova!",
                                                                      { .textboxType = 2 });
                            } else {
                                CustomMessage::StartTextbox("You learned the New Wave Bossa Nova!\x1C\x02\x10",
                                                            { .textboxType = 2 });
                            }
                            Item_Give(gPlayState, ITEM_SONG_NOVA);
                        },
                    .drawItem =
                        [](Actor* actor, PlayState* play) {
                            Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                            Rando::DrawItem(RI_SONG_NOVA);
                        } });
            }
            SET_WEEKEVENTREG(WEEKEVENTREG_20_40);
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipLearningNewWaveBossaNova, { CVAR_NAME, "IS_RANDO" });
