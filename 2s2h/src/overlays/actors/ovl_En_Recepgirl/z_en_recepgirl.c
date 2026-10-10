/*
 * File: z_en_recepgirl.c
 * Overlay: ovl_En_Recepgirl
 * Description: Mayor's receptionist
 */

#include "z_en_recepgirl.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnRecepgirl_Init(Actor* thisx, PlayState* play);
void EnRecepgirl_Destroy(Actor* thisx, PlayState* play);
void EnRecepgirl_Update(Actor* thisx, PlayState* play);
void EnRecepgirl_Draw(Actor* thisx, PlayState* play);

void EnRecepgirl_SetupWait(EnRecepgirl* this);
void EnRecepgirl_Wait(EnRecepgirl* this, PlayState* play);
void EnRecepgirl_SetupTalk(EnRecepgirl* this);
void EnRecepgirl_Talk(EnRecepgirl* this, PlayState* play);

ActorProfile En_Recepgirl_Profile = {
    /**/ ACTOR_EN_RECEPGIRL,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_BG,
    /**/ sizeof(EnRecepgirl),
    /**/ EnRecepgirl_Init,
    /**/ EnRecepgirl_Destroy,
    /**/ EnRecepgirl_Update,
    /**/ EnRecepgirl_Draw,
};

static TexturePtr sEyeTextures[] = {
    object_bg_Tex_00F8F0,
    object_bg_Tex_00FCF0,
    object_bg_Tex_0100F0,
    object_bg_Tex_00FCF0,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_6, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 1000, ICHAIN_STOP),
};

static s32 sTexturesDesegmented = false;

void EnRecepgirl_Init(Actor* thisx, PlayState* play) {
    EnRecepgirl* this = (EnRecepgirl*)thisx;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, -60.0f, NULL, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_bg_Skel_011B60, &object_bg_Anim_009890, this->jointTable,
                       this->morphTable, OBJECT_BG_2_LIMB_MAX);

    if (!sTexturesDesegmented) {
        for (i = 0; i < ARRAY_COUNT(sEyeTextures); i++) {
            sEyeTextures[i] = Lib_SegmentedToVirtual(sEyeTextures[i]);
        }
        sTexturesDesegmented = true;
    }

    this->eyeTexIndex = 2;

    if (Flags_GetSwitch(play, ENRECEPGIRL_GET_SWITCH_FLAG(&this->actor))) {
        this->actor.textId = 0x2ADC; // hear directions again?
    } else {
        this->actor.textId = 0x2AD9; // "Welcome..."
    }

    EnRecepgirl_SetupWait(this);
}

void EnRecepgirl_Destroy(Actor* thisx, PlayState* play) {
}

void EnRecepgirl_UpdateEyes(EnRecepgirl* this) {
    if (this->eyeTexIndex != 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex == 4) {
            this->eyeTexIndex = 0;
        }
    } else if (Rand_ZeroOne() < 0.02f) {
        this->eyeTexIndex++;
    }
}

void EnRecepgirl_SetupWait(EnRecepgirl* this) {
    if (this->skelAnime.animation == &object_bg_Anim_001384) {
        Animation_MorphToPlayOnce(&this->skelAnime, &object_bg_Anim_00AD98, 5.0f);
    }
    this->actionFunc = EnRecepgirl_Wait;
}

void EnRecepgirl_Wait(EnRecepgirl* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->skelAnime.animation == &object_bg_Anim_00A280) {
            Animation_MorphToPlayOnce(&this->skelAnime, &object_bg_Anim_00AD98, 5.0f);
        } else {
            Animation_MorphToLoop(&this->skelAnime, &object_bg_Anim_009890, -4.0f);
        }
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        EnRecepgirl_SetupTalk(this);
    } else if (Actor_IsFacingPlayer(&this->actor, 0x2000)) {
        Actor_OfferTalk(&this->actor, play, 60.0f);
        if (Player_GetMask(play) == PLAYER_MASK_KAFEIS_MASK) {
            this->actor.textId = 0x2367;
        } else if (Flags_GetSwitch(play, ENRECEPGIRL_GET_SWITCH_FLAG(&this->actor))) {
            this->actor.textId = 0x2ADC; // hear directions again?
        } else {
            this->actor.textId = 0x2AD9;
        }
    }
}

