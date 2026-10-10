/*
 * File: z_oceff_wipe7.c
 * Overlay: ovl_Oceff_Wipe7
 * Description: Song of Healing Ocarina Effect
 */

#include "z_oceff_wipe7.h"
#include "BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void OceffWipe7_Init(Actor* thisx, PlayState* play);
void OceffWipe7_Destroy(Actor* thisx, PlayState* play);
void OceffWipe7_Update(Actor* thisx, PlayState* play);
void OceffWipe7_Draw(Actor* thisx, PlayState* play);

ActorProfile Oceff_Wipe7_Profile = {
    ACTOR_OCEFF_WIPE7,
    ACTORCAT_ITEMACTION,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(OceffWipe7),
    (ActorFunc)OceffWipe7_Init,
    (ActorFunc)OceffWipe7_Destroy,
    (ActorFunc)OceffWipe7_Update,
    (ActorFunc)OceffWipe7_Draw,
};

#include "assets/overlays/ovl_Oceff_Wipe7/ovl_Oceff_Wipe7.h"

static s32 sBssPad;

static Vtx* sSongofHealingEffectFrustrumVtxData;

void OceffWipe7_Init(Actor* thisx, PlayState* play) {
    OceffWipe7* this = (OceffWipe7*)thisx;
    sSongofHealingEffectFrustrumVtxData = ResourceMgr_LoadVtxByName(sSongofHealingEffectFrustrumVtx);

    Actor_SetScale(&this->actor, 1.0f);
    this->counter = 0;
    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
}

void OceffWipe7_Destroy(Actor* thisx, PlayState* play) {
    OceffWipe7* this = (OceffWipe7*)thisx;

    Magic_Reset(play);
    play->msgCtx.ocarinaSongEffectActive = false;
}

void OceffWipe7_Update(Actor* thisx, PlayState* play) {
    OceffWipe7* this = (OceffWipe7*)thisx;

    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    if (this->counter < 100) {
        this->counter++;
    } else {
        Actor_Kill(&this->actor);
    }
}

void OceffWipe7_Draw(Actor* thisx, PlayState* play) {
    OceffWipe7* this = (OceffWipe7*)thisx;
    f32 z;
    u8 alpha;
    s32 i;
    s32 counter;
    Vec3f activeCamEye = GET_ACTIVE_CAM(play)->eye;
    s32 pad;
    Vec3f quakeOffset;
    s32 pad2;

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
        z = Math_SinS(this->counter * 0x200) * effectDistance;
    } else {
        z = effectDistance;
    }

    if (this->counter >= 80) {
        alpha = 12 * (100 - this->counter);
    } else {
        alpha = 255;
    }

    for (i = 1; i < ResourceMgr_GetVtxArraySizeByName(sSongofHealingEffectFrustrumVtx); i += 2) {
        sSongofHealingEffectFrustrumVtxData[i].v.cn[3] = alpha;
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
    AnimatedMat_Draw(play, sSongofHealingEffectTexAnim);
    gSPDisplayList(POLY_XLU_DISP++, sSongOfHealingEffectFrustumDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
