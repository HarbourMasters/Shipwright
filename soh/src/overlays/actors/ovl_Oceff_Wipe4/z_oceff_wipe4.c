/*
 * File: z_oceff_wipe4.c
 * Overlay: ovl_Oceff_Wipe4
 * Description: Scarecrow's Song and an unused Ocarina Effect
 */

#include "z_oceff_wipe4.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_25)

void OceffWipe4_Init(Actor* thisx, PlayState* play);
void OceffWipe4_Destroy(Actor* thisx, PlayState* play);
void OceffWipe4_Update(Actor* thisx, PlayState* play);
void OceffWipe4_Draw(Actor* thisx, PlayState* play);

const ActorInit Oceff_Wipe4_InitVars = {
    ACTOR_OCEFF_WIPE4,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(OceffWipe4),
    (ActorFunc)OceffWipe4_Init,
    (ActorFunc)OceffWipe4_Destroy,
    (ActorFunc)OceffWipe4_Update,
    (ActorFunc)OceffWipe4_Draw,
    NULL,
};

void OceffWipe4_Init(Actor* thisx, PlayState* play) {
    OceffWipe4* this = (OceffWipe4*)thisx;

    Actor_SetScale(&this->actor, 0.1f);
    this->timer = 0;
    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    osSyncPrintf(VT_FGCOL(CYAN) " WIPE4 arg_data = %d\n" VT_RST, this->actor.params);
}

void OceffWipe4_Destroy(Actor* thisx, PlayState* play) {
    OceffWipe4* this = (OceffWipe4*)thisx;

    func_800876C8(play);
}

void OceffWipe4_Update(Actor* thisx, PlayState* play) {
    OceffWipe4* this = (OceffWipe4*)thisx;

    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    if (this->timer < 50) {
        this->timer++;
    } else {
        Actor_Kill(&this->actor);
    }
}

#include "overlays/ovl_Oceff_Wipe4/ovl_Oceff_Wipe4.h"

void OceffWipe4_Draw(Actor* thisx, PlayState* play) {
    u32 scroll = play->state.frames & 0xFFF;
    OceffWipe4* this = (OceffWipe4*)thisx;
    f32 z;
    u8 alpha;
    s32 pad[2];
    Vec3f eye;
    Vtx* vtxPtr;
    Vec3f vec;

    eye = GET_ACTIVE_CAM(play)->eye;
    Camera_GetSkyboxOffset(&vec, GET_ACTIVE_CAM(play));
    if (this->timer < 16) {
        z = Math_SinS(this->timer * 1024) * 1330.0f;
    } else {
        z = 1330.0f;
    }

    vtxPtr = ResourceMgr_LoadVtxByName(sFrustumVtx);
    if (this->timer >= 30) {
        alpha = 12 * (50 - this->timer);
    } else {
        alpha = 255;
    }

    vtxPtr[1].v.cn[3] = vtxPtr[3].v.cn[3] = vtxPtr[5].v.cn[3] = vtxPtr[7].v.cn[3] = vtxPtr[9].v.cn[3] =
        vtxPtr[11].v.cn[3] = vtxPtr[13].v.cn[3] = vtxPtr[15].v.cn[3] = vtxPtr[17].v.cn[3] = vtxPtr[19].v.cn[3] =
            vtxPtr[21].v.cn[3] = alpha;

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D84(play->state.gfxCtx);

    Matrix_Translate(eye.x + vec.x, eye.y + vec.y, eye.z + vec.z, MTXMODE_NEW);
    Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_Translate(0.0f, 0.0f, -z, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (this->actor.params == OCEFF_WIPE4_UNUSED) {
        gSPDisplayList(POLY_XLU_DISP++, sUnusedMaterialDL);
    } else {
        gSPDisplayList(POLY_XLU_DISP++, sMaterialDL);
    }

    gSPDisplayList(POLY_XLU_DISP++, sMaterial2DL);
    gSPDisplayList(POLY_XLU_DISP++, Gfx_TwoTexScroll(play->state.gfxCtx, 0, scroll * 2, scroll * (-2), 32, 64, 1,
                                                     scroll * (-1), scroll, 32, 32));
    gSPDisplayListOffset(POLY_XLU_DISP++, sMaterial2DL, 11);

    CLOSE_DISPS(play->state.gfxCtx);
}
