/*
 * File: z_bg_fu_kaiten.c
 * Overlay: ovl_Bg_Fu_Kaiten
 * Description: Honey & Darling's Shop - Rotating Platform
 */

#include "z_bg_fu_kaiten.h"
#include "objects/object_fu_kaiten/object_fu_kaiten.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void BgFuKaiten_Init(Actor* thisx, PlayState* play);
void BgFuKaiten_Destroy(Actor* thisx, PlayState* play);
void BgFuKaiten_Update(Actor* thisx, PlayState* play);
void BgFuKaiten_Draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Fu_Kaiten_Profile = {
    /**/ ACTOR_BG_FU_KAITEN,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_FU_KAITEN,
    /**/ sizeof(BgFuKaiten),
    /**/ BgFuKaiten_Init,
    /**/ BgFuKaiten_Destroy,
    /**/ BgFuKaiten_Update,
    /**/ BgFuKaiten_Draw,
};

void BgFuKaiten_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgFuKaiten* this = (BgFuKaiten*)thisx;
    CollisionHeader* header = NULL;

    Actor_SetScale(thisx, 1.0f);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
    CollisionHeader_GetVirtual(&object_fu_kaiten_Colheader_002D30, &header);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, header);

    this->bounceHeight = 0.0f;
    this->rotationSpeed = 0;
    this->bounceSpeed = 0;
    this->bounce = 0;
}

void BgFuKaiten_Destroy(Actor* thisx, PlayState* play) {
    BgFuKaiten* this = (BgFuKaiten*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgFuKaiten_UpdateRotation(BgFuKaiten* this) {
    f32 f0;

    this->dyna.actor.shape.rot.y += this->rotationSpeed;
    if (this->rotationSpeed > 0) {
        f0 = this->rotationSpeed * 0.002f;
        Audio_PlaySfx_AtPosWithFreq(&this->dyna.actor.projectedPos, NA_SE_EV_WOOD_GEAR - SFX_FLAG, f0);
    }
}

void BgFuKaiten_UpdateHeight(BgFuKaiten* this) {
    this->bounce += this->bounceSpeed;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + this->elevation + this->bounceHeight;

    this->dyna.actor.world.pos.y -= this->bounceHeight * Math_CosS(this->bounce);
}

void BgFuKaiten_Update(Actor* thisx, PlayState* play) {
    BgFuKaiten* this = (BgFuKaiten*)thisx;

    BgFuKaiten_UpdateRotation(this);
    BgFuKaiten_UpdateHeight(this);
}

void BgFuKaiten_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, object_fu_kaiten_DL_0005D0);
    CLOSE_DISPS(play->state.gfxCtx);
}
