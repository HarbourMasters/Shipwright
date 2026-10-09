/*
 * File: z_en_sth2.c
 * Overlay: ovl_En_Sth2
 * Description: Guy waving at the telescope in Termina Field
 */

#include "z_en_sth2.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnSth2_Init(Actor* thisx, PlayState* play);
void EnSth2_Destroy(Actor* thisx, PlayState* play);
void EnSth2_Update(Actor* thisx, PlayState* play);
void EnSth2_Draw(Actor* thisx, PlayState* play2);

void EnSth2_UpdateSkelAnime(EnSth2* this, PlayState* play);
void EnSth2_UpdateActionFunc(Actor* thisx, PlayState* play);

ActorProfile En_Sth2_Profile = {
    /**/ ACTOR_EN_STH2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnSth2),
    /**/ EnSth2_Init,
    /**/ EnSth2_Destroy,
    /**/ EnSth2_Update,
    /**/ NULL,
};

#include "overlays/ovl_En_Sth2/ovl_En_Sth2.h"

void EnSth2_Init(Actor* thisx, PlayState* play) {
    EnSth2* this = (EnSth2*)thisx;

    this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_STH);
    Actor_SetScale(&this->actor, 0.01f);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    this->unused = 0;

    if (play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON) {
        this->actor.flags |= (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED);
    } else {
        Actor_Kill(&this->actor);
        return;
    }
    this->actionFunc = EnSth2_UpdateSkelAnime;
}

void EnSth2_Destroy(Actor* thisx, PlayState* play) {
}

void EnSth2_UpdateSkelAnime(EnSth2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

void EnSth2_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSth2* this = (EnSth2*)thisx;

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        SkelAnime_InitFlex(play, &this->skelAnime, &gSthSkel, &gEnSth2WavingHandAnim, this->jointTable,
                           this->morphTable, STH_LIMB_MAX);
        Animation_PlayLoop(&this->skelAnime, &gEnSth2WavingHandAnim);
        this->actor.update = EnSth2_UpdateActionFunc;
        this->actor.draw = EnSth2_Draw;
    }
}

void EnSth2_UpdateActionFunc(Actor* thisx, PlayState* play) {
    EnSth2* this = (EnSth2*)thisx;

    this->actionFunc(this, play);
}

s32 EnSth2_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    s32 pad;

    if (limbIndex == STH_LIMB_HEAD) {
        *dList = gEnSth2HeadDL;
    }
    if ((limbIndex == STH_LIMB_CHEST) || (limbIndex == STH_LIMB_LEFT_FOREARM) ||
        (limbIndex == STH_LIMB_RIGHT_FOREARM)) {
        rot->y += TRUNCF_BINANG(Math_SinS(play->state.frames * ((limbIndex * 50) + 0x814)) * 200.0f);
        rot->z += TRUNCF_BINANG(Math_CosS(play->state.frames * ((limbIndex * 50) + 0x940)) * 200.0f);
    }
    return false;
}

void EnSth2_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sFocusOffset = { 700.0f, 400.0f, 0.0f };

    if (limbIndex == STH_LIMB_HEAD) {
        Matrix_MultVec3f(&sFocusOffset, &thisx->focus.pos);

        OPEN_DISPS(play->state.gfxCtx);

        gSPDisplayList(POLY_OPA_DISP++, gEnSth2HairDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnSth2_Draw(Actor* thisx, PlayState* play2) {
    static Color_RGB8 sEnvColors[] = {
        { 190, 110, 0 }, { 0, 180, 110 }, { 0, 255, 80 }, { 255, 160, 60 }, { 190, 230, 250 }, { 240, 230, 120 },
    };
    PlayState* play = play2;
    EnSth2* this = (EnSth2*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08,
               Gfx_EnvColor(play->state.gfxCtx, sEnvColors[1].r, sEnvColors[1].g, sEnvColors[1].b, 255));
    gSPSegment(POLY_OPA_DISP++, 0x09, Gfx_EnvColor(play->state.gfxCtx, 90, 110, 130, 255));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnSth2_OverrideLimbDraw, EnSth2_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
