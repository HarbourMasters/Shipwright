/*
 * File: z_oceff_wipe4.c
 * Overlay: ovl_Oceff_Wipe4
 * Description: Scarecrow's Song Ocarina Effect
 */

#include "z_oceff_wipe4.h"
#include "BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void OceffWipe4_Init(Actor* thisx, PlayState* play);
void OceffWipe4_Destroy(Actor* thisx, PlayState* play);
void OceffWipe4_Update(Actor* thisx, PlayState* play);
void OceffWipe4_Draw(Actor* thisx, PlayState* play);

ActorProfile Oceff_Wipe4_Profile = {
    ACTOR_OCEFF_WIPE4,
    ACTORCAT_ITEMACTION,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(OceffWipe4),
    (ActorFunc)OceffWipe4_Init,
    (ActorFunc)OceffWipe4_Destroy,
    (ActorFunc)OceffWipe4_Update,
    (ActorFunc)OceffWipe4_Draw,
};

#include "assets/overlays/ovl_Oceff_Wipe4/ovl_Oceff_Wipe4.h"

static s32 sBssPad;

void OceffWipe4_Init(Actor* thisx, PlayState* play) {
    OceffWipe4* this = (OceffWipe4*)thisx;

    Actor_SetScale(&this->actor, 0.1f);
    this->counter = 0;
    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
}

void OceffWipe4_Destroy(Actor* thisx, PlayState* play) {
    OceffWipe4* this = (OceffWipe4*)thisx;

    Magic_Reset(play);
    play->msgCtx.ocarinaSongEffectActive = false;
}

void OceffWipe4_Update(Actor* thisx, PlayState* play) {
    OceffWipe4* this = (OceffWipe4*)thisx;

    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    if (this->counter < 50) {
        this->counter++;
    } else {
        Actor_Kill(&this->actor);
    }
}

void OceffWipe4_Draw(Actor* thisx, PlayState* play) {
    u32 scroll = play->state.frames & 0xFFF;
    OceffWipe4* this = (OceffWipe4*)thisx;
    f32 z;
    u8 alpha;
    s32 pad[2];
    Vec3f eye = GET_ACTIVE_CAM(play)->eye;
    Vtx* vtxPtr;
    Vec3f quakeOffset;

    quakeOffset = Camera_GetQuakeOffset(GET_ACTIVE_CAM(play));

    // #region 2S2H [Widescreen] Ocarina Effects
    f32 effectDistance = 1220.0f; // Vanilla value
    s32 x = OTRGetRectDimensionFromLeftEdge(0) << 2;
    if (x < 0) {
        // Only render if the screen is wider then original
        effectDistance = 1220.0f / (OTRGetAspectRatio() * 0.85f); // Widescreen value
    }
    // #endregion

    if (this->counter < 16) {
        z = Math_SinS(this->counter * 0x400) * effectDistance;
    } else {
        z = effectDistance;
    }

    vtxPtr = ResourceMgr_LoadVtxByName(sScarecrowSongFrustumVtx);

    if (this->counter >= 30) {
        alpha = 12 * (50 - this->counter);
    } else {
        alpha = 255;
    }

    vtxPtr[1].v.cn[3] = vtxPtr[3].v.cn[3] = vtxPtr[5].v.cn[3] = vtxPtr[7].v.cn[3] = vtxPtr[9].v.cn[3] =
        vtxPtr[11].v.cn[3] = vtxPtr[13].v.cn[3] = vtxPtr[15].v.cn[3] = vtxPtr[17].v.cn[3] = vtxPtr[19].v.cn[3] =
            vtxPtr[21].v.cn[3] = alpha;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    Matrix_Translate(eye.x + quakeOffset.x, eye.y + quakeOffset.y, eye.z + quakeOffset.z, MTXMODE_NEW);
    Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_RotateXS(0x708, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, -z, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    if (this->actor.params == OCEFF_WIPE4_UNUSED) {
        gSPDisplayList(POLY_XLU_DISP++, sScarecrowSongUnusedMaterialDL);
    } else {
        gSPDisplayList(POLY_XLU_DISP++, sScarecrowSongMaterialDL);
    }

    // #region 2S2H [Port]
    // We need to first load the DL before we can index it on the port
    Gfx* scarecrowSongModelDL = ResourceMgr_LoadTexOrDListByName(sScarecrowSongModelDL);

    gSPDisplayList(POLY_XLU_DISP++, scarecrowSongModelDL);
    gSPDisplayList(POLY_XLU_DISP++, Gfx_TwoTexScrollEx(play->state.gfxCtx, G_TX_RENDERTILE, scroll * 2, scroll * -2, 32,
                                                       64, 1, scroll * -1, scroll, 32, 32, 2, -2, -1, 1));
    // Index adjust 11 -> 14 (for gsSPVertex) to account for our extraction size changes
    gSPDisplayList(POLY_XLU_DISP++, &scarecrowSongModelDL[14]);
    // #endregion

    CLOSE_DISPS(play->state.gfxCtx);
}
