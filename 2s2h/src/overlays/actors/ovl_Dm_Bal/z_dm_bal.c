/*
 * File: z_dm_bal.c
 * Overlay: ovl_Dm_Bal
 * Description: Tingle (cutscene)
 */

#include "z_dm_bal.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA)

void DmBal_Init(Actor* thisx, PlayState* play);
void DmBal_Destroy(Actor* thisx, PlayState* play);
void DmBal_Update(Actor* thisx, PlayState* play);
void DmBal_Draw(Actor* thisx, PlayState* play);

void DmBal_SetupDoNothing(DmBal* this);
void DmBal_DoNothing(DmBal* this, PlayState* play);

ActorProfile Dm_Bal_Profile = {
    /**/ ACTOR_DM_BAL,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_BAL,
    /**/ sizeof(DmBal),
    /**/ DmBal_Init,
    /**/ DmBal_Destroy,
    /**/ DmBal_Update,
    /**/ DmBal_Draw,
};

// These animations are the same and in the same order as in the main NPC actor EnBal
typedef enum {
    /*  -1 */ TINGLE_CS_ANIM_NONE = -1,
    /* 0x0 */ TINGLE_CS_ANIM_FLOAT_IDLE,
    /* 0x1 */ TINGLE_CS_ANIM_FALL_LOOP,
    /* 0x2 */ TINGLE_CS_ANIM_FALL_ONCE,
    /* 0x3 */ TINGLE_CS_ANIM_LAND,
    /* 0x4 */ TINGLE_CS_ANIM_TWIST,
    /* 0x5 */ TINGLE_CS_ANIM_TALK,
    /* 0x6 */ TINGLE_CS_ANIM_MAGIC,
    /* 0x7 */ TINGLE_CS_ANIM_HAPPY_DANCE_LOOP,
    /* 0x8 */ TINGLE_CS_ANIM_HAPPY_DANCE_ONCE,
    /* 0x9 */ TINGLE_CS_ANIM_MAGIC_REVERSE,
    /* 0xA */ TINGLE_CS_ANIM_IDLE,
    /* 0xB */ TINGLE_CS_ANIM_SPIN,
    /* 0xC */ TINGLE_CS_ANIM_HIDE_FACE,
    /* 0xD */ TINGLE_CS_ANIM_CONFETTI,
    /* 0xE */ TINGLE_CS_ANIM_MAX
} TingleCsAnimation;

static AnimationInfo sAnimationInfo[TINGLE_CS_ANIM_MAX] = {
    { &gTingleFloatIdleAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },          // TINGLE_CS_ANIM_FLOAT_IDLE
    { &gTingleFallAnim, 1.5f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },               // TINGLE_CS_ANIM_FALL_LOOP
    { &gTingleFallAnim, 1.5f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0f },               // TINGLE_CS_ANIM_FALL_ONCE
    { &gTingleLandAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0f },               // TINGLE_CS_ANIM_LAND
    { &gTingleTwistAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },              // TINGLE_CS_ANIM_TWIST
    { &gTingleTalkAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },               // TINGLE_CS_ANIM_TALK
    { &gTingleThrowConfettiAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -8.0f },      // TINGLE_CS_ANIM_MAGIC
    { &gTingleHappyDanceAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },         // TINGLE_CS_ANIM_HAPPY_DANCE_LOOP
    { &gTingleHappyDanceAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -8.0f },         // TINGLE_CS_ANIM_HAPPY_DANCE_ONCE
    { &gTingleThrowConfettiAnim, 1.0f, 23.0f, 0.0f, ANIMMODE_ONCE, -8.0f },     // TINGLE_CS_ANIM_MAGIC_REVERSE
    { &gTingleIdleAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },               // TINGLE_CS_ANIM_IDLE
    { &gTingleSpinAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -2.0f },               // TINGLE_CS_ANIM_SPIN
    { &gTingleFloatHideFaceAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -2.0f },      // TINGLE_CS_ANIM_HIDE_FACE
    { &gTingleFloatThrowConfettiAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -2.0f }, // TINGLE_CS_ANIM_CONFETTI
};

void DmBal_Init(Actor* thisx, PlayState* play) {
    DmBal* this = (DmBal*)thisx;

    this->actor.attentionRangeType = ATTENTION_RANGE_1;
    this->actor.cullingVolumeDistance = 3000.0f;
    Actor_SetScale(&this->actor, 0.02f);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gTingleSkel, &gTingleFloatIdleAnim, this->jointTable, this->morphTable,
                       TINGLE_LIMB_MAX);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    this->timer = 60;
    this->eyeIndex = 0;
    this->keepEyesShut = false;
    DmBal_SetupDoNothing(this);
}

void DmBal_Destroy(Actor* thisx, PlayState* play) {
}

void DmBal_SetupDoNothing(DmBal* this) {
    this->actionFunc = DmBal_DoNothing;
}

