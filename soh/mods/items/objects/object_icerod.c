/**
 * object_icerod.c - Ice Rod 3D model and draw functions
 *
 * Draws the cyan/blue glowing rod and ice projectiles.
 * Uses gEffFire1DL with ice colors for projectiles.
 */

// Fire effect display list from gameplay_keep (same as Fire Rod)
#include "objects/gameplay_keep/gameplay_keep.h"

// Ice Rod colors for projectile draw effects
// Uses same colors as item_rod_ice.h defines
static RodColor sIceRodDrawColor = {
    200, 255, 255, 255,   // primR, primG, primB, primA (cyan-white ice)
    0, 100, 255, 255      // envR, envG, envB, envA (blue glow)
};

// Texture scroll counter for ice ball animation
static s16 sIceRodBallScroll = 0;

// ============================================================================
// ICE ROD DISPLAY LIST - Simple rod shape using same format as fire rod
// ============================================================================

// Vertices using same format as fire rod (icy blue coloring via normals)
static Vtx icerod_vtx[] = {
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
static Gfx icerod_dl[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPClearGeometryMode(G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH | G_LIGHTING),

    // Set cyan/blue color via environment
    gsDPSetEnvColor(0, 100, 255, 255),
    gsDPSetPrimColor(0, 0, 200, 255, 255, 255),

    gsSPVertex(icerod_vtx, 18, 0),

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
Gfx* gIceRodBodyDL = icerod_dl;
Gfx* gIceRodGlowDL = icerod_dl;

// ============================================================================
// DRAW FUNCTION - Draws Ice Rod and active projectiles
// Uses ice crystal/spark effects for visual
// ============================================================================

void CustomItems_DrawIceRod(Player* this, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    // Draw rod when equipped
    if (gCustomItemState.iceRodActive) {
        Matrix_Translate(this->leftHandPos.x, this->leftHandPos.y, this->leftHandPos.z, MTXMODE_NEW);
        Matrix_RotateY(BINANG_TO_RAD(this->actor.shape.rot.y), MTXMODE_APPLY);

        f32 baseScale = 0.15f;
        Matrix_Scale(baseScale, baseScale, baseScale, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, icerod_dl);
    }

    // Draw all active ice balls (1-3 depending on attack type)
    // Ice projectiles are drawn as glowing translucent spheres
    if (gCustomItemState.iceRodProjActive) {
        f32 baseScale = gCustomItemState.iceRodProjScale;
        u8 projCount = gCustomItemState.iceRodProjCount;

        // Update texture scroll for animation
        sIceRodBallScroll -= 10;
        sIceRodBallScroll &= 0x1FF;

        Gfx_SetupDL_25Xlu(play->state.gfxCtx);

        // CRITICAL: Set up texture segment before using gEffFire1DL (same as Fire Rod)
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, sIceRodBallScroll, 0x20, 0x80));

        // Ice colors: cyan to white
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 200, 255, 255, 200);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 100, 255, 128);

        s16 camYaw = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000;
        f32 iceScale = baseScale * 0.002f;
        if (iceScale < 0.001f) iceScale = 0.001f;

        // Draw ice ball 1 (always)
        Matrix_Translate(gCustomItemState.iceRodProjPos.x, gCustomItemState.iceRodProjPos.y,
                         gCustomItemState.iceRodProjPos.z, MTXMODE_NEW);
        Matrix_RotateY(camYaw * (M_PI / 0x8000), MTXMODE_APPLY);
        Matrix_Scale(iceScale, iceScale, iceScale, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        // Use ice arrow effect display list if available, otherwise simple sphere
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);  // Placeholder - will look like blue fire

        // Draw ice ball 2 (slash spread)
        if (projCount >= 2) {
            Matrix_Translate(gCustomItemState.iceRodProjPos2.x, gCustomItemState.iceRodProjPos2.y,
                             gCustomItemState.iceRodProjPos2.z, MTXMODE_NEW);
            Matrix_RotateY(camYaw * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_Scale(iceScale, iceScale, iceScale, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        }

        // Draw ice ball 3 (slash spread)
        if (projCount >= 3) {
            Matrix_Translate(gCustomItemState.iceRodProjPos3.x, gCustomItemState.iceRodProjPos3.y,
                             gCustomItemState.iceRodProjPos3.z, MTXMODE_NEW);
            Matrix_RotateY(camYaw * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_Scale(iceScale, iceScale, iceScale, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        }

        // Draw trail for center ice ball (fading ice particles)
        for (s32 i = 1; i < 4; i++) {
            Vec3f* trailPos = &gCustomItemState.iceRodProjTrail[i];
            f32 trailScale = iceScale * (1.0f - (i * 0.25f));
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
