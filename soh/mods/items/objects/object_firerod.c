/**
 * object_firerod.c - Fire Rod 3D model and draw functions
 *
 * Draws the red glowing rod and fire projectiles.
 * Uses gEffFire1DL (torch-style flame) for projectiles.
 */

// Fire effect display list from gameplay_keep (torch flame)
#include "objects/gameplay_keep/gameplay_keep.h"

// Fire Rod color for projectile draw effects
// Uses same colors as item_rod_fire.h defines
static RodColor sFireRodDrawColor = {
    255, 255, 0, 255,   // primR, primG, primB, primA (yellow-white fire)
    255, 80, 0, 255     // envR, envG, envB, envA (orange glow)
};

// Flame texture scroll counter (for torch-style animation)
static s16 sFireRodFlameScroll = 0;

// ============================================================================
// FIRE ROD DISPLAY LIST - Simple rod shape using same format as spinner
// ============================================================================

// Vertices using same format as spinner (smaller values, proper Vtx struct)
static Vtx firerod_vtx[] = {
    // Bottom cap (y = 0)
    {{{   0,   0,   0}, 0, {0, 0}, {0, 129, 0, 255}}},
    // Bottom ring (y = 10)
    {{{ -15,  10,   0}, 0, {0, 0}, {129, 0, 0, 255}}},
    {{{ -10,  10, -10}, 0, {0, 0}, {165, 0, 165, 255}}},
    {{{   0,  10, -15}, 0, {0, 0}, {0, 0, 129, 255}}},
    {{{  10,  10, -10}, 0, {0, 0}, {91, 0, 165, 255}}},
    {{{  15,  10,   0}, 0, {0, 0}, {127, 0, 0, 255}}},
    {{{  10,  10,  10}, 0, {0, 0}, {91, 0, 91, 255}}},
    {{{   0,  10,  15}, 0, {0, 0}, {0, 0, 127, 255}}},
    {{{ -10,  10,  10}, 0, {0, 0}, {165, 0, 91, 255}}},
    // Top ring (y = 200)
    {{{ -15, 200,   0}, 0, {0, 0}, {129, 0, 0, 255}}},
    {{{ -10, 200, -10}, 0, {0, 0}, {165, 0, 165, 255}}},
    {{{   0, 200, -15}, 0, {0, 0}, {0, 0, 129, 255}}},
    {{{  10, 200, -10}, 0, {0, 0}, {91, 0, 165, 255}}},
    {{{  15, 200,   0}, 0, {0, 0}, {127, 0, 0, 255}}},
    {{{  10, 200,  10}, 0, {0, 0}, {91, 0, 91, 255}}},
    {{{   0, 200,  15}, 0, {0, 0}, {0, 0, 127, 255}}},
    {{{ -10, 200,  10}, 0, {0, 0}, {165, 0, 91, 255}}},
    // Tip (y = 230)
    {{{   0, 230,   0}, 0, {0, 0}, {0, 127, 0, 255}}},
};

// Display list
static Gfx firerod_dl[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPClearGeometryMode(G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH | G_LIGHTING),

    // Set red color via environment
    gsDPSetEnvColor(255, 50, 0, 255),
    gsDPSetPrimColor(0, 0, 255, 80, 20, 255),

    gsSPVertex(firerod_vtx, 18, 0),

    // Bottom cap triangles
    gsSP2Triangles(0, 1, 2, 0,  0, 2, 3, 0),
    gsSP2Triangles(0, 3, 4, 0,  0, 4, 5, 0),
    gsSP2Triangles(0, 5, 6, 0,  0, 6, 7, 0),
    gsSP2Triangles(0, 7, 8, 0,  0, 8, 1, 0),

    // Side faces
    gsSP2Triangles(1, 9, 10, 0,   1, 10, 2, 0),
    gsSP2Triangles(2, 10, 11, 0,  2, 11, 3, 0),
    gsSP2Triangles(3, 11, 12, 0,  3, 12, 4, 0),
    gsSP2Triangles(4, 12, 13, 0,  4, 13, 5, 0),
    gsSP2Triangles(5, 13, 14, 0,  5, 14, 6, 0),
    gsSP2Triangles(6, 14, 15, 0,  6, 15, 7, 0),
    gsSP2Triangles(7, 15, 16, 0,  7, 16, 8, 0),
    gsSP2Triangles(8, 16, 9, 0,   8, 9, 1, 0),

    // Tip cone
    gsSP2Triangles(9, 17, 10, 0,   10, 17, 11, 0),
    gsSP2Triangles(11, 17, 12, 0,  12, 17, 13, 0),
    gsSP2Triangles(13, 17, 14, 0,  14, 17, 15, 0),
    gsSP2Triangles(15, 17, 16, 0,  16, 17, 9, 0),

    gsSPEndDisplayList(),
};

