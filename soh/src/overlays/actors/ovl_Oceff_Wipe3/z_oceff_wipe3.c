/*
 * File: z_oceff_wipe3.c
 * Overlay: ovl_Oceff_Wipe3
 * Description: Saria's Song Effect
 */

#include "z_oceff_wipe3.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_25)

void OceffWipe3_Init(Actor* thisx, PlayState* play);
void OceffWipe3_Destroy(Actor* thisx, PlayState* play);
void OceffWipe3_Update(Actor* thisx, PlayState* play);
void OceffWipe3_Draw(Actor* thisx, PlayState* play);

const ActorInit Oceff_Wipe3_InitVars = {
    ACTOR_OCEFF_WIPE3,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(OceffWipe3),
    (ActorFunc)OceffWipe3_Init,
    (ActorFunc)OceffWipe3_Destroy,
    (ActorFunc)OceffWipe3_Update,
    (ActorFunc)OceffWipe3_Draw,
    NULL,
};

#include "overlays/ovl_Oceff_Wipe3/ovl_Oceff_Wipe3.h"

void OceffWipe3_Init(Actor* thisx, PlayState* play) {
    OceffWipe3* this = (OceffWipe3*)thisx;

    Actor_SetScale(&this->actor, 0.1f);
    this->counter = 0;
    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    // it's actually WIPE3...
    osSyncPrintf(VT_FGCOL(CYAN) " WIPE2 arg_data = %d\n" VT_RST, this->actor.params);
}

void OceffWipe3_Destroy(Actor* thisx, PlayState* play) {
    OceffWipe3* this = (OceffWipe3*)thisx;
    Player* player = GET_PLAYER(play);

    func_800876C8(play);
    if (gSaveContext.nayrusLoveTimer != 0) {
        player->stateFlags3 |= 0x40;
    }
}

void OceffWipe3_Update(Actor* thisx, PlayState* play) {
    OceffWipe3* this = (OceffWipe3*)thisx;

    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    if (this->counter < 100) {
        this->counter++;
    } else {
        Actor_Kill(&this->actor);
    }
}

void OceffWipe3_Draw(Actor* thisx, PlayState* play) {
    u32 scroll = play->state.frames & 0xFFF;
    OceffWipe3* this = (OceffWipe3*)thisx;
    f32 z;
    u8 alpha;
    s32 pad[2];
    Vec3f eye;
    Vtx* vtxPtr;
    Vec3f vec;

    eye = GET_ACTIVE_CAM(play)->eye;
    Camera_GetSkyboxOffset(&vec, GET_ACTIVE_CAM(play));
    if (this->counter < 32) {
        z = Math_SinS(this->counter << 9) * 1330;
    } else {
        z = 1330;
    }

    vtxPtr = ResourceMgr_LoadVtxByName(sFrustumVtx);
    if (this->counter >= 80) {
        alpha = 12 * (100 - this->counter);
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

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 170, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 100, 200, 0, 128);
    gSPDisplayList(POLY_XLU_DISP++, sMaterialDL);
    gSPDisplayList(POLY_XLU_DISP++, Gfx_TwoTexScroll(play->state.gfxCtx, 0, scroll * 12, scroll * (-12), 64, 64, 1,
                                                     scroll * 8, scroll * (-8), 64, 64));
    gSPDisplayList(POLY_XLU_DISP++, sFrustumDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
