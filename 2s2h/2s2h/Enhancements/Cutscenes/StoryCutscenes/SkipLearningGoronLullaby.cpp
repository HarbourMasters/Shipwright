#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Gk/z_en_gk.h"
#include "overlays/actors/ovl_En_Go/z_en_go.h"
#include "functions.h"

void func_80B5227C(EnGk* enGkActor, PlayState* play);
void EnGo_Sleep(EnGo* enGoActor, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static bool isGoronSleepQueued = false;

// This is a song tutorial, so the skip is forced on in rando for now
void RegisterSkipLearningGoronLullaby() {
    // Played Lullaby Intro for Baby Goron
    COND_VB_SHOULD(VB_START_CUTSCENE, CVAR || IS_RANDO, {
        s16* csId = va_arg(args, s16*);
        Actor* actor = va_arg(args, Actor*);

        if (*csId != 9 || actor == NULL || actor->id != ACTOR_EN_GK) {
            return;
        }

        EnGk* enGk = (EnGk*)actor;

        // Reset baby Goron cutscene flags state
        enGk->unk_1E4 &= ~0x20;
        *should = false;

        // Set action func which puts Baby to sleep, sets calm week event flag
        enGk->csAnimIndex = 1; // ENGK_ANIM_1
        enGk->animIndex = 1;   // ENGK_ANIM_1
        enGk->actionFunc = func_80B5227C;

        // Activate torches
        Flags_SetSwitch(gPlayState, ENGK_GET_SWITCH_FLAG(&enGk->actor));

        isGoronSleepQueued = true;

        if (GameInteractor_Should(VB_GIVE_ITEM_FROM_GK_LULLABY, !CHECK_QUEST_ITEM(QUEST_SONG_LULLABY))) {
            GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                .giveItem =
                    [](Actor* actor, PlayState* play) {
                        if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                            CustomMessage::SetActiveCustomMessage("You learned the complete Goron Lullaby!",
                                                                  { .textboxType = 2 });
                        } else {
                            CustomMessage::StartTextbox("You learned the complete Goron Lullaby!\x1C\x02\x10",
                                                        { .textboxType = 2 });
                        }
                        Item_Give(gPlayState, ITEM_SONG_LULLABY);
                    },
                .drawItem =
                    [](Actor* actor, PlayState* play) {
                        Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                        Rando::DrawItem(RI_SONG_LULLABY);
                    } });
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_GO, CVAR || IS_RANDO, [](Actor* actor) {
        EnGo* enGo = (EnGo*)actor;

        // Should only apply this to the Goron next to the Elder's Son
        // immediately after playing the Lullaby Intro.
        if (ENGO_GET_TYPE(&enGo->actor) != ENGO_ASIDE_ELDERSSON) {
            return;
        }

        // The Gorons use a very specific condition to check if they should be put to sleep.
        // It's easier to avoid side effects by directly changing their state such that they should start
        // to go to sleep.
        if (isGoronSleepQueued) {
            isGoronSleepQueued = false;

            SubS_SetOfferMode(&enGo->actionFlags, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
            enGo->sleepState = 1; // ENGO_ASLEEP_POS
            enGo->actionFunc = EnGo_Sleep;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipLearningGoronLullaby, { CVAR_NAME, "IS_RANDO" });
