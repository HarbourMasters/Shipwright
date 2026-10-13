/*
 * File: z_bg_f40_flift.c
 * Overlay: ovl_Bg_F40_Flift
 * Description: Stone Tower Temple - Grey Square Stone Elevator
 */

#include "z_bg_f40_flift.h"
#include "objects/object_f40_obj/object_f40_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgF40Flift_Init(Actor* thisx, PlayState* play);
void BgF40Flift_Destroy(Actor* thisx, PlayState* play);
void BgF40Flift_Update(Actor* thisx, PlayState* play);
void BgF40Flift_Draw(Actor* thisx, PlayState* play);

void func_808D75F0(BgF40Flift* this, PlayState* play);
void func_808D7714(BgF40Flift* this, PlayState* play);

ActorProfile Bg_F40_Flift_Profile = {
    /**/ ACTOR_BG_F40_FLIFT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_F40_OBJ,
    /**/ sizeof(BgF40Flift),
    /**/ BgF40Flift_Init,
    /**/ BgF40Flift_Destroy,
    /**/ BgF40Flift_Update,
    /**/ BgF40Flift_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 5000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgF40Flift_Init(Actor* thisx, PlayState* play) {
    BgF40Flift* this = (BgF40Flift*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, &gStoneTowerTempleSquareElevatorCol);
    this->dyna.actor.params = 1;
    this->actionFunc = func_808D75F0;
}

void BgF40Flift_Destroy(Actor* thisx, PlayState* play) {
    BgF40Flift* this = (BgF40Flift*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_808D75F0(BgF40Flift* this, PlayState* play) {
    if (((this->dyna.actor.params == 1) && DynaPolyActor_IsPlayerOnTop(&this->dyna)) ||
        ((this->dyna.actor.params == -1) && !DynaPolyActor_IsPlayerOnTop(&this->dyna))) {
        this->timer = 96;
        this->actionFunc = func_808D7714;
    } else {
        if (this->timer == 0) {
            this->timer = 48;
        }
        this->timer--;
        if (this->dyna.actor.params == 1) {
            this->dyna.actor.world.pos.y =
                (Math_SinF(this->timer * (M_PIf / 24.0f)) * 5.0f) + this->dyna.actor.home.pos.y;
        } else {
            this->dyna.actor.world.pos.y =
                (Math_SinF(this->timer * (M_PIf / 24.0f)) * 5.0f) + (926.8f + this->dyna.actor.home.pos.y);
        }
    }
}

void func_808D7714(BgF40Flift* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
        this->dyna.actor.world.pos.y =
            (((Math_CosF(this->timer * (M_PIf / 96.0f)) * this->dyna.actor.params) + 1.0f) * 463.4f) +
            this->dyna.actor.home.pos.y;
    } else {
        this->dyna.actor.params = -this->dyna.actor.params;
        this->actionFunc = func_808D75F0;
    }
}

void BgF40Flift_Update(Actor* thisx, PlayState* play) {
    BgF40Flift* this = (BgF40Flift*)thisx;

    this->actionFunc(this, play);
}

void BgF40Flift_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gStoneTowerTempleSquareElevatorDL);
}
