/*
 * File: z_oceff_wipe6.c
 * Overlay: ovl_Oceff_Wipe6
 * Description: Song of Soaring Ocarina Effect
 */

#include "z_oceff_wipe6.h"
#include "BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void OceffWipe6_Init(Actor* thisx, PlayState* play);
void OceffWipe6_Destroy(Actor* thisx, PlayState* play);
void OceffWipe6_Update(Actor* thisx, PlayState* play);
void OceffWipe6_Draw(Actor* thisx, PlayState* play);

ActorProfile Oceff_Wipe6_Profile = {
    ACTOR_OCEFF_WIPE6,
    ACTORCAT_ITEMACTION,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(OceffWipe6),
    (ActorFunc)OceffWipe6_Init,
    (ActorFunc)OceffWipe6_Destroy,
    (ActorFunc)OceffWipe6_Update,
    (ActorFunc)OceffWipe6_Draw,
};

#include "overlays/ovl_Oceff_Wipe6/ovl_Oceff_Wipe6.h"

static Vtx* gOceff6VtxData;

void OceffWipe6_Init(Actor* thisx, PlayState* play) {
    OceffWipe6* this = (OceffWipe6*)thisx;

    gOceff6VtxData = ResourceMgr_LoadVtxByName(gOceff6Vtx);

    Actor_SetScale(&this->actor, 1.0f);
    this->counter = 0;
    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
}

void OceffWipe6_Destroy(Actor* thisx, PlayState* play) {
    Magic_Reset(play);
    play->msgCtx.ocarinaSongEffectActive = false;
}

void OceffWipe6_Update(Actor* thisx, PlayState* play) {
    OceffWipe6* this = (OceffWipe6*)thisx;

    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    if (this->counter < 100) {
        this->counter++;
    } else {
        Actor_Kill(&this->actor);
    }
}

void OceffWipe6_Draw(Actor* thisx, PlayState* play) {
    OceffWipe6* this = (OceffWipe6*)thisx;
    f32 z;
    u8 alpha;
    s32 i;
    s32 counter;
    Vec3f activeCamEye;
    s32 pad;
    Vec3f quakeOffset;
    s32 pad2;

    activeCamEye = GET_ACTIVE_CAM(play)->eye;
    quakeOffset = Camera_GetQuakeOffset(GET_ACTIVE_CAM(play));

    // #region 2S2H [Widescreen] Ocarina Effects
    f32 effectDistance = 1220.0f; // Vanilla value
    s32 x = OTRGetRectDimensionFromLeftEdge(0) << 2;
    if (x < 0) {
        // Only render if the screen is wider then original
        effectDistance = 1220.0f / (OTRGetAspectRatio() * 0.85f); // Widescreen value
    }
    // #endregion

    if (this->counter < 32) {
        counter = this->counter;
        z = Math_SinS(counter * 0x200) * effectDistance;
    } else {
        z = effectDistance;
    }

    if (this->counter >= 80) {
        alpha = 12 * (100 - this->counter);
    } else {
        alpha = 255;
    }

    for (i = 1; i < ResourceMgr_GetVtxArraySizeByName(gOceff6Vtx); i += 2) {
        gOceff6VtxData[i].v.cn[3] = alpha;
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Matrix_Translate(activeCamEye.x + quakeOffset.x, activeCamEye.y + quakeOffset.y, activeCamEye.z + quakeOffset.z,
                     MTXMODE_NEW);
    Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_RotateXS(0x708, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, -z, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    AnimatedMat_Draw(play, ovl_Oceff_Wipe6_Matanimheader_000338);
    gSPDisplayList(POLY_XLU_DISP++, gOceff6DL);

    CLOSE_DISPS(play->state.gfxCtx);
}
