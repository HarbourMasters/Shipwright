#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Jg/z_en_jg.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipLearningGoronLullabyIntro() {
    COND_VB_SHOULD(VB_JG_THINK_YOU_KNOW_LULLABY, CVAR || IS_RANDO, {
        // Always consider lullaby known so we don't go into the cutscene to learn it
        *should = true;

        // Goron Elder sets this reg at the end of the cutscene
        // Manually set it here as it is skipped
        SET_WEEKEVENTREG(WEEKEVENTREG_24_40);

        if (GameInteractor_Should(VB_GIVE_ITEM_FROM_JG, !CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO))) {
            GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                .giveItem =
                    [](Actor* actor, PlayState* play) {
                        if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                            CustomMessage::SetActiveCustomMessage("You learned the Lullaby Intro!",
                                                                  { .textboxType = 2 });
                        } else {
                            CustomMessage::StartTextbox("You learned the Lullaby Intro!\x1C\x02\x10",
                                                        { .textboxType = 2 });
                        }
                        Item_Give(gPlayState, ITEM_SONG_LULLABY_INTRO);
                    },
                .drawItem =
                    [](Actor* actor, PlayState* play) {
                        Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                        Rando::DrawItem(RI_SONG_LULLABY_INTRO);
                    } });
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipLearningGoronLullabyIntro, { CVAR_NAME, "IS_RANDO" });
