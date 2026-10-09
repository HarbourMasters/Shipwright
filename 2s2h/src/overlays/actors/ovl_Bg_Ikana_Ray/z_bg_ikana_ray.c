/*
 * File: z_bg_ikana_ray.c
 * Overlay: ovl_Bg_Ikana_Ray
 * Description: Large light ray in Stone Tower Temple
 */

#include "z_bg_ikana_ray.h"
#include "objects/object_ikana_obj/object_ikana_obj.h"

#define FLAGS 0x00000000

void BgIkanaRay_Init(Actor* thisx, PlayState* play);
void BgIkanaRay_Destroy(Actor* thisx, PlayState* play);
void BgIkanaRay_Update(Actor* thisx, PlayState* play);
void BgIkanaRay_Draw(Actor* thisx, PlayState* play);

void BgIkanaRay_SetDeactivated(BgIkanaRay* this);
void BgIkanaRay_UpdateCheckForActivation(BgIkanaRay* this, PlayState* play);
void BgIkanaRay_SetActivated(BgIkanaRay* this);
void BgIkanaRay_UpdateActivated(BgIkanaRay* this, PlayState* play);

ActorProfile Bg_Ikana_Ray_Profile = {
    /**/ ACTOR_BG_IKANA_RAY,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_IKANA_OBJ,
    /**/ sizeof(BgIkanaRay),
    /**/ BgIkanaRay_Init,
    /**/ BgIkanaRay_Destroy,
    /**/ BgIkanaRay_Update,
    /**/ BgIkanaRay_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_OTHER,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00200000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 90, 420, -420, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgIkanaRay_Init(Actor* thisx, PlayState* play) {
    BgIkanaRay* this = (BgIkanaRay*)thisx;
    ColliderCylinder* collision = &this->collision;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitCylinder(play, collision);
    Collider_SetCylinder(play, collision, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collision);

    this->animatedTextures = Lib_SegmentedToVirtual(object_ikana_obj_Matanimheader_001228);

    if (Flags_GetSwitch(play, BGIKANARAY_GET_SWITCH_FLAG(&this->actor))) {
        BgIkanaRay_SetActivated(this);
    } else {
        BgIkanaRay_SetDeactivated(this);
    }
}

void BgIkanaRay_Destroy(Actor* thisx, PlayState* play) {
    BgIkanaRay* this = (BgIkanaRay*)thisx;

    ColliderCylinder* collision = &this->collision;
    Collider_DestroyCylinder(play, collision);
}

void BgIkanaRay_SetDeactivated(BgIkanaRay* this) {
    this->actor.draw = NULL;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actionFunc = BgIkanaRay_UpdateCheckForActivation;
}

void BgIkanaRay_UpdateCheckForActivation(BgIkanaRay* this, PlayState* play) {
    if (Flags_GetSwitch(play, BGIKANARAY_GET_SWITCH_FLAG(&this->actor))) {
        BgIkanaRay_SetActivated(this);
    }
}

void BgIkanaRay_SetActivated(BgIkanaRay* this) {
    this->actor.draw = BgIkanaRay_Draw;
    this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actionFunc = BgIkanaRay_UpdateActivated;
}

void BgIkanaRay_UpdateActivated(BgIkanaRay* this, PlayState* play) {
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collision.base);
}

void BgIkanaRay_Update(Actor* thisx, PlayState* play) {
    BgIkanaRay* this = (BgIkanaRay*)thisx;

    this->actionFunc(this, play);
}

void BgIkanaRay_Draw(Actor* thisx, PlayState* play) {
    BgIkanaRay* this = (BgIkanaRay*)thisx;

    AnimatedMat_Draw(play, this->animatedTextures);
    Gfx_DrawDListXlu(play, object_ikana_obj_DL_001100);
}
