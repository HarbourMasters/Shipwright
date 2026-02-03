/**
 * object_lightrod.c - Light Rod 3D model and draw functions
 *
 * Draws the golden/yellow glowing rod and light projectiles.
 * Uses gPhantomEnergyBallDL (Phantom Ganon energy ball) for projectiles.
 */

// Phantom Ganon energy ball display list (same as Dominion Rod)
#include "objects/object_fhg/object_fhg.h"

// Light Rod colors for projectile draw effects
// Uses same colors as item_rod_light.h defines
static RodColor sLightRodDrawColor = {
    255, 255, 200, 255,   // primR, primG, primB, primA (golden-white light)
    255, 255, 0, 255      // envR, envG, envB, envA (yellow glow)
};

// ============================================================================
// LIGHT ROD DISPLAY LIST - Simple rod shape using same format as fire rod
// ============================================================================

// Vertices using same format as fire rod (golden/yellow coloring via normals)
static Vtx lightrod_vtx[] = {
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
static Gfx lightrod_dl[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPClearGeometryMode(G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH | G_LIGHTING),

    // Set golden/yellow color via environment
    gsDPSetEnvColor(255, 255, 0, 255),
    gsDPSetPrimColor(0, 0, 255, 255, 200, 255),

    gsSPVertex(lightrod_vtx, 18, 0),

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
Gfx* gLightRodBodyDL = lightrod_dl;
Gfx* gLightRodGlowDL = lightrod_dl;

// ============================================================================
// LIGHT ROD ORB VISUAL CONSTANTS (same style as Dominion Rod)
// ============================================================================

#define LIGHTROD_ORB_SCALE       5.5f    // Same as Dominion Rod

// Colors - golden/yellow variant of Dominion Rod's orange
#define LIGHTROD_ORB_PRIM_R      255
#define LIGHTROD_ORB_PRIM_G      255
#define LIGHTROD_ORB_PRIM_B      255
#define LIGHTROD_ORB_PRIM_A      200

#define LIGHTROD_ORB_ENV_R       255
#define LIGHTROD_ORB_ENV_G       255     // More yellow than Dominion Rod's 215
#define LIGHTROD_ORB_ENV_B       50
#define LIGHTROD_ORB_ENV_A       0

// ============================================================================
// DRAW FUNCTION - Draws Light Rod and active projectiles
// Uses exact same rendering approach as Dominion Rod
// ============================================================================

void CustomItems_DrawLightRod(Player* this, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    // Draw rod when equipped
    if (gCustomItemState.lightRodActive) {
        Matrix_Translate(this->leftHandPos.x, this->leftHandPos.y, this->leftHandPos.z, MTXMODE_NEW);
        Matrix_RotateY(BINANG_TO_RAD(this->actor.shape.rot.y), MTXMODE_APPLY);

        f32 baseScale = 0.15f;
        Matrix_Scale(baseScale, baseScale, baseScale, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, lightrod_dl);
    }

    // Draw all active light balls (1-3 depending on attack type)
    // Uses EXACT same approach as Dominion Rod's CustomItems_DrawDominionRod
    if (gCustomItemState.lightRodProjActive) {
        u8 projCount = gCustomItemState.lightRodProjCount;

        // Rotation for energy ball effect (same formula as Dominion Rod)
        s16 rotZ = (play->gameplayFrames * 0x1000) + (s16)(Rand_ZeroOne() * 0x4000);

        // Draw light ball 1 (always) - exact same order as Dominion Rod
        Matrix_Translate(gCustomItemState.lightRodProjPos.x, gCustomItemState.lightRodProjPos.y,
                         gCustomItemState.lightRodProjPos.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(LIGHTROD_ORB_SCALE, LIGHTROD_ORB_SCALE, LIGHTROD_ORB_SCALE, MTXMODE_APPLY);
        Matrix_RotateZ((rotZ / (f32)0x8000) * M_PI, MTXMODE_APPLY);

        Gfx_SetupDL_25Xlu(play->state.gfxCtx);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0,
            LIGHTROD_ORB_PRIM_R, LIGHTROD_ORB_PRIM_G, LIGHTROD_ORB_PRIM_B, LIGHTROD_ORB_PRIM_A);
        gDPSetEnvColor(POLY_XLU_DISP++,
            LIGHTROD_ORB_ENV_R, LIGHTROD_ORB_ENV_G, LIGHTROD_ORB_ENV_B, LIGHTROD_ORB_ENV_A);
        gDPPipeSync(POLY_XLU_DISP++);

        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gPhantomEnergyBallDL);

        // Draw light ball 2 (slash spread)
        if (projCount >= 2) {
            Matrix_Translate(gCustomItemState.lightRodProjPos2.x, gCustomItemState.lightRodProjPos2.y,
                             gCustomItemState.lightRodProjPos2.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(LIGHTROD_ORB_SCALE, LIGHTROD_ORB_SCALE, LIGHTROD_ORB_SCALE, MTXMODE_APPLY);
            Matrix_RotateZ(((rotZ + 0x5555) / (f32)0x8000) * M_PI, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gPhantomEnergyBallDL);
        }

        // Draw light ball 3 (slash spread)
        if (projCount >= 3) {
            Matrix_Translate(gCustomItemState.lightRodProjPos3.x, gCustomItemState.lightRodProjPos3.y,
                             gCustomItemState.lightRodProjPos3.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(LIGHTROD_ORB_SCALE, LIGHTROD_ORB_SCALE, LIGHTROD_ORB_SCALE, MTXMODE_APPLY);
            Matrix_RotateZ(((rotZ + 0xAAAA) / (f32)0x8000) * M_PI, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gPhantomEnergyBallDL);
        }

        // Draw trail for center light ball (fading energy balls)
        for (s32 i = 1; i < 4; i++) {
            Vec3f* trailPos = &gCustomItemState.lightRodProjTrail[i];
            f32 trailScale = LIGHTROD_ORB_SCALE * (1.0f - (i * 0.25f));
            if (trailScale < 1.0f) continue;

            // Fade alpha for trail
            u8 trailAlpha = (u8)(LIGHTROD_ORB_PRIM_A * (1.0f - (i * 0.3f)));
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0,
                LIGHTROD_ORB_PRIM_R, LIGHTROD_ORB_PRIM_G, LIGHTROD_ORB_PRIM_B, trailAlpha);

            Matrix_Translate(trailPos->x, trailPos->y, trailPos->z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(trailScale, trailScale, trailScale, MTXMODE_APPLY);
            Matrix_RotateZ(((rotZ - (i * 0x2000)) / (f32)0x8000) * M_PI, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gPhantomEnergyBallDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
