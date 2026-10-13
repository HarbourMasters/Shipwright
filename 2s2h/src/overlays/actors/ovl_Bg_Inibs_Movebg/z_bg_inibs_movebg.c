/*
 * File: z_bg_inibs_movebg.c
 * Overlay: ovl_Bg_Inibs_Movebg
 * Description: Twinmold Arena
 */

#include "z_bg_inibs_movebg.h"
#include "objects/object_inibs_object/object_inibs_object.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void BgInibsMovebg_Init(Actor* thisx, PlayState* play);
void BgInibsMovebg_Destroy(Actor* thisx, PlayState* play);
void BgInibsMovebg_Draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Inibs_Movebg_Profile = {
    /**/ ACTOR_BG_INIBS_MOVEBG,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_INIBS_OBJECT,
    /**/ sizeof(BgInibsMovebg),
    /**/ BgInibsMovebg_Init,
    /**/ BgInibsMovebg_Destroy,
    /**/ Actor_Noop,
    /**/ BgInibsMovebg_Draw,
};

Gfx* sOpaDLists[] = { gTwinmoldArenaNormalModeSandDL, gTwinmoldArenaGiantModeSandDL };
Gfx* sXluDLists[] = { gTwinmoldArenaNormalModeCenterPlatformDL, gTwinmoldArenaGiantModeCenterPlatformDL };
AnimatedMaterial* sSandTexAnims[] = { gTwinmoldArenaNormalModeSandTexAnim, gTwinmoldArenaGiantModeSandTexAnim };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void BgInibsMovebg_Init(Actor* thisx, PlayState* play) {
    BgInibsMovebg* this = (BgInibsMovebg*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);

    this->opaDList = sOpaDLists[BG_INIBS_MOVEBG_GET_MODE(thisx)];
    this->xluDList = sXluDLists[BG_INIBS_MOVEBG_GET_MODE(thisx)];
    this->sandTexAnim = sSandTexAnims[BG_INIBS_MOVEBG_GET_MODE(thisx)];
}

void BgInibsMovebg_Destroy(Actor* thisx, PlayState* play) {
    BgInibsMovebg* this = (BgInibsMovebg*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgInibsMovebg_Draw(Actor* thisx, PlayState* play) {
    BgInibsMovebg* this = (BgInibsMovebg*)thisx;
    AnimatedMaterial* sandTexAnim;
    Gfx* opaDList;
    Gfx* xluDList;

    sandTexAnim = this->sandTexAnim;
    if (sandTexAnim != NULL) {
        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(this->sandTexAnim));
    }

    opaDList = this->opaDList;
    if (opaDList != NULL) {
        Gfx_DrawDListOpa(play, this->opaDList);
    }

    xluDList = this->xluDList;
    if (xluDList != NULL) {
        Gfx_DrawDListXlu(play, this->xluDList);
    }
}
