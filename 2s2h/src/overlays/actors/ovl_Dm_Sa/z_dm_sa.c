/*
 * File: z_dm_sa.c
 * Overlay: ovl_Dm_Sa
 * Description: Glitched early version of Skull Kid stuck in a T-pose
 */

#include "z_dm_sa.h"
#include "objects/object_stk/object_stk.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DmSa_Init(Actor* thisx, PlayState* play);
void DmSa_Destroy(Actor* thisx, PlayState* play);
void DmSa_Update(Actor* thisx, PlayState* play);
void DmSa_Draw(Actor* thisx, PlayState* play);

void DmSa_DoNothing(DmSa* this, PlayState* play);

ActorProfile Dm_Sa_Profile = {
    /**/ ACTOR_DM_SA,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_STK,
    /**/ sizeof(DmSa),
    /**/ DmSa_Init,
    /**/ DmSa_Destroy,
    /**/ DmSa_Update,
    /**/ DmSa_Draw,
};

typedef enum DmSaAnimation {
    /* -1 */ DMSA_ANIM_NONE = -1,
    /*  0 */ DMSA_ANIM_T_POSE,
    /*  1 */ DMSA_ANIM_MAX
} DmSaAnimation;

static AnimationInfo sAnimationInfo[DMSA_ANIM_MAX] = {
    { &gSkullKidTPoseAnim, 1.0f, 0, -1.0f, ANIMMODE_LOOP, 0 }, // DMSA_ANIM_T_POSE
};

void DmSa_ChangeAnim(SkelAnime* skelAnime, AnimationInfo* animInfo, u16 animIndex) {
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

void DmSa_Init(Actor* thisx, PlayState* play) {
    DmSa* this = (DmSa*)thisx;

    this->unk_2E0 = 0;
    this->alpha = 255;
    this->actor.lockOnArrowOffset = 3000.0f;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gSkullKidSkel, NULL, NULL, NULL, 0);
    DmSa_ChangeAnim(&this->skelAnime, &sAnimationInfo[DMSA_ANIM_T_POSE], 0);
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = DmSa_DoNothing;
}

void DmSa_Destroy(Actor* thisx, PlayState* play) {
}

void DmSa_DoNothing(DmSa* this, PlayState* play) {
}

void DmSa_Update(Actor* thisx, PlayState* play) {
    DmSa* this = (DmSa*)thisx;

    SkelAnime_Update(&this->skelAnime);
    this->alpha += 0;
    this->actionFunc(this, play);
}

s32 DmSa_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    return false;
}

void DmSa_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
}

void DmSa_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
}

Gfx* func_80A2EB58(GraphicsContext* gfxCtx, u32 alpha) {
    // 2S2H [Port] Fixing allocation size
    Gfx* gfxHead = GRAPH_ALLOC(gfxCtx, 3 * sizeof(Gfx)); //! @bug this does not allocate enough for 3 Gfx commands;
    Gfx* gfx = gfxHead;

    gDPSetRenderMode(gfx++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
    gDPSetEnvColor(gfx++, 0, 0, 0, alpha);
    gSPEndDisplayList(gfx++);

    return gfxHead;
}

Gfx* func_80A2EBB0(GraphicsContext* gfxCtx, u32 alpha) {
    Gfx* gfxHead = GRAPH_ALLOC(gfxCtx, 2 * sizeof(Gfx));
    Gfx* gfx = gfxHead;

    gDPSetEnvColor(gfx++, 0, 0, 0, alpha);
    gSPEndDisplayList(gfx++);

    return gfxHead;
}

void DmSa_Draw(Actor* thisx, PlayState* play) {
    DmSa* this = (DmSa*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (this->alpha < 255) {
        gSPSegment(POLY_OPA_DISP++, 0x0C, func_80A2EB58(play->state.gfxCtx, this->alpha));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x0C, func_80A2EBB0(play->state.gfxCtx, this->alpha));
    }

    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, DmSa_OverrideLimbDraw, DmSa_PostLimbDraw,
                                   DmSa_TransformLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
