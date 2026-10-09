/*
 * File: z_dm_nb.c
 * Overlay: ovl_Dm_Nb
 * Description: Anju's Grandma at wedding during the credits
 */

#include "z_dm_nb.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void DmNb_Init(Actor* thisx, PlayState* play);
void DmNb_Destroy(Actor* thisx, PlayState* play);
void DmNb_Update(Actor* thisx, PlayState* play);
void DmNb_Draw(Actor* thisx, PlayState* play);

ActorProfile Dm_Nb_Profile = {
    /**/ ACTOR_DM_NB,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_NB,
    /**/ sizeof(DmNb),
    /**/ DmNb_Init,
    /**/ DmNb_Destroy,
    /**/ DmNb_Update,
    /**/ DmNb_Draw,
};

typedef enum {
    /* -1 */ DMNB_ANIM_NONE = -1,
    /*  0 */ DMNB_ANIM_0,
    /*  1 */ DMNB_ANIM_MAX
} DmNbAnimation;

static AnimationInfoS sAnimationInfo[DMNB_ANIM_MAX] = {
    { &gNbIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 }, // DMNB_ANIM_0
};

s32 DmNb_ChangeAnim(DmNb* this, s32 animIndex) {
    s32 didAnimChange = false;

    if (this->animIndex != animIndex) {
        this->animIndex = animIndex;
        didAnimChange = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
    }

    return didAnimChange;
}

void DmNb_HandleCutscene(DmNb* this, PlayState* play) {
    s32 csAnimIndex[] = {
        DMNB_ANIM_0, DMNB_ANIM_0, DMNB_ANIM_0, DMNB_ANIM_0, DMNB_ANIM_0,
    };
    u16 cueId;
    s32 cueChannel;

    if (play->csCtx.state != CS_STATE_IDLE) {
        if (!this->isCutscenePlaying) {
            this->cueId = 255;
            this->isCutscenePlaying = true;
            this->prevAnimIndex = this->animIndex;
        }
        if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_562)) {
            cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_562);
            cueId = play->csCtx.actorCues[cueChannel]->id;
            if (this->cueId != (u8)cueId) {
                this->cueId = cueId;
                DmNb_ChangeAnim(this, csAnimIndex[cueId]);
            }
            Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
        }
    } else if (this->isCutscenePlaying) {
        this->isCutscenePlaying = false;
        DmNb_ChangeAnim(this, this->prevAnimIndex);
    }
}

void DmNb_Init(Actor* thisx, PlayState* play) {
    DmNb* this = (DmNb*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gNbSkel, NULL, this->jointTable, this->morphTable, NB_LIMB_MAX);
    this->animIndex = DMNB_ANIM_NONE;
    DmNb_ChangeAnim(this, DMNB_ANIM_0);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = DmNb_HandleCutscene;
}

void DmNb_Destroy(Actor* thisx, PlayState* play) {
}

void DmNb_Update(Actor* thisx, PlayState* play) {
    DmNb* this = (DmNb*)thisx;

    this->actionFunc(this, play);
    SkelAnime_Update(&this->skelAnime);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
}

void DmNb_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
}

void DmNb_Draw(Actor* thisx, PlayState* play) {
    DmNb* this = (DmNb*)thisx;

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, NULL, NULL, DmNb_TransformLimbDraw, &this->actor);
}
