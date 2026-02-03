/**
 * object_dekuleaf.c - Deku Leaf 3D model and draw functions
 *
 * Draws the leaf when held and during gliding/swinging.
 * Model: Custom procedural leaf geometry with animating segments.
 */

#include "z64.h"
#include "../custom_items.h"
#include "../logic/item_dekuleaf.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"

// Vertex colors
#define COL_SPINE_R     160
#define COL_SPINE_G     220
#define COL_SPINE_B     100
#define COL_MEMBRANE_R  90
#define COL_MEMBRANE_G  200
#define COL_MEMBRANE_B  90
#define COL_EDGE_R      50
#define COL_EDGE_G      160
#define COL_EDGE_B      60
#define COL_STEM_R      2
#define COL_STEM_G      15
#define COL_STEM_B      2

Vtx sDekuLeafVtx[29] = {
    // Spine (central vein)
    {{ {0, -10, 40}, 0, {0, 0}, {COL_SPINE_R, COL_SPINE_G, COL_SPINE_B, 255} }},
    {{ {0, 15, 10}, 0, {0, 0}, {COL_SPINE_R, COL_SPINE_G, COL_SPINE_B, 255} }},
    {{ {0, 30, -20}, 0, {0, 0}, {COL_SPINE_R, COL_SPINE_G, COL_SPINE_B, 255} }},
    {{ {0, 35, -50}, 0, {0, 0}, {COL_SPINE_R, COL_SPINE_G, COL_SPINE_B, 255} }},
    {{ {0, 40, -80}, 0, {0, 0}, {COL_SPINE_R, COL_SPINE_G, COL_SPINE_B, 255} }},

    // Left veins
    {{ {-30, 5, 20}, 0, {0, 0}, {COL_SPINE_R, COL_SPINE_G, COL_SPINE_B, 255} }},
    {{ {-45, 15, -10}, 0, {0, 0}, {COL_SPINE_R, COL_SPINE_G, COL_SPINE_B, 255} }},
    {{ {-50, 25, -40}, 0, {0, 0}, {COL_SPINE_R, COL_SPINE_G, COL_SPINE_B, 255} }},
    {{ {-30, 35, -70}, 0, {0, 0}, {COL_SPINE_R, COL_SPINE_G, COL_SPINE_B, 255} }},

    // Right veins
    {{ {30, 5, 20}, 0, {0, 0}, {COL_SPINE_R, COL_SPINE_G, COL_SPINE_B, 255} }},
    {{ {45, 15, -10}, 0, {0, 0}, {COL_SPINE_R, COL_SPINE_G, COL_SPINE_B, 255} }},
    {{ {50, 25, -40}, 0, {0, 0}, {COL_SPINE_R, COL_SPINE_G, COL_SPINE_B, 255} }},
    {{ {30, 35, -70}, 0, {0, 0}, {COL_SPINE_R, COL_SPINE_G, COL_SPINE_B, 255} }},

    // Left edge (membrane)
    {{ {-40, -10, 50}, 0, {0, 0}, {COL_MEMBRANE_R, COL_MEMBRANE_G, COL_MEMBRANE_B, 255} }},
    {{ {-60, -5, 10}, 0, {0, 0}, {COL_EDGE_R, COL_EDGE_G, COL_EDGE_B, 255} }},
    {{ {-80, 10, -20}, 0, {0, 0}, {COL_MEMBRANE_R, COL_MEMBRANE_G, COL_MEMBRANE_B, 255} }},
    {{ {-65, 20, -55}, 0, {0, 0}, {COL_EDGE_R, COL_EDGE_G, COL_EDGE_B, 255} }},

    // Right edge (membrane)
    {{ {40, -10, 50}, 0, {0, 0}, {COL_MEMBRANE_R, COL_MEMBRANE_G, COL_MEMBRANE_B, 255} }},
    {{ {60, -5, 10}, 0, {0, 0}, {COL_EDGE_R, COL_EDGE_G, COL_EDGE_B, 255} }},
    {{ {80, 10, -20}, 0, {0, 0}, {COL_MEMBRANE_R, COL_MEMBRANE_G, COL_MEMBRANE_B, 255} }},
    {{ {65, 20, -55}, 0, {0, 0}, {COL_EDGE_R, COL_EDGE_G, COL_EDGE_B, 255} }},

    // Stem (handle)
    {{ {2, -15, 45}, 0, {0, 0}, {COL_STEM_R, COL_STEM_G, COL_STEM_B, 255} }},
    {{ {-2, -15, 45}, 0, {0, 0}, {COL_STEM_R, COL_STEM_G, COL_STEM_B, 255} }},
    {{ {-2, -15, 41}, 0, {0, 0}, {COL_STEM_R, COL_STEM_G, COL_STEM_B, 255} }},
    {{ {2, -15, 41}, 0, {0, 0}, {COL_STEM_R, COL_STEM_G, COL_STEM_B, 255} }},
    {{ {1, -50, 55}, 0, {0, 0}, {COL_STEM_R, COL_STEM_G, COL_STEM_B, 255} }},
    {{ {-1, -50, 55}, 0, {0, 0}, {COL_STEM_R, COL_STEM_G, COL_STEM_B, 255} }},
    {{ {-1, -50, 51}, 0, {0, 0}, {COL_STEM_R, COL_STEM_G, COL_STEM_B, 255} }},
    {{ {1, -50, 51}, 0, {0, 0}, {COL_STEM_R, COL_STEM_G, COL_STEM_B, 255} }},
};

