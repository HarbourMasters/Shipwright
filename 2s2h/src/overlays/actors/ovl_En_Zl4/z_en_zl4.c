/*
 * File: z_en_zl4.c
 * Overlay: ovl_En_Zl4
 * Description: Glitched early version of Skull Kid stuck in a T-pose
 */

#include "z_en_zl4.h"
#include "objects/object_stk/object_stk.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnZl4_Init(Actor* thisx, PlayState* play);
void EnZl4_Destroy(Actor* thisx, PlayState* play);
void EnZl4_Update(Actor* thisx, PlayState* play);
void EnZl4_Draw(Actor* thisx, PlayState* play);

void EnZl4_DoNothing(EnZl4* this, PlayState* play);

ActorProfile En_Zl4_Profile = {
    /**/ ACTOR_EN_ZL4,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_STK,
    /**/ sizeof(EnZl4),
    /**/ EnZl4_Init,
    /**/ EnZl4_Destroy,
    /**/ EnZl4_Update,
    /**/ EnZl4_Draw,
};

typedef enum EnZl4Animation {
    /* -1 */ ENZL4_ANIM_NONE = -1,
    /*  0 */ ENZL4_ANIM_T_POSE,
    /*  1 */ ENZL4_ANIM_MAX
} EnZl4Animation;

static AnimationInfo sAnimationInfo[ENZL4_ANIM_MAX] = {
    { &gSkullKidTPoseAnim, 1.0f, 0, -1.0f, ANIMMODE_LOOP, 0 }, // ENZL4_ANIM_T_POSE
};

void EnZl4_ChangeAnim(SkelAnime* skelAnime, AnimationInfo* animInfo, u16 animIndex) {
    f32 endFrame;

    animInfo += animIndex;

    if (animInfo->frameCount < 0.0f) {
        endFrame = Animation_GetLastFrame(animInfo->animation);
    } else {
        endFrame = animInfo->frameCount;
    }

    Animation_Change(skelAnime, animInfo->animation, animInfo->playSpeed, animInfo->startFrame, endFrame,
                     animInfo->mode, animInfo->morphFrames);
}

void EnZl4_Init(Actor* thisx, PlayState* play) {
    EnZl4* this = (EnZl4*)thisx;

    this->unk_2E0 = 0;
    this->alpha = 255;
    this->actor.lockOnArrowOffset = 3000.0f;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gSkullKidSkel, NULL, NULL, NULL, 0);
    EnZl4_ChangeAnim(&this->skelAnime, &sAnimationInfo[ENZL4_ANIM_T_POSE], 0);
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = EnZl4_DoNothing;
}

void EnZl4_Destroy(Actor* thisx, PlayState* play) {
}

void EnZl4_DoNothing(EnZl4* this, PlayState* play) {
}

void EnZl4_Update(Actor* thisx, PlayState* play) {
    EnZl4* this = (EnZl4*)thisx;

    SkelAnime_Update(&this->skelAnime);
    this->alpha += 0;
    this->actionFunc(this, play);
}

s32 EnZl4_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    Vec3f D_809A1F98 = { 0.0f, 0.0f, 0.0f };

    return false;
}

void EnZl4_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
}

void EnZl4_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
}

Gfx* func_809A1DD0(GraphicsContext* gfxCtx, u32 alpha) {
    Gfx* gfxHead = GRAPH_ALLOC(gfxCtx, 2 * sizeof(Gfx)); //! @bug this does not allocate enough for 3 Gfx commands;
    Gfx* gfx = gfxHead;

    gDPSetRenderMode(gfx++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
    gDPSetEnvColor(gfx++, 0, 0, 0, alpha);
    gSPEndDisplayList(gfx++);

    return gfxHead;
}

Gfx* func_809A1E28(GraphicsContext* gfxCtx, u32 alpha) {
    Gfx* gfxHead = GRAPH_ALLOC(gfxCtx, 2 * sizeof(Gfx));
    Gfx* gfx = gfxHead;

    gDPSetEnvColor(gfx++, 0, 0, 0, alpha);
    gSPEndDisplayList(gfx++);

    return gfxHead;
}

void EnZl4_Draw(Actor* thisx, PlayState* play) {
    EnZl4* this = (EnZl4*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (this->alpha < 255) {
        gSPSegment(POLY_OPA_DISP++, 0x0C, func_809A1DD0(play->state.gfxCtx, this->alpha));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x0C, func_809A1E28(play->state.gfxCtx, this->alpha));
    }

    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, EnZl4_OverrideLimbDraw, EnZl4_PostLimbDraw,
                                   EnZl4_TransformLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
