/**
 * object_cane_of_somaria.c - Cane of Somaria 3D model and draw functions
 */

#include "z64.h"
#include "../custom_items.h"
#include "../logic/item_cane_of_somaria.h"
#include "macros.h"
#include "functions.h"
#include "somaria_cane_DL/header.h"

void CustomItems_DrawCaneOfSomaria(Player* player, PlayState* play) {
    if (!shSomariaActive) return;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    Vec3f handPos = player->bodyPartsPos[PLAYER_BODYPART_R_HAND];

    f32 forwardOffset = 3.0f;
    Matrix_Translate(
        handPos.x + Math_SinS(player->actor.shape.rot.y) * forwardOffset,
        handPos.y + 5,
        handPos.z + Math_CosS(player->actor.shape.rot.y) * forwardOffset,
        MTXMODE_NEW
    );
    Matrix_RotateY(BINANG_TO_RAD(player->actor.shape.rot.y), MTXMODE_APPLY);
    Matrix_RotateY(BINANG_TO_RAD(0x4000), MTXMODE_APPLY);
    Matrix_Scale(0.05f, 0.05f, 0.05f, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, g_somaria_cane_dl);

    CLOSE_DISPS(play->state.gfxCtx);
}
