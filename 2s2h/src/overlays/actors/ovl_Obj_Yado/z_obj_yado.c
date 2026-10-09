/*
 * File: z_obj_yado.c
 * Overlay: ovl_Obj_Yado
 * Description: Stockpot Inn - 2nd Floor Window
 */

#include "z_obj_yado.h"
#include "objects/object_yado_obj/object_yado_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void ObjYado_Init(Actor* thisx, PlayState* play);
void ObjYado_Destroy(Actor* thisx, PlayState* play);
void ObjYado_Update(Actor* thisx, PlayState* play);
void ObjYado_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Yado_Profile = {
    /**/ ACTOR_OBJ_YADO,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_YADO_OBJ,
    /**/ sizeof(ObjYado),
    /**/ ObjYado_Init,
    /**/ ObjYado_Destroy,
    /**/ ObjYado_Update,
    /**/ ObjYado_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

AnimatedMaterial* D_80C16470;

void ObjYado_Init(Actor* thisx, PlayState* play) {
    ObjYado* this = (ObjYado*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    D_80C16470 = Lib_SegmentedToVirtual(object_yado_obj_Matanimheader_0012E8);
    this->isNight = gSaveContext.save.isNight;
}

void ObjYado_Destroy(Actor* thisx, PlayState* play) {
}

void ObjYado_Update(Actor* thisx, PlayState* play) {
    ObjYado* this = (ObjYado*)thisx;

    this->isNight = gSaveContext.save.isNight;
}

void ObjYado_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjYado* this = (ObjYado*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->isNight) {
        gSPSegment(POLY_XLU_DISP++, 0x09, Gfx_PrimColor(play->state.gfxCtx, 128, 95, 95, 70, 155));
        gSPSegment(POLY_OPA_DISP++, 0x0A, Gfx_PrimColor(play->state.gfxCtx, 128, 0, 40, 40, 255));
    } else {
        gSPSegment(POLY_XLU_DISP++, 0x09, Gfx_PrimColor(play->state.gfxCtx, 128, 255, 255, 215, 110));
        gSPSegment(POLY_OPA_DISP++, 0x0A, Gfx_PrimColor(play->state.gfxCtx, 128, 255, 255, 215, 255));
    }

    AnimatedMat_Draw(play, D_80C16470);
    Gfx_DrawDListOpa(play, object_yado_obj_DL_000430);
    Gfx_DrawDListXlu(play, object_yado_obj_DL_000320);

    CLOSE_DISPS(play->state.gfxCtx);
}
