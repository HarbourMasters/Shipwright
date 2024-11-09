#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/OTRGlobals.h"

extern "C" {
    #include "src/overlays/actors/ovl_En_Zl4/z_en_zl4.h"
}

/**
 * This overrides Zelda's update function to effectively skip the dialog and cutscenes played when
 * you meet with her in Hyrule Castle Courtyard. As you approach her she will turn around, and talking
 * with her will place you at the very last dialog option where she gives you the letter.
 */

u16 EnZl4_GiveItemTextId(PlayState* play, Actor* actor) {
    return 0x207D;
}

void EnZl4_SkipToGivingZeldasLetter(EnZl4* enZl4, PlayState* play) {
    if (enZl4->csState == 0 && enZl4->actor.xzDistToPlayer < 700.0f && EnZl4_SetNextAnim(enZl4, 3)) {
        Audio_PlayFanfare(NA_BGM_APPEAR);
        enZl4->csState = 8; // ZL4_CS_PLAN
    } else {
        Npc_UpdateTalking(play, &enZl4->actor, &enZl4->interactInfo.talkState, enZl4->collider.dim.radius + 60.0f, EnZl4_GiveItemTextId, func_80B5B9B0);
        func_80B5BB78(enZl4, play);

        if (enZl4->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
            enZl4->talkState = 6;
            enZl4->actionFunc = EnZl4_Cutscene;
        }
    }
}

void SkipToGivingZeldasLetter_OnActorInit(void* actorPtr) {
    if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO)) {
        EnZl4* enZl4 = static_cast<EnZl4*>(actorPtr);
        if (enZl4->actionFunc != EnZl4_Cutscene || enZl4->csState != 0) return;

        enZl4->actionFunc = EnZl4_SkipToGivingZeldasLetter;
    }
}

void SkipToGivingZeldasLetter_Register() {
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorInit>(ACTOR_EN_ZL4, SkipToGivingZeldasLetter_OnActorInit);
}
