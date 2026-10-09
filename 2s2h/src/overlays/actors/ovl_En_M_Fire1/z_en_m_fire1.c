/*
 * File: z_en_m_fire1.c
 * Overlay: ovl_En_M_Fire1
 * Description: Deku Nut Effect
 */

#include "z_en_m_fire1.h"

#define FLAGS 0x00000000

void EnMFire1_Init(Actor* thisx, PlayState* play);
void EnMFire1_Destroy(Actor* thisx, PlayState* play);
void EnMFire1_Update(Actor* thisx, PlayState* play);

ActorProfile En_M_Fire1_Profile = {
    /**/ ACTOR_EN_M_FIRE1,
    /**/ ACTORCAT_MISC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnMFire1),
    /**/ EnMFire1_Init,
    /**/ EnMFire1_Destroy,
    /**/ EnMFire1_Update,
    /**/ NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000001, 0x00, 0x01 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { 100, 100, 0, { 0, 0, 0 } },
};

void EnMFire1_Init(Actor* thisx, PlayState* play) {
    EnMFire1* this = (EnMFire1*)thisx;
    s32 pad;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    if (this->actor.params != 0) {
        this->collider.elem.atDmgInfo.dmgFlags = 0x40000;
    }
}

void EnMFire1_Destroy(Actor* thisx, PlayState* play) {
    EnMFire1* this = (EnMFire1*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnMFire1_Update(Actor* thisx, PlayState* play) {
    EnMFire1* this = (EnMFire1*)thisx;
    s32 pad;

    if (Math_StepToF(&this->timer, 1.0f, 0.2f)) {
        Actor_Kill(&this->actor);
        return;
    }

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
}
