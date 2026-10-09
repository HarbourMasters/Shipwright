/*
 * File: z_oceff_wipe2.c
 * Overlay: ovl_Oceff_Wipe2
 * Description: Epona's Song Ocarina Effect
 */

#include "z_oceff_wipe2.h"
#include "BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void OceffWipe2_Init(Actor* thisx, PlayState* play);
void OceffWipe2_Destroy(Actor* thisx, PlayState* play);
void OceffWipe2_Update(Actor* thisx, PlayState* play);
void OceffWipe2_Draw(Actor* thisx, PlayState* play);

ActorProfile Oceff_Wipe2_Profile = {
    ACTOR_OCEFF_WIPE2,
    ACTORCAT_ITEMACTION,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(OceffWipe2),
    (ActorFunc)OceffWipe2_Init,
    (ActorFunc)OceffWipe2_Destroy,
    (ActorFunc)OceffWipe2_Update,
    (ActorFunc)OceffWipe2_Draw,
};

#include "assets/overlays/ovl_Oceff_Wipe2/ovl_Oceff_Wipe2.h"

static s32 sBssPad;

void OceffWipe2_Init(Actor* thisx, PlayState* play) {
    OceffWipe2* this = (OceffWipe2*)thisx;

    Actor_SetScale(&this->actor, 0.1f);
    this->timer = 0;
    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
}

void OceffWipe2_Destroy(Actor* thisx, PlayState* play) {
    OceffWipe2* this = (OceffWipe2*)thisx;

    Magic_Reset(play);
    play->msgCtx.ocarinaSongEffectActive = false;
}

void OceffWipe2_Update(Actor* thisx, PlayState* play) {
    OceffWipe2* this = (OceffWipe2*)thisx;

    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    if (this->timer < 100) {
        this->timer++;
    } else {
        Actor_Kill(&this->actor);
    }
}

void OceffWipe2_Draw(Actor* thisx, PlayState* play) {
    u32 scroll = play->state.frames & 0xFF;
    OceffWipe2* this = (OceffWipe2*)thisx;
    f32 z;
    u8 alpha;
    s32 pad[2];
    Vec3f eye = GET_ACTIVE_CAM(play)->eye;
    Vtx* vtxPtr;
    Vec3f quakeOffset;

    quakeOffset = Camera_GetQuakeOffset(GET_ACTIVE_CAM(play));

    vtxPtr = ResourceMgr_LoadVtxByName(sEponaSongFrustumVtx);

    // #region 2S2H [Widescreen] Ocarina Effects
    f32 effectDistance = 1220.0f; // Vanilla value
    s32 x = OTRGetRectDimensionFromLeftEdge(0) << 2;
    if (x < 0) {
        // Only render if the screen is wider then original
        effectDistance = 1220.0f / (OTRGetAspectRatio() * 0.85f); // Widescreen value
    }
    // #endregion

    if (this->timer < 32) {
        z = Math_SinS(this->timer * 0x200) * effectDistance;
    } else {
        z = effectDistance;
    }

    if (this->timer >= 80) {
        alpha = 12 * (100 - this->timer);
    } else {
        alpha = 255;
    }

    vtxPtr[1].v.cn[3] = vtxPtr[3].v.cn[3] = vtxPtr[5].v.cn[3] = vtxPtr[7].v.cn[3] = vtxPtr[9].v.cn[3] =
        vtxPtr[11].v.cn[3] = vtxPtr[13].v.cn[3] = vtxPtr[15].v.cn[3] = vtxPtr[16].v.cn[3] = vtxPtr[18].v.cn[3] =
            vtxPtr[20].v.cn[3] = alpha;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    Matrix_Translate(eye.x + quakeOffset.x, eye.y + quakeOffset.y, eye.z + quakeOffset.z, MTXMODE_NEW);
    Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_RotateXS(0x708, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, -z, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 170, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 100, 0, 128);
    gSPDisplayList(POLY_XLU_DISP++, sEponaSongFrustumMaterialDL);
    gSPDisplayList(POLY_XLU_DISP++, Gfx_TwoTexScrollEx(play->state.gfxCtx, G_TX_RENDERTILE, scroll * 6, scroll * -6, 64,
                                                       64, 1, scroll * -6, 0, 64, 64, 6, -6, -6, 0));
    gSPDisplayList(POLY_XLU_DISP++, sEponaSongFrustumModelDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
