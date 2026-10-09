/*
 * File: z_en_hata.c
 * Overlay: ovl_En_Hata
 * Description: Red Flag on Post
 */

#include "z_en_hata.h"
#include "objects/object_hata/object_hata.h"

#define FLAGS 0x00000000

void EnHata_Init(Actor* thisx, PlayState* play);
void EnHata_Destroy(Actor* thisx, PlayState* play);
void EnHata_Update(Actor* thisx, PlayState* play2);
void EnHata_Draw(Actor* thisx, PlayState* play);

ActorProfile En_Hata_Profile = {
    /**/ ACTOR_EN_HATA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_HATA,
    /**/ sizeof(EnHata),
    /**/ EnHata_Init,
    /**/ EnHata_Destroy,
    /**/ EnHata_Update,
    /**/ EnHata_Draw,
};

void EnHata_Init(Actor* thisx, PlayState* play) {
    EnHata* this = (EnHata*)thisx;
    s32 rand;
    f32 endFrame;

    SkelAnime_Init(play, &this->skelAnime, &gFlagpoleSkel, NULL, this->jointTable, this->morphTable, FLAGPOLE_LIMB_MAX);
    endFrame = Animation_GetLastFrame(&gFlagpoleFlapAnim);
    Animation_Change(&this->skelAnime, &gFlagpoleFlapAnim, 1.0f, 0.0f, endFrame, ANIMMODE_LOOP, 0.0f);
    rand = Rand_ZeroFloat(endFrame);
    this->skelAnime.curFrame = rand;
    DynaPolyActor_LoadMesh(play, &this->dyna, &gFlagpoleCol);
    Actor_SetScale(&this->dyna.actor, 0.013f);
    this->dyna.actor.cullingVolumeScale = 500.0f;
    this->dyna.actor.cullingVolumeDownward = 500.0f;
    this->dyna.actor.cullingVolumeDistance = 2200.0f;
}

void EnHata_Destroy(Actor* thisx, PlayState* play) {
    EnHata* this = (EnHata*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void EnHata_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnHata* this = (EnHata*)thisx;
    Vec3f sp34;
    f32 phi_fv0;
    s32 pad;

    phi_fv0 = CLAMP(play->envCtx.windSpeed / 120.0f, 0.0f, 1.0f);
    this->skelAnime.playSpeed = 2.75f * phi_fv0;
    this->skelAnime.playSpeed += 1.0f + Rand_ZeroFloat(1.25f);

    sp34.x = play->envCtx.windDirection.x;
    sp34.y = play->envCtx.windDirection.y + ((1.0f - phi_fv0) * 240.0f);
    sp34.y = CLAMP(sp34.y, -118.0f, 118.0f);
    sp34.z = play->envCtx.windDirection.z;

    phi_fv0 = CLAMP(phi_fv0, 0.1f, 0.4f);
    Math_ApproachF(&this->unk_2A4.x, sp34.x, phi_fv0, 1000.0f);
    Math_ApproachF(&this->unk_2A4.y, sp34.y, phi_fv0, 1000.0f);
    Math_ApproachF(&this->unk_2A4.z, sp34.z, phi_fv0, 1000.0f);

    sp34 = this->unk_2A4;
    this->unk_29C = Math_Vec3f_Pitch(&gZeroVec3f, &sp34);
    this->unk_29C = -this->unk_29C;
    this->unk_2A0 = Math_Vec3f_Yaw(&gZeroVec3f, &sp34);
    this->unk_2A0 += -0x4000;
    SkelAnime_Update(&this->skelAnime);
}

s32 EnHata_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnHata* this = (EnHata*)thisx;

    if ((limbIndex == FLAGPOLE_LIMB_FLAG1_HOIST_END_BASE) || (limbIndex == FLAGPOLE_LIMB_FLAG2_HOIST_END_BASE)) {
        rot->y += this->unk_29C;
        rot->z += this->unk_2A0;
    }
    return false;
}

void EnHata_Draw(Actor* thisx, PlayState* play) {
    EnHata* this = (EnHata*)thisx;

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnHata_OverrideLimbDraw, NULL,
                      &this->dyna.actor);
}
