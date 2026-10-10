/*
 * File: z_bg_mbar_chair.c
 * Overlay: ovl_Bg_Mbar_Chair
 * Description: Milk Bar - Chair
 */

#include "z_bg_mbar_chair.h"
#include "objects/object_mbar_obj/object_mbar_obj.h"

#define FLAGS 0x00000000

void BgMbarChair_Init(Actor* thisx, PlayState* play);
void BgMbarChair_Destroy(Actor* thisx, PlayState* play);
void BgMbarChair_Update(Actor* thisx, PlayState* play);
void BgMbarChair_Draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Mbar_Chair_Profile = {
    /**/ ACTOR_BG_MBAR_CHAIR,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_MBAR_OBJ,
    /**/ sizeof(BgMbarChair),
    /**/ BgMbarChair_Init,
    /**/ BgMbarChair_Destroy,
    /**/ BgMbarChair_Update,
    /**/ BgMbarChair_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 60, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 80, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgMbarChair_Init(Actor* thisx, PlayState* play) {
    BgMbarChair* this = (BgMbarChair*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(play, &this->dyna, &object_mbar_obj_Colheader_0019B4);
}

void BgMbarChair_Destroy(Actor* thisx, PlayState* play) {
    BgMbarChair* this = (BgMbarChair*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgMbarChair_Update(Actor* thisx, PlayState* play) {
}

void BgMbarChair_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_mbar_obj_DL_000288);
}
