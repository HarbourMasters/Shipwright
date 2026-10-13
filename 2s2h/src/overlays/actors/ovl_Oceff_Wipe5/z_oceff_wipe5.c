/*
 * File: z_oceff_wipe5.c
 * Overlay: ovl_Oceff_Wipe5
 * Description: Sonata/Lullaby/Bossa Nova/Elegy/Oath Ocarina Effect
 */

#include "z_oceff_wipe5.h"
#include "assets/overlays/ovl_Oceff_Wipe5/ovl_Oceff_Wipe5.h"
#include "BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void OceffWipe5_Init(Actor* thisx, PlayState* play);
void OceffWipe5_Destroy(Actor* thisx, PlayState* play);
void OceffWipe5_Update(Actor* thisx, PlayState* play);
void OceffWipe5_Draw(Actor* thisx, PlayState* play);

ActorProfile Oceff_Wipe5_Profile = {
    ACTOR_OCEFF_WIPE5,
    ACTORCAT_ITEMACTION,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(OceffWipe5),
    (ActorFunc)OceffWipe5_Init,
    (ActorFunc)OceffWipe5_Destroy,
    (ActorFunc)OceffWipe5_Update,
    (ActorFunc)OceffWipe5_Draw,
};

static s32 sBssPad;
static Vtx* gOceff5VtxData;

void OceffWipe5_Init(Actor* thisx, PlayState* play) {
    OceffWipe5* this = (OceffWipe5*)thisx;

    gOceff5VtxData = ResourceMgr_LoadVtxByName(gOceff5Vtx);

    Actor_SetScale(&this->actor, 1.0f);
    this->counter = 0;
    this->actor.world.pos = play->cameraPtrs[play->activeCamId]->eye;
}

void OceffWipe5_Destroy(Actor* thisx, PlayState* play) {
    OceffWipe5* this = (OceffWipe5*)thisx;

    Magic_Reset(play);
    play->msgCtx.ocarinaSongEffectActive = false;
}

void OceffWipe5_Update(Actor* thisx, PlayState* play) {
    OceffWipe5* this = (OceffWipe5*)thisx;

    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    if (this->counter < 100) {
        this->counter++;
    } else {
        Actor_Kill(&this->actor);
    }
}

static u8 sPrimColors[] = {
    255, 255, 200, 255, 255, 200, 200, 255, 255, 255, 255, 200, 255, 200, 255,
};

static u8 sEnvColors[] = {
    50, 200, 0, 255, 50, 0, 0, 100, 255, 255, 150, 0, 255, 0, 200,
};

void OceffWipe5_Draw(Actor* thisx, PlayState* play) {
    OceffWipe5* this = (OceffWipe5*)thisx;
    f32 z;
    s32 pad;
    s32 i;
    s32 pad2;
    Vec3f activeCamEye = GET_ACTIVE_CAM(play)->eye;
    Camera* activeCam = GET_ACTIVE_CAM(play);
    Vec3f quakeOffset;
    u8 alpha;
    s32 colorIndex = OCEFF_WIPE5_GET_SONG_TYPE(thisx) * 3;
    f32 phi_fv1 = 1220.0f;

    // #region 2S2H [Widescreen] Ocarina Effects
    s32 x = OTRGetRectDimensionFromLeftEdge(0) << 2;
    if (x < 0) {
        // Only render if the screen is wider then original
        phi_fv1 = 1220.0f / (OTRGetAspectRatio() * 0.85f); // Widescreen value
    }
    // #endregion

    if ((((OCEFF_WIPE5_GET_SONG_TYPE(thisx) == 2) && (play->sceneId == SCENE_LABO)) &&
         ((play->csCtx.scriptIndex == 0) || (play->csCtx.scriptIndex == 1))) &&
        (play->csCtx.state != CS_STATE_IDLE)) {
        // #region 2S2H [Widescreen] Ocarina Effects
        if (x < 0) {
            // Only render if the screen is wider then original
            phi_fv1 = 1150.0f / (OTRGetAspectRatio() * 0.85f); // Widescreen value
        } else {
            phi_fv1 = 1150.0f; // Vanilla value
        }
        // #endregion
    }

    if (colorIndex >= 13) {
        colorIndex = 0;
    }

    quakeOffset = Camera_GetQuakeOffset(activeCam);

    if (this->counter < 32) {
        z = Math_SinS(this->counter << 9) * phi_fv1;
    } else {
        z = phi_fv1;
    }

    if (this->counter >= 80) {
        alpha = 12 * (100 - this->counter);
    } else {
        alpha = 255;
    }

    for (i = 1; i < ResourceMgr_GetVtxArraySizeByName(gOceff5Vtx); i += 2) {
        gOceff5VtxData[i].v.cn[3] = alpha;
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
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, sPrimColors[colorIndex], sPrimColors[colorIndex + 1],
                    sPrimColors[colorIndex + 2], 255);
    gDPSetEnvColor(POLY_XLU_DISP++, sEnvColors[colorIndex], sEnvColors[colorIndex + 1], sEnvColors[colorIndex + 2],
                   255);

    AnimatedMat_Draw(play, gOceff5TexAnim);

    gSPDisplayList(POLY_XLU_DISP++, &gOceff5DL);

    CLOSE_DISPS(play->state.gfxCtx);
}
