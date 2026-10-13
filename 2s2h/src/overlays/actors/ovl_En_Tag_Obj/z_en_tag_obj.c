/*
 * File: z_en_tag_obj.c
 * Overlay: ovl_En_Tag_Obj
 * Description: Unused Seahorse Spawner
 */

#include "z_en_tag_obj.h"
#include "overlays/actors/ovl_En_Ot/z_en_ot.h"

#define FLAGS 0x00000000

void EnTagObj_Init(Actor* thisx, PlayState* play);
void EnTagObj_Destroy(Actor* thisx, PlayState* play);
void EnTagObj_Update(Actor* thisx, PlayState* play);

static ColliderCylinderInit sUnusedColliderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
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
    { 20, 30, 0, { 0, 0, 0 } },
};

ActorProfile En_Tag_Obj_Profile = {
    /**/ ACTOR_EN_TAG_OBJ,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnTagObj),
    /**/ EnTagObj_Init,
    /**/ EnTagObj_Destroy,
    /**/ EnTagObj_Update,
    /**/ NULL,
};

void EnTagObj_Init(Actor* thisx, PlayState* play) {
    EnTagObj* this = (EnTagObj*)thisx;

    this->hasSpawnedSeahorse = false;
}

void EnTagObj_Destroy(Actor* thisx, PlayState* play) {
}

void EnTagObj_Update(Actor* thisx, PlayState* play) {
    EnTagObj* this = (EnTagObj*)thisx;

    if (!this->hasSpawnedSeahorse) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_OT, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, 0, 0, SEAHORSE_PARAMS(SEAHORSE_TYPE_0, 0, 0));
        this->hasSpawnedSeahorse = true;
    }
}