void DmBal_DoNothing(DmBal* this, PlayState* play) {
}

void DmBal_HandleCutscene(DmBal* this, PlayState* play) {
    static u16 sCueId = 99;
    s32 cueChannel;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_568)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_568);

        if (sCueId != play->csCtx.actorCues[cueChannel]->id) {
            sCueId = play->csCtx.actorCues[cueChannel]->id;
            switch (play->csCtx.actorCues[cueChannel]->id) {
                case 1:
                    this->keepEyesShut = false;
                    this->eyeIndex = 0;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_CS_ANIM_FLOAT_IDLE);
                    break;

                case 2:
                    this->keepEyesShut = true;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_CS_ANIM_HIDE_FACE);
                    break;

                case 3:
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, TINGLE_CS_ANIM_CONFETTI);
                    break;

                default:
                    break;
            }
        } else if (sCueId == 3) {
            if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
                this->keepEyesShut = true;
            } else if (Animation_OnFrame(&this->skelAnime, 29.0f)) {
                this->keepEyesShut = false;
                this->eyeIndex = 0;
            }
        }
        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
        this->actor.home.pos = this->actor.world.pos;
    } else {
        this->keepEyesShut = false;
        this->eyeIndex = 0;
        sCueId = 99;
    }
}

void func_80C1EC60(DmBal* this, PlayState* play) {
    f32 scaleX;

    this->unk_338 += 0x320;
    this->unk_33A += 0x3E8;
    this->scale.y = this->scale.z = Math_CosS(this->unk_338) * 0.1f + 1.0f;
    scaleX = (Math_SinS(this->unk_338) * 0.1f) + 1.0f;
    this->scale.x = SQ(scaleX);
    this->actor.world.pos.y = this->actor.home.pos.y + (Math_SinS(this->unk_338) * 25.0f);
}

void DmBal_UpdateEyes(DmBal* this) {
    if (this->keepEyesShut == true) {
        this->eyeIndex = 1;
    } else if (this->timer >= 4) {
        this->timer--;
    } else if (this->timer != 0) {
        this->eyeIndex = 1;
        this->timer--;
    } else {
        this->eyeIndex = 0;
        this->timer = 60;
    }
}

void DmBal_SpawnPaper(DmBal* this, PlayState* play, Vec3f* pos, Vec3f* velocity, f32 gravity) {
    Actor* paper = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_PAPER, pos->x, pos->y, pos->z, 0, 0, 0, 0);

    if (paper != NULL) {
        paper->velocity = *velocity;
        paper->gravity = gravity;
    }
}

void DmBal_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    DmBal* this = (DmBal*)thisx;

    // Throw confetti
    if (Animation_OnFrame(&this->skelAnime, 29.0f) && (this->skelAnime.animation == &gTingleFloatThrowConfettiAnim)) {
        Vec3f pos = this->actor.world.pos;
        Vec3f velocity = { 0.0f, 9.0f, 0.0f };

        pos.x += 7.0f * Math_SinS(this->actor.shape.rot.y);
        pos.y += 2.5f;
        pos.z += 7.0f * Math_CosS(this->actor.shape.rot.y);
        velocity.x = Math_SinS(this->actor.shape.rot.y) * 5.0f;
        velocity.z = Math_CosS(this->actor.shape.rot.y) * 5.0f;
        DmBal_SpawnPaper(this, play, &pos, &velocity, -0.4f);
        DmBal_SpawnPaper(this, play, &pos, &velocity, -0.5f);
    }
    this->actionFunc(this, play);
    DmBal_HandleCutscene(this, play);
    func_80C1EC60(this, play);
    DmBal_UpdateEyes(this);
    SkelAnime_Update(&this->skelAnime);
}

s32 DmBal_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    DmBal* this = (DmBal*)thisx;
    Vec3s rots;

    if (limbIndex == TINGLE_LIMB_BALLOON) {
        rots.x = Math_SinS(this->unk_33A) * (0x10000 / 18);
        rots.z = Math_CosS(this->unk_33A) * (0x10000 / 18);
        Matrix_RotateZYX(rots.x, 0, rots.z, MTXMODE_APPLY);
        Matrix_Scale(this->scale.x, this->scale.y, this->scale.z, MTXMODE_APPLY);
        Matrix_RotateZS(-rots.z, MTXMODE_APPLY);
        Matrix_RotateXS(-rots.x, MTXMODE_APPLY);
    }
    return false;
}

void DmBal_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
}

static TexturePtr sEyeTextures[] = { gTingleEyeOpenTex, gTingleEyeClosedTex };

void DmBal_Draw(Actor* thisx, PlayState* play) {
    DmBal* this = (DmBal*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_K0(sEyeTextures[this->eyeIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          DmBal_OverrideLimbDraw, DmBal_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
