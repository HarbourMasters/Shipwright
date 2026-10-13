/*
 * File: z_en_ds2n.c
 * Overlay: ovl_En_Ds2n
 * Description: Potion Shop Owner from OoT (unused)
 *
 *   This actor was placed in the Staff Office of Stockpot inn
 *   but does not spawn because their object is missing.
 */

#include "z_en_ds2n.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnDs2n_Init(Actor* thisx, PlayState* play);
void EnDs2n_Destroy(Actor* thisx, PlayState* play);
void EnDs2n_Update(Actor* thisx, PlayState* play);
void EnDs2n_Draw(Actor* thisx, PlayState* play);

void EnDs2n_Idle(EnDs2n* this, PlayState* play);

ActorProfile En_Ds2n_Profile = {
    /**/ ACTOR_EN_DS2N,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DS2N,
    /**/ sizeof(EnDs2n),
    /**/ EnDs2n_Init,
    /**/ EnDs2n_Destroy,
    /**/ EnDs2n_Update,
    /**/ EnDs2n_Draw,
};

typedef enum {
    /* 0 */ ENDS2N_ANIM_IDLE,
    /* 1 */ ENDS2N_ANIM_MAX
} EnDs2nAnimation;

static AnimationInfo sAnimationInfo[ENDS2N_ANIM_MAX] = {
    { &gDs2nIdleAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f }, // ENDS2N_ANIM_IDLE
};

void EnDs2n_SetupIdle(EnDs2n* this) {
    this->blinkTimer = 20;
    this->blinkState = 0;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENDS2N_ANIM_IDLE);
    this->actionFunc = EnDs2n_Idle;
}

void EnDs2n_Idle(EnDs2n* this, PlayState* play) {
    SubS_UpdateFidgetTables(play, this->fidgetTableY, this->fidgetTableZ, DS2N_LIMB_MAX);
}

void EnDs2n_UpdateEyes(EnDs2n* this) {
    s16 nextBlinkTime = this->blinkTimer - 1;

    if (nextBlinkTime >= 3) {
        this->blinkState = 0;
        this->blinkTimer = nextBlinkTime;
    } else if (nextBlinkTime == 0) {
        this->blinkState = 2;
        this->blinkTimer = (s32)(Rand_ZeroOne() * 60.0f) + 20;
    } else {
        this->blinkState = 1;
        this->blinkTimer = nextBlinkTime;
    }
}

void EnDs2n_Init(Actor* thisx, PlayState* play) {
    EnDs2n* this = (EnDs2n*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gDs2nSkel, &gDs2nIdleAnim, NULL, NULL, 0);
    EnDs2n_SetupIdle(this);
}

void EnDs2n_Destroy(Actor* thisx, PlayState* play) {
    EnDs2n* this = (EnDs2n*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
}

void EnDs2n_Update(Actor* thisx, PlayState* play) {
    EnDs2n* this = (EnDs2n*)thisx;

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    SkelAnime_Update(&this->skelAnime);

    Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    EnDs2n_UpdateEyes(this);
}

s32 EnDs2n_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnDs2n* this = (EnDs2n*)thisx;

    if (limbIndex == DS2N_LIMB_HEAD) {
        Matrix_RotateXS(this->headRot.y, MTXMODE_APPLY);
    }

    return false;
}

void EnDs2n_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnDs2n* this = (EnDs2n*)thisx;
    Vec3f focusOffset = { 0.0f, 0.0f, 0.0f };

    if ((limbIndex == DS2N_LIMB_HIPS) || (limbIndex == DS2N_LIMB_LEFT_UPPER_ARM) ||
        (limbIndex == DS2N_LIMB_RIGHT_UPPER_ARM)) {
        rot->y += TRUNCF_BINANG(Math_SinS(this->fidgetTableY[limbIndex])) * 200;
        rot->z += TRUNCF_BINANG(Math_CosS(this->fidgetTableZ[limbIndex])) * 200;
    }

    if (limbIndex == DS2N_LIMB_HEAD) {
        Matrix_MultVec3f(&focusOffset, &thisx->focus.pos);
    }
}

static TexturePtr sEyeTextures[] = { gDs2nEyeOpenTex, gDs2nEyeHalfTex, gDs2nEyeClosedTex };

void EnDs2n_Draw(Actor* thisx, PlayState* play) {
    EnDs2n* this = (EnDs2n*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->blinkState]));

    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sEyeTextures[this->blinkState]));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnDs2n_OverrideLimbDraw, EnDs2n_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
