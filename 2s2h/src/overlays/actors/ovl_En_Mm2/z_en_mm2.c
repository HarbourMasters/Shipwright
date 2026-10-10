/*
 * File: z_en_mm2.c
 * Overlay: ovl_En_Mm2
 * Description: Postman's Letter to Himself
 */

#include "z_en_mm2.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnMm2_Init(Actor* thisx, PlayState* play);
void EnMm2_Destroy(Actor* thisx, PlayState* play);
void EnMm2_Update(Actor* thisx, PlayState* play);
void EnMm2_Draw(Actor* thisx, PlayState* play);

void EnMm2_Reading(EnMm2* this, PlayState* play);
void EnMm2_WaitForRead(EnMm2* this, PlayState* play);

ActorProfile En_Mm2_Profile = {
    /**/ ACTOR_EN_MM2,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnMm2),
    /**/ EnMm2_Init,
    /**/ EnMm2_Destroy,
    /**/ EnMm2_Update,
    /**/ EnMm2_Draw,
};

#include "overlays/ovl_En_Mm2/ovl_En_Mm2.h"

void EnMm2_Init(Actor* thisx, PlayState* play) {
    EnMm2* this = (EnMm2*)thisx;

    Actor_SetScale(&this->actor, 0.015f);
    this->actionFunc = EnMm2_WaitForRead;
}

void EnMm2_Destroy(Actor* thisx, PlayState* play) {
}

/**
 * Action function whilst Link is reading the letter.
 */
void EnMm2_Reading(EnMm2* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                this->actionFunc = EnMm2_WaitForRead;
            }
            break;

        case TEXT_STATE_CLOSING:
            this->actionFunc = EnMm2_WaitForRead;
            break;

        default:
            break;
    }
}

/**
 * Action function that awaits Link to read the letter, changing the A button to "Check" when he is within range to do
 * so (and facing the letter).
 */
void EnMm2_WaitForRead(EnMm2* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_StartTextbox(play, 0x277B, &this->actor);
        this->actionFunc = EnMm2_Reading;
    } else if ((this->actor.xzDistToPlayer < 60.0f) && Player_IsFacingActor(&this->actor, 0x3000, play)) {
        Actor_OfferTalk(&this->actor, play, 110.0f);
    }
}

void EnMm2_Update(Actor* thisx, PlayState* play) {
    EnMm2* this = (EnMm2*)thisx;

    this->actionFunc(this, play);
}

void EnMm2_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, sEnMm2DL);

    CLOSE_DISPS(play->state.gfxCtx);
}
