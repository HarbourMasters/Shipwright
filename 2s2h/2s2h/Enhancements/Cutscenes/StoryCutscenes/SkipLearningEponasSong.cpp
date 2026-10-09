#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "overlays/actors/ovl_En_Ma4/z_en_ma4.h"
void EnMa4_SetupDialogueHandler(EnMa4* enMa4);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipLearningEponasSong() {
    COND_VB_SHOULD(VB_QUEUE_CUTSCENE, CVAR || IS_RANDO, {
        s16* csId = va_arg(args, s16*);

        if (gPlayState->sceneId != SCENE_F01) { // Romani Ranch
            return;
        }

        EnMa4* enMa4 =
            (EnMa4*)Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor, ACTOR_EN_MA4, ACTORCAT_NPC, 99999.9f);
        if (!enMa4) {
            return;
        }

        if (*csId != 14) { // Epona Reveal
            return;
        }

        if (GameInteractor_Should(VB_GIVE_ITEM_FROM_ROMANI, true, enMa4)) {
            GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                .showGetItemCutscene = true,
                .giveItem =
                    [](Actor* actor, PlayState* play) {
                        if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                            CustomMessage::SetActiveCustomMessage("You received Epona's Song!", { .textboxType = 2 });
                        } else {
                            CustomMessage::StartTextbox("You received Epona's Song!\x1C\x02\x10", { .textboxType = 2 });
                        }
                        Item_Give(gPlayState, ITEM_SONG_EPONA);
                    },
                .drawItem =
                    [](Actor* actor, PlayState* play) {
                        Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                        Rando::DrawItem(RI_SONG_EPONA);
                    } });
        }

        *should = false;
        Message_StartTextbox(gPlayState, 0x334C, &enMa4->actor);
        enMa4->textId = 0x334C;
        GET_PLAYER(gPlayState)->stateFlags1 &= ~PLAYER_STATE1_20;
        enMa4->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        EnMa4_SetupDialogueHandler(enMa4);
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipLearningEponasSong, { CVAR_NAME, "IS_RANDO" });
