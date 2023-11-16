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
    float decoSize = 10.0f;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(0, 0, 0, MTXMODE_APPLY);
    Matrix_RotateZYX(0, 0, 0, MTXMODE_APPLY);
    Matrix_Scale(decoSize, decoSize, decoSize, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (play->sceneNum == SCENE_KAKARIKO_VILLAGE) {
        gSPDisplayList(POLY_OPA_DISP++, gFrontGateDecorDL);
        gSPDisplayList(POLY_OPA_DISP++, gKakarikoDecorDL); //Assertion Halt in Debug mode, switch to Release when testing.
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
