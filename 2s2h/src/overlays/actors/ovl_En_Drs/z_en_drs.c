/*
 * File: z_en_drs.c
 * Overlay: ovl_En_Drs
 * Description: Anju's Wedding Dress Mannequin
 */

#include "z_en_drs.h"

#define FLAGS 0x00000000

void EnDrs_Init(Actor* thisx, PlayState* play);
void EnDrs_Destroy(Actor* thisx, PlayState* play);
void EnDrs_Update(Actor* thisx, PlayState* play);
void EnDrs_Draw(Actor* thisx, PlayState* play);

void EnDrs_Idle(EnDrs* this, PlayState* play);

ActorProfile En_Drs_Profile = {
    /**/ ACTOR_EN_DRS,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_DRS,
    /**/ sizeof(EnDrs),
    /**/ EnDrs_Init,
    /**/ EnDrs_Destroy,
    /**/ EnDrs_Update,
    /**/ NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT1,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 16, 62, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum {
    /* 0 */ WEDDING_DRESS_MANNEQUIN_ANIM_IDLE,
    /* 1 */ WEDDING_DRESS_MANNEQUIN_ANIM_MAX
} WeddingDressMannequinAnimation;

static AnimationInfoS sAnimationInfo[WEDDING_DRESS_MANNEQUIN_ANIM_MAX] = {
    { &gWeddingDressMannequinIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 }, // WEDDING_DRESS_MANNEQUIN_ANIM_IDLE
};

void EnDrs_CollisionUpdate(EnDrs* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnDrs_Setup(EnDrs* this, PlayState* play) {
    s32 pad[2];

    if ((this->moonMaskObjectSlot > OBJECT_SLOT_NONE) && SubS_IsObjectLoaded(this->moonMaskObjectSlot, play)) {
        ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
        SkelAnime_InitFlex(play, &this->skelAnime, &gWeddingDressMannequinSkel, NULL, this->jointTable,
                           this->morphTable, WEDDING_DRESS_MANNEQUIN_LIMB_MAX);
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, WEDDING_DRESS_MANNEQUIN_ANIM_IDLE);
        Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
        CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);
        Actor_SetScale(&this->actor, 0.01f);
        this->actor.draw = EnDrs_Draw;
        this->actionFunc = EnDrs_Idle;
    }
}

void EnDrs_Idle(EnDrs* this, PlayState* play) {
}

void EnDrs_Init(Actor* thisx, PlayState* play) {
    EnDrs* this = (EnDrs*)thisx;

    this->moonMaskObjectSlot = SubS_GetObjectSlot(OBJECT_MSMO, play);
    this->actionFunc = EnDrs_Setup;
}

void EnDrs_Destroy(Actor* thisx, PlayState* play) {
    EnDrs* this = (EnDrs*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnDrs_Update(Actor* thisx, PlayState* play) {
    EnDrs* this = (EnDrs*)thisx;

    this->actionFunc(this, play);
    if (this->actor.draw != NULL) {
        SkelAnime_Update(&this->skelAnime);
        EnDrs_CollisionUpdate(this, play);
    }
}

void EnDrs_PostLimbDraw(PlayState* play2, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnDrs* this = (EnDrs*)thisx;
    PlayState* play = play2;
    s8 temp = this->moonMaskObjectSlot;
    s8 temp2 = this->actor.objectSlot;

    // Anju removes the Moon Mask at the start of the Couple's Mask cutscene
    // after that it will no longer be rendered.
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_COUPLES_MASK_CUTSCENE_STARTED) &&
        (limbIndex == WEDDING_DRESS_MANNEQUIN_LIMB_MASK)) {
        OPEN_DISPS(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[temp].segment);
        gSPDisplayList(POLY_OPA_DISP++, &gMoonMaskDL);
        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[temp2].segment);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnDrs_Draw(Actor* thisx, PlayState* play) {
    EnDrs* this = (EnDrs*)thisx;

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          EnDrs_PostLimbDraw, &this->actor);
}
