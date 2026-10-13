/*
 * File: z_obj_y2lift.c
 * Overlay: ovl_Obj_Y2lift
 * Description: Unused elevator platform
 */

#include "z_obj_y2lift.h"
#include "objects/object_kaizoku_obj/object_kaizoku_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjY2lift_Init(Actor* thisx, PlayState* play);
void ObjY2lift_Destroy(Actor* thisx, PlayState* play);
void ObjY2lift_Update(Actor* thisx, PlayState* play);
void ObjY2lift_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Y2lift_Profile = {
    /**/ ACTOR_OBJ_Y2LIFT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_KAIZOKU_OBJ,
    /**/ sizeof(ObjY2lift),
    /**/ ObjY2lift_Init,
    /**/ ObjY2lift_Destroy,
    /**/ ObjY2lift_Update,
    /**/ ObjY2lift_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 800, ICHAIN_STOP),
};

void ObjY2lift_Init(Actor* thisx, PlayState* play) {
    ObjY2lift* this = (ObjY2lift*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, &gPirateLiftPlatformCol);
}

void ObjY2lift_Destroy(Actor* thisx, PlayState* play) {
    ObjY2lift* this = (ObjY2lift*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjY2lift_Update(Actor* thisx, PlayState* play) {
    ObjY2lift* this = (ObjY2lift*)thisx;
    f32 temp_fv0 = this->dyna.actor.world.pos.y;
    f32 targetVelocityY = 0.0f;
    s32 isPlayerOnTop = DynaPolyActor_IsPlayerOnTop(&this->dyna);

    if (isPlayerOnTop || DynaPolyActor_IsActorOnTop(&this->dyna)) {
        if (!this->unk15D) {
            this->unk15D = true;
            this->unk15F = 12;
        } else if ((this->unk15F == 0) && isPlayerOnTop) {
            this->unk15C = 16;
        }
    } else {
        this->unk15D = false;
    }
    if (DECR(this->unk15C) != 0) {
        temp_fv0 = this->dyna.actor.home.pos.y + 180.0f;
        targetVelocityY = 2.0f;
    } else if (!isPlayerOnTop && (this->dyna.actor.velocity.y <= 0.0f)) {
        temp_fv0 = this->dyna.actor.home.pos.y;
        targetVelocityY = -2.0f;
    }
    Math_StepToF(&this->dyna.actor.velocity.y, targetVelocityY, 0.1f);
    this->dyna.actor.world.pos.y += this->dyna.actor.velocity.y;
    if (((this->dyna.actor.world.pos.y - temp_fv0) * targetVelocityY) >= 0.0f) {
        this->dyna.actor.world.pos.y = temp_fv0;
        this->dyna.actor.velocity.y = 0.0f;
        if (!this->unk15E) {
            this->unk15E = true;
            this->unk15F = 12;
        }
    } else {
        this->unk15E = false;
        Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_PLATE_LIFT_LEVEL - SFX_FLAG);
    }
    if (DECR(this->unk15F) != 0) {
        this->dyna.actor.shape.yOffset = (2.0f * (this->unk15F & 1)) * this->unk15F;
    }
}

void ObjY2lift_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gPirateLiftPlatformDL);
}
