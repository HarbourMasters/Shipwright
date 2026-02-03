/**
 * object_spinner.c - Spinner 3D model and draw functions
 *
 * Draws the spinner when riding and during tricks.
 * Model: Custom DL in spinner_giveDL/
 */
#include "z64.h"
#include "../custom_items.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "spinner_giveDL/header.h"

// ============================================================================
// DRAW FUNCTION CALLER
// ============================================================================

void CustomItems_DrawSpinner(Player* this, PlayState* play) {
    if (gCustomItemState.spinnerActive) {
        OPEN_DISPS(play->state.gfxCtx);

        // Position the spinner at the player's location
        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);

        // Constant rotation calculation
        s16 spinRot = play->gameplayFrames * 0x800;
        Matrix_RotateY(spinRot, MTXMODE_APPLY);

        // Handle scaling logic (Expand when attacking)
        f32 baseScale = 0.20f;
        Matrix_Scale(baseScale, baseScale, baseScale, MTXMODE_APPLY);

        // Apply the generated matrix to the display list
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, g_spinner_dl);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}