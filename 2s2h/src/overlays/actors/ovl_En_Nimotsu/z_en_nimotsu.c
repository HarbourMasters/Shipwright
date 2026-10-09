/*
 * File: z_en_nimotsu.c
 * Overlay: ovl_En_Nimotsu
 * Description: Bomb Shop Bag Stolen by Sakon
 */

#include "z_en_nimotsu.h"
#include "assets/objects/object_boj/object_boj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnNimotsu_Init(Actor* thisx, PlayState* play);
void EnNimotsu_Destroy(Actor* thisx, PlayState* play);
void EnNimotsu_Update(Actor* thisx, PlayState* play);
void EnNimotsu_Draw(Actor* thisx, PlayState* play);

ActorProfile En_Nimotsu_Profile = {
    /**/ ACTOR_EN_NIMOTSU,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_BOJ,
    /**/ sizeof(EnNimotsu),
    /**/ EnNimotsu_Init,
    /**/ EnNimotsu_Destroy,
    /**/ EnNimotsu_Update,
    /**/ EnNimotsu_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK4,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 10, 30, 0, { 0, 0, 0 } },
};

void EnNimotsu_UpdateCollision(EnNimotsu* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 32.0f, 30.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
}

void EnNimotsu_Init(Actor* thisx, PlayState* play) {
    EnNimotsu* this = (EnNimotsu*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    this->timer = 10;
    this->actor.gravity = -0.5f;

    Actor_SetScale(&this->actor, 0.01f);
}

void EnNimotsu_Destroy(Actor* thisx, PlayState* play) {
    EnNimotsu* this = (EnNimotsu*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnNimotsu_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnNimotsu* this = (EnNimotsu*)thisx;
    Vec3f dustPosition;

    Actor_MoveWithGravity(&this->actor);

    if (!(this->dustDone & 1) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (DECR(this->timer) == 0) {
            this->dustDone |= 1;
        }

        if ((play->state.frames % 3) == 0) {
            dustPosition.x = this->actor.world.pos.x + Rand_CenteredFloat(15.0f);
            dustPosition.y = this->actor.world.pos.y;
            dustPosition.z = this->actor.world.pos.z + Rand_CenteredFloat(15.0f);
            Actor_SpawnFloorDustRing(play, &this->actor, &dustPosition, 20.0f, 0, 2.0f, 0, 0, false);
        }
    }

    EnNimotsu_UpdateCollision(this, play);
}

void EnNimotsu_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnNimotsu* this = (EnNimotsu*)thisx;
    Vec3f position;
    Vec3f scale;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, &gBombShopBagDL);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    position.x = this->actor.world.pos.x + 7.0f;
    position.y = this->actor.world.pos.y;
    position.z = this->actor.world.pos.z + 2.0f;

    scale.x = 0.2f;
    scale.y = 0.2f;
    scale.z = 0.2f;

    func_800BC620(&position, &scale, 255, play);

    CLOSE_DISPS(play->state.gfxCtx);
}
