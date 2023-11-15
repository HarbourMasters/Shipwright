/*
 * File: z_en_christmasdeco.c
 * Overlay: ovl_En_ChristmasDeco
 * Description: Custom Christmas Decorations
 */

#include "z_en_christmasdeco.h"
#include "soh_assets.h"

void EnChristmasDeco_Init(Actor* thisx, PlayState* play);
void EnChristmasDeco_Destroy(Actor* thisx, PlayState* play);
void EnChristmasDeco_Update(Actor* thisx, PlayState* play);
void EnChristmasDeco_Draw(Actor* thisx, PlayState* play);

void EnChristmasDeco_Init(Actor* thisx, PlayState* play) {
    
}

void EnChristmasDeco_Destroy(Actor* thisx, PlayState* play) {
    
}

void EnChristmasDeco_Update(Actor* thisx, PlayState* play) {
    
}

void EnChristmasDeco_Draw(Actor* thisx, PlayState* play) {
    float decoSize = 1000.0f;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    Matrix_Scale(decoSize, decoSize, decoSize, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);

    if (play->sceneNum == SCENE_KAKARIKO_VILLAGE) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gFrontGateDecorDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
