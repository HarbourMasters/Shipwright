/*
 * File: z_dm_al.c
 * Overlay: ovl_Dm_Al
 * Description: Madame Aroma (cutscene)
 */

#include "z_dm_al.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void DmAl_Init(Actor* thisx, PlayState* play);
void DmAl_Destroy(Actor* thisx, PlayState* play);
void DmAl_Update(Actor* thisx, PlayState* play);
void DmAl_Draw(Actor* thisx, PlayState* play);

ActorProfile Dm_Al_Profile = {
    /**/ ACTOR_EN_AL,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_AL,
    /**/ sizeof(DmAl),
    /**/ DmAl_Init,
    /**/ DmAl_Destroy,
    /**/ DmAl_Update,
    /**/ DmAl_Draw,
};

typedef enum {
    /* -1 */ MADAME_AROMA_ANIM_NONE = -1,
    /*  0 */ MADAME_AROMA_ANIM_0,
    /*  1 */ MADAME_AROMA_ANIM_MAX
} DmAlAnimation;

static AnimationInfoS sAnimationInfo[MADAME_AROMA_ANIM_MAX] = {
    { &object_al_Anim_00DBE0, 1.0f, 0, -1, ANIMMODE_LOOP, 0 }, // MADAME_AROMA_ANIM_0
};

s32 DmAl_ChangeAnim(DmAl* this, s32 animIndex) {
    s32 didAnimChange = false;

    if (this->animIndex != animIndex) {
        this->animIndex = animIndex;
        didAnimChange = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
    }

    return didAnimChange;
}

void DmAl_HandleCutscene(DmAl* this, PlayState* play) {
    s32 csAnimIndex[] = {
        MADAME_AROMA_ANIM_0, MADAME_AROMA_ANIM_0, MADAME_AROMA_ANIM_0, MADAME_AROMA_ANIM_0, MADAME_AROMA_ANIM_0,
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
                DmAl_ChangeAnim(this, csAnimIndex[cueId]);
            }
            Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
        }
    } else if (this->isCutscenePlaying) {
        this->isCutscenePlaying = false;
        DmAl_ChangeAnim(this, this->prevAnimIndex);
    }
}

void DmAl_Init(Actor* thisx, PlayState* play) {
    DmAl* this = (DmAl*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gMadameAromaSkel, NULL, this->jointTable, this->morphTable,
                       MADAME_AROMA_LIMB_MAX);
    this->animIndex = MADAME_AROMA_ANIM_NONE;
    DmAl_ChangeAnim(this, MADAME_AROMA_ANIM_0);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = DmAl_HandleCutscene;
}

void DmAl_Destroy(Actor* thisx, PlayState* play) {
}

void DmAl_Update(Actor* thisx, PlayState* play) {
    DmAl* this = (DmAl*)thisx;

    this->actionFunc(this, play);
    SkelAnime_Update(&this->skelAnime);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
}

s32 DmAl_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* rot, Vec3s* pos, Actor* thisx) {
    switch (limbIndex) {
        case MADAME_AROMA_LIMB_SHAWL_MIDDLE:
        case MADAME_AROMA_LIMB_SHAWL_UPPER:
        case MADAME_AROMA_LIMB_SHAWL_LEFT_LOWER_MIDDLE:
        case MADAME_AROMA_LIMB_SHAWL_LEFT_LOWER:
        case MADAME_AROMA_LIMB_SHAWL_RIGHT_LOWER_MIDDLE:
        case MADAME_AROMA_LIMB_SHAWL_RIGHT_LOWER:
            *dList = NULL;
            break;

        default:
            break;
    }
    return false;
}

void DmAl_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    DmAl* this = (DmAl*)thisx;

    switch (limbIndex) {
        case MADAME_AROMA_LIMB_SHAWL_MIDDLE:
            Matrix_Get(&this->shawlMatrices[0]);
            break;

        case MADAME_AROMA_LIMB_SHAWL_UPPER:
            Matrix_Get(&this->shawlMatrices[1]);
            break;

        case MADAME_AROMA_LIMB_SHAWL_LEFT_LOWER_MIDDLE:
            Matrix_Get(&this->shawlMatrices[2]);
            break;

        case MADAME_AROMA_LIMB_SHAWL_LEFT_LOWER:
            Matrix_Get(&this->shawlMatrices[3]);
            break;

        case MADAME_AROMA_LIMB_SHAWL_RIGHT_LOWER_MIDDLE:
            Matrix_Get(&this->shawlMatrices[4]);
            break;

        case MADAME_AROMA_LIMB_SHAWL_RIGHT_LOWER:
            Matrix_Get(&this->shawlMatrices[5]);
            break;

        default:
            break;
    }
}

void DmAl_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
}

static Gfx* sDlists[] = {
    gMadameAromaShawlMiddleDL,           gMadameAromaShawlUpperDL,
    gMadameAromaShawlLeftLowerMiddleDL,  gMadameAromaShawlLeftLowerDL,
    gMadameAromaShawlRightLowerMiddleDL, gMadameAromaShawlRightLowerDL,
};

void DmAl_Draw(Actor* thisx, PlayState* play) {
    u32 i;
    DmAl* this = (DmAl*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, DmAl_OverrideLimbDraw, DmAl_PostLimbDraw,
                                   DmAl_TransformLimbDraw, &this->actor);
    for (i = 0; i < ARRAY_COUNT(this->shawlMatrices); i++) {
        Matrix_Put(&this->shawlMatrices[i]);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, sDlists[i]);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
