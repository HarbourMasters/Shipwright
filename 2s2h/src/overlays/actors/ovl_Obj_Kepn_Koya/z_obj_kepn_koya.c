/*
 * File: z_obj_kepn_koya.c
 * Overlay: ovl_Obj_Kepn_Koya
 * Description: Gorman Bros. Buildings
 */

#include "z_obj_kepn_koya.h"
#include "objects/object_kepn_koya/object_kepn_koya.h"

#define FLAGS 0x00000000

void ObjKepnKoya_Init(Actor* thisx, PlayState* play);
void ObjKepnKoya_Destroy(Actor* thisx, PlayState* play);
void ObjKepnKoya_Update(Actor* thisx, PlayState* play);
void ObjKepnKoya_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Kepn_Koya_Profile = {
    /**/ ACTOR_OBJ_KEPN_KOYA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_KEPN_KOYA,
    /**/ sizeof(ObjKepnKoya),
    /**/ ObjKepnKoya_Init,
    /**/ ObjKepnKoya_Destroy,
    /**/ ObjKepnKoya_Update,
    /**/ ObjKepnKoya_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 900, ICHAIN_STOP),
};

void ObjKepnKoya_Init(Actor* thisx, PlayState* play) {
    ObjKepnKoya* this = (ObjKepnKoya*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    Actor_SetScale(&this->dyna.actor, 0.1f);
    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(play, &this->dyna, &object_kepn_koya_Colheader_00805C);
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        Actor_Kill(&this->dyna.actor);
    }
}

void ObjKepnKoya_Destroy(Actor* thisx, PlayState* play) {
    ObjKepnKoya* this = (ObjKepnKoya*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjKepnKoya_Update(Actor* thisx, PlayState* play) {
}

void ObjKepnKoya_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_kepn_koya_DL_003478);
}
