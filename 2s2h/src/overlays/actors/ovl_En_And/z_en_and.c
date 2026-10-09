/*
 * File: z_en_and.c
 * Overlay: ovl_En_And
 * Description: Anju in Wedding Dress
 */

#include "z_en_and.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

#define EYE_TEXTURES_COUNT 4

void EnAnd_Init(Actor* thisx, PlayState* play);
void EnAnd_Destroy(Actor* thisx, PlayState* play);
void EnAnd_Update(Actor* thisx, PlayState* play);
void EnAnd_Draw(Actor* thisx, PlayState* play);

ActorProfile En_And_Profile = {
    /**/ ACTOR_EN_AND,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_AND,
    /**/ sizeof(EnAnd),
    /**/ EnAnd_Init,
    /**/ EnAnd_Destroy,
    /**/ EnAnd_Update,
    /**/ EnAnd_Draw,
};

typedef enum {
    /* -1 */ ENAND_ANIM_NONE = -1,
    /*  0 */ ENAND_ANIM_0,
    /*  1 */ ENAND_ANIM_1,
    /*  2 */ ENAND_ANIM_2,
    /*  3 */ ENAND_ANIM_3,
    /*  4 */ ENAND_ANIM_4,
    /*  5 */ ENAND_ANIM_5,
    /*  6 */ ENAND_ANIM_6,
    /*  7 */ ENAND_ANIM_7,
    /*  8 */ ENAND_ANIM_MAX
} EnAndAnimation;

static AnimationInfoS sAnimationInfo[ENAND_ANIM_MAX] = {
    { &gAndStaticAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },         // ENAND_ANIM_0
    { &gAndIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },           // ENAND_ANIM_1
    { &gAndWalkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },           // ENAND_ANIM_2
    { &gAndRaiseHeadAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },      // ENAND_ANIM_3
    { &gAndRaisedHeadLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 }, // ENAND_ANIM_4
    { &gAndRaiseHandAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },      // ENAND_ANIM_5
    { &gAndRaisedHandLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 }, // ENAND_ANIM_6
    { &gAndRaisedHandWalkAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 }, // ENAND_ANIM_7
};

s32 EnAnd_ChangeAnim(EnAnd* this, s32 animIndex) {
    s32 didAnimChange = false;

    if (this->animIndex != animIndex) {
        this->animIndex = animIndex;
        didAnimChange = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
    }
    return didAnimChange;
}

void EnAnd_Blink(EnAnd* this) {
    if (DECR(this->blinkTimer) == 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex >= EYE_TEXTURES_COUNT) {
            this->blinkTimer = Rand_S16Offset(30, 30);
            this->eyeTexIndex = 0;
        }
    }
}

void EnAnd_HandleCutscene(EnAnd* this, PlayState* play) {
    s32 csAnimIndex[] = {
        ENAND_ANIM_0, ENAND_ANIM_1, ENAND_ANIM_2, ENAND_ANIM_3, ENAND_ANIM_5, ENAND_ANIM_7,
    };
    u16 cueId;
    s32 cueChannel;

    if (play->csCtx.state != CS_STATE_IDLE) {
        if (!this->isCutscenePlaying) {
            // Cutscene is starting
            this->cueId = 255;
            this->isCutscenePlaying = true;
            this->prevAnimIndex = this->animIndex;
        }
        if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_565)) {
            cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_565);
            cueId = play->csCtx.actorCues[cueChannel]->id;
            if (this->cueId != (u8)cueId) {
                this->cueId = cueId;
                EnAnd_ChangeAnim(this, csAnimIndex[cueId]);
            }
            switch (this->cueId) {
                case 3:
                case 4:
                    if ((this->animIndex == ENAND_ANIM_3) || (this->animIndex == ENAND_ANIM_5)) {
                        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                            EnAnd_ChangeAnim(this, this->animIndex + 1);
                        }
                    }
                    break;

                default:
                    break;
            }
            Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
        }
    } else if (this->isCutscenePlaying) {
        this->isCutscenePlaying = false;
        EnAnd_ChangeAnim(this, this->prevAnimIndex);
    }
}

void EnAnd_Init(Actor* thisx, PlayState* play) {
    EnAnd* this = (EnAnd*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 14.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gAndSkel, NULL, this->jointTable, this->morphTable,
                       OBJECT_AND_LIMB_MAX);
    this->animIndex = ENAND_ANIM_NONE;
    EnAnd_ChangeAnim(this, ENAND_ANIM_0);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->flags |= 8;
    this->actionFunc = EnAnd_HandleCutscene;
}

void EnAnd_Destroy(Actor* thisx, PlayState* play) {
}

void EnAnd_Update(Actor* thisx, PlayState* play) {
    EnAnd* this = (EnAnd*)thisx;

    this->actionFunc(this, play);
    SkelAnime_Update(&this->skelAnime);
    EnAnd_Blink(this);
}

void EnAnd_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnAnd* this = (EnAnd*)thisx;
    s32 stepRot;
    s32 overrideRot;

    stepRot = this->flags & 8 ? false : true;
    overrideRot = this->flags & 2 ? true : false;
    if (!stepRot) {
        overrideRot = false;
    }

    switch (limbIndex) {
        case OBJECT_AND_LIMB_HEAD:
            SubS_UpdateLimb(this->headRotZ + this->torsoRotZ + 0x4000,
                            this->headRotY + this->torsoRotY + this->actor.shape.rot.y + 0x4000, &this->headComputedPos,
                            &this->headComputedRot, stepRot, overrideRot);
            Matrix_Pop();
            Matrix_Translate(this->headComputedPos.x, this->headComputedPos.y, this->headComputedPos.z, MTXMODE_NEW);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Matrix_RotateYS(this->headComputedRot.y, MTXMODE_APPLY);
            Matrix_RotateXS(this->headComputedRot.x, MTXMODE_APPLY);
            Matrix_RotateZS(this->headComputedRot.z, MTXMODE_APPLY);
            Matrix_Push();
            break;

        case OBJECT_AND_LIMB_TORSO:
            SubS_UpdateLimb(this->torsoRotZ + 0x4000, this->torsoRotY + this->actor.shape.rot.y + 0x4000,
                            &this->torsoComputedPos, &this->torsoComputedRot, stepRot, overrideRot);
            Matrix_Pop();
            Matrix_Translate(this->torsoComputedPos.x, this->torsoComputedPos.y, this->torsoComputedPos.z, MTXMODE_NEW);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Matrix_RotateYS(this->torsoComputedRot.y, MTXMODE_APPLY);
            Matrix_RotateXS(this->torsoComputedRot.x, MTXMODE_APPLY);
            Matrix_RotateZS(this->torsoComputedRot.z, MTXMODE_APPLY);
            Matrix_Push();
            break;

        default:
            break;
    }
}

void EnAnd_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sMouthTextures[] = { gAndMouthNeutralTex, gAndMouthSmileTex };
    static TexturePtr sEyeTextures[EYE_TEXTURES_COUNT] = {
        gAndEyeOpenTex,
        gAndEyeClosingTex,
        gAndEyeClosedTex,
        gAndEyeOpeningTex,
    };
    EnAnd* this = (EnAnd*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sMouthTextures[0]));

    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, NULL, NULL, EnAnd_TransformLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
