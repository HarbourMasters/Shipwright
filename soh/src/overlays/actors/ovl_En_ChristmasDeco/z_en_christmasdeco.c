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
    if (play->sceneNum == SCENE_TEMPLE_OF_TIME) {
        EnChristmasDeco* this = (EnChristmasDeco*)thisx;
        this->actor.room = -1;
    }
}

void EnChristmasDeco_Destroy(Actor* thisx, PlayState* play) {
    
}

void EnChristmasDeco_Update(Actor* thisx, PlayState* play) {
    
}

void EnChristmasDeco_Draw(Actor* thisx, PlayState* play) {
    float decoSize = 10.0f;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    Matrix_Scale(decoSize, decoSize, decoSize, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);

    // Assertion Halt in Debug mode, switch to Release when testing.
    if (play->sceneNum == SCENE_KAKARIKO_VILLAGE) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gKakarikoDecorDL);
        if (LINK_IS_CHILD) {
            gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gKakarikoChildDecorDL);
        } else {
            gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gKakarikoAdultDecorDL);
        }
    }

    if (play->sceneNum == SCENE_TEMPLE_OF_TIME) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gTempleOfTimeDecorDL);
        if (LINK_IS_CHILD) {
            gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gTempleOfTimeDecorDL);
        } else {
            gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gTempleOfTimeDecorDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
