/*
 * File: z_en_hata.c
 * Overlay: ovl_En_Hata
 * Description: Wooden post with red cloth
 */

#include "z_en_hata.h"
#include "objects/object_hata/object_hata.h"

#define FLAGS 0

void EnHata_Init(Actor* thisx, PlayState* play);
void EnHata_Destroy(Actor* thisx, PlayState* play);
void EnHata_Update(Actor* thisx, PlayState* play);
void EnHata_Draw(Actor* thisx, PlayState* play);

const ActorInit En_Hata_InitVars = {
    ACTOR_EN_HATA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_HATA,
    sizeof(EnHata),
    (ActorFunc)EnHata_Init,
    (ActorFunc)EnHata_Destroy,
    (ActorFunc)EnHata_Update,
    (ActorFunc)EnHata_Draw,
    NULL,
};

// Unused Collider and CollisionCheck data
static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000080, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 16, 246, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

void EnHata_Init(Actor* thisx, PlayState* play) {
    EnHata* this = (EnHata*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;
    f32 frameCount = Animation_GetLastFrame(&gFlagpoleFlapAnim);

    Actor_SetScale(&this->dyna.actor, 1.0f / 75.0f);
    SkelAnime_Init(play, &this->skelAnime, &gFlagpoleSkel, &gFlagpoleFlapAnim, NULL, NULL, 0);
    Animation_Change(&this->skelAnime, &gFlagpoleFlapAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gFlagpoleCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->dyna.actor.uncullZoneScale = 500.0f;
    this->dyna.actor.uncullZoneDownward = 550.0f;
    this->dyna.actor.uncullZoneForward = 2200.0f;
    this->invScale = 6;
    this->maxStep = 1000;
    this->minStep = 1;
    this->unk_278 = Rand_ZeroOne() * 0xFFFF;
}

void EnHata_Destroy(Actor* thisx, PlayState* play) {
    EnHata* this = (EnHata*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void EnHata_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnHata* this = (EnHata*)thisx;
    s32 pitch;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f windVec;
    f32 sin;

    SkelAnime_Update(&this->skelAnime);
    // Rotate to hang down by default
    this->limbs[FLAGPOLE_LIMB_FLAG_1_BASE].y = this->limbs[FLAGPOLE_LIMB_FLAG_2_BASE].y = -0x4000;
    windVec.x = play->envCtx.windDirection.x;
    windVec.y = play->envCtx.windDirection.y;
    windVec.z = play->envCtx.windDirection.z;

    if (play->envCtx.windSpeed > 255.0f) {
        play->envCtx.windSpeed = 255.0f;
    }

    if (play->envCtx.windSpeed < 0.0f) {
        play->envCtx.windSpeed = 0.0f;
    }

    if (Rand_ZeroOne() > 0.5f) {
        this->unk_278 += 6000;
    } else {
        this->unk_278 += 3000;
    }

    // Mimic varying wind gusts
    sin = Math_SinS(this->unk_278) * 80.0f;
    pitch = -Math_Vec3f_Pitch(&zeroVec, &windVec);
    pitch = ((s32)((15000 - pitch) * (1.0f - (play->envCtx.windSpeed / (255.0f - sin))))) + pitch;
    Math_SmoothStepToS(&this->limbs[FLAGPOLE_LIMB_FLAG_1_HOIST_END_BASE].y, pitch, this->invScale, this->maxStep,
                       this->minStep);
    this->limbs[FLAGPOLE_LIMB_FLAG_2_HOIST_END_BASE].y = this->limbs[FLAGPOLE_LIMB_FLAG_1_HOIST_END_BASE].y;
    this->limbs[FLAGPOLE_LIMB_FLAG_1_HOIST_END_BASE].z = -Math_Vec3f_Yaw(&zeroVec, &windVec);
    this->limbs[FLAGPOLE_LIMB_FLAG_2_HOIST_END_BASE].z = this->limbs[FLAGPOLE_LIMB_FLAG_1_HOIST_END_BASE].z;
    this->skelAnime.playSpeed = (Rand_ZeroFloat(1.25f) + 2.75f) * (play->envCtx.windSpeed / 255.0f);
}

s32 EnHata_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnHata* this = (EnHata*)thisx;
    Vec3s* limbs;

    if (limbIndex == FLAGPOLE_LIMB_FLAG_2_BASE || limbIndex == FLAGPOLE_LIMB_FLAG_1_BASE ||
        limbIndex == FLAGPOLE_LIMB_FLAG_2_HOIST_END_BASE || limbIndex == FLAGPOLE_LIMB_FLAG_1_HOIST_END_BASE) {
        limbs = this->limbs;
        rot->x += limbs[limbIndex].x;
        rot->y += limbs[limbIndex].y;
        rot->z += limbs[limbIndex].z;
    }
    return false;
}

void EnHata_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
}

void EnHata_Draw(Actor* thisx, PlayState* play) {
    EnHata* this = (EnHata*)thisx;

    Gfx_SetupDL_37Opa(play->state.gfxCtx);
    Matrix_Scale(1.0f, 1.1f, 1.0f, MTXMODE_APPLY);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnHata_OverrideLimbDraw,
                      EnHata_PostLimbDraw, this);
}
