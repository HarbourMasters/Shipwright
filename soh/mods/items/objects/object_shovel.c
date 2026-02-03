/**
 * object_shovel.c - Shovel 3D model and draw functions
 *
 * Draws the shovel in Link's hands during digging animation.
 * Model: Placeholder brown cylinder (TODO: custom DL)
 */
#include "z64.h"
#include "../custom_items.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"

// ============================================================================
// SHOVEL DISPLAY LIST (Placeholder - simple brown cylinder for now)
// ============================================================================

// Vertices for a simple shovel shape (handle + blade)
static Vtx shovel_vertices[] = {
    // Handle (brown cylinder)
    VTX(-2, 0, 0, 0, 0, 139, 90, 43, 255),
    VTX( 2, 0, 0, 0, 0, 139, 90, 43, 255),
    VTX( 2, 40, 0, 0, 0, 139, 90, 43, 255),
    VTX(-2, 40, 0, 0, 0, 139, 90, 43, 255),

    // Blade (darker brown/metal)
    VTX(-6, -8, 0, 0, 0, 101, 67, 33, 255),
    VTX( 6, -8, 0, 0, 0, 101, 67, 33, 255),
    VTX( 6, 0, 0, 0, 0, 101, 67, 33, 255),
    VTX(-6, 0, 0, 0, 0, 101, 67, 33, 255),
};

static Gfx shovel_display_list[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
    gsSPClearGeometryMode(G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),

    // Draw handle
    gsDPSetPrimColor(0, 0, 139, 90, 43, 255),
    gsSPVertex(shovel_vertices, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),

    // Draw blade
    gsDPSetPrimColor(0, 0, 101, 67, 33, 255),
    gsSPVertex(&shovel_vertices[4], 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),

    gsSPEndDisplayList(),
};

// ============================================================================
// DRAW FUNCTION
// ============================================================================

void CustomItems_DrawShovel(Player* this, PlayState* play) {
    if (!gCustomItemState.shovelAnimating && !gCustomItemState.shovelActive) {
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);

    // Calculate shovel position relative to player
    Vec3f shovelPos;
    s16 yaw = this->actor.shape.rot.y;

    // Base position at player's hand
    shovelPos.x = this->actor.world.pos.x;
    shovelPos.y = this->actor.world.pos.y + 30.0f; // At chest height
    shovelPos.z = this->actor.world.pos.z;

    // Offset forward based on animation frame
    f32 forwardOffset = 10.0f;
    if (gCustomItemState.shovelAnimating) {
        // Animation: frames 0-10 move forward, 10-20 stay planted, 20-30 pull back
        if (gCustomItemState.shovelAnimTimer < 10) {
            forwardOffset += gCustomItemState.shovelAnimTimer * 2.0f;
        } else if (gCustomItemState.shovelAnimTimer < 20) {
            forwardOffset += 20.0f;
        } else {
            forwardOffset += 20.0f - (gCustomItemState.shovelAnimTimer - 20) * 2.0f;
        }
    }

    shovelPos.x += Math_SinS(yaw) * forwardOffset;
    shovelPos.z += Math_CosS(yaw) * forwardOffset;

    // Setup render state
    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    // Transformation matrix
    Matrix_Translate(shovelPos.x, shovelPos.y, shovelPos.z, MTXMODE_NEW);
    Matrix_RotateY(BINANG_TO_RAD(yaw), MTXMODE_APPLY);

    // Rotation animation - shovel goes down during dig
    f32 digAngle = 0.0f;
    if (gCustomItemState.shovelAnimating) {
        if (gCustomItemState.shovelAnimTimer < 10) {
            // Rotating down to dig
            digAngle = (gCustomItemState.shovelAnimTimer / 10.0f) * 60.0f;
        } else if (gCustomItemState.shovelAnimTimer < 20) {
            // Held at dig angle
            digAngle = 60.0f;
        } else {
            // Rotating back up
            digAngle = 60.0f - ((gCustomItemState.shovelAnimTimer - 20) / 10.0f) * 60.0f;
        }
    }
    Matrix_RotateX(DEG_TO_RAD(-digAngle), MTXMODE_APPLY);

    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    // Draw the shovel
    gSPDisplayList(POLY_OPA_DISP++, shovel_display_list);

    CLOSE_DISPS(play->state.gfxCtx);
}
