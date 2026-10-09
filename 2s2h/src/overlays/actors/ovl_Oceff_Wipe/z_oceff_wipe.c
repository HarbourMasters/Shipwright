/*
 * File: z_oceff_wipe.c
 * Overlay: ovl_Oceff_Wipe
 * Description: Song of Time Ocarina Effect
 */

#include "z_oceff_wipe.h"
#include "BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void OceffWipe_Init(Actor* thisx, PlayState* play);
void OceffWipe_Destroy(Actor* thisx, PlayState* play);
void OceffWipe_Update(Actor* thisx, PlayState* play);
void OceffWipe_Draw(Actor* thisx, PlayState* play);

ActorProfile Oceff_Wipe_Profile = {
    ACTOR_OCEFF_WIPE,
    ACTORCAT_ITEMACTION,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(OceffWipe),
    (ActorFunc)OceffWipe_Init,
    (ActorFunc)OceffWipe_Destroy,
    (ActorFunc)OceffWipe_Update,
    (ActorFunc)OceffWipe_Draw,
};

static s32 sBssPad;

void OceffWipe_Init(Actor* thisx, PlayState* play) {
    OceffWipe* this = (OceffWipe*)thisx;

    Actor_SetScale(&this->actor, 0.1f);
    this->counter = 0;
    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
}

void OceffWipe_Destroy(Actor* thisx, PlayState* play) {
    OceffWipe* this = (OceffWipe*)thisx;

    Magic_Reset(play);
    play->msgCtx.ocarinaSongEffectActive = false;
}

void OceffWipe_Update(Actor* thisx, PlayState* play) {
    OceffWipe* this = (OceffWipe*)thisx;

    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    if (this->counter < 100) {
        this->counter++;
    } else {
        Actor_Kill(&this->actor);
    }
}

#include "assets/overlays/ovl_Oceff_Wipe/ovl_Oceff_Wipe.h"

static u8 sAlphaIndices[] = {
    0x01, 0x10, 0x22, 0x01, 0x20, 0x12, 0x01, 0x20, 0x12, 0x01,
    0x10, 0x22, 0x01, 0x20, 0x12, 0x01, 0x12, 0x21, 0x01, 0x02,
};

void OceffWipe_Draw(Actor* thisx, PlayState* play) {
    u32 scroll = play->state.frames & 0xFF;
    OceffWipe* this = (OceffWipe*)thisx;
    f32 z;
    s32 pad;
    u8 alphaTable[3];
    s32 i;
    Vec3f eye = GET_ACTIVE_CAM(play)->eye;
    Vtx* vtxPtr;
    Vec3f quakeOffset;

    quakeOffset = Camera_GetQuakeOffset(GET_ACTIVE_CAM(play));

    OPEN_DISPS(play->state.gfxCtx);

    // #region 2S2H [Widescreen] Ocarina Effects
    f32 effectDistance;
    s32 x = OTRGetRectDimensionFromLeftEdge(0) << 2;
    if (x < 0) {
        // Only render if the screen is wider then original
        effectDistance = 1360.0f / (OTRGetAspectRatio() * 0.85f); // Widescreen value
    } else {
        effectDistance = 1360.0f; // Vanilla value
    }
    // #endregion

    if (this->counter < 32) {
        z = Math_SinS(this->counter << 9) * effectDistance;
    } else {
        z = effectDistance;
    }

    if (this->counter >= 80) {
        alphaTable[0] = 0;
        alphaTable[1] = (100 - this->counter) * 8;
        alphaTable[2] = (100 - this->counter) * 12;
    } else {
        alphaTable[0] = 0;
        alphaTable[1] = 160;
        alphaTable[2] = 255;
    }

    // 2S2H [Port] Originally this was just a pointer to the vertices, now it's the OTR path so we need to grab it's
    // actual address and move out of loop as we don't need to load the resource each iteration
    vtxPtr = ResourceMgr_LoadVtxByName(sSongOfTimeFrustumVtx);

    for (i = 0; i < 20; i++) {
        vtxPtr[i * 2 + 0].v.cn[3] = alphaTable[(sAlphaIndices[i] & 0xF0) >> 4];
        vtxPtr[i * 2 + 1].v.cn[3] = alphaTable[sAlphaIndices[i] & 0xF];
    }

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    Matrix_Translate(eye.x + quakeOffset.x, eye.y + quakeOffset.y, eye.z + quakeOffset.z, MTXMODE_NEW);
    Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_RotateXS(0x708, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, -z, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    if (this->actor.params != OCEFF_WIPE_ZL) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 170, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 150, 255, 128);
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 200, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 100, 0, 255, 128);
    }

    gSPDisplayList(POLY_XLU_DISP++, sSongOfTimeFrustumMaterialDL);
    gSPDisplayList(POLY_XLU_DISP++, Gfx_TwoTexScrollEx(play->state.gfxCtx, G_TX_RENDERTILE, 0 - scroll, scroll * -2, 32,
                                                       32, 1, 0 - scroll, scroll * -2, 32, 32, -1, -2, -1, -2));
    gSPDisplayList(POLY_XLU_DISP++, sSongOfTimeFrustumModelDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