void EnRecepgirl_SetupTalk(EnRecepgirl* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &object_bg_Anim_00A280, -4.0f);
    this->actionFunc = EnRecepgirl_Talk;
}

void EnRecepgirl_Talk(EnRecepgirl* this, PlayState* play) {
    u8 talkState;

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->skelAnime.animation == &object_bg_Anim_00A280) {
            Animation_PlayLoop(&this->skelAnime, &object_bg_Anim_001384);
        } else if (this->skelAnime.animation == &object_bg_Anim_00AD98) {
            if (this->actor.textId == 0x2ADA) { // Mayor's office is on the left (meeting ongoing)
                Animation_MorphToPlayOnce(&this->skelAnime, &object_bg_Anim_000968, 10.0f);
            } else {
                Animation_MorphToLoop(&this->skelAnime, &object_bg_Anim_009890, 10.0f);
            }
        } else {
            if (this->actor.textId == 0x2ADA) { // Mayor's office is on the left (meeting ongoing)
                Animation_MorphToLoop(&this->skelAnime, &object_bg_Anim_009890, 10.0f);
            } else {
                Animation_MorphToPlayOnce(&this->skelAnime, &object_bg_Anim_00A280, -4.0f);
            }
        }
    }

    talkState = Message_GetState(&play->msgCtx);
    if (talkState == TEXT_STATE_CLOSING) {
        this->actor.textId = 0x2ADC; // hear directions again?
        EnRecepgirl_SetupWait(this);
    } else if ((talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->actor.textId == 0x2AD9) {
            Flags_SetSwitch(play, ENRECEPGIRL_GET_SWITCH_FLAG(&this->actor));
            Animation_MorphToPlayOnce(&this->skelAnime, &object_bg_Anim_00AD98, 10.0f);

            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING)) {
                this->actor.textId = 0x2ADF; // Mayor's office is on the left (meeting ended)
            } else {
                this->actor.textId = 0x2ADA; // Mayor's office is on the left (meeting ongoing)
            }
        } else if (this->actor.textId == 0x2ADC) { // hear directions again?
            Animation_MorphToPlayOnce(&this->skelAnime, &object_bg_Anim_00AD98, 10.0f);
            this->actor.textId = 0x2ADD;
        } else {
            Animation_MorphToPlayOnce(&this->skelAnime, &object_bg_Anim_000968, 10.0f);

            if (this->actor.textId == 0x2ADD) {
                this->actor.textId = 0x2ADE;           // Mayor's office is on the left, drawing room on the right
            } else if (this->actor.textId == 0x2ADA) { // Mayor's office is on the left (meeting ongoing)
                this->actor.textId = 0x2ADB;           // drawing room on the right
            } else {
                this->actor.textId = 0x2AE0; // drawing room on the right, don't go in without an appointment
            }
        }
        Message_ContinueTextbox(play, this->actor.textId);
    }
}

void EnRecepgirl_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRecepgirl* this = (EnRecepgirl*)thisx;
    Vec3s torsoRot;

    this->actionFunc(this, play);
    Actor_TrackPlayer(play, &this->actor, &this->headRot, &torsoRot, this->actor.focus.pos);
    EnRecepgirl_UpdateEyes(this);
}

s32 EnRecepgirl_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnRecepgirl* this = (EnRecepgirl*)thisx;

    if (limbIndex == OBJECT_BG_2_LIMB_05) {
        rot->x += this->headRot.y;
    }
    return false;
}

void EnRecepgirl_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnRecepgirl* this = (EnRecepgirl*)thisx;

    if (limbIndex == OBJECT_BG_2_LIMB_05) {
        Matrix_RotateYS(0x400 - this->headRot.x, MTXMODE_APPLY);
        Matrix_MultVecX(500.0f, &this->actor.focus.pos);
    }
}

void EnRecepgirl_Draw(Actor* thisx, PlayState* play) {
    EnRecepgirl* this = (EnRecepgirl*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, sEyeTextures[this->eyeTexIndex]);

    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, EnRecepgirl_OverrideLimbDraw, NULL,
                                   EnRecepgirl_TransformLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
