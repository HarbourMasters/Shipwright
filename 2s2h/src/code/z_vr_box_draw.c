#include "global.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

Mtx* sSkyboxDrawMatrix;

Mtx* Skybox_UpdateMatrix(SkyboxContext* skyboxCtx, f32 x, f32 y, f32 z) {
    Matrix_Translate(x, y, z, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_RotateXFApply(skyboxCtx->rot.x);
    Matrix_RotateYF(skyboxCtx->rot.y, MTXMODE_APPLY);
    Matrix_RotateZF(skyboxCtx->rot.z, MTXMODE_APPLY);
    return Matrix_ToMtx(sSkyboxDrawMatrix);
}

void Skybox_SetColors(SkyboxContext* skyboxCtx, u8 primR, u8 primG, u8 primB, u8 envR, u8 envG, u8 envB) {
    skyboxCtx->prim.r = primR;
    skyboxCtx->prim.g = primG;
    skyboxCtx->prim.b = primB;
    skyboxCtx->env.r = envR;
    skyboxCtx->env.g = envG;
    skyboxCtx->env.b = envB;
}

void Skybox_Draw(SkyboxContext* skyboxCtx, GraphicsContext* gfxCtx, s16 skyboxId, s16 blend, f32 x, f32 y, f32 z) {
    OPEN_DISPS(gfxCtx);
    FrameInterpolation_RecordOpenChild(NULL, FrameInterpolation_GetCameraEpoch());

    Gfx_SetupDL40_Opa(gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x0B, skyboxCtx->palette);
    gSPTexture(POLY_OPA_DISP++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);

    // Prepare matrix
    sSkyboxDrawMatrix = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    Matrix_Translate(x, y, z, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_RotateXFApply(skyboxCtx->rot.x);
    Matrix_RotateYF(skyboxCtx->rot.y, MTXMODE_APPLY);
    Matrix_RotateZF(skyboxCtx->rot.z, MTXMODE_APPLY);
    Matrix_ToMtx(sSkyboxDrawMatrix);
    gSPMatrix(POLY_OPA_DISP++, sSkyboxDrawMatrix, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    // Enable magic square RGB dithering and bilinear filtering
    gDPSetColorDither(POLY_OPA_DISP++, G_CD_MAGICSQ);
    gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);

    // All skyboxes use CI8 textures with an RGBA16 palette
    gDPLoadTLUT_pal256(POLY_OPA_DISP++, skyboxCtx->palette);
    gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_RGBA16);

    // Enable texture filtering RDP pipeline stages for bilinear filtering
    gDPSetTextureConvert(POLY_OPA_DISP++, G_TC_FILT);

    // Set skybox color
    gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL1, TEXEL0, PRIMITIVE_ALPHA, TEXEL0, TEXEL1, TEXEL0, PRIMITIVE, TEXEL0,
                      PRIMITIVE, ENVIRONMENT, COMBINED, ENVIRONMENT, 0, 0, 0, COMBINED);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, skyboxCtx->prim.r, skyboxCtx->prim.g, skyboxCtx->prim.b, blend);
    gDPSetEnvColor(POLY_OPA_DISP++, skyboxCtx->env.r, skyboxCtx->env.g, skyboxCtx->env.b, 0);

    // Draw each face
    gSPDisplayList(POLY_OPA_DISP++, &skyboxCtx->dListBuf[0]); // -z face
    gSPDisplayList(POLY_OPA_DISP++, &skyboxCtx->dListBuf[2]); // +z face
    gSPDisplayList(POLY_OPA_DISP++, &skyboxCtx->dListBuf[4]); // -x face
    gSPDisplayList(POLY_OPA_DISP++, &skyboxCtx->dListBuf[6]); // +x face
    gSPDisplayList(POLY_OPA_DISP++, &skyboxCtx->dListBuf[8]); // +y face

    if (skyboxId == SKYBOX_CUTSCENE_MAP) {
        // Skip the bottom face unless in the cutscene map
        gSPDisplayList(POLY_OPA_DISP++, &skyboxCtx->dListBuf[10]); // -y face
    }

    gDPPipeSync(POLY_OPA_DISP++);

    FrameInterpolation_RecordCloseChild();
    CLOSE_DISPS(gfxCtx);
}

void Skybox_Update(SkyboxContext* skyboxCtx) {
}
