/**
 * object_ballchain.c - Ball and Chain 3D model and draw functions
 *
 * Draws the ball and chain item when equipped and during use.
 * Model: Custom DL in ball_and_chainDL/
 */

#include "z64.h"
#include "../custom_items.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_link_boy/object_link_boy.h"
#include "ball_and_chainDL/header.h"

#define BALLCHAIN_STATE_EQUIP 1

static void BallChain_DrawBall(PlayState* play, Vec3f* pos, f32 scale, u8 shouldRotate) {
    OPEN_DISPS(play->state.gfxCtx);

    gSPClearGeometryMode(POLY_OPA_DISP++, G_CULL_BACK | G_LIGHTING);
    gSPSetGeometryMode(POLY_OPA_DISP++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK | G_LIGHTING | G_ZBUFFER);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_SHADE, G_CC_SHADE);

    Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_NEW);

    if (shouldRotate) {
        s16 spinRot = play->gameplayFrames * 0x400;
        Matrix_RotateY(spinRot * 0.05f, MTXMODE_APPLY);
        Matrix_RotateX(spinRot * 0.03f, MTXMODE_APPLY);
    }

    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gBallDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

static void BallChain_DrawChain(PlayState* play, Vec3f* start, Vec3f* end) {
    f32 dx = end->x - start->x;
    f32 dy = end->y - start->y;
    f32 dz = end->z - start->z;
    f32 dist = sqrtf(dx * dx + dy * dy + dz * dz);
    s32 linkCount = (s32)(dist / 15.0f);
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 85, 85, 85, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 40, 40, 40, 255);

    if (linkCount < 1) linkCount = 1;
    if (linkCount > 40) linkCount = 40;

    for (i = 0; i <= linkCount; i++) {
        f32 t = (f32)i / (f32)linkCount;
        Vec3f linkPos;
        f32 yaw, pitch;

        linkPos.x = start->x + dx * t;
        linkPos.y = start->y + dy * t;
        linkPos.z = start->z + dz * t;

        Matrix_Translate(linkPos.x, linkPos.y, linkPos.z, MTXMODE_NEW);

        yaw = Math_FAtan2F(dx, dz);
        pitch = Math_FAtan2F(-dy, sqrtf(dx * dx + dz * dz));

        Matrix_RotateY(yaw, MTXMODE_APPLY);
        Matrix_RotateX(pitch, MTXMODE_APPLY);

        if (i % 2 != 0) {
            Matrix_RotateZ(M_PI / 2, MTXMODE_APPLY);
        }

        Matrix_Scale(0.02f, 0.02f, 0.02f, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gLinkAdultHookshotChainDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void CustomItems_DrawBallChain(Player* this, PlayState* play) {
    s32 state;
    Vec3f* ballPos;
    Vec3f* leftHand;
    Vec3f* rightHand;
    Vec3f midHand;
    f32 scale;
    u8 shouldRotate;

    if (!gCustomItemState.ballAndChainThrown) return;

    state = gCustomItemState.timer2;
    ballPos = &gCustomItemState.sharedProjectilePos;

    // Use midpoint between hands
    leftHand = &this->bodyPartsPos[PLAYER_BODYPART_L_HAND];
    rightHand = &this->bodyPartsPos[PLAYER_BODYPART_R_HAND];
    midHand.x = (leftHand->x + rightHand->x) * 0.5f;
    midHand.y = (leftHand->y + rightHand->y) * 0.5f;
    midHand.z = (leftHand->z + rightHand->z) * 0.5f;

    if (state != BALLCHAIN_STATE_EQUIP) {
        BallChain_DrawChain(play, &midHand, ballPos);
    }

    scale = (state == BALLCHAIN_STATE_EQUIP) ? 0.06f : 0.1f;
    shouldRotate = (state != BALLCHAIN_STATE_EQUIP);
    BallChain_DrawBall(play, ballPos, scale, shouldRotate);
}