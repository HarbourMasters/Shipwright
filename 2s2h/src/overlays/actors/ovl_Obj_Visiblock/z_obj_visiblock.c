/*
 * File: z_obj_visiblock.c
 * Overlay: ovl_Obj_Visiblock
 * Description: Lens of Truth Platform
 */

#include "z_obj_visiblock.h"
#include "objects/object_visiblock/object_visiblock.h"

#define FLAGS (ACTOR_FLAG_REACT_TO_LENS)

void ObjVisiblock_Init(Actor* thisx, PlayState* play);
void ObjVisiblock_Destroy(Actor* thisx, PlayState* play);
void ObjVisiblock_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Visiblock_Profile = {
    /**/ ACTOR_OBJ_VISIBLOCK,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_VISIBLOCK,
    /**/ sizeof(ObjVisiblock),
    /**/ ObjVisiblock_Init,
    /**/ ObjVisiblock_Destroy,
    /**/ Actor_Noop,
    /**/ ObjVisiblock_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 150, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void ObjVisiblock_Init(Actor* thisx, PlayState* play) {
    ObjVisiblock* this = (ObjVisiblock*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(play, &this->dyna, &gLensOfTruthPlatformCol);
}

void ObjVisiblock_Destroy(Actor* thisx, PlayState* play) {
    ObjVisiblock* this = (ObjVisiblock*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjVisiblock_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListXlu(play, gLensOfTruthPlatformDL);
}
