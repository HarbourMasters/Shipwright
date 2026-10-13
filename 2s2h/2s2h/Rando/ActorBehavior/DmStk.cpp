#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/ShipUtils.h"
#include "2s2h/Rando/Logic/Logic.h"

extern "C" {
#include "functions.h"
#include "variables.h"
#include "overlays/actors/ovl_Dm_Char02/z_dm_char02.h"
#include "overlays/actors/ovl_Dm_Stk/z_dm_stk.h"

void DmChar02_PlaySfxForCutscenes(DmChar02* dmChar02, PlayState* play);
void DmStk_ClockTower_Idle(DmStk* dmStk, PlayState* play);
void DmStk_ClockTower_WaitForDeflectionToEnd(DmStk* DmStk, PlayState* play);
}

void ApplyOathHint(u16* textId, bool* loadFromMessageTable) {
    DmStk* dmStk = (DmStk*)Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor, ACTOR_DM_STK,
                                            ACTORCAT_ITEMACTION, 1000.0f);
    std::string msg;

    if (dmStk == NULL || dmStk->actionFunc != DmStk_ClockTower_Idle) {
        return;
    }

    if (Rando::Logic::RemainsCount() < RANDO_SAVE_OPTIONS[RO_ACCESS_MOON_REMAINS_COUNT]) {
        msg = "You think you can defeat me? The Giants are trapped and powerless to stop me. Even if they were free, "
              "they couldn't save you.";
    } else {
        msg = "I can hear the Giants Melody echoing "
              "%y{{location}}%w. But it's too late! They can't help you now!";
    }

    RandoCheckId randoCheckId = Rando::FindItemPlacement(RI_SONG_OATH);
    CustomMessage::Replace(&msg, "{{location}}", Rando::StaticData::GetLocationNameForHint(randoCheckId, false));

    CustomMessage::Entry entry = {
        .nextMessageID = (u16)0xFFFF,
        .msg = msg,
    };

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void DmChar02_UpdateCustom(Actor* actor, PlayState* play) {
    DmChar02* dmChar02 = (DmChar02*)actor;

    SkelAnime_Update(&dmChar02->skelAnime);
    dmChar02->unk_2F0 = dmChar02->unk_2F0;
    dmChar02->actionFunc(dmChar02, play);
    if ((actor->xzDistToPlayer <= 30.0f) && (fabsf(actor->playerHeightRel) <= fabsf(80.0f))) {
        Actor_Kill(&dmChar02->actor);

        auto& randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_ROOF_OCARINA];
        randoSaveCheck.eligible = true;
        auto& randoSaveCheck2 = RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_ROOF_SONG_OF_TIME];
        randoSaveCheck2.eligible = true;
    }

    DmChar02_PlaySfxForCutscenes(dmChar02, play);
}

// This handles the two checks for the Clock Tower Roof, the Ocarina and Song of Time checks. It also handles
// overriding the drawing of the Ocarina in the hand of the Skull Kid.
void Rando::ActorBehavior::InitDmStkBehavior() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_DM_CHAR02, IS_RANDO,
                 [](Actor* actor, bool* should) { actor->update = DmChar02_UpdateCustom; });

    COND_VB_SHOULD(VB_DRAW_OCARINA_IN_STK_HAND, IS_RANDO, {
        Actor* dmStk = va_arg(args, Actor*);

        if (*should) {
            *should = false;

            Matrix_Scale(15.0f, 15.0f, 15.0f, MTXMODE_APPLY);
            Vec3s rot;
            rot.x = -19737;
            rot.y = 31100;
            rot.z = 8674;
            Vec3f pos;
            pos.x = -26.5f;
            pos.y = -50.8f;
            pos.z = 10.9f;
            Matrix_TranslateRotateZYX(&pos, &rot);

            auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_ROOF_OCARINA];
            Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, RC_CLOCK_TOWER_ROOF_OCARINA),
                            RC_CLOCK_TOWER_ROOF_OCARINA, dmStk);
        }
    });

    COND_VB_SHOULD(VB_OVERRIDE_CHAR02_LIMB, IS_RANDO, {
        Gfx** dList = va_arg(args, Gfx**);

        *dList = NULL;
    });

    COND_VB_SHOULD(VB_POST_CHAR02_LIMB, IS_RANDO, {
        Actor* dmChar02 = va_arg(args, Actor*);

        Matrix_Scale(15.0f, 15.0f, 15.0f, MTXMODE_APPLY);
        Vec3s rot;
        rot.x = -11554;
        Vec3f pos;
        pos.x = 10.4f;
        pos.y = -26.5f;
        pos.z = 13.2f;
        Matrix_TranslateRotateZYX(&pos, &rot);

        auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_ROOF_OCARINA];
        Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, RC_CLOCK_TOWER_ROOF_OCARINA),
                        RC_CLOCK_TOWER_ROOF_OCARINA, dmChar02);
    });

    COND_VB_SHOULD(VB_STK_HAVE_OCARINA, IS_RANDO, {
        auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_ROOF_OCARINA];
        *should = !randoSaveCheck.cycleObtained;
    });

    COND_ID_HOOK(OnOpenText, 0x2013, IS_RANDO && RANDO_SAVE_OPTIONS[RO_HINTS_OATH_TO_ORDER], ApplyOathHint);

    COND_ID_HOOK(
        ShouldActorUpdate, ACTOR_DM_STK, IS_RANDO && RANDO_SAVE_OPTIONS[RO_HINTS_OATH_TO_ORDER],
        [](Actor* actor, bool* should) {
            DmStk* dmStk = (DmStk*)actor;

            if ((dmStk->actionFunc != DmStk_ClockTower_Idle || CAN_PLAY_SONG(OATH))) {
                return;
            }

            if ((dmStk->actor.xzDistToPlayer < 200.0f) && Player_IsFacingActor(&dmStk->actor, 0x3000, gPlayState)) {
                Actor_OfferTalk(&dmStk->actor, gPlayState, 200.0f);
            }

            if (Actor_TalkOfferAccepted(&dmStk->actor, &gPlayState->state)) {
                Message_StartTextbox(gPlayState, 0x2013, &dmStk->actor);
                if ((Message_GetState(&gPlayState->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(gPlayState)) {
                    Message_CloseTextbox(gPlayState);
                }
            }
        });
}