// Public display list references
Gfx* gFireRodBodyDL = firerod_dl;
Gfx* gFireRodGlowDL = firerod_dl;

// ============================================================================
// DRAW FUNCTION - Draws Fire Rod and active projectile
// Uses torch-style flame (gEffFire1DL) like En_Honotrap and Obj_Syokudai
// ============================================================================

void CustomItems_DrawFireRod(Player* this, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    // Draw rod when equipped
    if (gCustomItemState.fireRodActive) {
        Matrix_Translate(this->leftHandPos.x, this->leftHandPos.y, this->leftHandPos.z, MTXMODE_NEW);
        Matrix_RotateY(BINANG_TO_RAD(this->actor.shape.rot.y), MTXMODE_APPLY);

        f32 baseScale = 0.15f;
        Matrix_Scale(baseScale, baseScale, baseScale, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, firerod_dl);
    }

    // Draw all active fireballs (1-3 depending on attack type)
    if (gCustomItemState.fireRodProjActive) {
        f32 baseScale = gCustomItemState.fireRodProjScale;
        u8 projCount = gCustomItemState.fireRodProjCount;

        sFireRodFlameScroll -= 20;
        sFireRodFlameScroll &= 0x1FF;

        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, sFireRodFlameScroll, 0x20, 0x80));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);

        s16 camYaw = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000;
        f32 flameScale = baseScale * 0.0015f;
        if (flameScale < 0.001f) flameScale = 0.001f;

        // Draw fireball 1 (always)
        Matrix_Translate(gCustomItemState.fireRodProjPos.x, gCustomItemState.fireRodProjPos.y,
                         gCustomItemState.fireRodProjPos.z, MTXMODE_NEW);
        Matrix_RotateY(camYaw * (M_PI / 0x8000), MTXMODE_APPLY);
        Matrix_Scale(flameScale, flameScale, flameScale, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

        // Draw fireball 2 (slash spread)
        if (projCount >= 2) {
            Matrix_Translate(gCustomItemState.fireRodProjPos2.x, gCustomItemState.fireRodProjPos2.y,
                             gCustomItemState.fireRodProjPos2.z, MTXMODE_NEW);
            Matrix_RotateY(camYaw * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_Scale(flameScale, flameScale, flameScale, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        }

        // Draw fireball 3 (slash spread)
        if (projCount >= 3) {
            Matrix_Translate(gCustomItemState.fireRodProjPos3.x, gCustomItemState.fireRodProjPos3.y,
                             gCustomItemState.fireRodProjPos3.z, MTXMODE_NEW);
            Matrix_RotateY(camYaw * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_Scale(flameScale, flameScale, flameScale, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        }

        // Draw trail for center fireball
        for (s32 i = 1; i < 4; i++) {
            Vec3f* trailPos = &gCustomItemState.fireRodProjTrail[i];
            f32 trailScale = flameScale * (1.0f - (i * 0.25f));
            if (trailScale < 0.0005f) continue;

            Matrix_Translate(trailPos->x, trailPos->y, trailPos->z, MTXMODE_NEW);
            Matrix_RotateY(camYaw * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_Scale(trailScale, trailScale, trailScale, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
