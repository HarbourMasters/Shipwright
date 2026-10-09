/*
 * File: z_en_hit_tag.c
 * Overlay: ovl_En_Hit_Tag
 * Description: Invisible hitbox that can spawn rupees
 */

#include "z_en_hit_tag.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnHitTag_Init(Actor* thisx, PlayState* play);
void EnHitTag_Destroy(Actor* thisx, PlayState* play);
void EnHitTag_Update(Actor* thisx, PlayState* play);

void EnHitTag_WaitForHit(EnHitTag* this, PlayState* play);

ActorProfile En_Hit_Tag_Profile = {
    /**/ ACTOR_EN_HIT_TAG,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnHitTag),
    /**/ EnHitTag_Init,
    /**/ EnHitTag_Destroy,
    /**/ EnHitTag_Update,
    /**/ NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 16, 32, 0, { 0, 0, 0 } },
};

void EnHitTag_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHitTag* this = (EnHitTag*)thisx;

    Actor_SetScale(&this->actor, 1.0f);
    this->actionFunc = EnHitTag_WaitForHit;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    if (Flags_GetSwitch(play, ENHITTAG_GET_SWITCH_FLAG(thisx))) {
        Actor_Kill(&this->actor);
    }
}

void EnHitTag_Destroy(Actor* thisx, PlayState* play) {
    EnHitTag* this = (EnHitTag*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnHitTag_WaitForHit(EnHitTag* this, PlayState* play) {
    Vec3f dropLocation;
    s32 i;

    if (this->collider.base.acFlags & AC_HIT) {
        Audio_PlaySfx(NA_SE_SY_GET_RUPY);
        Actor_Kill(&this->actor);
        dropLocation.x = this->actor.world.pos.x;
        dropLocation.y = this->actor.world.pos.y;
        dropLocation.z = this->actor.world.pos.z;

        for (i = 0; i < 3; i++) {
            Item_DropCollectible(play, &dropLocation, ITEM00_RUPEE_GREEN);
        }
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnHitTag_Update(Actor* thisx, PlayState* play) {
    EnHitTag* this = (EnHitTag*)thisx;
    this->actionFunc(this, play);
}