Gfx sDekuLeafDL[] = {
    gsSPVertex(sDekuLeafVtx, 29, 0),

    // Right side surface
    gsSP2Triangles(0, 9, 1, 0, 1, 9, 10, 0),
    gsSP2Triangles(1, 10, 2, 0, 2, 10, 11, 0),
    gsSP2Triangles(2, 11, 3, 0, 3, 11, 12, 0),
    gsSP1Triangle(3, 12, 4, 0),

    // Left side surface
    gsSP2Triangles(0, 1, 5, 0, 1, 6, 5, 0),
    gsSP2Triangles(1, 2, 6, 0, 2, 7, 6, 0),
    gsSP2Triangles(2, 3, 7, 0, 3, 8, 7, 0),
    gsSP1Triangle(3, 4, 8, 0),

    // Right edge wings
    gsSP2Triangles(9, 17, 10, 0, 10, 17, 18, 0),
    gsSP2Triangles(10, 18, 11, 0, 11, 18, 19, 0),
    gsSP2Triangles(11, 19, 12, 0, 12, 19, 20, 0),

    // Left edge wings
    gsSP2Triangles(5, 6, 13, 0, 6, 14, 13, 0),
    gsSP2Triangles(6, 7, 14, 0, 7, 15, 14, 0),
    gsSP2Triangles(7, 8, 15, 0, 8, 16, 15, 0),

    // Back fills
    gsSP2Triangles(0, 5, 13, 0, 0, 17, 9, 0),

    // Stem faces
    gsSP2Triangles(21, 25, 22, 0, 22, 25, 26, 0),
    gsSP2Triangles(22, 26, 23, 0, 23, 26, 27, 0),
    gsSP2Triangles(23, 27, 24, 0, 24, 27, 28, 0),
    gsSP2Triangles(24, 28, 21, 0, 21, 28, 25, 0),
    gsSP2Triangles(25, 28, 26, 0, 26, 28, 27, 0),

    // Stem to leaf connection
    gsSP2Triangles(0, 21, 22, 0, 0, 22, 23, 0),

    gsSPEndDisplayList(),
};

static void DekuLeaf_SetupGeometryMode(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);
    gSPClearGeometryMode(POLY_OPA_DISP++, G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN);
    gSPSetGeometryMode(POLY_OPA_DISP++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_SHADE, G_CC_SHADE);
    CLOSE_DISPS(gfxCtx);
}

static void DekuLeaf_RestoreGeometryMode(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);
    gSPSetGeometryMode(POLY_OPA_DISP++, G_CULL_BACK | G_LIGHTING);
    CLOSE_DISPS(gfxCtx);
}

static void DekuLeaf_DrawModel(PlayState* play, f32 posX, f32 posY, f32 posZ, s16 rotY, f32 scale) {
    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(posX, posY, posZ, MTXMODE_NEW);
    Matrix_RotateY(rotY, MTXMODE_APPLY);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, sDekuLeafDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void CustomItems_DrawDekuLeaf(Player* p, PlayState* play) {
    if (!dlGliding && !dlBlowing) return;

    DekuLeaf_SetupGeometryMode(play->state.gfxCtx);

    if (dlGliding) {
        // Gliding: draw above Link at a fixed position
        f32 posX = p->actor.world.pos.x;
        f32 posY = p->actor.world.pos.y + 42.0f;
        f32 posZ = p->actor.world.pos.z;
        s16 rotY = p->actor.shape.rot.y;
        f32 scale = 0.2f;

        DekuLeaf_DrawModel(play, posX, posY, posZ, rotY, scale);
    } else if (dlBlowing) {
        // Blowing: draw attached to LEFT hand with frame-based scale
        Vec3f handPos = p->bodyPartsPos[PLAYER_BODYPART_L_HAND];
        s16 rotY = p->actor.shape.rot.y;

        // Determine scale based on current animation frame
        f32 scale;
        if (dlAnimTimer >= DEKULEAF_ATTACK_FRAME_START && dlAnimTimer <= DEKULEAF_ATTACK_FRAME_END) {
            scale = DEKULEAF_ATTACK_SCALE;
        } else {
            scale = DEKULEAF_HOLD_SCALE;
        }

        f32 forwardOffset = 3.0f;
        f32 posX = handPos.x + Math_SinS(rotY) * forwardOffset;
        f32 posY = handPos.y + 5.0f;
        f32 posZ = handPos.z + Math_CosS(rotY) * forwardOffset;

        DekuLeaf_DrawModel(play, posX, posY, posZ, rotY, scale);
    }

    DekuLeaf_RestoreGeometryMode(play->state.gfxCtx);
}
