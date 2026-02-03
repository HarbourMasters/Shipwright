/**
 * object_beetle.c - Beetle 3D model and draw functions
 */

#include "z64.h"
#include "../custom_items.h"
#include "../logic/item_beetle.h"
#include "beetle_giveDL/header.h"
#include "macros.h"
#include "functions.h"

static void Beetle_SetupGeometryMode(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);
    Gfx_SetupDL_25Opa(gfxCtx);
    CLOSE_DISPS(gfxCtx);
}

static void Beetle_DrawBody(PlayState* play, Vec3f* pos, Vec3s* rot, f32 scale) {
    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_NEW);
    Matrix_RotateY(rot->y * BEETLE_ANGLE_TO_RAD, MTXMODE_APPLY);
    Matrix_RotateX(rot->x * BEETLE_ANGLE_TO_RAD, MTXMODE_APPLY);
    Matrix_RotateY(M_PI / 2.0f, MTXMODE_APPLY);
    Matrix_Scale(scale * 3.0f, scale * 3.0f, scale * 3.0f, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, g_beetle_body_dl);

    CLOSE_DISPS(play->state.gfxCtx);
}

static void Beetle_DrawWings(PlayState* play, f32 wingScale) {
    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Push();
    Matrix_Scale(1.0f, wingScale, 1.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, g_beetle_wings_dl);
    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

void Beetle_UpdateWingAnimation(f32* scale, s8* direction) {
    *scale += (*direction) * BEETLE_WING_ANIM_SPEED;

    if (*scale >= BEETLE_WING_SCALE_MAX) {
        *scale = BEETLE_WING_SCALE_MAX;
        *direction = -1;
    } else if (*scale <= BEETLE_WING_SCALE_MIN) {
        *scale = BEETLE_WING_SCALE_MIN;
        *direction = 1;
    }
}

void CustomItems_DrawBeetle(Player* player, PlayState* play) {
    if (!beetleActive) return;

    Beetle_SetupGeometryMode(play->state.gfxCtx);

    if (beetleState == BEETLE_STATE_AIMING) {
        Vec3f handPos = player->bodyPartsPos[PLAYER_BODYPART_L_HAND];
        Vec3s handRot;
        handRot.x = 0;
        handRot.y = player->actor.shape.rot.y;
        handRot.z = 0;

        handPos.x += Math_SinS(handRot.y) * 8.0f;
        handPos.z += Math_CosS(handRot.y) * 8.0f;

        Beetle_DrawBody(play, &handPos, &handRot, BEETLE_MODEL_SCALE * 0.8f);
        Beetle_DrawWings(play, beetleWingScale);
    } else if (beetleState == BEETLE_STATE_FLYING || beetleState == BEETLE_STATE_RETURNING) {
        Beetle_DrawBody(play, &beetlePos, &beetleRot, BEETLE_MODEL_SCALE);
        Beetle_DrawWings(play, beetleWingScale);
    }
}
