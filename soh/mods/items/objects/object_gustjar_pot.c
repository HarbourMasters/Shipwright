/**
 * object_gustjar_pot.c - Gust Jar 3D model and draw functions
 *
 * Draws the jar in Link's hands when using the Gust Jar.
 * Model: Custom jar DLs (jar_body_dl, jar_decoration_dl)
 */
#include "objects/object_vase/object_vase.h"

extern Gfx jar_body_dl[];
extern Gfx jar_decoration_dl[];

static void GustJarPot_Draw(Player* player, PlayState* play) {
    if (!gCustomItemState.gustJarEquipped) return;

    OPEN_DISPS(play->state.gfxCtx);

    // 1. Calculate position
    Vec3f potPos = player->actor.world.pos;
    potPos.y += 20.0f;
    f32 frontOffset = 15.0f;
    s16 yaw = player->actor.shape.rot.y;
    potPos.x += Math_SinS(yaw) * frontOffset;
    potPos.z += Math_CosS(yaw) * frontOffset;

    // 2. Setup render state (OPA for solid geometry)
    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    // 3. Transformation matrix
    Matrix_Translate(potPos.x, potPos.y, potPos.z, MTXMODE_NEW);
    Matrix_RotateY(BINANG_TO_RAD(yaw), MTXMODE_APPLY);
    Matrix_RotateX(DEG_TO_RAD(90.0f), MTXMODE_APPLY);
    Matrix_Scale(1.5f, 1.5f, 1.5f, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    // 4. Draw body (gray/base)
    gSPDisplayList(POLY_OPA_DISP++, jar_body_dl);

    // 5. Draw decoration (recolorable) - set PrimColor before DL
    if (gCustomItemState.gustJarProjectileActive || gCustomItemState.gustJarMode == 3) {
        // ACTIVE STATE: Bright red
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 50, 50, 255);
    } else {
        // IDLE/SUCTION STATE: Original blue
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 86, 169, 255, 255);
    }

    gSPDisplayList(POLY_OPA_DISP++, jar_decoration_dl);

    CLOSE_DISPS(play->state.gfxCtx);
}