/*
 * File: z_dm_char04.c
 * Overlay: ovl_Dm_Char04
 * Description: Unused(?) Tatl and Tael actors
 */

#include "z_dm_char04.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DmChar04_Init(Actor* thisx, PlayState* play);
void DmChar04_Destroy(Actor* thisx, PlayState* play);
void DmChar04_Update(Actor* thisx, PlayState* play);
void DmChar04_Draw(Actor* thisx, PlayState* play);

void DmChar04_HandleCutscene(DmChar04* this, PlayState* play);

ActorProfile Dm_Char04_Profile = {
    /**/ ACTOR_DM_CHAR04,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(DmChar04),
    /**/ DmChar04_Init,
    /**/ DmChar04_Destroy,
    /**/ DmChar04_Update,
    /**/ DmChar04_Draw,
};

typedef enum DmChar04Animation {
    /* 0 */ DMCHAR04_ANIM_0,
    /* 1 */ DMCHAR04_ANIM_1,
    /* 2 */ DMCHAR04_ANIM_MAX
} DmChar04Animation;

static AnimationInfo sAnimationInfo[DMCHAR04_ANIM_MAX] = {
    { &gameplay_keep_Anim_02B2E8, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f }, // DMCHAR04_ANIM_0
    { &gameplay_keep_Anim_029140, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f }, // DMCHAR04_ANIM_1
};

void DmChar04_ChangeAnim(SkelAnime* skelAnime, AnimationInfo* animInfo, u16 animIndex) {
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

static Color_RGBAf sPrimColors[] = {
    { 250.0f, 255.0f, 230.0f, 255.0f },
    { 10.0f, 10.0f, 40.0f, 255.0f },
    { 255.0f, 235.0f, 220.0f, 255.0f },
};
static Color_RGBAf sEnvColors[] = {
    { 220.0f, 160.0f, 80.0f, 255.0f },
    { 120.0f, 255.0f, 255.0f, 255.0f },
    { 255.0f, 235.0f, 220.0f, 255.0f },
};

void DmChar04_Init(Actor* thisx, PlayState* play) {
    DmChar04* this = (DmChar04*)thisx;

    this->primColors = sPrimColors[this->actor.params];
    this->envColors = sEnvColors[this->actor.params];
    this->actor.lockOnArrowOffset = 3000.0f;
    this->cueId = 99;
    this->timer = this->actor.params << 0xB;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    SkelAnime_Init(play, &this->skelAnime, &gameplay_keep_Skel_02AF58, &gameplay_keep_Anim_029140, this->jointTable,
                   this->morphTable, FAIRY_LIMB_MAX);
    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 15.0f);
    DmChar04_ChangeAnim(&this->skelAnime, &sAnimationInfo[DMCHAR04_ANIM_0], 0);
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = DmChar04_HandleCutscene;
}

void DmChar04_Destroy(Actor* thisx, PlayState* play) {
}

void DmChar04_HandleCutscene(DmChar04* this, PlayState* play) {
    u16 cueType = CS_CMD_ACTOR_CUE_113 + this->actor.params;

    if (Cutscene_IsCueInChannel(play, cueType)) {
        s32 cueChannel = Cutscene_GetCueChannel(play, cueType);

        if (play->csCtx.curFrame == play->csCtx.actorCues[cueChannel]->startFrame) {
            if (this->cueId != play->csCtx.actorCues[cueChannel]->id) {
                this->cueId = play->csCtx.actorCues[cueChannel]->id;
                if (play->csCtx.actorCues[cueChannel]->id == 1) {
                    this->animIndex = DMCHAR04_ANIM_0;
                } else {
                    this->animIndex = DMCHAR04_ANIM_0;
                }
                DmChar04_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
            }
        }
        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
    } else {
        this->cueId = 99;
    }
}

void DmChar04_Update(Actor* thisx, PlayState* play) {
    DmChar04* this = (DmChar04*)thisx;

    SkelAnime_Update(&this->skelAnime);
    this->actionFunc(this, play);
    this->timer++;
}
static Vec3f D_80AAC4F0 = { 0.0f, 0.0f, 0.0f };

s32 DmChar04_OverrideLimbDraw(PlayState* play2, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                              Gfx** gfx) {
    PlayState* play = play2;
    f32 sp28;
    Vec3f sp1C;
    DmChar04* this = (DmChar04*)thisx;

    if (limbIndex == FAIRY_LIMB_6) {
        sp28 = ((Math_SinS(this->timer * 0x1000) * 0.1f) + 1.0f) * 0.012f * (this->actor.scale.x * (1.0f / 0.008f));
        Matrix_MultVec3f(&D_80AAC4F0, &sp1C);
        Matrix_Translate(sp1C.x, sp1C.y, sp1C.z, MTXMODE_NEW);
        Matrix_Scale(sp28, sp28, sp28, MTXMODE_APPLY);
    }
    return false;
}

void DmChar04_Draw(Actor* thisx, PlayState* play) {
    Gfx* gfx = GRAPH_ALLOC(play->state.gfxCtx, 4 * sizeof(Gfx));
    s32 alpha;
    s32 pad;
    DmChar04* this = (DmChar04*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL27_Xlu(play->state.gfxCtx);
    alpha = (this->timer * 50) & 0x1FF;
    if (alpha > 255) {
        alpha = 511 - alpha;
    }
    gSPSegment(POLY_XLU_DISP++, 0x08, &gfx[0]);
    gDPPipeSync(gfx++);
    gDPSetPrimColor(gfx++, 0, 0x01, (u8)this->primColors.r, (u8)this->primColors.g, (u8)this->primColors.b,
                    (u8)(this->primColors.a * 1));
    gDPSetRenderMode(gfx++, G_RM_PASS, G_RM_ZB_CLD_SURF2);
    gSPEndDisplayList(gfx);
    gDPSetEnvColor(POLY_XLU_DISP++, (u8)(s8)this->envColors.r, (u8)(s8)this->envColors.g, (u8)(s8)this->envColors.b,
                   (u8)(s8)((f32)alpha * 1));
    gDPSetDither(POLY_XLU_DISP++, G_CD_BAYER);
    POLY_XLU_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   DmChar04_OverrideLimbDraw, NULL, &this->actor, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}
