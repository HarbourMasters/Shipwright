/**
 * object_mogma_mitts.c - Mogma Mitts 3D model and draw functions
 *
 * Draws white gauntlets on Link's right hand when mitts are active.
 * Model: Custom DL in mogma_mittsDL/
 */

#include "z64.h"
#include "../custom_items.h"
#include "macros.h"
#include "functions.h"
#include "mogma_mittsDL/header.h"

void CustomItems_DrawMogmaMitts(Player* player, PlayState* play) {
    if (!gCustomItemState.mogmaMittsActive) return;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    // Position at right hand
    Vec3f handPos = player->bodyPartsPos[PLAYER_BODYPART_R_HAND];
    f32 forwardOffset = 8.0f;

    Matrix_Translate(
        handPos.x + Math_SinS(player->actor.shape.rot.y) * forwardOffset,
        handPos.y + 3.0f,
        handPos.z + Math_CosS(player->actor.shape.rot.y) * forwardOffset,
        MTXMODE_NEW
    );
    Matrix_RotateY(BINANG_TO_RAD(player->actor.shape.rot.y), MTXMODE_APPLY);
    Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++,
              Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_OPA_DISP++, gMogmaMittsGiveDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
